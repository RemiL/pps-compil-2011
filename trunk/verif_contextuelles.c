#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "verif_contextuelles.h"

// TODO mettre ça à un endroit plus approprié.
#define VRAI 1
#define FAUX 0

void est_valide_classe(liste_classes_t decl, classe_t* classe)
{  
  classe->classe_mere = chercher_classe(decl, classe->nom_classe_mere);
  
  if (chercher_classe(decl, classe->nom) != NULL)
  {
    printf("Classe %s invalide : une classe portant de le même nom existe déjà.\n", classe->nom);
    exit(EXIT_FAILURE);
  }
  
  if (classe->nom_classe_mere != NULL)
  {
    if (!strcmp(classe->nom, classe->nom_classe_mere))
    {
      printf("Classe %s invalide : une classe ne peut pas hériter d'elle-même.\n", classe->nom);
      exit(EXIT_FAILURE);
    }
    else if (classe->classe_mere == NULL)
    {
      printf("Classe %s invalide : la classe mère %s n'existe pas.\n", classe->nom, classe->nom_classe_mere);
      exit(EXIT_FAILURE);
    }
    else if (!strcmp("Entier", classe->classe_mere->nom) || !strcmp("Chaine", classe->classe_mere->nom))
    {
      printf("Classe %s invalide : la classe %s ne peut pas avoir de sous-classes.\n", classe->nom, classe->classe_mere->nom);
      exit(EXIT_FAILURE);
    }
  }
  
  sont_valides_attributs(decl, classe);
  sont_valides_methodes(decl, classe);
}

void sont_valides_attributs(liste_classes_t decl, classe_t* classe)
{
  var_t* attribut = classe->attributs.tete;
  char* nom_classe_masquage;
  
  while (attribut != NULL)
  {
    attribut->type = chercher_classe(decl, attribut->nom_type);
    
    if (attribut->type == NULL)
    {
      if (!strcmp(attribut->nom_type, classe->nom))
        attribut->type = classe;
      else
      {
        printf("Classe %s : Type invalide pour l'attribut %s : la classe %s est inconnue.\n", classe->nom, attribut->nom, attribut->nom_type);
        exit(EXIT_FAILURE);
      }
    }
    else if ((nom_classe_masquage = masque_attribut(classe->classe_mere, attribut->nom)) != NULL)
    {
      printf("Classe %s : l'attribut %s masque un attribut de la classe mère %s.\n", classe->nom, attribut->nom, nom_classe_masquage);
      exit(EXIT_FAILURE);
    }
    
    attribut = attribut->suiv;
  }
}

char* masque_attribut(classe_t* classe_mere, char* attribut)
{
  if (classe_mere == NULL)
    return NULL;
  else if (chercher_variable(classe_mere->attributs, attribut) != NULL)
    return classe_mere->nom;
  else
    return masque_attribut(classe_mere->classe_mere, attribut);
}

void sont_valides_methodes(liste_classes_t decl, classe_t* classe)
{
  methode_t* methode = classe->methodes.tete;
  
  while (methode != NULL)
  {
    methode->type_retour = chercher_classe(decl, methode->nom_type_retour);
    
    if (methode->nom_type_retour != NULL && methode->type_retour == NULL)
    {
      if (!strcmp(methode->nom_type_retour, classe->nom))
        methode->type_retour = classe;
      else
      {
        printf("Classe %s : Type de retour invalide pour la méthode %s : la classe %s est inconnue.\n", classe->nom, methode->nom, methode->nom_type_retour);
        exit(EXIT_FAILURE);
      }
    }
    
    methode = methode->suiv;
  }
}
