#include "io.h"
#include <stdio.h>
#include <string.h>

void ler_linha(char* buffer, size_t tamanho) {
    if (fgets(buffer, tamanho, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    getchar();
}