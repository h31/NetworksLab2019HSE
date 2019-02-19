#ifndef RATING_SERVER_RATING_H
#define RATING_SERVER_RATING_H


#include <cstdint>
#include <string>

struct Rating {
  enum State : uint8_t {
    CLOSE = 0, OPEN = 1
  };
  std::string name;
  uint32_t id;
  uint8_t size, capacity;
  std::string **choices;
  uint32_t *statistics;
  State state = CLOSE;

  Rating(uint32_t id, std::string name, uint8_t capacity);

  Rating() = default;

  ~Rating();

  bool serialise(const std::string &path);

  bool add_choice(std::string &choice);

  static Rating *createRating(const std::string &folder, const std::string &filename);
};


#endif //RATING_SERVER_RATING_H
