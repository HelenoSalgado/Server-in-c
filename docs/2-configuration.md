# Configuração e Uso

O comportamento do servidor é configurado via argumentos de linha de comando.

[<- Voltar ao Índice](./index.md)

## Executando o Servidor

O executável compilado está localizado em `./bin/server`.

```bash
./bin/server [opções]
```

Sem opções, o servidor inicia em primeiro plano na porta `3000`, servindo arquivos de `./web`.

## Opções de Linha de Comando

| Opção       | Descrição                                    | Padrão       |
|-------------|----------------------------------------------|--------------|
| `-p <port>` | Porta de escuta                              | `3000`       |
| `-d <dir>`  | Diretório raiz dos arquivos                  | `./web`      |
| `-b`        | Executar como daemon (segundo plano)        | (primeiro plano) |
| `-h`        | Exibir mensagem de ajuda                     |              |

## Exemplos

**Execução padrão:**
```bash
./bin/server
```

**Porta e diretório customizados:**
```bash
./bin/server -p 8080 -d /var/www/html
```

**Executar em segundo plano:**
```bash
./bin/server -b -p 80
```

## Logs

- **Primeiro plano**: Logs exibidos no terminal
- **Daemon**: Logs salvos em `server.log` no diretório atual e exibidos no terminal

## Encerrando o Servidor

**Primeiro plano:** Pressione `Ctrl+C`

**Daemon:** Use o PID exibido na inicialização ou arquivo PID:
```bash
kill $(cat server.pid)
```

[<- Anterior: Compilação](./1-compilation.md) | [Próximo: Funcionalidades ->](./3-features.md)
