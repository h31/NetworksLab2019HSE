#ifndef RATING_SERVER_WORKER_H
#define RATING_SERVER_WORKER_H

#include "Client.h"
#include "Server.h"

class Worker {

public:
  void operator()(Server *server, Client *client);
};



#endif //RATING_SERVER_WORKER_H
