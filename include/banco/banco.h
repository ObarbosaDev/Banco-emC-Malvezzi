#ifndef BANCO_BANCO_H
#define BANCO_BANCO_H

#include <stddef.h>

#include "banco/cliente.h"

typedef struct Banco Banco;

typedef enum {
    BANCO_OK = 0,
    BANCO_ERRO_ARGUMENTO,
    BANCO_ERRO_MEMORIA,
    BANCO_ERRO_PERSISTENCIA,
    BANCO_TRANSACAO_PENDENTE,
    BANCO_ERRO_DADOS_INVALIDOS,
    BANCO_ERRO_CONTA_EXISTENTE,
    BANCO_ERRO_CPF_EXISTENTE,
    BANCO_ERRO_CONTA_NAO_ENCONTRADA,
    BANCO_ERRO_VALOR_INVALIDO,
    BANCO_ERRO_SALDO_INSUFICIENTE,
    BANCO_ERRO_SALDO_NAO_ZERADO,
    BANCO_ERRO_MESMA_CONTA
} BancoResultado;

typedef enum {
    BANCO_ORDENAR_POR_NOME = 0,
    BANCO_ORDENAR_POR_CONTA
} BancoCriterioOrdenacao;

Banco *banco_criar(const char *arquivo_clientes, const char *arquivo_movimentos);
void banco_destruir(Banco *banco);
BancoResultado banco_carregar(Banco *banco);

BancoResultado banco_abrir_conta(Banco *banco, Cliente cliente);
BancoResultado banco_encerrar_conta(Banco *banco, const char *conta);
const Cliente *banco_consultar_por_conta(const Banco *banco, const char *conta);
const Cliente *banco_consultar_por_cpf(const Banco *banco, const char *cpf);
BancoResultado banco_atualizar_cliente(
    Banco *banco,
    const char *conta,
    const ClienteAtualizacao *atualizacao
);

BancoResultado banco_depositar(Banco *banco, const char *conta, Centavos valor);
BancoResultado banco_sacar(Banco *banco, const char *conta, Centavos valor);
BancoResultado banco_transferir(
    Banco *banco,
    const char *conta_origem,
    const char *conta_destino,
    Centavos valor
);
BancoResultado banco_pagar_cartao(Banco *banco, const char *conta, Centavos valor);

BancoResultado banco_obter_clientes_ativos(
    const Banco *banco,
    BancoCriterioOrdenacao criterio,
    Cliente **clientes,
    size_t *quantidade
);
void banco_liberar_lista(Cliente *clientes);

const char *banco_descrever_resultado(BancoResultado resultado);

#endif
