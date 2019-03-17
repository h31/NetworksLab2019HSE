#include <utility>
#include <iostream>

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



void Data::print() {
    std::cout << "WALLETS" << std::endl;
    for (const auto &wallet : wallets) {
        std::cout << wallet.number << " " << wallet.count << std::endl;
    }

    std::cout << "Events" << std::endl;
    for (auto event : events) {
        std::cout << event.walletNumberFrom << " " << event.walletNumberTo << " " <<event.count << " " << event.isClosed << std::endl;
    }
}


bool Data::isExist(uint64_t num, std::string password) {
    Wallet findWallet(num, password, 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return false;
    }
    return (walletsIterator -> password == password);
}

uint64_t Data::getMoney(uint64_t num, std::string password) {
    Wallet findWallet(num, std::move(password), 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return 0;
    }
    return walletsIterator->count;
}

bool Data::decMoney(uint64_t num, uint64_t count) {
    Wallet findWallet(num, "", 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return false;
    }
    Wallet wallet = walletsIterator.operator*();
    if (wallet.count < count) {
        return false;
    }
    wallet.count -= count;
    wallets.erase(walletsIterator);
    wallets.insert(wallet);
    return true;
}

bool Data::incMoney(uint64_t num, uint64_t count) {
    Wallet findWallet(num, "", 0);
    auto walletsIterator = wallets.find(findWallet);
    if (walletsIterator == wallets.end()) {
        return false;
    }
    Wallet wallet = walletsIterator.operator*();
    wallet.count += count;
    wallets.erase(walletsIterator);
    wallets.insert(wallet);
    return true;
}

void Data::addHistoryEvents(uint64_t numFrom, uint64_t numTo, uint64_t count, HistoryEventType historyEventType) {
    HistoryEvent historyEvent(historyEventType, numFrom, numTo, count, getEnvetId(), false);
    Data::events.insert(historyEvent);
}

bool Data::setHistoryStatus(uint64_t id, bool isClosed) {
    for (HistoryEvent event : events) {
        if (event.id == id) {
            events.erase(event);
            event.isClosed = isClosed;
            events.insert(event);
            return true;
        }
    }
    return false;
}

HistoryEvent Data::getEvent(uint64_t id) {
    for (HistoryEvent event : events) {
        if (event.id == id) {
            return event;
        }
    }
    return HistoryEvent(NOTHING, 0, 0, 0, 0, true);
}

std::set<HistoryEvent> Data::getRequests(uint64_t num) {
    std::set<HistoryEvent> requestsForCurWallet;
    for (HistoryEvent event : events) {
        if (event.walletNumberTo == num && !event.isClosed) {
            requestsForCurWallet.insert(event);
        }
    }
    return requestsForCurWallet;
}

