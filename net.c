#include "net.h"
#include "http.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sem_t semaphore;

void sigchld_handler() {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

int create_socket(const char *port) {
  // INFO: Step 0: load up address structs
  int socketfd, status, yes = 1;
  struct addrinfo hints, *servinfo, *servtemp;

  memset(&hints, 0, sizeof(hints)); // makesure the struct is empty
  hints.ai_family = AF_INET;        // IPv4; use `AF_UNSPEC` to support all
  hints.ai_socktype = SOCK_STREAM;  // TCP stream socket
  hints.ai_flags = AI_PASSIVE;      // set my IP

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    perror("[ERRO] getaddrinfo");
    exit(1);
    return -1;
  }

  // Since getaddrinfo is a linkedlist,
  // we should loop through all the results,
  // and setup socket to the first node
  for (servtemp = servinfo; servtemp != NULL; servtemp = servtemp->ai_next) {
    // INFO: Step 1: Setup socket
    if ((socketfd = socket(servtemp->ai_family, servtemp->ai_socktype,
                           servtemp->ai_protocol)) == -1) {
      perror("[ERRO] Socket");
      exit(1);
      continue;
    }

    // INFO: Step 1.5: Avoid hogging port by reusing it
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      perror("[ERRO] setsockopt");
      exit(1);
    }

    // INFO: Step 2: Bind port and get the file descriptor
    if (bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
      close(socketfd);
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
    return -1;
  }

  return socketfd;
}

void start_listening(int socketfd, const char *port) {
  // INFO: Step 3: listening to the network
  if (listen(socketfd, BACKLOG) == -1) {
    perror("[ERRO] Failed to listen");
    exit(1);
  }

  printf("Server started %shttp://127.0.0.1:%s%s\n", "\033[92m", port,
         "\033[0m");
}

void handle_request(int socketfd) {
  // Ignore SIGCHLD to avoid zombie threads
  signal(SIGCHLD, SIG_IGN);

  // INFO: Step 4: Always accept connection
  while (1) {
    struct sockaddr_storage clientinfo;
    socklen_t addr_size = sizeof(clientinfo);

    // clientfd is a file descriptor for send and receive data to client
    int clientfd = accept(socketfd, (struct sockaddr *)&clientinfo, &addr_size);
    if (clientfd == -1) {
      perror("[ERRO] Failed to accept connection");
      exit(1);
    };

    sem_wait(&semaphore); // wait for available slot

    // NOTE: handle message using multi-process
    if (!fork()) {
      // no need listener, so we can close socket file descriptor,
      // and accept next connection
      close(socketfd);
      respond(clientfd);
      close(clientfd);
      sem_post(&semaphore); // release the semaphore
      exit(0);
    }

    close(clientfd);
  }
}

void start_server(const char *port) {
  int socketfd;
  struct sigaction sa;

  // initialize semaphore to handle multiconnection
  sem_init(&semaphore, 0, MAX_CONNECTION);

  socketfd = create_socket(port);

  start_listening(socketfd, port);

  // reap all deadge processes, avoid zombie because they're scary
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  handle_request(socketfd);
}
