#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <cmath>

#pragma pack(0)

struct pstp_request_header {
#define TEXT_UNIT_SIZE (std::size_t)64

    explicit pstp_request_header(uint8_t type = 0,
                                 const std::string &wallet_id = "",
                                 const std::string &password = "",
                                 uint32_t content_size = 0)
            : type(type), content_size(content_size) {
        strncpy(this->wallet_id, wallet_id.c_str(), std::min(wallet_id.length(), TEXT_UNIT_SIZE));
        strncpy(this->password, password.c_str(), std::min(password.length(), TEXT_UNIT_SIZE));
    }

    uint8_t type = 0;
    char wallet_id[TEXT_UNIT_SIZE] = "";
    char password[TEXT_UNIT_SIZE] = "";
    uint32_t content_size = 0;
};

enum request_type {
    RESERVED = 0,
    REGISTER,
    CHECK_LOGIN,
    GET_ALL_WALLETS,
    ACCOUNT_INFO,
    ASK_FOR_PAYMENT,
    PAYMENT,
    CONFIRM_PAYMENT,
    GET_REQUESTS_FOR_PAYMENTS,
    PAYMENT_RESULTS
};

/**
 * Register request.
 */
#pragma pack(0)

struct pstp_register_request {
    explicit pstp_register_request(std::string const &password)
            : header(REGISTER, "", password, 0) {}

    pstp_request_header header;
};

#pragma pack(0)

struct pstp_check_login_request {
    pstp_check_login_request(const std::string &wallet_id, const std::string &password)
            : header(CHECK_LOGIN, wallet_id, password, 0) {}

    pstp_request_header header;
};

#pragma pack(0)

struct pstp_get_all_request {
    pstp_get_all_request(const std::string &wallet_id, const std::string &password)
            : header(GET_ALL_WALLETS, wallet_id, password, 0) {}

    pstp_request_header header;
};

#pragma pack(0)

struct pstp_account_info_request {
    pstp_account_info_request(const std::string &wallet_id, const std::string &password)
            : header(ACCOUNT_INFO, wallet_id, password, 0) {}

    pstp_request_header header;
};

#pragma pack(0)

struct pstp_payment_request {
    pstp_payment_request() = default;

    pstp_payment_request(const std::string &wallet_id,
                         const std::string &password,
                         const std::string &recipient_id,
                         uint64_t amount)
            : header(PAYMENT, wallet_id, password, sizeof(this->recipient_id) + sizeof(this->amount)),
              amount(amount) {
        strncpy(this->recipient_id, recipient_id.c_str(), std::min(recipient_id.length(), TEXT_UNIT_SIZE));
    }

    pstp_request_header header;
    uint64_t amount;
    char recipient_id[TEXT_UNIT_SIZE] = "";
};

#pragma pack(0)

struct pstp_ask_for_payment_request {
    pstp_ask_for_payment_request() = default;

    pstp_ask_for_payment_request(const std::string &wallet_id,
                                 const std::string &password,
                                 const std::string &recipient_id,
                                 uint64_t amount)
            : header(ASK_FOR_PAYMENT, wallet_id, password, sizeof(this->recipient_id) + sizeof(this->amount)),
              amount(amount) {
        strncpy(this->recipient_id, recipient_id.c_str(), std::min(recipient_id.length(), TEXT_UNIT_SIZE));
    }

    pstp_request_header header;
    uint64_t amount; // > 0
    char recipient_id[TEXT_UNIT_SIZE] = "";
};

#pragma pack(0)

struct pstp_confirm_payment_request {
    pstp_confirm_payment_request() = default;

    pstp_confirm_payment_request(const std::string &wallet_id,
                                 const std::string &password,
                                 const std::string &recipient_id,
                                 uint64_t amount)
            : header(CONFIRM_PAYMENT, wallet_id, password, sizeof(this->recipient_id) + sizeof(this->amount)),
              amount(amount) {
        strncpy(this->recipient_id, recipient_id.c_str(), std::min(recipient_id.length(), TEXT_UNIT_SIZE));
    }

    pstp_request_header header;
    uint64_t amount; // if 0 then payment is rejected
    char recipient_id[TEXT_UNIT_SIZE] = "";
};

#pragma pack(0)

struct pstp_get_requests_for_payments_request {
    pstp_get_requests_for_payments_request(const std::string &wallet_id, const std::string &password)
            : header(GET_REQUESTS_FOR_PAYMENTS, wallet_id, password, 0) {}

    pstp_request_header header;
};

#pragma pack(0)

struct pstp_payment_results_request {
    pstp_payment_results_request(const std::string &wallet_id, const std::string &password)
            : header(PAYMENT_RESULTS, wallet_id, password, 0) {}

    pstp_request_header header;
};