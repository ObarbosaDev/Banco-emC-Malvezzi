/*
 * ARQUIVO: src/banco.h
 * RESPONSABILIDADE: Interface pública do TAD (Tipo Abstrato de Dados) Banco
 * 
 * CAMADA: Serviços (Lógica de Negócio)
 * 
 * Define operações de alto nível sobre o banco. Cada função implementa
 * uma regra de negócio específica (abrir conta, depositar, sacar, etc).
 * 
 * TÓPICOS APLICADOS:
 *  - Encapsulamento de dados (struct Banco)
 *  - Abstração de complexidade de gerenciamento de memória
 *  - Modularização de funções por responsabilidade
 */

#ifndef BANCO_H
#define BANCO_H

#include <stddef.h>
#include "cliente.h"

/*
 * STRUCT: Banco (TAD - Tipo Abstrato de Dados)
 * RESPONSABILIDADE: Encapsular e gerenciar estado do banco
 * 
 * CAMPOS:
 *  - dados: Ponteiro para vetor dinâmico de clientes alocado com malloc
 *  - tam: Número atual de clientes cadastrados
 *  - cap: Capacidade total alocada (para realloc quando necessário)
 *  - arq_clientes: Caminho do arquivo onde persistir dados
 *  - arq_movimentos: Caminho do arquivo para registrar transações
 * 
 * Esta estrutura implementa um vetor dinâmico que cresce sob demanda.
 */
typedef struct {
    Cliente* dados;                  // Vetor dinâmico (heap)
    size_t tam;                      // Quantidade de elementos
    size_t cap;                      // Capacidade alocada
    const char* arq_clientes;        // Persistência de clientes
    const char* arq_movimentos;      // Log de transações
} Banco;

/* ============ FUNÇÕES DE GERENCIAMENTO DO BANCO ============ */

/*
 * FUNÇÃO: banco_init()
 * PARÂMETROS: 
 *  - b: ponteiro para Banco (passagem por referência para modificação)
 *  - arq_cli: caminho do arquivo de clientes
 *  - arq_mov: caminho do arquivo de movimentos
 * DESCRIÇÃO: Inicializa a estrutura Banco com valores padrão (NULL, 0).
 *            Essa função DEVE ser chamada antes de qualquer outra.
 */
void banco_init(Banco* b, const char* arq_cli, const char* arq_mov);

/*
 * FUNÇÃO: banco_carregar()
 * PARÂMETROS: b (ponteiro para Banco)
 * RETORNO: 1 = sucesso, 0 = erro de memória
 * DESCRIÇÃO: Lê clientes do arquivo e popula o vetor dinâmico.
 *            Realoca memória conforme necessário.
 *            Se arquivo não existe, inicia banco vazio (sem erro).
 */
int banco_carregar(Banco* b);

/*
 * FUNÇÃO: banco_salvar()
 * PARÂMETROS: b (ponteiro para Banco)
 * RETORNO: 1 = sucesso, 0 = erro de escrita
 * DESCRIÇÃO: Persiste todos os clientes atuais em arquivo texto (separado por ;).
 *            Sobrescreve arquivo anterior (completo).
 */
int banco_salvar(Banco* b);

/*
 * FUNÇÃO: banco_free()
 * PARÂMETROS: b (ponteiro para Banco)
 * DESCRIÇÃO: Libera memória do vetor dinâmico (free).
 *            Reseta campos para estado seguro (NULL, 0).
 *            DEVE ser chamada ao final do programa.
 */
void banco_free(Banco* b);

/* ============ OPERAÇÕES BANCÁRIAS ============ */

/*
 * FUNÇÃO: banco_abrir_conta()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - c: cliente com dados a cadastrar
 * RETORNO: 1 = sucesso, 0 = erro
 * DESCRIÇÃO: Abre nova conta após validações:
 *            - Verifica unicidade de CPF
 *            - Verifica unicidade de número de conta
 *            - Inicializa saldo em 0.0 e status em ativo
 *            - Realoca vetor se necessário
 *            - Salva em arquivo
 */
int banco_abrir_conta(Banco* b, Cliente c);

