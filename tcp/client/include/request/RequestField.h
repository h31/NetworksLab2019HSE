#include <utility>

#ifndef CLIENT_REQUEST_FIELD_H
#define CLIENT_REQUEST_FIELD_H


#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <unistd.h>

class RequestField {

public:
    enum class Type: uint8_t {
        STRING, INT, BYTE
    };

private:
    Type type;
    uint8_t byte_value;
    uint32_t int_value;
    std::string string_value;

    bool writeInt(int socketfd) {
        uint32_t nval = htonl(int_value);
        return ::write(socketfd, &nval, sizeof(nval)) == sizeof(nval);
    }

    bool readInt(int socketfd) {
        uint32_t nval;
        bool result = ::read(socketfd, &nval, sizeof(nval)) == sizeof(nval);
        if (result) {
            int_value = ntohl(nval);
        }
        return result;
    }

    bool writeByte(int socketfd) {
        return ::write(socketfd, &byte_value, sizeof(byte_value)) == sizeof(byte_value);
    }

    bool readByte(int socketfd) {
        return ::read(socketfd, &byte_value, sizeof(byte_value)) == sizeof(byte_value);
    }

    bool writeString(int socketfd) {
        return ::write(socketfd, string_value.c_str(), string_value.size() + 1) == string_value.size() + 1;
    }

    bool readString(int socketfd) {
        string_value = std::string();
        uint8_t next_char;
        do {
            if (::read(socketfd, &next_char, sizeof(next_char)) < sizeof(next_char)) {
                return false;
            }
            if (next_char == 0) {
                break;
            }
            string_value += next_char;
        } while (next_char != 0);
        return true;
    }

public:

    explicit RequestField(Type type): type(type) { }

    explicit RequestField(uint8_t val): type(Type::BYTE) {
        byte_value = val;
    }

    explicit RequestField(uint32_t val): type(Type::INT) {
        int_value = val;
    }

    explicit RequestField(const std::string &val): type(Type::INT) {
        string_value = val;
    }

    bool write(int socketfd) {
        switch (type) {
            case Type::INT:
                return writeInt(socketfd);
            case Type::BYTE:
                return writeByte(socketfd);
            case Type::STRING:
                return writeString(socketfd);
        }
    }

    bool read(int socketfd) {
        switch (type) {
            case Type::INT:
                return readInt(socketfd);
            case Type::BYTE:
                return readByte(socketfd);
            case Type::STRING:
                return readString(socketfd);
        }
    }

    std::string getString() const {
        return type == Type::STRING ? string_value : "";
    }

    uint32_t getInt() const {
        return type == Type::INT ? int_value : 0;
    }

    uint8_t getByte() const {
        return static_cast<uint8_t>(type == Type::BYTE ? byte_value : 0);
    }
};


#endif
