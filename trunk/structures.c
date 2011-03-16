#include <stdlib.h>
#include <string.h>

#include "structures.h"

// TODO mettre ça à un endroit plus approprié.
#define VRAI 1
#define FAUX 0

var_t* nouvelle_variable(char* nom, char* type, int constante, int statique)
{
  var_t* var = (var_t*) malloc(sizeof(var_t));
  
  var->nom = nom;
  var->type = type;
  var->constante = constante;
  var->statique = statique;
  var->suiv = NULL;
  
  return var;
}

liste_vars_t ajouter_variable(liste_vars_t liste_vars, var_t* var)
{
  liste_vars_t l;
  
  if (liste_vars.tete == NULL) /* Liste vide */
    l.tete = var;
  else
  {
    l.queue->suiv = var;
    l.tete = liste_vars.tete;
  }
  
  l.queue = var;
  
  return l;
}

var_t* chercher_variable(liste_vars_t* liste_vars, char* nom)
{
  var_t* var = liste_vars->tete;
  
  while (var != NULL && (strcmp(nom, var->nom) != 0))
    var = var->suiv;
  
  return var;
}

liste_vars_t nouvelle_liste_variables(var_t* var)
{
  liste_vars_t liste_vars;
  
  liste_vars.tete = var;
  liste_vars.queue = var;
  
  return liste_vars;
}

param_t* nouveau_param(char* nom, char* type /* TODO expression par défaut */)
{
  param_t* param = (param_t*) malloc(sizeof(param_t));
  
  param->nom = nom;
  param->type = type;
  param->suiv = NULL;
  
  return param;
}

liste_params_t ajouter_param(liste_params_t liste_params, param_t* param)
{
  liste_params_t l;
  
  if (liste_params.tete == NULL) /* Liste vide */
    l.tete = param;
  else
  {
    l.queue->suiv = param;
    l.tete = liste_params.tete;
  }
  
  l.queue = param;
  
  return l;
}

param_t* chercher_param(liste_params_t* liste_params, char* nom)
{
  param_t* param = liste_params->tete;
  
  while (param != NULL && (strcmp(nom, param->nom) != 0))
    param = param->suiv;
  
  return param;
}

liste_params_t nouvelle_liste_params(param_t* param)
{
  liste_params_t liste_params;
  
  liste_params.tete = param;
  liste_params.queue = param;
  
  return liste_params;
}

methode_t* nouvelle_methode(char* nom, type_methode_t type_methode, liste_params_t params, char* type_retour)
{
  methode_t* methode = (methode_t*) malloc(sizeof(methode_t));
  
  methode->nom = nom;
  methode->type_methode = type_methode;
  methode->params = params;
  methode->type_retour = type_retour;
  methode->suiv = NULL;
  
  return methode;
}

liste_methodes_t ajouter_methode(liste_methodes_t liste_methodes, methode_t* methode)
{
  liste_methodes_t l;
  
  if (liste_methodes.tete == NULL) /* Liste vide */
    l.tete = methode;
  else
  {
    l.queue->suiv = methode;
    l.tete = liste_methodes.tete;
  }
  
  l.queue = methode;
  
  return l;
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

liste_methodes_t nouvelle_liste_methodes(methode_t* methode)
{
  liste_methodes_t liste_methodes;
  
  liste_methodes.tete = methode;
  liste_methodes.queue = methode;
  
  return liste_methodes;
}

classe_t* nouvelle_classe(char* nom, char* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes)
{
  classe_t* classe = (classe_t*) malloc(sizeof(classe_t));
  
  classe->nom = nom;
  classe->classe_mere = classe_mere;
  classe->attributs = attributs;
  classe->methodes = methodes;
  classe->suiv = NULL;
  
  /* Ajout constructeur (p-e à revoir ?) */
  ajouter_methode_tete(&classe->methodes, nouvelle_methode(nom, NORMALE, params_constructeur, NULL));
  
  return classe;
}

liste_classes_t ajouter_classe(liste_classes_t liste_classes, classe_t* classe)
{
  liste_classes_t l;
  
  if (liste_classes.tete == NULL) /* Liste vide */
    l.tete = classe;
  else
  {
    l.queue->suiv = classe;
    l.tete = liste_classes.tete;
  }
  
  l.queue = classe;
  
  return l;
}

classe_t* chercher_classe(liste_classes_t* liste_classes, char* nom)
{
  classe_t* classe = liste_classes->tete;
  
  while (classe != NULL && (strcmp(nom, classe->nom) != 0))
    classe = classe->suiv;
  
  return classe;
}

liste_classes_t nouvelle_liste_classes(classe_t* classe)
{
  liste_classes_t liste_classes;
  
  liste_classes.tete = classe;
  liste_classes.queue = classe;
  
  return liste_classes;
}
