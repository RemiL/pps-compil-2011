#ifndef VERIF_CONTEXTUELLES_H
#define VERIF_CONTEXTUELLES_H

#include "structures.h"

void est_valide_classe(liste_classes_t decl, classe_t* classe);
void sont_valides_attributs(liste_classes_t decl, classe_t* classe);
char* masque_attribut(classe_t* classe_mere, char* attribut);
void sont_valides_methodes(liste_classes_t decl, classe_t* classe);

#endif
