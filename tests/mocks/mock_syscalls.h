#ifndef MOCK_SYSCALLS_H
#define MOCK_SYSCALLS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

// Declarações para o mecanismo de wrap do linker
// A função __wrap_open será chamada em vez de open
int __wrap_open(const char *pathname, int flags, ...);
// A função __real_open pode ser usada dentro de __wrap_open para chamar a open original
int __real_open(const char *pathname, int flags, ...);

#ifdef TEST_BUILD
// Mock para a função write
ssize_t mock_write(int fd, const void *buf, size_t count);
#define write mock_write

// Mock para a função read
ssize_t __wrap_read(int fd, void *buf, size_t count);
#define read __wrap_read
#endif // TEST_BUILD

// Função para obter o último buffer escrito
const char* get_last_write_buffer();
size_t get_last_write_count();
void clear_mock_write_buffer();

// Função para definir o valor de retorno para mock_open (agora __wrap_open)
void set_mock_open_return(int val);
void set_mock_write_return(ssize_t val);
void set_mock_read_buffer(const char* buf, size_t count);
void set_mock_file_content_buffer(const char* buf, size_t count);

#ifdef TEST_BUILD
// Mock para realpath
extern "C" {
char *__wrap_realpath(const char *path, char *resolved_path) noexcept;
}

#define realpath __wrap_realpath
#endif // TEST_BUILD
void set_mock_realpath_return(const char *resolved_path_val);
void set_mock_realpath_fail(void);
void set_mock_realpath_sequence(const char **values, int count);
void clear_mock_realpath_sequence();

#endif // MOCK_SYSCALLS_H
