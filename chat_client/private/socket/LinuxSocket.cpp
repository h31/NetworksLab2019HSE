//
// Created by machine on 09-Feb-19.
//

#include <cstring>
#include "../../public/socket/LinuxSocket.h"

#ifdef linux

MySocket *MySocket::create_socket(std::string &host, uint16_t port) {
  return new LinuxSocket(host, port);
}

#endif


LinuxSocket::LinuxSocket(std::string &host, uint16_t port) {
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

int LinuxSocket::send(void *data, int size) {
  return static_cast<int>(::write(socket_connection, (char *) data, size));
}

int LinuxSocket::receive(void *data, int size) {
  auto remaining_size = static_cast<ssize_t>(size);
  while (remaining_size > 0) {
    if (buffer_position >= buffer_size) {
      read_buffer();
      if (buffer_size == 0) {
        return static_cast<int>(size - remaining_size);
      }
    }
    auto copy_size = static_cast<size_t>(std::min(remaining_size, buffer_size - buffer_position));
    memcpy(data, buffer + buffer_position, copy_size);
    remaining_size -= copy_size;
    buffer_position += copy_size;
    data = (char *) data + copy_size;
  }
  return size;
}

void LinuxSocket::read_buffer() {
  buffer_position = 0;
  buffer_size = read(socket_connection, buffer, BUFFER_CAPACITY);
}

void LinuxSocket::close() {
  ::shutdown(socket_connection, SHUT_RDWR);
  ::close(socket_connection);
  socket_connection = -1;
}

bool LinuxSocket::receiveString(std::string &destination) {
  char next_char;
  do {
    if (buffer_position >= buffer_size) {
      buffer_position = 0;
      buffer_size = ::read(socket_connection, buffer, BUFFER_CAPACITY);
      if (buffer_size == 0) {
        return false;
      }
    }
    next_char = buffer[buffer_position++];
    destination += next_char;
  } while (next_char != 0);
  destination.pop_back();
  return true;
}

LinuxSocket::~LinuxSocket() {
  delete[] buffer;
}
