#include "socketUtils.hpp"

void writeToSocket(sockaddr_in &host, int socket, uint8_t const *data, size_t size) {
    ssize_t n = sendto(socket, data, size, 0, (const sockaddr *) &host, sizeof(host));
    if (size != n) {
        error("write", "failed to write to socket!");
    }
}

void readFromSocket(sockaddr_in &host, int socket, uint8_t *data, size_t size) {
    socklen_t len = sizeof(host);
    ssize_t n = recvfrom(socket, (void *) data, size, 0, (sockaddr *) &host, &len);
    if (size != n) {
        error("read", "failed to read from socket!");
    }
}

void printError(const std::string &s) {
    std::cerr << s << std::endl;
}

void error(const std::string &type, const std::string &s) {
    printError("Error " + type + ": " + s);
    exit(0);
}

int socketInit() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        error("Init socket", "Network problem.");
    }

    return sockfd;
}

sockaddr_in hostInit(std::string const &hostname, uint16_t port) {
    hostent *server = gethostbyname(hostname.c_str());

    if (server == nullptr) {
        error("Host init", "Incorrect host.");
    }

    sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port);

    return serv_addr;
}
