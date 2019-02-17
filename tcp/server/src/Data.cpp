#include <utility>

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

bool Data::isExist(uint64_t num, std::string password) {
    Wallet findWallet(num, std::move(password), 0);
    return wallets.find(findWallet) != wallets.end();
}

uint64_t Data::getMoney(uint64_t num, std::string password) {
    Wallet findWallet(num, std::move(password), 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return 0;
    }
    return walletsIterator->count;
}

bool Data::incMoney(uint64_t num, int64_t count) {
    Wallet findWallet(num, "", 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return false;
    }
    Wallet wallet = walletsIterator.operator*();
    wallet.count += count;
    return true;
}

void Data::addHistoryEvents(uint64_t numFrom, uint64_t numTo, uint64_t count, HistoryEventType historyEventType) {
    HistoryEvent historyEvent(historyEventType, numFrom, numTo, count, getEnvetId(), false);
    Data::events.insert(historyEvent);
}

