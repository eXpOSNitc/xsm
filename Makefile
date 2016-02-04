#Our compiler.
CC = cc
CFLAGS = -g
LEX = lex

default: xsm

xsm: lex.yy.o machine.o main.o simulator.o word.o memory.o registers.o tokenize.o disk.o
	$(CC) $(CFLAGS) -o xsm lex.yy.o machine.o main.o simulator.o word.o memory.o registers.o tokenize.o disk.o

lex.yy.c: parse.l
	$(LEX) parse.l

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

machine.o: machine.c machine.h
	$(CC) $(CFLAGS) -c machine.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

simulator.o: simulator.c simulator.h
	$(CC) $(CFLAGS) -c simulator.c

word.o: word.c word.h
	$(CC) $(CFLAGS) -c word.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

registers.o: registers.c registers.h
	$(CC) $(CFLAGS) -c registers.c

tokenize.o: tokenize.c tokenize.h
	$(CC) $(CFLAGS) -c tokenize.c

disk.o: disk.c disk.h
	$(CC) $(CFLAGS) -c disk.c
