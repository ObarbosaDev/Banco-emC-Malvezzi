#include "ordenacao.h"
#include <string.h>

static void trocar(Cliente* a, Cliente* b) {
    Cliente tmp = *a;
    *a = *b;
    *b = tmp;
}

static int particionar_nome(Cliente v[], int l, int r) {
    Cliente pivo = v[r];
    int i = l;
    
    for (int j = l; j < r; j++) {
        if (strcmp(v[j].nome, pivo.nome) <= 0) {
            trocar(&v[i], &v[j]);
            i++;
        }
    }
    trocar(&v[i], &v[r]);
    return i;
}

void quicksort_por_nome(Cliente v[], int l, int r) {
    if (l < r) {
        int p = particionar_nome(v, l, r);
        quicksort_por_nome(v, l, p - 1);
        quicksort_por_nome(v, p + 1, r);
    }
}

static int particionar_conta(Cliente v[], int l, int r) {
    Cliente pivo = v[r];
    int i = l;
    
    for (int j = l; j < r; j++) {
        if (strcmp(v[j].conta, pivo.conta) <= 0) {
            trocar(&v[i], &v[j]);
            i++;
        }
    }
    trocar(&v[i], &v[r]);
    return i;
}

void quicksort_por_conta(Cliente v[], int l, int r) {
    if (l < r) {
        int p = particionar_conta(v, l, r);
        quicksort_por_conta(v, l, p - 1);
        quicksort_por_conta(v, p + 1, r);
    }
}