#pragma once

#include <netdb.h>
#include "socketUtils.hpp"

class PacketManager {
public:

  explicit PacketManager() {}

  PacketManager(sockaddr_in const &host, uint16_t port, int socket)
    : m_host(host), m_port(port), m_socket(socket) {}

  void sendMessage(uint8_t const *data, size_t size) {
    ssize_t sent = sendto(m_socket, data, size, 0, (const sockaddr *) &m_host, sizeof(m_host));

    if (size != sent) {
      error("Server unavailable.");
    }
  }

  void receiveMessage(uint8_t *data, size_t size) {
    socklen_t addrLength = sizeof(m_host);
    ssize_t received = recvfrom(m_socket, (void *) data, size, 0, (sockaddr *) &m_host, &addrLength);

    if (size != received) {
      error("Lost connection.");
    }
  }

private:
  int m_socket;
  sockaddr_in m_host;
  uint16_t m_port;
};