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
  LinuxSocket(std::string &host, std::string &port);

  virtual int send(void *data, int size) override;

  virtual int receive(void *data, int size) override;

  virtual void close() override;
};


#endif //CHAT_CLIENT_LINUXSOCKET_H
