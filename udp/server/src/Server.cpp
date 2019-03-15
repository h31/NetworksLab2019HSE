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
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

    unsigned int clilen = sizeof(cli_addr);
    int n;
    int8_t writingBuffer[BUFFER_SIZE];
    
    while (true) {
        bufferPosition = 0;
        message.clear();

        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &cli_addr, &clilen);
        
        if (n < 0) {
            perror("ERROR on receiving");
            continue;    
        }
        
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
        
        int32_t dataSize = BUFFER_SIZE - 2 * sizeof(int32_t);
        int32_t dGramNumber = (message.size() + dataSize - 1) / dataSize;

        for (int32_t i = 0, currentDGram = 0; i < message.size(); i += dataSize, currentDGram++) {
            bzero((char *) &writingBuffer, BUFFER_SIZE);
            
            memcpy(writingBuffer, &dataSize, sizeof(int32_t));
            memcpy(writingBuffer + sizeof(int32_t), &currentDGram, sizeof(int32_t));
            
            for (int j = 0; j < BUFFER_SIZE && i + j < message.size(); j++) {
                writingBuffer[2 * sizeof(int32_t) + j] = message[i + j];
            }

            n = sendto(sockfd, writingBuffer, BUFFER_SIZE, 0, (struct sockaddr*) &cli_addr, clilen);
            
            if (n < 0) {
                perror("ERROR on sending");
                break;
            }
        }
    }
}

void Server::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

void Server::processCurrencyListQuery() {
    for (auto it = currencies.begin(); it != currencies.end(); ++it) {
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
    
}

void Server::processNewCurrencyQuery() {
    string name = readCurrencyName();
    int32_t rate = readCurrencyRate();
    
    Currency currency = Currency(name, rate);
    auto result = currencies.emplace(name, currency);        
    int8_t success = result.second ? 1 : 0;

    sendInt8(success);
}

void Server::processDeleteCurrencyQuery() {
    string name = readCurrencyName();
    
    int8_t success;
    
    if (currencies.find(name) != currencies.end()) {
        success = 1;
        currencies.erase(name);
    } else {
        success = 0;
    }

    sendInt8(success);
}

void Server::processAddCurrencyRateQuery() {
    string name = readCurrencyName();
    int32_t rate = readCurrencyRate();
    
    int8_t success;
    
    if (currencies.find(name) != currencies.end()) {
        success = 1;
        Currency &currency = currencies.find(name)->second;
        currency.addRate(rate);
    } else {
        success = 0;
    }
    
    sendInt8(success);
}

void Server::processCurrencyRateHistoryQuery() {
    string name = readCurrencyName();
    
    Currency &currency = currencies.find(name)->second;
    auto rates = currency.getRates();
    
    for (auto rate : rates) {
        sendInt32(rate);
    }   
}

int32_t Server::readCommand() {
    return readInt32();    
}

const string Server::readCurrencyName() {
    char name[CURRENCY_NAME_SIZE + 1];
    bzero(name, CURRENCY_NAME_SIZE + 1);
    readChars(name, CURRENCY_NAME_SIZE);
    return string(name);    
}

int32_t Server::readCurrencyRate() {
    return readInt32();
}

int32_t Server::readInt32() {
    int32_t intValue = 0;
    memcpy(&intValue, buffer + bufferPosition, sizeof(intValue));
    bufferPosition += sizeof(intValue);
    return intValue;
}

void Server::readChars(char *dst, size_t size) {
    memcpy(dst, buffer + bufferPosition, size);
    bufferPosition += size;
}

void Server::sendString(const string &src, size_t len) {
    std::copy(src.begin(), src.end(), std::back_inserter(message));
    
    for (int i = 0; i < len - src.size(); i++) {
        message.push_back(0);
    }
}

void Server::sendInt32(int32_t d) {
    message.push_back(d & 0x000000ff);
    message.push_back((d & 0x0000ff00) >> 8);
    message.push_back((d & 0x00ff0000) >> 16);
    message.push_back((d & 0xff000000) >> 24);
}

void Server::sendInt8(int8_t d) {
    message.push_back(d);
}
