#pragma once

#include <iostream>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

void writeToSocket(int socketDescriptor, const void *buffer, size_t size);

void readFromSocket(int socketDescriptor, uint8_t *buffer, size_t size);

void printError(const std::string &s);

void error(const std::string &s);

void error(const std::string &type, const std::string &s);

int connectToServer(std::string const &hostname, uint16_t port);

template<typename T>
void writeObject(int socketDescriptor, const T &object) {
    writeToSocket(socketDescriptor, (uint8_t const *) &object, sizeof(T));
}

template<typename T>
T readObject(int socketDescriptor) {
    T object;
    readFromSocket(socketDescriptor, (uint8_t *) &object, sizeof(T));

    return object;
}

std::string read_until_zero(int *ptr, char *buffer, size_t buffer_size);
