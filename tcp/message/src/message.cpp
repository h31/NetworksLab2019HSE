#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

#include "../include/message.h"

Message::Message() : type(UNDEFINED), body() {}

Message::Message(Message::Type type) : type(type), body() {}

Message::Message(Message::Type type, std::string message) : type(type), body(std::move(message)) {}

bool Message::Write(int sockfd) {
    size_t message_len = 2 * sizeof(uint32_t) + body.size();
    auto data = new char[message_len];
    PutInt32(type, data);
    PutInt32(static_cast<uint32_t>(body.size()), data + sizeof(uint32_t));
    PutBody(data + 2 * sizeof(uint32_t));

    ssize_t written = ::write(sockfd, data, message_len);
    if (written < 0) {
        std::cerr << strerror(errno) << std::endl;
        delete[] data;
        return false;
    }

    delete[] data;
    return true;
}

Message Message::Read(int sockfd) {
    uint32_t type, length;
    std::string data;
    if (GetInt32(&type, sockfd) &&
        GetInt32(&length, sockfd) &&
        (!length || GetBody(&data, length, sockfd))) {
        return Message(Type(type), data);
    }
    return Message();
}

void Message::PutInt32(uint32_t i, char *buf) {
    i = htonl(i);
    memcpy(buf, &i, sizeof(uint32_t));
}

void Message::PutBody(char *buf) {
    memcpy(buf, body.c_str(), body.size());
}

bool Message::GetInt32(uint32_t *i, int sockfd) {
    if (Get(reinterpret_cast<char *>(i), sizeof(uint32_t), sockfd)) {
        *i = ntohl(*i);
        return true;
    }
    return false;
}

bool Message::GetBody(std::string *body, size_t length, int sockfd) {
    auto buf = new char[length];
    if (Get(buf, length, sockfd)) {
        *body = std::string(buf);
        delete[] buf;
        return true;
    }
    delete[] buf;
    return length != 0;
}

bool Message::Get(char *dst, size_t message_len, int sockfd) {
    for (ssize_t read = ::read(sockfd, dst, message_len); message_len; read = ::read(sockfd, dst, message_len)) {
        if (read <= 0) {
            return false;
        } else {
            dst += read;
            message_len -= read;
        }
    }
    return true;
}
