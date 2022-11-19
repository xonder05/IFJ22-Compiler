CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

all: prekladac

prekladac: main.o parser.o scanner.o dyn_string.o testing_utils.o stack.o expressions.o
	$(CC) $(CFLAGS) main.o parser.o scanner.o dyn_string.o testing_utils.o stack.o expressions.o -o prekladac


main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

dyn_string.o: dyn_string.c
	$(CC) $(CFLAGS) -c dyn_string.c -o dyn_string.o

testing_utils.o: testing_utils.c
	$(CC) $(CFLAGS) -c testing_utils.c -o testing_utils.o

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c -o parser.o

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c -o stack.o

expressions.o: expressions.c
	$(CC) $(CFLAGS) -c expressions.c -o expressions.o

run: prekladac
	./prekladac

clean:
	-rm -f *.o main

zip:
	zip xbahou00.zip *.c *.h Makefile