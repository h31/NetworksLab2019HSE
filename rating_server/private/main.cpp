#include <iostream>
#include <Server.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "should be port argument";
  }
  auto port = static_cast<uint16_t>(std::strtol(argv[1], nullptr, 10));
  Server server(port);
  server.run();
  return 0;
}