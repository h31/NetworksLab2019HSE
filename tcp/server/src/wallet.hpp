#pragma once

#include <string>
#include <vector>
#include <map>

using id_type = std::string;
using money_type = uint64_t;


struct wallet {
    wallet(std::string wallet_id, std::string password)
            : wallet_id(std::move(wallet_id)),
              password(std::move(password)) {}

    std::string wallet_id;
    std::string password;
    money_type balance = 0;
    std::map<id_type, money_type> payment_requests = {};
    std::vector<std::pair<id_type, money_type>> payment_results = {};
};
