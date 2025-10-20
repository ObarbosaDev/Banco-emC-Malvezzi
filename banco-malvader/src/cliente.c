#include "cliente.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void cliente_init(Cliente* c) {
    memset(c, 0, sizeof(Cliente));
    c->saldo = 0.0;
    c->ativo = 1;
}

void cliente_imprimir(const Cliente* c) {
    printf("\n========================================\n");
    printf("           DADOS DO CLIENTE\n");
    printf("========================================\n");
    printf("Agencia........: %s\n", c->agencia);
    printf("Conta..........: %s\n", c->conta);
    printf("Nome...........: %s\n", c->nome);
    printf("CPF............: %s\n", c->cpf);
    printf("Data Nasc......: %s\n", c->data_nasc);
    printf("Telefone.......: %s\n", c->telefone);
    printf("Endereco.......: %s, %s\n", c->endereco, c->numero_casa);
    printf("Bairro.........: %s\n", c->bairro);
    printf("Cidade/Estado..: %s/%s\n", c->cidade, c->estado);
    printf("CEP............: %s\n", c->cep);
    printf("Saldo..........: R$ %.2f\n", c->saldo);
    printf("Status.........: %s\n", c->ativo ? "ATIVA" : "ENCERRADA");
    printf("========================================\n");
}

int cliente_validar_cpf(const char* cpf) {
    int digitos = 0;
    for (int i = 0; cpf[i]; i++) {
        if (isdigit(cpf[i])) digitos++;
    }
    return digitos == 11;
}

int cliente_validar_cep(const char* cep) {
    int digitos = 0;
    for (int i = 0; cep[i]; i++) {
        if (isdigit(cep[i])) digitos++;
    }
    return digitos == 8;
}