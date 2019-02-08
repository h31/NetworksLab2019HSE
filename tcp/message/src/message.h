#include <utility>

#ifndef CLIENT_LINUX_MESSAGE_H
#define CLIENT_LINUX_MESSAGE_H

#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>



class Message {
 public:
    enum Type {
        UNDEFINED = -1,
        SUCCESS = 0,
        UNAUTHORIZED = 1,
        INCORRECT_MESSAGE = 2,
        NEW_PLAYER = 100,
        CANT_ADD_PLAYER = 101,
        NEW_CROUPIER = 200,
        CANT_ADD_CROUPIER = 201,
        CROUPIER_ALREADY_EXISTS = 202,
        NEW_BET = 300,
        REPEATED_BET = 301,
        UNTIMELY_BET = 302,
        START_DRAW = 400,
        CANT_START_DRAW = 401,
        END_DRAW = 500,
        CANT_END_DWAW = 501,
        DRAW_RESULTS = 600,
        GET_ALL_BETS = 700,
        LIST_OF_BETS = 800
    };

    Message() : type(UNDEFINED), body() {}  // I dont like -1 because we cast to uint then.

    explicit Message(Type type) : type(type), body() {}

    Message(Type type, std::string message) : type(type), body(std::move(message)) {}

    bool Write(int sockfd);

    static Message Read(int sockfd);

    Type type;
    std::string body;

 private:

    static void PutInt32(uint32_t i, char* buf);

    void PutBody(char* buf);

    static bool GetInt32(uint32_t* i, int sockfd);

    static bool GetBody(std::string* body, size_t length, int sockfd);

    static bool Get(char* dst, size_t message_len, int sockfd);
};


#endif //CLIENT_LINUX_MESSAGE_H
