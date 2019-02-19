//
// Created by machine on 18-Feb-19.
//

#ifndef RATING_SERVER_SERVER_H
#define RATING_SERVER_SERVER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include <mutex>
#include "Client.h"
#include "Rating.h"

class Client;

class Server {
private:
  int server_socket_fd;
  std::unordered_map<std::string, Client *> active_clients;
  std::unordered_map<uint32_t, Rating *> ratings;
  std::mutex active_clients_mtx;
  std::mutex ratings_mtx;
  uint32_t max_id;

  void shutdown_socket();

public:
  explicit Server(unsigned short port);

  ~Server();

  int accept();

  void remove_client(const std::string &user_name);

  bool add_client(const std::string &user_name, Client *client);

  void run();

  void shutdown_all_clients();

  bool create_new_rating(std::string &name, uint8_t cnt, Client *client);

  bool delete_rating(uint32_t id, Client *client);

  bool open_rating(uint32_t id, Client *client);

  bool close_rating(uint32_t id, Client *client);

  bool add_choice(uint32_t id, std::string &choice, Client *client);

  bool rating_list(Client *client);

  bool show_rating(uint32_t i, Client *client);

  bool vote_rating(uint32_t id, uint8_t choice, Client *client);

  void save_all_state();

  void load_state();

  void read_data_file();
};

#endif //RATING_SERVER_SERVER_H
