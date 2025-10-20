#include "banco.h"
#include "ordenacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void banco_init(Banco* b, const char* arq_cli, const char* arq_mov) {
    b->clientes.dados = NULL;
    b->clientes.tam = 0;
    b->clientes.cap = 0;
    strncpy(b->arquivo_clientes, arq_cli, sizeof(b->arquivo_clientes) - 1);
    strncpy(b->arquivo_movimentos, arq_mov, sizeof(b->arquivo_movimentos) - 1);
}

void banco_free(Banco* b) {
    if (b->clientes.dados) {
        free(b->clientes.dados);
        b->clientes.dados = NULL;
    }
    b->clientes.tam = 0;
    b->clientes.cap = 0;
}

static int adicionar_cliente(Banco* b, Cliente c) {
    if (b->clientes.tam == b->clientes.cap) {
        size_t nova_cap = b->clientes.cap == 0 ? 4 : b->clientes.cap * 2;
        Cliente* tmp = realloc(b->clientes.dados, nova_cap * sizeof(Cliente));
        if (!tmp) {
            fprintf(stderr, "Erro: Falha na alocacao de memoria\n");
            return 0;
        }
        b->clientes.dados = tmp;
        b->clientes.cap = nova_cap;
    }
    b->clientes.dados[b->clientes.tam++] = c;
    return 1;
}

int banco_carregar(Banco* b) {
    FILE* f = fopen(b->arquivo_clientes, "r");
    if (!f) {
        printf("Aviso: Arquivo de clientes nao encontrado. Iniciando banco vazio.\n");
        return 1;
    }
    
    char linha[1024];
    if (fgets(linha, sizeof(linha), f)) {
        // Pular cabecalho
    }
    
    while (fgets(linha, sizeof(linha), f)) {
        Cliente c;
        cliente_init(&c);
        linha[strcspn(linha, "\n")] = 0;
        
        char* token = strtok(linha, ";");
        int campo = 0;
        
        while (token != NULL && campo < 16) {
            switch(campo) {
                case 0: strncpy(c.agencia, token, sizeof(c.agencia) - 1); break;
                case 1: strncpy(c.conta, token, sizeof(c.conta) - 1); break;
                case 2: strncpy(c.nome, token, sizeof(c.nome) - 1); break;
                case 3: strncpy(c.cpf, token, sizeof(c.cpf) - 1); break;
                case 4: strncpy(c.data_nasc, token, sizeof(c.data_nasc) - 1); break;
                case 5: strncpy(c.telefone, token, sizeof(c.telefone) - 1); break;
                case 6: strncpy(c.endereco, token, sizeof(c.endereco) - 1); break;
                case 7: strncpy(c.cep, token, sizeof(c.cep) - 1); break;
                case 8: strncpy(c.local, token, sizeof(c.local) - 1); break;
                case 9: strncpy(c.numero_casa, token, sizeof(c.numero_casa) - 1); break;
                case 10: strncpy(c.bairro, token, sizeof(c.bairro) - 1); break;
                case 11: strncpy(c.cidade, token, sizeof(c.cidade) - 1); break;
                case 12: strncpy(c.estado, token, sizeof(c.estado) - 1); break;
                case 13: strncpy(c.senha, token, sizeof(c.senha) - 1); break;
                case 14: c.saldo = atof(token); break;
                case 15: c.ativo = atoi(token); break;
            }
            token = strtok(NULL, ";");
            campo++;
        }
        
        if (!adicionar_cliente(b, c)) {
            fclose(f);
            return 0;
        }
    }
    
    fclose(f);
    printf("Sucesso: %zu cliente(s) carregado(s)\n", b->clientes.tam);
    return 1;
}

int banco_salvar(Banco* b) {
    FILE* f = fopen(b->arquivo_clientes, "w");
    if (!f) {
        fprintf(stderr, "Erro: Nao foi possivel salvar os dados\n");
        return 0;
    }
    
    fprintf(f, "AGENCIA;CONTA;NOME;CPF;DATA_NASC;TELEFONE;ENDERECO;CEP;LOCAL;NUMERO_CASA;BAIRRO;CIDADE;ESTADO;SENHA;SALDO;ATIVO\n");
    
    for (size_t i = 0; i < b->clientes.tam; i++) {
        Cliente* c = &b->clientes.dados[i];
        fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%.2f;%d\n",
                c->agencia, c->conta, c->nome, c->cpf, c->data_nasc, c->telefone,
                c->endereco, c->cep, c->local, c->numero_casa, c->bairro,
                c->cidade, c->estado, c->senha, c->saldo, c->ativo);
    }
    
    fclose(f);
    return 1;
}

