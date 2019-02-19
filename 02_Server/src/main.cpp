#include <iostream>
#include "Server.h"

using namespace std;

uint16_t read_port() 
{
    uint16_t port;

    cout << "Enter port" << endl;
    cin >> port >> port;

    return port;
}

int main() 
{
    try {
        //uint16_t port = read_port();
        uint16_t port = 5001;
        Server server(port);
        server.run();
    } catch (...) {
        cout << "AAAAAAAAAAAAAAA" << endl;
    }

    return 0;
}