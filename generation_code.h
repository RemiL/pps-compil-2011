/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#ifndef GENERATION_CODE_H
#define GENERATION_CODE_H

#include "utils.h"
#include "structures.h"
#include "arbres.h"

int calculer_index(liste_classes_t classes, arbre_t* prog_principal);
int calculer_index_attributs(classe_t* classe, int decalage_bg);
int calculer_index_methodes(classe_t* classe, int decalage_bg);
void calculer_index_params(methode_t* methode);
int calculer_index_variables_locales_arbre(arbre_t* arbre, int index);
int calculer_index_variables_locales(liste_vars_t variables, int index);

void generer_code(FILE* fichier, liste_classes_t classes, arbre_t* prog_principal, int nb_var_prog_principal);
void generer_code_classes_predefinies(FILE* fichier);
void generer_code_classes(FILE* fichier, liste_classes_t classes);
void generer_code_constructeur(FILE* fichier, classe_t* classe);
void generer_code_appel_constructeur_classes_parentes(FILE* fichier, classe_t* classe);
void generer_code_valeurs_defaut_attributs_non_statiques(FILE* fichier, classe_t* classe);
void generer_code_methodes(FILE* fichier, classe_t* classe);
void generer_code_tables_sauts_attributs_statiques(FILE* fichier, liste_classes_t classes);
void generer_code_table_sauts(FILE* fichier, classe_t* classe);
void generer_code_prog_principal(FILE* fichier, arbre_t* prog_principal, int nb_var_prog_principal);
void generer_code_arbre(FILE* fichier, arbre_t* arbre);
void generer_code_valeurs_defaut_variables(FILE* fichier, liste_vars_t variables);
void generer_code_identifiant(FILE* fichier, arbre_t* arbre);
void generer_code_selection(FILE* fichier, arbre_t* arbre);
void generer_code_affectation(FILE* fichier, arbre_t* arbre);
void generer_code_appel_constructeur(FILE* fichier, arbre_t* arbre);
void generer_code_appel(FILE* fichier, arbre_t* arbre);
int generer_etiquette_ITE();

#endif
