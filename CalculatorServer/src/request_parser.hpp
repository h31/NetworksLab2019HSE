#include <netinet/in.h>
#include <unistd.h>
#include <memory>
#include <cmath>
#include <sys/socket.h>
#include <cstdint>
#include <stdint-gcc.h>
#include "operations.hpp"

// arg1 -- 4 bytes
// arg2 or executation time -- 4 bytes
// opertation type -- 1 byte
const int REQUEST_SIZE = 4 + 4 + 1;

int32_t parseInt(const char *buffer, const size_t start) {
    int32_t result = buffer[start + 4 - 1];
    for (size_t i = start + 4 - 1; i > start; --i) {
        result <<= 8;
        result += buffer[i - 1];
    }
    return result;
}

using namespace operations;

operations::Operation *parseRequest(const int socket) {
    std::unique_ptr<char[]> buffer(new char[REQUEST_SIZE]);

    const ssize_t &ssize = read(socket, buffer.get(), REQUEST_SIZE);
    if (ssize < 0) {
        perror("Invalid request");
    }

    char &operationType = buffer[REQUEST_SIZE - 2];
    int32_t argument1 = parseInt(buffer.get(), 0);
    int32_t argument2 = parseInt(buffer.get(), 4);

    return createOperation(operationType, argument1, argument2);
}
