# Servidor Web em C

Um servidor web simples e leve desenvolvido em C como um projeto de aprendizado. O foco principal foi criar uma base de código modular, segura e de fácil manutenção.

## Funcionalidades

- **Servidor HTTP/1.1 Básico:** Capaz de servir arquivos estáticos.
- **Suporte a Múltiplos Tipos de Arquivos:** HTML, CSS, JavaScript, SVG, e mais, com detecção de *MIME type*.
- **Segurança:** Proteção contra ataques de *path traversal*, garantindo que apenas arquivos do diretório web sejam servidos.
- **Estrutura Modular:** O código é organizado com uma clara separação de responsabilidades (socket, HTTP, manipulação de MIME type, etc.).
- **Livre de Variáveis Globais:** Utiliza uma estrutura `http_context` para gerenciar o estado de cada requisição, tornando o código mais limpo e preparado para concorrência.

## Melhorias da Refatoração

Uma refatoração significativa foi realizada para melhorar a qualidade e a segurança do código. Para um detalhamento completo das mudanças, consulte o arquivo [IMPROVEMENTS.md](IMPROVEMENTS.md).

## Como Começar

### Pré-requisitos

- GCC (GNU Compiler Collection)
- Make

### Compilando e Executando

1.  **Navegue até o diretório refatorado:**

    ```bash
    cd mime-type
    ```

2.  **Compile o projeto:**

    ```bash
    make
    ```

3.  **Execute o servidor:**

    ```bash
    ./bin/server
    ```

O servidor estará rodando em `http://localhost:3000`.

### Limpando os Arquivos de Build

Para remover os arquivos de objeto e o executável, execute:

```bash
make clean
```