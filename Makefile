CC = gcc
CFLAGS = -Wall -Wextra -g -Isrc/include
LDFLAGS = -pthread
TARGET = bin/server

# Source files com nova estrutura organizada
SOURCES = src/core/server.c src/core/config.c \
          src/http/http_handler.c \
          src/socket/create.c \
          src/utils/logger.c src/utils/regex.c src/utils/verify.c src/utils/path.c src/utils/cache.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo "  LD $@"
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "✓ Build successful! Executable at ./bin/server"

%.o: %.c
	@echo "  CC $< "
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "  CLEAN"
	@rm -f $(OBJECTS)
	@rm -rf $(TARGET) bin/simple_* *.o src/**/*.o
	@echo "✓ Clean complete."

# Testes simples adaptados para nova estrutura
simple_test: bin/simple_path_test
	@echo "\n-- Executando Teste de Caminho --"
	@./bin/simple_path_test

bin/simple_path_test: tests/simple_path_test.c src/utils/path.c
	@mkdir -p $(dir $@)
	@echo "  CC $^"
	@$(CC) $(CFLAGS) $^ -o $@

utils_test: bin/simple_utils_test
	@echo "\n-- Executando Teste de Utilitários --"
	@./bin/simple_utils_test

bin/simple_utils_test: tests/simple_utils_test.c src/utils/regex.c src/utils/verify.c
	@mkdir -p $(dir $@)
	@echo "  CC $^"
	@$(CC) $(CFLAGS) $^ -o $@

cache_test: bin/simple_cache_test
	@echo "\n-- Executando Teste do Cache --"
	@./bin/simple_cache_test

bin/simple_cache_test: tests/simple_cache_test.c src/utils/cache.c
	@mkdir -p $(dir $@)
	@echo "  CC $^"
	@$(CC) $(CFLAGS) $^ -o $@

integration_test: $(TARGET)
	@echo "\n-- Executando Testes de Integração --"
	@python3 tests/integration_test.py

test: simple_test utils_test cache_test integration_test
	@echo "\n✅ Todos os testes concluídos com sucesso!"

.PHONY: all clean test simple_test utils_test cache_test integration_test