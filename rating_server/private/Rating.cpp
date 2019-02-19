#include <utility>

#include <fstream>
#include <sstream>
#include <Rating.h>
#include <iostream>

Rating::Rating(uint32_t id, std::string name, uint8_t capacity) : id(id), name(std::move(name)), capacity(capacity) {
  choices = new std::string *[capacity];
  statistics = new uint32_t[capacity];
}

bool Rating::serialise(const std::string &path) {
  std::stringstream i_to_str;
  i_to_str << id;

  std::ofstream file(path + i_to_str.str());
  std::cout << "file: " + path + i_to_str.str() << std::endl;
  file << name << std::endl;
  file << (int) capacity << ' ' << (int) size << ' ' << (int) state << std::endl;
  for (int i = 0; i < size; i++) {
    file << *choices[i] << std::endl;
    file << statistics[i] << std::endl;
  }
  file.close();
  return false;
}

bool Rating::add_choice(std::string &choice) {
  if (size == capacity) return false;
  choices[size] = new std::string(choice);
  statistics[size] = 0;
  size++;
  return true;
}

Rating *Rating::createRating(const std::string &folder, const std::string &filename) {
  auto id = static_cast<uint32_t>(std::stol(filename));
  std::ifstream file(folder + filename);
  std::string name;
  int size, capacity;
  std::getline(file, name);
  file >> capacity >> size;
  Rating *rating = new Rating(id, name, static_cast<uint8_t>(capacity));
  int state;
  file >> state;
  rating->size = static_cast<uint8_t>(size);
  rating->state = (State) state;
  for (int i = 0; i < size; i++) {
    file.get();
    rating->choices[i] = new std::string;
    std::getline(file, *rating->choices[i]);
    file >> rating->statistics[i];
  }
  return rating;
}

Rating::~Rating() {
  for (int i = 0; i < size; i++) {
    delete choices[i];
  }
  delete choices;
  delete statistics;
}
