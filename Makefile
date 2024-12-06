CC ?= gcc
CFLAGS += -g -Wall -Wextra -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code


build: main.o net.o
	$(CC) $(CFLAGS) -o server $^

main.o: main.c net.h
	$(CC) $(CFLAGS) -c -o main.o main.c

net.o: net.c net.h
	$(CC) $(CFLAGS) -c -o net.o net.c


.PHONY: clean

clean:
	@rm server
	@rf -rf *.o
	@rm -rf build/*
