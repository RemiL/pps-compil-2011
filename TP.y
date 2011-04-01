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
 */
//TODO
%type <S> ID ID_CLASS ExtendsOpt AppelConstr
//%type <A> EXPR BEXPR
//%type <C> REL
%type <LClasses> LDefClass
%type <Classe> DefClass
%type <Corps> Corps
%type <LMethodes> LDeclMethOpt LDeclMeth
%type <Methode> DeclMeth
%type <TypeMethode> Def
%type <Bloc> Bloc
%type <LParams> LParam LParamOpt LParamInit
%type <Param> Param ParamInit
%type <LVars> LDeclAttrOpt LDeclAttr LDeclA
%type <Var> DeclAttr DeclA
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

S : LDefClass Bloc { liberer_liste_classes($1); liberer_liste_variables($2.variables); }
;

LDefClass : LDefClass DefClass        // Ldef : liste non vide de declaration de classe
  {
   	est_valide_classe($1, $2); 
    $$ = ajouter_classe($1, $2);	
  }
          | DefClass
  {
    liste_classes_t l = nouvelle_liste_classes_preinitialisee();
    est_valide_classe(l, $1);
    $$ = ajouter_classe(l, $1);
  }
;

DefClass : CLASS ID_CLASS '(' LParamOpt ')' ExtendsOpt InitBlocOpt IS Corps    // une declaration de classe
{
	//classe_t* nouvelle_classe(char* nom, classe_t* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes);

	$$ = nouvelle_classe($2, $6, $4, $9.variables, $9.methodes);
}
;

LParamOpt : LParam
	{$$ = $1;}
          | 
	{$$ = nouvelle_liste_params(NIL(param_t));}
;

LParam : Param ',' LParam
	{
    est_valide_nom_param($3, $1);
    $$ = ajouter_param($3, $1);
  }
       | Param
	{ $$ = nouvelle_liste_params($1); }
       | LParamInit
	{ $$ = $1; }
;

LParamInit : LParamInit ',' ParamInit
	{ 
    est_valide_nom_param($3, $1);
    $$ = ajouter_param($1, $3);
 }
           | ParamInit
	{ $$ = nouvelle_liste_params($1); }
;

Param : ID ':' ID_CLASS
	{ $$ = nouveau_param($1, $3 /* TODO expression par défaut */); }
;

ParamInit : ID ':' ID_CLASS AFF ExprSansAffect
	{ $$ = nouveau_param($1, $3 /* TODO expression par défaut */); }
;

ExtendsOpt : EXTENDS AppelConstr
	{ $$ = $2;}
           | 
	{ $$ = NULL;}
;

AppelConstr : ID_CLASS '(' LArgOpt ')'
  { $$ = $1; } // TODO arguments
;

InitBlocOpt : Bloc
            | 
;

Affect : ID AFF ExprSansAffect { /* TODO */ }
       | Selection AFF ExprSansAffect { /* TODO */ }
;

Corps : '{' LDeclAttrOpt LDeclMethOpt '}'        // corps d'une classe
	{ $$ = nouveau_corps($2, $3); }
;

LDeclAttrOpt : LDeclAttr        // liste de déclaration d'attributs optionnelle
   	{ $$ = $1; }
             |   	{ $$ = nouvelle_liste_variables(NIL(var_t)); }
;


LDeclMethOpt : LDeclMeth        // liste de déclaration de méthodes optionnelle
   	{ $$ = $1; }
     |   	{ $$ = nouvelle_liste_methodes(NIL(methode_t)); }
;

LDeclAttr : LDeclAttr ';' DeclAttr    // liste de déclaration d'un attribut
   	{ $$ = ajouter_variable($1, $3); }
          | DeclAttr   	{ $$ = nouvelle_liste_variables($1); }
;

DeclAttr : STATIC DeclA    // déclaration d'un attribut statique ou pas statique
   	{ $2->statique = 1; $$ = $2; /* TODO améliorer ça ? */ }
         | DeclA   	{ $$ = $1;}
;

DeclA : VARIABLE ID ':' ID_CLASS InitOpt   // déclaration d'un attribut 
   	{ $$ = nouvelle_variable($2, $4, 0, 0); }
      | VAL ID ':' ID_CLASS InitOpt   	{ $$ = nouvelle_variable($2, $4, 1, 0); }
;

InitOpt : AFF ExprSansAffect   	{ /*TODO $$ = $1;*/ }
        |    	{ /*TODO $$ = ; */}
;

LDeclMeth : LDeclMeth DeclMeth
   	{
   	  
   	  est_valide_nom_methode($1, $2);
   	  $$ = ajouter_methode($1, $2);
   	}
          | DeclMeth   	{ $$ = nouvelle_liste_methodes($1); }
;

DeclMeth : Def ID '(' LParamOpt ')' RETURNS ID_CLASS IS Bloc
{
	$$ = nouvelle_methode($2, $1, $4, $9.variables, $7);
}
;

Def : DEF STATIC   	{ $$ = STATIQUE; }
    | DEF   	{ $$ = NORMALE; }
    | DEF OVERRIDE   	{ $$ = OVERRIDE; }
;

Bloc : '{' LExpr '}'   	{ $$ = nouveau_bloc(nouvelle_liste_variables(NIL(var_t))); }
     | '{' LDeclA IS LExpr '}'   	{ $$ = nouveau_bloc($2); }
;

LDeclA : LDeclA ';' DeclA        // liste de déclaration d'attribut non statique
   	{ $$ = ajouter_variable($1, $3); }
       | DeclA   	{ $$ = nouvelle_liste_variables($1); }
;

LExpr : LExpr ';' Expr            // liste d'expression d'un bloc
   	{ /*TODO $$ = ; */ }
      | Expr   	{ /*TODO $$ = ; */ }
;

BlocExpr : Expr   	{ /* TODO $$ = $1; */ }
         | Bloc   	{ /* TODO $$ = $1; */ }
;

Expr : ExprSansAffect %prec PREC_MIN   	{ /* TODO $$ = $1; */ }
     | Affect   	{ /* TODO $$ = $1;*/ }
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

ExprSelec : '(' Expr ')'   	{ /*TODO $$ = $1; */ }
          | CSTE   	{ /*TODO $$ = ; */ }
          | STRING   	{ /*TODO $$ = ; */ }
          | ID   	{ /*TODO $$ = ; */ }
          | Selection   	{ /*TODO $$ = ; */ }
          | EnvoiMsg   	{ /*TODO $$ = ; */ }
;

Selection : ExprSelec '.' ID   	{ /*TODO $$ = ; */ }
          | ID_CLASS '.' ID   	{ /*TODO $$ = ; */ }
;

EnvoiMsg : ExprSelec '.' ID '(' LArgOpt ')'     // envoi d'un message simple ou appel à une fonction statique
   	{ /*TODO $$ = ; */ }
         | ID_CLASS '.' ID '(' LArgOpt ')'   	{ /*TODO $$ = ; */ }
;

IfThenElse : IF Expr THEN BlocExpr ELSE BlocExpr   	{ /*TODO $$ = ; */ }
;

// liste d'arguments optionnelle
LArgOpt : LArg   	{ /*TODO $$ = $1; */ }
        |    	{ /*TODO$$ = nouvelle_liste_variables(NULL); */ }
;
 // liste d'arguments
LArg : LArg ',' Expr   	{ /*TODO $$ = ajouter_variable($1, $3); */ }
     | Expr	{ /*TODO $$ = nouvelle_liste_variables($1); */ }
;
