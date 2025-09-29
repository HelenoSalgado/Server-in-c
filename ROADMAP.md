# Roadmap de Melhorias do Servidor

Este documento descreve metas para a evolução deste servidor web em C.

## 1. Implementar Concorrência com Multi-threading

**Objetivo:** Permitir que o servidor atenda múltiplos clientes simultaneamente, eliminando o gargalo de uma requisição por vez não escalável.

-   [x] Modificar o loop principal em `server.c`.
-   [x] Para cada nova conexão (`accept`), criar uma nova thread (usando `pthread_create`) para lidar com a requisição.
-   [x] A thread será responsável por chamar `http_handler`, enviar a resposta e fechar o `client_fd`.
-   [x] Garantir que a thread principal (`main`) faça a limpeza adequada das threads filhas para evitar memory leaks (usando `pthread_detach` ou `pthread_join`).
-   [x] Compilar com a flag `-pthread`.

## 2. Flexibilizar a Configuração

**Objetivo:** Remover configurações fixas (hardcoded) do código, como a porta e o diretório raiz.

-   [x] Implementar o parsing de argumentos de linha de comando (usando `getopt`).
-   [x] Permitir a configuração da porta (ex: `-p 8080`).
-   [x] Permitir a configuração do diretório raiz dos arquivos estáticos (ex: `-d /var/www`).
-   [x] Usar valores padrão caso nenhum argumento seja fornecido.

## 3. Melhorar o Tratamento do Protocolo HTTP

**Objetivo:** Aumentar a conformidade com o padrão HTTP e adicionar funcionalidades úteis.

-   [x] Implementar suporte ao método `HEAD`.
-   [x] Retornar o código de erro `501 Not Implemented` para métodos não suportados (`POST`, `PUT`, etc.).
-   [x] Adicionar um sistema de logging para registrar informações sobre as requisições (IP, data, método, URI, status).

## 4. Daemonização

**Objetivo:** Permitir que o servidor rode como um processo em segundo plano (daemon).

-   [x] Implementar a lógica de `fork`, criação de uma nova sessão (`setsid`), e desvinculação do terminal.

---

## Histórico de Conclusão

Este roadmap foi concluído, servindo como um histórico do desenvolvimento inicial e das melhorias implementadas.

## Próximos Passos: Testes Automatizados

**Objetivo:** Criar uma suíte de testes robusta para garantir a estabilidade do servidor e prevenir regressões.

-   [x] **Escolher um Framework de Teste:** Pesquisar e selecionar um framework de teste de unidade para C (ex: Check, CUnit ou Unity).
-   [x] **Testes de Unidade:**
    -   [x] Testar as funções de `utils` (MIME type, regex).
    -   [ ] Testar a lógica de parsing do `http_handler`.
-   [ ] **Testes de Integração/Funcionais:**
    -   [ ] Criar um script (em Bash, Python, etc.) que inicie o servidor.
    -   [ ] Usar `curl` ou outra ferramenta para fazer requisições HTTP e validar as respostas.
    -   [ ] Testar o método `GET` para arquivos existentes e inexistentes (404).
    -   [ ] Testar o método `HEAD`.
    -   [ ] Testar métodos não suportados (`POST`, etc.) para a resposta `501`.
    -   [ ] Testar os argumentos de linha de comando (`-p`, `-d`).
-   [ ] **Integração com `make`**: Adicionar um target `make test` para rodar todos os testes automaticamente.

### Erro silêncioso a resolver registrado journalctl:
ng degraded feature set UDP instead of TCP for DNS server 1.0.0.1.
ng degraded feature set TCP instead of UDP for DNS server 1.1.1.1.

