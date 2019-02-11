#include <Client.h>
#include <iostream>
#include <sstream>

using namespace std;

#define PORT 4567

int main() {
    string serverAddress;
    cout << "Enter server address (blank for 127.0.0.1) :> ";
    getline(cin, serverAddress);
    if (serverAddress.empty()) {
        serverAddress = "127.0.0.1";
    }

    uint16_t port = PORT;
    std::cout << "Enter port (or leave empty for " << PORT << ") :> ";
    std::string input;
    getline(std::cin, input);
    if (!input.empty()) {
        std::istringstream stream( input );
        stream >> port;
    }

    vacancy::Client client(serverAddress, port);
    client.run();
    return 0;
}