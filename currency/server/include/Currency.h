#ifndef CURRENCY_H
#define CURRENCY_H

#include <vector>
#include <string>

class Currency {
public:
    static const int32_t ABSENT_CHANGE_VALUE = -1;

    Currency(std::string name, int32_t currentRate);
    
    const std::string &getName() const;

    int32_t getCurrentRate() const;

    const std::vector<int32_t> &getRates() const;
    
    void addRate(int32_t rate);

    int32_t getAbsoluteChange() const;

    int32_t getRelativeChange() const;

private:
    std::string name;
    std::vector<int32_t> rates;
    int32_t absoluteChange;
    int32_t relativeChange;
};

#endif // CURRENCY_H

