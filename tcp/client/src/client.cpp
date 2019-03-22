#include <cstring>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>

#include "client.h"
#include "socket_io.h"

#include "no_such_host_exception.h"
#include "socket_opening_exception.h"
#include "connection_opening_exception.h"
#include "nonexistend_product_exception.h"

client::client(const char *host, const char *port): port((uint16_t) strtol(port, nullptr, 0)) {
    server = gethostbyname(host);
    if (server == nullptr) {
        throw no_such_host_exception();
    }
}

void client::start() {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0) {
        throw socket_opening_exception();
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);
    server_address.sin_port = htons(port);

    if (connect(socket_file_descriptor, (sockaddr *) &server_address, sizeof(server_address)) < 0) {
        throw connection_opening_exception();
    }
}

void client::output_help() {
    std::cout << "This application supports the following commands:" << "\n";
    std::cout << "1: add a new product;" << "\n";
    std::cout << "2: list products;" << "\n";
    std::cout << "3: purchase a product;" << "\n";
    std::cout << "4: terminate the connection, exit the application;" << "\n";
    std::cout << "5: output this help message;" << "\n";
    std::cout << "Input a single digit to make a query." << "\n";
}

void client::output_incorrect_argument_for_cli_option_message() {
    std::cout << "Incorrect argument. Your input must contain a single digit between 1 and 5." << "\n";
}

void client::request_response_cycle() {
    std::string input;
    socket_io io(socket_file_descriptor);

    output_help();
    try {
        while (getline(std::cin, input)) {
            int command_number = 0;
            try {
                command_number = std::stoi(input);
            } catch (...) {
                output_incorrect_argument_for_cli_option_message();
                continue;
            }
            switch (command_number) {
                case 1: {
                    std::cout << "Input, on a new line each time, the following:" << "\n";
                    std::cout << " - product name:" << "\n";
                    std::cout << " - product price:" << "\n";
                    std::cout << " - product amount:" << "\n";
                    std::string name;
                    size_t price = 0;
                    size_t amount = 0;
                    getline(std::cin, name);
                    try {
                        getline(std::cin, input);
                        price = std::stoul(input);
                        getline(std::cin, input);
                        amount = std::stoul(input);
                    } catch (std::invalid_argument& e) {
                        std::cout << "Invalid argument; try again" << "\n";
                        output_help();
                        break;
                    } catch (std::out_of_range& e) {
                        std::cout << "Your number should be non-negative and not bigger than " <<
                                      std::numeric_limits<size_t >::max() << "\n";
                        output_help();
                        break;
                    }
                    add_product_query(name, price, amount, io);
                    std::cout << "Added new product" << "\n";
                    break;
                }
                case 2 : {
                    list_products_query(io);
                    break;
                }
                case 3: {
                    std::cout << "Input the name of the product you are willing to purchase." << "\n";
                    std::string name;
                    getline(std::cin, name);
                    int result;
                    try {
                        result = purchase_product_query(name, io);
                    } catch (nonexistent_product_exception& e) {
                        std::cout << "No such product in the shop" << "\n";
                        break;
                    }
                    switch (result) {
                        case 1:
                            std::cout << "Successful purchase" << "\n";
                            break;
                        case 0:
                            std::cout << "Not enough product in stock" << "\n";
                            break;
                        case -1:
                            std::cout << "The product could not be purchased" << "\n";
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case 4:
                    stop();
                    return;
                case 5:
                    output_help();
                    break;
                default:
                    output_incorrect_argument_for_cli_option_message();
                    break;
            }
        }
    } catch (...) {
        std::cout << "The connection has been unexpectedly closed." << std::endl;
        close(socket_file_descriptor);
    }
}

void client::add_product_query(const std::string& name, size_t price, size_t amount, socket_io& io) {
    io.write_data(1);
    io.write_data(name.size());
    io.write_data(name);
    io.write_data(price);
    io.write_data(amount);
}

int client::find_product_id(const std::string &name, socket_io& io) {
    io.write_data(2);
    auto products_amount = io.read_data<size_t>();
    int product_id = -1;
    for (size_t i = 0; i < products_amount; i++) {
        int id = io.read_data<int>();
        auto string_size = io.read_data<size_t>();
        std::string product_name = io.read_string(string_size);

        if (name == product_name) {
            product_id = id;
        }

        io.read_data<size_t>(); // read price; no need to save the value
        io.read_data<size_t>(); // read amount; no need to save the value
    }
    return product_id;
}

int client::purchase_product_query(const std::string& name, socket_io& io) {
    int id = find_product_id(name, io);
    if (id == -1) {
        throw nonexistent_product_exception();
    }
    io.write_data(3);
    io.write_data(id);
    int response = io.read_data<int>();
    return response;
}

void client::list_products_query(socket_io& io) {
    io.write_data(2);
    auto products_amount = io.read_data<size_t>();
    for (size_t i = 0; i < products_amount; i++) {
        io.read_data<int>(); // read id; no need to store the value
        auto string_size = io.read_data<size_t>();
        std::string product_name = io.read_string(string_size);
        auto price = io.read_data<size_t>();
        auto amount = io.read_data<size_t>();

        std::cout << "Name: " << product_name << "; price: " << price << "; in stock: " << amount << "\n";
    }
}

void client::stop() {
    shutdown(socket_file_descriptor, SHUT_RDWR);
    close(socket_file_descriptor);
}

