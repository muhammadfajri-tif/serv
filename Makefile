CC ?= gcc
CFLAGS += -g -Wall -Wextra -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code

DEST = ./build
SRC = main.c net.c
OBJ = $(SRC:%.c=%(DEST)/%.o)

.PHONY: build clean

build: $(DEST)/main.o $(DEST)/net.o
	$(CC) $(CFLAGS) -o server $^

$(DEST)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm server
	@rm -rf $(DEST)/*
