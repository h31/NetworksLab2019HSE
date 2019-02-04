#include <Client.h>
#include <iostream>

using namespace std;

#define PORT 4567

int main() {
    string serverAddress;
    cout << "Enter server address (blank for 127.0.0.1) :> ";
    getline(cin, serverAddress);
    if (serverAddress.empty()) {
        serverAddress = "127.0.0.1";
    }
    vacancy::Client client(serverAddress, PORT);
    client.run();
    return 0;
}