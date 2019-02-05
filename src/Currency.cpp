#include <utility>

//
// Created by mikhail on 03.02.19.
//

#include <include/Currency.h>

Currency::Currency(std::string name, std::vector<int32_t> rates) : name(std::move(name)), rates(std::move(rates)) {
    if (rates.size() >= 2) {
        int32_t current = get_current_rate(), previous = get_rate(rates.size() - 2);
        absolute_change = current - previous;
        relative_change = static_cast<int32_t>((current * 100.0) / previous );
    }
}

Currency::Currency(std::string name, int32_t current_rate) : name(std::move(name)) {
    rates = {current_rate};
}

Currency::Currency(std::string name, int32_t current_rate, int32_t absolute_change, int32_t relative_change) :
        name(std::move(name)), absolute_change(absolute_change), relative_change(relative_change) {
    rates = {current_rate - absolute_change, current_rate};
}

const std::string &Currency::get_name() const {
    return name;
}

int32_t Currency::get_current_rate() const {
    return get_rate(rates.size() - 1);
}

int32_t Currency::get_rate(size_t i) const {
    return 0 <= i && i < rates.size() ? rates[i] : -1;
}

int32_t Currency::get_absolute_change() const {
    return absolute_change;
}

int32_t Currency::get_relative_change() const {
    return relative_change;
}

