#if !defined(http_H)
#define http_H

#include "config.h"
#include <semaphore.h>
#include <stdio.h>
#include <string.h>

/* HTTP Status Code */
enum status_code {
  ERROR = 0,
  LOG = 1,
  OK = 200,                    // 200
  CREATED = 201,               // 201
  MOVED_PERMANENTLY = 301,     // 301
  FOUND = 302,                 // 302
  BAD_REQUEST = 400,           // 400
  UNAUTHORIZED = 401,          // 401
  FORBIDDEN = 403,             // 403
  NOTFOUND = 404,              // 404
  INTERNAL_SERVER_ERROR = 500, // 500
  NOT_IMPLEMENTED_ERROR = 501  // 501
};

/* Type for HTTP Header */
typedef struct {
  char *name, *value;
} header_t;

/* Type for Client HTTP Request */
struct client_request_t {
  char *method;
  char *uri;
  char *querystring;
  char *prot;
  char *payload;
};

/* Semaphore for handle multi-connection */
extern sem_t semaphore;

/* Type for handle client request */
extern struct client_request_t req;

/* Buffer for store payload size */
extern int payload_size;

/* Array for store HTTP request headers */
static header_t reqhdr[20] = {{(char *)"\0", (char *)"\0"}};

/* Function for get HTTP request header by name */
char *request_header(const char *name);

/* Function for get all HTTP request headers */
header_t *request_headers(void);

/* Procedure for respond client request */
void respond(int clientfd);

/* Mock app. Define routes for each path and method */
void route();

#endif // !defined (http_H)
