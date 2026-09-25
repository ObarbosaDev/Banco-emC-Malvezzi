#include "infrastructure/persistencia.h"

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define QUANTIDADE_CAMPOS_CLIENTE 15U
#define TAMANHO_LINHA 2048U
#define CABECALHO_CLIENTES \
    "agencia;conta;nome;cpf;data_nascimento;telefone;endereco;cep;local;numero;bairro;cidade;estado;saldo_centavos;ativo\n"
#define CABECALHO_MOVIMENTOS \
    "data_hora;conta;tipo;valor_centavos;saldo_centavos\n"

static bool copiar_campo(char *destino, size_t capacidade, const char *origem) {
    const size_t tamanho = strlen(origem);
    if (tamanho >= capacidade) {
        return false;
    }
    memcpy(destino, origem, tamanho + 1U);
    return true;
}

static size_t separar_campos(char *linha, char **campos, size_t limite) {
    if (!linha || !campos || limite == 0U) {
        return 0U;
    }

    size_t quantidade = 1U;
    campos[0] = linha;
    for (char *cursor = linha; *cursor; cursor++) {
        if (*cursor == ';') {
            *cursor = '\0';
            if (quantidade >= limite) {
                return limite + 1U;
            }
            campos[quantidade++] = cursor + 1;
        }
    }
    return quantidade;
}

static bool converter_centavos(const char *texto, Centavos *valor) {
    if (!texto || !valor || texto[0] == '\0') {
        return false;
    }

    errno = 0;
    char *fim = NULL;
    const long long convertido = strtoll(texto, &fim, 10);
    if (errno == ERANGE || !fim || *fim != '\0') {
        return false;
    }
    *valor = (Centavos)convertido;
    return true;
}

static bool converter_ativo(const char *texto, bool *ativo) {
    if (!texto || !ativo || (strcmp(texto, "0") != 0 && strcmp(texto, "1") != 0)) {
        return false;
    }
    *ativo = texto[0] == '1';
    return true;
}

static bool converter_cliente(char *linha, Cliente *cliente) {
    char *campos[QUANTIDADE_CAMPOS_CLIENTE];
    if (separar_campos(linha, campos, QUANTIDADE_CAMPOS_CLIENTE)
        != QUANTIDADE_CAMPOS_CLIENTE) {
        return false;
    }

    return copiar_campo(cliente->agencia, sizeof cliente->agencia, campos[0])
        && copiar_campo(cliente->conta, sizeof cliente->conta, campos[1])
        && copiar_campo(cliente->nome, sizeof cliente->nome, campos[2])
        && copiar_campo(cliente->cpf, sizeof cliente->cpf, campos[3])
        && copiar_campo(
            cliente->data_nascimento,
            sizeof cliente->data_nascimento,
            campos[4]
        )
        && copiar_campo(cliente->telefone, sizeof cliente->telefone, campos[5])
        && copiar_campo(cliente->endereco, sizeof cliente->endereco, campos[6])
        && copiar_campo(cliente->cep, sizeof cliente->cep, campos[7])
        && copiar_campo(cliente->local, sizeof cliente->local, campos[8])
        && copiar_campo(cliente->numero, sizeof cliente->numero, campos[9])
        && copiar_campo(cliente->bairro, sizeof cliente->bairro, campos[10])
        && copiar_campo(cliente->cidade, sizeof cliente->cidade, campos[11])
        && copiar_campo(cliente->estado, sizeof cliente->estado, campos[12])
        && converter_centavos(campos[13], &cliente->saldo_centavos)
        && converter_ativo(campos[14], &cliente->ativo);
}

static char *criar_caminho(const char *base, const char *sufixo) {
    if (!base || !sufixo) {
        return NULL;
    }
    const size_t tamanho_base = strlen(base);
    const size_t tamanho_sufixo = strlen(sufixo);
    if (tamanho_base > SIZE_MAX - tamanho_sufixo - 1U) {
        return NULL;
    }

    const size_t capacidade = tamanho_base + tamanho_sufixo + 1U;
    char *caminho = malloc(capacidade);
    if (caminho) {
        (void)snprintf(caminho, capacidade, "%s%s", base, sufixo);
    }
    return caminho;
}

static bool substituir_arquivo(const char *temporario, const char *destino) {
#ifdef _WIN32
    return MoveFileExA(
        temporario,
        destino,
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH
    ) != 0;
#else
    return rename(temporario, destino) == 0;
#endif
}

