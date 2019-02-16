//
// Created by mikhail on 16.02.19.
//

#ifndef NETWORKSLAB2019HSE_PRIMENUMBERS_H
#define NETWORKSLAB2019HSE_PRIMENUMBERS_H


#include <zconf.h>
#include <vector>

class PrimeNumbers {
public:
    virtual int64_t get_max() const;

    virtual std::vector<int64_t> get_last(size_t N) const;

    virtual int64_t get_bound_for_calculation() const;

    virtual bool add_prime_numbers(std::vector<int64_t> numbers);

private:
    std::vector<int64_t> prime_numbers;
};


#endif //NETWORKSLAB2019HSE_PRIMENUMBERS_H
