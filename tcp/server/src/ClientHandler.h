//
// Created by Мария on 2019-02-18.
//

#ifndef SERVER_CLIENTHANDLER_H
#define SERVER_CLIENTHANDLER_H


#include "server.h"

class ClientHandler {
  public:
    ClientHandler(int i, Server server);
    void run();
};


#endif //SERVER_CLIENTHANDLER_H
