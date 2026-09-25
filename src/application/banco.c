#include "banco/banco.h"

#include "domain/ordenacao.h"
#include "infrastructure/persistencia.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Banco {
    Cliente *clientes;
    size_t quantidade;
    size_t capacidade;
    char *arquivo_clientes;
    char *arquivo_movimentos;
    bool operacional;
};

static char *duplicar_texto(const char *texto) {
    if (!texto) {
        return NULL;
    }

    const size_t tamanho = strlen(texto) + 1U;
    char *copia = malloc(tamanho);
    if (copia) {
        memcpy(copia, texto, tamanho);
    }
    return copia;
}

static void copiar_texto(char *destino, size_t capacidade, const char *origem) {
    if (capacidade == 0U) {
        return;
    }
    (void)snprintf(destino, capacidade, "%s", origem ? origem : "");
}

static bool texto_serializavel(const char *texto) {
    return texto && strpbrk(texto, ";\r\n") == NULL;
}

static bool texto_obrigatorio(const char *texto) {
    if (!texto_serializavel(texto)) {
        return false;
    }
    for (const unsigned char *p = (const unsigned char *)texto; *p; p++) {
        if (!isspace(*p)) {
            return true;
        }
    }
    return false;
}

static bool agencia_valida(const char *agencia) {
    if (!agencia || strlen(agencia) != 4U) {
        return false;
    }
    for (size_t i = 0; i < 4U; i++) {
        if (!isdigit((unsigned char)agencia[i])) {
            return false;
        }
    }
    return true;
}

static bool conta_valida(const char *conta) {
    if (!texto_obrigatorio(conta)) {
        return false;
    }

    bool possui_digito = false;
    for (const unsigned char *p = (const unsigned char *)conta; *p; p++) {
        if (isdigit(*p)) {
            possui_digito = true;
        } else if (*p != '-') {
            return false;
        }
    }
    return possui_digito;
}

static bool estado_valido(const char *estado) {
    return estado
        && strlen(estado) == 2U
        && isalpha((unsigned char)estado[0])
        && isalpha((unsigned char)estado[1]);
}

