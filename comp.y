/* les tokens ici sont ceux qui doivent etre renvoyes par l'aanalyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans le TP1.
 *
 * Attention: BISON va ecraser le contenu de TP.h a partir de la description
 * de la ligne suivante. Donc c'est cette ligne qu'il faut mettre a jour, si elle
 * ne correspond pas a ce que vous avez fait, pas TP.h !
 */
%token ID ID_CLASS CSTE AFF RELOP IF THEN ELSE STRING CLASS STATIC DEF RETURNS IS VAR VAL OVERRIDE EXTENDS NOUVEAU PREC_MIN PREC_UNAIRE

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
%type <S> ID ID_CLASS STRING
%type <LClasses> LDefClass
%type <Classe> DefClass
%type <Heritage> ExtendsOpt
%type <Corps> Corps
%type <LMethodes> LDeclMethOpt LDeclMeth
%type <Methode> DeclMeth
%type <TypeMethode> Def
%type <LParams> LParam LParamOpt LParamInit
%type <Param> Param ParamInit
%type <LVars> LDeclAttrOpt LDeclAttr LDeclA
%type <Var> DeclAttr DeclA
%type <LArgs> LArgOpt LArg
%type <A> LBlocExpr BlocExpr Bloc InitBlocOpt Expr ExprSansAffect IfThenElse ExprSelec Selection EnvoiMsg Affect InitOpt
%type <E> CSTE
%type <C> RELOP REL
%{
#include <stdio.h>    /* needed for compiling the file generated by Bison */
#include "main.h"     /* definition des types et des etiquettes utilises */

extern int yylex();
extern void yyerror();
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

S : LDefClass Bloc { est_valide_arbre_syntaxique($1, NIL(decl_vars_t), $2, NIL(classe_t), FAUX); liberer_liste_classes($1); liberer_arbre($2); }
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
  $$ = nouvelle_classe($2, $6.nom_classe_mere, $6.args_classe_mere, $4, $7, $9.variables, $9.methodes);
}
;

LParamOpt : LParam
  {$$ = $1;}
          | 
  {$$ = nouvelle_liste_params(NIL(param_t));}
;

LParam : Param ',' LParam
  {
    est_valide_param($3, $1);
    /* règle récursive droite, on doit ajouter en
     * tête pour avoir l'ordre attendu */
    $$ = ajouter_param_en_tete($3, $1);
  }
       | Param
  { $$ = nouvelle_liste_params($1); }
       | LParamInit
  { $$ = $1; }
;

LParamInit : LParamInit ',' ParamInit
  { 
    est_valide_param($1, $3);
    $$ = ajouter_param($1, $3);
  }
           | ParamInit
  { $$ = nouvelle_liste_params($1); }
;

Param : ID ':' ID_CLASS
  { $$ = nouveau_param($1, $3, NIL(arbre_t)); }
;

ParamInit : ID ':' ID_CLASS AFF ExprSansAffect
  { $$ = nouveau_param($1, $3, $5); }
;

ExtendsOpt : EXTENDS ID_CLASS '(' LArgOpt ')'   { $$ = nouvel_heritage($2, $4); }
           |    { $$ = nouvel_heritage(NULL, nouvelle_liste_arguments(NIL(arg_t))); }
;

InitBlocOpt : Bloc  { $$ = $1; }
            |       { $$ = NIL(arbre_t); }
;

Affect : ID AFF ExprSansAffect { $$ = creer_noeud(Aff, creer_feuille_id($1), $3); }
       | Selection AFF ExprSansAffect { $$ = creer_noeud(Aff, $1, $3); }
;

Corps : '{' LDeclAttrOpt LDeclMethOpt '}'        // corps d'une classe
  { $$ = nouveau_corps($2, $3); }
;

LDeclAttrOpt : LDeclAttr        // liste de déclaration d'attributs optionnelle
     { $$ = $1; }
             |     { $$ = nouvelle_liste_variables(NIL(var_t)); }
;


LDeclMethOpt : LDeclMeth        // liste de déclaration de méthodes optionnelle
     { $$ = $1; }
     |     { $$ = nouvelle_liste_methodes(NIL(methode_t)); }
;

LDeclAttr : LDeclAttr ';' DeclAttr    // liste de déclaration d'un attribut
     { $$ = ajouter_variable($1, $3); }
          | DeclAttr     { $$ = nouvelle_liste_variables($1); }
;

DeclAttr : STATIC DeclA    // déclaration d'un attribut statique ou pas statique
     { $2->statique = 1; $$ = $2; /* TODO améliorer ça ? */ }
         | DeclA     { $$ = $1;}
;

DeclA : VAR ID ':' ID_CLASS InitOpt   // déclaration d'un attribut 
     { $$ = nouvelle_variable($2, $4, 0, 0, $5); }
      | VAL ID ':' ID_CLASS InitOpt     { $$ = nouvelle_variable($2, $4, 1, 0, $5); }
