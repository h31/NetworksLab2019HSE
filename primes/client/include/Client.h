#ifndef CLIENT_H
#define CLIENT_H

#include <string>

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
    int64_t readInt64();
    int32_t readInt32();
    int16_t readInt16();
    int8_t readInt8();
    
    void sendMessageDelimeter();
    void sendInt64(int64_t d);
    void sendInt32(int32_t d);
    void sendInt8(int8_t d);
    
    void checkStatus(int n) const;

    int sockfd;
    static const int BUFFER_SIZE = 256;
    int8_t buffer[BUFFER_SIZE];
};

#endif // CLIENT_H

