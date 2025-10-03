# Daemonização

Esta seção explica como executar o servidor como daemon (processo em segundo plano).

[<- Voltar ao Índice](./index.md)

## Executando em Segundo Plano

Para executar o servidor como daemon, use a flag `-b`:

```bash
./bin/server -b
```

Com esta flag, o servidor se desconecta imediatamente do terminal controlador e executa em segundo plano. Você recupera o prompt do shell e o processo continua executando.

## Como Funciona

O processo de daemonização segue convenções Unix padrão para garantir que o processo seja adequadamente desconectado:

1. **Primeiro Fork**: O processo principal faz fork e o processo pai sai imediatamente, retornando controle ao shell
2. **Nova Sessão**: O processo filho chama `setsid()` para criar nova sessão e grupo de processos, tornando-se líder da sessão e desconectando-se do terminal controlador
3. **Segundo Fork**: O processo faz fork novamente e o pai (líder da sessão) sai. Isso garante que o daemon não seja líder de sessão e não possa readquirir um terminal controlador
4. **Mudar Diretório**: Mantém o diretório de trabalho atual para preservar caminhos relativos
5. **Definir Máscara de Arquivo**: `umask(0)` é chamado para garantir que arquivos criados pelo daemon tenham as permissões desejadas
6. **Fechar Descritores**: Todos os descritores de arquivo abertos herdados do pai são fechados, incluindo `stdin`, `stdout` e `stderr`

## Gerenciamento PID

O servidor daemon:
- **Exibe PID**: Mostra o PID correto no terminal durante a inicialização
- **Arquivo PID**: Cria `server.pid` no diretório atual com o PID do processo
- **Sincronização**: O PID exibido e o arquivo são idênticos e funcionais

## Parando o Daemon

Use o PID exibido na inicialização ou o arquivo PID:

```bash
# Usando arquivo PID (recomendado)
kill $(cat server.pid)

# Encontrando manualmente
ps aux | grep server
kill <PID>
```

O daemon responde a sinais SIGTERM e SIGINT, limpando recursos e removendo o arquivo PID.

[<- Anterior: Funcionalidades](./features.md) | [Próximo: Estrutura do Projeto ->](./project-structure.md)
