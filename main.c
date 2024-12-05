#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3000"
#define BACKLOG 10

int main(int argc, char *argv[]) {
  // INFO: Step 0: load up address structs
  int status;
  struct addrinfo hints, *servinfo;

  memset(&hints, 0, sizeof(hints)); // makesure the struct is empty
  hints.ai_family = AF_INET;        // IPv4; use `AF_UNSPEC` to support all
  hints.ai_socktype = SOCK_STREAM;  // TCP stream socket
  hints.ai_flags = AI_PASSIVE;      // set my IP

  if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    perror("[ERRO] getaddrinfo");
    exit(1);
  }

  // INFO: Step 1: Setup socket
  int sockfd;
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                       servinfo->ai_protocol)) == -1) {
    perror("[ERRO] Socket");
    exit(1);
  }

  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("[ERRO] setsockopt");
    exit(1);
  }

  // INFO: Step 2: Bind port and get the file descriptor
  if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(sockfd);
    perror("[ERRO] Binding port");
    exit(1);
  }

  // INFO: Step 3: listening to the network
  if (listen(sockfd, BACKLOG) == -1) {
    perror("[ERRO] Failed to listen");
    exit(1);
  }

  // At this point, we no longer use servinfo anymore, so we can free
  freeaddrinfo(servinfo);

  printf("[INFO] Server waiting for connection...\n");

  // INFO: Step 4: Always accept connection
  while (1) {
    struct sockaddr_storage clientinfo;
    socklen_t addr_size = sizeof(clientinfo);

    // newfd is a file descriptor for send and receive data to client
    int newfd = accept(sockfd, (struct sockaddr *)&clientinfo, &addr_size);
    if (newfd == -1) {
      perror("[ERRO] Failed to accept connection");
      exit(1);
    };

    // log client information
    char client_addr[INET_ADDRSTRLEN];
    inet_ntop(clientinfo.ss_family, (struct sockaddr *)&clientinfo.ss_family,
              client_addr, sizeof(client_addr));
    printf("\n[INFO] Server got connection from %s\n", client_addr);

    // NOTE: handle message using multi-process
    if (!fork()) {
      // no need listener, so we can close socket file descriptor,
      // and accept next connection
      close(sockfd);

      // INFO: Step 5.1: Send welcome message
      if (send(newfd, "Welcome!\n", 9, 0) == -1) {
        perror("[ERRO] Failed to sent message");
        exit(1);
      }

      // INFO: Step 5.2: Receive message
      char buffer_msg[256];
      int bytes_receive = recv(newfd, buffer_msg, sizeof(buffer_msg), 0);
      if (bytes_receive == -1) {
        perror("[ERRO] Failed receive message");
        exit(1);
      } else if (bytes_receive == 0) {
        perror("[WARN] Remote connection already close with status");
      } else {
        printf("Receive message from client: %s", buffer_msg);
      }
      printf("[INFO] Number of bytes receive from the client: %d\n",
             bytes_receive);

      // INFO: Step 5.3: Send exit message
      char *msg = "Thank you for using our service!\n";

      if (send(newfd, msg, strlen(msg), 0) == -1) {
        perror("[ERRO] Failed to sent message");
        exit(1);
      }

      close(newfd);
      exit(0);
    }

    close(newfd);
  }

  // unreachable code

  return 0;
}
