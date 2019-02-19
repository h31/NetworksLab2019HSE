#include "../include/server.h"
#include "../../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Server::Server(uint16_t port)
{
    port_ = port;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    sender_thread_ = std::thread(&Server::SenderLifeCycle, this);

    if (sockfd_ < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr_, sizeof(serv_addr_));

    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_addr.s_addr = INADDR_ANY;
    serv_addr_.sin_port = htons(port_);

    if (bind(sockfd_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }
}

void Server::Run()
{
    listen(sockfd_, 10); // could be changed
    socklen_t clilen = sizeof(cli_addr_);
    while (!exit_)
    {
        int newsockfd = accept(sockfd_, (struct sockaddr *)&cli_addr_, &clilen);

        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            continue;
        }

        clients_lock.lock();

        int id = last_client_id_++;
        client_sockets_.insert(std::pair<int, int>(id, newsockfd));
        clients_lock.unlock();
        client_threads_.emplace_back(&Server::ClientLifeCycle, this, newsockfd, id);
    }
}

void Server::Stop()
{
    shutdown(sockfd_, SHUT_RD);
    exit_ = 1;
}

void Server::ClientLifeCycle(int newsockfd, int id)
{
    while (true)
    {
        try {
            RequestMessage message = ReadRequestMessage(newsockfd);

            RequestType type = message.GetType();
            switch (type) {
                case RequestType::CONNECT: {
                    SendSuccessMessage(id);
                    break;
                }
                case RequestType::DISCONNECT: {
                    clients_lock.lock();
                    client_sockets_.erase(client_sockets_.find(id));
                    clients_lock.unlock();
                    shutdown(newsockfd, SHUT_RD);
                    close(newsockfd);
                    return;
                }
                case RequestType::NEW_MESSAGE: {
                    SendMessage(id, message);
                }
            }
        } catch (...) {
            continue;
        }
    }
}

void Server::SendMessage(int sender, RequestMessage message) {
    queue_lock.lock();
    message_queue_.push(InternalMessage(
                                message.GetReceiverId(),
                                ResponseMessage(sender, message.GetBody())
                        )
                    );
    queue_lock.unlock();
}

void Server::SenderLifeCycle() {
    while (true) {
        queue_lock.lock();
        if (message_queue_.empty()) {
            queue_lock.unlock();
            continue;
        }
        InternalMessage message = message_queue_.front();
        message_queue_.pop();
        queue_lock.unlock();

        if (message.GetReceiver() != 0) {
            clients_lock.lock();
            auto it = client_sockets_.find(message.GetReceiver());
            if (it == client_sockets_.end()) {
                clients_lock.unlock();
                continue;
            }
            int sockfd = it->second;
            clients_lock.unlock();

            WriteResponseMessage(sockfd, message.GetResponseMessage());
        } else {
            clients_lock.lock();
            for (auto it: client_sockets_) {
                WriteResponseMessage(it.second, message.GetResponseMessage());
            }
            clients_lock.unlock();
        }
    }
}

Server::~Server()
{
    for (auto &thread : client_threads_)
    {
        thread.join();
    }
    sender_thread_.join();
    close(sockfd_);
}

void Server::SendSuccessMessage(int id) {
    queue_lock.lock();
    message_queue_.push(InternalMessage(id, ResponseMessage(ResponseMessage::CONNECT_OK())));
    queue_lock.unlock();
}


int InternalMessage::GetReceiver() {
    return receiver_;
}

ResponseMessage InternalMessage::GetResponseMessage() {
    return response_message_;
}
