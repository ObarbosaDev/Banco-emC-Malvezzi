#include "banco/banco.h"
#include "presentation/io.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ARQUIVO_CLIENTES "data/clientes.csv"
#define ARQUIVO_MOVIMENTOS "data/movimentos.csv"

static void copiar_texto(char *destino, size_t capacidade, const char *origem) {
    (void)snprintf(destino, capacidade, "%s", origem ? origem : "");
}

static bool solicitar_texto(
    const char *rotulo,
    char *destino,
    size_t capacidade,
    bool obrigatorio
) {
    for (;;) {
        printf("%s", rotulo);
        const IoResultado resultado = io_ler_linha(destino, capacidade);
        if (resultado == IO_FIM_ENTRADA) {
            return false;
        }
        if (resultado == IO_ENTRADA_LONGA) {
            printf("Entrada muito longa. Tente novamente.\n");
            continue;
        }
        if (resultado != IO_SUCESSO || (obrigatorio && destino[0] == '\0')) {
            printf("Informe um valor valido.\n");
            continue;
        }
        return true;
    }
}

static bool solicitar_valor(const char *rotulo, Centavos *valor) {
    for (;;) {
        printf("%s", rotulo);
        const IoResultado resultado = io_ler_valor(valor);
        if (resultado == IO_FIM_ENTRADA) {
            return false;
        }
        if (resultado != IO_SUCESSO || *valor <= 0) {
            printf("Informe um valor positivo com no maximo duas casas decimais.\n");
            continue;
        }
        return true;
    }
}

static bool solicitar_opcao(int *opcao) {
    for (;;) {
        printf("Opcao: ");
        const IoResultado resultado = io_ler_inteiro(opcao);
        if (resultado == IO_FIM_ENTRADA) {
            return false;
        }
        if (resultado != IO_SUCESSO) {
            printf("Opcao invalida.\n");
            continue;
        }
        return true;
    }
}

static void exibir_menu(void) {
    printf("\n========== BANCO MALVEZZI ==========\n");
    printf("1. Abrir conta\n");
    printf("2. Encerrar conta\n");
    printf("3. Consultar cliente\n");
    printf("4. Alterar cliente\n");
    printf("5. Depositar\n");
    printf("6. Sacar\n");
    printf("7. Consultar saldo\n");
    printf("8. Listar clientes por nome\n");
    printf("9. Listar clientes por conta\n");
    printf("10. Transferir entre contas\n");
    printf("11. Pagar compra no cartao\n");
    printf("0. Sair\n");
    printf("=====================================\n");
}

static bool informar_resultado(BancoResultado resultado, const char *sucesso) {
    if (resultado == BANCO_OK) {
        printf("%s\n", sucesso);
        return true;
    }
    if (resultado == BANCO_TRANSACAO_PENDENTE) {
        printf(
            "A transacao ficou pendente de confirmacao. "
            "Reinicie o programa para concluir a recuperacao.\n"
        );
        return false;
    }

    printf("Operacao nao realizada: %s.\n", banco_descrever_resultado(resultado));
    return true;
}

static bool executar_abertura(Banco *banco) {
    Cliente cliente = {0};
    printf("\n--- ABERTURA DE CONTA ---\n");

    if (!solicitar_texto("Agencia (4 digitos): ", cliente.agencia, sizeof cliente.agencia, true)
        || !solicitar_texto("Numero da conta: ", cliente.conta, sizeof cliente.conta, true)
        || !solicitar_texto("Nome completo: ", cliente.nome, sizeof cliente.nome, true)
        || !solicitar_texto("CPF: ", cliente.cpf, sizeof cliente.cpf, true)
        || !solicitar_texto(
            "Data de nascimento (AAAA-MM-DD): ",
            cliente.data_nascimento,
            sizeof cliente.data_nascimento,
            true
        )
        || !solicitar_texto(
            "Telefone (opcional): ",
            cliente.telefone,
            sizeof cliente.telefone,
            false
        )
        || !solicitar_texto("Endereco: ", cliente.endereco, sizeof cliente.endereco, true)
        || !solicitar_texto("CEP (opcional): ", cliente.cep, sizeof cliente.cep, false)
        || !solicitar_texto("Local (opcional): ", cliente.local, sizeof cliente.local, false)
        || !solicitar_texto("Numero (opcional): ", cliente.numero, sizeof cliente.numero, false)
        || !solicitar_texto("Bairro (opcional): ", cliente.bairro, sizeof cliente.bairro, false)
        || !solicitar_texto("Cidade: ", cliente.cidade, sizeof cliente.cidade, true)
        || !solicitar_texto("Estado (UF): ", cliente.estado, sizeof cliente.estado, true)) {
        return false;
    }

    return informar_resultado(
        banco_abrir_conta(banco, cliente),
        "Conta aberta com sucesso."
    );
}

