#ifndef ARBRE_H
#define ARBRE_H

#include "utils.h"
#include "structures.h"

/* Etiquettes pour les noeuds de l'arbre de syntaxe abstraite representant une
 * expression.
 * Les opérateurs arithmétiques sont directement utilisés aussi comme
 * etiquettes.
 */
typedef enum
{ 
  Id,
  Cste,
  Chaine,
  EQ, NEQ, GT, GE, LT, LE, /* les differents operateurs de comparaison */
  ITE,                     /* le if-then-else */
  NOP 			               /* etiquette "auxiliaire */
} etiquette_t;

typedef struct arbre arbre_t;

/* la structure d'un noeud de l'arbre: un noeud interne ou une feuille */
typedef union
{
  char* S;        /* feuille de type Identificateur */
  int E;	 	      /* feuille de type constante entiere */
  arbre_t* A;     /* noeud interne : un operateur et deux operandes */
} noeud_t;

/* la structure d'un noeud interne */
struct arbre
{
  char op;		            /* une etiquette : voir l'enumeration ci-dessus ou directement le caractère correspondant à l'opérateur. */
  noeud_t gauche, droit;  /* deux noeuds : internes ou feuilles */
};

arbre_t* creer_feuille_id(char* var);
arbre_t* creer_feuille_cste(int val);
arbre_t* creer_feuille_chaine(char* chaine);
arbre_t* creer_noeud(char op, arbre_t* g, arbre_t* d);
arbre_t* creer_noeud_oppose(arbre_t* expr);
arbre_t* creer_arbre_ITE(arbre_t* pCond, arbre_t* pThen, arbre_t* pElse);

#endif
