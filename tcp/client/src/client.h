#pragma once
#include <string>
#include <netinet/in.h>

#include "ReadWriteHelper.h"

class Client {

  public:
    Client(char* host_name, int port_number);

    bool connect();
    bool sendRequest();
    void getResponse();

  private:
    const char* host_name;
    int port_number;
    int sockfd;
    uint8_t buffer[256];
    static ReadWriteHelper readWriteHelper;
    void addUser();
    void sendMoney();
    void requestMoney();
    void confirmRequest();
    void checkMoney();
    void checkRequests();
    void readPassword(int offset);
    void readClientWallet(int offset);
    void readWallet(int offset, std::string whoseWallet);
    void readMoney(int offset);
    void readConfirm(int offset);
    void readRequestId(int offset);
    void addUserResponse();
    uint64_t longNumberResponse(int offset);
    uint32_t shortNumberResponse(int offset);
    void getAllWalletsResponse();
    void successResponse();
    void checkRequestsResponse();
};

