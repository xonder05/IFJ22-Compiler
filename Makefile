CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

all: prekladac

prekladac: main.o scanner.o dyn_string.o
	$(CC) $(CFLAGS) main.o scanner.o dyn_string.o -o prekladac


main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

dyn_string.o: dyn_string.c
	$(CC) $(CFLAGS) -c dyn_string.c -o dyn_string.o

run: prekladac
	./prekladac


clean:
	-rm -f *.o main

zip:
	zip xbahou00.zip *.c *.h Makefile