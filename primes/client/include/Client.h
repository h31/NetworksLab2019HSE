#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <unistd.h>

class Client {
public:
    Client(std::string &hostname, uint16_t portNumber);
    ~Client();
     
    int64_t getMaxPrime();
    
    int64_t getLastPrimes(int64_t n, int64_t *dst);
    
    int64_t getCalculationInterval(int64_t len);
    
    bool processInterval(int64_t start, int64_t len);

private:
    int64_t calculatePrimesInInterval(int64_t start, int64_t len, int64_t *dst);
    bool isPrime(int64_t n);

    void sendCommand(int32_t command);
    int16_t readMessageDelimeter();
    void sendMessageDelimeter();

    template<class T>
    T readInt();
    template<class T>
    void sendInt(T d);
    
    void checkStatus(int n) const;

    int sockfd;
    static const int BUFFER_SIZE = 256;
    int8_t buffer[BUFFER_SIZE];
};

template<class T>
T Client::readInt() {
    T intValue = 0;
    int n = read(sockfd, &intValue, sizeof(intValue));
    checkStatus(n);
    
    return intValue;
}

template<class T>
void Client::sendInt(T d) {
    int n = write(sockfd, &d, sizeof(d));
    checkStatus(n);
}

#endif // CLIENT_H

