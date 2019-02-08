//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CURRENCY_H
#define NETWORKSLAB2019HSE_CURRENCY_H


#include <string>
#include <vector>

class Currency {
public:
    Currency(std::string name, std::vector<int32_t> rates);

    Currency(std::string name, int32_t current_rate);

    Currency(std::string name, int32_t current_rate, int32_t absolute_change, int32_t relative_change);

    const std::string &get_name() const;

    int32_t get_current_rate() const;

    int32_t get_rate(size_t i) const;

    int32_t get_absolute_change() const;

    int32_t get_relative_change() const;

    bool operator==(const Currency &rhs) const;

    bool operator!=(const Currency &rhs) const;

private:
    std::string name;
    std::vector<int32_t> rates;
    int32_t absolute_change;
    int32_t relative_change;
};


#endif //NETWORKSLAB2019HSE_CURRENCY_H
