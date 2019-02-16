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
    uint16_t portNumber = static_cast<uint16_t>(strtol(argv[2], nullptr, 10));

    Client client(hostname, portNumber);
    string command;
    
    cout << "maxp -- get maximum prime on server" << endl;
    cout << "lastn N -- get last N primes on server" << endl;
    cout << "calc N -- get interval start from server and check next N numbers, sending result to server" << endl;
    cout << "exit -- exit" << endl;
    
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
            client.getLastPrimes(n, lastPrimes);
            
            for (int64_t i = 0; i < n; i++) {
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
            client.processInterval(start, n);
        } else if (command == "exit") {
            break;
        } else {
            cout << "unknown command" << endl;
        }
    }
    
    return 0;
}