static PersistenciaResultado verificar_arquivo(const char *caminho, bool *existe) {
    errno = 0;
    FILE *fluxo = fopen(caminho, "rb");
    if (!fluxo) {
        if (errno == ENOENT) {
            *existe = false;
            return PERSISTENCIA_OK;
        }
        return PERSISTENCIA_ERRO_IO;
    }

    *existe = true;
    return fclose(fluxo) == 0 ? PERSISTENCIA_OK : PERSISTENCIA_ERRO_IO;
}

static PersistenciaResultado escrever_clientes(
    const char *caminho,
    const Cliente *clientes,
    size_t quantidade
) {
    FILE *fluxo = fopen(caminho, "w");
    if (!fluxo) {
        return PERSISTENCIA_ERRO_IO;
    }

    bool sucesso = fputs(CABECALHO_CLIENTES, fluxo) != EOF;
    for (size_t i = 0; i < quantidade && sucesso; i++) {
        const Cliente *cliente = &clientes[i];
        sucesso = fprintf(
            fluxo,
            "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%" PRId64 ";%d\n",
            cliente->agencia,
            cliente->conta,
            cliente->nome,
            cliente->cpf,
            cliente->data_nascimento,
            cliente->telefone,
            cliente->endereco,
            cliente->cep,
            cliente->local,
            cliente->numero,
            cliente->bairro,
            cliente->cidade,
            cliente->estado,
            cliente->saldo_centavos,
            cliente->ativo ? 1 : 0
        ) >= 0;
    }

    if (sucesso && fflush(fluxo) != 0) {
        sucesso = false;
    }
    if (fclose(fluxo) != 0) {
        sucesso = false;
    }
    if (!sucesso) {
        (void)remove(caminho);
        return PERSISTENCIA_ERRO_IO;
    }
    return PERSISTENCIA_OK;
}

static PersistenciaResultado escrever_movimentos(
    const char *destino,
    const char *origem,
    const Movimento *movimentos,
    size_t quantidade
) {
    FILE *saida = fopen(destino, "wb");
    if (!saida) {
        return PERSISTENCIA_ERRO_IO;
    }

    errno = 0;
    FILE *entrada = fopen(origem, "rb");
    bool sucesso = true;
    size_t bytes_copiados = 0U;
    int ultimo = '\n';
    if (entrada) {
        unsigned char buffer[4096];
        size_t lidos = 0U;
        while ((lidos = fread(buffer, 1U, sizeof buffer, entrada)) > 0U) {
            if (fwrite(buffer, 1U, lidos, saida) != lidos) {
                sucesso = false;
                break;
            }
            bytes_copiados += lidos;
            ultimo = buffer[lidos - 1U];
        }
        const bool erro_leitura = ferror(entrada) != 0;
        const bool erro_fechamento = fclose(entrada) != 0;
        if (erro_leitura || erro_fechamento) {
            sucesso = false;
        }
    } else if (errno != ENOENT) {
        sucesso = false;
    }

    if (sucesso && bytes_copiados == 0U) {
        sucesso = fputs(CABECALHO_MOVIMENTOS, saida) != EOF;
    } else if (sucesso && ultimo != '\n') {
        sucesso = fputc('\n', saida) != EOF;
    }

    const time_t agora = time(NULL);
    const struct tm *data_local = localtime(&agora);
    char data_hora[20] = {0};
    if (!data_local
        || strftime(data_hora, sizeof data_hora, "%Y-%m-%dT%H:%M:%S", data_local) == 0U) {
        sucesso = false;
    }

    for (size_t i = 0; i < quantidade && sucesso; i++) {
        sucesso = fprintf(
            saida,
            "%s;%s;%s;%" PRId64 ";%" PRId64 "\n",
            data_hora,
            movimentos[i].conta,
            movimentos[i].tipo,
            movimentos[i].valor_centavos,
            movimentos[i].saldo_centavos
        ) >= 0;
    }

    if (sucesso && fflush(saida) != 0) {
        sucesso = false;
    }
    if (fclose(saida) != 0) {
        sucesso = false;
    }
    if (!sucesso) {
        (void)remove(destino);
        return PERSISTENCIA_ERRO_IO;
    }
    return PERSISTENCIA_OK;
}

