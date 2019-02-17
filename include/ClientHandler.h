//
// Created by mikhail on 16.02.19.
//

#ifndef NETWORKSLAB2019HSE_CLIENTHANDLER_H
#define NETWORKSLAB2019HSE_CLIENTHANDLER_H


class ClientHandler {
public:
    ClientHandler(int client_sockfd, PrimeNumbersConcurrent &prime_numbers);

    void run() const;

private:
    int client_sockfd;
    PrimeNumbersConcurrent &prime_numbers;

    int32_t read_command() const;

    void process_max_number() const;

    void process_last_numbers() const;

    void process_bound_for_calculation() const;

    void process_add_prime_numbers() const;

    int16_t read_message_end() const;

    void write_message_end() const;
};


#endif //NETWORKSLAB2019HSE_CLIENTHANDLER_H