static bool data_valida(const char *data) {
    if (!data || strlen(data) != 10U || data[4] != '-' || data[7] != '-') {
        return false;
    }

    for (size_t i = 0; i < 10U; i++) {
        if (i != 4U && i != 7U && !isdigit((unsigned char)data[i])) {
            return false;
        }
    }

    const int ano = (data[0] - '0') * 1000 + (data[1] - '0') * 100
        + (data[2] - '0') * 10 + (data[3] - '0');
    const int mes = (data[5] - '0') * 10 + (data[6] - '0');
    const int dia = (data[8] - '0') * 10 + (data[9] - '0');
    if (ano < 1900 || mes < 1 || mes > 12 || dia < 1) {
        return false;
    }

    static const int dias_por_mes[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    int limite = dias_por_mes[mes - 1];
    const bool bissexto = (ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0;
    if (mes == 2 && bissexto) {
        limite = 29;
    }
    return dia <= limite;
}

static bool cpf_normalizar(const char *cpf, char normalizado[12]) {
    if (!cpf || !normalizado) {
        return false;
    }

    const size_t tamanho = strlen(cpf);
    int digitos[11];
    size_t quantidade = 0U;
    if (tamanho == 11U) {
        for (size_t i = 0; i < tamanho; i++) {
            if (!isdigit((unsigned char)cpf[i])) {
                return false;
            }
            digitos[quantidade] = cpf[i] - '0';
            normalizado[quantidade++] = cpf[i];
        }
    } else if (tamanho == 14U) {
        for (size_t i = 0; i < tamanho; i++) {
            if (i == 3U || i == 7U) {
                if (cpf[i] != '.') {
                    return false;
                }
            } else if (i == 11U) {
                if (cpf[i] != '-') {
                    return false;
                }
            } else {
                if (!isdigit((unsigned char)cpf[i]) || quantidade >= 11U) {
                    return false;
                }
                digitos[quantidade] = cpf[i] - '0';
                normalizado[quantidade++] = cpf[i];
            }
        }
    } else {
        return false;
    }
    normalizado[11] = '\0';

    bool todos_iguais = true;
    for (size_t i = 1; i < 11U; i++) {
        if (digitos[i] != digitos[0]) {
            todos_iguais = false;
            break;
        }
    }
    if (todos_iguais) {
        return false;
    }

    int soma = 0;
    for (int i = 0; i < 9; i++) {
        soma += digitos[i] * (10 - i);
    }
    int verificador = 11 - soma % 11;
    if (verificador >= 10) {
        verificador = 0;
    }
    if (digitos[9] != verificador) {
        return false;
    }

    soma = 0;
    for (int i = 0; i < 10; i++) {
        soma += digitos[i] * (11 - i);
    }
    verificador = 11 - soma % 11;
    if (verificador >= 10) {
        verificador = 0;
    }
    return digitos[10] == verificador;
}

static bool cliente_valido(const Cliente *cliente) {
    char cpf_normalizado[12];
    if (!cliente
        || !agencia_valida(cliente->agencia)
        || !conta_valida(cliente->conta)
        || !texto_obrigatorio(cliente->nome)
        || !cpf_normalizar(cliente->cpf, cpf_normalizado)
        || !data_valida(cliente->data_nascimento)
        || !texto_obrigatorio(cliente->endereco)
        || !texto_obrigatorio(cliente->cidade)
        || !estado_valido(cliente->estado)
        || cliente->saldo_centavos < 0
        || (!cliente->ativo && cliente->saldo_centavos != 0)) {
        return false;
    }

    const char *campos[] = {
        cliente->telefone,
        cliente->cep,
        cliente->local,
        cliente->numero,
        cliente->bairro
    };
    for (size_t i = 0; i < sizeof campos / sizeof campos[0]; i++) {
        if (!texto_serializavel(campos[i])) {
            return false;
        }
    }
    return true;
}

static BancoResultado garantir_capacidade(Banco *banco) {
    if (banco->quantidade < banco->capacidade) {
        return BANCO_OK;
    }

    const size_t nova_capacidade = banco->capacidade == 0U
        ? 4U
        : banco->capacidade * 2U;
    if (nova_capacidade < banco->capacidade
        || nova_capacidade > SIZE_MAX / sizeof *banco->clientes) {
        return BANCO_ERRO_MEMORIA;
    }

    Cliente *novos_clientes = realloc(
        banco->clientes,
        nova_capacidade * sizeof *novos_clientes
    );
    if (!novos_clientes) {
        return BANCO_ERRO_MEMORIA;
    }

    banco->clientes = novos_clientes;
    banco->capacidade = nova_capacidade;
    return BANCO_OK;
}

static Cliente *buscar_conta(Banco *banco, const char *conta, bool apenas_ativa) {
    if (!banco || !conta) {
        return NULL;
    }
    for (size_t i = 0; i < banco->quantidade; i++) {
        Cliente *cliente = &banco->clientes[i];
        if ((!apenas_ativa || cliente->ativo) && strcmp(cliente->conta, conta) == 0) {
            return cliente;
        }
    }
    return NULL;
}

static BancoResultado salvar(Banco *banco) {
    if (!banco || !banco->operacional) {
        return BANCO_ERRO_PERSISTENCIA;
    }
    return persistencia_salvar_clientes(
        banco->arquivo_clientes,
        banco->clientes,
        banco->quantidade
    ) == PERSISTENCIA_OK
        ? BANCO_OK
        : BANCO_ERRO_PERSISTENCIA;
}

static BancoResultado confirmar_saldos(
    Banco *banco,
    Cliente *primeiro,
    Centavos saldo_anterior_primeiro,
    Cliente *segundo,
    Centavos saldo_anterior_segundo,
    const Movimento *movimentos,
    size_t quantidade_movimentos
) {
    if (!banco || !banco->operacional) {
        primeiro->saldo_centavos = saldo_anterior_primeiro;
        if (segundo) {
            segundo->saldo_centavos = saldo_anterior_segundo;
        }
        return BANCO_ERRO_PERSISTENCIA;
    }

    const PersistenciaResultado resultado = persistencia_confirmar_transacao(
        banco->arquivo_clientes,
        banco->clientes,
        banco->quantidade,
        banco->arquivo_movimentos,
        movimentos,
        quantidade_movimentos
    );
    if (resultado == PERSISTENCIA_OK) {
        return BANCO_OK;
    }

    if (resultado == PERSISTENCIA_TRANSACAO_PENDENTE) {
        banco->operacional = false;
        return BANCO_TRANSACAO_PENDENTE;
    }

    primeiro->saldo_centavos = saldo_anterior_primeiro;
    if (segundo) {
        segundo->saldo_centavos = saldo_anterior_segundo;
    }
    return BANCO_ERRO_PERSISTENCIA;
}

Banco *banco_criar(const char *arquivo_clientes, const char *arquivo_movimentos) {
    if (!arquivo_clientes || !arquivo_movimentos) {
        return NULL;
    }

    Banco *banco = calloc(1U, sizeof *banco);
    if (!banco) {
        return NULL;
    }

    banco->arquivo_clientes = duplicar_texto(arquivo_clientes);
    banco->arquivo_movimentos = duplicar_texto(arquivo_movimentos);
    if (!banco->arquivo_clientes || !banco->arquivo_movimentos) {
        banco_destruir(banco);
        return NULL;
    }
    banco->operacional = true;
    return banco;
}

void banco_destruir(Banco *banco) {
    if (!banco) {
        return;
    }
    free(banco->clientes);
    free(banco->arquivo_clientes);
    free(banco->arquivo_movimentos);
    free(banco);
}

BancoResultado banco_carregar(Banco *banco) {
    if (!banco) {
        return BANCO_ERRO_ARGUMENTO;
    }

    PersistenciaResultado resultado = persistencia_recuperar_transacao(
        banco->arquivo_clientes,
        banco->arquivo_movimentos
    );
    if (resultado != PERSISTENCIA_OK) {
        banco->operacional = false;
        return resultado == PERSISTENCIA_ERRO_MEMORIA
            ? BANCO_ERRO_MEMORIA
            : BANCO_ERRO_PERSISTENCIA;
    }

    resultado = persistencia_carregar_clientes(
        banco->arquivo_clientes,
        &banco->clientes,
        &banco->quantidade,
        &banco->capacidade
    );
    if (resultado == PERSISTENCIA_ERRO_MEMORIA) {
        return BANCO_ERRO_MEMORIA;
    }
    if (resultado != PERSISTENCIA_OK) {
        return resultado == PERSISTENCIA_ERRO_FORMATO
            ? BANCO_ERRO_DADOS_INVALIDOS
            : BANCO_ERRO_PERSISTENCIA;
    }

    for (size_t i = 0; i < banco->quantidade; i++) {
        char cpf[12];
        if (!cpf_normalizar(banco->clientes[i].cpf, cpf)) {
            return BANCO_ERRO_DADOS_INVALIDOS;
        }
        copiar_texto(banco->clientes[i].cpf, sizeof banco->clientes[i].cpf, cpf);
        if (!cliente_valido(&banco->clientes[i])) {
            return BANCO_ERRO_DADOS_INVALIDOS;
        }
        for (size_t j = 0; j < i; j++) {
            if (strcmp(banco->clientes[i].conta, banco->clientes[j].conta) == 0
                || strcmp(banco->clientes[i].cpf, banco->clientes[j].cpf) == 0) {
                return BANCO_ERRO_DADOS_INVALIDOS;
            }
        }
    }
    return BANCO_OK;
}

BancoResultado banco_abrir_conta(Banco *banco, Cliente cliente) {
    if (!banco) {
        return BANCO_ERRO_ARGUMENTO;
    }
    if (!banco->operacional) {
        return BANCO_ERRO_PERSISTENCIA;
    }

    char cpf[12];
    if (!cpf_normalizar(cliente.cpf, cpf)) {
        return BANCO_ERRO_DADOS_INVALIDOS;
    }
    copiar_texto(cliente.cpf, sizeof cliente.cpf, cpf);
    cliente.saldo_centavos = 0;
    cliente.ativo = true;
    if (!cliente_valido(&cliente)) {
        return BANCO_ERRO_DADOS_INVALIDOS;
    }

    for (size_t i = 0; i < banco->quantidade; i++) {
        if (strcmp(banco->clientes[i].conta, cliente.conta) == 0) {
            return BANCO_ERRO_CONTA_EXISTENTE;
        }
        if (strcmp(banco->clientes[i].cpf, cliente.cpf) == 0) {
            return BANCO_ERRO_CPF_EXISTENTE;
        }
    }

    BancoResultado resultado = garantir_capacidade(banco);
    if (resultado != BANCO_OK) {
        return resultado;
    }

    banco->clientes[banco->quantidade++] = cliente;
    resultado = salvar(banco);
    if (resultado != BANCO_OK) {
        banco->quantidade--;
    }
    return resultado;
}

BancoResultado banco_encerrar_conta(Banco *banco, const char *conta) {
    Cliente *cliente = buscar_conta(banco, conta, true);
    if (!cliente) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }
    if (cliente->saldo_centavos != 0) {
        return BANCO_ERRO_SALDO_NAO_ZERADO;
    }

    cliente->ativo = false;
    const BancoResultado resultado = salvar(banco);
    if (resultado != BANCO_OK) {
        cliente->ativo = true;
    }
    return resultado;
}

const Cliente *banco_consultar_por_conta(const Banco *banco, const char *conta) {
    return buscar_conta((Banco *)banco, conta, true);
}

const Cliente *banco_consultar_por_cpf(const Banco *banco, const char *cpf) {
    char normalizado[12];
    if (!banco || !cpf_normalizar(cpf, normalizado)) {
        return NULL;
    }
    for (size_t i = 0; i < banco->quantidade; i++) {
        if (banco->clientes[i].ativo
            && strcmp(banco->clientes[i].cpf, normalizado) == 0) {
            return &banco->clientes[i];
        }
    }
    return NULL;
}

BancoResultado banco_atualizar_cliente(
    Banco *banco,
    const char *conta,
    const ClienteAtualizacao *atualizacao
) {
    if (!atualizacao
        || !texto_obrigatorio(atualizacao->nome)
        || !texto_serializavel(atualizacao->telefone)
        || !texto_obrigatorio(atualizacao->endereco)) {
        return BANCO_ERRO_DADOS_INVALIDOS;
    }

    Cliente *cliente = buscar_conta(banco, conta, true);
    if (!cliente) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }

    const Cliente anterior = *cliente;
    copiar_texto(cliente->nome, sizeof cliente->nome, atualizacao->nome);
    copiar_texto(cliente->telefone, sizeof cliente->telefone, atualizacao->telefone);
    copiar_texto(cliente->endereco, sizeof cliente->endereco, atualizacao->endereco);

    const BancoResultado resultado = salvar(banco);
    if (resultado != BANCO_OK) {
        *cliente = anterior;
    }
    return resultado;
}

