#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * File descriptor for socket connection.
 */
static int sockfd;

void sigchld_handler(int s) {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

void start_server(const char *port) {
  // INFO: Step 0: load up address structs
  int status, yes = 1;
  struct addrinfo hints, *servinfo, *servtemp;
  struct sigaction sa;

  memset(&hints, 0, sizeof(hints)); // makesure the struct is empty
  hints.ai_family = AF_INET;        // IPv4; use `AF_UNSPEC` to support all
  hints.ai_socktype = SOCK_STREAM;  // TCP stream socket
  hints.ai_flags = AI_PASSIVE;      // set my IP

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    perror("[ERRO] getaddrinfo");
    exit(1);
  }

  // Since getaddrinfo is a linkedlist,
  // we should loop through all the results,
  // and setup socket to the first node
  for (servtemp = servinfo; servtemp != NULL; servtemp = servtemp->ai_next) {
    // INFO: Step 1: Setup socket
    if ((sockfd = socket(servtemp->ai_family, servtemp->ai_socktype,
                         servtemp->ai_protocol)) == -1) {
      perror("[ERRO] Socket");
      exit(1);
      continue;
    }

    // INFO: Step 1.5: Avoid hogging port by reusing it
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("[ERRO] setsockopt");
      exit(1);
    }

    // INFO: Step 2: Bind port and get the file descriptor
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
      close(sockfd);
      perror("[ERRO] Binding port");
      continue;
    }

    // exit loop immediately after found the first socket
    break;
  }

  // At this point, we no longer use servinfo anymore, so we can free
  freeaddrinfo(servinfo);

  if (servtemp == NULL) {
    perror("[ERRO] Server failed to bind");
    exit(1);
  }

  // INFO: Step 3: listening to the network
  if (listen(sockfd, BACKLOG) == -1) {
    perror("[ERRO] Failed to listen");
    exit(1);
  }

  // reap all deadge processes, avoid zombie because they're scary
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("[INFO] Server waiting for connection...\n");
}

void serv_forever(const char *port) {
  start_server(port);

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
}
