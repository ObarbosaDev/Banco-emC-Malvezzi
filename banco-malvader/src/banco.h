#ifndef BANCO_H
#define BANCO_H

#include "cliente.h"

typedef struct {
    ListaClientes clientes;
    char arquivo_clientes[256];
    char arquivo_movimentos[256];
} Banco;

void banco_init(Banco* b, const char* arq_cli, const char* arq_mov);
void banco_free(Banco* b);

int banco_carregar(Banco* b);
int banco_salvar(Banco* b);

int banco_abrir_conta(Banco* b, Cliente c);
int banco_encerrar_conta(Banco* b, const char* conta, const char* senha);
int banco_buscar_por_conta(Banco* b, const char* conta);
int banco_buscar_por_cpf(Banco* b, const char* cpf);
Cliente* banco_obter_cliente(Banco* b, int idx);
int banco_alterar_dados(Banco* b, const char* conta, Cliente novo);

int banco_depositar(Banco* b, const char* conta, double valor);
int banco_sacar(Banco* b, const char* conta, const char* senha, double valor);
double banco_consultar_saldo(Banco* b, const char* conta);

void banco_listar_ordenado_por_nome(Banco* b);
void banco_listar_ordenado_por_conta(Banco* b);
void banco_exibir_extrato(Banco* b, const char* conta);

#endif