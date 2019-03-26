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
    static const size_t MAX_LEN = 500;
    static const size_t TIME_OUT_SEC = 5;

    std::string _host_name;
    uint16_t _port;

    int try_send_message(char* message, size_t message_length, char* response) const;
    uint64_t get_check_sum(const std::vector<uint64_t> &v, size_t prefix_length) const;
    size_t data2message(const std::vector<uint64_t> &message_data, size_t size_num, char* message) const;
    int massage2data(char* response, int response_length, size_t size_num, std::vector<uint64_t> &response_data) const;

public:
    Network(std::string host_name, uint16_t port);

    std::vector<uint64_t> send_message(const std::vector<uint64_t> &v, size_t size_num) const;
};