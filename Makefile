CC = gcc
#CFLAGS=-std=c11 -Wall -Wextra -pedantic -fcommon -g -fsanitize=address -O0 
#LDFLAGS+= -lm -fsanitize=address

# PRED ODEVYDANIM ZMENIT NA:
CFLAGS=-std=c11 -Wall -Wextra -pedantic -fcommon -O2
LDFLAGS+= -lm

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY:all run clean zip


all: prekladac

prekladac: $(OBJ)
	$(CC) $(LDFLAGS)  $^ -o prekladac

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

run: prekladac
	./prekladac


clean:
	-rm -f *.o prekladac main

zip:
	zip xbahou00.zip *.c *.h Makefile