int banco_buscar_por_conta(Banco* b, const char* conta) {
    for (size_t i = 0; i < b->clientes.tam; i++) {
        if (strcmp(b->clientes.dados[i].conta, conta) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int banco_buscar_por_cpf(Banco* b, const char* cpf) {
    for (size_t i = 0; i < b->clientes.tam; i++) {
        if (strcmp(b->clientes.dados[i].cpf, cpf) == 0) {
            return (int)i;
        }
    }
    return -1;
}

Cliente* banco_obter_cliente(Banco* b, int idx) {
    if (idx < 0 || idx >= (int)b->clientes.tam) {
        return NULL;
    }
    return &b->clientes.dados[idx];
}

int banco_abrir_conta(Banco* b, Cliente c) {
    if (banco_buscar_por_conta(b, c.conta) >= 0) {
        printf("Erro: Numero de conta ja existe\n");
        return 0;
    }
    
    if (banco_buscar_por_cpf(b, c.cpf) >= 0) {
        printf("Erro: CPF ja cadastrado\n");
        return 0;
    }
    
    if (!cliente_validar_cpf(c.cpf)) {
        printf("Erro: CPF invalido\n");
        return 0;
    }
    
    if (strlen(c.senha) < 4) {
        printf("Erro: Senha deve ter no minimo 4 caracteres\n");
        return 0;
    }
    
    c.saldo = 0.0;
    c.ativo = 1;
    
    if (!adicionar_cliente(b, c)) {
        return 0;
    }
    
    return banco_salvar(b);
}

int banco_encerrar_conta(Banco* b, const char* conta, const char* senha) {
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }
    
    Cliente* c = &b->clientes.dados[idx];
    
    if (strcmp(c->senha, senha) != 0) {
        printf("Erro: Senha incorreta\n");
        return 0;
    }
    
    if (c->saldo != 0.0) {
        printf("Erro: Conta deve ter saldo zero para encerramento (saldo atual: R$ %.2f)\n", c->saldo);
        return 0;
    }
    
    c->ativo = 0;
    return banco_salvar(b);
}

int banco_alterar_dados(Banco* b, const char* conta, Cliente novo) {
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }
    
    Cliente* c = &b->clientes.dados[idx];
    
    strncpy(c->nome, novo.nome, sizeof(c->nome) - 1);
    strncpy(c->telefone, novo.telefone, sizeof(c->telefone) - 1);
    strncpy(c->endereco, novo.endereco, sizeof(c->endereco) - 1);
    strncpy(c->cep, novo.cep, sizeof(c->cep) - 1);
    strncpy(c->local, novo.local, sizeof(c->local) - 1);
    strncpy(c->numero_casa, novo.numero_casa, sizeof(c->numero_casa) - 1);
    strncpy(c->bairro, novo.bairro, sizeof(c->bairro) - 1);
    strncpy(c->cidade, novo.cidade, sizeof(c->cidade) - 1);
    strncpy(c->estado, novo.estado, sizeof(c->estado) - 1);
    
    return banco_salvar(b);
}

static void registrar_movimento(Banco* b, const char* conta, const char* tipo, double valor, double saldo_novo) {
    FILE* f = fopen(b->arquivo_movimentos, "a");
    if (!f) return;
    
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char data[20];
    strftime(data, sizeof(data), "%Y-%m-%d", tm_info);
    
    fprintf(f, "%s;%s;%s;%.2f;saldo=%.2f\n", data, conta, tipo, valor, saldo_novo);
    fclose(f);
}

int banco_depositar(Banco* b, const char* conta, double valor) {
    if (valor <= 0) {
        printf("Erro: Valor invalido\n");
        return 0;
    }
    
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }
    
    Cliente* c = &b->clientes.dados[idx];
    
    if (!c->ativo) {
        printf("Erro: Conta encerrada\n");
        return 0;
    }
    
    c->saldo += valor;
    registrar_movimento(b, conta, "DEPOSITO", valor, c->saldo);
    
    return banco_salvar(b);
}

