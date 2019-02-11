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
        UNDEFINED = 0,
        UNAUTHORIZED = 1,
        INCORRECT_MESSAGE = 2,
        NEW_PLAYER = 100,
        PLAYER_ADDED = 101,
        CANT_ADD_PLAYER = 102,
        NEW_CROUPIER = 200,
        CROUPIER_ADDED = 201,
        CANT_ADD_CROUPIER = 202,
        CROUPIER_ALREADY_EXISTS = 203,
        NEW_BET = 300,
        BET_ACCEPTED = 301,
        REPEATED_BET = 302,
        UNTIMELY_BET = 303,
        START_DRAW = 400,
        DRAW_STARTED = 401,
        CANT_START_DRAW = 402,
        END_DRAW = 500,
        DRAW_ENDED = 501,
        CANT_END_DRAW = 502,
        DRAW_RESULTS = 600,
        GET_ALL_BETS = 700,
        LIST_OF_BETS = 701
    };

    Type type;
    std::string body;

    Message();

    explicit Message(Type type);

    Message(Type type, std::string message);

    bool Write(int sockfd);

    static Message Read(int sockfd);

 private:

    static void PutInt32(uint32_t i, char* buf);

    void PutBody(char* buf);

    static bool GetInt32(uint32_t* i, int sockfd);

    static bool GetBody(std::string* body, size_t length, int sockfd);

    static bool Get(char* dst, size_t message_len, int sockfd);
};


#endif //CLIENT_LINUX_MESSAGE_H
