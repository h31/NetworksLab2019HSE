#include "../include/server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Server::Server(uint16_t port)
{
    port_ = port;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr_, sizeof(serv_addr_));

    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_addr.s_addr = INADDR_ANY;
    serv_addr_.sin_port = htons(port_);

    if (bind(sockfd_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }
}

void Server::Run()
{

    listen(sockfd_, 5);
    socklen_t clilen = sizeof(cli_addr_);
    while (!exit_)
    {
        int newsockfd = accept(sockfd_, (struct sockaddr *)&cli_addr_, &clilen);

        if (newsockfd < 0)
        {
            perror("ERROR on accept");
        }
    }
}

void Server::ClientLifeCycle() {

}

Server::~Server()
{
    close(sockfd_);
}