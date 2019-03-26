//
// Created by machine on 09-Feb-19.
//

#include "../../public/socket/WindowsSocket.h"

#ifdef _WIN32
MySocket* MySocket::create_socket(std::string &host, std::string port) {
  return new WindowsSocket(host, port);
}
#endif

WindowsSocket::WindowsSocket(std::string &host, uint16_t port) {
  socket_connection = socket(AF_INET, SOCK_STREAM, 0);

  hostent *server = gethostbyname(host.c_str());


  sockaddr_in server_address{};
  std::fill((char *) &server_address, (char *) &server_address + sizeof(server_address), 0);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  std::copy(server->h_addr, server->h_addr + (size_t) server->h_length, (char *) &server_address.sin_addr.s_addr);

  if (connect(socket_connection, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    perror("ERROR connecting");
    exit(1);
  }
}

int WindowsSocket::send(void *data, int size) {
  return ::send(socket_connection, (char *) data, size, 0);
}

int WindowsSocket::receive(void *data, int size) {
  return ::recv(socket_connection, (char *) data, size, 0);
}

void WindowsSocket::close() {
  closesocket(socket_connection);
  WSACleanup();
}

bool LinuxSocket::receiveString(std::string &destination) {
  return false;
}
