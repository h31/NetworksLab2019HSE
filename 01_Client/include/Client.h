#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>
#include <string>
#include "Network.h"
#include "SimpleChecker.h"

typedef uint64_t calc_t;
typedef uint64_t count_t;

class Client
{
private:
    typedef uint64_t code_t;

    static const code_t CODE_GET_MAX_SIMPLE = 1;
    static const code_t CODE_GET_LAST_N = 2;
    static const code_t CODE_CALCULATE = 3;

    Network _network;

public:
    Client(std::string host_name, uint16_t port);

    calc_t get_max_prime() const;
    std::vector<calc_t> get_last_n(count_t n) const;
    void calculate(count_t n) const;
};