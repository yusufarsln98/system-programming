CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

all: clean clear main run

clear:
	clear

main: main.c
	$(CC) $(CFLAGS) -o main main.c utils.c helpers.c

run: main
	./main

clean:
	rm -f main log.txt grades.txt
