/*
 * ARQUIVO: src/banco.c
 * RESPONSABILIDADE: Implementação do TAD Banco
 * 
 * CAMADA: Serviços (Lógica de Negócio)
 * 
 * Implementa operações de alto nível sobre o banco com regras
 * de negócio e validações. Orquestra persistência e ordenação.
 * 
 * TÓPICOS APLICADOS:
 *  - Alocação dinâmica com malloc e realloc
 *  - Passagem por referência com ponteiros
 *  - Strings e comparação (strcmp)
 *  - Vetor dinâmico com expansão sob demanda
 */

#include "banco.h"
#include "persistencia.h"
#include "ordenacao.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>


/*
 * FUNÇÃO: banco_init()
 * Inicializa Banco com estado vazio
 */
void banco_init(Banco* b, const char* arq_cli, const char* arq_mov) {
    b->dados = NULL;          // Sem clientes inicialmente
    b->tam = 0;               // Tamanho zero
    b->cap = 0;               // Capacidade zero (aloca sob demanda)
    b->arq_clientes = arq_cli;
    b->arq_movimentos = arq_mov;
}

/*
 * FUNÇÃO: banco_carregar()
 * Carrega dados persistidos do arquivo
 */
int banco_carregar(Banco* b) {
    return carregar_clientes(b->arq_clientes, &b->dados, &b->tam, &b->cap);
}

/*
 * FUNÇÃO: banco_salvar()
 * Persiste estado atual em arquivo
 */
int banco_salvar(Banco* b) {
    return salvar_clientes(b->arq_clientes, b->dados, b->tam);
}

/*
 * FUNÇÃO: banco_free()
 * Libera recursos e reseta estado
 */
void banco_free(Banco* b) {
    if (b->dados) free(b->dados);  // Libera vetor dinâmico
    b->dados = NULL;
    b->tam = 0;
    b->cap = 0;
}

/*
 * FUNÇÃO: banco_abrir_conta()
 * TÓPICOS:
 *  - Validação de duplicação (busca linear)
 *  - Realocação dinâmica com realloc (crescimento 2x)
 *  - Tratamento de erro (verificação NULL)
 */
int banco_abrir_conta(Banco* b, Cliente c) {
    // Validar se conta e CPF já existem (evitar duplicação)
    for (size_t i = 0; i < b->tam; i++) {
        if (b->dados[i].ativo && strcmp(b->dados[i].conta, c.conta) == 0) {
            printf("Erro: Conta ja existe\n");
            return 0;
        }
        if (strcmp(b->dados[i].cpf, c.cpf) == 0 && b->dados[i].ativo) {
            printf("Erro: CPF ja cadastrado\n");
            return 0;
        }
    }

    // Inicializar valores obrigatórios
    c.saldo = 0.0;
    c.ativo = 1;

    // Se tam == cap, dobra a capacidade (estratégia amortizada)
    if (b->tam == b->cap) {
        size_t nova = (b->cap == 0) ? 4 : (b->cap * 2);  // Inicia com 4, depois dobra
        Cliente* tmp = realloc(b->dados, nova * sizeof *tmp);
        if (!tmp) {
            fprintf(stderr, "Erro: falha de memoria\n");
            return 0;  // Falha de alocação = retorno 0
        }
        b->dados = tmp;
        b->cap = nova;
    }

    // Adicionar cliente ao vetor
    b->dados[b->tam++] = c;
    return banco_salvar(b);
}

/*
 * FUNÇÃO: banco_encerrar_conta()
 * Valida pré-condição: saldo == 0
 */
int banco_encerrar_conta(Banco* b, const char* conta) {
    for (size_t i = 0; i < b->tam; i++) {
        if (b->dados[i].ativo && strcmp(b->dados[i].conta, conta) == 0) {
            // Validar saldo zero como pré-condição
            if (b->dados[i].saldo != 0.0) {
                printf("Erro: Saldo deve ser zero para encerrar\n");
                return 0;
            }
            // Marcar como inativa (não remove, apenas marca)
            b->dados[i].ativo = 0;
            return banco_salvar(b);
        }
    }
    printf("Erro: Conta inexistente\n");
    return 0;
}

/*
 * FUNÇÃO: banco_consultar_por_conta()
 * Retorna ponteiro (referência) ao cliente
 * TÓPICO: Ponteiros para retornar acesso a dados externos
 */
Cliente* banco_consultar_por_conta(Banco* b, const char* conta) {
    for (size_t i = 0; i < b->tam; i++) {
        if (b->dados[i].ativo && strcmp(b->dados[i].conta, conta) == 0) {
            return &b->dados[i];  // Retorna endereço do cliente
        }
    }
    return NULL;
}

/*
 * FUNÇÃO: banco_consultar_por_cpf()
 * Similar à consulta por conta
 */
Cliente* banco_consultar_por_cpf(Banco* b, const char* cpf) {
    for (size_t i = 0; i < b->tam; i++) {
        if (b->dados[i].ativo && strcmp(b->dados[i].cpf, cpf) == 0) {
            return &b->dados[i];
        }
    }
    return NULL;
}

/*
 * FUNÇÃO: banco_alterar_dados()
 * Altera campos permitidos (preserva conta e saldo)
 */
