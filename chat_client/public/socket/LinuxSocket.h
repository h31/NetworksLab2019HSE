//
// Created by machine on 09-Feb-19.
//

#ifndef CHAT_CLIENT_LINUXSOCKET_H
#define CHAT_CLIENT_LINUXSOCKET_H


#include "MySocket.h"
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

class LinuxSocket : public MySocket {
private:
  int socket_connection;

public:
  LinuxSocket(std::string &host, uint16_t port);

  ~LinuxSocket();

  int send(void *data, int size) override;

  int receive(void *data, int size) override;

  bool receiveString(std::string &destination);

  void close() override;

private:
  static const size_t BUFFER_CAPACITY = 4096;
  char *buffer = new char[BUFFER_CAPACITY];
  ssize_t buffer_size = 0;
  ssize_t buffer_position = 0;

  void read_buffer();
};


#endif //CHAT_CLIENT_LINUXSOCKET_H
