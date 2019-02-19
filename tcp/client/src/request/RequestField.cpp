#include "request/RequestField.h"

bool RequestField::writeInt(int socketfd) {
    uint32_t nval = htonl(int_value);
    return ::write(socketfd, &nval, sizeof(nval)) == sizeof(nval);
}

bool RequestField::readInt(int socketfd) {
    uint32_t nval;
    bool result = ::read(socketfd, &nval, sizeof(nval)) == sizeof(nval);
    if (result) {
        int_value = ntohl(nval);
    }
    return result;
}

bool RequestField::writeByte(int socketfd) {
    return ::write(socketfd, &byte_value, sizeof(byte_value)) == sizeof(byte_value);
}

bool RequestField::readByte(int socketfd) {
    return ::read(socketfd, &byte_value, sizeof(byte_value)) == sizeof(byte_value);
}

bool RequestField::writeString(int socketfd) {
    return ::write(socketfd, string_value.c_str(), string_value.size() + 1) == string_value.size() + 1;
}

bool RequestField::readString(int socketfd) {
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

RequestField::RequestField(RequestField::Type type) : type(type) { }

RequestField::RequestField(uint8_t val) : type(Type::BYTE) {
    byte_value = val;
}

RequestField::RequestField(uint32_t val) : type(Type::INT) {
    int_value = val;
}

RequestField::RequestField(const std::string &val) : type(Type::STRING) {
    string_value = val;
}

bool RequestField::write(int socketfd) {
    switch (type) {
        case Type::INT:
            return writeInt(socketfd);
        case Type::BYTE:
            return writeByte(socketfd);
        case Type::STRING:
            return writeString(socketfd);
    }
}

bool RequestField::read(int socketfd) {
    switch (type) {
        case Type::INT:
            return readInt(socketfd);
        case Type::BYTE:
            return readByte(socketfd);
        case Type::STRING:
            return readString(socketfd);
    }
}

std::string RequestField::getString() const {
    return type == Type::STRING ? string_value : "";
}

uint32_t RequestField::getInt() const {
    return type == Type::INT ? int_value : 0;
}

uint8_t RequestField::getByte() const {
    return static_cast<uint8_t>(type == Type::BYTE ? byte_value : 0);
}
