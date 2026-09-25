#include "domain/ordenacao.h"

#include <stddef.h>
#include <string.h>

static int comparar(
    const Cliente *primeiro,
    const Cliente *segundo,
    OrdenacaoCliente criterio
) {
    if (criterio == ORDENAR_CLIENTE_POR_NOME) {
        return strcmp(primeiro->nome, segundo->nome);
    }
    return strcmp(primeiro->conta, segundo->conta);
}

static void trocar(Cliente *primeiro, Cliente *segundo) {
    Cliente temporario = *primeiro;
    *primeiro = *segundo;
    *segundo = temporario;
}

static void quicksort(
    Cliente *clientes,
    ptrdiff_t esquerda,
    ptrdiff_t direita,
    OrdenacaoCliente criterio
) {
    ptrdiff_t i = esquerda;
    ptrdiff_t j = direita;
    const Cliente pivo = clientes[esquerda + (direita - esquerda) / 2];

    while (i <= j) {
        while (comparar(&clientes[i], &pivo, criterio) < 0) {
            i++;
        }
        while (comparar(&clientes[j], &pivo, criterio) > 0) {
            j--;
        }

        if (i <= j) {
            trocar(&clientes[i], &clientes[j]);
            i++;
            j--;
        }
    }

    if (esquerda < j) {
        quicksort(clientes, esquerda, j, criterio);
    }
    if (i < direita) {
        quicksort(clientes, i, direita, criterio);
    }
}

void ordenar_clientes(Cliente *clientes, size_t quantidade, OrdenacaoCliente criterio) {
    if (!clientes || quantidade < 2U) {
        return;
    }

    quicksort(clientes, 0, (ptrdiff_t)quantidade - 1, criterio);
}
