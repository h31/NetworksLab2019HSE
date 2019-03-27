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
        NEW_CUSTOMER = 100,
        CUSTOMER_ADDED = 101,
        CANT_ADD_CUSTOMER = 102,
        NEW_FREELANCER = 200,
        FREELANCER_ADDED = 201,
        CANT_ADD_FREELANCER = 202,
        NEW_ORDER = 300,
        ORDER_ACCEPTED = 301,
        GET_MY_ORDERS = 400,
        LIST_OF_MY_ORDERS = 401,
        GET_OPEN_ORDERS = 500,
        LIST_OF_OPEN_ORDERS = 501,
        TAKE_ORDER = 600,
        TAKE_ORDER_SUCCESSFUL = 601,
        TAKE_ORDER_NOT_SUCCESSFUL = 602,
        GIVE_ORDER_TO_FREELANCER = 700,
        GIVE_ORDER_SUCCESSFUL = 701,
        GIVE_ORDER_NOT_SUCCESSFUL = 702,
        WORK_STARTED = 800,
        WORK_STARTED_SUCCESSFUL = 801,
        WORK_STARTED_NOT_SUCCESSFUL = 802,
        WORK_FINISHED = 900,
        WORK_FINISHED_SUCCESSFUL = 801,
        WORK_FINISHED_NOT_SUCCESSFUL = 802,
        WORK_ACCEPTED = 1000,
        WORK_ACCEPTED_SUCCESSFUL = 1001,
        WORK_ACCEPTED_NOT_SUCCESSFUL = 1002,
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
