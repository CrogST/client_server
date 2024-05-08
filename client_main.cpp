
#include <iostream>
#include <tuple>

#include "client.h"

static const bool _debug = false;

auto parse_args(auto argc, auto argv) {
  if(argc < 4) {
    std::cout << "client <name> <port number> <time>" << std::endl;
    exit(EXIT_FAILURE);
  }

  const char * name = argv[1];
  unsigned portno;
  unsigned time;
  try {
    portno = std::stoi(argv[2]);
    time = std::stoi(argv[3]);
  } catch(...) {
    std::cout << "client <name> <port number> <time>" << std::endl;
    exit(EXIT_FAILURE);
  }
  return std::make_tuple(name, portno, time);
}

int main(int argc, char * argv[]) {

  const char * name = argv[1];
  unsigned portno;
  unsigned time;
  if(_debug) {
    std::cout << "debug mode!" << std::endl;
    name = "client1";
    portno = 10005;
    time = 2;
  } else {
    std::tie(name, portno, time) = parse_args(argc, argv);
  }

  client_t c(name, portno, 2);
  c.run();
  return 0;
}
