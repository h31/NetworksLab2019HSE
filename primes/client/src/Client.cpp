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
    
    int64_t p = readInt64();
    readMessageDelimeter();

    return p;
}

void Client::getLastPrimes(int64_t n, int64_t *dst) {
    int32_t command = 1;
    sendCommand(command);
    sendInt64(n);
    sendMessageDelimeter();

    int64_t m = readInt64();
    for (int64_t i = 0; i < m; i++) {
        dst[i] = readInt64();
    }
    readMessageDelimeter();
}

int64_t Client::getCalculationInterval(int64_t len) {
    int32_t command = 2;
    sendCommand(command);
    sendInt64(len);
    sendMessageDelimeter();
    
    int64_t start = readInt64();
    readMessageDelimeter();

    return start;
}

bool Client::processInterval(int64_t start, int64_t len) {
    int64_t primes[len];
    int64_t cnt = calculatePrimesInInterval(start, len, primes);

    int32_t command = 3;
    sendCommand(command);
    sendInt64(cnt);
    for (int64_t i = 0; i < cnt; i++) {
        sendInt64(primes[i]);
    }
    sendMessageDelimeter();

    int8_t success = readInt8();
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
    if (n == 1) {
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
    return readInt16();
}

int64_t Client::readInt64() {
    bzero(buffer, 8);
    int n = read(sockfd, buffer, 8); // recv on Windows
    checkStatus(n);
    int64_t intValue = (int64_t)buffer[0] |
                       ((int64_t)buffer[1] << 8) |
                       ((int64_t)buffer[2] << 16) |
                       ((int64_t)buffer[3] << 24) |
                       ((int64_t)buffer[4] << 32) |
                       ((int64_t)buffer[5] << 40) |
                       ((int64_t)buffer[6] << 48) |
                       ((int64_t)buffer[7] << 54);
    return intValue;
}

int16_t Client::readInt16() {
    bzero(buffer, 2);
    int n = read(sockfd, buffer, 2); // recv on Windows
    checkStatus(n);
    int16_t intValue = (int16_t)buffer[0] |
                       ((int16_t)buffer[1] << 8);
    return intValue;
}

int8_t Client::readInt8() {
    bzero(buffer, 1);
    int n = read(sockfd, buffer, 1); // recv on Windows
    checkStatus(n);
    int8_t intValue = buffer[0];
    return intValue;
}

void Client::sendMessageDelimeter() {
    sendInt8((int8_t) '\\');
    sendInt8(0);
}

void Client::sendCommand(int32_t command) {
    sendInt32(command);
}

void Client::sendInt64(int64_t d) {
    int n = write(sockfd, &d, 8); // send on Windows
    checkStatus(n);
}

void Client::sendInt32(int32_t d) {
    int n = write(sockfd, &d, 4); // send on Windows
    checkStatus(n);
}

void Client::sendInt8(int8_t d) {
    int n = write(sockfd, &d, 1); // send on Windows
    checkStatus(n);
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

