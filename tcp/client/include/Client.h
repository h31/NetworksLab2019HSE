//
// Created by Olga Alehina on 2019-02-18.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "ReadWriteHelper.h"
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <Client.h>
#include <vector>

class Client {
public:
    void start(char* host, uint16_t port);

private:
    void connect(char* host, uint16_t port);
    void getTopics(uint32_t type);
    void voicesCount(uint32_t type);
    void createTopic(uint32_t type);
    void removeTopic(uint32_t type);
    void createAlternative(uint32_t type);
    void closeTopic(uint32_t type);
    void vote(uint32_t type);
    void printDefaultResponse();
    void addTopicId(uint8_t *buffer, int &writeOffset);
    void addType(uint8_t *buffer, int &writeOffset, uint32_t type);
    void addTopicName(uint8_t *buffer, int &writeOffset);
    void addAlternatives(uint8_t *buffer, int &writeOffset, int maxSize);
    void addAlternative(uint8_t *buffer, int &writeOffset);
    void addTopicStatus(uint8_t *buffer, int &writeOffset);
    void addAlternativeId(uint8_t *buffer, int &writeOffset);
    void sendMessage(uint8_t *buffer, size_t size);

    void printAllTopics();
    void printTop();
    void printHelp();
    int socketFd;

};


#endif //CLIENT_CLIENT_H
