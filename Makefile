CFLAGS = -pedantic -Wall -Wextra -std=c11 # -Werror
CC = gcc

all:
	$(CC) parserMock.c -o parserMock.o

clean:
	rm *.o