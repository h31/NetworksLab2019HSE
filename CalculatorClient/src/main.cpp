#include <iostream>
#include <sstream>
#include "calc_client.hpp"


int main() {
    std::cout << "Hello in calculator client!" << std::endl;

    string host;

    std::cout << "Enter server address" << std::endl;
    getline(std::cin, host);


    uint16_t port;
    std::cout << "Enter server port" << std::endl;

    std::string input;
    getline(std::cin, input);
    if (!input.empty()) {
        std::istringstream stream(input);
        stream >> port;
    }

    CalcClient client(host.c_str(), port);

    while (true) {

        string command;

        int32_t arg1, arg2;

        std::cout << "Enter one of comands: fact, square, sub, add, mult, div, exit " << std::endl;

        getline(std::cin, command);
        if (command == "exit") {
            std::cout << "Exit.";
            break;
        }

        std::string arg_input;
        std::cout << "Enter first arg" << std::endl;
        getline(std::cin, arg_input);
        if (!arg_input.empty()) {
            std::istringstream stream(arg_input);
            stream >> arg1;
        }

        std::cout << "Enter second arg" << std::endl;
        getline(std::cin, arg_input);
        if (!arg_input.empty()) {
            std::istringstream stream(arg_input);
            stream >> arg2;
        }

        client.execute(command, arg1, arg2);
    }

    return 0;
}