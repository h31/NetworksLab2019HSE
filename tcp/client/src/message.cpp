#include <cstring>
#include "message.h"

Message::Message(uint32_t type, const char *content) : type(type) {
    this->content = content;
}

Message::Message() : Message(0) {}

Message::Message(uint32_t type) : type(type), content("") {}