BancoResultado banco_depositar(Banco *banco, const char *conta, Centavos valor) {
    if (valor <= 0) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }

    Cliente *cliente = buscar_conta(banco, conta, true);
    if (!cliente) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }
    if (cliente->saldo_centavos > INT64_MAX - valor) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }

    const Centavos saldo_anterior = cliente->saldo_centavos;
    cliente->saldo_centavos += valor;
    const Movimento movimento = {
        cliente->conta,
        "DEPOSITO",
        valor,
        cliente->saldo_centavos
    };
    return confirmar_saldos(banco, cliente, saldo_anterior, NULL, 0, &movimento, 1U);
}

BancoResultado banco_sacar(Banco *banco, const char *conta, Centavos valor) {
    if (valor <= 0) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }

    Cliente *cliente = buscar_conta(banco, conta, true);
    if (!cliente) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }
    if (cliente->saldo_centavos < valor) {
        return BANCO_ERRO_SALDO_INSUFICIENTE;
    }

    const Centavos saldo_anterior = cliente->saldo_centavos;
    cliente->saldo_centavos -= valor;
    const Movimento movimento = {
        cliente->conta,
        "SAQUE",
        -valor,
        cliente->saldo_centavos
    };
    return confirmar_saldos(banco, cliente, saldo_anterior, NULL, 0, &movimento, 1U);
}

