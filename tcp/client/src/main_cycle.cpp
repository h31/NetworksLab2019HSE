#include <iostream>
#include <limits>
#include <login.h>
#include <request.hpp>
#include <response.hpp>

#include "main_cycle.h"
#include "util.h"

void print_help() {
    println("Functions list:");

    println("WI - to get wallet info;");

    println("PAY <receiver-id> <sum> - to pay to user with receiver-id sum;");

    println("RES - to see all unseen payment requests' results;");

    println("ASK <receiver-id> <sum> - to ask user with receiver-id for sum;");
    println("RQT - to see all requests of payment to your user;");
    println("ARQ <receiver-id> <sum | 0> - "
            "confirm request of payment by user with receiver-id for sum (sum must be the same as user requested), "
            "or reject it (then sum is 0);");

    println("GA - to get list of all wallets;");

    println("H - to show all functions;");
    println("D - to disconnect and exit.");

    println("");
}

void main_cycle(Identifier ident, int socket_descriptor) {
    while (true) {
        std::string code;
        std::cin >> code;

        if (code == "D") {
            println("Bye!");
            break;
        } else if (code == "H") {
            print_help();
        } else if (code == "GA") {
            struct pstp_get_all_request get_all_request(ident.login, ident.password);
            write_to_socket(socket_descriptor, (char *) &get_all_request, sizeof(get_all_request));

            struct pstp_response_header get_all_response_header = read_header(socket_descriptor);

            auto num_of_accounts = read_thing<uint32_t>(socket_descriptor);

            size_t acc_size_byte = get_all_response_header.content_size - sizeof(uint32_t);
            char buffer[acc_size_byte];
            read_from_socket(socket_descriptor, buffer, acc_size_byte);

            int ptr = 0;
            for (int i = 0; i < num_of_accounts; i++) {
                std::string acc = read_until_zero(&ptr, buffer, acc_size_byte);
                std::cout << "Account name: " << acc << std::endl;
            }
        } else if (code == "WI") {
            struct pstp_account_info_request account_info_request(ident.login, ident.password);
            write_to_socket(socket_descriptor, (char *) &account_info_request, sizeof(account_info_request));

            struct pstp_response_header account_info_response_header = read_header(socket_descriptor);

            uint32_t content_size = account_info_response_header.content_size;
            if (content_size != sizeof(uint64_t)) {
                error("contract violation!");
            }

            auto sum = read_thing<uint64_t>(socket_descriptor);
            println("You have " + std::to_string(sum) + " kukareks!");
        } else if (code == "PAY") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;

            struct pstp_payment_request payment_request(ident.login, ident.password, user_id, sum);
            write_to_socket(socket_descriptor, (char *) &payment_request, sizeof(payment_request));

            struct pstp_response_header payment_response_header = read_header(socket_descriptor);

            if (payment_response_header.code == OK) {
                println("Payment succeeded!");
            } else {
                println("Payment failed!");
            }
        } else if (code == "RES") {
            struct pstp_payment_results_request results_request(ident.login, ident.password);
            write_to_socket(socket_descriptor, (char *) &results_request, sizeof(results_request));

            struct pstp_response_header payment_response_header = read_header(socket_descriptor);

            auto num_of_results = read_thing<uint32_t>(socket_descriptor);

            if (num_of_results == 0) {
                println("No results!");
            } else {
                size_t res_size_byte = payment_response_header.content_size - sizeof(uint32_t);
                char buffer[res_size_byte];
                read_from_socket(socket_descriptor, buffer, res_size_byte);

                int ptr = 0;
                for (int i = 0; i < num_of_results; i++) {
                    std::string acc = read_until_zero(&ptr, buffer, res_size_byte);;
                    std::string payment = read_until_zero(&ptr, buffer, res_size_byte);

                    std::cout << "Account " << acc << " ";
                    if (payment != "0") {
                        std::cout << "payed: " << payment << std::endl;
                    } else {
                        std::cout << ": payment failed or was rejected." << std::endl;
                    }
                }
            }
        } else if (code == "RQT") {
            struct pstp_get_requests_for_payments_request get_rfp_request(ident.login, ident.password);
            write_to_socket(socket_descriptor, (char *) &get_rfp_request, sizeof(get_rfp_request));

            struct pstp_response_header payment_response_header = read_header(socket_descriptor);

            auto num_of_requests = read_thing<uint32_t>(socket_descriptor);

            if (num_of_requests == 0) {
                println("No requests!");
            } else {
                size_t res_size_byte = payment_response_header.content_size - sizeof(uint32_t);
                char buffer[res_size_byte];
                read_from_socket(socket_descriptor, buffer, res_size_byte);

                int ptr = 0;
                for (int i = 0; i < num_of_requests; i++) {

                    std::string acc = read_until_zero(&ptr, buffer, res_size_byte);;
                    std::string payment = read_until_zero(&ptr, buffer, res_size_byte);

                    std::cout << "Request from user " + acc;
                    std::cout << " on sum " << payment << std::endl;

                }
            }
        } else if (code == "ASK") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;
            struct pstp_ask_for_payment_request ask_for_payment_request(ident.login, ident.password, user_id, sum);
            write_to_socket(socket_descriptor, (char *) &ask_for_payment_request, sizeof(ask_for_payment_request));

            struct pstp_response_header payment_response_header = read_header(socket_descriptor);

            if (payment_response_header.code == OK) {
                println("Request was sent! When user answers, payment result will be available.");
            } else {
                println("Request failed! Check if user-id was real.");
            }
        } else if (code == "ARQ") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;
            struct pstp_confirm_payment_request confirm_payment_request(ident.login, ident.password, user_id, sum);
            write_to_socket(socket_descriptor, (char *) &confirm_payment_request, sizeof(confirm_payment_request));

            struct pstp_response_header payment_response_header = read_header(socket_descriptor);

            if (sum == 0) {
                if (payment_response_header.code == OK) {
                    println("Payment successfully rejected!");
                } else {
                    println("Something went wrong while rejecting!");
                }
            } else {
                if (payment_response_header.code == OK) {
                    println("Payment confirmation succeeded!");
                } else {
                    println("Payment confirmation failed!");
                }
            }
        }
    }
}

