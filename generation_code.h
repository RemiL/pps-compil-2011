#ifndef GENERATION_CODE_H
#define GENERATION_CODE_H

#include "utils.h"
#include "structures.h"
#include "arbres.h"

void calculer_index(liste_classes_t classes, arbre_t* prog_principal);
int calculer_index_attributs(classe_t* classe, int decalage_bg);
int calculer_index_methodes(classe_t* classe, int decalage_bg);
void calculer_index_params(methode_t* methode);
int calculer_index_variables_locales_arbre(arbre_t* arbre, int index);
int calculer_index_variables_locales(liste_vars_t variables, int index);

#endif
