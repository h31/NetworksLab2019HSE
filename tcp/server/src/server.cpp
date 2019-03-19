//
// Created by karvozavr on 07/02/19.
//

#include "server.hpp"
#include "logger.hpp"

server::server(uint16_t port) : port(port) {
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("Error opening socket");
  }

  int k = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed.");
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
  for (std::thread *t : threads) {
    t->join();
  }
  close(socket_fd);
}

void server::start() {
  write_log("Server listening on port: " + std::to_string(port));
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
  write_log("Connected");
  while (true) {

    ssize_t n = read(client_socket_fd, (char *) &header, sizeof(header));
    size_t bytes_read = n;

    while (n > 0 && bytes_read < sizeof(header)) {
      n = read(client_socket_fd, ((char *) &header) + bytes_read, sizeof(header) - bytes_read);
      bytes_read += n;
    }

    if (n < 0) {
      perror("ERROR reading from socket");
    }

    if (n == 0) {
      break;
    }

    bool success;
    switch (header.type) {
      case REGISTER:
        success = handle_register(client_socket_fd, header);
        write_log("Register");
        break;
      case CHECK_LOGIN:
        success = handle_check_login(client_socket_fd, header);
        write_log("Check login: " + std::string(header.wallet_id));
        break;
      case GET_ALL_WALLETS:
        success = handle_get_all_wallets(client_socket_fd, header);
        write_log("Get all: " + std::string(header.wallet_id));
        break;
      case ACCOUNT_INFO:
        success = handle_account_info(client_socket_fd, header);
        write_log("Account info: " + std::string(header.wallet_id));
        break;
      case PAYMENT:
        success = handle_payment(client_socket_fd, header);
        write_log("Payment: " + std::string(header.wallet_id));
        break;
      case ASK_FOR_PAYMENT:
        success = handle_ask_for_payment(client_socket_fd, header);
        write_log("Ask for payment: " + std::string(header.wallet_id));
        break;
      case CONFIRM_PAYMENT:
        success = handle_confirm_payment(client_socket_fd, header);
        write_log("Confirm payment: " + std::string(header.wallet_id));
        break;
      case GET_REQUESTS_FOR_PAYMENTS:
        success = handle_get_payment_requests(client_socket_fd, header);
        write_log("Requests: " + std::string(header.wallet_id));
        break;
      case PAYMENT_RESULTS:
        success = handle_payment_results(client_socket_fd, header);
        write_log("Results: " + std::string(header.wallet_id));
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

bool check_authentification(std::unordered_map<std::string, wallet> const &wallets,
                            std::unordered_map<std::string, wallet>::iterator const &wallet_,
                            pstp_request_header const &header) {
  return wallet_ != wallets.end() && std::string(header.password) == (*wallet_).second.password;
}

bool server::handle_check_login(int client_socket_fd, pstp_request_header const &header) {
  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet = wallets.find(id);
  if (check_authentification(wallets, wallet, header)) {
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

  if (check_authentification(wallets, wallet_id, header)) {
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
  threads.push_back(thread);
}

bool server::handle_account_info(int client_socket_fd, pstp_request_header const &header) {
  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);
  if (check_authentification(wallets, wallet_, header)) {
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
  pstp_payment_request request;
  request.header = header;

  ssize_t red = read(client_socket_fd, ((char *) &request) + sizeof(header), header.content_size);

  if (red != header.content_size) {
    return false;
  }

  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);

  if (check_authentification(wallets, wallet_, header)) {
    wallet &w = wallet_->second;
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
  pstp_ask_for_payment_request request;
  request.header = header;

  ssize_t red = read(client_socket_fd, ((char *) &request) + sizeof(header), header.content_size);

  if (red != header.content_size) {
    return false;
  }

  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);

  if (check_authentification(wallets, wallet_, header)) {
    wallet &w = wallet_->second;
    if (wallets.find(request.recipient_id) == wallets.end() || request.amount == 0) {
      wallets_mutex.unlock();
      auto response = pstp_ask_for_payment_response(INVALID_CONTENT);
      return send_simple_response(client_socket_fd, response);
    } else {
      wallet &recipient = wallets.find(std::string(request.recipient_id))->second;
      recipient.payment_requests.insert({w.wallet_id, request.amount});
      wallets_mutex.unlock();
      auto response = pstp_ask_for_payment_response(OK);
      return send_simple_response(client_socket_fd, response);
    }
  } else {
    wallets_mutex.unlock();
    auto response = pstp_ask_for_payment_response(INVALID_PASSWORD);
    return send_simple_response(client_socket_fd, response);
  }
}

bool server::handle_confirm_payment(int client_socket_fd, pstp_request_header const &header) {
  pstp_confirm_payment_request request;
  request.header = header;

  ssize_t red = read(client_socket_fd, ((char *) &request) + sizeof(header), header.content_size);

  if (red != header.content_size) {
    return false;
  }

  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);

  if (check_authentification(wallets, wallet_, header)) {
    wallet &w = wallet_->second;
    if (wallets.find(request.recipient_id) == wallets.end() ||
        w.payment_requests.find(request.recipient_id) == w.payment_requests.end() ||
        (request.amount > w.balance) ||
        (w.payment_requests.find(request.recipient_id)->second != request.amount &&
         request.amount != 0)) {
      wallets_mutex.unlock();
      auto response = pstp_confirm_payment_response(INVALID_CONTENT);
      return send_simple_response(client_socket_fd, response);
    } else {
      wallet &recipient = wallets.find(std::string(request.recipient_id))->second;
      recipient.payment_results.emplace_back(w.wallet_id, request.amount);
      w.payment_requests.erase(recipient.wallet_id);

      recipient.balance += request.amount;
      w.balance -= request.amount;

      wallets_mutex.unlock();

      auto response = pstp_confirm_payment_response(OK);
      return send_simple_response(client_socket_fd, response);
    }
  } else {
    wallets_mutex.unlock();
    auto response = pstp_confirm_payment_response(INVALID_PASSWORD);
    return send_simple_response(client_socket_fd, response);
  }
}

bool server::handle_get_payment_requests(int client_socket_fd, pstp_request_header const &header) {
  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);

  if (check_authentification(wallets, wallet_, header)) {
    wallet &w = wallet_->second;
    std::vector<std::pair<id_type, money_type>> requests;
    for (auto const &entry : w.payment_requests) {
      requests.emplace_back(entry);
    }
    wallets_mutex.unlock();

    auto response = pstp_get_request_for_payments_response(OK, requests);
    return send_serializable_response(client_socket_fd, response);
  } else {
    wallets_mutex.unlock();
    auto response = pstp_get_request_for_payments_response(INVALID_PASSWORD);
    return send_serializable_response(client_socket_fd, response);
  }
}

bool server::handle_payment_results(int client_socket_fd, pstp_request_header const &header) {
  wallets_mutex.lock();
  id_type id = header.wallet_id;
  auto wallet_ = wallets.find(id);
  if (check_authentification(wallets, wallet_, header)) {
    wallet &w = wallet_->second;
    auto response = pstp_get_payment_results_response(OK, w.payment_results);
    w.payment_results = {};
    wallets_mutex.unlock();
    return send_serializable_response(client_socket_fd, response);
  } else {
    wallets_mutex.unlock();
    auto response = pstp_get_payment_results_response(INVALID_PASSWORD);
    return send_serializable_response(client_socket_fd, response);
  }
}

int server::get_socket_fd() {
  return socket_fd;
}
