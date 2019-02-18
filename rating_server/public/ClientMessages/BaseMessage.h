#ifndef RATING_SERVER_CLIENTMESSAGEBASE_H
#define RATING_SERVER_CLIENTMESSAGEBASE_H


#include <Client.h>
#include <Server.h>
#include <Socket.h>

class Server;

class Client;

class Socket;

namespace ClientMessage {

  class BaseMessage {

  public:
    virtual bool read_body(Socket &socket) { return true; };

    virtual bool process(Server *server, Client *client) = 0;

    virtual ~BaseMessage() = default;
  };
}

#endif //RATING_SERVER_CLIENTMESSAGEBASE_H
