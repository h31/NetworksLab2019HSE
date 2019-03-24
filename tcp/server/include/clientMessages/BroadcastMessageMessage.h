#ifndef SERVER_BROADCAST_MESSAGE_MESSAGE_H
#define SERVER_BROADCAST_MESSAGE_MESSAGE_H

#include "SocketReader.h"
#include "ClientMessage.h"

class BroadcastMessageMessage : public ClientMessage {

private:
    std::string message;

public:
    bool ReadBody(SocketReader &reader) override;
    bool Process(Server *server, Client *client) override;
};

#endif
