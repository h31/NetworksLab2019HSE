#ifndef SERVER_PRODUCT_H
#define SERVER_PRODUCT_H

#include <string>


struct product {
    public:
        std::string name;
        size_t price;
        size_t amount;

        product(std::string name,
                size_t price,
                size_t amount) : name(std::move(name)), price(price), amount(amount) {}
};

#endif //SERVER_PRODUCT_H
