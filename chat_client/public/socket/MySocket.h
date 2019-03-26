//
// Created by machine on 09-Feb-19.
//

#ifndef CHAT_CLIENT_MYSOCKET_H
#define CHAT_CLIENT_MYSOCKET_H


#include <string>

class MySocket {
public:
  static MySocket* create_socket(std::string &host, uint16_t port);

  virtual int send(void* data, int size) = 0;
  virtual int receive(void* data, int size) = 0;
  virtual void close() = 0;
  virtual bool receiveString(std::string &destination) = 0;
};


#endif //CHAT_CLIENT_MYSOCKET_H
