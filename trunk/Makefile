OBJ=comp.o lex.o main.o structures.o arbres.o verif_contextuelles.o
CC=gcc
CFLAGS=-Wall -I./ -g 
LDFLAGS= -g -lfl

comp : comp.l $(OBJ)
	$(CC) -o comp $(CFLAGS) $(OBJ) -lfl

comp.c : comp.y 
	bison -v -b comp -o comp.c -d comp.y

comp.o: comp.h main.h comp.c
	$(CC) $(CFLAGS) -c comp.c

main.o: comp.h utils.h main.h main.c
	$(CC) $(CFLAGS) -c main.c

lex.o: comp.h main.h lex.c
	$(CC) $(CFLAGS) -c lex.c

structures.o: utils.h structures.h structures.c
	$(CC) $(CFLAGS) -c structures.c
	
arbres.o: utils.h arbres.h arbres.c
	$(CC) $(CFLAGS) -c arbres.c
	
verif_contextuelles.o: utils.h verif_contextuelles.h verif_contextuelles.c
	$(CC) $(CFLAGS) -c verif_contextuelles.c

lex.c : comp.l
	flex --yylineno -o lex.c comp.l

clean:	
	rm -f *~ *.o lex.c comp.c comp.h comp comp.output
