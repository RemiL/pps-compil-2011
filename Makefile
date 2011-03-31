OBJ=TP.o lex.o main.o structures.o verif_contextuelles.o
CC=gcc
CFLAGS=-Wall -I./ -g 
LDFLAGS= -g -lfl
tp2 : TP.l $(OBJ)
	$(CC) -o tp2 $(CFLAGS) $(OBJ) -lfl

TP.c : TP.y 
	bison -v -b TP -o TP.c -d TP.y

TP.o:  TP.c TP.h main.h
	$(CC) $(CFLAGS) -c TP.c

main.o: main.h main.c TP.h
	$(CC) $(CFLAGS) -c main.c

lex.o: TP.h main.h lex.c
	$(CC) $(CFLAGS) -c lex.c

structures.o: structures.h structures.c
	$(CC) $(CFLAGS) -c structures.c
	
verif_contextuelles.o: verif_contextuelles.h verif_contextuelles.c
	$(CC) $(CFLAGS) -c verif_contextuelles.c

lex.c : TP.l
	flex --yylineno -olex.c TP.l

teste-lex : teste-lex.o lex.o
	$(CC) -o teste-lex $(CFLAGS) lex.o teste-lex.o -lfl

teste-lex.o : TP.h main.h teste-lex.c
	$(CC) $(CFLAGS) -DYYDEBUG -c teste-lex.c

clean:	
	rm -f *.o lex.c TP.c TP.h tp2 teste-lex TP.output
