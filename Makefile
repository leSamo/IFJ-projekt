CFLAGS = -std=c11 -pedantic -Werror -Wall -Wextra
CC = gcc

all:
	$(CC) src/leParser.c -o leParser.o

clean:
	rm *.o