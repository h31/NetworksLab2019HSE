#ifndef SERVER_CLIENT_MESSAGE_H
#define SERVER_CLIENT_MESSAGE_H

#include <string>

#include "Server.h"

enum struct ClientMessageType : unsigned char {
    LOGIN = 0x00,
    SINGLE_MESSAGE = 0x01,
    BROADCAST_MESSAGE = 0x02
};

class ClientMessage {

protected:
    static bool ReadString(int socket_fd, std::string &dst);

public:
    virtual bool ReadBody(int socket_fd) { return true; };
    virtual bool Process(Server *server, Client *client) = 0;
};

#endif
