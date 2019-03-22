#ifndef CLIENT_H
#define CLIENT_H

#include <cstdint>
#include <cstdlib>
#include <string>

#include <netdb.h>

#include "socket_io.h"

class client {
private:
    const uint16_t port;
    const hostent* server;
    sockaddr_in server_address;
    int socket_file_descriptor = -1;
    int find_product_id(const std::string& name, socket_io& io);
    void output_help();
    void output_incorrect_argument_for_cli_option_message();

public:
    client(const char* host, const char* port);
    void start();
    void request_response_cycle();
    void add_product_query(const std::string& name, size_t price, size_t amount, socket_io& io);
    void list_products_query(socket_io& io);
    int purchase_product_query(const std::string& name, socket_io& io);
    void stop();
};



#endif //CLIENT_H
