CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

main : main.c declarations.h lexikalniAnalyzator.c semantickyAnalyzator.c syntaktickyAnalyzator.c generatorKodu.c
	$(CC) $(CFLAGS) main.c lexikalniAnalyzator.c semantickyAnalyzator.c syntaktickyAnalyzator.c generatorKodu.c -o prekladac
