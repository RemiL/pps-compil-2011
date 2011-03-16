#include <stdlib.h>

/* deux macros pratiques, utilisées dans les allocations */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define nil(type) (type *) 0


/* Etiquettes pour les noeuds de l'arbre de syntaxe abstraite representant une
 * expression.
 * Les opérateurs arithmétiques sont directement utilisés aussi comme
 * etiquettes.
 */
typedef enum { 
  Id, 
  Cste, 
  EQ, NEQ, GT, GE, LT, LE, /* les differents operateurs de comparaison */
  ITE,                     /* le if-then-else */
  NOP, 			   /* etiquette "auxiliaire */
  Opp,			   /* l'oppose */
  GetEti,
  PutEti
} etiquette;



/* la structure ci-dessous permet de memoriser des listes variable/valeur
 * (entiere). On va construire des listes de la forme { (x 5), (y, 27) } au fur
 * et a mesure qu'on interprete les declarations dans le programme.
 */
typedef struct var
{ char *nom;
  int val;
  struct var *suiv;
} VAR, *PVAR;


/* la structure d'un noeud de l'arbre: un noeud interne ou une feuille */
typedef union {
        char *S;	  /* feuille de type Identificateur */
        int E;	 	  /* feuille de type constante entiere */
        struct arbre *A;  /* noeud interne : un operateur et deux operandes */
} NOEUD;


/* la structure d'un noeud interne ... */
typedef struct arbre
{ char op;		/* une etiquette: voir l'enumeration ci-dessus */
  NOEUD gauche, droit;  /* deux noeuds: internes ou feuilles */
} ARBRE, *PARBRE;


typedef union
{ char C;        /* necessaire pour flex */
  PVAR V;        /* les autres correspondent aux variantes utilisees */
  PARBRE A;        /* dans les actions associees aux productions de    */
  int E;        /* la grammaire.                     */
  char *S;
  liste_params_t LParam;
  liste_classes_t LClasse;
  classe_t Classe;
  corps_t Cor;
  type_methode_t TypeMethode;
} YYSTYPE;

#define YYSTYPE YYSTYPE
