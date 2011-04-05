#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "utils.h"

/* Propositions pour les structures permettant de stocker les informations récupérés via les attributs de la grammaire */

/* Déclaration des types */
typedef struct var_t var_t;
typedef struct param_t param_t;
typedef struct methode_t methode_t;
typedef struct classe_t classe_t;

/* Pour éviter les dépendances cylindriques */
#ifndef arbre_t
typedef struct arbre arbre_t;
#define arbre_t arbre_t
#endif

/* Structure représentant une variable */
struct var_t
{
  char* nom;
  char* nom_type;
  classe_t* type;
  /* On peut avoir un attribut statique et constant dans le même temps,
     d'où l'utilisation de deux booléens plutôt qu'un enum. */
  int constante;
  int statique;
  /* TBD : un pointeur sur l'arbre correspondant à 
           l'expression donnant la valeur par défaut ? */

  /* On utilise une liste chainée */
  var_t* suiv;
};

/* Structure représentant une file chainée de variables */
typedef struct
{
  var_t* tete;
  var_t* queue;
} liste_vars_t;

/* Enum permettant de caractériser le "type" d'une méthode ou d'un attribut */
typedef enum
{
  NORMALE,
  STATIQUE,
  REDEFINIE
} type_methode_t;

/* Structure représentant un paramètre de méthode (ou de constructeur puisque 
   le constructeur n'est qu'une méthode particulière d'après le prof */
struct param_t
{
  char* nom;
  char* nom_type;
  classe_t* type;
  /* TBD : un pointeur sur l'arbre correspondant à 
           l'expression donnant la valeur par défaut ? */

  /* On utilise une liste chainée */
  param_t* suiv;
};

/* Structure représentant une file chainée de paramètres */
typedef struct
{
  param_t* tete;
  param_t* queue;
} liste_params_t;

/* Structure représentant une méthode */
struct methode_t
{
  char* nom;
  type_methode_t type_methode;
  liste_params_t params;
  arbre_t* bloc; /* L'arbre syntaxique du bloc */
  char* nom_type_retour;
  classe_t* type_retour;

  /* On utilise une liste chainée */
  methode_t* suiv;
};

/* Structure représentant une file chainée de méthodes */
typedef struct
{
  methode_t* tete;
  methode_t* queue;
} liste_methodes_t;

/* Structure représentant une classe */
struct classe_t
{
  char* nom;
  char* nom_classe_mere;
  classe_t* classe_mere;
  liste_vars_t attributs;
  liste_methodes_t methodes; /* le constructeur n'est rien de plus qu'une méthode particulière */

  /* On utilise une liste chainée */
  classe_t* suiv;
};

typedef struct
{
  classe_t* tete;
  classe_t* queue;
} liste_classes_t;

typedef struct
{
  liste_vars_t variables;
  liste_methodes_t methodes;
} corps_t;

/* Déclaration des fonctions permettant de manipuler les structures */

var_t* nouvelle_variable(char* nom, char* type, int constante, int statique);
liste_vars_t ajouter_variable(liste_vars_t liste_vars, var_t* var);
var_t* chercher_variable(liste_vars_t liste_vars, char* nom);
liste_vars_t nouvelle_liste_variables(var_t* var);
void liberer_liste_variables(liste_vars_t liste_variables);

param_t* nouveau_param(char* nom, char* type /* TODO expression par défaut */);
liste_params_t ajouter_param(liste_params_t liste_params, param_t* param);
param_t* chercher_param(liste_params_t liste_params, char* nom);
liste_params_t nouvelle_liste_params(param_t* param);
void liberer_liste_params(liste_params_t liste_params);

methode_t* nouvelle_methode(char* nom, type_methode_t type_methode, liste_params_t params, arbre_t* bloc, char* type_retour);
liste_methodes_t ajouter_methode(liste_methodes_t liste_methodes, methode_t* methode);
void ajouter_methode_tete(liste_methodes_t* liste_methodes, methode_t* methode);
methode_t* chercher_methode(liste_methodes_t liste_methodes, char* nom);
liste_methodes_t nouvelle_liste_methodes(methode_t* methode);
void liberer_liste_methodes(liste_methodes_t liste_methodes);

classe_t* nouvelle_classe(char* nom, char* classe_mere, liste_params_t params_constructeur, liste_vars_t attributs, liste_methodes_t methodes);
liste_classes_t ajouter_classe(liste_classes_t liste_classes, classe_t* classe);
classe_t* chercher_classe(liste_classes_t liste_classes, char* nom);
liste_classes_t nouvelle_liste_classes(classe_t* var);
liste_classes_t nouvelle_liste_classes_preinitialisee();
void liberer_liste_classes(liste_classes_t liste_classes);

corps_t nouveau_corps(liste_vars_t variables, liste_methodes_t methodes);

#endif
