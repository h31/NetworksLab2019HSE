#include "include/Server.h"

#include <iostream>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

Server::Server(uint16_t portNumber)
    : portNumber(portNumber) {}

void Server::start() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNumber);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, SOMAXCONN);
    unsigned int clilen = sizeof(cli_addr);
    
    while (true) {
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            break;
        }

        clientHandlers.push_back(ClientHandler(newsockfd, *this));
        clientThreads.push_back(pthread_t());
        
        typedef void * (*PTHREAD_FUNC_PTR)(void *);
        int rc = pthread_create(&clientThreads.back(),
                                nullptr,
                                (PTHREAD_FUNC_PTR) &ClientHandler::run,
                                &clientHandlers.back());

        if (rc) {
            perror("Failed to create client thread.");
            continue;
        }
    }
    
    finish();
}

void Server::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

void Server::finish() const {
    for (auto clientHandler : clientHandlers) {
        clientHandler.stop();
    }
    
    void *status;
    for (auto thread : clientThreads) {
        pthread_join(thread, &status);    
    }
}

Server::ClientHandler::ClientHandler(int sockfd, Server &server)
    : sockfd(sockfd),
      server(server) {}

void Server::ClientHandler::run() {
    try {
        while (true) {
            int32_t command = readCommand();
            
            switch (command) {
                case 0:
                    processCurrencyListQuery();
                    break;
                case 1:
                    processNewCurrencyQuery();
                    break;
                case 2:
                    processDeleteCurrencyQuery();
                    break;
                case 3:
                    processAddCurrencyRateQuery();
                    break;
                case 4:
                    processCurrencyRateHistoryQuery();
                    break;
            }
        }
    } catch (const char *ignored) {
    }
}

void Server::ClientHandler::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

void Server::ClientHandler::processCurrencyListQuery() {
    readMessageDelimeter();
    
    server.mtx.lock();
    
    for (auto it = server.currencies.begin(); it != server.currencies.end(); ++it) {
        Currency &currency = it->second;
        const string &name = currency.getName();
        int32_t rate = currency.getCurrentRate();
        int32_t absoluteChange = currency.getAbsoluteChange();
        int32_t relativeChange = currency.getRelativeChange();
        int8_t hasPreviousRate = 1;
        
        if (relativeChange == Currency::ABSENT_CHANGE_VALUE) {
            hasPreviousRate = 0;
            absoluteChange = 0;
            relativeChange = 0;
        }
                
        sendString(name, CURRENCY_NAME_SIZE);
        sendInt32(rate);
        sendInt8(hasPreviousRate);
        sendInt32(absoluteChange);
        sendInt32(relativeChange);  
    }
    
    server.mtx.unlock();
    
    sendMessageDelimeter();
}

void Server::ClientHandler::processNewCurrencyQuery() {
    string name = readCurrencyName();
    int32_t rate = readCurrencyRate();
    readMessageDelimeter();
    
    server.mtx.lock();
    
    Currency currency = Currency(name, rate);
    auto result = server.currencies.emplace(name, currency);        
    int8_t success = result.second ? 1 : 0;

    server.mtx.unlock();
    
    sendInt8(success);
    sendMessageDelimeter();
}

void Server::ClientHandler::processDeleteCurrencyQuery() {
    string name = readCurrencyName();
    readMessageDelimeter();   

    server.mtx.lock();
    
    int8_t success;
    
    if (server.currencies.find(name) != server.currencies.end()) {
        success = 1;
        server.currencies.erase(name);
    } else {
        success = 0;
    }

    server.mtx.unlock();
    
    sendInt8(success);
    sendMessageDelimeter();
}

void Server::ClientHandler::processAddCurrencyRateQuery() {
    string name = readCurrencyName();
    int32_t rate = readCurrencyRate();
    readMessageDelimeter();
    
    server.mtx.lock();
    
    int8_t success;
    
    if (server.currencies.find(name) != server.currencies.end()) {
        success = 1;
        Currency &currency = server.currencies.find(name)->second;
        currency.addRate(rate);
    } else {
        success = 0;
    }

    server.mtx.unlock();
    
    sendInt8(success);
    sendMessageDelimeter();
}

void Server::ClientHandler::processCurrencyRateHistoryQuery() {
    string name = readCurrencyName();
    readMessageDelimeter();
    
    server.mtx.lock();
    
    Currency &currency = server.currencies.find(name)->second;
    auto rates = currency.getRates();
    
    server.mtx.unlock();
    
    for (auto rate : rates) {
        sendInt32(rate);
    }   

    sendMessageDelimeter();
}

int32_t Server::ClientHandler::readCommand() {
    return readInt32();    
}

int16_t Server::ClientHandler::readMessageDelimeter() {
    return readInt16();
}

const string Server::ClientHandler::readCurrencyName() {
    char name[CURRENCY_NAME_SIZE + 1];
    bzero(name, CURRENCY_NAME_SIZE + 1);
    readChars(name, CURRENCY_NAME_SIZE);
    return string(name);    
}

int32_t Server::ClientHandler::readCurrencyRate() {
    return readInt32();
}

int32_t Server::ClientHandler::readInt32() {
    bzero(buffer, 4);
    int n = read(sockfd, buffer, 4); // recv on Windows
    checkStatus(n);
    int32_t intValue = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    return intValue;
}

int16_t Server::ClientHandler::readInt16() {
    bzero(buffer, 2);
    int n = read(sockfd, buffer, 2); // recv on Windows
    checkStatus(n);
    int16_t intValue = buffer[0] | (buffer[1] << 8);
    return intValue;
}

void Server::ClientHandler::readChars(char *dst, size_t size) {
    bzero(buffer, size);
    int n = read(sockfd, buffer, size); // recv on Windows
    checkStatus(n);
    memcpy(dst, buffer, size);
}

void Server::ClientHandler::sendMessageDelimeter() {
    sendInt8((int8_t) '\\');
    sendInt8(0);
}

void Server::ClientHandler::sendString(const string &message, size_t len) {
    string fitMessage(message);
    fitMessage.resize(len, 0);
    int n = write(sockfd, fitMessage.c_str(), fitMessage.size()); // send on Windows
    checkStatus(n);
}

void Server::ClientHandler::sendInt32(int32_t d) {
    int n = write(sockfd, &d, 4); // send on Windows
    checkStatus(n);
}

void Server::ClientHandler::sendInt8(int8_t d) {
    int n = write(sockfd, &d, 1); // send on Windows
    checkStatus(n);
}

void Server::ClientHandler::checkStatus(int n) const {
    if (n < 0) {
        string message = "ERROR communicating with socket";
        perror(message.c_str());
        throw message;
    }

    if (n == 0) {
        throw "socket closed";
    }    
}