BancoResultado banco_transferir(
    Banco *banco,
    const char *conta_origem,
    const char *conta_destino,
    Centavos valor
) {
    if (valor <= 0) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }
    if (!conta_origem || !conta_destino || strcmp(conta_origem, conta_destino) == 0) {
        return BANCO_ERRO_MESMA_CONTA;
    }

    Cliente *origem = buscar_conta(banco, conta_origem, true);
    Cliente *destino = buscar_conta(banco, conta_destino, true);
    if (!origem || !destino) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }
    if (origem->saldo_centavos < valor) {
        return BANCO_ERRO_SALDO_INSUFICIENTE;
    }
    if (destino->saldo_centavos > INT64_MAX - valor) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }

    const Centavos saldo_origem = origem->saldo_centavos;
    const Centavos saldo_destino = destino->saldo_centavos;
    origem->saldo_centavos -= valor;
    destino->saldo_centavos += valor;

    const Movimento movimentos[] = {
        {origem->conta, "PIX_ENVIADO", -valor, origem->saldo_centavos},
        {destino->conta, "PIX_RECEBIDO", valor, destino->saldo_centavos}
    };
    return confirmar_saldos(
        banco,
        origem,
        saldo_origem,
        destino,
        saldo_destino,
        movimentos,
        sizeof movimentos / sizeof movimentos[0]
    );
}

