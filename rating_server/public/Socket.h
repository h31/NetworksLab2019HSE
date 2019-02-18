#ifndef RATING_SERVER_COMMUNICATOR_H
#define RATING_SERVER_COMMUNICATOR_H


#include <string>
#include <unistd.h>
#include "ServerMessage.h"

class Socket {
private:
  int socket;

public:
  explicit Socket(int socket) : socket(socket) {};

  ~Socket() {
    shutdown();
  }

  inline bool read(char *result, size_t len) {
    return ::read(socket, result, len) != len;
  }

  bool read_uint32(uint32_t &result);

  bool read_string(std::string &result);

  template<class T>
  bool read_default(T &result) {
    return read((char *) &result, sizeof(T));
  }

  inline bool write(const char *data, size_t len) {
    return ::write(socket, data, len) != len;
  }

  bool write_string(const std::string &data);


  bool write_uint32(uint32_t data);

  template<class T>
  bool write_default(const T &data) {
    return write((char *) &data, sizeof(T));
  }

  void shutdown();
};


#endif //RATING_SERVER_COMMUNICATOR_H
