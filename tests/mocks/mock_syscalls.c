#include "mock_syscalls.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Mock for open
static int open_return_value = -1;

// Mock for write
static char *write_buffer = NULL;
static size_t write_buffer_size = 0;
static ssize_t write_return_value = -1;

// Mock for read
static const char *mock_read_buffer = NULL;
static size_t mock_read_count = 0;
static const char *mock_file_content_buffer = NULL;
static size_t mock_file_content_count = 0;

// Mock for realpath
static const char *mock_realpath_return_value = NULL;
static int mock_realpath_fail_flag = 0;

// Sequence for realpath
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

int mock_open(const char *pathname, int flags, ...) {
    (void)flags;    // Unused
    // Handle mode_t for creat/open with O_CREAT
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        (void)mode; // Unused
        va_end(args);
    }
    printf("MOCK_OPEN: pathname=%s, returning %d\n", pathname, open_return_value);
    return open_return_value;
}

ssize_t mock_write(int fd, const void *buf, size_t count) {
    (void)fd; // Unused
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
    printf("MOCK_WRITE: fd=%d, count=%zu, returning %zd\n", fd, count, (write_return_value != -1) ? write_return_value : (ssize_t)count);
    return (write_return_value != -1) ? write_return_value : (ssize_t)count;
}

ssize_t mock_read(int fd, void *buf, size_t count) {
    // Handle client_fd (1) specifically
    if (fd == 1) {
        size_t bytes_to_copy = (count < mock_read_count) ? count : mock_read_count;
        if (mock_read_buffer && buf) {
            memcpy(buf, mock_read_buffer, bytes_to_copy);
            printf("MOCK_READ (client_fd): fd=%d, count=%zu, copied=%zu\n", fd, count, bytes_to_copy);
            return (ssize_t)bytes_to_copy;
        }
    }
    // Handle file content reads
    else if (mock_file_content_buffer && buf) { // This assumes file content reads are not on fd 1
        size_t bytes_to_copy = (count < mock_file_content_count) ? count : mock_file_content_count;
        memcpy(buf, mock_file_content_buffer, bytes_to_copy);
        printf("MOCK_READ (file_content): fd=%d, count=%zu, copied=%zu\n", fd, count, bytes_to_copy);
        return (ssize_t)bytes_to_copy;
    }
    // For any other fd (including 0 for stdin), return EOF immediately
    printf("MOCK_READ: fd=%d, returning 0 (EOF)\n", fd);
    return 0;
}

void set_mock_realpath_return(const char *resolved_path_val) {
    mock_realpath_return_value = resolved_path_val;
    mock_realpath_fail_flag = 0;
    clear_mock_realpath_sequence(); // Clear sequence if single return is set
}

void set_mock_realpath_fail(void) {
    mock_realpath_fail_flag = 1;
    mock_realpath_return_value = NULL;
    clear_mock_realpath_sequence(); // Clear sequence if single return is set
}

char *mock_realpath(const char *path, char *resolved_path) {
    printf("MOCK_REALPATH: path=%s\n", path);
    if (realpath_sequence_count > 0 && realpath_sequence_index < realpath_sequence_count) {
        const char *current_return = realpath_sequence[realpath_sequence_index++];
        if (current_return == NULL) {
            printf("MOCK_REALPATH: returning NULL from sequence\n");
            return NULL;
        }
        strncpy(resolved_path, current_return, 1023);
        resolved_path[1023] = '\0';
        printf("MOCK_REALPATH: returning %s from sequence\n", resolved_path);
        return resolved_path;
    }

    if (mock_realpath_fail_flag) {
        printf("MOCK_REALPATH: returning NULL due to fail flag\n");
        return NULL;
    }
    if (mock_realpath_return_value) {
        strncpy(resolved_path, mock_realpath_return_value, 1023);
        resolved_path[1023] = '\0';
        printf("MOCK_REALPATH: returning %s from single return value\n", resolved_path);
        return resolved_path;
    }
    // Fallback to actual realpath if not mocked
    printf("MOCK_REALPATH: returning NULL (no mock set)\n");
    return NULL; // Always return NULL if not explicitly mocked
}

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
            mock_realpath_fail_flag = 0; // Disable single fail flag
            mock_realpath_return_value = NULL; // Disable single return value
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