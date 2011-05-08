/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#include <stdio.h>

#include "arbres.h"

extern int yylineno;

/**
 * Constructeur de feuille de type ID pour l'arbre de syntaxe abstraite: 
 * on stocke l'id.
 */
arbre_t* creer_feuille_id(char* var)
{
  arbre_t* res = NEW(1, arbre_t);
  res->num_ligne = yylineno;
  res->op = Id; res->gauche.S = var; res->droit.A = NIL(arbre_t);
  return res;
}

/**
 * Idem pour une feuille de type CSTE : on stocke la valeur.
 */
arbre_t* creer_feuille_cste(int val)
{
  arbre_t* res = NEW(1, arbre_t);
  res->num_ligne = yylineno;
  res->op = Cste; res->gauche.E = val; res->droit.A = NIL(arbre_t);
  return res;
}

/**
 * Constructeur de feuille de type CHAINE : on stocke la valeur.
 */
arbre_t* creer_feuille_chaine(char* chaine)
{
  arbre_t* res = NEW(1, arbre_t);
  res->num_ligne = yylineno;
  res->op = Chaine; res->gauche.S = chaine; res->droit.A = NIL(arbre_t);
  return res;
}

/**
 * Constructeur pour un noeud interne: une etiquette pour l'operateur, et deux
 * sous-arbres.
 */
arbre_t* creer_noeud(char op, arbre_t* g, arbre_t* d)
{
  arbre_t* res = NEW(1, arbre_t);
  res->num_ligne = yylineno;
  res->op = op; res->gauche.A = g; res->droit.A = d;
  return res;
}

/**
 * Constructeur de noeud pour l'opérateur "opposé".
 * On traite simplement -expr comme 0 - expr.
 */
arbre_t* creer_noeud_oppose(arbre_t* expr)
{
  return creer_noeud('-', creer_feuille_cste(0), expr);
}

/**
 * Constructeur pour le if_then_else qui oblige a coder un arbre ternaire avec
 * des arbres binaires. On cree un arbre racine d'etiquette ITE, de sous-arbre
 * gauche la condition et de sous-arbre droit un autre arbre d'etiquette
 * arbitraire (NOP) et dont les fils sont les arbres des parties then et else.
 */
arbre_t* creer_arbre_ITE(arbre_t* cond, arbre_t* expr_then, arbre_t* expr_else)
{
  arbre_t* a = creer_noeud(NOP, expr_then, expr_else);
  return creer_noeud(ITE, cond, a);
}

/**
 * Constructeur pour les noeuds de type Bloc : le fils gauche est une feuille
 * contenant un pointeur sur une liste de variables, le fils droit est l'arbre
 * syntaxique du bloc.
 */
arbre_t* creer_noeud_bloc(liste_vars_t vars, arbre_t* expr)
{
  arbre_t* res = NEW(1, arbre_t);
  res->num_ligne = yylineno;
  res->op = Bloc; res->gauche.vars = vars; res->droit.A = expr;
  return res;
}

/**
 * Constructeur pour les noeuds de type sélection : le fils gauche est le destinataire
 * et le fils droit est une feuille contenant le nom de l'attribut.
 */
arbre_t* creer_noeud_selection(arbre_t* dest, char* nom_attribut, int statique)
{
  arbre_t* selection = NEW(1, arbre_t);
  selection->num_ligne = yylineno;
  selection->op = statique ? SelectionStatique : Selection;
  selection->gauche.A = dest;
  selection->droit.S = nom_attribut;
  
  return selection;
}

/**
 * Constructeur pour l'envoi de message qui oblige a coder un arbre ternaire avec
 * des arbres binaires. On cree un arbre racine d'etiquette Appel ou AppelStatique,
 * de sous-arbre gauche le destinataire et de sous-arbre droit un autre arbre d'etiquette
 * AppelAuxiliaire dont le fil gauche est le nom de la methode et le fils droit est
 * un pointeur vers une liste d'arguments.
 */
arbre_t* creer_noeud_appel(arbre_t* dest, char* nom_methode, liste_args_t args, int statique)
{
  arbre_t* a = NEW(1, arbre_t);
  a->op = AppelAuxiliaire;
  a->gauche.S = nom_methode;
  a->droit.args = args;
  
  return creer_noeud(statique ? AppelStatique : Appel, dest, a);
}

/**
 * Constructeur pour les noeuds de type New : le fils gauche est une feuille contenant
 * le nom de la classe de l'objet à créer et le fils droit est une feuille contenant
 * un pointeur vers une liste d'argument.
 */
arbre_t* creer_noeud_new(char* nom_classe, liste_args_t args)
{
  arbre_t* new = NEW(1, arbre_t);
  new->num_ligne = yylineno;
  new->op = New;
  new->gauche.S = nom_classe;
  new->droit.args = args;
  
  return new;
}

/* Libère la mémoire occupée par un arbre syntaxique */
void liberer_arbre(arbre_t* arbre)
{
  if (arbre)
  {
    switch (arbre->op)
    {
      case Cste:
        break;
      case Id:
      case Chaine:
        free(arbre->gauche.S);
        break;
      case Bloc:
        liberer_liste_variables(arbre->gauche.vars);
        liberer_arbre(arbre->droit.A);
        break;
      case Selection:
      case SelectionStatique:
        liberer_arbre(arbre->gauche.A);
        free(arbre->droit.S);
        break;
      case AppelAuxiliaire:
      case New:
        free(arbre->gauche.S);
        liberer_liste_arguments(arbre->droit.args);
        break;
      default:
        liberer_arbre(arbre->gauche.A);
        liberer_arbre(arbre->droit.A);
    }
    
    free(arbre);
  }
}
