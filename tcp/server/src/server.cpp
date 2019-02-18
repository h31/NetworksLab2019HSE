//
// Created by Мария on 2019-02-18.
//

#include "server.h"
#include "ReadWriteHelper.h"

#include <utility>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>



Server::Server(int port_number) {
  this->port_number = port_number;
}

ReadWriteHelper Server::readWriteHelper;

void Server::run() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        return;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        return;
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return;
        }

        handlers.push_back(ClientHandler(newsockfd, *this));
        threads.push_back(pthread_t());
        if (pthread_create(&threads.back(), nullptr, (void*) (&ClientHandler::run), handlers.back())) {
            perror("Failed to create client thread.");
            continue;
        }
    }
}

uint32_t Server::getRequest(){
    bzero(buffer, 256);
    ssize_t n = read(newsockfd, buffer, 256);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Here is the message: %s\n", buffer);
    return 1;
}

void Server::sendResponse(uint32_t sent_message_type){
    ssize_t n = write(newsockfd, "I got your message", 18);

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

}
