//
// Created by machine on 18-Feb-19.
//

#ifndef RATING_SERVER_CLIENT_H
#define RATING_SERVER_C

#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>

#include "clientMessages/ClientMessage.h"
#include "serverMessages/ServerMessage.h"

class Client {

private:
  int socket_fd;
  std::string user_name;
  std::recursive_mutex user_name_mtx;

public:
  explicit Client(int socket_fd);
  Client(const Client &) = delete;
  Client(Client &&) = delete;
  Client &operator=(const Client &) = delete;
  Client &operator=(Client &&) = delete;
  ~Client();

  bool Send(const ServerMessage &message);
  bool Receive(ClientMessage **message);
  bool IsLoggedIn();
  const std::string &GetUserName();
  void SetUserName(std::string &user_name);
  void ShutdownSocket();
};

#endif //RATING_SERVER_CLIENT_H
