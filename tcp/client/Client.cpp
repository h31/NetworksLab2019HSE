#include "Client.h"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fcntl.h>
#include "protocol.h"

int fd_set_blocking(int fd, int blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}

Client Client::create_client(const char *hostname, uint16_t port, const char *username) {
    struct hostent* host = gethostbyname(hostname);
    struct sockaddr_in serv_addr{};

    /* Create socket. */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Validate host */
    if (host == nullptr) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(host->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) host->h_length);
    serv_addr.sin_port = htons(port);

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    /* Register username on server */
    Protocol::ClientHeader client_header = Protocol::ClientHeader(0, username);
    client_header.write_to_fd(sockfd, static_cast<int>(Protocol::ClientOperationTypes::REGISTER));
    Protocol::ServerHeader server_header = Protocol::ServerHeader();
    server_header.read_from_fd(sockfd);
    std::cout << "Server returned: " << server_header.get_type();

    //TODO

    return {sockfd, client_header};
}

Client::Client(int sockfd, const Protocol::ClientHeader header) : sockfd(sockfd), client_header(header) {}

void Client::send_message(size_t reciever_length, char* reciever, size_t message_length, char* message) {
    client_header.write_to_fd(sockfd, /* type */ 1);
    write(sockfd, (char *)reciever_length, sizeof(reciever_length));
    write(sockfd, reciever, reciever_length);
    write(sockfd, (char *)message_length, sizeof(message_length));
    write(sockfd, message, message_length);

    time_t t = std::time(nullptr);
    auto now = static_cast<int64_t> (t);
    write(sockfd, (char *) now, sizeof(now));

    processing_messages.insert(std::pair<std::string, int64_t>(std::string(reciever_length, reciever_length), now));
}

void Client::disconnect() {
    client_header.write_to_fd(sockfd, static_cast<int>(Protocol::ClientOperationTypes::DISCONNECT));
    close(sockfd);
}

bool Client::has_new_message() {
    fd_set_blocking(sockfd, true);
    int32_t operation_type = 0;
    ssize_t n = read(sockfd, (char *) &operation_type, sizeof(operation_type));
    if (n == 0)
        return false;
    fd_set_blocking(sockfd, false);

    switch (Protocol::ServerOperationTypes(operation_type)) {
        case Protocol::ServerOperationTypes::REGISTERED:
            printf("Warning: unexpected registration confirmation");
            break;
        case Protocol::ServerOperationTypes::SEND_MESSAGE:
            return true;
        case Protocol::ServerOperationTypes::RECIEVED_MESSAGE:
            break;
        case Protocol::ServerOperationTypes::DISCONNECT:
            exit(1);
    }
    return false;
}

void Client::read_message() {}
