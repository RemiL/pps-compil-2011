#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "utils.h"

/* Propositions pour les structures permettant de stocker les informations récupérés via les attributs de la grammaire */

/* Déclaration des types */
typedef struct var_t var_t;
typedef struct param_t param_t;
typedef struct methode_t methode_t;
typedef struct arg_t arg_t;
typedef struct classe_t classe_t;
typedef struct decl_vars_t decl_vars_t;

/* Pour éviter les dépendances cylindriques */
#ifndef arbre_t
typedef struct arbre arbre_t;
#define arbre_t arbre_t
#endif

typedef enum
{
  NON_CONSTANTE,
  CONSTANTE_NON_INITIALISEE,
  CONSTANTE_INITIALISEE
} type_const_t;

/* Structure représentant une variable */
struct var_t
{
  char* nom;
  char* nom_type;
  classe_t* type;
  /* On peut avoir un attribut statique et constant dans le même temps,
     d'où l'utilisation de deux variables séparées plutôt qu'une seule. */
  type_const_t constante;
  int statique;
  /* Arbre syntaxique donnant l'expression de la valeur par défaut */
  arbre_t* valeur_defaut;
  /* Décalage à effectuer pour accéder à la variable */
  int index;

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
  /* Arbre syntaxique donnant l'expression de la valeur par défaut */
  arbre_t* valeur_defaut;
  /* Décalage à effectuer pour accéder au paramètre */
  int index;

  /* On utilise une liste chainée */
  param_t* suiv;
};

/* Structure représentant une file chainée de paramètres */
typedef struct
{
  param_t* tete;
  param_t* queue;
  int nb; /* Nombre de paramètres */
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
  /* Index dans la table des sauts */
  int index;

  /* On utilise une liste chainée */
  methode_t* suiv;
};

/* Structure représentant une file chainée de méthodes */
typedef struct
{
  methode_t* tete;
  methode_t* queue;
} liste_methodes_t;

/* Structure représentant un argument */
struct arg_t
{
  arbre_t* expr;
  arg_t* suiv;
};

/* Structure représentant une file chainée d'arguments */
typedef struct
{
  arg_t* tete;
  arg_t* queue;
} liste_args_t;

/* Structure représentant une classe */
struct classe_t
{
  char* nom;
  char* nom_classe_mere;
  classe_t* classe_mere;
  liste_args_t args_classe_mere; /* Les arguments de la classe mère en cas d'héritage. */
  liste_vars_t attributs;
  /* Nombre d'attributs non statiques (en incluant ceux des classes parentes) */
  int nb_attributs_non_statiques;
  liste_methodes_t methodes;
  /* Nombre de methodes disponibles (en incluant celles des classes parentes) */
  int nb_methodes;
  methode_t* constructeur; /* le constructeur n'est rien de plus qu'une méthode particulière */
  /* Décalage à effectuer à partir du fond de pile pour accéder
   * à la table des sauts de la classe. */
  int decalage_table_sauts;

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
  char* nom_classe_mere;
  liste_args_t args_classe_mere;
} heritage_t;

typedef enum
{
  ATTRIBUT = 1,
  VARIABLE,
  PARAM,
  THIS,
  SUPER
} type_decl_t;

struct decl_vars_t
{
  union
  {
    liste_vars_t vars;
    liste_params_t params;
  } decl;
  type_decl_t type;
  
  decl_vars_t* suiv;
};

typedef struct
{
  liste_vars_t variables;
  liste_methodes_t methodes;
} corps_t;

/* Déclaration des fonctions permettant de manipuler les structures */

var_t* nouvelle_variable(char* nom, char* type, int constante, int statique, arbre_t* valeur_defaut);
liste_vars_t ajouter_variable(liste_vars_t liste_vars, var_t* var);
var_t* chercher_variable(liste_vars_t liste_vars, char* nom);
liste_vars_t nouvelle_liste_variables(var_t* var);
liste_vars_t concatener_liste_variables(liste_vars_t l1, liste_vars_t l2);
void liberer_liste_variables(liste_vars_t liste_variables);

param_t* nouveau_param(char* nom, char* type, arbre_t* valeur_defaut);
liste_params_t ajouter_param(liste_params_t liste_params, param_t* param);
liste_params_t ajouter_param_en_tete(liste_params_t liste_params, param_t* param);
param_t* chercher_param(liste_params_t liste_params, char* nom);
liste_params_t nouvelle_liste_params(param_t* param);
void liberer_liste_params(liste_params_t liste_params);

methode_t* nouvelle_methode(char* nom, type_methode_t type_methode, liste_params_t params, arbre_t* bloc, char* type_retour);
liste_methodes_t ajouter_methode(liste_methodes_t liste_methodes, methode_t* methode);
void ajouter_methode_tete(liste_methodes_t* liste_methodes, methode_t* methode);
methode_t* chercher_methode(liste_methodes_t liste_methodes, char* nom);
liste_methodes_t nouvelle_liste_methodes(methode_t* methode);
void liberer_liste_methodes(liste_methodes_t liste_methodes);

arg_t* nouvel_argument(arbre_t* expr);
liste_args_t ajouter_argument(liste_args_t liste_arguments, arg_t* arg);
liste_args_t nouvelle_liste_arguments(arg_t* arg);
void liberer_liste_arguments(liste_args_t liste_arguments);

classe_t* nouvelle_classe(char* nom, char* classe_mere, liste_args_t args_classe_mere,
                          liste_params_t params_constructeur, arbre_t* bloc_constructeur,
                          liste_vars_t attributs, liste_methodes_t methodes);
liste_classes_t ajouter_classe(liste_classes_t liste_classes, classe_t* classe);
classe_t* chercher_classe(liste_classes_t liste_classes, char* nom);
var_t* chercher_attribut_arborescence_classe(classe_t* classe, char* nom);
methode_t* chercher_methode_arborescence_classe(classe_t* classe, char* nom);
liste_classes_t nouvelle_liste_classes(classe_t* classe);
liste_classes_t nouvelle_liste_classes_preinitialisee();
void liberer_liste_classes(liste_classes_t liste_classes);

heritage_t nouvel_heritage(char* nom_classe_mere, liste_args_t args_classe_mere);

decl_vars_t* decl_ajouter_attributs(decl_vars_t* decl, liste_vars_t attributs);
decl_vars_t* decl_ajouter_variables(decl_vars_t* decl, liste_vars_t variables);
decl_vars_t* decl_ajouter_params(decl_vars_t* decl, liste_params_t params);
decl_vars_t* decl_generer_depuis_classe(classe_t* classe);
type_decl_t decl_chercher_id(decl_vars_t* decl, char* id, var_t** var, param_t** param);

corps_t nouveau_corps(liste_vars_t variables, liste_methodes_t methodes);

#endif
