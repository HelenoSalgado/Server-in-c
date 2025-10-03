# Salop Web Server

🚀 **Servidor web multi-threaded em C** com cache inteligente e suporte expandido a tipos MIME.

[![Build](https://img.shields.io/badge/build-passing-brightgreen)](#)
[![Tests](https://img.shields.io/badge/tests-4%2F4%20passing-brightgreen)](#)
[![License](https://img.shields.io/badge/license-MIT-blue)](#)
[![Version](https://img.shields.io/badge/version-1.0.0-blue)](#)

## ✨ Características

- 🔥 **Multi-threading** com suporte a conexões simultâneas
- 🧠 **Cache inteligente** com TTL configurável e thread-safety
- 📁 **25+ tipos MIME** suportados (imagens, fontes, mídia, documentos)
- ⚙️ **Sistema de build moderno** com Meson para instalação no sistema  
- 🛡️ **Segurança** com proteção contra path traversal
- 📊 **Logging estruturado** com cores e rastreamento de cache
- 🔧 **Configuração flexível** via linha de comando
- 🐧 **Daemon mode** com gerenciamento PID e serviço systemd

## 🚀 Instalação Rápida

### Com Meson (Recomendado)
```bash
# Instalar dependências (Ubuntu/Debian)
sudo apt install meson ninja-build gcc python3-requests

# Configurar, compilar e instalar
meson setup builddir
meson compile -C builddir
meson test -C builddir
sudo meson install -C builddir

# Executar
salop-server -p 8080
```

### Com Make (Alternativa)
```bash
make
./bin/server -p 8080
```

## 📖 Documentação

Para documentação completa, veja: **[docs/](docs/index.md)**

- **[Instalação](docs/install.md)** - Guia completo de instalação
- **[Configuração](docs/configuration.md)** - Opções e uso
- **[Funcionalidades](docs/features.md)** - Recursos detalhados
- **[Roadmap](docs/roadmap.md)** - Funcionalidades e desenvolvimento

## 🎯 Uso Básico

```bash
# Servidor básico
salop-server

# Porta e diretório customizados  
salop-server -p 8080 -d /var/www/html

# Executar como daemon
salop-server -b -p 80

# Como serviço systemd
sudo systemctl start salop-server
```

## 🧪 Cache e Performance

O servidor inclui um sistema de cache inteligente:
- **100 entradas** em memória (configurável)
- **TTL de 5 minutos** (configurável)  
- **Headers X-Cache** para debug (HIT/MISS)
- **Thread-safe** para alta concorrência

```bash
# Teste o cache
curl -I http://localhost:8080/data.json  # X-Cache: MISS
curl -I http://localhost:8080/data.json  # X-Cache: HIT  
```

## 🔧 Desenvolvimento

```bash
# Configurar com TLS (experimental)
meson setup builddir -Denable_tls=true

# Executar testes
meson test -C builddir

# Testes individuais
make cache_test utils_test
```

## 📋 Tipos MIME Suportados

- **Web**: HTML, CSS, JavaScript, JSON, XML
- **Imagens**: PNG, JPEG, GIF, ICO, BMP, SVG, WebP
- **Fontes**: WOFF, WOFF2, TTF, OTF, EOT
- **Mídia**: MP4, WebM, MP3, WAV, OGG
- **Documentos**: PDF, TXT, Markdown
- **Arquivos**: ZIP, GZIP

## 🤝 Contribuindo

1. Fork o projeto
2. Crie uma branch: `git checkout -b feature/amazing-feature`
3. Execute os testes: `meson test -C builddir`
4. Commit: `git commit -m 'Add amazing feature'`
5. Push: `git push origin feature/amazing-feature`
6. Abra um Pull Request

## 📄 Licença

Este projeto está sob a licença MIT. Veja [LICENSE](LICENSE) para detalhes.

---

Para mais detalhes, consulte a [documentação completa](docs/index.md).
