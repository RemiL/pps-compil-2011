#ifndef ARBRE_H
#define ARBRE_H

#include "utils.h"
#include "structures.h"

/* Pour éviter les dépendances cylindriques */
#ifndef arbre_t
typedef struct arbre arbre_t;
#define arbre_t arbre_t
#endif

/**
 * Etiquettes pour les noeuds de l'arbre de syntaxe abstraite representant
 * une expression.
 * Les opérateurs arithmétiques sont directement utilisés aussi comme
 * etiquettes.
 */
typedef enum
{ 
  Id,
  Cste,
  Chaine,
  Bloc,
  Aff,                     /* Affectation */
  EQ, NEQ, GT, GE, LT, LE, /* les differents operateurs de comparaison */
  ITE,                     /* le if-then-else */
  Selection,               /* Selection */
  SelectionStatique,       /* Selection statique */
  Appel,                   /* Envoi de message */
  AppelStatique,           /* Envoi de message statique */
  New,                     /* Création dynamique d'objets */
  NOP 			               /* etiquette "auxiliaire */
} etiquette_t;

/* la structure d'un noeud de l'arbre: un noeud interne ou une feuille */
typedef union
{
  char* S;        /* feuille de type Identificateur ou Chaine */
  int E;	 	      /* feuille de type constante entiere */
  liste_vars_t vars; /* feuille de type liste de variables utilisé pour les noeuds de type Bloc */
  liste_args_t args; /* feuille de type liste d'arguments utilisé pour les appels */
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
arbre_t* creer_arbre_ITE(arbre_t* cond, arbre_t* expr_then, arbre_t* expr_else);
arbre_t* creer_noeud_bloc(liste_vars_t vars, arbre_t* expr);
arbre_t* creer_noeud_selection(arbre_t* dest, char* nom_attribut, int statique);
arbre_t* creer_noeud_appel(arbre_t* dest, char* nom_methode, liste_args_t args, int statique);
arbre_t* creer_noeud_new(char* nom_classe, liste_args_t args);

#endif
