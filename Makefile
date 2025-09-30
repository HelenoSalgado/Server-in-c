CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -g -I.
CXXFLAGS = -Wall -Wextra -g -I.
LDFLAGS = -pthread
TARGET = bin/server

# Source files for the main application
SOURCES = server.c config.c \
          http/http_handler.c \
          socket/create.c \
          utils/logger.c utils/regex.c utils/verify.c utils/path.c

OBJECTS = $(SOURCES:.c=.o)

# Test specific variables
TEST_BUILD_DIR = bin/tests
TEST_CFLAGS = $(CFLAGS) -I. -I./tests -DTEST_BUILD -DverifyMimeType=mock_verifyMimeType
TEST_CXXFLAGS = $(CXXFLAGS) -I. -I./tests -DTEST_BUILD -DverifyMimeType=mock_verifyMimeType \
                -I./tests/googletest/googletest/include -I./tests/googletest/googletest/googletest/ -I./tests/googletest/googlemock/include \
                -I/usr/include/c++/15.2.1

GTEST_OBJS = tests/googletest/googletest/googletest/src/gtest-all.o tests/googletest/googletest/googlemock/src/gmock-all.o

GTEST_COMPILATION_CXXFLAGS = -isystem./tests/googletest/googletest/include -isystem./tests/googletest/googletest/googletest/

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo "  LD $@"
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "✓ Build successful! Executable at ./bin/server"

%.o: %.c
	@echo "  CC $< "
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo "  CXX $< "
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "  CLEAN"
	@rm -f $(OBJECTS)
	@rm -f tests/mocks/*.o
	@rm -f $(GTEST_OBJS)
	@rm -rf $(TEST_BUILD_DIR) $(TARGET) bin/simple_path_test bin/simple_utils_test bin/simple_http_handler_test *.o
	@echo "✓ Clean complete."

# --- Test Targets ---

# Main test target
#test: $(TEST_BUILD_DIR)/http/test_http_handler
#	@echo "\n-- Running Google Tests --"
#	@./$(TEST_BUILD_DIR)/http/test_http_handler

simple_test: bin/simple_path_test
	@echo "\n-- Executando Teste de Caminho --"
	@./bin/simple_path_test

bin/simple_path_test: tests/simple_path_test.c utils/path.c
	@mkdir -p $(dir $@)
	@echo "  CC $^"
	@$(CC) $(CFLAGS) $^ -o $@

utils_test: bin/simple_utils_test
	@echo "\n-- Executando Teste de Utilitários --"
	@./bin/simple_utils_test

bin/simple_utils_test: tests/simple_utils_test.c utils/regex.c utils/verify.c
	@mkdir -p $(dir $@)
	@echo "  CC $^"
	@$(CC) $(CFLAGS) $^ -o $@

http_handler_test: bin/simple_http_handler_test
	@echo "\n-- Executando Teste do Manipulador HTTP --"
	@./bin/simple_http_handler_test

bin/simple_http_handler_test: tests/simple_http_handler_test.o http_handler.o regex.o verify.o config.o
	@mkdir -p $(dir $@)
	@echo "  LD $@"
	@$(CC) $(CFLAGS) $^ -o $@ -Wl,--wrap=read -Wl,--wrap=close -Wl,--wrap=logger_log -Wl,--wrap=httpResponse -Wl,--wrap=sanitize_path -Wl,--wrap=log_request -Wl,--wrap=open

tests/simple_http_handler_test.o: tests/simple_http_handler_test.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

http_handler.o: http/http_handler.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

regex.o: utils/regex.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

verify.o: utils/verify.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

config.o: config.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

integration_test: $(TARGET)
	@echo "\n-- Executando Testes de Integração --"
	@python3 tests/integration_test.py

test: simple_test utils_test http_handler_test integration_test
	@echo "\nTodos os testes concluídos com sucesso!"

# Build the test executable
TEST_OBJS = tests/http/test_http_handler.o $(GTEST_OBJS) \
            http/http_handler_test.o \
            tests/mocks/mock_syscalls.o tests/mocks/mock_http_response.o tests/mocks/mock_verify.o \
            utils/regex.o utils/logger.o utils/path_test.o config.o

$(TEST_BUILD_DIR)/http/test_http_handler: $(TEST_OBJS)
	@mkdir -p $(dir $@)
	@echo "  LD $@"
	@$(CXX) $(TEST_CXXFLAGS) -o $@ $^ $(LDFLAGS) -L/usr/lib -lgtest -lgtest_main -pthread

# --- Test Object Compilation ---

# Compile production code with mocks
tests/http/test_http_handler.o: tests/http/test_http_handler.c
	@echo "  CXX $<"
	@$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

http/http_handler_test.o: http/http_handler.c
	@echo "  CC (test) $< "
	@$(CC) $(TEST_CFLAGS) -Dread=__wrap_read -include tests/http/test_http_handler_includes.h -c $< -o $@

utils/path_test.o: utils/path.c
	@echo "  CC (test) $< "
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

tests/mocks/mock_syscalls.o: tests/mocks/mock_syscalls.c
	@echo "  CC $< "
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

tests/mocks/mock_http_response.o: tests/mocks/mock_http_response.c
	@echo "  CC $< "	@$(CC) $(TEST_CFLAGS) -c $< -o $@

tests/mocks/mock_verify.o: tests/mocks/mock_verify.c
	@echo "  CC $< "
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

tests/googletest/googletest/googletest/src/gtest-all.o: tests/googletest/googletest/googletest/src/gtest-all.cc
	@echo "  CXX (gtest) $< "
	@$(CXX) $(GTEST_COMPILATION_CXXFLAGS) -c $< -o $@

tests/googletest/googletest/googlemock/src/gmock-all.o: tests/googletest/googletest/googlemock/src/gmock-all.cc
	@echo "  CXX (gtest) $< "
	@$(CXX) $(GTEST_COMPILATION_CXXFLAGS) -c $< -o $@

.PHONY: all clean test simple_test utils_test http_handler_test integration_test