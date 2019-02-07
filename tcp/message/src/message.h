#include <utility>

//
// Created by oquechy on 2/7/19.
//

#ifndef CLIENT_LINUX_MESSAGE_H
#define CLIENT_LINUX_MESSAGE_H

#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <bitset>

class Message {
public:
    enum Type {
        UNDEFINED = -1,
        SUCCESS = 0,
    };

    Message(Type type, std::string message) : type(type), body(std::move(message)) {}

    bool write(int sockfd) {
        size_t message_len = 2 * sizeof(uint32_t) + body.size();
        char *data = new char[message_len];
        put_int32(type, data);
        put_int32(static_cast<uint32_t>(body.size()), data + sizeof(uint32_t));
        put_body(data + 2 * sizeof(uint32_t));

        for (ssize_t written = 0; message_len; written = ::write(sockfd, data, message_len)) {
            if (written < 0) {
                std::cerr << strerror(errno) << std::endl;
                return false;
            } else {
                data += written;
                message_len -= written;
            }
        }

        return true;
    }

    static Message read(int sockfd) {
        uint32_t type, length;
        std::string data;
        if (get_int32(&type, sockfd) &&
            get_int32(&length, sockfd) &&
            get_body(&data, length, sockfd)) {
            return Message(Type(type), data);
        }
        return Message();
    }

    Type type;
    std::string body;

private:
    Message() : type(UNDEFINED), body() {}

    static void put_int32(uint32_t i, char *buf) {
        i = htonl(i);
        memcpy(buf, &i, sizeof(uint32_t));
    }

    void put_body(char *buf) {
        memcpy(buf, body.c_str(), body.size());
    }

    static bool get_int32(uint32_t *i, int sockfd) {
        if (get(reinterpret_cast<char *>(i), sizeof(uint32_t), sockfd)) {
            *i = ntohl(*i);
            return true;
        }
        return false;
    }

    static bool get_body(std::string *body, size_t length, int sockfd) {
        char *buf = new char[length];
        if (get(buf, length, sockfd)) {
            *body = std::string(buf);
            return true;
        }
        return false;
    }

    static bool get(char *dst, size_t message_len, int sockfd) {
        for (ssize_t read = 0; message_len; read = ::read(sockfd, dst, message_len)) {
            if (read < 0) {
                std::cerr << strerror(errno) << std::endl;
                return false;
            } else {
                dst += read;
                message_len -= read;
            }
        }
        return true;
    }
};


#endif //CLIENT_LINUX_MESSAGE_H
