#include <vector>
#include "Server.h"

void Server::start(int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
}

void Server::listenSocket() {
    listen(sockfd, 5);
    std::vector<std::thread> clients;
    std::thread killer(clientKiller);
    while (true) {
        sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        clients.emplace_back(clientWork, newsockfd, getRootDirectory());
    }
}

std::string Server::getRootDirectory() {
    return this->directory;
}

Server::Server(std::string root_dir) {
    directory = root_dir;
}
