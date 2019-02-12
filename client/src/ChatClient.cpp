//
// Created by Владислав Калинин on 11/02/2019.
//

#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <zconf.h>
#include <iostream>
#include "../include/ChatClient.h"
#include "../../common/include/constants.h"
#include "../../common/include/Message.h"

ChatClient::ChatClient(const char *hostname, uint16_t port) {
    sockaddr_in serv_addr;
    hostent *server;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(hostname);

    if (server == nullptr) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(this->sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
}

void ChatClient::registry(std::string name, std::string password) {
    std::string msg = RegistryMessage(name, password).to_json_format();
    authorization(msg);
}

void ChatClient::login(std::string name, std::string password) {
    if (this->username == "") {
        std::string msg = AuthenticationMessage(name, password).to_json_format();
        authorization(msg);
        this->username = name;
    } else {
        throw std::runtime_error("client already authentication");
    }
}

void ChatClient::authorization(std::string msg) {
    sendMessage(msg);
    char *responseBuffer = recvMessage();
    json response = json::parse(responseBuffer);
    delete[] responseBuffer;
    if (response.at(STATUS).get<int>() == STATUS_ERROR) {
        throw std::runtime_error(response.at(CAUSE).get<std::string>());
    }
}

void ChatClient::sendMessage(std::string msg) {
    std::string size = std::to_string(msg.size());
    write(sockfd, msg.c_str(), msg.size());
}

char *ChatClient::recvMessage() {
    char *buffer = new char[1024];
    int size = read(sockfd, buffer, 1024);
    buffer[size] = '\0';
    return buffer;
}

void ChatClient::sendBroadcastMessage(std::string message) {
    std::string msg = TextMessage(this->username, message).to_json_format();
    sendMessage(msg);
}

TextMessage *ChatClient::recieveMessage() {
    char *responseBuffer = recvMessage();
    json response = json::parse(responseBuffer);
    delete[] responseBuffer;
    return new TextMessage(response.at(SENDER).get<std::string>(), response.at(MESSAGE).get<std::string>());
}

void ChatClient::sendPrivateMessage(std::string reciever, std::string message) {
    std::string msg = PrivateMessage(this->username, reciever, message).to_json_format();
    sendMessage(msg);
}

void ChatClient::close_client() {
    close(sockfd);
}

void ChatClient::kickUser(std::string name) {
    std::string msg = KickMessage(name).to_json_format();
    sendMessage(msg);
}