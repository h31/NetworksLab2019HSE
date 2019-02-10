#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstdint>

class Server
{
  public:
    Server(uint16_t port);

    ~Server();

    void Run();

  private:
};

#endif // TCP_SERVER_H