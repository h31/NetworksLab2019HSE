#include <utility>

//
// Created by Olga Alehina on 2019-02-10.
//

#ifndef SERVER_DATA_H
#define SERVER_DATA_H

#include <set>
#include <string>


enum HistoryEventType {
    REMITTANCE_REQUEST,
    NOTHING
};

struct HistoryEvent {
    HistoryEventType eventType;
    uint64_t walletNumberFrom;
    uint64_t walletNumberTo;
    uint64_t count;
    uint64_t id;
    bool isClosed;

    HistoryEvent(HistoryEventType eventType,
                 uint64_t walletNumberFrom,
                 uint64_t walletNumberTo,
                 uint64_t count,
                 uint64_t id,
                 bool isClosed) : eventType(eventType),
                                  walletNumberFrom(walletNumberFrom),
                                  walletNumberTo(walletNumberTo),
                                  count(count),
                                  id(id),
                                  isClosed(isClosed) {}

};

inline bool operator<(const HistoryEvent &historyEvent1, const HistoryEvent &historyEvent2) {
    return historyEvent1.id < historyEvent2.id;
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

    HistoryEvent getEvent(uint64_t id);

    void addHistoryEvents(uint64_t numFrom, uint64_t numTo, uint64_t count, HistoryEventType historyEventType);

    bool isExist(uint64_t num, std::string password);

    bool setHistoryStatus(uint64_t id, bool isClosed);

    uint64_t getMoney(uint64_t num, std::string password);

    uint64_t getFreeNumber();

    void print();

    bool decMoney(uint64_t num, uint64_t count);

    bool incMoney(uint64_t num, uint64_t count);

    std::set<HistoryEvent> getRequests(uint64_t num);

    uint64_t getEnvetId() {
        return events.size();
    }

    std::set<uint64_t> getNumbers();

private:
    std::set<Wallet> wallets;
    std::set<HistoryEvent> events;
};


#endif //SERVER_DATA_H
