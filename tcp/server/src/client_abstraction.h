#ifndef SERVER_CLIENT_ABSTRACTION_H
#define SERVER_CLIENT_ABSTRACTION_H


#include <string>
#include <netinet/in.h>

class ClientAbstraction {
 private:
    const std::string name;
    const sockaddr_in addr;
    const int socket_fd;
};


#endif //SERVER_CLIENT_ABSTRACTION_H
