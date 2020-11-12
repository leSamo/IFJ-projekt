CFLAGS = -pedantic -Wall -Wextra -std=c11 # -Werror
CC = gcc

all:
	$(CC) leParser.c -o leParser.o

clean:
	rm *.o