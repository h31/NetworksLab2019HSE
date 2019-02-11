#include <utility>

//
// Created by Olga Alehina on 2019-02-10.
//

#ifndef SERVER_DATA_H
#define SERVER_DATA_H

#include <set>
#include <string>


enum HistoryEventType {
    REMITTANCE_REQUEST
};

struct HistoryEvent {
    HistoryEventType eventType;
    uint8_t walletNumberFrom;
    uint8_t walletNumberTo;
    uint8_t count;
};

inline bool operator<(const HistoryEvent &historyEvent1, const HistoryEvent &historyEvent2) {
    if (historyEvent1.walletNumberTo != historyEvent2.walletNumberTo) {
        return historyEvent1.walletNumberTo < historyEvent2.walletNumberTo;
    }

    return historyEvent1.walletNumberFrom < historyEvent2.walletNumberFrom;
}

struct Wallet {
    uint64_t number;
    std::string password;
    uint64_t count;

    Wallet(uint64_t number, std::string password, uint64_t count) : number(number),
                                                                    password(std::move(password)),
                                                                    count(count) {}
};


inline bool operator<(const Wallet &w1, const Wallet &w2) {
    return w1.number < w2.number;
}

class Data {
public:
    void addNewWaller(Wallet wallet);

    uint64_t getFreeNumber();

    std::set<uint64_t> getNumbers();

private:

    std::set<Wallet> wallets;
    std::set<HistoryEvent> events;
};


#endif //SERVER_DATA_H
