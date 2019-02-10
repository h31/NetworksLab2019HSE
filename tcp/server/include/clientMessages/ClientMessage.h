#ifndef SERVER_CLIENT_MESSAGE_H
#define SERVER_CLIENT_MESSAGE_H

#include <string>
#include "serverMessages/ServerMessage.h"
#include "Server.h"

enum struct ClientMessageType : unsigned char {
    LOGIN = 0x00,
    SINGLE_MESSAGE = 0x01,
    BROADCAST_MESSAGE = 0x02
};

class ClientMessage {

protected:
    static void ReadString(int socket_fd, std::string &dst);

public:
    virtual void ReadBody(int socket_fd) { };
    virtual void Process(Server *server, Client *client) = 0;
};

#endif
