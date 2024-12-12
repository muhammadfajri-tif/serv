#include "net.h"

int main(int argc, char *argv[]) {
  // specify port otherwise use default port
  char *port = argc == 1 ? PORT : argv[1];

  start_server(port);

  return 0;
}
