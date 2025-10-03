# Documentação do Salop Web Server

Este é um servidor web escrito em C com suporte a múltiplas funcionalidades.

## Características

- **Multi-threading**: Suporte a conexões simultâneas
- **HTTP/1.1**: Implementação completa do protocolo
- **Tipos MIME Expandidos**: Suporte a imagens, fontes, mídia e documentos
- **Segurança**: Proteção contra path traversal
- **Logging**: Sistema robusto de logs com cores
- **Daemon**: Execução em background

## Tipos MIME Suportados

### Imagens
- PNG, JPEG, GIF, ICO, BMP, SVG, WebP

### Fontes
- WOFF, WOFF2, TTF, OTF, EOT

### Mídia
- MP4, WebM, MP3, WAV, OGG

### Documentos
- PDF, TXT, Markdown

### Arquivos
- ZIP, GZIP, JSON, XML

## Como usar

```bash
# Compilar
make

# Executar
./bin/server

# Executar em background na porta 8080
./bin/server -b -p 8080

# Executar servindo diretório específico
./bin/server -d /var/www/html
```

## Testes

```bash
# Executar todos os testes
make test

# Testes específicos
make utils_test
make simple_test
make integration_test
```

---

**Status**: Ativo  
**Versão**: 1.0.0  
**Última atualização**: 2024-10-02