#include "Client.h"
#include <iostream>

using namespace std;

#define PORT 4567

int main() {
    string hostEmail, serverAddress;
//    cin.ignore( numeric_limits<streamsize>::max(), '\n' );

    cout << "Enter your email (blank for aaa@gmail.com) :> ";
    getline(cin, hostEmail);
    if (hostEmail.empty()) {
        hostEmail = "a";
    }
    cout << "Enter server address (blank for 127.0.0.1) :> ";
//    cin >> serverAddress;
    getline(cin, serverAddress);
    if (serverAddress.empty()) {
        serverAddress = "127.0.0.1";
    }
    Client client(hostEmail, serverAddress, PORT);
    client.run();
    return 0;
}