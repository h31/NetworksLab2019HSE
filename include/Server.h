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

class Server {
public:
    Server(uint16_t portno, int max_number_of_pending_connections);

    void run();

    void stop() const;

    bool shutdown_client(int client_id) const;

    virtual ~Server();


private:
    class ClientHandler {
    public:
        explicit ClientHandler(int sockfd, PrimeNumbersConcurrent &prime_numbers);

        void run() const;

        void stop() const;

        int get_client_id() const;

        virtual ~ClientHandler();

    private:
        const int client_id;
        const int sockfd;
        PrimeNumbersConcurrent &prime_numbers;

        int32_t read_command() const;

        void process_max_number() const;

        void process_last_numbers() const;

        void process_bound_for_calculation() const;

        void process_add_prime_numbers() const;

        int16_t read_message_end() const;

        void write_message_end() const;
    };

    class ClientHandlerWithTask {
    public:
        ClientHandlerWithTask(int clientsockfd, PrimeNumbersConcurrent &prime_numbers);

        virtual ~ClientHandlerWithTask();

        bool is_ready() const;

        int get_client_id() const;

        void stop() const;

    private:
        ClientHandler client_handler;
        std::future<void> client_handler_task;
    };

    PrimeNumbersConcurrent prime_numbers;
    const uint16_t portno;
    const int sockfd;
    bool accept_clients = true;
    const int max_number_of_pending_connections;
    std::vector<ClientHandlerWithTask> client_handlers_with_tasks;

    void finish_client_handlers();

    void remove_finished_clients();
};

#endif //NETWORKSLAB2019HSE_SERVER_H
