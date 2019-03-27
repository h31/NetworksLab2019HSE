//
// Created by Мария on 2019-02-18.
//

#include "server.h"



Server::Server(int port_number) {
  this->port_number = port_number;
}

bool Server::run() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        return false;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        return false;
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return false;
        }

        handlers.emplace_back(ClientHandler(&mutex, newsockfd, &ratings, &freeId));
        threads.push_back(pthread_t());
        if (pthread_create(&threads.back(), nullptr, ClientHandler::staticFunction, &handlers.back())) {
            perror("Failed to create thread.");
            continue;
        }
    }
}
