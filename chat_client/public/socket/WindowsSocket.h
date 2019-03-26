//
// Created by machine on 09-Feb-19.
//

#ifndef CHAT_CLIENT_WINDOWSSOCKET_H
#define CHAT_CLIENT_WINDOWSSOCKET_H


#include "MySocket.h"
#include <winsock.h>

class WindowsSocket : public MySocket {
private:
  SOCKET socket_connection;

public:
  WindowsSocket(std::string &host, uint16_t port);

  virtual int send(void *data, int size) override;

  virtual int receive(void *data, int size) override;

  virtual void close() override;
};


#endif //CHAT_CLIENT_WINDOWSSOCKET_H
