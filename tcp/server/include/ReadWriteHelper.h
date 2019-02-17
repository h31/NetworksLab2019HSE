//
// Created by Olga Alehina on 2019-02-17.
//

#ifndef SERVER_READWRITEHELPER_H
#define SERVER_READWRITEHELPER_H


#include <cstdint>
#include <cstring>

class ReadWriteHelper {
public:
    uint64_t get8Bytes(uint8_t* buffer, int offset);
    uint32_t get4Bytes(uint8_t* buffer, int offset);
    void set4Bytes(uint8_t* buffer, int offset, uint32_t value);
    void set8Bytes(uint8_t* buffer, int offset, uint64_t value);
    uint8_t getByte(uint8_t* buffer, int offset);
    void setByte(uint8_t* buffer, int offset, uint32_t value);
};


#endif //SERVER_READWRITEHELPER_H
