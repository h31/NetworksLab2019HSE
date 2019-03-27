//
// Created by mikhail on 16.02.19.
//

#include <PrimeNumbers.h>
#include <algorithm>

#include "PrimeNumbers.h"

int64_t PrimeNumbers::get_max() const {
    return prime_numbers.empty() ? 0 : prime_numbers.back();
}

std::vector<int64_t> PrimeNumbers::get_last(size_t N) const {
    if (N <= 0) {
        return std::vector<int64_t>();
    }
    return std::vector<int64_t>(prime_numbers.end() - std::min(N, prime_numbers.size()), prime_numbers.end());
}

int64_t PrimeNumbers::get_bound_for_calculation(int64_t N) {
    int64_t prev_bound = left_bound;
    left_bound += N;
    return prev_bound;
}

bool PrimeNumbers::add_prime_numbers(std::vector<int64_t> numbers) {
    prime_numbers.insert(prime_numbers.end(), numbers.begin(), numbers.end());
    std::sort(prime_numbers.begin(), prime_numbers.end());
    return true;
}

std::vector<int64_t> PrimeNumbers::get_all() const {
    return prime_numbers;
}
