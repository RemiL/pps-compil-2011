#ifndef MAIN_H
#define MAIN_H

#include "utils.h"
#include "structures.h"
#include "arbres.h"
#include "verif_contextuelles.h"

typedef union
{ 
  char C;             /* necessaire pour flex                             */
  char* S;            /* les autres correspondent aux variantes utilisees */
  arbre_t* A;         /* dans les actions associees aux productions de    */
  int E;              /* la grammaire.                                    */
  liste_classes_t LClasses;
  classe_t* Classe;
  corps_t Corps;
  liste_methodes_t LMethodes;
  methode_t* Methode;
  type_methode_t TypeMethode;
  liste_params_t LParams;
  param_t* Param;
  liste_vars_t LVars;
  var_t* Var;
} YYSTYPE;

#define YYSTYPE YYSTYPE

#endif