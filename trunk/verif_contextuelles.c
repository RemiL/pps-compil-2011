/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "verif_contextuelles.h"

void est_valide_classe(liste_classes_t decl, classe_t* classe)
{  
  classe->classe_mere = chercher_classe(decl, classe->nom_classe_mere);
  
  if (chercher_classe(decl, classe->nom) != NIL(classe_t))
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
    else if (classe->classe_mere == NIL(classe_t))
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
  
  /* Vérification du constructeur */
  sont_valides_params(decl, classe, classe->constructeur);
  est_valide_arbre_syntaxique(ajouter_classe(decl, classe), decl_ajouter_params(decl_generer_depuis_classe(classe), classe->constructeur->params), classe->constructeur->bloc, classe, FAUX);
  
  /* Vérification de l'appel au constructeur de la classe mère */
  if (classe->classe_mere != NIL(classe_t))
    sont_valides_arguments(ajouter_classe(decl, classe), decl_ajouter_params(NIL(decl_vars_t), classe->constructeur->params),
                           classe->classe_mere->constructeur, classe->args_classe_mere, NIL(classe_t));
  
  sont_initialisees_attributs_constants(classe);
}

void sont_valides_attributs(liste_classes_t decl, classe_t* classe)
{
  var_t* attribut = classe->attributs.tete;
  char* nom_classe_masquage;
  
  while (attribut != NIL(var_t))
  {
    /* On ne peut pas déclarer des attributs utilisant les identificateurs spéciaux this et super */
    if (!strcmp(attribut->nom, "this") || !strcmp(attribut->nom, "super"))
    {
      printf("Classe %s : un attribut ne peut pas utiliser l'identificateur spécial %s.\n", classe->nom, attribut->nom);
      exit(EXIT_FAILURE);
    }
    
    /* On fait la vérification de la valeur par défaut immédiatement pour n'avoir le bon environnement */
    if (attribut->valeur_defaut != NIL(arbre_t) && !type_est_compatible(est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                                                                    decl_generer_depuis_classe(classe),
                                                                                                    attribut->valeur_defaut, classe, attribut->statique),
                                                                        attribut->type))
    {
      printf("Classe %s : Type incohérent pour la valeur par défaut de l'attribut %s (ligne %d).\n", classe->nom, attribut->nom, attribut->valeur_defaut->num_ligne);
      exit(EXIT_FAILURE);
    }
    
    attribut->type = chercher_classe(decl, attribut->nom_type);
    
    if (attribut->type == NIL(classe_t))
    {
      if (!strcmp(attribut->nom_type, classe->nom))  // si un attribut de la classe a est de type a
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

void sont_initialisees_attributs_constants(classe_t* classe)
{
  var_t* attribut = classe->attributs.tete;
  
  while (attribut != NIL(var_t))
  {
    if (attribut->constante == CONSTANTE_NON_INITIALISEE)
    {
      printf("Classe %s : l'attribut constant %s n'est pas initialisé avec une valeur par défaut ou dans le constructeur.\n", classe->nom, attribut->nom);
      exit(EXIT_FAILURE);
    }
    
    attribut = attribut->suiv;
  }
}

char* masque_attribut(classe_t* classe_mere, char* attribut)
{
  if (classe_mere == NIL(classe_t))
    return NULL;
  else if (chercher_variable(classe_mere->attributs, attribut) != NULL)
    return classe_mere->nom;
  else
    return masque_attribut(classe_mere->classe_mere, attribut);
}

void sont_valides_methodes(liste_classes_t decl, classe_t* classe)
{
  methode_t* methode = classe->methodes.tete;
  methode_t* methode_redefinie;
  classe_t* c;
  
  while (methode != NIL(methode_t))
  {
    /* On ne peut pas déclarer des méthodes avec pour nom les identificateurs spéciaux this et super */
    if (!strcmp(methode->nom, "this") || !strcmp(methode->nom, "super"))
    {
      printf("Classe %s : une méthode ne peut pas utiliser l'identificateur spécial %s.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    /* Bizarre mais demandé par le sujet : un attribut et une méthode ne peuvent avoir le même nom. */
    else if (chercher_variable(classe->attributs, methode->nom) != NIL(var_t))
    {
      printf("Classe %s : la méthode %s est invalide, elle porte le même nom qu'un attribut.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    
    methode->type_retour = chercher_classe(decl, methode->nom_type_retour);
    
    if (methode->nom_type_retour != NULL && methode->type_retour == NIL(classe_t))
    {
      if (!strcmp(methode->nom_type_retour, classe->nom))
        methode->type_retour = classe;
      else
      {
        printf("Classe %s : Type de retour invalide pour la méthode %s : la classe %s est inconnue.\n", classe->nom, methode->nom, methode->nom_type_retour);
        exit(EXIT_FAILURE);
      }
    }
    
    sont_valides_params(decl, classe, methode);
    
    methode_redefinie = NIL(methode_t);
    c = classe->classe_mere;
    
    while (c != NIL(classe_t) && methode_redefinie == NIL(methode_t))
    {
      methode_redefinie = chercher_methode(c->methodes, methode->nom);
      c = c->classe_mere;
    }
    
    if (methode_redefinie != NIL(methode_t))
    {
      if (methode->type_methode != REDEFINIE)
      {
        printf("Classe %s : la méthode %s devrait être déclarée comme redéfinie.\n", classe->nom, methode->nom);
        exit(EXIT_FAILURE);
      }
      else
        est_valide_redefinition(classe, methode, methode_redefinie);
    }
    else if (methode->type_methode == REDEFINIE)
    {
      printf("Classe %s : la méthode %s ne devrait pas être déclarée comme redéfinie.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    
    if (!type_est_compatible(est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                         decl_ajouter_params(decl_generer_depuis_classe(classe), methode->params),
                                                         methode->bloc, classe, methode->type_methode == STATIQUE),
                             methode->type_retour))
    {
      printf("Classe %s : Type de retour réel incohérent avec le type de retour déclaré par la méthode %s (ligne %d).\n", classe->nom, methode->nom, methode->bloc->num_ligne);
      exit(EXIT_FAILURE);
    }
    
    methode = methode->suiv;
  }
}

void sont_valides_params(liste_classes_t decl, classe_t* classe, methode_t* methode)
{
  param_t* param = methode->params.tete;
  
  while (param != NIL(param_t))
  {
    /* On ne peut pas déclarer des attributs utilisant les identificateurs spéciaux this et super */
    if (!strcmp(param->nom, "this") || !strcmp(param->nom, "super"))
    {
      printf("Classe %s : méthode %s : un paramètre ne peut pas utiliser l'identificateur spécial %s.\n", classe->nom, methode->nom, param->nom);
      exit(EXIT_FAILURE);
    }
    
    param->type = chercher_classe(decl, param->nom_type);
    
    if (param->type == NIL(classe_t))
    {
      if (!strcmp(param->nom_type, classe->nom))  // Si un param de la classe A est de type A
        param->type = classe;
      else
      {
        printf("Classe %s : Type invalide pour le paramètre %s de la méthode %s : la classe %s est inconnue.\n", classe->nom, param->nom, methode->nom, param->nom_type);
        exit(EXIT_FAILURE);
      }
    }
    
    if (param->valeur_defaut != NIL(arbre_t) && !type_est_compatible(est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                                                                 decl_generer_depuis_classe(classe),
                                                                                                 param->valeur_defaut, NIL(classe_t), FAUX),
                                                                     param->type))
    {
      printf("Classe %s : Type incohérent pour la valeur par défaut du paramètre %s de la méthode %s (ligne %d).\n", classe->nom, param->nom, methode->nom, param->valeur_defaut->num_ligne);
      exit(EXIT_FAILURE);
    }
    
    param = param->suiv;
  }
}

void est_valide_redefinition(classe_t* classe, methode_t* methode, methode_t* methode_redefinie)
{
  param_t* param = methode->params.tete;
  param_t* param_redef = methode_redefinie->params.tete;
  
  if (methode_redefinie->type_methode == STATIQUE)
  {
    printf("Classe %s : la méthode %s n'est pas valide, la redéfinition d'une méthode statique n'est pas autorisée.\n", classe->nom, methode->nom);
    exit(EXIT_FAILURE);
  }
  
  if (methode->type_retour != methode_redefinie->type_retour)
  {
    printf("Classe %s : la méthode %s n'est pas valide, elle n'a pas le même type de retour que la méthode qu'elle redéfinit.\n", classe->nom, methode->nom);
    exit(EXIT_FAILURE);
  }
  
  while (param != NIL(param_t) && param_redef != NIL(param_t))
  {
    if (param->type != param_redef->type)
    {
      printf("Classe %s : la méthode %s n'est pas valide, ses paramètres n'ont pas les même types que ceux de la méthode qu'elle redéfinit.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    
    param = param->suiv;
    param_redef = param_redef->suiv;
  }
  
  if (param != NIL(param_t) || param_redef != NIL(param_t))
  {
    printf("Classe %s : la méthode %s n'est pas valide, elle n'a pas le même nombre de paramètres que la méthode qu'elle redéfinit.\n", classe->nom, methode->nom);
    exit(EXIT_FAILURE);
  }
}

/**
 * Vérifie qu'un paramètre avec le même nom n'a pas été déclaré.
 */
void est_valide_param(liste_params_t liste, param_t* parametre)
{
  if (chercher_param(liste, parametre->nom) != NIL(param_t))
  {
    printf("Paramètre %s invalide : un paramètre avec le même nom a déjà été déclaré.\n", parametre->nom);
    exit(EXIT_FAILURE);
  }
}

/**
 * Vérifie qu'il n'y a pas 2 méthodes avec le même nom déclarées dans la même classe.
 */
void est_valide_methode(liste_methodes_t liste, methode_t* methode)
{  
  if (chercher_methode(liste, methode->nom) != NIL(methode_t))
  {
    printf("Méthode %s invalide : une méthode avec le même nom a déjà été déclarée.\n", methode->nom);
    exit(EXIT_FAILURE);
  }
}

void sont_valides_variables(liste_classes_t decl_classes, decl_vars_t* decl_vars, liste_vars_t variables, classe_t* type_this, int statique)
{
  var_t* var = variables.tete;
  
  while (var != NIL(var_t))
  {
    /* On ne peut pas déclarer des variables utilisant les identificateurs spéciaux this et super */
    if (!strcmp(var->nom, "this") || !strcmp(var->nom, "super"))
    {
      printf("Une variable ne peut pas utiliser l'identificateur spécial %s.\n", var->nom);
      exit(EXIT_FAILURE);
    }
    
    var->type = chercher_classe(decl_classes, var->nom_type);
    
    if (var->type == NIL(classe_t))
    {
      printf("Type invalide pour la variable %s : la classe %s est inconnue.\n", var->nom, var->nom_type);
      exit(EXIT_FAILURE);
    }
    
    if (var->valeur_defaut != NIL(arbre_t) && var->type != est_valide_arbre_syntaxique(decl_classes, decl_vars, var->valeur_defaut, type_this, statique))
    {
      printf("Type incohérent pour la valeur par défaut de la variable %s.\n", var->nom);
      exit(EXIT_FAILURE);
    }
    
    if (var->constante == CONSTANTE_NON_INITIALISEE)
    {
      printf("La variable constante %s n'est pas initialisée lors de sa déclaration.\n", var->nom);
      exit(EXIT_FAILURE);
    }
    
    var = var->suiv;
  }
}

/**
 * Vérifie que les arguments passés à une méthode sont du bon type et en nombres suffisants
 */
void sont_valides_arguments(liste_classes_t decl_classes, decl_vars_t* decl_vars, methode_t* methode, liste_args_t arguments, classe_t* type_this)
{
  int num = 1;
  param_t* p = methode->params.tete;
  arg_t* a = arguments.tete;

  // Pour tous les arguments on vérifie qu'ils correspondent aux paramètres attendus
  while (a != NIL(arg_t) && p != NIL(param_t))
  {
    if(!type_est_compatible(est_valide_arbre_syntaxique(decl_classes, decl_vars, a->expr, type_this, FAUX), p->type))
    {
      printf("L'argument n°%d passé à la méthode %s n'a pas le type attendu du paramètre %s (ligne %d).\n",  num, methode->nom, p->nom, a->expr->num_ligne);
      exit(EXIT_FAILURE);
    }

    a = a->suiv;  // argument suivant
    p = p->suiv;  // paramètre suivant
    num++; 
  }
   
  if (p == NIL(param_t) && a != NIL(arg_t))
  {
    printf("Les arguments passés à la methode %s ne correspondent pas : il y a trop d'arguments (ligne %d).\n", methode->nom, a->expr->num_ligne);
    exit(EXIT_FAILURE);
  }
   
  if (p != NIL(param_t) && a == NIL(arg_t) && p->valeur_defaut == NIL(arbre_t))
  {
     printf("Les arguments passés à la methode %s ne correspondent pas : il n'y a pas assez d'arguments.\n", methode->nom);
     exit(EXIT_FAILURE);
  }
}

/**
 * Vérifie si un objet de type1 peut être manipulé comme un objet de type2.
 * Revient à vérifier si la classe type2 a pour descendant la classe type1
 * ou si type1 et type2 sont égaux.
 */
int type_est_compatible(classe_t* type1, classe_t* type2)
{
  int est_compatible = (type1 == type2);
  
  while (type1 != NIL(classe_t) && !est_compatible)
  {
    est_compatible = (type1->classe_mere == type2);
    type1 = type1->classe_mere;
  }
  
  return est_compatible;
}

/**
 * Retourne l'éventuel ancètre commun de deux classes
 * ou NIL(classe_t) s'il n'en existe pas.
 */
classe_t* ancetre_commun(classe_t* type1, classe_t* type2)
{
  classe_t* t = type2;
  classe_t* ancetre = NIL(classe_t);
  
  while (type1 != NIL(classe_t) && ancetre == NIL(classe_t))
  {
    while (t != NIL(classe_t) && ancetre == NIL(classe_t))
    {
      if (type1 == t)
        ancetre = t;
      
      t = t->classe_mere;
    }
    
    t = type2;
    type1 = type1->classe_mere;
  }
  
  return ancetre;
}

/**
 * Vérifications contexuelles dans l'arbre syntaxique.
 * Retourne le type de l'arbre et remplit les informations supplémentaires
 * caractérisant un noeud disponibles uniquement quand l'environnement est
 * accessible.
 */
classe_t* est_valide_arbre_syntaxique(liste_classes_t decl_classes, decl_vars_t* decl_vars, arbre_t* arbre, classe_t* type_this, int statique)
{
  classe_t* type = NIL(classe_t);
  var_t* var = NIL(var_t);
  param_t* param = NIL(param_t);
  
  if (arbre)
  {
    switch (arbre->op)
    {
      case Cste:
        arbre->type = chercher_classe(decl_classes, "Entier");
        return arbre->type;
        
      case Chaine:
        arbre->type = chercher_classe(decl_classes, "Chaine");
        return arbre->type;
        
      case Id:
        // Soit on a un identifiant de classe
        if ((type = chercher_classe(decl_classes, arbre->gauche.S)) != NIL(classe_t))
          arbre->type = type;
        // Soit on a un identifiant de variable
        else if ((arbre->type_var = decl_chercher_id(decl_vars, arbre->gauche.S, &var, &param)) != 0)
        {
          if (arbre->type_var == PARAM)
          {
            arbre->info.param = param;
            arbre->type = arbre->info.param->type;
          }
          else if (statique && arbre->type_var == ATTRIBUT && !var->statique)
          {
            printf("Impossible de faire référence à l'attribut non statique %s dans un contexte statique (ligne : %d).\n", var->nom, arbre->num_ligne);
            exit(EXIT_FAILURE);
          }
          /* Pas top mais évite des problèmes avec les valeurs par défaut des paramètres */
          else if (arbre->type_var == ATTRIBUT && !var->statique && type_this == NIL(classe_t))
          {
            printf("Impossible de faire référence à l'attribut non statique %s dans la valeur par défaut d'un paramètre (ligne : %d).\n", var->nom, arbre->num_ligne);
            exit(EXIT_FAILURE);
          }
          else
          {
            arbre->info.var = var;
            arbre->type = arbre->info.var->type;
          }
        }
        else // identificateurs réservés
        {
          if (!strcmp("this", arbre->gauche.S))
          {
            if (type_this == NIL(classe_t))
            {
              printf("L'identificateur this ne peut pas être utilisé en dehors d'une classe (ligne : %d).\n", arbre->num_ligne);
              exit(EXIT_FAILURE);
            }
            else if (statique)
            {
              printf("L'identificateur this ne peut pas être utilisé dans un contexte statique (ligne : %d).\n", arbre->num_ligne);
              exit(EXIT_FAILURE);
            }
            
            arbre->type_var = THIS;
            arbre->type = type_this;
          }
          else if (!strcmp("super", arbre->gauche.S))
          {
            if (type_this == NIL(classe_t) || type_this->classe_mere == NIL(classe_t))
            {
              printf("L'identificateur super ne peut pas être utilisé en dehors d'une classe définie par héritage (ligne : %d).\n", arbre->num_ligne);
              exit(EXIT_FAILURE);
            }
            else if (statique)
            {
              printf("L'identificateur super ne peut pas être utilisé dans un contexte statique (ligne : %d).\n", arbre->num_ligne);
              exit(EXIT_FAILURE);
            }
            
            arbre->type_var = SUPER;
            arbre->type = type_this->classe_mere;
          }
        }
        
        if (arbre->type == NIL(classe_t))
        {
          printf("Identifiant inconnu : %s (ligne : %d).\n", arbre->gauche.S, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        return arbre->type;
        
      case Appel:
      case AppelStatique:
        type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique);
        arbre->info.methode = chercher_methode_arborescence_classe(type, arbre->droit.A->gauche.S);
        if (arbre->info.methode == NIL(methode_t))
        {
          printf("La classe %s ne possède pas de méthode %s (ligne : %d).\n", type->nom, arbre->droit.A->gauche.S, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (arbre->op == AppelStatique && arbre->info.methode->type_methode != STATIQUE)
        {
          printf("La méthode %s de la classe %s n'est pas statique (ligne : %d).\n", arbre->droit.A->gauche.S, type->nom, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (arbre->gauche.A->type_var == SUPER && chercher_methode(type_this->methodes, arbre->info.methode->nom) == NIL(methode_t))
        {
          printf("L'identificateur super ne peut être utilisé que comme destinataire d'une méthode redéfinie (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        sont_valides_arguments(decl_classes, decl_vars, arbre->info.methode, arbre->droit.A->droit.args, type_this);
        arbre->type = arbre->info.methode->type_retour;
        return arbre->type;
        
      case Bloc:
        // On ajoute à la liste courante de variables déclarées la liste des variables locales du bloc (en tête pour avoir la bonne notion de portée).
        decl_vars = decl_ajouter_variables(decl_vars, arbre->gauche.vars);
        sont_valides_variables(decl_classes, decl_vars, arbre->gauche.vars, type_this, statique);
        arbre->type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A, type_this, statique);
        return arbre->type;
        
      case ';': // Séparateur d'expression, on renvoie le type de l'expression de droite.
        est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique);
        arbre->type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A, type_this, statique);
        return arbre->type;
      
      // Opérateurs arithméritiques et de comparaison : il faut que les deux opérandes soient des entiers
      case '+':  
      case '-': 
      case '*':
      case '/':
      case EQ:
      case NEQ:
      case GT:
      case GE:
      case LT:
      case LE:
        arbre->type = chercher_classe(decl_classes, "Entier");
        if (est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique) != arbre->type
            || est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A, type_this, statique) != arbre->type)
        {
          printf("Les opérateurs arithmétiques et de comparaison sont réservés aux entiers (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        return arbre->type;
      
      case Selection:
      case SelectionStatique:
        type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique);
        if (arbre->gauche.A->type_var == SUPER)
        {
          printf("L'identificateur super ne peut être utilisé que pour les envois de message (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if ((arbre->info.var = chercher_attribut_arborescence_classe(type, arbre->droit.S)) == NIL(var_t))
        {
          printf("La classe %s ne possède pas d'attribut %s (ligne : %d).\n", type->nom, arbre->droit.S, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (type_this == NIL(classe_t) || !type_est_compatible(type_this, type)) // Les attributs sont protected
        {
          printf("L'accès à l'attribut %s est interdit en dehors de la classe %s ou de ses classes filles (ligne : %d).\n", arbre->droit.S, type->nom, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (arbre->op == SelectionStatique && !arbre->info.var->statique)
        {
          printf("L'attribut %s de la classe %s n'est pas statique (ligne : %d).\n", arbre->droit.S, type->nom, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        arbre->type_var = ATTRIBUT;
        arbre->type = arbre->info.var->type;
        return arbre->type;
      
      case New:
        type = chercher_classe(decl_classes, arbre->gauche.S);
        if (type == NIL(classe_t))
        {
          printf("La classe %s n'a pas été déclarée (ligne : %d).\n", arbre->gauche.S, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (!strcmp("Entier", type->nom) || !strcmp("Chaine", type->nom))
        {
          printf("La classe %s est un type prédéfini et ne peut pas être instanciée dynamiquement (ligne : %d).\n", type->nom, arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        
        arbre->info.methode = type->constructeur;
        // On vérifie les paramètres du constructeur
        sont_valides_arguments(decl_classes, decl_vars, arbre->info.methode, arbre->droit.args, type_this);
        arbre->type = arbre->info.methode->type_retour;
        return arbre->type;
      
      case Aff:
        arbre->type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique);
        if (!type_est_compatible(est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A, type_this, statique), arbre->type))
        {
          printf("Affectation impossible, les types des opérandes diffèrent (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        else if (arbre->gauche.A->info.var->constante == CONSTANTE_NON_INITIALISEE)
          arbre->gauche.A->info.var->constante = CONSTANTE_INITIALISEE;
        else if (arbre->gauche.A->info.var->constante == CONSTANTE_INITIALISEE)
        {
          printf("Affectation impossible, une constante ne peut pas être modifiée une fois initialisée (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        return arbre->type;
      
      case ITE:
        if (chercher_classe(decl_classes, "Entier") != est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A, type_this, statique))
        {
          printf("La condition d'une expression conditionnelle doit être de type entier (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        arbre->type = ancetre_commun(est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A->gauche.A, type_this, statique),
                                     est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A->droit.A, type_this, statique));
        if (arbre->type == NIL(classe_t))
        {
          printf("Les parties else et then d'une expression conditionnelles doivent être de type compatible (ligne : %d).\n", arbre->num_ligne);
          exit(EXIT_FAILURE);
        }
        return arbre->type;
      
      default:
        printf("Erreur dans la vérification contextuelle : cas %c (%d) non pris en compte (ligne : %d).\n", arbre->op, arbre->op, arbre->num_ligne);
        exit(EXIT_FAILURE);
    }
  }
  
  return type;
}
