#include "../include/client.h"
#include "../../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>


void Client::Start() {
    // TODO

    Connect();
}

void Client::Stop() {
    Disconnect();
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
        print_message("Error sending message to user" + receiver_id ? " id" : "s.");
    }
}

void Client::process_incoming_messages() {
    while (is_running_) {
        
    }
}

Client::~Client() {
    close(sockfd_);
}

void Client::print_message(const std::string& message) {
    writing_lock_.lock();
    std::cout << message;
    writing_lock_.unlock();
}
