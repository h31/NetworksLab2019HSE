#include "../include/client.h"
#include "../../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h>

#include <iostream>


Client::Client(const Client& other) {
    host_  = other.host_;
    port_ = other.port_;
}

bool Client::Start() {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    if (sockfd_ < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server_ = gethostbyname(host_.c_str());

    if (server_ == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server_->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server_->h_length);
    serv_addr.sin_port = htons(port_);

    if (connect(sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        print_message("Error connecting to the server");
        return false;
    }

    Connect();
    return true;
}

void Client::Connect() {
    if (is_running_)
        return;
    connection_lock_.lock();
    is_running_ = 1;
    WriteRequestMessage(sockfd_, RequestMessage::CONNECT());
    main_thread_ = std::thread(&Client::process_incoming_messages, *this);
    connection_lock_.unlock();
}

void Client::Disconnect() {
    if (!is_running_)
        return;
    connection_lock_.lock();
    is_running_ = 0;
    WriteRequestMessage(sockfd_, RequestMessage::DISCONNECT());
    main_thread_.join();
    connection_lock_.unlock();
}

void Client::SendMessageToAll(const std::string& message) {
    SendMessageToOne(0, message);
}

void Client::SendMessageToOne(int32_t receiver_id, const std::string& message) {
    RequestMessage request_message(receiver_id, message);
    if (!WriteRequestMessage(sockfd_, request_message)) {
        print_message("Error sending message to user" + receiver_id ? std::to_string(receiver_id) + "." : "s.");
    }
}

void Client::process_incoming_messages() {
    while (is_running_) {
        try {
            ResponseMessage response = ReadResponseMessage(sockfd_);
            if (response.GetType() == ResponseType::CONNECT_OK) {
                print_message("Connected to " + host_ + ":" + std::to_string(port_)
                 + " , your id: " + std::to_string(response.GetSenderId()));
            } else if (response.GetType() == ResponseType::CONNECT_FAIL) {
                print_message("Can't establish a connection with server.");
            } else {
                print_message(std::to_string(response.GetSenderId()) + ": " + response.GetBody());
            }
        } catch (...) {
            print_message("Error receiving message from server.");
        }
    }
}

Client::~Client() {
    close(sockfd_);
    delete server_;
}

void Client::print_message(const std::string& message) {
    writing_lock_.lock();
    std::cout << message;
    writing_lock_.unlock();
}

void Client::SetHostname(const std::string& hostname) {
    host_ = hostname;
}

void Client::SetPort(uint16_t port) {
    port_ = port;
}