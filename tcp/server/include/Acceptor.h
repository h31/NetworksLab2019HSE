#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include "Server.h"

class Acceptor {

public:
    void operator()(Server *server);
};

#endif
