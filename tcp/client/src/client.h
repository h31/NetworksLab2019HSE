#pragma once
#include <string>
#include <netinet/in.h>

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
    char buffer[256];
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
    void addUserRerponse();
    int64_t longNumberResponse(int offset);
    int32_t shortNumberResponse(int offset);
    void getAllWalletsResponse();
    void successResponse();
    void checkRequestsResponse();
};

