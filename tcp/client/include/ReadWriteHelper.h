
#ifndef CLIENT_READWRITEHELPER_H
#define CLIENT_READWRITEHELPER_H

#include <cstdint>
#include <cstring>
#include <string>

class ReadWriteHelper {
public:
    static uint64_t get8Bytes(uint8_t *buffer, size_t bufferSize, int &offset);

    static uint32_t get4Bytes(uint8_t *buffer, size_t bufferSize, int &offset);

    static void set4Bytes(uint8_t *buffer, size_t bufferSize, int &offset, uint32_t value);

    static void set8Bytes(uint8_t *buffer, size_t bufferSize, int &offset, uint64_t value);

    static uint8_t getByte(uint8_t *buffer, size_t bufferSize, int &offset);

    static void setByte(uint8_t *buffer, size_t bufferSize, int &offset, uint8_t value);

    static std::string get32Byte(uint8_t *buffer, size_t bufferSize, int &offset);

    static void set32Byte(uint8_t *buffer, size_t bufferSize, int &offset, std::string value);
};

#endif //CLIENT_READWRITEHELPER_H
