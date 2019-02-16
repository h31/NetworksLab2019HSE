//
// Created by mikhail on 16.02.19.
//

#ifndef NETWORKSLAB2019HSE_PRIMENUMBERSCONCURRENT_H
#define NETWORKSLAB2019HSE_PRIMENUMBERSCONCURRENT_H


#include <PrimeNumbers.h>
#include <mutex>
#include <memory>

class PrimeNumbersConcurrent  {
public:
    int64_t get_max();

    std::vector<int64_t> get_last(size_t N);

    int64_t get_bound_for_calculation();

    bool add_prime_numbers(std::vector<int64_t> numbers);

private:
    std::shared_ptr<std::mutex> prime_numbers_mutex = std::make_shared<std::mutex>();
    PrimeNumbers prime_numbers;
};


#endif //NETWORKSLAB2019HSE_PRIMENUMBERSCONCURRENT_H
