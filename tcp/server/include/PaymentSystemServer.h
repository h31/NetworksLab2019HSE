//
// Created by Olga Alehina on 2019-02-10.
//

#ifndef SERVER_PAYMENTSYSTEMSERVER_H
#define SERVER_PAYMENTSYSTEMSERVER_H


#include <string>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "ClientHandler.h"


class PaymentSystemServer {
public:
    void run(int portNo);

private:
    bool isEnabled = true;
    static Data data;
};

class PaymentSystemServerException : public std::exception {
public:
    char *what() {
        return strdup(msg.c_str());
    }

    explicit PaymentSystemServerException(std::string msg) {
        this->msg = std::move(msg);
    }

private:
    std::string msg;
};
#endif //SERVER_PAYMENTSYSTEMSERVER_H
