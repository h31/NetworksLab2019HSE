//
// Created by iisuslik on 17.02.19.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "string"
#include <iostream>

#include <string.h>

const char SPLIT = '\n';

class Client {
public:
    Client(const std::string&, uint16_t port);
    void start();
    ~Client();
private:
    void workingCycle();
    std::string getRequest();
    bool sendRequest(std::string request);
    void getResponse();
    void handleResponse(std::string response);
    void printBugList(std::string list);

    std::string host;
    uint16_t port;
    int sockfd;
    bool stop = false;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    std::string login = "";
    std::string status = "";


    const std::string help = "Welcome to bug tracker \"ITrack\"!!!\n"
                       "Here is commands list:\n"
                       "regist - register in ITrack\n"
                       "auth - authorization\n"
                       "exit - exit from this programm\n"
                       "help - list of commands"
                       "More commands will appear after your authorization\n";

    const std::string helpTester = "Commands available for tester\n"
                             "closed - closed issues in ITrack\n"
                             "open - open issues in ITrack\n"
                             "add - add new issue\n"
                             "accept - accept fixed issue\n"
                             "reject - reopen closed issue\n";

    const std::string helpDev = "Commands available for developer\n"
                                "open - open issues in ITrack\n"
                                "fix - try to close issue in ITrack(to status \"fixed\")\n";
};

#endif //CLIENT_CLIENT_H
