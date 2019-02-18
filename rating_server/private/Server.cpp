#include "Server.h"

#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <Server.h>


#include "Client.h"

static const int TRUE_VALUE = 1;
static constexpr const char *EXIT_COMMAND = "exit";
static const int BACKLOG_SIZE = 5;
static const char *const NO_SUCH_RATING_MESSAGE = "There is no such rating id";

Server::Server(unsigned short port) {
  server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_fd < 0) {
    perror("Error opening socket");
    return;
  }
  if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &TRUE_VALUE, sizeof(TRUE_VALUE)) < 0) {
    perror("Error setting SO_REUSEADDR");
      shutdown_socket();
    return;
  }
  struct sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (bind(server_socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("Error binding socket");
      shutdown_socket();
    return;
  }
}

Server::~Server() {
    shutdown_socket();
}

void Server::run() {
  if (server_socket_fd < 0) {
    return;
  }
  listen(server_socket_fd, BACKLOG_SIZE);

  std::thread accept_thread([](Server *server) {
      std::vector<std::thread> threads;
      while (true) {
        int client_socket_fd = server->accept();
        if (client_socket_fd < 0) {
          break;
        }
        auto client = new Client(client_socket_fd);
        std::thread client_thread(*client, server);
        threads.push_back(std::move(client_thread));
      }
      perror("Error on accept");
      server->shutdown_all_clients();
      for (auto &thread : threads) {
        thread.join();
      }
    }, this);

  while (true) {
    std::string input;
    std::cin >> input;
    if (input == EXIT_COMMAND) {
      break;
    }
    std::cout << "To exit enter: " << EXIT_COMMAND << std::endl;
  }
    shutdown_socket();
  accept_thread.join();
}

bool Server::add_client(const std::string &user_name, Client *client) {
  std::unique_lock<std::mutex> lock(active_clients_mtx);
  if (active_clients.count(user_name)) {
    return false;
  }
  active_clients[user_name] = client;
  return true;
}

void Server::remove_client(const std::string &user_name) {
  std::unique_lock<std::mutex> lock(active_clients_mtx);
  active_clients.erase(user_name);
}

//void Server::SendToAll(const std::string &from, const GroupMessageMessage &message) {
//  std::unique_lock<std::mutex> lock(active_clients_mtx);
//  for (const auto &client : active_clients) {
//    if (client.first != from) {
//      client.second->Send(message);
//    }
//  }
//}
//
//bool Server::SendTo(const std::string &from, const std::string &to, const MessageMessage &message) {
//  std::unique_lock<std::mutex> lock(active_clients_mtx);
//  if (!active_clients.count(to)) {
//    return false;
//  }
//  active_clients[to]->Send(message);
//  return true;
//}

void Server::shutdown_socket() {
  static std::mutex mtx;
  std::unique_lock<std::mutex> lock(mtx);
  if (server_socket_fd < 0) {
    return;
  }
  shutdown(server_socket_fd, SHUT_RDWR);
  server_socket_fd = -1;
}

void Server::shutdown_all_clients() {
  std::unique_lock<std::mutex> lock(active_clients_mtx);
  for (const auto &client : active_clients) {
//    if (client.second->Send(DisconnectMessage())) {
//      client.second->shutdown_socket();
//    }
  }
}

int Server::accept() {
  struct sockaddr_in client_address{};
  unsigned int client_length = sizeof(client_address);
  return ::accept(server_socket_fd, (struct sockaddr *) &client_address, &client_length);
}

bool Server::create_new_rating(std::string &name, uint8_t cnt, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  ratings[maxId] = Rating(maxId, name, cnt);
  maxId++;
  return client->send_success();
}

bool Server::delete_rating(uint32_t id, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  Rating &lol = ratings[id];
  if (0 < ratings.erase(id)) {
    return client->send_success();
  }
  return client->send_error(NO_SUCH_RATING_MESSAGE);
}

bool Server::open_rating(uint32_t id, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  if (!ratings.count(id)) {
    return client->send_error(NO_SUCH_RATING_MESSAGE);
  }
  ratings[id].state = Rating::OPEN;
  return client->send_success();
}

bool Server::close_rating(uint32_t id, Client* client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  if (!ratings.count(id)) {
    return client->send_error(NO_SUCH_RATING_MESSAGE);
  }
  ratings[id].state = Rating::CLOSE;
  return client->send_success();
}

bool Server::add_choice(uint32_t id, std::string &choice, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  if (!ratings.count(id)) {
    return client->send_error(NO_SUCH_RATING_MESSAGE);
  }
  auto &rating = ratings[id];
  if (!rating.add_choice(choice)) {
    return client->send_error("The rating is full");
  }
  return client->send_success();
}

bool Server::rating_list(Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  Socket &socket = client->getSocket();
  if (!socket.write_uint32((uint32_t) ratings.size())) return false;
  for (auto &rating : ratings) {
    Rating &r = rating.second;
    if (!(socket.write_string(r.name) && socket.write_uint32(r.id) && socket.write_default(r.state))) return false;
  }
  return true;
}

bool Server::show_rating(uint32_t id, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  if (!ratings.count(id)) {
    return client->send_error(NO_SUCH_RATING_MESSAGE);
  }
  Socket &socket = client->getSocket();
  auto &r = ratings[id];
  if (!(socket.write_string(r.name) && socket.write_default(r.state) && socket.write_default(r.size))) return false;
  for (int i = 0; i < r.size; i++) {
    if (!(socket.write_string(r.choices[i]) && socket.write_uint32(r.statistics[i]))) return false;
  }
  return true;
}

bool Server::vote_rating(uint32_t id, uint8_t choice, Client *client) {
  std::unique_lock<std::mutex> lock(ratings_mtx);
  if (!ratings.count(id)) {
    return client->send_error(NO_SUCH_RATING_MESSAGE);
  }
  auto &rating = ratings[id];
  if (choice >= rating.size) {
    return client->send_error("There is no such choice option");
  }
  rating.statistics[choice]++;
  return client->send_success();
}
