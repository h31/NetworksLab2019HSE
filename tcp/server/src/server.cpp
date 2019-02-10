//
// Created by karvozavr on 07/02/19.
//

#include "server.hpp"

void term_handler(int signum) {
    done = 1;
}

server::server(uint16_t port) : port(port) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        perror("Error opening socket");
    }

    /* Initialize socket structure */
    bzero((char *) &server_addr, sizeof(server_addr));


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Error binding socket.");
    }
}

server::~server() {
    shutdown(socket_fd, SHUT_RD);
    close(socket_fd);
}

void server::start() {
    listen(socket_fd, 5);

    int client_socket_fd;
    unsigned int client_size = sizeof(client_addr);

    while (!done) {
        client_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_size);

        if (client_socket_fd < 0) {
            perror("ERROR on accept");
            continue;
        }

        new_client(client_socket_fd);
    }
}

void server::handle_client(int client_socket_fd) {
    pstp_request_header header;

    while (true) {

        ssize_t n = read(client_socket_fd, (char *) &header, sizeof(header));

        if (n < 0) {
            perror("ERROR reading from socket");
        }

        if (n == 0) {
            break;
        }

        bool success;
        std::cout << "Connected: " << header.wallet_id << '\n';
        switch (header.type) {
            case REGISTER:
                success = handle_register(client_socket_fd, header);
                std::cout << "Register" << '\n';
                break;
            case CHECK_LOGIN:
                success = handle_check_login(client_socket_fd, header);
                std::cout << "Check login: " << header.wallet_id << '\n';
                break;
            case GET_ALL_WALLETS:
                success = handle_get_all_wallets(client_socket_fd, header);
                std::cout << "Get all: " << header.wallet_id << '\n';
                break;
            case ACCOUNT_INFO:
                success = handle_account_info(client_socket_fd, header);
                std::cout << "Account info: " << header.wallet_id << '\n';
                break;
            case PAYMENT:
                success = handle_payment(client_socket_fd, header);
                std::cout << "Payment: " << header.wallet_id << '\n';
                break;
            default:
                auto response = pstp_response_header{0, UNSUPPORTED_REQUEST_TYPE, 0};
                success = send_simple_response(client_socket_fd, response);
                break;
        }

        if (!success) {
            break;
        }
    }

    shutdown(client_socket_fd, SHUT_RD);
    close(client_socket_fd);
}

bool server::handle_register(int client_socket_fd, pstp_request_header const &header) {
    wallets_mutex.lock();
    id_type id = std::to_string(wallets.size());
    wallet w = wallet(id, header.password);
    w.balance = 10000L;
    wallets.insert({id, w});
    wallets_mutex.unlock();

    auto response = pstp_register_response(OK, id);
    return send_simple_response(client_socket_fd, response);
}

bool server::handle_check_login(int client_socket_fd, pstp_request_header const &header) {
    wallets_mutex.lock();
    id_type id = header.wallet_id;
    auto wallet = wallets.find(id);
    if (wallets.find(id) != wallets.end() && std::string(header.password) == (*wallet).second.password) {
        wallets_mutex.unlock();
        auto response = pstp_check_login_response(OK);
        return send_simple_response(client_socket_fd, response);
    } else {
        wallets_mutex.unlock();
        auto response = pstp_check_login_response(INVALID_PASSWORD);
        return send_simple_response(client_socket_fd, response);
    }
}

bool server::handle_get_all_wallets(int client_socket_fd, pstp_request_header const &header) {
    wallets_mutex.lock();
    id_type id = header.wallet_id;
    auto wallet_id = wallets.find(id);

    if (wallets.find(id) != wallets.end() && std::string(header.password) == (*wallet_id).second.password) {
        std::vector<id_type> wallet_ids;
        for (auto const &entry : wallets) {
            wallet_ids.push_back(entry.first);
        }
        wallets_mutex.unlock();

        auto response = pstp_get_all_wallets_response(OK, wallet_ids);
        return send_serializable_response(client_socket_fd, response);
    } else {
        wallets_mutex.unlock();
        auto response = pstp_get_all_wallets_response(INVALID_PASSWORD);
        return send_serializable_response(client_socket_fd, response);
    }
}

void server::new_client(int client_socket_fd) {
    std::thread *thread = new std::thread(&server::handle_client, this, client_socket_fd);
}

bool server::
handle_account_info(int client_socket_fd, pstp_request_header const &header) {
    wallets_mutex.lock();
    id_type id = header.wallet_id;
    auto wallet_ = wallets.find(id);
    if (wallets.find(id) != wallets.end() && std::string(header.password) == (*wallet_).second.password) {
        auto response = pstp_account_info_response(OK, (*wallet_).second.balance);
        wallets_mutex.unlock();
        return send_simple_response(client_socket_fd, response);
    } else {
        wallets_mutex.unlock();
        auto response = pstp_account_info_response(INVALID_PASSWORD);
        return send_simple_response(client_socket_fd, response);
    }
}

bool server::handle_payment(int client_socket_fd, pstp_request_header const &header) {
    char request_data[sizeof(pstp_payment_request)];
    pstp_payment_request &request = reinterpret_cast<pstp_payment_request &>(request_data);

    wallets_mutex.lock();
    id_type id = header.wallet_id;
    auto wallet_ = wallets.find(id);
    wallet &w = wallet_->second;
    if (wallets.find(id) != wallets.end() && std::string(header.password) == w.password) {
        if (request.amount > w.balance || wallets.find(request.recipient_id) == wallets.end()) {
            wallets_mutex.unlock();
            auto response = pstp_payment_response(INVALID_CONTENT);
            return send_simple_response(client_socket_fd, response);
        } else {
            wallet &recipient = wallets.find(std::string(request.recipient_id))->second;
            recipient.balance += request.amount;
            w.balance -= request.amount;
            wallets_mutex.unlock();
            auto response = pstp_payment_response(OK);
            return send_simple_response(client_socket_fd, response);
        }
    } else {
        wallets_mutex.unlock();
        auto response = pstp_account_info_response(INVALID_PASSWORD);
        return send_simple_response(client_socket_fd, response);
    }
}

bool server::handle_ask_for_payment(int client_socket_fd, pstp_request_header const &header) {
    char request_data[sizeof(pstp_ask_for_payment_request)];
    pstp_ask_for_payment_request &request = reinterpret_cast<pstp_ask_for_payment_request &>(request_data);

    wallets_mutex.lock();
    id_type id = header.wallet_id;
    auto wallet_ = wallets.find(id);
    wallet &w = wallet_->second;

    if (wallets.find(id) != wallets.end() && std::string(header.password) == w.password) {
        if (wallets.find(request.recipient_id) == wallets.end()) {
            wallets_mutex.unlock();
            auto response = pstp_payment_response(INVALID_CONTENT);
            return send_simple_response(client_socket_fd, response);
        } else {
            wallet &recipient = wallets.find(std::string(request.recipient_id))->second;
            recipient.payment_requests.insert({request.recipient_id, request.amount});
            wallets_mutex.unlock();
            auto response = pstp_payment_response(OK);
            return send_simple_response(client_socket_fd, response);
        }
    } else {
        wallets_mutex.unlock();
        auto response = pstp_account_info_response(INVALID_PASSWORD);
        return send_simple_response(client_socket_fd, response);
    }
}
