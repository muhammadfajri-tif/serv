#if !defined(net_H)
#define net_H

#define PORT "3000"
#define BACKLOG 10

/* Signal handler to get errno from child process.
  Further used for reap all dead process to avoid zombie. */
void sigchld_handler();

/* Function to create socket and bind it to defined port.
  Return socket file descriptor. */
int create_socket(const char *port);

/* Procedure to start listening to incoming connection. */
void start_listening(int socketfd, const char *port);

/* Procedure for handling clients request.
  Handled using child processes. */
void handle_request(int socketfd);

/* Entry point to start server. */
void start_server(const char *port);

#endif // !defined (net_H)
