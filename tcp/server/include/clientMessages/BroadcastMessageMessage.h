#ifndef SERVER_BROADCAST_MESSAGE_MESSAGE_H
#define SERVER_BROADCAST_MESSAGE_MESSAGE_H

#include "ClientMessage.h"

class BroadcastMessageMessage : public ClientMessage {

private:
    std::string message;

public:
    bool ReadBody(int socket_fd) override;
    bool Process(Server *server, Client *client) override;
};

#endif
