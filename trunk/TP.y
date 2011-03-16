/* les tokens ici sont ceux qui doivent etre renvoyes par l'aanalyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans le TP1.
 *
 * Attention: BISON va ecraser le contenu de TP.h a partir de la description
 * de la ligne suivante. Donc c'est cette ligne qu'il faut mettre a jour, si elle
 * ne correspond pas a ce que vous avez fait, pas TP.h !
 */
%token ID ID_CLASS CSTE AFF RELOP IF THEN ELSE STRING CLASS STATIC DEF RETURNS IS VARIABLE VAL OVERRIDE EXTENDS NOUVEAU PREC_MIN PREC_UNAIRE

 /* indications de precedence et d'associativite. Les precedences sont
  * donnees en ordre croissant. Les operateurs sur une meme ligne (separes par
  * un espace) auront la meme priorite.
  */
%nonassoc PREC_MIN
%nonassoc ELSE
%nonassoc AFF
%nonassoc RELOP
%left '+' '-'
%left '*' '/'
%right PREC_UNAIRE

/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associées a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE.
 * La "valeur" associée a un terminal doit toujours utiliser la variante de
 * meme type
//TODO
*/
%type <S> ExendsOpt AppelConstr 
%type <V> LOptDec LDec
%type <A> EXPR BEXPR
%type <C> REL
%type <LParam> LParam LParamOpt
%type <LClasse> LDefClass
%type <Classe> DefClass
%type <TypeMethode> Def
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

S : LDefClass Bloc { }
;

LDefClass : LDefClass DefClass        // Ldef : liste non vide de declaration de classe
	{$$ = ajouter_classe($0, $1);	}
          | DefClass
	{$$ = nouvelle_liste_classes($0);}
;

DefClass : CLASS ID_CLASS '(' LParamOpt ')' ExtendsOpt InitBlocOpt IS Corps    // une declaration de classe
{
	//classe_t* nouvelle_classe(char* nom, classe_t* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes);

	$$ = nouvelle_classe($1, $6, $3, $9.vars, $9.meths)
}
;

LParamOpt : LParam
	{$$ = $1;}
          | 
	{$$ = nouvelle_liste_params(null);}
;

LParam : Param ',' LParam
	{$$ = ajouter_param($1, $0);}
       | Param
	{ $$ = nouvelle_liste_params($0); }
       | LParamInit
	{ $$ = $1; }
;

LParamInit : LParamInit ',' ParamInit
	{ $$ = ajouter_params($0, $3); }
           | ParamInit
	{ $$ = nouveau_param(char* nom, classe_t* type /* TODO expression par défaut */); }
;

Param : ID ':' ID_CLASS
	{ $$ = nouveau_param($0, $2 /* TODO expression par défaut */); }
;

ParamInit : ID ':' ID_CLASS AFF ExprSansAffect
	{ $$ = nouveau_param($0, $2 /* TODO expression par défaut */); }
;

ExtendsOpt : EXTENDS AppelConstr
	{ $$ = $1;}
           | 
	{ $$ = null}
;

AppelConstr : ID_CLASS '(' LArgOpt ')'
;

InitBlocOpt : Bloc
            | 
;

Affect : ID AFF ExprSansAffect { /* TODO */ }
       | Selection AFF ExprSansAffect { /* TODO */ }
;

Corps : '{' LDeclAttrOpt LDeclMethOpt '}'        // corps d'une classe
	{ $$ = nouveau_corps($1, $2); }
;

LDeclAttrOpt : LDeclAttr        // liste de déclaration d'attributs optionnelle
   	{ $$ = $0; }
             |   	{ $$ = nouvelle_liste_params(NULL); }
;

LDeclMethOpt : LDeclMeth        // liste de déclaration de méthodes optionnelle
   	{ $$ = $1; }
             |   	{ $$ = nouvelle_liste_methodes(NULL); }
;

