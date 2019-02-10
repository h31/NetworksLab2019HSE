#ifndef SERVER_BROADCAST_MESSAGE_MESSAGE_H
#define SERVER_BROADCAST_MESSAGE_MESSAGE_H

#include "ClientMessage.h"

class BroadcastMessageMessage : public ClientMessage {

private:
    std::string message;

public:
    void ReadBody(int socket_fd) override;
    void Process(Server *server, Client *client) override;
};

#endif
