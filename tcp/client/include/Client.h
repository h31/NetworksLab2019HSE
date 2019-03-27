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
    static void Client::voicesCount(uint8_t * buffer, int & writeOffset);
    static void Client::createTopic(uint8_t *buffer, int &writeOffset);
    static void Client::removeTopic(uint8_t *buffer, int &writeOffset);
    static void createAlternative(uint8_t *buffer, int &writeOffset);
    static void closeTopic(uint8_t *buffer, int &writeOffset);
    static void vote(uint8_t *buffer, int &writeOffset);
    void printDefaultResponse();
    static void addTopicId(uint8_t *buffer, int &writeOffset);
    static void addType(uint8_t *buffer, int &writeOffset, uint32_t type);
    static void addTopicName(uint8_t *buffer, int &writeOffset);
    static void addAlternatives(uint8_t *buffer, int &writeOffset, int maxSize);
    static void addAlternative(uint8_t *buffer, int &writeOffset);
    static void addTopicStatus(uint8_t *buffer, int &writeOffset);
    static void addAlternativeId(uint8_t *buffer, int &writeOffset);
    void sendMessage(uint8_t *buffer, size_t size);
    uint8_t* respond(size_t size, uint32_t type, void (*addParams)(uint8_t*, int&));
    void printAllTopics();
    void printTop();
    void printHelp();
    int socketFd;

};


#endif //CLIENT_CLIENT_H
