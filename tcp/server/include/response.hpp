#include <utility>

#pragma once

#include <cstdint>
#include <vector>
#include "request.hpp"

#pragma pack(0)
struct pstp_response_header {
    uint8_t type = 0;
    uint8_t code = 0;
    uint32_t content_size = 0;
};

enum return_code {
    OK = 0,
    INVALID_PASSWORD,
    UNSUPPORTED_REQUEST_TYPE,
    INVALID_CONTENT
};

/**
 * Register response.
 */
#pragma pack(0)

struct pstp_register_response {
    explicit pstp_register_response(uint8_t code, std::string const &wallet_id = "") {
        header = {REGISTER, code, 0};
        if (code == OK) {
            header.content_size = sizeof(this->wallet_id);
            strncpy(this->wallet_id, wallet_id.c_str(), std::min(wallet_id.length(), TEXT_UNIT_SIZE));
        }
    }

    pstp_response_header header;
    char wallet_id[TEXT_UNIT_SIZE] = "";
};

#pragma pack(0)

struct pstp_check_login_response {
    explicit pstp_check_login_response(uint8_t code)
            : header({CHECK_LOGIN, code, 0}) {}

    pstp_response_header header;
};


#pragma pack(0)

struct pstp_account_info_response {
    explicit pstp_account_info_response(uint8_t code, uint64_t money_amount = 0)
            : header({ACCOUNT_INFO, code, sizeof(uint64_t)}), money_amount(money_amount) {}

    pstp_response_header header;
    uint64_t money_amount = 0;
};

struct pstp_get_all_wallets_response {
    explicit pstp_get_all_wallets_response(uint8_t code, std::vector<std::string> accounts = std::vector<std::string>())
            : header({GET_ALL_WALLETS, code, 0}), accounts(std::move(accounts)) {}

    uint32_t content_size() {
        uint32_t size = sizeof(uint32_t);
        for (const std::string &val : accounts) {
            size += (val.length() + 1) * sizeof(char);
        }
        return size;
    }

    void serialize(uint8_t *buffer) {
        header.content_size = content_size();
        *(reinterpret_cast<pstp_response_header *>(buffer)) = header;
        buffer += sizeof(header);
        *(reinterpret_cast<uint32_t *>(buffer)) = static_cast<uint32_t>(accounts.size());
        buffer += sizeof(uint32_t);
        for (const std::string &account : accounts) {
            strncpy(reinterpret_cast<char *>(buffer), account.c_str(), account.length());
            buffer += account.length();
            *buffer = '\0';
            ++buffer;
        }
    }

    pstp_response_header header;
    std::vector<std::string> accounts;
};

#pragma pack(0)

struct pstp_payment_response {
    explicit pstp_payment_response(uint8_t code)
            : header({PAYMENT, code, 0}) {}

    pstp_response_header header;
};