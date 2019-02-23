
#include <ReadWriteHelper.h>

#include "ReadWriteHelper.h"

uint64_t ReadWriteHelper::get8Bytes(uint8_t *buffer, int &offset) {
    uint64_t x = 0;
    memcpy(&x, buffer + offset, 8);
    offset += 8;
    return x;
}

uint32_t ReadWriteHelper::get4Bytes(uint8_t *buffer, int &offset) {
    uint32_t x = 0;
    memcpy(&x, buffer + offset, 4);
    offset += 4;
    return x;
}

uint8_t ReadWriteHelper::getByte(uint8_t *buffer, int &offset) {
    uint8_t byte = buffer[offset];
    offset++;
    return byte;

}


void ReadWriteHelper::set4Bytes(uint8_t *buffer, int &offset, uint32_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
    offset += 4;
}

void ReadWriteHelper::set8Bytes(uint8_t *buffer, int &offset, uint64_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
    offset += 8;
}


void ReadWriteHelper::setByte(uint8_t *buffer, int &offset, uint32_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
    offset++;
}

std::string ReadWriteHelper::get32Byte(uint8_t *buffer, int &offset) {
    char string[32];
    bzero(string, 32);
    int offsetForString = 0;
    for (char &i : string) {
        int curOffset = offset + offsetForString;
        i = (char) ReadWriteHelper::getByte(buffer, curOffset);
        offsetForString++;
    }
    offset += 32;
    return string;
}

void ReadWriteHelper::set32Byte(uint8_t *buffer, int &offset, std::string value) {
    char string[32];
    bzero(string, 32);
    for (int i = 0; i < value.size() && i < 32; i++) {
        string[i] = value[i];
    }
    memcpy(buffer + offset, string, 32);
    offset +=32;
}
