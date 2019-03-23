#include <iostream>
#include <string>
#include <pthread.h>

#include "include/Client.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        return 0;
    }
    string hostname = argv[1];
    long portNumber = strtol(argv[1], nullptr, 10);
    
    if (portNumber <= 0 || portNumber > UINT16_MAX) {
        fprintf(stderr, "illegal port number\n");
        return 0;
    }

    Client client(hostname, static_cast<uint16_t>(portNumber));
    string command;
    
    cout << "maxp -- get maximum prime on server" << endl;
    cout << "lastn N -- get last N primes on server" << endl;
    cout << "calc N -- get interval start from server and check next N numbers, sending result to server" << endl;
    cout << "exit -- exit" << endl;
    cout << endl;
    
    try {
        while (true) {
            cin >> command;
            
            if (command == "maxp") {
                int64_t p = client.getMaxPrime();
                cout << p << endl;
            } else if (command == "lastn") {
                int64_t n;
                cin >> n;
                if (n <= 0) {
                    cout << "N should be positive" << endl;
                    continue;
                }

                int64_t lastPrimes[n];
                int64_t m = client.getLastPrimes(n, lastPrimes);
                
                for (int64_t i = 0; i < m; i++) {
                    cout << lastPrimes[i] << " ";            
                }

                cout << endl;
            } else if (command == "calc") {
                int64_t n;
                cin >> n;        
                if (n <= 0) {
                    cout << "N should be positive" << endl;
                    continue;
                }

                int64_t start = client.getCalculationInterval(n);
                bool success = client.processInterval(start, n);

                if (!success) {
                    cout << "server failed to add calculated primes" << endl;
                }
            } else if (command == "exit") {
                break;
            } else {
                cout << "unknown command" << endl;
            }
        }
    } catch (const char *message) {
        cout << message << endl;
    }
    
    return 0;
}

