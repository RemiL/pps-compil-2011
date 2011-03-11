/* Propositions pour les structures permettant de stocker les informations récupérés via les attributs de la grammaire */

/* Structure représentant une classe */
typedef struct
{
  char* nom;
  classe_t* classeMere;
  attribut_t* attributs;
  methode_t* methodes; /* le constructeur n'est rien de plus qu'une méthode particulière */

  /* On utilise une liste chainée */
  classe_t* suiv;
} classe_t;

/* Structure représentant un attribut */
typedef struct
{
  char* nom;
  classe_t* type;
  /* On peut avoir un attribut statique et constant dans le même temps,
     d'où l'utilisation de deux booléens plutôt q'un enum. */
  int constante;
  int statique;
  /* TBD : un pointeur sur l'arbre correspondant à 
           l'expression donnant la valeur par défaut ? */

  /* On utilise une liste chainée */
  attribut_t* suiv;
} attribut_t;

/* Enum permettant de caractériser le "type" d'une méthode ou d'un attribut */
typedef enum
{
  NORMALE,
  STATIQUE,
  SURCHARGEE
} type_methode_t;

/* Structure représentant une méthode */
typedef struct
{
  char* nom;
  type_methode_t type_methode;
  param_t* params;
  classe_t* type_retour;

  /* On utilise une liste chainée */
  methode_t* suiv;
} methode_t;

/* Structure représentant un paramètre de méthode (ou de constructeur puisque 
   le constructeur n'est qu'une méthode particulière d'après le prof */
typedef struct
{
  char* nom;
  classe_t* type;
  /* TBD : un pointeur sur l'arbre correspondant à 
           l'expression donnant la valeur par défaut ? */

  /* On utilise une liste chainée */
  param_t* suiv;
} param_t;

