/*
 * ARQUIVO: src/cliente.h
 * RESPONSABILIDADE: Definição da estrutura Cliente (modelo de dados)
 * 
 * CAMADA: Domínio
 * 
 * Este arquivo contém a definição da struct Cliente que representa
 * um cliente bancário com todos os seus dados cadastrais.
 * 
 * TÓPICOS APLICADOS:
 *  - Structs para agrupamento de dados heterogêneos
 *  - typedef para simplificar sintaxe
 *  - Tamanho de buffers conforme requisitos de entrada
 */

#ifndef CLIENTE_H
#define CLIENTE_H

#include <stddef.h>

/*
 * STRUCT: Cliente
 * DESCRIÇÃO: Agrupa todas as informações de um cliente bancário.
 *            Cada campo armazena um tipo específico de dado.
 * 
 * CAMPOS:
 *  - agencia: Código da agência (4 dígitos)
 *  - conta: Número da conta (único)
 *  - nome: Nome completo do cliente
 *  - cpf: CPF (formato: XXX.XXX.XXX-XX)
 *  - data_nasc: Data de nascimento (formato: YYYY-MM-DD)
 *  - telefone: Telefone de contato
 *  - endereco: Endereço residencial
 *  - cep: Código de endereçamento postal
 *  - local: Identificação de zona/bairro
 *  - numero_casa: Número do imóvel
 *  - bairro: Nome do bairro
 *  - cidade: Cidade de residência
 *  - estado: Estado (UF - 2 caracteres)
 *  - senha: Senha de acesso à conta
 *  - saldo: Saldo atual da conta (sempre >= 0)
 *  - ativo: Status (1 = ativa, 0 = encerrada)
 * 
 * OBSERVAÇÃO: Saldo e senha pertencem a Cliente, NÃO a uma struct Conta separada.
 */
typedef struct {
    char agencia[8];        // Tamanho: 4 dígitos + terminador nulo
    char conta[16];         // Tamanho suficiente para formato variado
    char nome[100];         // Tamanho para nomes completos
    char cpf[15];           // Tamanho: XXX.XXX.XXX-XX (14 chars + \0)
    char data_nasc[11];     // Tamanho: YYYY-MM-DD (10 chars + \0)
    char telefone[20];      // Tamanho para telefone com DDD
    char endereco[120];     // Tamanho para endereços longos
    char cep[10];           // Tamanho: XXXXX-XXX (8 chars + \0)
    char local[60];         // Tamanho para identificação de zona
    char numero_casa[10];   // Tamanho para número de imóvel
    char bairro[60];        // Tamanho para nome de bairro
    char cidade[60];        // Tamanho para nome de cidade
    char estado[3];         // Tamanho: UF (2 chars + \0)
    char senha[20];         // Tamanho para senha de acesso
    double saldo;           // Saldo em ponto flutuante (até 2 casas decimais)
    int ativo;              // 1 = ativa, 0 = encerrada
} Cliente;

#endif
