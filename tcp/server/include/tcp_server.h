#ifndef SERVER_TCP_SERVER_H
#define SERVER_TCP_SERVER_H


#include <cstdint>
#include <netinet/in.h>
#include <vector>

class TcpServer {
 public:
    void StartServer(uint16_t port_number);

 protected:
    std::vector<std::thread> threads_;

 private:
    virtual void StartWorkingWithClient(int sock_fd) = 0;
};


#endif //SERVER_TCP_SERVER_H
