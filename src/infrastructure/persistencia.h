#ifndef BANCO_INFRASTRUCTURE_PERSISTENCIA_H
#define BANCO_INFRASTRUCTURE_PERSISTENCIA_H

#include <stddef.h>

#include "banco/cliente.h"

typedef enum {
    PERSISTENCIA_OK = 0,
    PERSISTENCIA_ERRO_IO,
    PERSISTENCIA_ERRO_FORMATO,
    PERSISTENCIA_ERRO_MEMORIA,
    PERSISTENCIA_TRANSACAO_PENDENTE
} PersistenciaResultado;

typedef struct {
    const char *conta;
    const char *tipo;
    Centavos valor_centavos;
    Centavos saldo_centavos;
} Movimento;

PersistenciaResultado persistencia_recuperar_transacao(
    const char *arquivo_clientes,
    const char *arquivo_movimentos
);
PersistenciaResultado persistencia_carregar_clientes(
    const char *arquivo,
    Cliente **clientes,
    size_t *quantidade,
    size_t *capacidade
);
PersistenciaResultado persistencia_salvar_clientes(
    const char *arquivo,
    const Cliente *clientes,
    size_t quantidade
);
PersistenciaResultado persistencia_confirmar_transacao(
    const char *arquivo_clientes,
    const Cliente *clientes,
    size_t quantidade_clientes,
    const char *arquivo_movimentos,
    const Movimento *movimentos,
    size_t quantidade_movimentos
);

#endif
