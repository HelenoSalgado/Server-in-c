#include <stddef.h>
#include <stdlib.h>

static const char* mock_read_buffer = NULL;
static size_t mock_read_count = 0;

void set_mock_read_buffer(const char* buf, size_t count) {
    mock_read_buffer = buf;
    mock_read_count = count;
}