BancoResultado banco_pagar_cartao(Banco *banco, const char *conta, Centavos valor) {
    if (valor <= 0) {
        return BANCO_ERRO_VALOR_INVALIDO;
    }

    Cliente *cliente = buscar_conta(banco, conta, true);
    if (!cliente) {
        return BANCO_ERRO_CONTA_NAO_ENCONTRADA;
    }
    if (cliente->saldo_centavos < valor) {
        return BANCO_ERRO_SALDO_INSUFICIENTE;
    }

    const Centavos saldo_anterior = cliente->saldo_centavos;
    cliente->saldo_centavos -= valor;
    const Movimento movimento = {
        cliente->conta,
        "CARTAO",
        -valor,
        cliente->saldo_centavos
    };
    return confirmar_saldos(banco, cliente, saldo_anterior, NULL, 0, &movimento, 1U);
}

BancoResultado banco_obter_clientes_ativos(
    const Banco *banco,
    BancoCriterioOrdenacao criterio,
    Cliente **clientes,
    size_t *quantidade
) {
    if (!banco || !clientes || !quantidade) {
        return BANCO_ERRO_ARGUMENTO;
    }

    *clientes = NULL;
    *quantidade = 0U;
    size_t ativos = 0U;
    for (size_t i = 0; i < banco->quantidade; i++) {
        if (banco->clientes[i].ativo) {
            ativos++;
        }
    }
    if (ativos == 0U) {
        return BANCO_OK;
    }

    Cliente *copia = malloc(ativos * sizeof *copia);
    if (!copia) {
        return BANCO_ERRO_MEMORIA;
    }

    size_t indice = 0U;
    for (size_t i = 0; i < banco->quantidade; i++) {
        if (banco->clientes[i].ativo) {
            copia[indice++] = banco->clientes[i];
        }
    }

    ordenar_clientes(
        copia,
        ativos,
        criterio == BANCO_ORDENAR_POR_NOME
            ? ORDENAR_CLIENTE_POR_NOME
            : ORDENAR_CLIENTE_POR_CONTA
    );
    *clientes = copia;
    *quantidade = ativos;
    return BANCO_OK;
}

void banco_liberar_lista(Cliente *clientes) {
    free(clientes);
}

const char *banco_descrever_resultado(BancoResultado resultado) {
    switch (resultado) {
        case BANCO_OK:
            return "operacao concluida";
        case BANCO_ERRO_ARGUMENTO:
            return "argumento invalido";
        case BANCO_ERRO_MEMORIA:
            return "memoria insuficiente";
        case BANCO_ERRO_PERSISTENCIA:
            return "falha ao persistir os dados";
        case BANCO_TRANSACAO_PENDENTE:
            return "transacao pendente de recuperacao";
        case BANCO_ERRO_DADOS_INVALIDOS:
            return "dados invalidos";
        case BANCO_ERRO_CONTA_EXISTENTE:
            return "numero de conta ja utilizado";
        case BANCO_ERRO_CPF_EXISTENTE:
            return "CPF ja cadastrado";
        case BANCO_ERRO_CONTA_NAO_ENCONTRADA:
            return "conta ativa nao encontrada";
        case BANCO_ERRO_VALOR_INVALIDO:
            return "valor invalido";
        case BANCO_ERRO_SALDO_INSUFICIENTE:
            return "saldo insuficiente";
        case BANCO_ERRO_SALDO_NAO_ZERADO:
            return "a conta ainda possui saldo";
        case BANCO_ERRO_MESMA_CONTA:
            return "as contas de origem e destino devem ser diferentes";
        default:
            return "erro desconhecido";
    }
}
