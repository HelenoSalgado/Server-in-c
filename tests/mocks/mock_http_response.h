#ifndef MOCK_HTTP_RESPONSE_H
#define MOCK_HTTP_RESPONSE_H

#include <stddef.h> // For size_t

#ifdef TEST_BUILD
// Mock function for httpResponse
int mock_httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size);
#define httpResponse mock_httpResponse
#endif // TEST_BUILD

// Functions to control mock_httpResponse behavior
void set_mock_httpResponse_return_fd(int fd);
void set_mock_httpResponse_status_and_mime(const char* status, const char* mime);

#endif // MOCK_HTTP_RESPONSE_H
