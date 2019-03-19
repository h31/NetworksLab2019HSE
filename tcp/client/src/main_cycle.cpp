#include <iostream>
#include <limits>
#include <login.h>

#include "main_cycle.h"
#include "io_util.h"
#include "client_logic.h"

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
        println("Enter command (h for help): ");
        std::string code;
        std::cin >> code;

        if (code == "D") {
            println("Bye!");
            break;
        } else if (code == "H") {
            print_help();
        } else if (code == "GA") {
            auto result = get_all(ident, socket_descriptor);

            for (std::string& account: result) {
                std::cout << "Account name: " << account << std::endl;
            }
        } else if (code == "WI") {
            auto sum = get_account_info(ident, socket_descriptor);

            println("Your login: " + ident.login);
            println("You have " + std::to_string(sum) + " kukareks!");
        } else if (code == "PAY") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;

            if (payment(ident, socket_descriptor, user_id, sum)) {
                println("Payment succeeded!");
            } else {
                println("Payment failed!");
            }
        } else if (code == "RES") {
            auto result = get_payment_results(ident, socket_descriptor);

            if (result.empty()) {
                println("No results!");
            } else {
                for (const auto &payment: result) {
                    std::string acc = payment.first;
                    std::string amount = payment.second;

                    print("Account " + acc + " ");
                    if (amount != "0") {
                        println("payed: " + amount);
                    } else {
                        println(": payment failed or was rejected.");
                    }
                }
            }
        } else if (code == "RQT") {
            auto result = get_request_for_payments(ident, socket_descriptor);

            if (result.empty()) {
                println("No requests!");
            } else {
                for (const auto &request: result) {
                    println("Request from user " + request.first + " on sum " + request.second);
                }
            }
        } else if (code == "ASK") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;

            if (ask_for_payment(ident, socket_descriptor, user_id, sum)) {
                println("Request was sent! When user answers, payment result will be available.");
            } else {
                println("Request failed! Check if user-id was real or sum is correct.");
            }
        } else if (code == "ARQ") {
            std::string user_id;
            uint64_t sum;
            std::cin >> user_id >> sum;
            auto result = confirm_payment(ident, socket_descriptor, user_id, sum);

            switch (result) {
                case confirm_payment_status::CONFIRMED_SUCCESSFULLY:
                    println("Payment confirmation succeeded!");
                    break;
                case confirm_payment_status::CONFIRMATION_FAILED:
                    println("Payment confirmation failed!");
                    break;
                case confirm_payment_status::REJECTED_SUCCESSFULLY:
                    println("Payment successfully rejected!");
                    break;
                case confirm_payment_status::REJECTION_FAILED:
                    println("Something went wrong while rejecting!");
                    break;
            }
        } else {
            println("No such command!");
        }
    }
}

