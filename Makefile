OBJ=structures.o arbres.o verif_contextuelles.o comp.o lex.o main.o
CC=gcc
CFLAGS=-Wall -I./ -g 
LDFLAGS= -g -lfl

comp : comp.l $(OBJ)
	$(CC) -o comp $(CFLAGS) $(OBJ) -lfl

comp.c : comp.y
	bison -v -b comp -o comp.c -d comp.y

comp.o: comp.c comp.h main.h
	$(CC) $(CFLAGS) -c comp.c

main.o: utils.h main.h main.c comp.h
	$(CC) $(CFLAGS) -c main.c

lex.o: main.h lex.c comp.h
	$(CC) $(CFLAGS) -c lex.c

structures.o: structures.c structures.h utils.h
	$(CC) $(CFLAGS) -c structures.c
	
arbres.o: arbres.c arbres.h utils.h
	$(CC) $(CFLAGS) -c arbres.c
	
verif_contextuelles.o: verif_contextuelles.c verif_contextuelles.h utils.h
	$(CC) $(CFLAGS) -c verif_contextuelles.c

lex.c : comp.l
	flex --yylineno -o lex.c comp.l

clean:	
	rm -f *~ *.o lex.c comp.c comp.h comp comp.output
