CFLAGS = -pedantic -Wall -Wextra -std=c11 # -Werror
CC = gcc

all:
	$(CC) leScanner.c -o leScanner.o

clean:
	rm *.o