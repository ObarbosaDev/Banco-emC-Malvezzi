#ifndef BANCO_DOMAIN_ORDENACAO_H
#define BANCO_DOMAIN_ORDENACAO_H

#include <stddef.h>

#include "banco/cliente.h"

typedef enum {
    ORDENAR_CLIENTE_POR_NOME = 0,
    ORDENAR_CLIENTE_POR_CONTA
} OrdenacaoCliente;

void ordenar_clientes(Cliente *clientes, size_t quantidade, OrdenacaoCliente criterio);

#endif
