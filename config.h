#if !defined(config_H)
#define config_H

/* Web Server Configuration */
// Port Number
#define PORT "8080"
// Buffer for store data from client
#define BUFFER_SIZE 4096
// Max length for queue of pending connection
#define BACKLOG 10
// Max connection for limit child processes
#define MAX_CONNECTION 1000

/* Static Content */
#define ROOT_DIR "./www"
#define ROOT_FILE "/index.html"
#define NOT_FOUND_FILE "/404.html"

/* Logging */
#define CHUNK_SIZE 1024
#define LOG_FILE "./serv.log"

#endif // !defined (config_H)
