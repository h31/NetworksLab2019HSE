#include "include/Currency.h"

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

Currency::Currency(string name, int32_t currentRate) : name(move(name)) {
    rates = {currentRate};
    absoluteChange = ABSENT_CHANGE_VALUE;
    relativeChange = ABSENT_CHANGE_VALUE;
}

const string &Currency::getName() const {
    return name;
}

int32_t Currency::getCurrentRate() const {
    return rates[rates.size() - 1];
}

const vector<int32_t> &Currency::getRates() const {
    return rates;
}

void Currency::addRate(int32_t rate) {
    int32_t lastRate = getCurrentRate();
    rates.push_back(rate);
    absoluteChange = rate - lastRate;
    relativeChange = static_cast<int32_t>((rate * 100.0) / lastRate);   
}

int32_t Currency::getAbsoluteChange() const {
    return absoluteChange;
}

int32_t Currency::getRelativeChange() const {
    return relativeChange;
}

