#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "structures.h"
#include "arbres.h"

var_t* nouvelle_variable(char* nom, char* type, int constante, int statique, arbre_t* valeur_defaut)
{
  var_t* var = NEW(1, var_t);
  
  var->nom = nom;
  var->nom_type = type;
  if (constante)
    var->constante = (valeur_defaut != NIL(arbre_t)) ? CONSTANTE_INITIALISEE : CONSTANTE_NON_INITIALISEE;
  else
    var->constante = NON_CONSTANTE;
  var->statique = statique;
  var->valeur_defaut = valeur_defaut;
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

liste_vars_t concatener_liste_variables(liste_vars_t l1, liste_vars_t l2)
{
  liste_vars_t liste_vars;
  
  liste_vars.tete = (l1.tete != NIL(var_t)) ? l1.tete : l2.tete;
  liste_vars.queue = (l2.queue != NIL(var_t)) ? l2.queue : l1.queue;

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
    liberer_arbre(variable->valeur_defaut);
    
    tmp = variable;
    variable = variable->suiv;
    
    free(tmp);
  }
}

param_t* nouveau_param(char* nom, char* type, arbre_t* valeur_defaut)
{
  param_t* param = NEW(1, param_t);
  
  param->nom = nom;
  param->nom_type = type;
  param->valeur_defaut = valeur_defaut;
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

liste_params_t ajouter_param_en_tete(liste_params_t liste_params, param_t* param)
{
  liste_params_t l;
  
  if (liste_params.tete == NIL(param_t)) /* Liste vide */
    l.queue = param;
  else
  {
    param->suiv = liste_params.tete;
    l.queue = liste_params.queue;
  }
  
  l.tete = param;
  
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
    liberer_arbre(param->valeur_defaut);
    
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
    liberer_arbre(methode->bloc);
    
    free(methode->nom);
    if (methode->nom_type_retour)
      free(methode->nom_type_retour);
    
    tmp = methode;
    methode = methode->suiv;
    
    free(tmp);
  }
}

arg_t* nouvel_argument(arbre_t* expr)
{
  arg_t* arg = NEW(1, arg_t);
  
  arg->expr = expr;
  arg->suiv = NULL;
  
  return arg;
}

liste_args_t ajouter_argument(liste_args_t liste_arguments, arg_t* arg)
{
  liste_args_t l;
  
  if (liste_arguments.tete == NIL(arg_t)) /* Liste vide */
    l.tete = arg;
  else
  {
    liste_arguments.queue->suiv = arg;
    l.tete = liste_arguments.tete;
  }
  
  l.queue = arg;
  
  return l;
}

liste_args_t nouvelle_liste_arguments(arg_t* arg)
{
  liste_args_t liste_arguments;
  
  liste_arguments.tete = arg;
  liste_arguments.queue = arg;
  
  return liste_arguments;
}

void liberer_liste_arguments(liste_args_t liste_arguments)
{
  arg_t* arg = liste_arguments.tete;
  arg_t* tmp;
  
  while (arg != NIL(arg_t))
  {    
    liberer_arbre(arg->expr);
    
    tmp = arg;
    arg = arg->suiv;
    
    free(tmp);
  }
}

