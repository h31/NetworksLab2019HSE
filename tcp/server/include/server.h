#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <csignal>
#include <cstdint>

#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{
public:
  Server(uint16_t port);

  ~Server();

  void Run();

private:
  uint16_t port_;

  int sockfd_;

  sockaddr_in serv_addr_;

  sockaddr_in cli_addr_;

  volatile std::sig_atomic_t exit_ = 0;

  void ClientLifeCycle();
};

#endif // TCP_SERVER_H