LDeclAttr : LDeclAttr ';' DeclAttr    // liste de déclaration d'un attribut
   	{ $$ = ajouter_param($0, &$1; }
          | DeclAttr   	{ $$ = nouvelle_liste_variables($0); }
;

DeclAttr : STATIC DeclA    // déclaration d'un attribut statique ou pas statique
   	{ $1.statique = 1; $$ = $1 }
         | DeclA   	{ $$ = $1;}
;

DeclA : VARIABLE ID ':' ID_CLASS InitOpt   // déclaration d'un attribut 
   	{ $$ = nouvelle_variable($1, $3, 0, 0); }
      | VAL ID ':' ID_CLASS InitOpt   	{ $$ = nouvelle_variable($1, $3, 1, 0); }
;

InitOpt : AFF ExprSansAffect   	{ $$ = $1; }
        |    	{ /*TODO $$ = ; */}
;

LDeclMeth : LDeclMeth DeclMeth   	{ $$ = ajouter_methode($0, $1); }
          | DeclMeth   	{ $$ = nouvelle_liste_methodes($0); }
;

DeclMeth : Def ID '(' LParamOpt ')' RETURNS ID_CLASS IS Bloc
{
	$$ = nouvelle_methode($1.lexval, $0, $3, classe_t* type_retour); //FIXME : char* type_retour
}
;

Def : DEF STATIC   	{ $$ = type_methode_t.STATIQUE; }
    | DEF   	{ $$ = type_methode_t.NORMALE; }
    | DEF OVERRIDE   	{ $$ = type_methode_t.OVERRIDE; }
;

Bloc : '{' LExpr '}'   	{ /*TODO $$ = ; */}
     | '{' LDeclA IS LExpr '}'   	{ /*TODO $$ = ; */ }
;

LDeclA : LDeclA ';' DeclA        // liste de déclaration d'attribut non statique
   	{ $$ = ajouter_variable($0, $2); }
       | DeclA   	{ $$ = nouvelle_liste_variables($0); }
;

LExpr : LExpr ';' Expr            // liste d'expression d'un bloc
   	{ /*TODO $$ = ; */ }
      | Expr   	{ /*TODO $$ = ; */ }
;

BlocExpr : Expr   	{ $$ = $0 } //XXX
         | Bloc   	{ $$ = $0 }
;

Expr : ExprSansAffect %prec PREC_MIN   	{ $$ = $0; } //XXX
     | Affect   	{ $$ = $0; }
;

ExprSansAffect : IfThenElse   	{ /*TODO $$ = ; */ }
               | ID   	{ /*TODO $$ = ; */}
               | Selection   	{ /*TODO $$ = ; */ }
               | CSTE   	{ /*TODO $$ = ; */ }
               | STRING   	{ /*TODO $$ = ; */ }
               | NOUVEAU ID_CLASS '(' LArgOpt ')'   	{ /*TODO $$ = ; */ }
               | EnvoiMsg   	{ /*TODO $$ = ; */ }
               | ExprSansAffect '+' ExprSansAffect   	{ /*TODO $$ = ; */ }
               | ExprSansAffect '-' ExprSansAffect   	{ /*TODO $$ = ; */ }
               | ExprSansAffect '*' ExprSansAffect   	{ /*TODO $$ = ; */ }
               | ExprSansAffect '/' ExprSansAffect   	{ /*TODO $$ = ; */ }
               | '+' ExprSansAffect %prec PREC_UNAIRE   	{ /*TODO $$ = ; */ }
               | '-' ExprSansAffect %prec PREC_UNAIRE   	{ /*TODO $$ = ; */ }
               | ExprSansAffect RELOP ExprSansAffect   	{ /*TODO $$ = ; */ }
               | '(' Expr ')'   	{ /*TODO $$ = ; */ }
;

ExprSelec : '(' Expr ')'   	{ $$ = $1; }
          | CSTE   	{ /*TODO $$ = ; */ }
          | STRING   	{ /*TODO $$ = ; */ }
          | ID   	{ /*TODO $$ = ; */ }
          | Selection   	{ /*TODO $$ = ; */ }
          | EnvoiMsg   	{ /*TODO $$ = ; */ }
;

Selection : ExprSelec '.' ID   	{ /*TODO $$ = ; */ }
          | ID_CLASS '.' ID   	{ /*TODO $$ = ; */ }S : LDefClass Bloc { }
;

LDefClass : LDefClass DefClass        // Ldef : liste non vide de declaration de classe
	{$$ = ajouter_classe($0, $1);	}
          | DefClass
	{$$ = nouvelle_liste_classes($0);}
;

DefClass : CLASS ID_CLASS '(' LParamOpt ')' ExtendsOpt InitBlocOpt IS Corps    // une declaration de classe
{
	//classe_t* nouvelle_classe(char* nom, classe_t* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes);

	$$ = nouvelle_classe($1, $6, $3, $9.vars, $9.meths)
}
;

LParamOpt : LParam
	{$$ = $1;}
          | 
	{$$ = nouvelle_liste_params(null);}
;

LParam : Param ',' LParam
	{$$ = ajouter_param($1, $0);}
       | Param
	{ $$ = nouvelle_liste_params($0); }
       | LParamInit
	{ $$ = $1; }
;

LParamInit : LParamInit ',' ParamInit
	{ $$ = ajouter_params($0, $3); }
           | ParamInit
	{ $$ = nouveau_param(char* nom, classe_t* type /* TODO expression par défaut */); }
;

Param : ID ':' ID_CLASS
	{ $$ = nouveau_param($0, $2 /* TODO expression par défaut */); }
;

ParamInit : ID ':' ID_CLASS AFF ExprSansAffect
	{ $$ = nouveau_param($0, $2 /* TODO expression par défaut */); }
;

ExtendsOpt : EXTENDS AppelConstr
	{ $$ = $1;}
           | 
	{ $$ = null}
;
;

EnvoiMsg : ExprSelec '.' ID '(' LArgOpt ')'     // envoi d'un message simple ou appel à une fonction statique
   	{ /*TODO $$ = ; */ }
         | ID_CLASS '.' ID '(' LArgOpt ')'   	{ /*TODO $$ = ; */ }
;

IfThenElse : IF Expr THEN BlocExpr ELSE BlocExpr   	{ /*TODO $$ = ; */ }
;

// liste d'arguments optionnelle
LArgOpt : LArg   	{ $$ = $1; }
        |    	{ $$ = nouvelle_liste_variables(NULL); }
;
 // liste d'arguments
LArg : LArg ',' Expr   	{ $$ = ajouter_variable($0, $2); }
     | Expr	{ $$ = nouvelle_liste_variables($0); }
;
