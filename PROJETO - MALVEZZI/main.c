/*
 * ARQUIVO: main.c
 * RESPONSABILIDADE: Interface de Terminal - Camada de Apresentação
 * 
 * Este arquivo contém:
 *  - Menu principal do sistema bancário
 *  - Captura de entrada do usuário
 *  - Fluxo de navegação entre operações
 *  - Chamadas para funções da fachada Banco (TAD - Tipo Abstrato de Dados)
 * 
 * TÓPICOS APLICADOS:
 *  - Funções e modularização (função main como orquestrador)
 *  - Laços de repetição com do-while para menu interativo
 *  - Passagem de parâmetros por valor e referência
 *  - Manipulação de dados com structs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/banco.h"
#include "src/io.h"
#include <stddef.h>

/*
 * FUNÇÃO: menu_principal()
 * RETORNO: int - opção escolhida pelo usuário
 * DESCRIÇÃO: Exibe o menu principal e lê a opção do usuário.
 *            Trata a entrada de forma segura.
 */
int menu_principal(void) {
    printf("========== BANCO MALVADER ==========\n");
    printf("1. Abrir conta\n");
    printf("2. Encerrar conta\n");
    printf("3. Consultar dados do cliente\n");
    printf("4. Alterar dados do cliente\n");
    printf("5. Deposito\n");
    printf("6. Saque\n");
    printf("7. Consultar saldo\n");
    printf("8. Listar clientes por nome\n");
    printf("9. Listar clientes por conta\n");
    printf("10. PIX (transferencia)\n");
    printf("11. Pagamento com cartao\n");
    printf("0. Sair\n");
    printf("====================================\n");

    return ler_opcao();
}

/*
 * FUNÇÃO: criar_cliente()
 * RETORNO: Cliente - estrutura preenchida com dados do novo cliente
 * DESCRIÇÃO: Lê dados completos do cliente do teclado usando entrada segura (fgets).
 *            Inicializa saldo em 0 e status como ativo.
 * 
 * TÓPICO APLICADO:
 *  - Entrada segura com fgets e remoção do newline
 *  - Inicialização de struct com valores padrão
 */
Cliente criar_cliente(void) {
    Cliente c = {0};  // Inicializa struct com zeros

    printf("Agencia (4 digitos): ");
    ler_linha(c.agencia, sizeof c.agencia);

    printf("Numero da conta: ");
    ler_linha(c.conta, sizeof c.conta);

    printf("Nome: ");
    ler_linha(c.nome, sizeof c.nome);

    printf("CPF (XXX.XXX.XXX-XX): ");
    ler_linha(c.cpf, sizeof c.cpf);

    printf("Data nascimento (YYYY-MM-DD): ");
    ler_linha(c.data_nasc, sizeof c.data_nasc);

    printf("Telefone: ");
    ler_linha(c.telefone, sizeof c.telefone);

    printf("Endereco: ");
    ler_linha(c.endereco, sizeof c.endereco);

    printf("CEP (XXXXX-XXX): ");
    ler_linha(c.cep, sizeof c.cep);

    printf("Local (bairro/zona): ");
    ler_linha(c.local, sizeof c.local);

    printf("Numero da casa: ");
    ler_linha(c.numero_casa, sizeof c.numero_casa);

    printf("Bairro: ");
    ler_linha(c.bairro, sizeof c.bairro);

    printf("Cidade: ");
    ler_linha(c.cidade, sizeof c.cidade);

    printf("Estado (UF): ");
    ler_linha(c.estado, sizeof c.estado);

    printf("Senha (minimo 4 caracteres): ");
    ler_linha(c.senha, sizeof c.senha);

    c.saldo = 0.0;  // Saldo inicial obrigatoriamente zero
    c.ativo = 1;    // Conta inicia como ativa

    return c;
}

/*
 * FUNÇÃO: main()
 * RETORNO: int - status de saída (0 = sucesso)
 * DESCRIÇÃO: Ponto de entrada do programa. Orquestra o fluxo principal:
 *            - Inicializa o TAD Banco
 *            - Carrega dados persistidos em arquivo
 *            - Executa loop interativo de menu
 *            - Libera memória ao encerrar
 * 
 * TÓPICOS APLICADOS:
 *  - Modularização: delegação de responsabilidades
 *  - Passagem por referência (&banco) para modificar estado
 *  - do-while para menu interativo
 *  - Liberação de recursos (banco_free)
 */
