#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <map>
#include <cstddef>

#include "include/Currency.h"

class Server {
public:
    Server(uint16_t portNumber);
     
    void start();
    
    void stop() const;

private:
    void processCurrencyListQuery();
    void processNewCurrencyQuery();
    void processDeleteCurrencyQuery();
    void processAddCurrencyRateQuery();
    void processCurrencyRateHistoryQuery();

    int32_t readRequestId();
    int32_t readCommand();
    const std::string readCurrencyName();
    int32_t readCurrencyRate();
    
    int32_t readInt32();
    void readChars(char *dst, size_t size);

    void sendString(const std::string &message, size_t len);
    void sendInt32(int32_t n);
    void sendInt8(int8_t n);
    
    void checkStatus(int n) const;

    static const int CURRENCY_NAME_SIZE = 16;
    static const size_t BUFFER_SIZE = 508;
    int8_t buffer[BUFFER_SIZE];
    int bufferPosition;
    std::vector<int8_t> message;
    std::map<std::string, Currency> currencies;

    int sockfd;
    uint16_t portNumber;
};

#endif // SERVER_H

