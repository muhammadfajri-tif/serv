#if !defined(utils_H)
#define utils_H

#include "http.h"

/* Lowercase a string */
char *strlower(char *s);

/* Function to check weather file exist or not.
  Return 0 if exist, otherwise 1 */
int file_exists(const char *file_name);

/* Function to read file and write to stdout.
  Return 0 if there's error, otherwise 1 */
int read_file(const char *file_name);

/* Procedure to handle escape characters */
void uri_unescape(char *uri);

/* Procedure to write log to file and the coresponding file descriptor */
void logger(int socket_fd, enum status_code type, char *s1, char *s2);

#endif // !defined (utils_H)
