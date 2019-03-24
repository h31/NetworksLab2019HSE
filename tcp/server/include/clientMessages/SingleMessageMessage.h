#ifndef SERVER_SINGLE_MESSAGE_MESSAGE_H
#define SERVER_SINGLE_MESSAGE_MESSAGE_H

#include "SocketReader.h"
#include "ClientMessage.h"

class SingleMessageMessage : public ClientMessage {

private:
    std::string user_name;
    std::string message;

public:
    bool ReadBody(SocketReader &reader) override;
    bool Process(Server *server, Client *client) override;
};

#endif
