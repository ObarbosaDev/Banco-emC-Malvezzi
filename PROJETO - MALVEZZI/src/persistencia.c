/*
 * ARQUIVO: src/persistencia.c
 * RESPONSABILIDADE: Implementação de persistência em arquivos
 * 
 * CAMADA: Persistência (Acesso a Dados)
 * 
 * Operações de leitura/escrita em arquivos texto com format CSV-like.
 * Gerencia alocação dinâmica ao carregar dados.
 * 
 * TÓPICOS APLICADOS:
 *  - Alocação dinâmica (malloc, realloc, free)
 *  - Manipulação de arquivos (fopen, fgets, fprintf, fclose)
 *  - Strings e sscanf para parsing
 *  - Data/hora com time.h
 */

#include "persistencia.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>

/*
 * FUNÇÃO: carregar_clientes()
 * TÓPICOS:
 *  - Leitura com fgets (segura)
 *  - sscanf para parsing CSV
 *  - Realocação dinâmica com realloc sob demanda
 */
int carregar_clientes(const char* arq, Cliente** dados, size_t* tam, size_t* cap) {
    FILE* f = fopen(arq, "r");
    if (!f) {
        printf("Aviso: Arquivo nao encontrado, criando novo\n");
        return 1;  // Não é erro; inicia com banco vazio
    }

    char linha[512];  // Buffer para linha do arquivo
    while (fgets(linha, sizeof linha, f)) {
        Cliente c = {0};  // Inicializa com zeros
        linha[strcspn(linha, "\n")] = 0;  // Remove newline

        // Variável temporária para ler campo ativo
        int ativo_tmp = 0;
        
        // Cada %Xd[^;] significa: lê até X caracteres, parando em ';'
        // IMPORTANTE: order dos argumentos DEVE ser igual ao da struct Cliente
        if (sscanf(linha, "%7[^;];%15[^;];%99[^;];%14[^;];%10[^;];%19[^;];%119[^;];%9[^;];%59[^;];%9[^;];%59[^;];%59[^;];%2[^;];%19[^;];%lf;%d",
                   c.agencia, c.conta, c.nome, c.cpf, c.data_nasc, c.telefone,
                   c.endereco, c.cep, c.local, c.numero_casa, c.bairro,
                   c.cidade, c.estado, c.senha, &c.saldo, &ativo_tmp) == 16) {
            c.ativo = ativo_tmp;

            if (*tam == *cap) {
                size_t nova = (*cap == 0) ? 4 : (*cap * 2);  // Inicia com 4, depois dobra
                Cliente* tmp = realloc(*dados, nova * sizeof **dados);
                if (!tmp) {
                    fprintf(stderr, "Erro: memoria insuficiente\n");
                    fclose(f);
                    return 0;  // Falha de memória
                }
                *dados = tmp;
                *cap = nova;
            }

            // Adicionar cliente ao vetor
            (*dados)[(*tam)++] = c;
        }
    }

    fclose(f);
    return 1;
}

/*
 * FUNÇÃO: salvar_clientes()
 * DESCRIÇÃO: Escreve todos os clientes em arquivo.
 *            Formato CSV-like com ; como delimitador.
 */
int salvar_clientes(const char* arq, const Cliente* dados, size_t tam) {
    FILE* f = fopen(arq, "w");
    if (!f) {
        fprintf(stderr, "Erro: Nao foi possivel abrir arquivo para escrita\n");
        return 0;
    }

    // Escrever cabeçalho (opcional mas útil para legibilidade)
    // fprintf(f, "AGENCIA;CONTA;NOME;CPF;DATA_NASC;TELEFONE;ENDERECO;CEP;LOCAL;NUMERO_CASA;BAIRRO;CIDADE;ESTADO;SENHA;SALDO;ATIVO\n");

    // Escrever cada cliente em uma linha
    for (size_t i = 0; i < tam; i++) {
        const Cliente* c = &dados[i];
        fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%.2f;%d\n",
                c->agencia, c->conta, c->nome, c->cpf, c->data_nasc, c->telefone,
                c->endereco, c->cep, c->local, c->numero_casa, c->bairro,
                c->cidade, c->estado, c->senha, c->saldo, c->ativo);
    }

    fclose(f);
    return 1;
}

/*
 * FUNÇÃO: registrar_movimento()
 * DESCRIÇÃO: Registra transação (depósito/saque) em arquivo de movimentos.
 *            Append (adiciona) ao final do arquivo.
 *            Gera timestamp automático com time.h.
 * 
 * TÓPICO: time.h para data/hora atual
 */
int registrar_movimento(const char* arq, const char* conta, const char* tipo, double valor, double saldo) {
    FILE* f = fopen(arq, "a");  // Modo "a" = append (adiciona ao final)
    if (!f) {
        fprintf(stderr, "Erro: Nao foi possivel registrar movimento\n");
        return 0;
    }

    time_t agora = time(NULL);
    struct tm* data = localtime(&agora);
    
    // Escrever registro: DATA;CONTA;TIPO;VALOR;SALDO
    fprintf(f, "%04d-%02d-%02d;%s;%s;%.2f;saldo=%.2f\n",
            data->tm_year + 1900,  // Ano (adiciona 1900 ao offset)
            data->tm_mon + 1,       // Mês (adiciona 1 pois janeiro = 0)
            data->tm_mday,          // Dia do mês
            conta, tipo, valor, saldo);

    fclose(f);
    return 1;
}
