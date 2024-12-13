#include "http.h"
#include "config.h"
#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct {
  int code;
  char *message;
} status_code_t[] = {
    {200, (char *)"OK"},
    {201, (char *)"Created"},
    {301, (char *)"Moved Permanently"},
    {302, (char *)"Found"},
    {400, (char *)"Bad Request"},
    {401, (char *)"Unauthorized"},
    {403, (char *)"Forbidden"},
    {404, (char *)"Not Found"},
    {500, (char *)"Internal Server Error"},
    {501, (char *)"Not Implemented"},
};

struct {
  char *ext;
  char *name;
} mime_type[] = {
    {0, "application/octet-stream"},
    {"gif", "image/gif"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"ico", "image/ico"},
    {"zip", "image/zip"},
    {"gz", "image/gz"},
    {"tar", "image/tar"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"css", "text/css"},
    {"txt", "text/plain"},
    {"js", "application/javascript"},
    {"json", "application/json"},
};

struct client_request_t req;
int payload_size;

/* Array for store HTTP request headers */
static header_t reqhdr[20] = {{(char *)"\0", (char *)"\0"}};

// get request header by name
char *request_header(const char *name) {
  header_t *h = reqhdr;
  while (h->name) {
    if (strcmp(h->name, name) == 0)
      return h->value;
    h++;
  }
  return NULL;
}

// get all request headers
header_t *request_headers() { return reqhdr; }

// Handle escape characters (%xx)
static void uri_unescape(char *uri) {
  char *src, *dst, chr = 0;
  src = dst = uri;

  // Skip inital non encoded character
  while (*src && !isspace((int)(*src)) && (*src != '%'))
    src++;

  // Replace encoded characters with corresponding code.
  dst = src;
  while (*src && !isspace((int)(*src))) {
    if (*src == '+')
      chr = ' ';
    else if ((*src == '%') && src[1] && src[2]) {
      src++;
      chr = ((*src & 0x0F) + 9 * (*src > '9')) * 16;
      src++;
      chr += ((*src & 0x0F) + 9 * (*src > '9'));
    } else
      chr = *src;
    *dst++ = chr;
    src++;
  }
  *dst = '\0';
}

// client connection
void respond(int clientfd) {
  int rcvd;

  char *buf = malloc(BUFFER_SIZE);
  rcvd = recv(clientfd, buf, BUFFER_SIZE, 0);

  if (rcvd < 0) // receive error
    perror("[ERRO] Something went wrong with `recv` function. Failed to "
           "receive data.\n");
  else if (rcvd == 0) // receive socket closed
    perror("Client disconnected unexpectedly.\n");
  else // message received
  {
    buf[rcvd] = '\0';

    req.method = strtok(buf, " \t\r\n");
    req.uri = strtok(NULL, " \t");
    req.prot = strtok(NULL, " \t\r\n");

    uri_unescape(req.uri);

    fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", req.method, req.uri);

    req.querystring = strchr(req.uri, '?');

    if (req.querystring)
      *req.querystring++ = '\0'; // split Req
    else
      req.querystring = req.uri - 1; // use an empty string

    header_t *h = reqhdr;
    char *t, *t2;
    while (h < reqhdr + 19) {
      char *key, *val;

      key = strtok(NULL, "\r\n: \t");
      if (!key)
        break;

      val = strtok(NULL, "\r\n");
      while (*val && *val == ' ')
        val++;

      h->name = key;
      h->value = val;
      h++;
      fprintf(stderr, "[H] %s: %s\n", key, val);
      t = val + 1 + strlen(val);
      if (t[1] == '\r' && t[2] == '\n')
        break;
    }
    t = strtok(NULL, "\r\n");
    t2 = request_header("Content-Length"); // and the related header if there is
    req.payload = t;
    payload_size = t2 ? atol(t2) : (rcvd - (t - buf));

    // bind clientfd to stdout, making it easier to write
    int client = clientfd;
    dup2(client, STDOUT_FILENO);
    close(client);
    sem_post(&semaphore); // release the semaphore

    // call router
    route();

    // clean up
    fflush(stdout);
    shutdown(STDOUT_FILENO, SHUT_WR);
    close(STDOUT_FILENO);
  }

  free(buf);
}
