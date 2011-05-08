/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#ifndef VERIF_CONTEXTUELLES_H
#define VERIF_CONTEXTUELLES_H

#include "structures.h"
#include "arbres.h"

void est_valide_classe(liste_classes_t decl, classe_t* classe);
void sont_valides_attributs(liste_classes_t decl, classe_t* classe);
void sont_initialisees_attributs_constants(classe_t* classe);
char* masque_attribut(classe_t* classe_mere, char* attribut);
void sont_valides_methodes(liste_classes_t decl, classe_t* classe);
void sont_valides_params(liste_classes_t decl, classe_t* classe, methode_t* methode);
void est_valide_methode(liste_methodes_t liste, methode_t* methode);
void est_valide_redefinition(classe_t* classe, methode_t* methode, methode_t* methode_redefinie);
void est_valide_param(liste_params_t liste, param_t* parametre);
void sont_valides_variables(liste_classes_t decl_classes, decl_vars_t* decl_vars, liste_vars_t variables, classe_t* type_this, int statique);
void sont_valides_arguments(liste_classes_t decl_classes, decl_vars_t* decl_vars, methode_t* methode, liste_args_t arguments, classe_t* type_this);
int type_est_compatible(classe_t* type1, classe_t* type2);
classe_t* ancetre_commun(classe_t* type1, classe_t* type2);
classe_t* est_valide_arbre_syntaxique(liste_classes_t decl_classes, decl_vars_t* decl_vars, arbre_t* arbre, classe_t* type_this, int statique);

#endif
