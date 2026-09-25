#ifndef BANCO_CLIENTE_H
#define BANCO_CLIENTE_H

#include <stdbool.h>
#include <stdint.h>

typedef int64_t Centavos;

typedef struct {
    char agencia[8];
    char conta[16];
    char nome[100];
    char cpf[15];
    char data_nascimento[11];
    char telefone[20];
    char endereco[120];
    char cep[10];
    char local[60];
    char numero[10];
    char bairro[60];
    char cidade[60];
    char estado[3];
    Centavos saldo_centavos;
    bool ativo;
} Cliente;

typedef struct {
    char nome[100];
    char telefone[20];
    char endereco[120];
} ClienteAtualizacao;

#endif
