#ifndef COMMON_COMMUNICATION_COMMUNICATION_H
#define COMMON_COMMUNICATION_COMMUNICATION_H

#include <cstdint>
#include <cstdio>
#include "model/request.h"
#include "model/response.h"


namespace util {
    using encoded_message = std::pair<uint32_t, std::shared_ptr<uint8_t[]>>;

    uint32_t readMessageSize(int socket);

    // throwing `const char *` if socket not available
    void readToBuffer(int socket, uint8_t *buffer, size_t size);
    void writeToSocket(int socket, encoded_message message);

    void sendResponse(int socket, std::unique_ptr<response::Response> &&response);
    void sendRequest(int socket, std::unique_ptr<request::Request> &&request);

    std::unique_ptr<uint8_t[]> receiveMessage(int socket);
}

#endif //COMMON_COMMUNICATION_COMMUNICATION_H
