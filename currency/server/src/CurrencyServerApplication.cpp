#include <iostream>
#include <string>
#include <pthread.h>

#include "include/Server.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: .%s <port>\n", argv[0]);
        return 0;
    }
    uint16_t portNumber = static_cast<uint16_t>(strtol(argv[1], nullptr, 10));

    Server server(portNumber);
    pthread_t serverThread;
    typedef void * (*PTHREAD_FUNC_PTR)(void *);
    
    int rc = pthread_create(&serverThread,
                            nullptr,
                            (PTHREAD_FUNC_PTR) &Server::start,
                            &server);

    if (rc) {
        cout << "Failed to create server." << endl;
        exit(1);
    }

    std::string input;
    while (true) {
        cin >> input;
        
        if (input == "q") {
            server.stop();
            break;
        }
    }

    void *status;
    pthread_join(serverThread, &status);
    
    return 0;
}

