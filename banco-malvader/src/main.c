#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banco.h"
#include "io.h"

void exibir_menu() {
    printf("\n");
    printf("========================================\n");
    printf("         BANCO MALVADER - MENU\n");
    printf("========================================\n");
    printf(" 1. Abrir conta\n");
    printf(" 2. Encerrar conta\n");
    printf(" 3. Consultar cliente\n");
    printf(" 4. Alterar dados do cliente\n");
    printf(" 5. Depositar\n");
    printf(" 6. Sacar\n");
    printf(" 7. Consultar saldo\n");
    printf(" 8. Exibir extrato\n");
    printf(" 9. Listar clientes por nome\n");
    printf("10. Listar clientes por conta\n");
    printf(" 0. Sair\n");
    printf("========================================\n");
    printf("Escolha uma opcao: ");
}

void abrir_conta(Banco* b) {
    Cliente c;
    cliente_init(&c);
    
    printf("\n=== ABERTURA DE CONTA ===\n");
    
    printf("Nome completo: ");
    ler_linha(c.nome, sizeof(c.nome));
    
    printf("CPF (formato: 000.000.000-00): ");
    ler_linha(c.cpf, sizeof(c.cpf));
    
    printf("Data de nascimento (DD/MM/AAAA): ");
    ler_linha(c.data_nasc, sizeof(c.data_nasc));
    
    printf("Telefone: ");
    ler_linha(c.telefone, sizeof(c.telefone));
    
    printf("Endereco (rua/avenida): ");
    ler_linha(c.endereco, sizeof(c.endereco));
    
    printf("Numero: ");
    ler_linha(c.numero_casa, sizeof(c.numero_casa));
    
    printf("Bairro: ");
    ler_linha(c.bairro, sizeof(c.bairro));
    
    printf("Cidade: ");
    ler_linha(c.cidade, sizeof(c.cidade));
    
    printf("Estado (sigla): ");
    ler_linha(c.estado, sizeof(c.estado));
    
    printf("CEP: ");
    ler_linha(c.cep, sizeof(c.cep));
    
    printf("Numero da conta (ex: 123456-7): ");
    ler_linha(c.conta, sizeof(c.conta));
    
    printf("Senha (minimo 4 caracteres): ");
    ler_linha(c.senha, sizeof(c.senha));
    
    strcpy(c.agencia, "0001");
    
    if (banco_abrir_conta(b, c)) {
        printf("\n*** Conta aberta com sucesso! ***\n");
    }
}

void encerrar_conta(Banco* b) {
    char conta[16], senha[20];
    
    printf("\n=== ENCERRAMENTO DE CONTA ===\n");
    printf("Numero da conta: ");
    ler_linha(conta, sizeof(conta));
    
    printf("Senha: ");
    ler_linha(senha, sizeof(senha));
    
    if (banco_encerrar_conta(b, conta, senha)) {
        printf("\n*** Conta encerrada com sucesso! ***\n");
    }
}

void consultar_cliente(Banco* b) {
    char opcao[10];
    
    printf("\n=== CONSULTAR CLIENTE ===\n");
    printf("1. Por numero da conta\n");
    printf("2. Por CPF\n");
    printf("Escolha: ");
    ler_linha(opcao, sizeof(opcao));
    
    int idx = -1;
    
    if (strcmp(opcao, "1") == 0) {
        char conta[16];
        printf("Numero da conta: ");
        ler_linha(conta, sizeof(conta));
        idx = banco_buscar_por_conta(b, conta);
    } else if (strcmp(opcao, "2") == 0) {
        char cpf[15];
        printf("CPF: ");
        ler_linha(cpf, sizeof(cpf));
        idx = banco_buscar_por_cpf(b, cpf);
    }
    
    if (idx >= 0) {
        Cliente* c = banco_obter_cliente(b, idx);
        cliente_imprimir(c);
    } else {
        printf("Cliente nao encontrado.\n");
    }
}

