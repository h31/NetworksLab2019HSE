#ifndef FILE_SYSTEM_MESSAGE_H
#define FILE_SYSTEM_MESSAGE_H


#include <cstdint>

class Message {
public:
    uint32_t type;
    char* content;

    Message();

    explicit Message(uint32_t type);

    Message(uint32_t type, const char *content);

    ~Message();

};


#endif //FILE_SYSTEM_MESSAGE_H
