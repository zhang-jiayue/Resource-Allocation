# makefile for banker's alg program        
CC=gcc
DEPS = banker.h list.h input.h task.h
OBJ = banker.o list.o
CFLAGS=-Wall -g
STD_FLAG=-std=c99


banker.o: banker.c banker.h
	$(CC) $(CFLAGS) banker.c $(STD_FLAG)  -o banker

list.o: list.c list.h task.h input.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -rf *.o    

