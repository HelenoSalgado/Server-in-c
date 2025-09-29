CC = gcc
CFLAGS = -Wall -Wextra -g -I.
TEST_CFLAGS = $(CFLAGS) -DUNITY_INCLUDE_CONFIG_H -I. -I./tests -DTEST_BUILD -DverifyMimeType=mock_verifyMimeType
LDFLAGS = -pthread
TARGET = bin/server

# Find all .c files in the project
SOURCES = server.c config.c \
          http/http_handler.c \
          methods/get/response.c \
          socket/create.c \
          utils/logger.c utils/regex.c utils/verify.c

# Replace .c with .o to get the object file names
OBJECTS = $(SOURCES:.c=.o)

# Test configuration
TEST_BUILD_DIR = bin/tests

# Find all test source files
TEST_SOURCES = $(wildcard tests/utils/*.c) \
               $(wildcard tests/http/*.c) \
               $(wildcard tests/mocks/*.c)

# Create a list of test executables from the test source files
TESTS = $(patsubst tests/%.c,$(TEST_BUILD_DIR)/%,$(filter tests/utils/%.c,$(TEST_SOURCES))) \
        $(patsubst tests/http/%.c,$(TEST_BUILD_DIR)/http/%,$(filter tests/http/%.c,$(TEST_SOURCES)))

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
	@rm -f $(OBJECTS) $(TARGET)
	@rm -rf $(TEST_BUILD_DIR)
	@echo "✓ Clean complete."

# --- Test Targets ---

# Main test target
test: $(TESTS)
	@echo "\n-- Running Tests --"
	@for test in $(TESTS); do \
	    ./$$test; \
	    done
	@echo "\n✓ Tests complete."

# Generic rule to build a test executable
# This links the test file with the corresponding source file and unity
TEST_MOCKS_OBJECTS = tests/mocks/mock_syscalls.o tests/mocks/mock_http_response.o tests/mocks/mock_verify.o

$(TEST_BUILD_DIR)/utils/test_mimetype: tests/utils/test_mimetype.o tests/unity/unity.o $(TEST_MOCKS_OBJECTS)
	@mkdir -p $(dir $@)
	@$(CC) $(TEST_CFLAGS) -o $@ tests/utils/test_mimetype.o tests/unity/unity.o $(TEST_MOCKS_OBJECTS)

$(TEST_BUILD_DIR)/utils/test_regex: tests/utils/test_regex.o utils/regex.o tests/unity/unity.o $(TEST_MOCKS_OBJECTS)
	@mkdir -p $(dir $@)
	@$(CC) $(TEST_CFLAGS) -o $@ tests/utils/test_regex.o utils/regex.o tests/unity/unity.o $(TEST_MOCKS_OBJECTS)

tests/http/test_http_handler.o: tests/http/test_http_handler.c
	@$(CC) $(TEST_CFLAGS) -Dread=mock_read -Dopen=mock_open -Drealpath=mock_realpath -c $< -o $@

tests/utils/test_mimetype.o: tests/utils/test_mimetype.c
	@$(CC) $(TEST_CFLAGS) -Dopen=mock_open -Drealpath=mock_realpath -c $< -o $@

tests/utils/test_regex.o: tests/utils/test_regex.c
	@$(CC) $(TEST_CFLAGS) -Drealpath=mock_realpath -c $< -o $@

http/http_handler_test.o: http/http_handler.c
	@$(CC) $(TEST_CFLAGS) -Dread=mock_read -Dopen=mock_open -Drealpath=mock_realpath -include tests/http/test_http_handler_includes.h -c $< -o $@

methods/get/response_test.o: methods/get/response.c
	@$(CC) $(TEST_CFLAGS) -Dopen=mock_open -Drealpath=mock_realpath -c $< -o $@

$(TEST_BUILD_DIR)/http/test_http_handler: tests/http/test_http_handler.o utils/regex.o utils/logger.o tests/unity/unity.o http/http_handler_test.o methods/get/response_test.o config.o $(TEST_MOCKS_OBJECTS)
	@mkdir -p $(dir $@)
	@$(CC) $(TEST_CFLAGS) -o $@ tests/http/test_http_handler.o utils/regex.o utils/logger.o tests/unity/unity.o http/http_handler_test.o methods/get/response_test.o config.o $(TEST_MOCKS_OBJECTS)

# Rule to compile mock_syscalls.c
tests/mocks/mock_syscalls.o: tests/mocks/mock_syscalls.c
	@mkdir -p $(dir $@)
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

# Rule to compile mock_http_response.c
tests/mocks/mock_http_response.o: tests/mocks/mock_http_response.c
	@mkdir -p $(dir $@)
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

# Generic rule to compile test source files into object files
%.o: %.c
	@$(CC) $(TEST_CFLAGS) -c $< -o $@

# Specific rule for http/http_handler.o when building tests
# This rule is removed to avoid compiling http/http_handler.o with TEST_CFLAGS for the main build.


.PHONY: all clean test
