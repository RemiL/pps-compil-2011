Chiffre    [0-9]
Entier     {Chiffre}+
LettreMin  [a-z]
LettreMaj  [A-Z]
Lettre	   ({LettreMin}|{LettreMaj})
LC 	   ({Lettre}|{Chiffre})
Ident      {LettreMin}{LC}*
IdentClass {LettreMaj}{LC}*
Delim	   [ \n\t\r]
Symbole    [+\-*/();,:{}]
Guill	   "\""
Star  	   "*"
NotStar	   [^*]
NotStarPar [^*/]
InComment  ({NotStar}|({Star}+{NotStarPar}+))*{Star}*
Comment    "/*"{InComment}"*/"
BS         \\.
PasBSG     [^\\"\n]
IntString  ({BS}|{PasBSG})*


%{
#include "main.h"
#include "TP.h"

extern YYSTYPE yylval;
extern int yylineno;


/* Dans cette version, GET et PUT sont vus comme des mots-clef, puisqu'ils
 * interviennent dans la grammaire sous une forme tres specifique.
 * Si on avait a traiter des appels de fonctions generaux, il serait
 * probablement plus simple de considerer PUT comme un identificateur
 * predefini. Il en serait de meme pour GET si la syntaxe d'appel prenait
 * un couple vide de parentheses, comme les fonctions sans argument.
 */
int TesteMotClef(char *s)
{ 
  if (! strcmp(s, "if")) return(IF);
  if (! strcmp(s, "then")) return(THEN);
  if (! strcmp(s, "else")) return(ELSE);
  if (! strcmp(s, "class")) return(CLASS);
  if (! strcmp(s, "static")) return(STATIC);
  if (! strcmp(s, "def")) return(DEF);
  if (! strcmp(s, "returns")) return(RETURNS);
  if (! strcmp(s, "is")) return(IS);
  if (! strcmp(s, "val")) return(VAL);
  if (! strcmp(s, "var")) return(VARIABLE);
  if (! strcmp(s, "override")) return(OVERRIDE);
  if (! strcmp(s, "extends")) return(EXTENDS);
  if (! strcmp(s, "new")) return(NOUVEAU);
  yylval.S = strdup(s); return(ID);
}

%}

%%
{Entier} 	{ yylval.E = atoi(yytext); return(CSTE); }
{IdentClass}  	{ yylval.S = strdup(yytext); return(ID_CLASS); }
{Ident}  	{ return TesteMotClef(yytext); }
{Symbole}	{ return(yytext[0]); }
:=		{ return(AFF); }
=		{ yylval.C = EQ; return(RELOP); }
\<>		{ yylval.C = NEQ; return(RELOP); }
\<=		{ yylval.C = LE; return(RELOP); }
>=		{ yylval.C = GE; return(RELOP); }
>		{ yylval.C = GT; return(RELOP); }
\<		{ yylval.C = LT; return(RELOP); }
\"{IntString}\" { yylval.S = strdup(yytext); return STRING; }
{Delim}+	{ /* l'option --yylineno permet de gerer directement le
                   * decompte des sauts de lignes via la variable yylineno.
                   * Il n'y a donc rien de special a faire.
                   */}
{Comment}	{ }
.		printf("Caractere errone: %c\n", yytext[0]);
%%