static bool executar_encerramento(Banco *banco) {
    char conta[16];
    printf("\n--- ENCERRAMENTO DE CONTA ---\n");
    if (!solicitar_texto("Numero da conta: ", conta, sizeof conta, true)) {
        return false;
    }
    return informar_resultado(
        banco_encerrar_conta(banco, conta),
        "Conta encerrada com sucesso."
    );
}

static bool executar_consulta(Banco *banco) {
    char conta[16];
    printf("\n--- CONSULTA DE CLIENTE ---\n");
    if (!solicitar_texto("Numero da conta: ", conta, sizeof conta, true)) {
        return false;
    }

    const Cliente *cliente = banco_consultar_por_conta(banco, conta);
    if (!cliente) {
        printf("Conta ativa nao encontrada.\n");
    } else {
        io_imprimir_cliente(cliente);
    }
    return true;
}

static bool executar_atualizacao(Banco *banco) {
    char conta[16];
    printf("\n--- ALTERACAO DE CLIENTE ---\n");
    if (!solicitar_texto("Numero da conta: ", conta, sizeof conta, true)) {
        return false;
    }

    const Cliente *cliente = banco_consultar_por_conta(banco, conta);
    if (!cliente) {
        printf("Conta ativa nao encontrada.\n");
        return true;
    }

    ClienteAtualizacao atualizacao = {0};
    copiar_texto(atualizacao.nome, sizeof atualizacao.nome, cliente->nome);
    copiar_texto(atualizacao.telefone, sizeof atualizacao.telefone, cliente->telefone);
    copiar_texto(atualizacao.endereco, sizeof atualizacao.endereco, cliente->endereco);

    char novo_nome[sizeof atualizacao.nome];
    printf("Nome atual: %s\n", cliente->nome);
    if (!solicitar_texto(
            "Novo nome (Enter mantem): ",
            novo_nome,
            sizeof novo_nome,
            false
        )) {
        return false;
    }
    if (novo_nome[0]) {
        copiar_texto(atualizacao.nome, sizeof atualizacao.nome, novo_nome);
    }

    char novo_telefone[sizeof atualizacao.telefone];
    printf("Telefone atual: %s\n", cliente->telefone);
    if (!solicitar_texto(
            "Novo telefone (Enter mantem): ",
            novo_telefone,
            sizeof novo_telefone,
            false
        )) {
        return false;
    }
    if (novo_telefone[0]) {
        copiar_texto(atualizacao.telefone, sizeof atualizacao.telefone, novo_telefone);
    }

    char novo_endereco[sizeof atualizacao.endereco];
    printf("Endereco atual: %s\n", cliente->endereco);
    if (!solicitar_texto(
            "Novo endereco (Enter mantem): ",
            novo_endereco,
            sizeof novo_endereco,
            false
        )) {
        return false;
    }
    if (novo_endereco[0]) {
        copiar_texto(atualizacao.endereco, sizeof atualizacao.endereco, novo_endereco);
    }

    return informar_resultado(
        banco_atualizar_cliente(banco, conta, &atualizacao),
        "Dados atualizados com sucesso."
    );
}

typedef BancoResultado (*OperacaoValor)(Banco *, const char *, Centavos);

static bool executar_operacao_valor(
    Banco *banco,
    const char *titulo,
    const char *rotulo_valor,
    const char *mensagem_sucesso,
    OperacaoValor operacao
) {
    char conta[16];
    Centavos valor = 0;
    printf("\n--- %s ---\n", titulo);
    if (!solicitar_texto("Numero da conta: ", conta, sizeof conta, true)
        || !solicitar_valor(rotulo_valor, &valor)) {
        return false;
    }

    return informar_resultado(
        operacao(banco, conta, valor),
        mensagem_sucesso
    );
}

