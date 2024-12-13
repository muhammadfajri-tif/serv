#if !defined(router_H)
#define router_H

#include "http.h"

extern enum status_code sc;

// Response
#define RESPONSE_PROTOCOL "HTTP/1.1"

#define HTTP_200 printf("%s %d OK\n\n", RESPONSE_PROTOCOL, OK)
#define HTTP_201 printf("%s %d Created\n\n", RESPONSE_PROTOCOL, CREATED)
#define HTTP_301                                                               \
  printf("%s %d Moved Permanently\n\n", RESPONSE_PROTOCOL, MOVED_PERMANENTLY)
#define HTTP_302 printf("%s %d Found\n\n", RESPONSE_PROTOCOL, FOUND)
#define HTTP_400 printf("%s %d Bad Request\n\n", RESPONSE_PROTOCOL, BAD_REQUEST)
#define HTTP_401                                                               \
  printf("%s %d Unauthorized\n\n", RESPONSE_PROTOCOL, UNAUTHORIZED)
#define HTTP_403 printf("%s %d Forbidden\n\n", RESPONSE_PROTOCOL, FORBIDDEN)
#define HTTP_404 printf("%s %d Not found\n\n", RESPONSE_PROTOCOL, NOTFOUND)
#define HTTP_500                                                               \
  printf("%s %d Internal Server Error\n\n", RESPONSE_PROTOCOL,                 \
         INTERNAL_SERVER_ERROR)
#define HTTP_501                                                               \
  printf("%s %d Not Implemented\n\n", RESPONSE_PROTOCOL, NOT_IMPLEMENTED_ERROR)

// some interesting macro for `route()`
#define ROUTE_START() if (0) {
#define ROUTE(METHOD, URI)                                                     \
  }                                                                            \
  else if (strcmp(URI, req.uri) == 0 && strcmp(METHOD, req.method) == 0) {
#define HEAD(URI) ROUTE("HEAD", URI)
#define GET(URI) ROUTE("GET", URI)
#define POST(URI) ROUTE("POST", URI)
#define PUT(URI) ROUTE("PUT", URI)
#define PATCH(URI) ROUTE("PATCH", URI)
#define DELETE(URI) ROUTE("DELETE", URI)
#define ROUTE_END()                                                            \
  }                                                                            \
  else HTTP_500;

#endif // !defined (router_H)
