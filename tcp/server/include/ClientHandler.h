

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
#include "ReadWriteHelper.h"
#include "Data.h"

struct Response {
    uint8_t *buffer;
    uint32_t count;

    Response(uint8_t *buffer, uint32_t count) : buffer(buffer), count(count) {};
};

class ClientHandler {
public:
    explicit ClientHandler(Data &data1,
                           std::shared_mutex &mutex_,
                           std::shared_mutex &historyMutex) : data(data1),
                                                              mutex_(mutex_),
                                                              historyMutex(historyMutex) {};

    void operator()(int clientSocketFd);

private:
    static ReadWriteHelper readWriteHelper;

    Response addWallet(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    Response getWalletNumbers(uint32_t type);

    Response sendTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    Response requestTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    Response acceptTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    Response getWalletInfo(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    Response getRequests(uint8_t *inputBuffer, uint32_t type, ssize_t countRead);

    uint32_t transfer(uint64_t from, uint64_t to, std::string passwordFrom, uint64_t count);
    std::shared_mutex &mutex_;
    std::shared_mutex &historyMutex;
    Data &data;

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
