//
// Created by Мария on 2019-02-18.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <netinet/in.h>
#include "ReadWriteHelper.h"
#include <vector>
#include "ClientHandler.h"
#include <mutex>

class Server {
  public:
    Server(int port_number);
    void run();
    uint32_t getRequest();
    void sendResponse(uint32_t sent_message_type);

  private:
    int sockfd, newsockfd;
    int port_number;
    unsigned int clilen;
    uint8_t buffer[256];
    std::vector<pthread_t> threads;
    std::vector<ClientHandler> handlers;
    std::vector<int> sockfds;
    std::mutex mutex;

    struct sockaddr_in serv_addr, cli_addr;
    static ReadWriteHelper readWriteHelper;
};


#endif //SERVER_SERVER_H
