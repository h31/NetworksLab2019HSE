#include "ReadWriteHelper.h"

uint64_t ReadWriteHelper::get8Bytes(uint8_t *buffer, int offset) {
    uint64_t x = 0;
    memcpy(&x, buffer + offset, 8);
    return x;
}

uint32_t ReadWriteHelper::get4Bytes(uint8_t *buffer, int offset) {
    uint32_t x = 0;
    memcpy(&x, buffer + offset, 4);
    return x;
}

uint8_t ReadWriteHelper::getByte(uint8_t *buffer, int offset) {
    return buffer[offset];
}


void ReadWriteHelper::set4Bytes(uint8_t *buffer, int offset, uint32_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
}

void ReadWriteHelper::set8Bytes(uint8_t *buffer, int offset, uint64_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
}


void ReadWriteHelper::setByte(uint8_t *buffer, int offset, uint32_t value) {
    memcpy(buffer + offset, &value, sizeof(value));
}