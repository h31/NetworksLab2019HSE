//
// Created by mikhail on 03.02.19.
//

#include <include/Currency.h>

#include "include/Currency.h"

Currency::Currency(const std::string &name, const std::vector<int32_t> &rates) : name(name), rates(rates) {
    if (rates.size() >= 2) {
        int32_t current = get_current_rate(), previous = get_rate(rates.size() - 2);
        absolute_change = current - previous;
        relative_change = static_cast<int32_t>((current * 100.0) / previous );
    }
}

Currency::Currency(const std::string &name, const int32_t &current_rate,
        const int32_t &absolute_change, const int32_t &relative_change) :
        name(name), absolute_change(absolute_change), relative_change(relative_change) {
    rates = {current_rate - absolute_change, current_rate};
}

const std::string &Currency::get_name() {
    return name;
}

const int32_t &Currency::get_current_rate() {
    return get_rate(rates.size() - 1);
}

const int32_t &Currency::get_rate(size_t i) {
    return 0 <= i && i < rates.size() ? rates[i] : nullptr;
}

const int32_t &Currency::get_absolute_change() {
    return absolute_change;
}

const int32_t &Currency::get_relative_change() {
    return relative_change;
}

