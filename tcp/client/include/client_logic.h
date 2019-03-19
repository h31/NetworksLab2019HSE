#ifndef CLIENT_LOGIC_H
#define CLIENT_LOGIC_H

#include <string>
#include <vector>
#include "login.h"

std::vector<std::string> get_all(Identifier& ident, int socket_descriptor);

uint64_t get_account_info(Identifier& ident, int socket_descriptor);

bool payment(Identifier& ident, int socket_descriptor, std::string& id, uint64_t sum);

std::vector<std::pair<std::string, std::string>> get_payment_results(Identifier& ident, int socket_descriptor);

std::vector<std::pair<std::string, std::string>> get_request_for_payments(Identifier& ident, int socket_descriptor);

bool ask_for_payment(Identifier& ident, int socket_descriptor, std::string& id, uint64_t sum);

enum class confirm_payment_status {
    CONFIRMED_SUCCESSFULLY,
    CONFIRMATION_FAILED,
    REJECTED_SUCCESSFULLY,
    REJECTION_FAILED
};

confirm_payment_status confirm_payment(Identifier& ident, int socket_descriptor, std::string& id, uint64_t sum);

#endif //CLIENT_LOGIC_H