int banco_sacar(Banco* b, const char* conta, const char* senha, double valor) {
    if (valor <= 0) {
        printf("Erro: Valor invalido\n");
        return 0;
    }
    
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Erro: Conta inexistente\n");
        return 0;
    }
    
    Cliente* c = &b->clientes.dados[idx];
    
    if (!c->ativo) {
        printf("Erro: Conta encerrada\n");
        return 0;
    }
    
    if (strcmp(c->senha, senha) != 0) {
        printf("Erro: Senha incorreta\n");
        return 0;
    }
    
    if (c->saldo < valor) {
        printf("Erro: Saldo insuficiente (disponivel: R$ %.2f)\n", c->saldo);
        return 0;
    }
    
    c->saldo -= valor;
    registrar_movimento(b, conta, "SAQUE", valor, c->saldo);
    
    return banco_salvar(b);
}

double banco_consultar_saldo(Banco* b, const char* conta) {
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        return -1.0;
    }
    return b->clientes.dados[idx].saldo;
}

void banco_listar_ordenado_por_nome(Banco* b) {
    if (b->clientes.tam == 0) {
        printf("\nNenhum cliente cadastrado.\n");
        return;
    }
    
    Cliente* copia = malloc(b->clientes.tam * sizeof(Cliente));
    if (!copia) {
        fprintf(stderr, "Erro de memoria\n");
        return;
    }
    
    memcpy(copia, b->clientes.dados, b->clientes.tam * sizeof(Cliente));
    quicksort_por_nome(copia, 0, (int)b->clientes.tam - 1);
    
    printf("\n========================================\n");
    printf("    CLIENTES ORDENADOS POR NOME\n");
    printf("========================================\n");
    
    for (size_t i = 0; i < b->clientes.tam; i++) {
        printf("%s | Conta: %s | Saldo: R$ %.2f | %s\n",
               copia[i].nome, copia[i].conta, copia[i].saldo,
               copia[i].ativo ? "ATIVA" : "ENCERRADA");
    }
    
    free(copia);
}

void banco_listar_ordenado_por_conta(Banco* b) {
    if (b->clientes.tam == 0) {
        printf("\nNenhum cliente cadastrado.\n");
        return;
    }
    
    Cliente* copia = malloc(b->clientes.tam * sizeof(Cliente));
    if (!copia) {
        fprintf(stderr, "Erro de memoria\n");
        return;
    }
    
    memcpy(copia, b->clientes.dados, b->clientes.tam * sizeof(Cliente));
    quicksort_por_conta(copia, 0, (int)b->clientes.tam - 1);
    
    printf("\n========================================\n");
    printf("    CLIENTES ORDENADOS POR CONTA\n");
    printf("========================================\n");
    
    for (size_t i = 0; i < b->clientes.tam; i++) {
        printf("Conta: %s | %s | Saldo: R$ %.2f | %s\n",
               copia[i].conta, copia[i].nome, copia[i].saldo,
               copia[i].ativo ? "ATIVA" : "ENCERRADA");
    }
    
    free(copia);
}

void banco_exibir_extrato(Banco* b, const char* conta) {
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Erro: Conta inexistente\n");
        return;
    }
    
    FILE* f = fopen(b->arquivo_movimentos, "r");
    if (!f) {
        printf("Nenhum movimento registrado.\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("       EXTRATO DA CONTA %s\n", conta);
    printf("========================================\n");
    
    char linha[256];
    int encontrou = 0;
    
    while (fgets(linha, sizeof(linha), f)) {
        if (strstr(linha, conta)) {
            printf("%s", linha);
            encontrou = 1;
        }
    }
    
    if (!encontrou) {
        printf("Nenhum movimento encontrado.\n");
    }
    
    printf("========================================\n");
    printf("Saldo atual: R$ %.2f\n", b->clientes.dados[idx].saldo);
    
    fclose(f);
}