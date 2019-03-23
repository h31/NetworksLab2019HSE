#include "include/Client.h"

#include <iostream>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

Client::Client(string &hostname, uint16_t portNumber) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(hostname.c_str());

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portNumber);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
}

Client::~Client() {
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int64_t Client::getMaxPrime() {
    int32_t command = 0;
    sendCommand(command);
    sendMessageDelimeter();
    
    int64_t p = readInt<int64_t>();
    readMessageDelimeter();

    return p;
}

int64_t Client::getLastPrimes(int64_t n, int64_t *dst) {
    int32_t command = 1;
    sendCommand(command);
    sendInt<int64_t>(n);
    sendMessageDelimeter();

    int64_t m = readInt<int64_t>();
    for (int64_t i = 0; i < m; i++) {
        dst[i] = readInt<int64_t>();
    }
    readMessageDelimeter();
    
    return m;
}

int64_t Client::getCalculationInterval(int64_t len) {
    int32_t command = 2;
    sendCommand(command);
    sendInt<int64_t>(len);
    sendMessageDelimeter();
    
    int64_t start = readInt<int64_t>();
    readMessageDelimeter();

    return start;
}

bool Client::processInterval(int64_t start, int64_t len) {
    int64_t primes[len];
    int64_t cnt = calculatePrimesInInterval(start, len, primes);

    int32_t command = 3;
    sendCommand(command);
    sendInt<int64_t>(cnt);
    for (int64_t i = 0; i < cnt; i++) {
        sendInt<int64_t>(primes[i]);
    }
    sendMessageDelimeter();

    int8_t success = readInt<int8_t>();
    readMessageDelimeter();
    
    return (bool)success;
}

int64_t Client::calculatePrimesInInterval(int64_t start, int64_t len, int64_t *dst) {
    int64_t end = start + len;
    int64_t cnt = 0;
    
    for (int64_t i = start; i < end; i++) {
        if (isPrime(i)) {
            dst[cnt++] = i; 
        }
    }
    
    return cnt;
}

bool Client::isPrime(int64_t n) {
    if (n == 0 || n == 1) {
        return false;
    }

    for (int64_t d = 2; d * d <= n; d++) {
        if (n % d == 0) {
            return false;
        }
    }
    
    return true;
}

int16_t Client::readMessageDelimeter() {
    return readInt<int16_t>();
}

void Client::sendMessageDelimeter() {
    sendInt<int8_t>((int8_t) '\\');
    sendInt<int8_t>(0);
}

void Client::sendCommand(int32_t command) {
    sendInt<int32_t>(command);
}

void Client::checkStatus(int n) const {
    if (n < 0) {
        string message = "ERROR communicating with socket";
        perror(message.c_str());
        throw message;
    }

    if (n == 0) {
        throw "socket closed";
    }    
}