/*
 * FUNÇÃO: banco_encerrar_conta()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - conta: número da conta a encerrar
 * RETORNO: 1 = sucesso, 0 = erro
 * DESCRIÇÃO: Encerra conta (marca como inativa).
 *            Valida pré-condição: saldo DEVE ser zero.
 *            Salva alteração em arquivo.
 */
int banco_encerrar_conta(Banco* b, const char* conta);

/*
 * FUNÇÃO: banco_consultar_por_conta()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - conta: número da conta a procurar
 * RETORNO: Ponteiro para Cliente (ou NULL se não encontrado)
 * DESCRIÇÃO: Busca linear por conta em clientes ativos.
 *            Retorna referência (não cópia) para permitir leitura.
 * TÓPICO: Ponteiros em funções para acesso a dados externos
 */
Cliente* banco_consultar_por_conta(Banco* b, const char* conta);

/*
 * FUNÇÃO: banco_consultar_por_cpf()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - cpf: CPF do cliente a procurar
 * RETORNO: Ponteiro para Cliente (ou NULL se não encontrado)
 * DESCRIÇÃO: Busca linear por CPF em clientes ativos.
 */
Cliente* banco_consultar_por_cpf(Banco* b, const char* cpf);

/*
 * FUNÇÃO: banco_alterar_dados()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - conta: número da conta cujos dados serão alterados
 *  - c_novo: Cliente com novos dados
 * RETORNO: 1 = sucesso, 0 = conta não encontrada
 * DESCRIÇÃO: Altera dados do cliente (preserva conta, CPF, saldo, senha).
 *            Salva alterações em arquivo.
 */
int banco_alterar_dados(Banco* b, const char* conta, Cliente c_novo);

/*
 * FUNÇÃO: banco_depositar()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - conta: número da conta
 *  - valor: valor a depositar (deve ser > 0)
 * RETORNO: 1 = sucesso, 0 = erro
 * DESCRIÇÃO: Realiza depósito:
 *            - Valida valor > 0
 *            - Localiza conta
 *            - Incrementa saldo
 *            - Registra movimento em arquivo
 *            - Salva em arquivo
 * TÓPICO: Passagem por referência para modificar saldo
 */
int banco_depositar(Banco* b, const char* conta, double valor);

/*
 * FUNÇÃO: banco_sacar()
 * PARÂMETROS:
 *  - b: ponteiro para Banco
 *  - conta: número da conta
 *  - valor: valor a sacar (deve ser > 0)
 * RETORNO: 1 = sucesso, 0 = erro
 * DESCRIÇÃO: Realiza saque com validações:
 *            - Valida valor > 0
 *            - Localiza conta
 *            - Valida saldo suficiente
 *            - Decrementa saldo
 *            - Registra movimento
 *            - Salva em arquivo
 */
int banco_sacar(Banco* b, const char* conta, double valor);

/* ============ RELATÓRIOS E LISTAGENS ============ */

/*
 * FUNÇÃO: banco_listar_por_nome()
 * PARÂMETROS: b (ponteiro para Banco)
 * DESCRIÇÃO: Lista todos os clientes ativos ordenados por nome.
 *            Utiliza Quick Sort recursivo.
 *            Formato: Conta | Nome | CPF | Saldo
 * TÓPICO: Aplicação de ordenação recursiva
 */
void banco_listar_por_nome(Banco* b);

/*
 * FUNÇÃO: banco_listar_por_conta()
 * PARÂMETROS: b (ponteiro para Banco)
 * DESCRIÇÃO: Lista todos os clientes ativos ordenados por número de conta.
 *            Utiliza Quick Sort recursivo.
 *            Formato: Conta | Nome | CPF | Saldo
 */
void banco_listar_por_conta(Banco* b);

/* ============ OPERAÇÕES PIX E CARTÃO ============ */

/*
 * FUNÇÃO: banco_pix()
 * DESCRIÇÃO:
 *  - Transfere valor entre contas usando o mecanismo PIX
 *  - Valida saldo, existência das contas e registra movimento
 */
int banco_pix(Banco* b, const char* conta_origem, const char* conta_destino, double valor);

/*
 * FUNÇÃO: banco_cartao()
 * DESCRIÇÃO:
 *  - Debita valor de uma compra no cartão diretamente do saldo
 *  - Valida saldo e registra movimento
 */
int banco_cartao(Banco* b, const char* conta, double valor);

#endif