static PersistenciaResultado aplicar_temporario(
    const char *temporario,
    const char *destino
) {
    bool existe = false;
    const PersistenciaResultado verificacao = verificar_arquivo(temporario, &existe);
    if (verificacao != PERSISTENCIA_OK || !existe) {
        return verificacao;
    }
    return substituir_arquivo(temporario, destino)
        ? PERSISTENCIA_OK
        : PERSISTENCIA_TRANSACAO_PENDENTE;
}

PersistenciaResultado persistencia_recuperar_transacao(
    const char *arquivo_clientes,
    const char *arquivo_movimentos
) {
    char *clientes_transacao = criar_caminho(arquivo_clientes, ".txn");
    char *movimentos_transacao = criar_caminho(arquivo_movimentos, ".txn");
    char *marcador = criar_caminho(arquivo_clientes, ".journal");
    if (!clientes_transacao || !movimentos_transacao || !marcador) {
        free(clientes_transacao);
        free(movimentos_transacao);
        free(marcador);
        return PERSISTENCIA_ERRO_MEMORIA;
    }

    bool existe_marcador = false;
    PersistenciaResultado resultado = verificar_arquivo(marcador, &existe_marcador);
    if (resultado != PERSISTENCIA_OK) {
        goto finalizar;
    }
    if (!existe_marcador) {
        (void)remove(clientes_transacao);
        (void)remove(movimentos_transacao);
        resultado = PERSISTENCIA_OK;
        goto finalizar;
    }

    resultado = aplicar_temporario(movimentos_transacao, arquivo_movimentos);
    if (resultado == PERSISTENCIA_OK) {
        resultado = aplicar_temporario(clientes_transacao, arquivo_clientes);
    }
    if (resultado == PERSISTENCIA_OK && remove(marcador) != 0) {
        resultado = PERSISTENCIA_TRANSACAO_PENDENTE;
    }

finalizar:
    free(clientes_transacao);
    free(movimentos_transacao);
    free(marcador);
    return resultado;
}

PersistenciaResultado persistencia_carregar_clientes(
    const char *arquivo,
    Cliente **clientes,
    size_t *quantidade,
    size_t *capacidade
) {
    if (!arquivo || !clientes || !quantidade || !capacidade) {
        return PERSISTENCIA_ERRO_FORMATO;
    }

    errno = 0;
    FILE *fluxo = fopen(arquivo, "r");
    if (!fluxo) {
        return errno == ENOENT ? PERSISTENCIA_OK : PERSISTENCIA_ERRO_IO;
    }

    Cliente *carregados = NULL;
    size_t total = 0U;
    size_t limite = 0U;
    size_t numero_linha = 0U;
    char linha[TAMANHO_LINHA];
    PersistenciaResultado resultado = PERSISTENCIA_OK;

    while (fgets(linha, (int)sizeof linha, fluxo)) {
        numero_linha++;
        const bool possui_quebra = strchr(linha, '\n') != NULL;
        if (!possui_quebra && !feof(fluxo)) {
            resultado = PERSISTENCIA_ERRO_FORMATO;
            break;
        }

        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') {
            continue;
        }
        if (numero_linha == 1U
            && strcmp(linha, CABECALHO_CLIENTES) == 0) {
            continue;
        }
        if (numero_linha == 1U
            && strcmp(
                linha,
                "agencia;conta;nome;cpf;data_nascimento;telefone;endereco;cep;local;numero;bairro;cidade;estado;saldo_centavos;ativo"
            ) == 0) {
            continue;
        }

        Cliente cliente = {0};
        if (!converter_cliente(linha, &cliente)) {
            fprintf(stderr, "Formato invalido em %s, linha %zu.\n", arquivo, numero_linha);
            resultado = PERSISTENCIA_ERRO_FORMATO;
            break;
        }

        if (total == limite) {
            const size_t novo_limite = limite == 0U ? 4U : limite * 2U;
            if (novo_limite < limite || novo_limite > SIZE_MAX / sizeof *carregados) {
                resultado = PERSISTENCIA_ERRO_MEMORIA;
                break;
            }
            Cliente *temporario = realloc(carregados, novo_limite * sizeof *temporario);
            if (!temporario) {
                resultado = PERSISTENCIA_ERRO_MEMORIA;
                break;
            }
            carregados = temporario;
            limite = novo_limite;
        }
        carregados[total++] = cliente;
    }

    if (ferror(fluxo)) {
        resultado = PERSISTENCIA_ERRO_IO;
    }
    if (fclose(fluxo) != 0 && resultado == PERSISTENCIA_OK) {
        resultado = PERSISTENCIA_ERRO_IO;
    }
    if (resultado != PERSISTENCIA_OK) {
        free(carregados);
        return resultado;
    }

    free(*clientes);
    *clientes = carregados;
    *quantidade = total;
    *capacidade = limite;
    return PERSISTENCIA_OK;
}

