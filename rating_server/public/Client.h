#ifndef RATING_SERVER_CLIENT_H
#define RATING_SERVER_CLIENT_H

#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>
#include "Server.h"
#include "ServerMessage.h"
#include "Socket.h"
#include "ClientMessages/ClientMessage.h"
#include "ClientMessages/BaseMessage.h"

class Server;
namespace ClientMessage {
  class BaseMessage;
}

class Client {

private:
  Socket socket;
  std::string *user_name = nullptr;

public:
  explicit Client(int socket_fd);

  ~Client();

  void set_username(std::string *name);

  void operator()(Server *server);

  void shutdown_socket();

  Socket& getSocket();

  bool send_error(std::string &&message);

  bool send_success();

  ClientMessage::BaseMessage *receive();
};

#endif //RATING_SERVER_CLIENT_H
