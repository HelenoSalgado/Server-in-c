# Funcionalidades Principais

Esta seção detalha as principais funcionalidades do servidor.

[<- Voltar ao Índice](./index.md)

## Multi-threading

O servidor usa arquitetura multi-threaded para lidar com conexões concorrentes. Quando uma nova conexão é aceita, a thread principal cria uma nova thread worker (`pthread`) para gerenciar todo o ciclo de vida da requisição HTTP.

Isso permite que a thread principal volte imediatamente a escutar novas conexões, evitando que novos clientes sejam bloqueados por requisições demoradas.

A thread worker é responsável por:
1. Ler a requisição do cliente
2. Processar os cabeçalhos HTTP
3. Enviar a resposta
4. Fechar a conexão

As threads são criadas em estado desconectado (`pthread_detach`), garantindo que seus recursos sejam automaticamente liberados pelo sistema.

## Métodos HTTP

O servidor implementa o seguinte comportamento baseado no método HTTP:

- **`GET`**: Recupera o arquivo solicitado e envia seu conteúdo com os cabeçalhos apropriados (`Content-Type`, `Content-Length`, etc.)
- **`HEAD`**: Comporta-se identicamente ao `GET` mas **não** envia o conteúdo do arquivo. Envia apenas a linha de status e cabeçalhos
- **Outros Métodos (`POST`, `PUT`, `DELETE`, etc.)**: Responde com `501 Not Implemented`

## Logging

O servidor registra todas as requisições com formato padronizado:

`[timestamp] - [client_ip] - "[method] [path]" [status_code]`

**Exemplo:**
```
2025-09-28T18:30:00Z - 127.0.0.1 - "GET /index.html" 200 OK
```

- **Primeiro plano**: Logs exibidos no terminal com cores
- **Daemon**: Logs salvos em `server.log` e exibidos no terminal

## Desligamento Seguro

O servidor manipula sinais SIGINT (Ctrl+C) e SIGTERM para encerramento limpo, exibindo "Stopped" e limpando recursos.

## Gerenciamento PID

Em modo daemon, o servidor:
- Exibe o PID correto no terminal
- Cria arquivo `server.pid` no diretório atual
- Remove o arquivo PID ao encerrar

[<- Anterior: Configuração](./2-configuration.md) | [Próximo: Daemonização ->](./4-daemonization.md)
