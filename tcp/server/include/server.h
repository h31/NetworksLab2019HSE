#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/thread.hpp>

#include <string.h>
#include <vector>
#include "socket_opening_exception.h"
#include "binding_exception.h"


class server {
    private:
        static const int CONNECTION_QUEUE_SIZE = 5;
        const uint16_t PORT;
        boost::thread *main_thread = nullptr;
        std::vector<boost::thread *> clients;
        boost::mutex clients_mutex;

        void client_accept_cycle(int server_socket_fd);

        void request_response_cycle();

    public:
        explicit server(uint16_t port);

        void start();

        void stop();
};

#endif //SERVER_SERVER_H
