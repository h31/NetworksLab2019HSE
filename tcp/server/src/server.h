#pragma once

#include <string>
#include <netinet/in.h>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include "ClientHandler.h"

class Server {
  public:
    Server(int port_number);
    bool run();

  private:
    int sockfd, newsockfd;
    int port_number;
    unsigned int clilen;
    std::vector<pthread_t> threads;
    std::vector<class ClientHandler> handlers;
    std::vector<int> sockfds;
    std::shared_mutex mutex;
    uint32_t freeId;

    struct sockaddr_in serv_addr, cli_addr;
    std::unordered_map<uint32_t, struct rating> ratings;
};