PersistenciaResultado persistencia_salvar_clientes(
    const char *arquivo,
    const Cliente *clientes,
    size_t quantidade
) {
    if (!arquivo || (quantidade > 0U && !clientes)) {
        return PERSISTENCIA_ERRO_FORMATO;
    }

    char *temporario = criar_caminho(arquivo, ".tmp");
    if (!temporario) {
        return PERSISTENCIA_ERRO_MEMORIA;
    }

    PersistenciaResultado resultado = escrever_clientes(temporario, clientes, quantidade);
    if (resultado == PERSISTENCIA_OK && !substituir_arquivo(temporario, arquivo)) {
        resultado = PERSISTENCIA_ERRO_IO;
    }
    if (resultado != PERSISTENCIA_OK) {
        (void)remove(temporario);
    }
    free(temporario);
    return resultado;
}

PersistenciaResultado persistencia_confirmar_transacao(
    const char *arquivo_clientes,
    const Cliente *clientes,
    size_t quantidade_clientes,
    const char *arquivo_movimentos,
    const Movimento *movimentos,
    size_t quantidade_movimentos
) {
    if (!arquivo_clientes
        || !arquivo_movimentos
        || (quantidade_clientes > 0U && !clientes)
        || (quantidade_movimentos > 0U && !movimentos)) {
        return PERSISTENCIA_ERRO_FORMATO;
    }

    PersistenciaResultado resultado = persistencia_recuperar_transacao(
        arquivo_clientes,
        arquivo_movimentos
    );
    if (resultado != PERSISTENCIA_OK) {
        return resultado;
    }

    char *clientes_transacao = criar_caminho(arquivo_clientes, ".txn");
    char *movimentos_transacao = criar_caminho(arquivo_movimentos, ".txn");
    char *marcador = criar_caminho(arquivo_clientes, ".journal");
    char *marcador_temporario = marcador ? criar_caminho(marcador, ".tmp") : NULL;
    if (!clientes_transacao || !movimentos_transacao || !marcador || !marcador_temporario) {
        resultado = PERSISTENCIA_ERRO_MEMORIA;
        goto finalizar;
    }

    resultado = escrever_clientes(clientes_transacao, clientes, quantidade_clientes);
    if (resultado != PERSISTENCIA_OK) {
        goto limpar_preparacao;
    }
    resultado = escrever_movimentos(
        movimentos_transacao,
        arquivo_movimentos,
        movimentos,
        quantidade_movimentos
    );
    if (resultado != PERSISTENCIA_OK) {
        goto limpar_preparacao;
    }

    FILE *fluxo_marcador = fopen(marcador_temporario, "w");
    if (!fluxo_marcador) {
        resultado = PERSISTENCIA_ERRO_IO;
        goto limpar_preparacao;
    }
    bool marcador_valido = fputs("BANCO_TXN_V1\n", fluxo_marcador) != EOF;
    if (marcador_valido && fflush(fluxo_marcador) != 0) {
        marcador_valido = false;
    }
    if (fclose(fluxo_marcador) != 0) {
        marcador_valido = false;
    }
    if (!marcador_valido || !substituir_arquivo(marcador_temporario, marcador)) {
        resultado = PERSISTENCIA_ERRO_IO;
        goto limpar_preparacao;
    }

    resultado = persistencia_recuperar_transacao(arquivo_clientes, arquivo_movimentos);
    if (resultado != PERSISTENCIA_OK) {
        resultado = PERSISTENCIA_TRANSACAO_PENDENTE;
    }
    goto finalizar;

limpar_preparacao:
    (void)remove(clientes_transacao);
    (void)remove(movimentos_transacao);
    (void)remove(marcador_temporario);

finalizar:
    free(clientes_transacao);
    free(movimentos_transacao);
    free(marcador);
    free(marcador_temporario);
    return resultado;
}
