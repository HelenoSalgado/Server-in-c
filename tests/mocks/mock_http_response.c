#include "mock_http_response.h"
#include <string.h>

static int mock_httpResponse_return_fd_val = -1;
static const char* mock_httpResponse_status = NULL;
static const char* mock_httpResponse_mime = NULL;

int mock_httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size) {
    (void)path; // Unused

    if (mock_httpResponse_status) {
        strncpy(status_msg, mock_httpResponse_status, status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';
    }
    if (mock_httpResponse_mime) {
        strncpy(mimeType, mock_httpResponse_mime, mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
    }

    return mock_httpResponse_return_fd_val;
}

void set_mock_httpResponse_return_fd(int fd) {
    mock_httpResponse_return_fd_val = fd;
}

void set_mock_httpResponse_status_and_mime(const char* status, const char* mime) {
    mock_httpResponse_status = status;
    mock_httpResponse_mime = mime;
}
