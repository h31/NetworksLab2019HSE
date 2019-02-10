#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <iostream>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

void write_to_socket(int socket_descriptor, const void *buf, size_t size);

void read_from_socket(int socket_descriptor, void *buf, size_t size);

void println(const std::string &s);

void error(const std::string &s);


#endif //CLIENT_UTIL_H
