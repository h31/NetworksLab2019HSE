#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <iostream>
#include "Client.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: hostname port";
        exit(0);
    }
    cout << "Hi! Please, enter your username:" << endl << ">";
    string username;
    cin >> username;
    cout << "Logging in as " << username << endl;
    Client client = Client::create_client(argv[1], static_cast<uint16_t>(atoi(argv[2])), username.c_str());
    cout << "Logged in successfully!" << endl << ">";

    const int BUFFER_SIZE = 256;
    char buffer[256];

    while (true) {
        /* check if user wrote anything */
        if (strlen(buffer) > 0)
            bzero(buffer, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE - 1, stdin) != nullptr) {
            string command = string(buffer);
            if (command != "new") {
                cout << "Unknown command: " + command + "...";
            }
        }

        IncomingEvent event = client.get_incoming_event();
        if (event.is_anything_happened()) {
            /* handle event */

            switch (event.get_event()) {
                case IncomingEventType::ERROR:
                case IncomingEventType::NEW_MESSAGE:
                case IncomingEventType::MESSAGE_CONFIRMED:
                    cout << endl << event.get_message() << endl << ">";
                    break;
                case IncomingEventType::SERVER_DISCONNECTED:
                    cout << endl << "Server was disconnected. Shutting down...";
                    client.shut_down();
                    exit(0);
            }
        }
    }
}