classe_t* nouvelle_classe(char* nom, char* classe_mere, liste_args_t args_classe_mere,
                          liste_params_t params_constructeur, arbre_t* bloc_constructeur,
                          liste_vars_t attributs, liste_methodes_t methodes)
{
  classe_t* classe = NEW(1, classe_t);
  
  classe->nom = nom;
  classe->nom_classe_mere = classe_mere;
  classe->args_classe_mere = args_classe_mere;
  classe->attributs = attributs;
  classe->methodes = methodes;
  classe->suiv = NIL(classe_t);
  
  /* Ajout constructeur (p-e à revoir ?) */
  classe->constructeur = nouvelle_methode(strdup(nom), NORMALE, params_constructeur, bloc_constructeur, NULL);
  classe->constructeur->type_retour = classe; /* Simplifie les manipulations mais un constructeur ne retourne rien */
  
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

/**
 * Recherche si une classe ou une des classes dont elle descend
 * possède une méthode du nom fourni.
 */
methode_t* chercher_methode_arborescence_classe(classe_t* classe, char* nom)
{
  methode_t* methode = NIL(methode_t);
  
  while (classe != NIL(classe_t) && methode == NIL(methode_t))
  {
    methode = chercher_methode(classe->methodes, nom);
    classe = classe->classe_mere;
  }
  
  return methode;
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
  liste_classes = nouvelle_liste_classes(nouvelle_classe(strdup("Entier"), NULL, nouvelle_liste_arguments(NIL(arg_t)),
                                         nouvelle_liste_params(NIL(param_t)), NIL(arbre_t),
                                         nouvelle_liste_variables(NIL(var_t)),
                                         nouvelle_liste_methodes(nouvelle_methode(strdup("imprimer"), NORMALE, nouvelle_liste_params(NIL(param_t)), NIL(arbre_t), strdup("Entier")))));
  liste_classes = ajouter_classe(liste_classes,
                                 nouvelle_classe(strdup("Chaine"), NULL, nouvelle_liste_arguments(NIL(arg_t)),
                                                 nouvelle_liste_params(NIL(param_t)), NIL(arbre_t),
                                                 nouvelle_liste_variables(NIL(var_t)),
                                                 nouvelle_liste_methodes(nouvelle_methode(strdup("imprimer"), NORMALE, nouvelle_liste_params(NIL(param_t)), NIL(arbre_t), strdup("Chaine")))));
  
  return liste_classes;
}

void liberer_liste_classes(liste_classes_t liste_classes)
{
  classe_t* classe = liste_classes.tete;
  classe_t* tmp;
  
  while (classe != NIL(classe_t))
  {
    liberer_liste_arguments(classe->args_classe_mere);
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

heritage_t nouvel_heritage(char* nom_classe_mere, liste_args_t args_classe_mere)
{
  heritage_t heritage;
  
  heritage.nom_classe_mere = nom_classe_mere;
  heritage.args_classe_mere = args_classe_mere;
  
  return heritage;
}

decl_vars_t* decl_ajouter_attributs(decl_vars_t* decl, liste_vars_t attributs)
{
  if (attributs.tete == NIL(var_t))
    return decl;
  
  decl_vars_t* d = NEW(1, decl_vars_t);
  
  d->decl.vars = attributs;
  d->type = ATTRIBUT;
  d->suiv = decl;
  
  return d;
}

decl_vars_t* decl_ajouter_variables(decl_vars_t* decl, liste_vars_t variables)
{
  if (variables.tete == NIL(var_t))
    return decl;
  
  decl_vars_t* d = NEW(1, decl_vars_t);
  
  d->decl.vars = variables;
  d->type = VARIABLE;
  d->suiv = decl;
  
  return d;
}

decl_vars_t* decl_ajouter_params(decl_vars_t* decl, liste_params_t params)
{
  if (params.tete == NIL(param_t))
    return decl;
  
  decl_vars_t* d = NEW(1, decl_vars_t);
  
  d->decl.params = params;
  d->type = PARAM;
  d->suiv = decl;

  return d;
}

decl_vars_t* decl_generer_depuis_classe(classe_t* classe)
{
  if (classe != NIL(classe_t))
    return decl_ajouter_attributs(decl_generer_depuis_classe(classe->classe_mere), classe->attributs);
  else
    return NIL(decl_vars_t);
}

type_decl_t decl_chercher_id(decl_vars_t* decl, char* id, var_t** var, param_t** param)
{
  type_decl_t type = 0;
  
  while (decl != NIL(decl_vars_t) && type == 0)
  {
    if (decl->type == PARAM)
    {
      if ((*param = chercher_param(decl->decl.params, id)) != NIL(param_t))
        type = PARAM;
    }
    else if ((*var = chercher_variable(decl->decl.vars, id)) != NIL(var_t))
      type = decl->type;
    
    decl = decl->suiv;
  }
  
  return type;
}

corps_t nouveau_corps(liste_vars_t variables, liste_methodes_t methodes)
{
  corps_t corps;
  
  corps.variables = variables;
  corps.methodes = methodes;
  
  return corps;
}
