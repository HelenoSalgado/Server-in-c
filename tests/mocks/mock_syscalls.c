#include "mock_syscalls.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Mock para open
static int open_return_value = -1;

// Mock para write
static char *write_buffer = NULL;
static size_t write_buffer_size = 0;
static ssize_t write_return_value = -1;

// Mock para read
static const char *mock_read_buffer = NULL;
static size_t mock_read_count = 0;
static const char *mock_file_content_buffer = NULL;
static size_t mock_file_content_count = 0;

// Mock para realpath
static const char *mock_realpath_return_value = NULL;
static int mock_realpath_fail_flag = 0;

// Sequência para realpath
static const char **realpath_sequence = NULL;
static int realpath_sequence_count = 0;
static int realpath_sequence_index = 0;


void set_mock_open_return(int val) {
    open_return_value = val;
}

void set_mock_write_return(ssize_t val) {
    write_return_value = val;
}

void set_mock_read_buffer(const char *buf, size_t count) {
    mock_read_buffer = buf;
    mock_read_count = count;
}

void set_mock_file_content_buffer(const char *buf, size_t count) {
    mock_file_content_buffer = buf;
    mock_file_content_count = count;
}

const char *get_last_write_buffer() {
    return write_buffer;
}

size_t get_last_write_count() {
    return write_buffer_size;
}

void clear_mock_write_buffer() {
    if (write_buffer) {
        free(write_buffer);
        write_buffer = NULL;
        write_buffer_size = 0;
    }
}

int __wrap_open(const char *pathname, int flags, ...) {
    fprintf(stderr, "DEBUG: __wrap_open called for %s, returning %d\n", pathname, open_return_value);
    (void)flags;    // Não utilizado
    // Lida com mode_t para creat/open com O_CREAT
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        (void)mode; // Não utilizado
        va_end(args);
    }
    return open_return_value;
}

ssize_t mock_write(int fd, const void *buf, size_t count) {
    (void)fd; // Não utilizado
    if (write_buffer) {
        free(write_buffer);
    }
    write_buffer = (char *)malloc(count + 1);
    if (write_buffer) {
        memcpy(write_buffer, buf, count);
        write_buffer[count] = '\0';
        write_buffer_size = count;
    } else {
        write_buffer_size = 0;
    }
    return (write_return_value != -1) ? write_return_value : (ssize_t)count;
}

ssize_t __wrap_read(int fd, void *buf, size_t count) {
    fprintf(stderr, "DEBUG: __wrap_read called for fd %d, count %zu\n", fd, count);
    fprintf(stderr, "DEBUG: mock_read_buffer: %s\n", mock_read_buffer ? mock_read_buffer : "(null)");
    fprintf(stderr, "DEBUG: mock_file_content_buffer: %s\n", mock_file_content_buffer ? mock_file_content_buffer : "(null)");
    // Trata o client_fd (1) especificamente
    if (fd == 1) {
        size_t bytes_to_copy = (count < mock_read_count) ? count : mock_read_count;
        if (mock_read_buffer && buf) {
            memcpy(buf, mock_read_buffer, bytes_to_copy);
            return (ssize_t)bytes_to_copy;
        }
    }
    // Trata leituras de conteúdo de arquivo
    else if (mock_file_content_buffer && buf) { // Assume que leituras de conteúdo de arquivo não estão no fd 1
        size_t bytes_to_copy = (count < mock_file_content_count) ? count : mock_file_content_count;
        memcpy(buf, mock_file_content_buffer, bytes_to_copy);
        return (ssize_t)bytes_to_copy;
    }
    // Para qualquer outro fd (incluindo 0 para stdin), retorna EOF imediatamente
    return 0;
}

void set_mock_realpath_return(const char *resolved_path_val) {
    mock_realpath_return_value = resolved_path_val;
    mock_realpath_fail_flag = 0;
    clear_mock_realpath_sequence(); // Limpa a sequência se um retorno único for definido
}

void set_mock_realpath_fail(void) {
    mock_realpath_fail_flag = 1;
    mock_realpath_return_value = NULL;
    clear_mock_realpath_sequence(); // Limpa a sequência se um retorno único for definido
}

char *__wrap_realpath(const char *path, char *resolved_path) {
    fprintf(stderr, "DEBUG: __wrap_realpath called for path: %s\n", path);
    char *ret = NULL;
    if (realpath_sequence_count > 0 && realpath_sequence_index < realpath_sequence_count) {
        const char *current_return = realpath_sequence[realpath_sequence_index++];
        if (current_return == NULL) {
            ret = NULL;
        } else {
            strncpy(resolved_path, current_return, 1023);
            resolved_path[1023] = '\0';
            ret = resolved_path;
        }
    } else if (mock_realpath_fail_flag) {
        ret = NULL;
    } else if (mock_realpath_return_value) {
        strncpy(resolved_path, mock_realpath_return_value, 1023);
        resolved_path[1023] = '\0';
        ret = resolved_path;
    } else {
        ret = NULL; // Fallback para o realpath real se não for mockado
    }
    fprintf(stderr, "DEBUG: __wrap_realpath returning: %s\n", ret ? ret : "(null)");
    return ret;}

void set_mock_realpath_sequence(const char **values, int count) {
    clear_mock_realpath_sequence();
    if (count > 0 && values != NULL) {
        realpath_sequence = (const char **)malloc(count * sizeof(const char *));
        if (realpath_sequence) {
            for (int i = 0; i < count; ++i) {
                realpath_sequence[i] = values[i];
            }
            realpath_sequence_count = count;
            realpath_sequence_index = 0;
            mock_realpath_fail_flag = 0; // Desabilita a flag de falha única
            mock_realpath_return_value = NULL; // Desabilita o valor de retorno único
        }
    }
}

void clear_mock_realpath_sequence(void) {
    if (realpath_sequence) {
        free(realpath_sequence);
        realpath_sequence = NULL;
    }
    realpath_sequence_count = 0;
    realpath_sequence_index = 0;
}