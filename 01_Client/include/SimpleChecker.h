#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>

class SimpleChecker
{
public:
    SimpleChecker() = delete;

    static bool is_simple(uint64_t n);
    static std::vector<uint64_t> check_interval(uint64_t start_num, size_t n);
};