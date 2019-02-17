//
// Created by mikhail on 16.02.19.
//

#ifndef NETWORKSLAB2019HSE_SERVER_H
#define NETWORKSLAB2019HSE_SERVER_H


#include <cstdint>
#include <thread>
#include <vector>
#include <mutex>
#include <future>
#include <math.h>
#include "PrimeNumbers.h"
#include "PrimeNumbersConcurrent.h"
#include "ClientHandler.h"

class Server {
public:
    Server(uint16_t portno, int max_number_of_pending_connections);

    void run();

    void stop() const;

    bool shutdown_client(int client_id);

    virtual ~Server();


private:
    PrimeNumbersConcurrent prime_numbers;
    const uint16_t portno;
    int sockfd;
    bool accept_clients = true;
    const int max_number_of_pending_connections;
    std::mutex client_handlers_with_tasks_mutex;
    std::vector<int> client_sockets;
    std::vector<ClientHandler*> client_handlers;
    std::vector<std::thread> client_handler_tasks;

    void finish_client_handlers();

    // void remove_finished_clients();
};

#endif //NETWORKSLAB2019HSE_SERVER_H
