#include "arbres.h"

/**
 * Constructeur de feuille de type ID pour l'arbre de syntaxe abstraite: 
 * on stocke l'id.
 */
arbre_t* creer_feuille_id(char* var)
{
  arbre_t* res = NEW(1, arbre_t);
  res->op = Id; res->gauche.S = var; res->droit.A = NIL(arbre_t);
  return(res);
}

/**
 * Idem pour une feuille de type CSTE : on stocke la valeur.
 */
arbre_t* creer_feuille_cste(int val)
{
  arbre_t* res = NEW(1, arbre_t);
  res->op = Cste; res->gauche.E = val; res->droit.A = NIL(arbre_t);
  return(res);
}

/**
 * Constructeur de feuille de type CHAINE : on stocke la valeur.
 */
arbre_t* creer_feuille_chaine(char* chaine)
{
  arbre_t* res = NEW(1, arbre_t);
  res->op = Chaine; res->gauche.S = chaine; res->droit.A = NIL(arbre_t);
  return(res);
}

/**
 * Constructeur pour un noeud interne: une etiquette pour l'operateur, et deux
 * sous-arbres.
 */
arbre_t* creer_noeud(char op, arbre_t* g, arbre_t* d)
{
 arbre_t* res = NEW(1, arbre_t);
 res->op = op; res->gauche.A = g; res->droit.A = d;
 return(res);
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
arbre_t* creer_arbre_ITE(arbre_t* pCond, arbre_t* pThen, arbre_t* pElse)
{
  arbre_t* p = creer_noeud(NOP, pThen, pElse);
  return creer_noeud(ITE, pCond, p); 
}

/* XXX : Pas utile pour le moment, puisqu'on construit un compilateur et non un
 *       interpréteur mais pourrait être peut-être  réutilisé pour autre chose ...
 * 
 * parcours recursif de l'arbre representant une expression. Les valeurs
 * des identificateurs situes aux feuilles de l'arbre sont a rechercher
 * dans la liste lv
 *
 * ATTENTION: tous les cas ne sont pas traites dans l'arbre !!!
int evalue(arbre_t* arbre, PVAR lv) {
  int g, d;

  switch (arbre->op) {
  case Id:
    while (lv) {
      if (! strcmp(lv->nom, arbre->gauche.S)) return(lv->val);
      else lv = lv->suiv;
    }
    fprintf(stderr, "Variable non declaree: %s\n", arbre->gauche.S);
    exit(3);
  case Cste:
    return(arbre->gauche.E);
  case EQ:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g == d);
  case NEQ:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g != d);
  case GT:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g > d);
  case GE:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g >= d);
  case LT:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g < d);
  case LE:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g <= d);
  case '+':
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g + d);
  case '*':
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g * d);
  case ITE: * if_then_else_ *
     * le fils gauche represente la condition, le fils droit un arbre dont
     * les deux fils correspondent respectivement aux parties then et else.
     * Attention a n'evaluer que la partie necessaire !
     *
    if (evalue(arbre->gauche.A, lv)) 
       return evalue(arbre->droit.A->gauche.A, lv);
    else return evalue(arbre->droit.A->droit.A, lv);
  default: fprintf(stderr, "Cas non prevu dans Evaluation: %c\n", arbre->op);
    exit(4);
  }
}
*/
