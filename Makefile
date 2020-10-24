CFLAGS = -pedantic -Wall -Wextra -std=gnu99 # -Werror
OBJECTS = $(SOURCES:.c=.o)
SOURCES = $(wildcard *.c)
CC = gcc

all: build

build: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm *.o