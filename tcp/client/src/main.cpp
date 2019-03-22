#include <iostream>

#include "client.h"
#include "no_such_host_exception.h"
#include "socket_opening_exception.h"
#include "connection_opening_exception.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Not enough arguments provided.\n";
        std::cout << "1st argument: host name or ip address\n";
        std::cout << "2nd argument: port number" << std::endl;
        return 0;
    }

    try {
        client cl(argv[1], argv[2]);
        cl.start();
        cl.request_response_cycle();
    } catch (no_such_host_exception& e) {
        std::cout << "No such host; cannot connect" << std::endl;
        return 0;
    } catch (...) {
        std::cout << "Could not connect to the server" << std::endl;
        return 0;
    }

    return 0;
}