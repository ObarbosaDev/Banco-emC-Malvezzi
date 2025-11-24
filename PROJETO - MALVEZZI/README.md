# Projeto Malvezzi – Sistema Bancário em C

O Projeto Malvezzi é um sistema bancário desenvolvido em linguagem C, com o objetivo de aplicar conceitos de modularização, manipulação de arquivos, ponteiros e estruturas de dados.
O sistema simula operações bancárias reais, permitindo o gerenciamento completo de clientes, contas, transações, PIX, cartões e relatórios.

## Objetivo do Projeto

O trabalho foi desenvolvido com foco em:

* Aplicar leitura e escrita em arquivos texto para armazenamento persistente.
* Trabalhar com estruturas e ponteiros.
* Dividir o projeto em múltiplos módulos com arquivos `.c` e `.h`.
* Criar um sistema funcional com menus e interações completas.
* Utilizar Makefile para automação da compilação.

## Estrutura do Projeto

PROJETO - MALVEZZI/
│
├── banco.exe                     # Executável principal
├── main.c                        # Arquivo que contém o menu principal
├── Makefile                      # Automação da compilação
│
├── src/                          # Módulos do sistema
│   ├── banco.c / banco.h         # Funções gerais do banco
│   ├── cliente.h                 # Estrutura de dados do cliente
│   ├── io.c / io.h               # Funções de entrada e saída
│   ├── ordenacao.c / ordenacao.h # Algoritmos de ordenação
│   ├── persistencia.c/persistencia.h # Leitura e escrita de arquivos
│
├── data/                         # Base de dados em arquivos texto
│   ├── clientes.txt
│   ├── cartoes.txt
│   ├── chaves_pix.txt
│   ├── movimentos.txt
│   ├── transacoes_pix.txt
│   ├── ...
│
└── .vscode/                      # Configurações de ambiente

## Funcionalidades do Sistema

## 1. Gerenciamento de Clientes

* Cadastro de novos clientes
* Edição de dados
* Listagem e busca
* Exclusão lógica com manutenção de histórico

## 2. Contas Bancárias

* Abertura de contas
* Encerramento de contas
* Consulta de informações completas
* Listagem por nome ou número
* Persistência automática em arquivos

## 3. Operações Financeiras

* Depósitos
* Saques
* Consulta de saldo
* Histórico de movimentações
* Emissão de extratos

## 4. Sistema PIX

* Cadastro de chaves PIX
* Transferências entre contas
* Registro detalhado de operações
* Armazenamento separado para fácil auditoria

## 5. Cartões

* Cadastro de cartões associados à conta
* Persistência em arquivo dedicado

## 6. Ordenação e Relatórios

* Ordenação de clientes e contas por diferentes critérios
* Implementação no módulo `ordenacao.c`
* Geração de relatórios a partir dos arquivos

## 7. Persistência de Dados

Todos os dados são armazenados na pasta `/data`, utilizando arquivos `.txt` de fácil leitura.

## Tecnologias e Conceitos Utilizados

* Linguagem C
* Modularização
* Structs e ponteiros
* Manipulação de arquivos
* Makefile
* Ordenação
* Organização de código
* VSCode como ambiente de desenvolvimento

## Como Compilar e Executar

Acesse a pasta raiz do projeto:

cd "PROJETO - MALVEZZI"

Compile o sistema:

make

## Testes Recomendados

* Criar um cliente e abrir uma conta
* Realizar depósito e saque
* Registrar e utilizar chave PIX
* Gerar extratos e conferir arquivos
* Criar vários clientes e testar a ordenação
* Encerrar contas e confirmar persistência

## Estrutura dos Arquivos de Dados

## clientes.txt

id;nome;cpf;telefone;status

### movimentos.txt

id_conta;data;tipo;valor

### transacoes_pix.txt

remetente;destinatario;chave;valor;data

Outros arquivos seguem o mesmo padrão de organização.

## Autor

Matheus Pinheiro Barbosa, Pedro Gabriel e Thiago da Costa.
Estudantes de Análise e Desenvolvimento de Sistemas – UCB

## Licença

Projeto desenvolvido para fins acadêmicos e livre para estudo, análise e aperfeiçoamento.
