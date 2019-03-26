#pragma once

#include <iostream>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

void writeToSocket(sockaddr_in &host, int socket, uint8_t const *data, size_t size);

void readFromSocket(sockaddr_in &host, int socket, uint8_t *data, size_t size);

void printError(const std::string &s);

void error(const std::string &type, const std::string &s);

int socketInit();

sockaddr_in hostInit(std::string const &hostname, uint16_t port);

template<typename T>
void writeObject(sockaddr_in &host, int socket, const T &object) {
    writeToSocket(host, socket, (uint8_t const *) &object, sizeof(T));
}

template<typename T>
T readObject(sockaddr_in &host, int socket) {
    T object;
    readFromSocket(host, socket, (uint8_t *) &object, sizeof(T));

    return object;
}
