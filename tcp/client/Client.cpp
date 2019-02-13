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
#include "IncomingEvent.h"

int64_t get_timestamp() {
    time_t t = std::time(nullptr);
    return static_cast<int64_t> (t);
}

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
    client_header.write_to_fd(sockfd, static_cast<int>(Protocol::OperationType::REGISTRATION));
    int32_t type = 0;
    read(sockfd, (char *) &type, sizeof(type));

    //TODO

    return {sockfd, client_header};
}

Client::Client(int sockfd, const Protocol::ClientHeader header) : sockfd(sockfd), client_header(header) {}

void Client::send_message(size_t reciever_length, const char* reciever, size_t message_length, const char* message) {
    client_header.write_to_fd(sockfd, /* type */ 1);
    write(sockfd, (char *)reciever_length, sizeof(reciever_length));
    write(sockfd, reciever, reciever_length);
    write(sockfd, (char *)message_length, sizeof(message_length));
    write(sockfd, message, message_length);

    int64_t now = get_timestamp();
    write(sockfd, (char *) now, sizeof(now));

    processing_messages.insert(std::pair<std::string, int64_t>(std::string(reciever_length, reciever_length), now));
}


IncomingEvent Client::get_incoming_event() {
    fd_set_blocking(sockfd, true);
    int32_t type = 0;
    ssize_t n = read(sockfd, (char *) &type, sizeof(type));
    if (n == 0)
        return {};
    fd_set_blocking(sockfd, false);

    auto operation_type = Protocol::OperationType(type);
    switch (operation_type) {
        case Protocol::OperationType::REGISTRATION:
            return {IncomingEventType::ERROR, "Warning: unexpected registration confirmation"};
        case Protocol::OperationType::NEW_MESSAGE:
            return {IncomingEventType::NEW_MESSAGE, read_message()};
        case Protocol::OperationType::MESSAGE_CONFIRMATION:
            return {IncomingEventType::MESSAGE_CONFIRMED, read_confirmation()};
        case Protocol::OperationType::DISCONNECTION:
            return {IncomingEventType::SERVER_DISCONNECTED, "Server disconnected"};
    }
}

void Client::shut_down() {
    close(sockfd);

}

void Client::disconnect() {
    client_header.write_to_fd(sockfd, static_cast<int>(Protocol::OperationType::DISCONNECTION));
    shut_down();
}

std::string Client::read_confirmation() {
    int32_t username_length;
    int64_t now;

    read(sockfd, (char *) &username_length, sizeof(username_length));
    char username[username_length + /* for \0 sign */1];
    read(sockfd, username, static_cast<size_t>(username_length));

    read(sockfd, (char *) &now, sizeof(now));

    auto str_username = std::string(username);

    processing_messages.erase(std::make_pair(str_username, now));

    return "Delivered message for " + std::string(str_username) + " sent at " + std::to_string(now);
}

std::string Client::read_message() {
    int32_t username_length, message_length;
    int64_t now;

    read(sockfd, (char *) &username_length, sizeof(username_length));
    char username[username_length + /* for \0 sign */1];
    read(sockfd, username, static_cast<size_t>(username_length));

    read(sockfd, (char *) &message_length, sizeof(message_length));
    char message[message_length + /* for \0 sign */1];
    read(sockfd, message, static_cast<size_t>(message_length));

    read(sockfd, (char *) &now, sizeof(now));

    write_confirmation(username_length, username, now);

    return "At " + std::to_string(now) + ": " + username + ": " + message;
}

void Client::write_confirmation(int32_t username_length, const char *username, int64_t now) {
    client_header.write_to_fd(sockfd, static_cast<int32_t>(Protocol::OperationType::MESSAGE_CONFIRMATION));

    write(sockfd, (char *) &username_length, sizeof(username_length));
    write(sockfd, username, static_cast<size_t>(username_length));
}