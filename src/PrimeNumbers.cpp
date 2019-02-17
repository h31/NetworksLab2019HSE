//
// Created by mikhail on 16.02.19.
//

#include <PrimeNumbers.h>

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

int64_t PrimeNumbers::get_bound_for_calculation() const {
    return get_max();
}

bool PrimeNumbers::add_prime_numbers(std::vector<int64_t> numbers) {
    if (numbers.empty()) {
        return true;
    }
    else if (prime_numbers.empty()) {
        prime_numbers.insert(prime_numbers.end(), numbers.begin(), numbers.end());
        return true;
    } else if (numbers[0] == prime_numbers.back()) {
        prime_numbers.insert(prime_numbers.end(), numbers.begin() + 1, numbers.end());
        return true;
    }
    return false;
}

std::vector<int64_t> PrimeNumbers::get_all() const {
    return prime_numbers;
}
