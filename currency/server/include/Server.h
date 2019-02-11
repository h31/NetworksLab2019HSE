#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <map>
#include <pthread.h>
#include <mutex>

#include "include/Currency.h"

class Server {
public:
    Server(uint16_t portNumber);
     
    void start();
    
    void stop() const;

private:
    class ClientHandler {
    public:
        ClientHandler(int sockfd, Server &server);

        void run();
        
        void stop() const;

    private:
        void processCurrencyListQuery();
        void processNewCurrencyQuery();
        void processDeleteCurrencyQuery();
        void processAddCurrencyRateQuery();
        void processCurrencyRateHistoryQuery();

        int32_t readCommand();
        int16_t readMessageDelimeter();
        const std::string readCurrencyName();
        int32_t readCurrencyRate();
        
        int32_t readInt32();
        int16_t readInt16();
        void readChars(char *dst, size_t size);

        void sendMessageDelimeter();
        void sendString(const std::string &message, size_t len);
        void sendInt32(int32_t n);
        void sendInt8(int8_t n);
        
        void checkStatus(int n) const;
    
    private:
        static const int BUFFER_SIZE = 256;
        static const int CURRENCY_NAME_SIZE = 16;
        int sockfd;
        Server &server;
        int8_t buffer[BUFFER_SIZE];
    };
    
    void finish() const;

    uint16_t portNumber;
    int sockfd;
    std::vector<ClientHandler> clientHandlers;
    std::vector<pthread_t> clientThreads;
    std::map<std::string, Currency> currencies;
    std::mutex mtx;
};

#endif // SERVER_H

