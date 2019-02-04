#ifndef VACANCY_COMMON_UTIL_H
#define VACANCY_COMMON_UTIL_H

#include <cstdint>
#include <cstdio>
#include <memory>
#include "json.hpp"

namespace util {
    const uint32_t INT_SIZE = 4;

    // throwing `const char *` if socket not available
    nlohmann::json communicate(int socket, const nlohmann::json &message);
    nlohmann::json receiveMessage(int socket);
    void sendMessage(int socket, const nlohmann::json &message);

    void writeToSocket(int socket, const void *buffer, uint32_t size);
    void readFromSocket(int socket, void *buffer, uint32_t size);

    std::shared_ptr<uint8_t[]> encodeNumber(uint32_t number);
    uint32_t decodeNumber(const uint8_t * buffer);
}

#endif //VACANCY_COMMON_UTIL_H