int banco_alterar_dados(Banco* b, const char* conta, Cliente c_novo) {
    Cliente* c = banco_consultar_por_conta(b, conta);
    if (!c) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }

    // Atualizar campos permitidos (usando strcpy para strings)
    strcpy(c->nome, c_novo.nome);
    strcpy(c->cpf, c_novo.cpf);
    strcpy(c->data_nasc, c_novo.data_nasc);
    strcpy(c->telefone, c_novo.telefone);
    strcpy(c->endereco, c_novo.endereco);
    strcpy(c->cep, c_novo.cep);
    strcpy(c->local, c_novo.local);
    strcpy(c->numero_casa, c_novo.numero_casa);
    strcpy(c->bairro, c_novo.bairro);
    strcpy(c->cidade, c_novo.cidade);
    strcpy(c->estado, c_novo.estado);

    return banco_salvar(b);
}

/*
 * FUNÇÃO: banco_depositar()
 * Modifica saldo por referência via ponteiro
 * TÓPICO: Passagem por referência para modificação de dados
 */
int banco_depositar(Banco* b, const char* conta, double valor) {
    // Validar valor
    if (valor <= 0) {
        printf("Erro: Valor invalido\n");
        return 0;
    }

    // Localizar conta
    Cliente* c = banco_consultar_por_conta(b, conta);
    if (!c) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }

    // Modificar saldo por referência
    c->saldo += valor;
    
    // Registrar movimento no extrato
    registrar_movimento(b->arq_movimentos, conta, "DEPOSITO", valor, c->saldo);
    
    return banco_salvar(b);
}

/*
 * FUNÇÃO: banco_sacar()
 * Valida saldo suficiente antes de modificar
 */
int banco_sacar(Banco* b, const char* conta, double valor) {
    // Validar valor
    if (valor <= 0) {
        printf("Erro: Valor invalido\n");
        return 0;
    }

    // Localizar conta
    Cliente* c = banco_consultar_por_conta(b, conta);
    if (!c) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }

    // Validar saldo suficiente
    if (valor > c->saldo) {
        printf("Erro: Saldo insuficiente\n");
        return 0;
    }

    // Executar saque
    c->saldo -= valor;
    registrar_movimento(b->arq_movimentos, conta, "SAQUE", valor, c->saldo);
    
    return banco_salvar(b);
}
    int banco_pix(Banco* b, const char* conta_origem, const char* conta_destino, double valor) {

        if (valor <= 0) return 0;

        Cliente* origem = banco_consultar_por_conta(b, conta_origem);
        Cliente* destino = banco_consultar_por_conta(b, conta_destino);

        if (!origem || !destino) return 0;
        if (origem->saldo < valor) return 0;

        origem->saldo -= valor;
        destino->saldo += valor;

        registrar_movimento(b->arq_movimentos, conta_origem, "PIX", -valor, origem->saldo);
        registrar_movimento(b->arq_movimentos, conta_destino, "PIX-RECEBIDO", valor, destino->saldo);

        banco_salvar(b);

        return 1;
    }

    int banco_cartao(Banco* b, const char* conta, double valor) {

    if (valor <= 0) return 0;

    Cliente* c = banco_consultar_por_conta(b, conta);
    if (!c) return 0;

    if (c->saldo < valor) return 0;

    c->saldo -= valor;

    registrar_movimento(b->arq_movimentos, conta, "CARTAO", -valor, c->saldo);

    banco_salvar(b);

    return 1;
}

/*
 * FUNÇÃO: banco_listar_por_nome()
 * TÓPICOS:
 *  - Alocação dinâmica (cópia temporária)
 *  - Cópia com memcpy
 *  - Ordenação recursiva (Quick Sort)
 */
void banco_listar_por_nome(Banco* b) {
    if (b->tam == 0) {
        printf("Nenhum cliente cadastrado\n");
        return;
    }

    // Alocar vetor temporário para não modificar ordem em memória
    Cliente* temp = malloc(b->tam * sizeof *temp);
    if (!temp) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        return;
    }

    // Copiar dados originais para vetor temporário
    memcpy(temp, b->dados, b->tam * sizeof *temp);
    
    // Ordenar por nome (tipo = 0)
    quicksort(temp, 0, (int)b->tam - 1, 0);

    // Exibir relatório
    printf("\n=== CLIENTES POR NOME ===\n");
    for (size_t i = 0; i < b->tam; i++) {
        if (temp[i].ativo) {
            printf("Conta: %s | Nome: %s | CPF: %s | Saldo: R$ %.2f\n",
                   temp[i].conta, temp[i].nome, temp[i].cpf, temp[i].saldo);
        }
    }
    printf("\n");

    free(temp);
}

/*
 * FUNÇÃO: banco_listar_por_conta()
 * Similar a listar por nome, mas ordena por número de conta (tipo = 1)
 */
void banco_listar_por_conta(Banco* b) {
    if (b->tam == 0) {
        printf("Nenhum cliente cadastrado\n");
        return;
    }

    Cliente* temp = malloc(b->tam * sizeof *temp);
    if (!temp) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        return;
    }

    memcpy(temp, b->dados, b->tam * sizeof *temp);
    quicksort(temp, 0, (int)b->tam - 1, 1);

    printf("\n=== CLIENTES POR CONTA ===\n");
    for (size_t i = 0; i < b->tam; i++) {
        if (temp[i].ativo) {
            printf("Conta: %s | Nome: %s | CPF: %s | Saldo: R$ %.2f\n",
                   temp[i].conta, temp[i].nome, temp[i].cpf, temp[i].saldo);
        }
    }
    printf("\n");

    free(temp);
}
