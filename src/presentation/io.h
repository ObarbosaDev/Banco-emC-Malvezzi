#ifndef BANCO_PRESENTATION_IO_H
#define BANCO_PRESENTATION_IO_H

#include <stddef.h>

#include "banco/cliente.h"

typedef enum {
    IO_SUCESSO = 0,
    IO_FIM_ENTRADA,
    IO_ENTRADA_INVALIDA,
    IO_ENTRADA_LONGA
} IoResultado;

IoResultado io_ler_linha(char *destino, size_t capacidade);
IoResultado io_ler_inteiro(int *valor);
IoResultado io_ler_valor(Centavos *valor);
void io_imprimir_cliente(const Cliente *cliente);
void io_imprimir_resumo_cliente(const Cliente *cliente);

#endif
