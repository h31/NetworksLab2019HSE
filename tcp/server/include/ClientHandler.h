

//
// Created by Olga Alehina on 2019-02-10.
//

#ifndef SERVER_CLIENTHANDLER_H
#define SERVER_CLIENTHANDLER_H

#include <string>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include "Data.h"

struct Response {
    uint8_t *buffer;
    uint32_t count;

    Response(uint8_t *buffer, uint32_t count) : buffer(buffer), count(count) {};
};

class ClientHandler {
public:
    explicit ClientHandler(Data &data1, std::shared_mutex &mutex_) : data(data1),
                                                                     mutex_(mutex_) {};

    void operator()(int clientSocketFd);

private:
    Response getWalletNumbers(int clientSocketFd, uint32_t type);
    Response addWallet(int clientSocketFd, uint32_t type);
    std::shared_mutex &mutex_;
    Data data;
};


class ClientHandlerException : public std::exception {
public:
    char *what() {
        return strdup(msg.c_str());
    }

    explicit ClientHandlerException(std::string msg) {
        this->msg = std::move(msg);
    }

private:
    std::string msg;
};

#endif //SERVER_CLIENTHANDLER_H
