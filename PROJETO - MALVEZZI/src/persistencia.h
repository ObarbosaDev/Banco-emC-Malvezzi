/*
 * ARQUIVO: src/persistencia.h
 * RESPONSABILIDADE: Interface de persistência em arquivos
 * 
 * CAMADA: Persistência (Acesso a Dados)
 * 
 * Define operações de leitura/escrita em arquivos texto:
 *  - Carregar clientes de arquivo
 *  - Salvar clientes em arquivo
 *  - Registrar movimentos (extrato)
 */

#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "cliente.h"
#include <stddef.h>

/*
 * FUNÇÃO: carregar_clientes()
 * PARÂMETROS:
 *  - arq: caminho do arquivo a ler
 *  - dados: ponteiro para ponteiro (para modificar dados externamente)
 *  - tam: ponteiro para tamanho (será atualizado)
 *  - cap: ponteiro para capacidade (será atualizado)
 * RETORNO: 1 = sucesso, 0 = erro de memória
 * DESCRIÇÃO: Lê arquivo CSV (;-delimitado) e popula vetor dinâmico.
 *            Realoca memória conforme necessário (expansão sob demanda).
 *            Se arquivo não existe, retorna 1 (não é erro).
 * 
 * TÓPICO: Arquivo texto e alocação dinâmica
 */
int carregar_clientes(const char* arq, Cliente** dados, size_t* tam, size_t* cap);

/*
 * FUNÇÃO: salvar_clientes()
 * PARÂMETROS:
 *  - arq: caminho do arquivo para escrever
 *  - dados: vetor de clientes a salvar
 *  - tam: quantidade de clientes
 * RETORNO: 1 = sucesso, 0 = erro de escrita
 * DESCRIÇÃO: Escreve todos os clientes em arquivo (format CSV-like).
 *            Sobrescreve arquivo anterior completamente.
 *            Formato: AGENCIA;CONTA;NOME;...;SALDO;ATIVO
 */
int salvar_clientes(const char* arq, const Cliente* dados, size_t tam);

/*
 * FUNÇÃO: registrar_movimento()
 * PARÂMETROS:
 *  - arq: caminho do arquivo de movimentos
 *  - conta: número da conta que originou o movimento
 *  - tipo: tipo de movimento ("DEPOSITO" ou "SAQUE")
 *  - valor: valor da transação
 *  - saldo: saldo resultante após transação
 * RETORNO: 1 = sucesso, 0 = erro de escrita
 * DESCRIÇÃO: Append (adiciona) um registro de movimento em arquivo.
 *            Usa data/hora atual para timestamp.
 *            Formato: DATA;CONTA;TIPO;VALOR;SALDO_RESULTANTE
 * 
 * TÓPICO: Persistência de log de transações
 */
int registrar_movimento(const char* arq, const char* conta, const char* tipo, double valor, double saldo);

#endif
