#ifndef CLIENTE_H
#define CLIENTE_H

#include <stddef.h>

typedef struct {
    char agencia[8];
    char conta[16];
    char nome[100];
    char cpf[15];
    char data_nasc[11];
    char telefone[20];
    char endereco[120];
    char cep[10];
    char local[60];
    char numero_casa[10];
    char bairro[60];
    char cidade[60];
    char estado[3];
    char senha[20];
    double saldo;
    int ativo;
} Cliente;

typedef struct {
    Cliente* dados;
    size_t tam;
    size_t cap;
} ListaClientes;

void cliente_init(Cliente* c);
void cliente_imprimir(const Cliente* c);
int cliente_validar_cpf(const char* cpf);
int cliente_validar_cep(const char* cep);

#endif