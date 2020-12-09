all:
	gcc -std=c11 src/leParser.c -o leParser.o

clean:
	rm *.o