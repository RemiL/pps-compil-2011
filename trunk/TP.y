/* les tokens ici sont ceux qui doivent etre renvoyes par l'aanalyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans le TP1.
 *
 * Attention: BISON va ecraser le contenu de TP.h a partir de la description
 * de la ligne suivante. Donc c'est cette ligne qu'il faut mettre a jour, si elle
 * ne correspond pas a ce que vous avez fait, pas TP.h !
 */
%token ID ID_CLASS CSTE AFF RELOP IF THEN ELSE STRING CLASS STATIC DEF RETURNS IS VARIABLE VAL OVERRIDE EXTENDS NOUVEAU

 /* indications de precedence et d'associativite. Les precedences sont
  * donnees en ordre croissant. Les operateurs sur une meme ligne (separes par
  * un espace) auront la meme priorite.
  */
%nonassoc ELSE
%nonassoc RELOP
%left '+' '-'
%left '*' '/'
%left '.'

/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associées a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE.
 * La "valeur" associée a un terminal doit toujours utiliser la variante de
 * meme type
//TODO
%type <S> NOM
%type <V> LOptDec LDec
%type <A> EXPR BEXPR
%type <C> REL
 */
%{
#include <stdio.h>    /* needed for compiling the file generated by Bison */
#include "main.h"       /* definition des types et des etiquettes utilises */

extern int yylex();
extern void yyerror();

  /* les fonctions ci-dessous sont definies dans main.c */

  /* Constructeurs d'arbres de syntaxe abstraite */
extern PVAR MakeVar();
extern PARBRE MakeID(), MakeCste(), MakeNoeud(), MakeITE(), MakeUnary();

  /* fonction d'evaluation de l'interprete */
extern int evalue();

  /* fonction d'impression de la liste des variables declarees */
extern void imprime();
%}

%% 

 /* Principe de construction de l'interprete:
  *
  * On reconnnait une liste (eventuellement vide) de declarations de variables
  * avec leur initialisation, puis le programme principal.
  * Au fur et a mesure qu'on reconnait des declarations de variables, on
  * construit une liste chainee avec les noms de variables et leur valeur.
  * Pendant l'analyse d'une declaration x := expr  on construit un arbre de
  * syntaxe abstraite qui represente l'expression. Pendant la construction
  * de l'arbre, on verifie a chaque occurrence d'un identificateur que cet
  * identificateur apparait bien dans la liste des variables deja declarees.
  * Une fois que l'arbre representant 'expr' est construit, on le passe en
  * parametre, avec la liste des variables deja declarees, a une fonction
  * d'evaluation qui va appliquer les opérateurs, en cherchant les valeurs
  * des variables dans la liste. Quand la valeur de l'expression est retournee
  * on ajoute un nouvel element a la liste des variables...
  *
  * Les macros d'allocation NEW et de nullite (nil) sont definies dans main.h.
  * Leur usage n'est bien sur pas obligatoire.
  * Voir la definition des types VAR, PVAR, ARBRE, PARBRE dans main.h
  *
  */

S : LDefClass Bloc
   { 
     /* on evalue EXPR (i.e. $3) dans le contexte forme par l'ensemble des
      * declaration qui constituent LOptDec (i.e. $1).
      * Ici, a titre d'exemple, on imprime d'abord la liste de toutes les
      * variables, puis la valeur de l'expression finale.
      
     int res = evalue($3, $1);
     imprime($1);
     fprintf(stdout, "Expression finale: %d\n", res); 
     */
   }
;

LDefClass : LDefClass DefClass        // Ldef : liste non vide de declaration de classe
          | DefClass
  {
  }
;

DefClass : CLASS ID_CLASS '(' LParamOpt ')' ExtendsOpt InitBlocOpt IS Corps    // une declaration de classe
  {
  }
;

LParamOpt : LParam
          | 
  {
  }
;

LParam : Param ',' LParam
       | Param
       | LParamInit
  {
  }
;

LParamInit : LParamInit ',' ParamInit
           | ParamInit
  {
  }
;

Param : ID ':' ID
  {
  }
;

ParamInit : ID ':' ID AFF E
  {
  }
;

ExtendsOpt : EXTENDS AppelConstr
           | 
  {
  }
;

AppelConstr : ID_CLASS '(' LArgOpt ')'
  {
  }
;

InitBlocOpt : '{' LAffectOpt '}'
  {
  }
;

LAffectOpt : LAffect    // liste d"affectation optionnelle
           | 
  {
  }
;

LAffect : LAffect ';' Affect
        | Affect
  {
  }
;

Affect : Selection AFF Expr
  {
  }
;

Selection : Expr '.' ID
  {
  }
;

Corps : '{' LDeclAttrOpt LDeclMethOpt '}'        // corps d'une classe
  {
  }
;

LDeclAttrOpt : LDeclAttr        // liste de déclaration d'attributs optionnelle
             |
  {
  }
;

LDeclMethOpt : LDeclMeth        // liste de déclaration de méthodes optionnelle
             |
  {
  }
;

LDeclAttr : LDeclAttr ';' DeclAttr    // liste de déclaration d'un attribut
          | DeclAttr
  {
  }
;

DeclAttr : STATIC DeclA    // déclaration d'un attribut statique ou pas statique
         | DeclA
  {
  }
;

DeclA : VARIABLE ID ':' ID InitOpt   // déclaration d'un attribut 
      | VAL ID ':' ID InitOpt
  {
  }
;

InitOpt : AFF E
        | 
  {
  }
;

LDeclMeth : LDeclMeth DeclMeth
          | DeclMeth
  {
  }
;

DeclMeth : Def ID '(' LParamOpt ')' RETURNS ID IS Bloc
  {
  }
;

Def : DEF STATIC
    | DEF
    | DEF OVERRIDE
  {
  }
;

Bloc : '{' Lexpr '}'
     | '{' LDeclA IS Lexpr '}'
  {
  }
;

LDeclA : LDeclA ';' DeclA        // liste de déclaration d'attribut non statique
       | DeclA
  {
  }
;

Lexpr : Lexpr ';' E            // liste d'expression d'un bloc
      | E
  {
  }
;

E : Expr
  | ExprComp
  {
  }
;

Expr : IfThenElse
     | ID
     | Selection
     | CSTE
     | '(' E ')'
     | NOUVEAU ID '(' LArgOpt ')'
     | EnvoiMsg
     | ExprArithm
  {
  }
;

IfThenElse : IF E THEN Bloc ELSE Bloc
           | IF E THEN E ELSE E
  {
  }
;

LArgOpt : LArg    // liste d'arguments optionnelle
        | 
  {
  }
;

LArg : LArg ',' E    // liste d'arguments
     | E
  {
  }
;

ExprArithm : E '+' E
           | E '-' E
           | E '*' E
           | E '/' E
           | '+' E
           | '-' E
  {
  }
;

ExprComp : Expr RELOP Expr
  {
  }
;

EnvoiMsg : EnvoiMsgStatic     // envoi d'un message simple ou appel à une fonction statique
         | EnvoiMsgNonStatic
  {
  }
;

EnvoiMsgStatic : ID_CLASS '.' EnvoiMsgNonStatic
               | ID_CLASS '.' ID '(' LArgOpt ')'
  {
  }
;

EnvoiMsgNonStatic : ID '.' EnvoiMsgNonStatic
                  | ID '.' ID '(' LArgOpt ')'
  {
  }
;