void alterar_dados(Banco* b) {
    char conta[16];
    
    printf("\n=== ALTERAR DADOS ===\n");
    printf("Numero da conta: ");
    ler_linha(conta, sizeof(conta));
    
    int idx = banco_buscar_por_conta(b, conta);
    if (idx < 0) {
        printf("Conta nao encontrada.\n");
        return;
    }
    
    Cliente novo;
    Cliente* atual = banco_obter_cliente(b, idx);
    novo = *atual;
    
    printf("\nDeixe em branco para manter o valor atual.\n");
    
    char buffer[120];
    
    printf("Telefone [%s]: ", atual->telefone);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.telefone, buffer);
    
    printf("Endereco [%s]: ", atual->endereco);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.endereco, buffer);
    
    printf("Numero [%s]: ", atual->numero_casa);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.numero_casa, buffer);
    
    printf("Bairro [%s]: ", atual->bairro);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.bairro, buffer);
    
    printf("Cidade [%s]: ", atual->cidade);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.cidade, buffer);
    
    printf("Estado [%s]: ", atual->estado);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.estado, buffer);
    
    printf("CEP [%s]: ", atual->cep);
    ler_linha(buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(novo.cep, buffer);
    
    if (banco_alterar_dados(b, conta, novo)) {
        printf("\n*** Dados alterados com sucesso! ***\n");
    }
}

void depositar(Banco* b) {
    char conta[16];
    double valor;
    
    printf("\n=== DEPOSITO ===\n");
    printf("Numero da conta: ");
    ler_linha(conta, sizeof(conta));
    
    printf("Valor: R$ ");
    scanf("%lf", &valor);
    limpar_buffer();
    
    if (banco_depositar(b, conta, valor)) {
        printf("\n*** Deposito realizado com sucesso! ***\n");
        printf("Novo saldo: R$ %.2f\n", banco_consultar_saldo(b, conta));
    }
}

void sacar(Banco* b) {
    char conta[16], senha[20];
    double valor;
    
    printf("\n=== SAQUE ===\n");
    printf("Numero da conta: ");
    ler_linha(conta, sizeof(conta));
    
    printf("Senha: ");
    ler_linha(senha, sizeof(senha));
    
    printf("Valor: R$ ");
    scanf("%lf", &valor);
    limpar_buffer();
    
    if (banco_sacar(b, conta, senha, valor)) {
        printf("\n*** Saque realizado com sucesso! ***\n");
        printf("Novo saldo: R$ %.2f\n", banco_consultar_saldo(b, conta));
    }
}

void consultar_saldo(Banco* b) {
    char conta[16];
    
    printf("\n=== CONSULTAR SALDO ===\n");
    printf("Numero da conta: ");
    ler_linha(conta, sizeof(conta));
    
    double saldo = banco_consultar_saldo(b, conta);
    if (saldo >= 0) {
        printf("\nSaldo atual: R$ %.2f\n", saldo);
    } else {
        printf("Conta nao encontrada.\n");
    }
}

int main(void) {
    Banco banco;
    banco_init(&banco, "data/clientes.txt", "data/movimentos.txt");
    
    printf("========================================\n");
    printf("      BEM-VINDO AO BANCO MALVADER\n");
    printf("========================================\n");
    
    if (!banco_carregar(&banco)) {
        printf("Erro ao carregar dados do banco.\n");
        return 1;
    }
    
    int opcao;
    
    do {
        exibir_menu();
        scanf("%d", &opcao);
        limpar_buffer();
        
        switch(opcao) {
            case 1:
                abrir_conta(&banco);
                pausar();
                break;
            case 2:
                encerrar_conta(&banco);
                pausar();
                break;
            case 3:
                consultar_cliente(&banco);
                pausar();
                break;
            case 4:
                alterar_dados(&banco);
                pausar();
                break;
            case 5:
                depositar(&banco);
                pausar();
                break;
            case 6:
                sacar(&banco);
                pausar();
                break;
            case 7:
                consultar_saldo(&banco);
                pausar();
                break;
            case 8: {
                char conta[16];
                printf("\nNumero da conta: ");
                ler_linha(conta, sizeof(conta));
                banco_exibir_extrato(&banco, conta);
                pausar();
                break;
            }
            case 9:
                banco_listar_ordenado_por_nome(&banco);
                pausar();
                break;
            case 10:
                banco_listar_ordenado_por_conta(&banco);
                pausar();
                break;
            case 0:
                printf("\nEncerrando sistema...\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
        
    } while (opcao != 0);
    
    banco_free(&banco);
    printf("\nObrigado por usar o Banco Malvader!\n");
    
    return 0;
}