int main(void) {
    Banco banco;
    
    banco_init(&banco, "data/clientes.txt", "data/movimentos.txt");
    
    banco_carregar(&banco);

    int op;
    do {
        op = menu_principal();

        switch (op) {
            case 1: {
                // Abrir conta: valida duplicação de CPF e conta
                printf("\n--- ABERTURA DE CONTA ---\n");
                Cliente c = criar_cliente();
                if (banco_abrir_conta(&banco, c)) {
                    printf("Conta aberta com sucesso!\n");
                } else {
                    printf("Erro ao abrir conta.\n");
                }
                break;
            }

            case 2: {
                // Encerrar conta: valida saldo zero
                printf("\n--- ENCERRAMENTO DE CONTA ---\n");
                char conta[16];
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                if (banco_encerrar_conta(&banco, conta)) {
                    printf("Conta encerrada com sucesso!\n");
                } else {
                    printf("Erro ao encerrar conta.\n");
                }
                break;
            }

            case 3: {
                // Consultar dados: busca linear por conta (ativa apenas)
                printf("\n--- CONSULTA DE CLIENTE ---\n");
                char conta[16];
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                Cliente* c = banco_consultar_por_conta(&banco, conta);
                if (c) {
                    imprimir_cliente(c);
                } else {
                    printf("Conta inexistente.\n");
                }
                break;
            }

            case 4: {
                // Alterar dados: preserva número da conta
                printf("\n--- ALTERACAO DE DADOS ---\n");
                char conta[16];
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                Cliente* c = banco_consultar_por_conta(&banco, conta);
                if (c) {
                    printf("Informe os novos dados (deixe em branco para manter):\n");
                    Cliente novo = *c;  // Cópia da estrutura

                    printf("Nome [%s]: ", c->nome);
                    fgets(novo.nome, sizeof novo.nome, stdin);
                    if (novo.nome[0] != '\n') {
                        novo.nome[strcspn(novo.nome, "\n")] = 0;
                    } else {
                        strcpy(novo.nome, c->nome);
                    }

                    printf("Telefone [%s]: ", c->telefone);
                    fgets(novo.telefone, sizeof novo.telefone, stdin);
                    if (novo.telefone[0] != '\n') {
                        novo.telefone[strcspn(novo.telefone, "\n")] = 0;
                    } else {
                        strcpy(novo.telefone, c->telefone);
                    }

                    printf("Endereco [%s]: ", c->endereco);
                    fgets(novo.endereco, sizeof novo.endereco, stdin);
                    if (novo.endereco[0] != '\n') {
                        novo.endereco[strcspn(novo.endereco, "\n")] = 0;
                    } else {
                        strcpy(novo.endereco, c->endereco);
                    }

                    if (banco_alterar_dados(&banco, conta, novo)) {
                        printf("Dados alterados com sucesso!\n");
                    }
                } else {
                    printf("Conta inexistente.\n");
                }
                break;
            }

            case 5: {
                // Depósito: adiciona valor ao saldo e registra movimento
                printf("\n--- DEPOSITO ---\n");
                char conta[16];
                double valor;
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                printf("Valor a depositar: R$ ");
                scanf("%lf", &valor);
                while (getchar() != '\n');  // Limpa buffer de entrada
                if (banco_depositar(&banco, conta, valor)) {
                    printf("Deposito realizado com sucesso!\n");
                } else {
                    printf("Erro ao realizar deposito.\n");
                }
                break;
            }

            case 6: {
                // Saque: desconta valor e valida saldo
                printf("\n--- SAQUE ---\n");
                char conta[16];
                double valor;
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                printf("Valor a sacar: R$ ");
                scanf("%lf", &valor);
                while (getchar() != '\n');
                if (banco_sacar(&banco, conta, valor)) {
                    printf("Saque realizado com sucesso!\n");
                } else {
                    printf("Erro ao realizar saque.\n");
                }
                break;
            }

            case 7: {
                // Consultar saldo: busca e exibe saldo atual
                printf("\n--- CONSULTA DE SALDO ---\n");
                char conta[16];
                printf("Numero da conta: ");
                ler_linha(conta, sizeof conta);
                Cliente* c = banco_consultar_por_conta(&banco, conta);
                if (c) {
                    printf("Saldo da conta %s: R$ %.2f\n", conta, c->saldo);
                } else {
                    printf("Conta inexistente.\n");
                }
                break;
            }

            case 8: {
                // Listar por nome: ordena com Quick Sort e exibe
                banco_listar_por_nome(&banco);
                break;
            }

            case 9: {
                // Listar por conta: ordena com Quick Sort por número de conta
                banco_listar_por_conta(&banco);
                break;
            }
            case 10: {
            // PIX: transferência entre contas
            printf("\n--- PIX ---\n");
            char origem[16], destino[16];
            double valor;

            printf("Conta origem: ");
            ler_linha(origem, sizeof origem);

            printf("Conta destino: ");
            ler_linha(destino, sizeof destino);

            printf("Valor: R$ ");
            scanf("%lf", &valor);
            while (getchar() != '\n');

            if (banco_pix(&banco, origem, destino, valor)) {
                printf("PIX realizado com sucesso!\n");
            } else {
                printf("Erro ao realizar PIX.\n");
            }
            break;
        }

        case 11: {
            // Pagamento no cartão: débito direto no saldo
            printf("\n--- PAGAMENTO COM CARTAO ---\n");
            char conta[16];
            double valor;

            printf("Numero da conta: ");
            ler_linha(conta, sizeof conta);

            printf("Valor da compra: R$ ");
            scanf("%lf", &valor);
            while (getchar() != '\n');

            if (banco_cartao(&banco, conta, valor)) {
                printf("Pagamento realizado com sucesso!\n");
            } else {
                printf("Pagamento negado.\n");
            }
            break;
        }

            case 0:
                printf("Encerrando... Ate logo!\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }

    } while (op != 0);  // Loop continua até usuário escolher sair (opção 0)

    banco_free(&banco);
    return 0;
}
