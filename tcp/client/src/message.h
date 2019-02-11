#ifndef FILE_SYSTEM_MESSAGE_H
#define FILE_SYSTEM_MESSAGE_H

#include <string>

class Message {
public:
    uint32_t type;
    std::string content;

    Message();

    explicit Message(uint32_t type);

    Message(uint32_t type, const char *content);
};


#endif //FILE_SYSTEM_MESSAGE_H
