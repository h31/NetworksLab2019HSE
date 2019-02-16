#include <utility>

//
// Created by mikhail on 16.02.19.
//

#include "PrimeNumbersConcurrent.h"

int64_t PrimeNumbersConcurrent::get_max() {
    prime_numbers_mutex.lock();
    int64_t result = prime_numbers.get_max();
    prime_numbers_mutex.unlock();
    return result;
}

std::vector<int64_t> PrimeNumbersConcurrent::get_last(size_t N) {
    prime_numbers_mutex.lock();
    std::vector<int64_t> result = prime_numbers.get_last(N);
    prime_numbers_mutex.unlock();
    return result;
}

int64_t PrimeNumbersConcurrent::get_bound_for_calculation() {
    prime_numbers_mutex.lock();
    int64_t result = prime_numbers.get_bound_for_calculation();
    prime_numbers_mutex.unlock();
    return result;
}

bool PrimeNumbersConcurrent::add_prime_numbers(std::vector<int64_t> numbers) {
    prime_numbers_mutex.lock();
    bool result = prime_numbers.add_prime_numbers(std::move(numbers));
    prime_numbers_mutex.unlock();
    return result;
}
