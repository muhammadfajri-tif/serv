#if !defined(net_H)
#define net_H

#define PORT "3000"
#define BACKLOG 10

void sigchld_handler(int s);

void start_server(const char *port);

void serv_forever(const char *port);

#endif // !defined (net_H)
