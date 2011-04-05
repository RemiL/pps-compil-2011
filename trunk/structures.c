#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "arbres.h"

var_t* nouvelle_variable(char* nom, char* type, int constante, int statique)
{
  var_t* var = NEW(1, var_t);
  
  var->nom = nom;
  var->nom_type = type;
  var->constante = constante;
  var->statique = statique;
  var->suiv = NIL(var_t);
  
  return var;
}

liste_vars_t ajouter_variable(liste_vars_t liste_vars, var_t* var)
{
  liste_vars_t l;
  
  if (liste_vars.tete == NIL(var_t)) /* Liste vide */
    l.tete = var;
  else
  {
    liste_vars.queue->suiv = var;
    l.tete = liste_vars.tete;
  }
  
  l.queue = var;
  
  return l;
}

var_t* chercher_variable(liste_vars_t liste_vars, char* nom)
{
  var_t* var = (nom == NULL) ? NIL(var_t) : liste_vars.tete;
  
  while (var != NIL(var_t) && (strcmp(nom, var->nom) != 0))
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

void liberer_liste_variables(liste_vars_t liste_variables)
{
  var_t* variable = liste_variables.tete;
  var_t* tmp;
  
  while (variable != NIL(var_t))
  {
    free(variable->nom);
    free(variable->nom_type);
    
    tmp = variable;
    variable = variable->suiv;
    
    free(tmp);
  }
}

param_t* nouveau_param(char* nom, char* type /* TODO expression par défaut */)
{
  param_t* param = NEW(1, param_t);
  
  param->nom = nom;
  param->nom_type = type;
  param->suiv = NIL(param_t);
  
  return param;
}

liste_params_t ajouter_param(liste_params_t liste_params, param_t* param)
{
  liste_params_t l;
  
  if (liste_params.tete == NIL(param_t)) /* Liste vide */
    l.tete = param;
  else
  {
    liste_params.queue->suiv = param;
    l.tete = liste_params.tete;
  }
  
  l.queue = param;
  
  return l;
}

param_t* chercher_param(liste_params_t liste_params, char* nom)
{
  param_t* param = (nom == NULL) ? NIL(param_t) : liste_params.tete;
  
  while (param != NIL(param_t) && (strcmp(nom, param->nom) != 0))
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

void liberer_liste_params(liste_params_t liste_params)
{
  param_t* param = liste_params.tete;
  param_t* tmp;
  
  while (param != NIL(param_t))
  {
    free(param->nom);
    free(param->nom_type);
    
    tmp = param;
    param = param->suiv;
    
    free(tmp);
  }
}

methode_t* nouvelle_methode(char* nom, type_methode_t type_methode, liste_params_t params, arbre_t* bloc, char* type_retour)
{
  methode_t* methode = NEW(1, methode_t);
  
  methode->nom = nom;
  methode->type_methode = type_methode;
  methode->params = params;
  methode->bloc = bloc;
  methode->nom_type_retour = type_retour;
  methode->suiv = NIL(methode_t);
  
  return methode;
}

liste_methodes_t ajouter_methode(liste_methodes_t liste_methodes, methode_t* methode)
{
  liste_methodes_t l;
  
  if (liste_methodes.tete == NIL(methode_t)) /* Liste vide */
    l.tete = methode;
  else
  {
    liste_methodes.queue->suiv = methode;
    l.tete = liste_methodes.tete;
  }
  
  l.queue = methode;
  
  return l;
}

void ajouter_methode_tete(liste_methodes_t* liste_methodes, methode_t* methode)
{
  if (liste_methodes->tete == NIL(methode_t)) /* Liste vide */
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

methode_t* chercher_methode(liste_methodes_t liste_methodes, char* nom)
{
  methode_t* methode = (nom == NULL) ? NIL(methode_t) : liste_methodes.tete;
  
  while (methode != NIL(methode_t) && (strcmp(nom, methode->nom) != 0))
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

void liberer_liste_methodes(liste_methodes_t liste_methodes)
{
  methode_t* methode = liste_methodes.tete;
  methode_t* tmp;
  
  while (methode != NIL(methode_t))
  {
    liberer_liste_params(methode->params);
    
    free(methode->nom);
    if (methode->nom_type_retour)
      free(methode->nom_type_retour);
    
    tmp = methode;
    methode = methode->suiv;
    
    free(tmp);
  }
}

classe_t* nouvelle_classe(char* nom, char* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes)
{
  classe_t* classe = NEW(1, classe_t);
  
  classe->nom = nom;
  classe->nom_classe_mere = classe_mere;
  classe->attributs = attributs;
  classe->methodes = methodes;
  classe->suiv = NIL(classe_t);
  
  /* Ajout constructeur (p-e à revoir ?) */
  ajouter_methode_tete(&classe->methodes, nouvelle_methode(strdup(nom), NORMALE, params_constructeur, NIL(arbre_t), NULL));
  
  return classe;
}

liste_classes_t ajouter_classe(liste_classes_t liste_classes, classe_t* classe)
{
  liste_classes_t l;
  
  if (liste_classes.tete == NIL(classe_t)) /* Liste vide */
    l.tete = classe;
  else
  {
    liste_classes.queue->suiv = classe;
    l.tete = liste_classes.tete;
  }
  
  l.queue = classe;
  
  return l;
}

classe_t* chercher_classe(liste_classes_t liste_classes, char* nom)
{
  classe_t* classe = (nom == NULL) ? NIL(classe_t) : liste_classes.tete;
  
  while (classe != NIL(classe_t) && (strcmp(nom, classe->nom) != 0))
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


liste_classes_t nouvelle_liste_classes_preinitialisee()
{
  liste_classes_t liste_classes; 
  
  /* TODO à compléter ? */
  liste_classes = nouvelle_liste_classes(nouvelle_classe(strdup("Entier"), NULL, nouvelle_liste_params(NIL(param_t)), nouvelle_liste_variables(NIL(var_t)), nouvelle_liste_methodes(NIL(methode_t))));
  liste_classes = ajouter_classe(liste_classes, nouvelle_classe(strdup("Chaine"), NULL, nouvelle_liste_params(NIL(param_t)), nouvelle_liste_variables(NIL(var_t)), nouvelle_liste_methodes(NIL(methode_t))));
  
  return liste_classes;
}

void liberer_liste_classes(liste_classes_t liste_classes)
{
  classe_t* classe = liste_classes.tete;
  classe_t* tmp;
  
  while (classe != NIL(classe_t))
  {
    liberer_liste_variables(classe->attributs);
    liberer_liste_methodes(classe->methodes);
    
    free(classe->nom);
    if (classe->nom_classe_mere)
      free(classe->nom_classe_mere);
    
    tmp = classe;
    classe = classe->suiv;
    
    free(tmp);
  }
}

corps_t nouveau_corps(liste_vars_t variables, liste_methodes_t methodes)
{
  corps_t corps;
  
  corps.variables = variables;
  corps.methodes = methodes;
  
  return corps;
}
