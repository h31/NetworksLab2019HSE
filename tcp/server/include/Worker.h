#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include "Client.h"
#include "Server.h"

class Worker {

public:
    void operator()(Server *server, Client *client);
};

#endif
