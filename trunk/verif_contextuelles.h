#ifndef VERIF_CONTEXTUELLES_H
#define VERIF_CONTEXTUELLES_H

#include "structures.h"
#include "arbres.h"

void est_valide_classe(liste_classes_t decl, classe_t* classe);
void sont_valides_attributs(liste_classes_t decl, classe_t* classe);
char* masque_attribut(classe_t* classe_mere, char* attribut);
void sont_valides_methodes(liste_classes_t decl, classe_t* classe);
void sont_valides_params(liste_classes_t decl, classe_t* classe, methode_t* methode);
void est_valide_methode(liste_methodes_t liste, methode_t* methode);
void est_valide_redefinition(classe_t* classe, methode_t* methode, methode_t* methode_redefinie);
void est_valide_param(liste_params_t liste, param_t* parametre);
void est_valide_attribut(classe_t* classe, char* attribut);
void sont_valides_variables(liste_classes_t decl_classes, decl_vars_t* decl_vars, liste_vars_t variables);
void sont_valides_arguments(liste_classes_t decl_classes, decl_vars_t* decl_vars, methode_t* methode, liste_args_t arguments);
classe_t* est_valide_arbre_syntaxique(liste_classes_t decl_classes, decl_vars_t* decl_vars, arbre_t* arbre);

#endif
