# Compilação e Dependências

Este servidor é projetado para ser compilado em sistemas compatíveis com POSIX.

[<- Voltar ao Índice](./index.md)

## Dependências

- **`gcc`**: Compilador GNU para código C
- **`make`**: Utilitário para automatizar o processo de build
- **`pthreads`**: Biblioteca POSIX para threads (incluída por padrão no Linux e macOS)

## Compilação

Use o `Makefile` fornecido para compilação simplificada:

```bash
make
```

Este comando irá:
1. Compilar arquivos `.c` em objetos `.o`
2. Vincular objetos no executável `bin/server`
3. Incluir automaticamente a flag `-pthread`

Para limpar arquivos compilados:
```bash
make clean
```

## Testes

Execute testes unitários e de integração:
```bash
make test
```

[Próximo: Configuração ->](./configuration.md)
