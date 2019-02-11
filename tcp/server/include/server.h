#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <vector>

#include <csignal>
#include <cstdint>

#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{
public:
  explicit Server(uint16_t port);

  ~Server();

  void Run();

  void Stop();
private:
  uint16_t port_;

  int sockfd_;

  sockaddr_in serv_addr_;

  sockaddr_in cli_addr_;

  std::vector<std::thread> client_threads_;

  volatile std::sig_atomic_t exit_ = 0;

  void ClientLifeCycle(int newsockfd);

  static double ProcessOperation(char operation, int left_arg, int right_arg);

  double ProcessLongOperation(char operation, int arg);
};

#endif // TCP_SERVER_H