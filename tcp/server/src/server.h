#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

class RouletteServer {
 public:

    static void WorkWithClient(RouletteServer* server, int sock_fd, const sockaddr_in& cli_addr);

    void StartServer(uint16_t port_number);

 private:
    volatile int croupier = -1;
};

#endif //SERVER_SERVER_H
