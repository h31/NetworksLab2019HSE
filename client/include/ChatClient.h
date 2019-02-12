//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_CHATCLIENT_H
#define NETWORKS_CHATCLIENT_H


#include "../../common/include/Message.h"

class ChatClient {
public:
    ChatClient(const char *hostname, uint16_t port);

    void registry(std::string name, std::string password);

    void login(std::string name, std::string password);

    void sendBroadcastMessage(std::string message);

    void sendPrivateMessage(std::string reciever, std::string message);

    void kickUser(std::string name);

    void close_client();

    TextMessage *recieveMessage();

private:
    int sockfd;
    std::string username = "";

    void authorization(std::string msg);

    void sendMessage(std::string msg);

    char *recvMessage();
};


#endif //NETWORKS_CHATCLIENT_H
