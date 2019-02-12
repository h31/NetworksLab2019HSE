#pragma once

#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdint>
#include <vector>
#include <string>

struct NetworkException
{
private:
    std::string message;

public:
    NetworkException(std::string message)
    : message(message)
    {
    }

    std::string get_message() const
    {
        return message;
    }
};

class Network
{
private:
    int _sockfd;

public:
    Network(std::string host_name, uint16_t port);

    void send_int(uint64_t x, size_t size) const;
    uint64_t read_int(size_t size) const;

    void send_vector(const std::vector<uint64_t> &v, size_t size_length, size_t size_num) const;
    std::vector<uint64_t> read_vector(size_t size_length, size_t size_num) const;
};