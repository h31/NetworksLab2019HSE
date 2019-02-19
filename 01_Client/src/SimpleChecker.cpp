#include "SimpleChecker.h"

using std::vector;

bool SimpleChecker::is_prime(uint64_t n)
{
    if (n < 2)
        return false;

    for (uint64_t d = 2; d*d <= n; d++)
        if ((n / d) * d == n)
            return false;
    return true;
}

vector<uint64_t> SimpleChecker::check_interval(uint64_t start_num, size_t n)
{
    vector<uint64_t> res;

    for (size_t dx = 0; dx < n; dx++) {
        size_t x = start_num + dx;
        if (is_prime(x))
            res.push_back(x);
    }

    return res;
}