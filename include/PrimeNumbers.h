//
// Created by mikhail on 16.02.19.
//

#ifndef NETWORKSLAB2019HSE_PRIMENUMBERS_H
#define NETWORKSLAB2019HSE_PRIMENUMBERS_H


#include <zconf.h>
#include <vector>

class PrimeNumbers {
public:
    int64_t get_max() const;

    std::vector<int64_t> get_last(size_t N) const;

    int64_t get_bound_for_calculation(int64_t N);

    bool add_prime_numbers(std::vector<int64_t> numbers);

    std::vector<int64_t> get_all() const;

private:
    int64_t left_bound = 0;
    std::vector<int64_t> prime_numbers;
};


#endif //NETWORKSLAB2019HSE_PRIMENUMBERS_H
