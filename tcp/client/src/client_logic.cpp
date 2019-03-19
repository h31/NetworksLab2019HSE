#include <request.hpp>
#include <io_util.h>
#include <client_logic.h>


std::vector<std::string> get_all(Identifier& ident, int socket_descriptor) {
    struct pstp_get_all_request get_all_request(ident.login, ident.password);
    write_to_socket(socket_descriptor, (char *) &get_all_request, sizeof(get_all_request));

    struct pstp_response_header get_all_response_header = read_header(socket_descriptor);

    auto num_of_accounts = read_thing<uint32_t>(socket_descriptor);

    size_t acc_size_byte = get_all_response_header.content_size - sizeof(uint32_t);
    char buffer[acc_size_byte];
    read_from_socket(socket_descriptor, buffer, acc_size_byte);

    int ptr = 0;
    std::vector<std::string> result(num_of_accounts);
    for (int i = 0; i < num_of_accounts; i++) {
        result[i] = read_until_zero(&ptr, buffer);
    }
    return result;
}

uint64_t get_account_info(Identifier& ident, int socket_descriptor) {
    struct pstp_account_info_request account_info_request(ident.login, ident.password);
    write_to_socket(socket_descriptor, (char *) &account_info_request, sizeof(account_info_request));

    struct pstp_response_header account_info_response_header = read_header(socket_descriptor);

    uint32_t content_size = account_info_response_header.content_size;
    if (content_size != sizeof(uint64_t)) {
        error("header contract violation!");
    }

    return read_thing<uint64_t>(socket_descriptor);
}

bool payment(Identifier& ident, int socket_descriptor, std::string &id, uint64_t sum) {
    struct pstp_payment_request payment_request(ident.login, ident.password, id, sum);
    write_to_socket(socket_descriptor, (char *) &payment_request, sizeof(payment_request));

    struct pstp_response_header payment_response_header = read_header(socket_descriptor);
    return payment_response_header.code == OK;
}

std::vector<std::pair<std::string, std::string>> get_payment_results(Identifier& ident, int socket_descriptor) {
    std::vector<std::pair<std::string, std::string>> result;

    struct pstp_payment_results_request results_request(ident.login, ident.password);
    write_to_socket(socket_descriptor, (char *) &results_request, sizeof(results_request));

    struct pstp_response_header payment_response_header = read_header(socket_descriptor);

    auto num_of_results = read_thing<uint32_t>(socket_descriptor);

    if (num_of_results != 0) {
        size_t res_size_byte = payment_response_header.content_size - sizeof(uint32_t);
        char buffer[res_size_byte];
        read_from_socket(socket_descriptor, buffer, res_size_byte);

        int ptr = 0;
        for (int i = 0; i < num_of_results; i++) {
            std::string acc = read_until_zero(&ptr, buffer);
            std::string amount = read_until_zero(&ptr, buffer);
            result.emplace_back(acc, amount);
        }
    }
    return result;
}

std::vector<std::pair<std::string, std::string>> get_request_for_payments(Identifier& ident, int socket_descriptor) {
    std::vector<std::pair<std::string, std::string>> result;

    struct pstp_get_requests_for_payments_request get_rfp_request(ident.login, ident.password);
    write_to_socket(socket_descriptor, (char *) &get_rfp_request, sizeof(get_rfp_request));

    struct pstp_response_header get_request_for_payments_response_header = read_header(socket_descriptor);

    auto num_of_requests = read_thing<uint32_t>(socket_descriptor);

    if (num_of_requests != 0) {
        size_t res_size_byte = get_request_for_payments_response_header.content_size - sizeof(uint32_t);
        char buffer[res_size_byte];
        read_from_socket(socket_descriptor, buffer, res_size_byte);

        int ptr = 0;
        for (int i = 0; i < num_of_requests; i++) {
            std::string acc = read_until_zero(&ptr, buffer);
            std::string amount = read_until_zero(&ptr, buffer);
            result.emplace_back(acc, amount);
        }
    }

    return result;
}

bool ask_for_payment(Identifier& ident, int socket_descriptor, std::string &id, uint64_t sum) {
    struct pstp_ask_for_payment_request ask_for_payment_request(ident.login, ident.password, id, sum);
    write_to_socket(socket_descriptor, (char *) &ask_for_payment_request, sizeof(ask_for_payment_request));

    struct pstp_response_header payment_response_header = read_header(socket_descriptor);

    return payment_response_header.code == OK;
}

confirm_payment_status confirm_payment(Identifier& ident, int socket_descriptor, std::string &id, uint64_t sum) {
    struct pstp_confirm_payment_request confirm_payment_request(ident.login, ident.password, id, sum);
    write_to_socket(socket_descriptor, (char *) &confirm_payment_request, sizeof(confirm_payment_request));

    struct pstp_response_header payment_response_header = read_header(socket_descriptor);

    if (sum == 0) {
        if (payment_response_header.code == OK) {
            return confirm_payment_status::REJECTED_SUCCESSFULLY;
        } else {
            return confirm_payment_status::REJECTION_FAILED;
        }
    } else {
        if (payment_response_header.code == OK) {
            return confirm_payment_status::CONFIRMED_SUCCESSFULLY;
        } else {
            return confirm_payment_status::CONFIRMATION_FAILED;
        }
    }
}
