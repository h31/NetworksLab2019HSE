
#include "../include/PaymentSystemServer.h"

Data PaymentSystemServer::data;

void PaymentSystemServer::run(int portNo) {
    int sockfd, newsockfd;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw PaymentSystemServerException("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw PaymentSystemServerException("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    std::shared_mutex mutex_;
    while(isEnabled) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        ClientHandler clientHandler(data, mutex_);
        std::thread thread(clientHandler, newsockfd);
        thread.detach();
    }
}
