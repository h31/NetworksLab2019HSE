#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace Protocol {
    enum class ClientOperationTypes {
        REGISTER = 0,
        SEND_MESSAGE = 1,
        RECIEVED_MESSAGE = 2,
        DISCONNECT = 3
    };

    enum class ServerOperationTypes {
        REGISTERED = 0,
        SEND_MESSAGE = 1,
        RECIEVED_MESSAGE = 2,
        DISCONNECT = 3
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

    class ServerHeader {
    public:
        ServerHeader() = default;
        ServerHeader(uint32_t type) : type(type) {};

        void read_from_fd(int fd);
        uint32_t get_type() {
            return type;
        }
    private:
        uint32_t type = 5;
    };
};


#endif //CLIENT_PROTOCOL_H
