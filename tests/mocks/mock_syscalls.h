#ifndef MOCK_SYSCALLS_H
#define MOCK_SYSCALLS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

#ifdef TEST_BUILD
// Mock function for open
int mock_open(const char *pathname, int flags, ...);
#define open mock_open

// Mock function for write
ssize_t mock_write(int fd, const void *buf, size_t count);
#define write mock_write

// Mock function for read
ssize_t mock_read(int fd, void *buf, size_t count);
#define read mock_read
#endif // TEST_BUILD

// Function to get the last written buffer
const char* get_last_write_buffer();
size_t get_last_write_count();
void clear_mock_write_buffer();

// Function to set the return value for mock_open
void set_mock_open_return(int val);
void set_mock_write_return(ssize_t val);
void set_mock_read_buffer(const char* buf, size_t count);
void set_mock_file_content_buffer(const char* buf, size_t count);

#ifdef TEST_BUILD
// Mock for realpath
char *mock_realpath(const char *path, char *resolved_path);
#define realpath mock_realpath
#endif // TEST_BUILD
void set_mock_realpath_return(const char *resolved_path_val);
void set_mock_realpath_fail(void);
void set_mock_realpath_sequence(const char **values, int count);
void clear_mock_realpath_sequence(void);

#endif // MOCK_SYSCALLS_H
