CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

all: prekladac

prekladac: main.o scanner.o dyn_string.o testing_utils.o code_gen_build.o code_gen.o symtable.o abstact_syntax_tree.o
	$(CC) $(CFLAGS) main.o scanner.o dyn_string.o testing_utils.o code_gen_build.o code_gen.o symtable.o abstact_syntax_tree.o -o prekladac


main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

dyn_string.o: dyn_string.c
	$(CC) $(CFLAGS) -c dyn_string.c -o dyn_string.o

testing_utils.o: testing_utils.c
	$(CC) $(CFLAGS) -c testing_utils.c -o testing_utils.o

code_gen_build.o: code_gen_build.c
	$(CC) $(CFLAGS) -c code_gen_build.c -o code_gen_build.o

code_gen.o: code_gen.c
	$(CC) $(CFLAGS) -c code_gen.c -o code_gen.o

abstact_syntax_tree.o: abstact_syntax_tree.c
	$(CC) $(CFLAGS) -c abstact_syntax_tree.c -o abstact_syntax_tree.o

symtable.o: symtable.c
	$(CC) $(CFLAGS) -c symtable.c -o symtable.o

run: prekladac
	./prekladac


clean:
	-rm -f *.o main

zip:
	zip xbahou00.zip *.c *.h Makefile