#include <stdlib.h>
#include <string.h>

#include "structures.h"

// TODO mettre ça à un endroit plus approprié.
#define VRAI 1
#define FAUX 0

attribut_t* nouvel_attribut(char* nom, classe_t* type, int constante, int statique)
{
  attribut_t* attribut = (attribut_t*) malloc(sizeof(attribut_t));
  
  attribut->nom = strdup(nom);
  attribut->type = type;
  attribut->constante = constante;
  attribut->statique = statique;
  attribut->suiv = NULL;
  
  return attribut;
}

void ajouter_attribut(liste_attributs_t* liste_attributs, attribut_t* attribut)
{
  if (liste_attributs->tete == NULL) /* Liste vide */
  {
    liste_attributs->tete = attribut;
    liste_attributs->queue = attribut;
  }
  else
  {
    liste_attributs->queue->suiv = attribut;
    liste_attributs->queue = attribut;
  }
}

attribut_t* chercher_attribut(liste_attributs_t* liste_attributs, char* nom)
{
  attribut_t* attribut = liste_attributs->tete;
  
  while (attribut != NULL && (strcmp(nom, attribut->nom) != 0))
    attribut = attribut->suiv;
  
  return attribut;
}

param_t* nouveau_param(char* nom, classe_t* type /* TODO expression par défaut */)
{
  param_t* param = (param_t*) malloc(sizeof(param_t));
  
  param->nom = strdup(nom);
  param->type = type;
  param->suiv = NULL;
  
  return param;
}

void ajouter_param(liste_params_t* liste_params, param_t* param)
{
  if (liste_params->tete == NULL) /* Liste vide */
  {
    liste_params->tete = param;
    liste_params->queue = param;
  }
  else
  {
    liste_params->queue->suiv = param;
    liste_params->queue = param;
  }
}

param_t* chercher_param(liste_params_t* liste_params, char* nom)
{
  param_t* param = liste_params->tete;
  
  while (param != NULL && (strcmp(nom, param->nom) != 0))
    param = param->suiv;
  
  return param;
}

methode_t* nouvelle_methode(char* nom, type_methode_t type_methode, liste_params_t params, classe_t* type_retour)
{
  methode_t* methode = (methode_t*) malloc(sizeof(methode_t));
  
  methode->nom = strdup(nom);
  methode->type_methode = type_methode;
  methode->params = params;
  methode->type_retour = type_retour;
  methode->suiv = NULL;
  
  return methode;
}

void ajouter_methode(liste_methodes_t* liste_methodes, methode_t* methode)
{
  if (liste_methodes->tete == NULL) /* Liste vide */
  {
    liste_methodes->tete = methode;
    liste_methodes->queue = methode;
  }
  else
  {
    liste_methodes->queue->suiv = methode;
    liste_methodes->queue = methode;
  }
}

void ajouter_methode_tete(liste_methodes_t* liste_methodes, methode_t* methode)
{
  if (liste_methodes->tete == NULL) /* Liste vide */
  {
    liste_methodes->tete = methode;
    liste_methodes->queue = methode;
  }
  else
  {
    methode->suiv = liste_methodes->tete;
    liste_methodes->tete = methode;
  }
}

methode_t* chercher_methode(liste_methodes_t* liste_methodes, char* nom)
{
  methode_t* methode = liste_methodes->tete;
  
  while (methode != NULL && (strcmp(nom, methode->nom) != 0))
    methode = methode->suiv;
  
  return methode;
}

classe_t* nouvelle_classe(char* nom, classe_t* classe_mere, liste_params_t params_constructeur, liste_attributs_t attributs, liste_methodes_t methodes)
{
  classe_t* classe = (classe_t*) malloc(sizeof(classe_t));
  
  classe->nom = strdup(nom);
  classe->classe_mere = classe_mere;
  classe->attributs = attributs;
  classe->methodes = methodes;
  classe->suiv = NULL;
  
  /* Ajout constructeur (p-e à revoir ?) */
  ajouter_methode_tete(&classe->methodes, nouvelle_methode(nom, NORMALE, params_constructeur, NULL));
  
  return classe;
}

void ajouter_classe(liste_classes_t* liste_classes, classe_t* classe)
{
  if (liste_classes->tete == NULL) /* Liste vide */
  {
    liste_classes->tete = classe;
    liste_classes->queue = classe;
  }
  else
  {
    liste_classes->queue->suiv = classe;
    liste_classes->queue = classe;
  }
}

classe_t* chercher_classe(liste_classes_t* liste_classes, char* nom)
{
  classe_t* classe = liste_classes->tete;
  
  while (classe != NULL && (strcmp(nom, classe->nom) != 0))
    classe = classe->suiv;
  
  return classe;
}

