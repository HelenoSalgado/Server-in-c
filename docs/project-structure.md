# Estrutura do Projeto - Nova Organização

Este documento fornece uma visão geral da estrutura **reorganizada e profissional** do código fonte.

[<- Voltar ao Índice](./index.md)

## 🏗️ Nova Estrutura Organizada

```
./
├── src/                   # 📁 Código fonte organizado
│   ├── core/              # ⚙️  Arquivos principais do servidor
│   │   ├── server.c       # Ponto de entrada e loop principal
│   │   └── config.c       # Configuração global
│   ├── include/           # 📋 Headers centralizados
│   │   ├── config.h       # Configuração (declarações)
│   │   ├── context.h      # Contexto HTTP
│   │   ├── create.h       # Socket
│   │   ├── http_handler.h # Handler HTTP
│   │   ├── logger.h       # Sistema de logs
│   │   ├── regex.h        # Parsing de requisições
│   │   ├── verify.h       # Tipos MIME
│   │   ├── path.h         # Sanitização de caminhos
│   │   ├── cache.h        # Sistema de cache
│   │   └── tls.h          # TLS/HTTPS (experimental)
│   ├── http/              # 🌐 Módulo HTTP
│   │   └── http_handler.c # Processamento de requisições HTTP
│   ├── socket/            # 🔌 Módulo de socket
│   │   └── create.c       # Criação e configuração de sockets
│   └── utils/             # 🛠️  Utilitários
│       ├── logger.c       # Sistema de logs coloridos
│       ├── regex.c        # Parsing simplificado de HTTP
│       ├── verify.c       # Detecção de tipos MIME
│       ├── path.c         # Sanitização segura de caminhos
│       ├── cache.c        # Sistema de cache em memória
│       └── tls.c          # TLS/HTTPS (experimental)
├── tests/                 # 🧪 Testes automatizados
│   ├── simple_cache_test.c      # Testes do sistema de cache
│   ├── simple_utils_test.c      # Testes de utilitários
│   ├── simple_path_test.c       # Testes de sanitização
│   ├── integration_test.py      # Testes de integração
│   └── meson.build              # Configuração de testes
├── docs/                  # 📚 Documentação organizada
│   ├── index.md           # Índice principal
│   ├── install.md         # Guia de instalação
│   ├── compilation.md     # Build e dependências
│   ├── configuration.md   # Configuração e uso
│   ├── features.md        # Funcionalidades
│   ├── daemonization.md   # Execução como serviço
│   ├── project-structure.md # Este arquivo
│   ├── roadmap.md         # Roadmap de desenvolvimento
│   ├── improvements.md    # Histórico de melhorias
│   └── changelog.md       # Resumo das melhorias recentes
├── contrib/               # 🤝 Arquivos de contribuição
│   ├── salop-server.service    # Serviço systemd
│   └── post-install.sh         # Script pós-instalação
├── web/                   # 🌍 Arquivos web de exemplo
│   ├── index.html         # Página inicial
│   ├── 404.html           # Página de erro
│   ├── mime-test.html     # Teste de tipos MIME
│   ├── data.json          # Exemplo JSON
│   ├── config.xml         # Exemplo XML
│   ├── documentation.md   # Documentação em Markdown
│   └── readme.txt         # Exemplo de texto
├── bin/                   # 📦 Saída compilada (Makefile)
├── builddir/              # 📦 Saída compilada (Meson)
├── meson.build            # 🔧 Sistema de build Meson
├── meson_options.txt      # ⚙️  Opções de configuração
├── Makefile               # 🔧 Sistema de build Make (legado)
└── README.md              # 📖 Documentação principal
```

## 📋 Detalhamento dos Módulos

### **src/core/**
Contém os arquivos principais do servidor:
- **server.c**: Ponto de entrada, gerenciamento de argumentos, loop principal de conexões, threads
- **config.c**: Variáveis de configuração global com valores padrão

### **src/include/**
Headers centralizados para facilitar includes:
- Todos os headers `.h` ficam aqui
- Include paths simplificados: `#include "config.h"` 
- Melhor organização e evita includes relativos complexos

### **src/http/**
Módulo de processamento HTTP:
- **http_handler.c**: Processa requisições GET/HEAD, gerencia cache, envia respostas

### **src/socket/**
Módulo de comunicação de rede:
- **create.c**: Cria socket, configura bind/listen, opções SO_REUSEADDR

### **src/utils/**
Utilitários do servidor:
- **logger.c**: Sistema de logs com cores e arquivo
- **regex.c**: Parser HTTP simplificado (sem dependência POSIX)
- **verify.c**: Detecção de 25+ tipos MIME
- **path.c**: Sanitização contra path traversal
- **cache.c**: Cache LRU com TTL e thread-safety
- **tls.c**: Base para HTTPS (experimental)

## ✅ Melhorias da Nova Estrutura

### **Organização Profissional**
- ✅ Separação clara entre módulos
- ✅ Headers centralizados em `src/include/`
- ✅ Código fonte organizado por responsabilidade

### **Build System Moderno**
- ✅ Suporte completo ao Meson
- ✅ Includes simplificados
- ✅ Configuração flexível

### **Manutenibilidade**
- ✅ Módulos bem definidos
- ✅ Dependencies claras
- ✅ Fácil localização de arquivos

### **Escalabilidade**
- ✅ Estrutura preparada para novos módulos
- ✅ Sistema de includes escalável
- ✅ Base sólida para crescimento

## 🔧 Sistema de Build

### **Meson (Recomendado)**
```bash
meson setup builddir
meson compile -C builddir
meson test -C builddir
sudo meson install -C builddir
```

### **Make (Alternativo)**
```bash
make
make test
```

## 🚀 Próximos Passos

A nova estrutura facilita:
- ✅ Adição de novos módulos
- ✅ Desenvolvimento colaborativo
- ✅ Manutenção e debugging
- ✅ Expansão de funcionalidades

[<- Anterior: Daemonização](./daemonization.md)