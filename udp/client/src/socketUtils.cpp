#include "socketUtils.hpp"

void writeToSocket(int socket_descriptor, const void *buf, size_t size) {
  ssize_t n = write(socket_descriptor, buf, size);
  if (n <= 0) {
    error("write", "failed to write to socket!");
  }

  if (n != size) {
    error("write", "unexpected end of socket! (Probably server disconnected)");
    exit(0);
  }
}

void readFromSocket(int socket_descriptor, uint8_t *buf, size_t size) {
  ssize_t n;
  while ((n = read(socket_descriptor, buf, size)) < size) {
    if (n <= 0) {
      error("read", "failed to read from socket!");
      exit(0);
    }

    buf += n;
    size -= n;
  }
}

void printError(const std::string &s) {
  std::cerr << s << std::endl;
}

void error(const std::string &s) {
  printError("Error: " + s);
  exit(0);
}

void error(const std::string &type, const std::string &s) {
  printError("Error " + type + ": " + s);
  exit(0);
}

std::string read_until_zero(int *ptr, char *buffer, size_t buffer_size) {
  std::string dest;
  while (*ptr < buffer_size && buffer[*ptr] != '\0') {
    dest += buffer[*ptr];
    (*ptr)++;
  }
  (*ptr)++;

  return dest;
}

sockaddr_in initializeHost(std::string const &hostname, uint16_t port) {
  hostent *server;

  server = gethostbyname(hostname.c_str());
  sockaddr_in serv_addr{};

  if (server == nullptr) {
    error("Connection", "Error, no such host");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
  serv_addr.sin_port = htons(port);

  return serv_addr;
}

int initializeSocket() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sockfd < 0) {
    error("Error opening socket");
  }

  return sockfd;
}