static bool executar_saldo(Banco *banco) {
    char conta[16];
    printf("\n--- CONSULTA DE SALDO ---\n");
    if (!solicitar_texto("Numero da conta: ", conta, sizeof conta, true)) {
        return false;
    }

    const Cliente *cliente = banco_consultar_por_conta(banco, conta);
    if (!cliente) {
        printf("Conta ativa nao encontrada.\n");
    } else {
        io_imprimir_resumo_cliente(cliente);
    }
    return true;
}

static void executar_listagem(Banco *banco, BancoCriterioOrdenacao criterio) {
    Cliente *clientes = NULL;
    size_t quantidade = 0U;
    const BancoResultado resultado = banco_obter_clientes_ativos(
        banco,
        criterio,
        &clientes,
        &quantidade
    );
    if (resultado != BANCO_OK) {
        informar_resultado(resultado, "");
        return;
    }

    printf(
        "\n=== CLIENTES POR %s ===\n",
        criterio == BANCO_ORDENAR_POR_NOME ? "NOME" : "CONTA"
    );
    if (quantidade == 0U) {
        printf("Nenhum cliente ativo cadastrado.\n");
    }
    for (size_t i = 0; i < quantidade; i++) {
        io_imprimir_resumo_cliente(&clientes[i]);
    }
    banco_liberar_lista(clientes);
}

static bool executar_transferencia(Banco *banco) {
    char origem[16];
    char destino[16];
    Centavos valor = 0;
    printf("\n--- TRANSFERENCIA ENTRE CONTAS ---\n");
    if (!solicitar_texto("Conta de origem: ", origem, sizeof origem, true)
        || !solicitar_texto("Conta de destino: ", destino, sizeof destino, true)
        || !solicitar_valor("Valor: R$ ", &valor)) {
        return false;
    }

    return informar_resultado(
        banco_transferir(banco, origem, destino, valor),
        "Transferencia realizada com sucesso."
    );
}

int main(void) {
    Banco *banco = banco_criar(ARQUIVO_CLIENTES, ARQUIVO_MOVIMENTOS);
    if (!banco) {
        fprintf(stderr, "Nao foi possivel inicializar o banco.\n");
        return 1;
    }

    const BancoResultado carga = banco_carregar(banco);
    if (carga != BANCO_OK) {
        fprintf(
            stderr,
            "Nao foi possivel carregar os dados: %s.\n",
            banco_descrever_resultado(carga)
        );
        banco_destruir(banco);
        return 1;
    }

    bool executando = true;
    while (executando) {
        exibir_menu();
        int opcao = 0;
        if (!solicitar_opcao(&opcao)) {
            printf("\nEntrada encerrada.\n");
            break;
        }

        bool entrada_disponivel = true;
        switch (opcao) {
            case 1:
                entrada_disponivel = executar_abertura(banco);
                break;
            case 2:
                entrada_disponivel = executar_encerramento(banco);
                break;
            case 3:
                entrada_disponivel = executar_consulta(banco);
                break;
            case 4:
                entrada_disponivel = executar_atualizacao(banco);
                break;
            case 5:
                entrada_disponivel = executar_operacao_valor(
                    banco,
                    "DEPOSITO",
                    "Valor: R$ ",
                    "Deposito realizado com sucesso.",
                    banco_depositar
                );
                break;
            case 6:
                entrada_disponivel = executar_operacao_valor(
                    banco,
                    "SAQUE",
                    "Valor: R$ ",
                    "Saque realizado com sucesso.",
                    banco_sacar
                );
                break;
            case 7:
                entrada_disponivel = executar_saldo(banco);
                break;
            case 8:
                executar_listagem(banco, BANCO_ORDENAR_POR_NOME);
                break;
            case 9:
                executar_listagem(banco, BANCO_ORDENAR_POR_CONTA);
                break;
            case 10:
                entrada_disponivel = executar_transferencia(banco);
                break;
            case 11:
                entrada_disponivel = executar_operacao_valor(
                    banco,
                    "PAGAMENTO NO CARTAO",
                    "Valor da compra: R$ ",
                    "Pagamento realizado com sucesso.",
                    banco_pagar_cartao
                );
                break;
            case 0:
                executando = false;
                printf("Ate logo.\n");
                break;
            default:
                printf("Opcao inexistente.\n");
                break;
        }

        if (!entrada_disponivel) {
            printf("\nEntrada encerrada.\n");
            executando = false;
        }
    }

    banco_destruir(banco);
    return 0;
}
