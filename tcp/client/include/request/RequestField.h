#ifndef CLIENT_REQUEST_FIELD_H
#define CLIENT_REQUEST_FIELD_H


#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include "SocketReader.h"

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

    bool writeInt(int socketfd);
    bool readInt(SocketReader &reader);
    bool writeByte(int socketfd);
    bool readByte(SocketReader &reader);
    bool writeString(int socketfd);
    bool readString(SocketReader &reader);

public:
    explicit RequestField(Type type);
    explicit RequestField(uint8_t val);
    explicit RequestField(uint32_t val);
    explicit RequestField(const std::string &val);
    bool write(int socketfd);
    bool read(SocketReader &reader);
    std::string getString() const;
    uint32_t getInt() const;
    uint8_t getByte() const;
};


#endif
