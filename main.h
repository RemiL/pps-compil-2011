/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#ifndef MAIN_H
#define MAIN_H

#include "utils.h"
#include "structures.h"
#include "arbres.h"
#include "verif_contextuelles.h"
#include "generation_code.h"

typedef union
{ 
  char C;             /* necessaire pour flex                             */
  char* S;            /* les autres correspondent aux variantes utilisees */
  arbre_t* A;         /* dans les actions associees aux productions de    */
  int E;              /* la grammaire.                                    */
  liste_classes_t LClasses;
  classe_t* Classe;
  heritage_t Heritage;
  corps_t Corps;
  liste_methodes_t LMethodes;
  methode_t* Methode;
  type_methode_t TypeMethode;
  liste_params_t LParams;
  param_t* Param;
  liste_vars_t LVars;
  var_t* Var;
  liste_args_t LArgs;
} YYSTYPE;

#define YYSTYPE YYSTYPE

#endif
