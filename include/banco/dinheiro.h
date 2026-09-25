#ifndef BANCO_DOMAIN_DINHEIRO_H
#define BANCO_DOMAIN_DINHEIRO_H

#include <stdbool.h>
#include <stddef.h>

#include "banco/cliente.h"

bool dinheiro_converter(const char *texto, Centavos *valor);
void dinheiro_formatar(Centavos valor, char *saida, size_t capacidade);

#endif
