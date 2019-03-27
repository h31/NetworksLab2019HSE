#include <utility>
#include <PrimeNumbersConcurrent.h>


//
// Created by mikhail on 16.02.19.
//

#include "PrimeNumbersConcurrent.h"

int64_t PrimeNumbersConcurrent::get_max() {
    std::lock_guard<std::mutex> lock(prime_numbers_mutex);
    int64_t result = prime_numbers.get_max();
    return result;
}

std::vector<int64_t> PrimeNumbersConcurrent::get_last(size_t N) {
    std::lock_guard<std::mutex> lock(prime_numbers_mutex);
    std::vector<int64_t> result = prime_numbers.get_last(N);
    return result;
}

int64_t PrimeNumbersConcurrent::get_bound_for_calculation(int64_t N) {
    std::lock_guard<std::mutex> lock(prime_numbers_mutex);
    int64_t result = prime_numbers.get_bound_for_calculation(N);
    return result;
}

bool PrimeNumbersConcurrent::add_prime_numbers(std::vector<int64_t> numbers) {
    std::lock_guard<std::mutex> lock(prime_numbers_mutex);
    bool result = prime_numbers.add_prime_numbers(std::move(numbers));
    return result;
}

std::vector<int64_t> PrimeNumbersConcurrent::get_all() {
    std::lock_guard<std::mutex> lock(prime_numbers_mutex);
    std::vector<int64_t> result = prime_numbers.get_all();
    return result;
}
