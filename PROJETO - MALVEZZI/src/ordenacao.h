/*
 * ARQUIVO: src/ordenacao.h
 * RESPONSABILIDADE: Interface para ordenação de clientes
 * 
 * Implementa Quick Sort recursivo para ordenar por:
 *  - Nome (tipo=0)
 *  - Número de conta (tipo=1)
 * 
 * TÓPICO EDUCACIONAL: Recursividade
 */

#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "cliente.h"
#include <stddef.h>

/*
 * FUNÇÃO: quicksort()
 * PARÂMETROS:
 *  - v: vetor de clientes a ordenar
 *  - l: índice esquerdo (início)
 *  - r: índice direito (fim)
 *  - tipo: 0 = ordena por nome, 1 = ordena por número de conta
 * DESCRIÇÃO: Implementação recursiva de Quick Sort.
 *            Algoritmo Divide-and-Conquer:
 *            1. Escolhe pivô (último elemento)
 *            2. Particiona vetor (elementos < pivô à esquerda)
 *            3. Recursivamente ordena left e right
 *            4. Caso base: l >= r (vetor de 1 elemento ou vazio)
 * 
 * COMPLEXIDADE: O(n log n) médio, O(n²) pior caso
 * 
 * TÓPICOS EDUCACIONAIS:
 *  - Recursividade: função chama a si mesma
 *  - Algoritmo Divide-and-Conquer
 *  - Comparação de strings (strcmp)
 */
void quicksort(Cliente v[], int l, int r, int tipo);

#endif
