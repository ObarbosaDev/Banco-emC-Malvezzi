/*
 * ARQUIVO: src/io.c
 * RESPONSABILIDADE: Implementação de funções de entrada/saída segura
 * 
 * CAMADA: Interface (Apresentação)
 * 
 * Fornece abstração segura para entrada de dados do usuário.
 * Todas as funções usam fgets em vez de scanf com %s (mais seguro).
 * 
 * TÓPICOS APLICADOS:
 *  - Entrada segura com fgets
 *  - Remoção de newline com strcspn
 *  - Limpeza de buffer com getchar()
 */

#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


/*
 * FUNÇÃO: ler_opcao()
 * DESCRIÇÃO: Lê inteiro com tratamento de erro e limpeza de buffer.
 *            scanf deixa caracteres no buffer; getchar() remove-os.
 */
int ler_opcao(void) {
    int op;
    printf("Opcao: ");
    if (scanf("%d", &op) != 1) {  // Verifica se scanf leu com sucesso
        while (getchar() != '\n');  // Limpa buffer se falhou
        return -1;
    }
    while (getchar() != '\n');  // Limpa newline deixado por scanf
    return op;
}

/*
 * FUNÇÃO: ler_linha()
 * DESCRIÇÃO: Lê até (tam-1) caracteres com fgets, depois remove newline.
 *            Estratégia segura: fgets nunca cria buffer overflow.
 * 
 * TÓPICO: Entrada segura
 * Como funciona:
 *  1. fgets lê no máximo (tam-1) bytes (deixa espaço para '\0')
 *  2. strcspn encontra posição do '\n'
 *  3. Substitui '\n' por '\0' para remover newline da string
 */
void ler_linha(char* buf, size_t tam) {
    if (fgets(buf, tam, stdin)) {
        buf[strcspn(buf, "\n")] = 0;  // Remove newline
    }
}

/*
 * FUNÇÃO: imprimir_cliente()
 * DESCRIÇÃO: Exibe estrutura Cliente em formato tabular e legível.
 *            Usa const para garantir que não modificará dados.
 */
void imprimir_cliente(const Cliente* c) {
    printf("\n=== DADOS DO CLIENTE ===\n");
    printf("Agencia: %s\n", c->agencia);
    printf("Conta: %s\n", c->conta);
    printf("Nome: %s\n", c->nome);
    printf("CPF: %s\n", c->cpf);
    printf("Data Nasc.: %s\n", c->data_nasc);
    printf("Telefone: %s\n", c->telefone);
    printf("Endereco: %s\n", c->endereco);
    printf("CEP: %s\n", c->cep);
    printf("Local: %s\n", c->local);
    printf("Numero: %s\n", c->numero_casa);
    printf("Bairro: %s\n", c->bairro);
    printf("Cidade: %s\n", c->cidade);
    printf("Estado: %s\n", c->estado);
    printf("Saldo: R$ %.2f\n", c->saldo);
    printf("Status: %s\n", c->ativo ? "Ativa" : "Encerrada");
    printf("\n");
}
