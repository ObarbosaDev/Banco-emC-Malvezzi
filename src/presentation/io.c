#include "presentation/io.h"

#include "banco/dinheiro.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IoResultado io_ler_linha(char *destino, size_t capacidade) {
    if (!destino || capacidade < 2U) {
        return IO_ENTRADA_INVALIDA;
    }

    if (!fgets(destino, (int)capacidade, stdin)) {
        destino[0] = '\0';
        return IO_FIM_ENTRADA;
    }

    char *quebra = strchr(destino, '\n');
    if (quebra) {
        *quebra = '\0';
        const size_t tamanho = strlen(destino);
        if (tamanho > 0U && destino[tamanho - 1U] == '\r') {
            destino[tamanho - 1U] = '\0';
        }
        return IO_SUCESSO;
    }

    const int proximo = getchar();
    if (proximo == '\n' || proximo == EOF) {
        const size_t tamanho = strlen(destino);
        if (tamanho > 0U && destino[tamanho - 1U] == '\r') {
            destino[tamanho - 1U] = '\0';
        }
        return IO_SUCESSO;
    }

    int caractere = proximo;
    while (caractere != '\n' && caractere != EOF) {
        caractere = getchar();
    }
    destino[0] = '\0';
    return IO_ENTRADA_LONGA;
}

IoResultado io_ler_inteiro(int *valor) {
    if (!valor) {
        return IO_ENTRADA_INVALIDA;
    }

    char linha[64];
    const IoResultado leitura = io_ler_linha(linha, sizeof linha);
    if (leitura != IO_SUCESSO) {
        return leitura;
    }

    errno = 0;
    char *fim = NULL;
    const long convertido = strtol(linha, &fim, 10);
    while (fim && (*fim == ' ' || *fim == '\t')) {
        fim++;
    }
    if (errno == ERANGE
        || fim == linha
        || !fim
        || *fim != '\0'
        || convertido < INT_MIN
        || convertido > INT_MAX) {
        return IO_ENTRADA_INVALIDA;
    }

    *valor = (int)convertido;
    return IO_SUCESSO;
}

IoResultado io_ler_valor(Centavos *valor) {
    if (!valor) {
        return IO_ENTRADA_INVALIDA;
    }

    char linha[64];
    const IoResultado leitura = io_ler_linha(linha, sizeof linha);
    if (leitura != IO_SUCESSO) {
        return leitura;
    }
    return dinheiro_converter(linha, valor) ? IO_SUCESSO : IO_ENTRADA_INVALIDA;
}

void io_imprimir_cliente(const Cliente *cliente) {
    if (!cliente) {
        return;
    }

    char saldo[64];
    dinheiro_formatar(cliente->saldo_centavos, saldo, sizeof saldo);
    printf("\n=== DADOS DO CLIENTE ===\n");
    printf("Agencia: %s\n", cliente->agencia);
    printf("Conta: %s\n", cliente->conta);
    printf("Nome: %s\n", cliente->nome);
    printf("CPF: %s\n", cliente->cpf);
    printf("Data de nascimento: %s\n", cliente->data_nascimento);
    printf("Telefone: %s\n", cliente->telefone[0] ? cliente->telefone : "Nao informado");
    printf("Endereco: %s, %s\n", cliente->endereco, cliente->numero);
    printf("CEP: %s\n", cliente->cep);
    printf("Local: %s\n", cliente->local);
    printf("Bairro: %s\n", cliente->bairro);
    printf("Cidade/UF: %s/%s\n", cliente->cidade, cliente->estado);
    printf("Saldo: R$ %s\n", saldo);
    printf("Status: %s\n\n", cliente->ativo ? "Ativa" : "Encerrada");
}

void io_imprimir_resumo_cliente(const Cliente *cliente) {
    if (!cliente) {
        return;
    }

    char saldo[64];
    dinheiro_formatar(cliente->saldo_centavos, saldo, sizeof saldo);
    printf(
        "Conta: %-15s | Nome: %-30s | CPF: %-14s | Saldo: R$ %s\n",
        cliente->conta,
        cliente->nome,
        cliente->cpf,
        saldo
    );
}
