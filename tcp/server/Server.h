#ifndef NETWORKS_SERVER_H
#define NETWORKS_SERVER_H


#include <cstdint>
#include <cstdio>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdlib>
#include <strings.h>
#include <thread>
#include <vector>

#include "Task.h"

class Server {
    public:
        void start(int);
        void listenSocket();
        std::string getRootDirectory();
        Server(std::string);

    private:
        int sockfd, newsockfd;
        uint16_t portno;
        struct sockaddr_in serv_addr;
        std::string directory;
};


#endif //NETWORKS_SERVER_H
