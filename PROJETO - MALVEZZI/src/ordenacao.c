/*
 * ARQUIVO: src/ordenacao.c
 * RESPONSABILIDADE: Implementação de Quick Sort recursivo
 * 
 * CAMADA: Domínio (Algoritmos)
 * 
 * Quick Sort é um algoritmo de ordenação recursivo eficiente.
 * Utiliza estratégia Divide-and-Conquer:
 *  1. Divide: particiona em relação a um pivô
 *  2. Conquista: ordena recursivamente cada partição
 *  3. Combina: resultado está automaticamente ordenado
 * 
 * TÓPICOS APLICADOS:
 *  - Recursividade (função chamando a si mesma)
 *  - Algoritmo Divide-and-Conquer
 *  - Troca de elementos (swap)
 *  - Comparação de strings (strcmp)
 *  - Funções estáticas (escopo privado do módulo)
 */

#include "ordenacao.h"
#include <string.h>
#include <stddef.h>

/*
 * FUNÇÃO: trocar_clientes()
 * PARÂMETROS: a, b - ponteiros para clientes a trocar
 * DESCRIÇÃO: Função auxiliar que realiza troca de dois clientes.
 *            Usa variável temporária para armazenar um durante troca.
 * 
 * TÓPICO: Ponteiros e passagem por referência
 * 
 * NOTA: Static = função privada (não visível fora deste módulo)
 */
static void trocar_clientes(Cliente* a, Cliente* b) {
    Cliente tmp = *a;  // Armazena cliente em a
    *a = *b;           // Copia cliente de b para a
    *b = tmp;          // Copia cliente original (tmp) para b
}

/*
 * FUNÇÃO: particionar()
 * PARÂMETROS:
 *  - v: vetor de clientes
 *  - l: índice esquerdo
 *  - r: índice direito (posição do pivô)
 *  - tipo: 0 = por nome, 1 = por conta
 * RETORNO: int - posição final do pivô após particionamento
 * 
 * DESCRIÇÃO: Particiona vetor em relação ao elemento pivot (v[r]).
 *            - Elementos menores que pivô ficam à esquerda
 *            - Elementos maiores ficam à direita
 *            - Pivô fica na posição correta (retornada)
 * 
 * ALGORITMO (Hoare partition):
 *  1. Escolhe pivô = v[r]
 *  2. i = l, varre vetor com j
 *  3. Se v[j] <= pivô, troca com v[i] e incrementa i
 *  4. Ao final, coloca pivô em posição i
 *  5. Retorna posição i (onde pivô ficou)
 * 
 * TÓPICO: Algoritmo Divide-and-Conquer
 */
static int particionar(Cliente v[], int l, int r, int tipo) {
    Cliente pivo = v[r];  // Pivô é o último elemento
    int i = l;            // Índice do primeiro elemento menor que pivô

    // Varrer vetor procurando elementos menores que pivô
    for (int j = l; j < r; j++) {
        int cmp = 0;
        
        if (tipo == 0) {
            cmp = strcmp(v[j].nome, pivo.nome);  // Compara strings (nome)
        } else {
            cmp = strcmp(v[j].conta, pivo.conta);  // Compara strings (conta)
        }

        // Se elemento é menor/igual ao pivô, move para esquerda
        if (cmp <= 0) {
            trocar_clientes(&v[i], &v[j]);  // Troca v[i] com v[j]
            i++;  // Incrementa índice de separação
        }
    }

    // Colocar pivô em sua posição final correta
    trocar_clientes(&v[i], &v[r]);
    return i;  // Retorna posição do pivô
}

/*
 * FUNÇÃO: quicksort() - Implementação Recursiva
 * PARÂMETROS:
 *  - v: vetor
 *  - l: índice esquerdo
 *  - r: índice direito
 *  - tipo: 0 = por nome, 1 = por conta
 * 
 * DESCRIÇÃO: Ordena vetor recursivamente.
 * 
 * PSEUDOCÓDIGO:
 * ┌─ Se l < r (vetor tem mais de 1 elemento):
 * │   ├─ p = particionar(v, l, r, tipo)
 * │   ├─ quicksort(v, l, p-1, tipo)  [Ordena esquerda]
 * │   └─ quicksort(v, p+1, r, tipo)  [Ordena direita]
 * └─ Caso contrário: retorna (caso base)
 * 
 * CASOS BASE (quando parar de recursão):
 *  - l > r: vetor vazio (não faz nada)
 *  - l == r: vetor com 1 elemento (já está ordenado)
 * 
 * TÓPICOS:
 *  - Recursividade: função chama a si mesma
 *  - Caso base: l < r
 *  - Duas chamadas recursivas para l e r
 *  - Usa particionamento para dividir problema
 */
void quicksort(Cliente v[], int l, int r, int tipo) {
    // Caso base: vetor com 0 ou 1 elemento não precisa ordenar
    if (l < r) {
        // Particiona vetor em relação ao pivô
        int p = particionar(v, l, r, tipo);
        
        // Recursivamente ordena partição esquerda (elementos menores que pivô)
        quicksort(v, l, p - 1, tipo);
        
        // Recursivamente ordena partição direita (elementos maiores que pivô)
        quicksort(v, p + 1, r, tipo);
    }
    // Se l >= r, vetor está ordenado (caso base implícito)
}