;

InitOpt : AFF ExprSansAffect     { $$ = $2; }
        |      { $$ = NIL(arbre_t); }
;

LDeclMeth : LDeclMeth DeclMeth
     {
       
       est_valide_methode($1, $2);
       $$ = ajouter_methode($1, $2);
     }
          | DeclMeth     { $$ = nouvelle_liste_methodes($1); }
;

DeclMeth : Def ID '(' LParamOpt ')' RETURNS ID_CLASS IS Bloc
{
  $$ = nouvelle_methode($2, $1, $4, $9, $7);
}
;

Def : DEF STATIC     { $$ = STATIQUE; }
    | DEF     { $$ = NORMALE; }
    | DEF OVERRIDE     { $$ = REDEFINIE; }
;

Bloc : '{' LBlocExpr '}'     { $$ = creer_noeud_bloc(nouvelle_liste_variables(NIL(var_t)), $2); }
     | '{' LDeclA IS LBlocExpr '}'     { $$ = creer_noeud_bloc($2, $4); }
;

LDeclA : LDeclA ';' DeclA        // liste de déclaration d'attribut non statique
     { $$ = ajouter_variable($1, $3); }
       | DeclA     { $$ = nouvelle_liste_variables($1); }
;

LBlocExpr : LBlocExpr ';' BlocExpr            // liste d'expression d'un bloc
     { $$ = creer_noeud(';', $1, $3); } /* L'étiquette ';' correspond à la liste d'expressions ou de blocs */
          | BlocExpr     { $$ = $1; }
;

BlocExpr : Expr     { $$ = $1; }
         | Bloc     { $$ = $1; }
;

Expr : ExprSansAffect %prec PREC_MIN     { $$ = $1; }
     | Affect     { $$ = $1; }
;

ExprSansAffect : IfThenElse     { $$ = $1; }
               | ID     { $$ = creer_feuille_id($1); }
               | Selection     { $$ = $1; }
               | CSTE     { $$ = creer_feuille_cste($1); }
               | STRING     { $$ = creer_feuille_chaine($1); }
               | NOUVEAU ID_CLASS '(' LArgOpt ')'     { $$ = creer_noeud_new($2, $4); }
               | EnvoiMsg     { $$ = $1; }
               | ExprSansAffect '+' ExprSansAffect     { $$ = creer_noeud('+', $1, $3); }
               | ExprSansAffect '-' ExprSansAffect     { $$ = creer_noeud('-', $1, $3); }
               | ExprSansAffect '*' ExprSansAffect     { $$ = creer_noeud('*', $1, $3); }
               | ExprSansAffect '/' ExprSansAffect     { $$ = creer_noeud('/', $1, $3); }
               | '+' ExprSansAffect %prec PREC_UNAIRE     { $$ = $2; }  /* L'opérateur unaire '+' n'apporte aucun changement. */
               | '-' ExprSansAffect %prec PREC_UNAIRE     { $$ = creer_noeud_oppose($2); }
               | ExprSansAffect REL ExprSansAffect %prec RELOP     { $$ = creer_noeud($2, $1, $3); }
               | '(' Expr ')'     { $$ = $2; }  /* Les parenthèses n'apportent aucune information nouvelle. */
;

/* Les expressions pouvant être placées devant l'opérateur de sélection */
ExprSelec : '(' Expr ')'     { $$ = $2; }
          | CSTE     { $$ = creer_feuille_cste($1); }
          | STRING     { $$ = creer_feuille_chaine($1); }
          | ID     { $$ = creer_feuille_id($1); }
          | Selection     { $$ = $1; }
          | EnvoiMsg     { $$ = $1; }
;

Selection : ExprSelec '.' ID     { $$ = creer_noeud_selection($1, $3, FAUX); }
          | ID_CLASS '.' ID     { $$ = creer_noeud_selection(creer_feuille_id($1), $3, VRAI); }
;

/* Envoi d'un message simple ou appel à une fonction statique */
EnvoiMsg : ExprSelec '.' ID '(' LArgOpt ')'    { $$ = creer_noeud_appel($1, $3, $5, FAUX); }
         | ID_CLASS '.' ID '(' LArgOpt ')'     { $$ = creer_noeud_appel(creer_feuille_id($1), $3, $5, VRAI); }
;

IfThenElse : IF Expr THEN BlocExpr ELSE BlocExpr     { $$ = creer_arbre_ITE($2, $4, $6); }
;

REL : RELOP { $$ = $1; } /* Nécessaire pour avoir une copie du caractère. */
;

// liste d'arguments optionnelle
LArgOpt : LArg     { $$ = $1; }
        |      { $$ = nouvelle_liste_arguments(NIL(arg_t)); }
;

// liste d'arguments
LArg : LArg ',' Expr     { $$ = ajouter_argument($1, nouvel_argument($3)); }
     | Expr  { $$ = nouvelle_liste_arguments(nouvel_argument($1)); }
;
