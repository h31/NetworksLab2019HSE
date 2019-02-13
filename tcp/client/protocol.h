#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace Protocol {
    enum class OperationType {
        REGISTRATION = 0,
        NEW_MESSAGE = 1,
        MESSAGE_CONFIRMATION = 2,
        DISCONNECTION = 3
    };


    class ClientHeader {
    public:
        ClientHeader() = default;
        ClientHeader(uint32_t type, const char* username);
        ~ClientHeader();

        void write_to_fd(int fd, int32_t type);
    private:
        uint32_t type;
        uint32_t username_length;
        char* username;
    };
};


#endif //CLIENT_PROTOCOL_H
