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

void generer_code(FILE* fichier, liste_classes_t classes, arbre_t* prog_principal);
void generer_code_classes_predefinies(FILE* fichier);
void generer_code_classes(FILE* fichier, liste_classes_t classes);
void generer_code_constructeur(FILE* fichier, classe_t* classe);
void generer_code_appel_constructeur_classes_parentes(FILE* fichier, classe_t* classe);
void generer_code_methodes(FILE* fichier, classe_t* classe);

#endif
