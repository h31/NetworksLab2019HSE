#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>
#include <string>
#include "Network.h"
#include "SimpleChecker.h"

typedef uint64_t calc_t;

class Client
{
private:
    static const calc_t CODE_GET_MAX_SIMPLE = 1;
    static const calc_t CODE_GET_LAST_N = 2;
    static const calc_t CODE_CALCULATE = 3;
    static const calc_t CODE_SEND_CALCULATED = 4;

    Network _network;

public:
    Client(std::string host_name, uint16_t port);

    calc_t get_max_prime() const;
    std::vector<calc_t> get_last_n(calc_t n) const;
    void calculate(calc_t n) const;
};