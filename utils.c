#include "utils.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char *strlower(char *s) {
  for (char *p = s; *p != '\0'; p++)
    *p = tolower(*p);

  return s;
}

int file_exists(const char *file_name) {
  struct stat buffer;
  int exists = (stat(file_name, &buffer) == 0);

  return exists;
}

int read_file(const char *file_name) {
  char buf[CHUNK_SIZE];
  FILE *file;
  size_t nread;
  int err = 1;

  file = fopen(file_name, "r");

  if (file) {
    while ((nread = fread(buf, 1, sizeof buf, file)) > 0)
      fwrite(buf, 1, nread, stdout);

    err = ferror(file);
    fclose(file);
  }
  return err;
}

void logger(int socket_fd, enum status_code type, char *s1, char *s2) {
  int fd;
  char logbuffer[BUFFER_SIZE];

  switch (type) {
  case ERROR:
    sprintf(logbuffer, "ERROR: %s:%s Errno=%d exiting pid=%d", s1, s2, errno,
            getpid());
    break;
  case LOG:
    sprintf(logbuffer, " INFO: %s:%s:%d", s1, s2, socket_fd);
    break;
  case FORBIDDEN:
    (void)write(socket_fd,
                "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: "
                "close\nContent-Type: text/html\n\n<html><head>\n<title>403 "
                "Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe "
                "requested URL, file type or operation is not allowed on this "
                "simple static file webserver.\n</body></html>\n",
                271);
    sprintf(logbuffer, "HTTP/1.1 403 FORBIDDEN: %s:%s", s1, s2);
    break;
  case NOTFOUND:
    (void)write(socket_fd,
                "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: "
                "close\nContent-Type: text/html\n\n<html><head>\n<title>404 "
                "Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe "
                "requested URL was not found on this server.\n</body></html>\n",
                224);
    sprintf(logbuffer, "HTTP/1.1 404 NOT FOUND: %s:%s", s1, s2);
    break;
  case OK:
    sprintf(logbuffer, "HTTP/1.1 200 OK: %s:%s:%d", s1, s2, socket_fd);
  case CREATED:
    sprintf(logbuffer, "HTTP/1.1 201 CREATED: %s:%s:%d", s1, s2, socket_fd);
    break;
  case MOVED_PERMANENTLY:
    sprintf(logbuffer, "HTTP/1.1 301 MOVED_PERMANENTLY: %s:%s:%d", s1, s2,
            socket_fd);
    break;
  case FOUND:
    sprintf(logbuffer, "HTTP/1.1 302 FOUND: %s:%s:%d", s1, s2, socket_fd);
    break;
  case BAD_REQUEST:
    sprintf(logbuffer, "HTTP/1.1 400 BAD BAD REQUEST: %s:%s:%d", s1, s2,
            socket_fd);
    break;
  case UNAUTHORIZED:
    sprintf(logbuffer, "HTTP/1.1 401 UNAUTHORIZED: %s:%s:%d", s1, s2,
            socket_fd);
    break;
  case INTERNAL_SERVER_ERROR:
    sprintf(logbuffer, "HTTP/1.1 500 INTERNAL SERVER: %s:%s:%d", s1, s2,
            socket_fd);
    break;
  case NOT_IMPLEMENTED_ERROR:
    sprintf(logbuffer, "HTTP/1.1 501 NOT IMPLEMENTED: %s:%s:%d", s1, s2,
            socket_fd);
    break;
  }

  /* No checks here, nothing can be done with a failure anyway */
  if ((fd = open(LOG_FILE, O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0) {
    (void)write(fd, logbuffer, strlen(logbuffer));
    (void)write(fd, "\n", 1);
    (void)close(fd);
  }

  if (type == ERROR || type == NOTFOUND || type == FORBIDDEN)
    exit(3);
}
