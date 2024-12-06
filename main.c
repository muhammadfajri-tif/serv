#include "net.h"

int main(int argc, char *argv[]) {
  // specify port otherwise use default port
  char *port = argc == 1 ? PORT : argv[1];

  serv_forever(port);

  return 0;
}
