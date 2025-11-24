/*
 * ARQUIVO: src/io.h
 * RESPONSABILIDADE: Interface de entrada/saída segura
 * 
 * Define funções para:
 *  - Leitura segura de linhas com fgets (previne buffer overflow)
 *  - Leitura de opções do menu
 *  - Exibição formatada de dados
 */

#ifndef IO_H
#define IO_H

#include "cliente.h"
#include <stddef.h>
#include <stddef.h>

/*
 * FUNÇÃO: ler_opcao()
 * RETORNO: int - opção lida do usuário
 * DESCRIÇÃO: Lê um inteiro do teclado e limpa buffer de entrada.
 *            Retorna -1 se falhar na leitura.
 */
int ler_opcao(void);

/*
 * FUNÇÃO: ler_linha()
 * PARÂMETROS:
 *  - buf: buffer onde armazenar a linha
 *  - tam: tamanho máximo do buffer
 * DESCRIÇÃO: Lê linha de texto com fgets (seguro) e remove newline.
 *            Previne buffer overflow e segmentation fault.
 * TÓPICO: Entrada segura com fgets
 */
void ler_linha(char* buf, size_t tam);

/*
 * FUNÇÃO: imprimir_cliente()
 * PARÂMETROS: c - ponteiro para Cliente (const = não modifica)
 * DESCRIÇÃO: Exibe todos os dados do cliente em formato formatado.
 */
void imprimir_cliente(const Cliente* c);

#endif
