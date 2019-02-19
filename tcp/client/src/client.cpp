#include "../include/client.h"
#include "../../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>


void Client::Start() {
    stop_ = 0;
    messages_thread_ = std::thread(&Client::print_messages, this);

    // TODO

    Connect();
}

void Client::Stop() {
    Disconnect();
    stop_ = 1;
    messages_thread_.join();
}

void Client::Connect() {
    if (is_running_)
        return;
    is_running_ = 1;
    WriteRequestMessage(sockfd_, RequestMessage::CONNECT());
    main_thread_ = std::thread(&Client::process_incoming_messages, *this);
}

void Client::Disconnect() {
    if (!is_running_)
        return;
    is_running_ = 0;
    WriteRequestMessage(sockfd_, RequestMessage::DISCONNECT());
    main_thread_.join();
}

void Client::SendMessageToAll(const std::string& message) const {
    SendMessageToOne(0, message);
}

void Client::SendMessageToOne(int32_t receiver_id, const std::string& message) const {
    RequestMessage request_message(receiver_id, message);
    if (!WriteRequestMessage(sockfd_, request_message)) {
        std::cerr << "Error sending message to user" << receiver_id ? " id" : "s.";
    }
}

void Client::process_incoming_messages() {
    while (is_running_) {
        // TODO
    }
}

void Client::print_messages() const {
    while (!exit) {
        
    }
}

Client::~Client() {
    close(sockfd_);
}
