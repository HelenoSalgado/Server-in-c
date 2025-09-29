# Salop Web Server

Servidor escrito em C com suporte a multi-threaded para servir arquivos estáticos.

## Dependências

- `gcc` (GNU Compiler Collection)
- `make`
- A POSIX-compliant system with `pthreads` support (e.g., Linux, macOS).

## Como rodar

1.  **Compile the server:**
    ```bash
    make
    ```

2.  **Run the server:**
    ```bash
    ./bin/server [options]
    ```

### Opções de linha de comando

| Option      | Description                                           | Default      |
|-------------|-------------------------------------------------------|--------------|
| `-p <port>` | Especifíque a porta ouvinte.                          | `3000`       |
| `-d <dir>`  | Especifíque o diretório a ser servido.                | `./web`      |
| `-b`        | Rode em background - libere o terminal.               | (foreground) |
| `-h`        | Mostrar mensagem de ajuda.                            |              |

**Example:**
```bash
# Rodando servidor em background na porta 8080, servindo arquivos de /var/www
./bin/server -b -p 8080 -d /var/www
```

## Features

- **Multi-threaded:** Receba múltiplas conexões simultâneas.
- **Configuração:** Defina porta e diretório raíz via linha de comando.
- **Arquivos estáticos:** Sirva arquivos estáticos especificando um diretório.
- **HTTP/1.1 Compliance:**
    - Supporta `GET` e `HEAD` métodos.
    - Returna `501 Not Implemented` para métodos não suportados.
- **Logging:** Logs de requisições padronizadas no formato: `[timestamp] - [client_ip] - "[method] [path]" [status_code]`.
- **Daemonization:** Pode rodar processos filhos em background.

---
Para mais detalhes, veja o diretório `docs`.
