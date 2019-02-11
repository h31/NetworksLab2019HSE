//
// Created by Olga Alehina on 2019-02-10.
//

#include "../include/Data.h"

void Data::addNewWaller(Wallet wallet) {
    wallets.insert(wallet);
}

std::set<uint64_t> Data::getNumbers() {
    std::set<uint64_t> result;
    for (const Wallet &wallet : wallets) {
       result.insert(wallet.number);
    }
    return result;
}

uint64_t Data::getFreeNumber() {
    return wallets.size();
}
