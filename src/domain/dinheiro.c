#include "banco/dinheiro.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

bool dinheiro_converter(const char *texto, Centavos *valor) {
    if (!texto || !valor) {
        return false;
    }

    const unsigned char *cursor = (const unsigned char *)texto;
    while (isspace(*cursor)) {
        cursor++;
    }

    bool negativo = false;
    if (*cursor == '+' || *cursor == '-') {
        negativo = *cursor == '-';
        cursor++;
    }

    if (!isdigit(*cursor)) {
        return false;
    }

    uint64_t parte_inteira = 0;
    while (isdigit(*cursor)) {
        unsigned int digito = (unsigned int)(*cursor - (unsigned char)'0');
        if (parte_inteira > (UINT64_MAX - digito) / 10U) {
            return false;
        }
        parte_inteira = parte_inteira * 10U + digito;
        cursor++;
    }

    uint64_t parte_decimal = 0;
    if (*cursor == '.' || *cursor == ',') {
        cursor++;
        if (!isdigit(*cursor)) {
            return false;
        }

        parte_decimal = (uint64_t)(*cursor - (unsigned char)'0') * 10U;
        cursor++;
        if (isdigit(*cursor)) {
            parte_decimal += (uint64_t)(*cursor - (unsigned char)'0');
            cursor++;
        }
        if (isdigit(*cursor)) {
            return false;
        }
    }

    while (isspace(*cursor)) {
        cursor++;
    }
    if (*cursor != '\0') {
        return false;
    }

    const uint64_t limite = negativo
        ? (uint64_t)INT64_MAX + 1U
        : (uint64_t)INT64_MAX;
    if (parte_inteira > (limite - parte_decimal) / 100U) {
        return false;
    }

    const uint64_t magnitude = parte_inteira * 100U + parte_decimal;
    if (negativo) {
        if (magnitude == (uint64_t)INT64_MAX + 1U) {
            *valor = INT64_MIN;
        } else {
            *valor = -(Centavos)magnitude;
        }
    } else {
        *valor = (Centavos)magnitude;
    }

    return true;
}

void dinheiro_formatar(Centavos valor, char *saida, size_t capacidade) {
    if (!saida || capacidade == 0U) {
        return;
    }

    const bool negativo = valor < 0;
    const uint64_t magnitude = negativo
        ? (uint64_t)(-(valor + 1)) + 1U
        : (uint64_t)valor;

    (void)snprintf(
        saida,
        capacidade,
        "%s%" PRIu64 ",%02" PRIu64,
        negativo ? "-" : "",
        magnitude / 100U,
        magnitude % 100U
    );
}
