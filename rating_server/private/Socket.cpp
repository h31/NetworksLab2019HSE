#include <Socket.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Socket.h"

bool Socket::read_string(std::string &result) {
  char c;
  do {
    if (!read(&c, 1)) {
      return false;
    }
    result += c;
  } while (c != 0);
  return true;
}


bool Socket::read_uint32(uint32_t &result) {
  uint32_t n;
  bool res = read((char *) &n, sizeof(n));
  result = ntohl(n);
  return res;
}

bool Socket::write(const char *data, size_t len) {
  return ::write(socket, data, len) != len;
}

bool Socket::write_uint32(const uint32_t data) {
  uint32_t h = htonl(data);
  return write((char *)&h, sizeof(h));
}

bool Socket::write_string(const std::string &data) {
  auto size = (size_t) (data.length() + 1);
  return write((char *) data.c_str(), size);
}

void Socket::shutdown() {
  if (socket < 0) {
    return;
  }
  ::shutdown(socket, SHUT_RDWR);
  socket = -1;
}

