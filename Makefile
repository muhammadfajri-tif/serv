CC ?= gcc
CFLAGS += -g -Wall -Wextra -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code

DEST = ./build
SRC = main.c http.c net.c utils.c router.c
OBJ = $(SRC:%.c=%(DEST)/%.o)

.PHONY: build clean

build: $(DEST)/main.o $(DEST)/net.o $(DEST)/http.o $(DEST)/utils.o $(DEST)/router.o
	$(CC) $(CFLAGS) -o serv $^

$(DEST)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm serv
	@rm -rf $(DEST)/*
