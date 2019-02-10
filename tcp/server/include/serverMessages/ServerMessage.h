#ifndef _SERVER_SERVER_MESSAGE_H
#define _SERVER_SERVER_MESSAGE_H

#include <unistd.h>
#include <string>

enum struct ServerMessageType : unsigned char {
    LOGIN_SUCCESS = 0xFF,
    MESSAGE = 0xFE,
    GROUP_MESSAGE = 0xFD,
    ERROR = 0xFC,
    DISCONNECT = 0xFB
};

class ServerMessage {

protected:
    ServerMessageType type;
    virtual bool WriteBody(int socket_fd) const { };
    static bool WriteString(int socket_fd, const std::string &value);

public:
    explicit ServerMessage(ServerMessageType type);
    bool Write(int socket_fd) const;
};

#endif
