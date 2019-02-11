#include <cstring>
#include "message.h"

Message::Message(uint32_t type, const char *content) : type(type) {
    const size_t size = strlen(content);
    this->content = new char[size];
    strcpy(this->content, content);
}

Message::Message() : Message(0) {}

Message::Message(uint32_t type) : type(type), content(nullptr) {}

Message::~Message() {
    delete [] this->content;
}
