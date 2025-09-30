# Estrutura do Projeto

Este documento fornece uma visão geral do layout do código fonte.

[<- Voltar ao Índice](./index.md)

```
/
├── http/               # Manipulação do protocolo HTTP
│   ├── context.h
│   ├── http_handler.c
│   └── http_handler.h
├── socket/             # Criação e gerenciamento de sockets
│   ├── create.c
│   └── create.h
├── utils/              # Funções utilitárias
│   ├── logger.c
│   ├── logger.h
│   ├── path.c
│   ├── path.h
│   ├── regex.c
│   ├── regex.h
│   ├── verify.c
│   └── verify.h
├── web/                # Diretório raiz padrão dos arquivos web
│   ├── 404.html
│   ├── assets/
│   └── index.html
├── bin/                # Saída compilada
│   └── server
├── config.c            # Variáveis de configuração global (definições)
├── config.h            # Variáveis de configuração global (declarações)
├── server.c            # Ponto de entrada principal, loop de conexões
├── Makefile            # Script de build
├── README.md           # Arquivo principal de documentação
└── docs/               # Documentação detalhada
    └── ...
```

## Detalhamento dos Diretórios

- **`http/`**: Contém a lógica central de manipulação de requisições HTTP. `http_handler.c` é responsável por analisar requisições e orquestrar respostas
- **`socket/`**: Gerencia operações de socket de baixo nível, especificamente criação e configuração do socket de escuta
- **`utils/`**: Coleção de módulos auxiliares:
    - `logger`: Para registro de requisições com cores e arquivo
    - `regex`: Para correspondência de padrões e extração de método/caminho das requisições
    - `verify`: Para determinação do tipo MIME de arquivos
    - `path`: Para sanitização e resolução segura de caminhos
- **`web/`**: Diretório padrão do qual arquivos estáticos são servidos
- **`bin/`**: Armazena o executável final do servidor. Criado pelo `Makefile`
- **`docs/`**: Contém arquivos de documentação detalhada

## Arquivos Principais

- **`server.c`**: Ponto de entrada da aplicação. Gerencia argumentos de linha de comando, inicia o socket de escuta e entra no loop principal `accept()`, criando nova thread para cada conexão
- **`config.c` / `config.h`**: Gerencia configuração global como `PORT` e `DIR_ROOT` padrão. Permite compartilhamento de configuração entre módulos sem dependências circulares
- **`Makefile`**: Automatiza todo o processo de build e testes

## Melhorias Recentes

- **Eliminação de redundâncias**: Removida lógica duplicada entre `regex.c` e `path.c`
- **Arquitetura simplificada**: Integração direta de funcionalidades de resposta HTTP
- **Logging aprimorado**: Sistema unificado com suporte a cores e arquivo PID
- **Desligamento seguro**: Manipulação adequada de sinais para encerramento limpo

[<- Anterior: Daemonização](./4-daemonization.md)
