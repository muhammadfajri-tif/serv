#include "router.h"
#include "utils.h"
#include <stdio.h>

void route() {
  ROUTE_START()

  GET("/") {
    char index_html[20];
    sprintf(index_html, "%s%s", ROOT_DIR, ROOT_FILE);

    HTTP_200;
    if (file_exists(index_html)) {
      read_file(index_html);
    } else {
      printf("Hello! You are using %s\n\n", request_header("User-Agent"));
    }
  }

  HEAD("/") {
    HTTP_200;
    header_t *h = request_headers();

    while (h->name) {
      printf("%s: %s\n", h->name, h->value);
      h++;
    }
  }

  GET("/test") {
    HTTP_200;

    printf("Hello. This is test page.");
  }

  POST("/") {
    HTTP_201;
    printf("Data sent %d bytes.\n", payload_size);
    if (payload_size > 0)
      printf("Request body: %s", req.payload);
  }

  GET(req.uri) {
    char file_name[255];
    snprintf(file_name, sizeof(file_name), "%s%s", ROOT_DIR, req.uri);

    if (file_exists(file_name)) {
      HTTP_200;
      read_file(file_name);
    } else {
      HTTP_404;
      sprintf(file_name, "%s%s", ROOT_DIR, NOT_FOUND_FILE);
      if (file_exists(file_name))
        read_file(file_name);
    }
  }

  ROUTE_END()
}
