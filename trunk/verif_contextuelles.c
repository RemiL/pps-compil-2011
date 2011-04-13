#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "verif_contextuelles.h"

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
  
  /* Vérification du constructeur */
  sont_valides_params(decl, classe, classe->constructeur);
  est_valide_arbre_syntaxique(decl, decl_ajouter_attributs(NIL(decl_vars_t), classe->attributs), classe->constructeur->bloc);
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
    
    if (attribut->valeur_defaut != NIL(arbre_t) && attribut->type != est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                                                                 decl_ajouter_attributs(NIL(decl_vars_t), classe->attributs),
                                                                                                 attribut->valeur_defaut))
    {
      printf("Classe %s : Type incohérent pour la valeur par défaut de l'attribut %s.\n", classe->nom, attribut->nom);
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
  methode_t* methode_redefinie;
  classe_t* c;
  
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
    
    sont_valides_params(decl, classe, methode);
    
    methode_redefinie = NULL;
    c = classe->classe_mere;
    
    while (c != NULL && methode_redefinie == NULL)
    {
      methode_redefinie = chercher_methode(c->methodes, methode->nom);
      c = c->classe_mere;
    }
    
    if (methode_redefinie != NULL)
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
    
    if (methode->type_retour != est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                            decl_ajouter_params(decl_ajouter_attributs(NIL(decl_vars_t), classe->attributs), methode->params),
                                                            methode->bloc))
    {
      printf("Classe %s : Type de retour réel incohérent avec le type déclaré pour de la méthode %s.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    
    methode = methode->suiv;
  }
}

void sont_valides_params(liste_classes_t decl, classe_t* classe, methode_t* methode)
{
  param_t* param = methode->params.tete;
  
  while (param != NULL)
  {
    param->type = chercher_classe(decl, param->nom_type);
    
    if (param->type == NULL)
    {
      if (!strcmp(param->nom_type, classe->nom))  // Si un param de la classe A est de type A
        param->type = classe;
      else
      {
        printf("Classe %s : Type invalide pour le paramètre %s de la méthode %s : la classe %s est inconnue.\n", classe->nom, param->nom, methode->nom, param->nom_type);
        exit(EXIT_FAILURE);
      }
    }
    
    if (param->valeur_defaut != NIL(arbre_t) && param->type != est_valide_arbre_syntaxique(ajouter_classe(decl, classe),
                                                                                           decl_ajouter_attributs(NIL(decl_vars_t), classe->attributs),
                                                                                           param->valeur_defaut))
    {
      printf("Classe %s : Type incohérent pour la valeur par défaut du paramètre %s de la méthode %s.\n", classe->nom, param->nom, methode->nom);
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
  
  while (param != NULL && param_redef != NULL)
  {
    if (param->type != param_redef->type)
    {
      printf("Classe %s : la méthode %s n'est pas valide, ses paramètres n'ont pas les même types que ceux de la méthode qu'elle redéfinit.\n", classe->nom, methode->nom);
      exit(EXIT_FAILURE);
    }
    
    param = param->suiv;
    param_redef = param_redef->suiv;
  }
  
  if (param != NULL || param_redef != NULL)
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
  if (chercher_param(liste, parametre->nom) != NULL)
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
  if (chercher_methode(liste, methode->nom) != NULL)
  {
    printf("Méthode %s invalide : une méthode avec le même nom a déjà été déclarée.\n", methode->nom);
    exit(EXIT_FAILURE);
  }
}

void sont_valides_variables(liste_classes_t decl_classes, decl_vars_t* decl_vars, liste_vars_t variables)
{
  var_t* var = variables.tete;
  
  while (var != NULL)
  {
    var->type = chercher_classe(decl_classes, var->nom_type);
    
    if (var->type == NULL)
    {
      printf("Type invalide pour la variable %s : la classe %s est inconnue.\n", var->nom, var->nom_type);
      exit(EXIT_FAILURE);
    }
    
    if (var->valeur_defaut != NIL(arbre_t) && var->type != est_valide_arbre_syntaxique(decl_classes, decl_vars, var->valeur_defaut))
    {
      printf("Type incohérent pour la valeur par défaut de la variable %s.\n", var->nom);
      exit(EXIT_FAILURE);
    }
    
    var = var->suiv;
  }
}

/**
 * Vérifie que les arguments passés à une méthode sont du bon type et en nombres suffisants
 */
void sont_valides_arguments(liste_classes_t decl_classes, decl_vars_t* decl_vars, methode_t* methode, liste_args_t arguments)
{
  int num = 1;
  param_t* p = methode->params.tete;
  arg_t* a = arguments.tete;

  // Pour tous les arguments on vérifie qu'ils correspondent aux paramètres attendus
  while (a != NIL(arg_t) && p != NIL(param_t))
  {
    if(est_valide_arbre_syntaxique(decl_classes, decl_vars, a->expr) != p->type)
    {
      printf("L'argument n°%d passé à la méthode %s n'a pas le type attendu du paramètre %s.\n",  num, methode->nom, p->nom);
      exit(EXIT_FAILURE);
    }

    a = a->suiv;  // argument suivant
    p = p->suiv;  // paramètre suivant
    num++; 
  }
   
  if (p == NIL(param_t) && a != NIL(arg_t))
  {
    printf("Les arguments passés à la methode %s ne correspondent pas : il y a trop d'arguments.\n", methode->nom);
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
 * Vérifications contexuelles dans l'arbre syntaxique.
 * Retourne le type de l'arbre et remplit les informations supplémentaires
 * caractérisant un noeud disponibles uniquement quand l'environnement est
 * accessible.
 */
classe_t* est_valide_arbre_syntaxique(liste_classes_t decl_classes, decl_vars_t* decl_vars, arbre_t* arbre)
{
  classe_t* type = NIL(classe_t);
  var_t* var = NIL(var_t);
  param_t* param = NIL(param_t);
  
  if (arbre)
  {
    switch (arbre->op)
    {
      case Cste:
        arbre->info.type = chercher_classe(decl_classes, "Entier");
        return arbre->info.type;
        
      case Chaine:
        arbre->info.type = chercher_classe(decl_classes, "Chaine");
        return arbre->info.type;
        
      case Id:
        // Soit on a un identifiant de classe
        if ((type = chercher_classe(decl_classes, arbre->gauche.S)) != NIL(classe_t))
          arbre->info.type = type;
        // Soit on a un identifiant de variable
        else if ((arbre->type_var = decl_chercher_id(decl_vars, arbre->gauche.S, &var, &param)) != 0)
        {
          if (arbre->type_var == PARAM)
          {
            arbre->info.param = param;
            type = arbre->info.param->type;
          }
          else
          {
            arbre->info.var = var;
            type = arbre->info.var->type;
          }
        }
        
        if (type == NIL(classe_t))
        {
          printf("Identifiant inconnu : %s.\n", arbre->gauche.S);
          exit(EXIT_FAILURE);
        }
        return type;
        
      case Appel:
      case AppelStatique:
        type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A);
        arbre->info.methode = chercher_methode(type->methodes, arbre->droit.A->gauche.S);
        if (arbre->info.methode == NIL(methode_t))
        {
          printf("La classe %s ne possède pas de méthode %s.\n", type->nom, arbre->droit.A->gauche.S);
          exit(EXIT_FAILURE);
        }
        else if (arbre->op == AppelStatique && arbre->info.methode->type_methode != STATIQUE)
        {
          printf("La méthode %s de la classe %s n'est pas statique.\n", arbre->droit.A->gauche.S, type->nom);
          exit(EXIT_FAILURE);
        }
        sont_valides_arguments(decl_classes, decl_vars, arbre->info.methode, arbre->droit.A->droit.args);
        return arbre->info.methode->type_retour;
        
      case Bloc:
        sont_valides_variables(decl_classes, decl_vars, arbre->gauche.vars);
        // On ajoute à la liste courante de variables déclarées la liste des variables locales du bloc (en tête pour avoir la bonne notion de portée).
        arbre->info.type = est_valide_arbre_syntaxique(decl_classes, decl_ajouter_variables(decl_vars, arbre->gauche.vars), arbre->droit.A);
        return arbre->info.type;
        
      case ';': // Séparateur d'expression, on renvoie le type de l'expression de droite.
        est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A);
        arbre->info.type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A);
        return arbre->info.type;
      
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
        type = chercher_classe(decl_classes, "Entier");
        if (est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A) != type || est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A) != type)
        {
          printf("Les opérateurs arithmétiques et de comparaison sont réservés aux entiers.\n");
          exit(EXIT_FAILURE);
        }
        arbre->info.type = type;
        return arbre->info.type;
      
      case Selection:
      case SelectionStatique:
        type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A);
        if ((arbre->info.var = chercher_variable(type->attributs, arbre->droit.S)) == NIL(var_t))
        {
          printf("La classe %s ne possède pas d'attribut %s.\n", type->nom, arbre->droit.S);
          exit(EXIT_FAILURE);
        }
        else if (arbre->op == SelectionStatique && !arbre->info.var->statique)
        {
          printf("L'attribut %s de la classe %s n'est pas statique.\n", arbre->droit.S, type->nom);
          exit(EXIT_FAILURE);
        }
        return arbre->info.var->type;
      
      case New:
        type = chercher_classe(decl_classes, arbre->gauche.S);
        if(type == NIL(classe_t))
        {
          printf("La classe %s n'a pas été déclarée.\n", arbre->gauche.S);
          exit(EXIT_FAILURE);
        }
        arbre->info.methode = type->constructeur;
        // On vérifie les paramètres du constructeur
        sont_valides_arguments(decl_classes, decl_vars, arbre->info.methode, arbre->droit.args);
        return arbre->info.methode->type_retour;
      
      case Aff:
        arbre->info.type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A);
        if (!type_est_compatible(est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A), arbre->info.type))
        {
          printf("Affectation impossible, les types des opérandes diffèrent.\n");
          exit(EXIT_FAILURE);
        }
        return arbre->info.type;
      
      case ITE:
        if (chercher_classe(decl_classes, "Entier") != est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->gauche.A))
        {
          printf("La condition d'une expression conditionnelle doit être de type entier.\n");
          exit(EXIT_FAILURE);
        }
        arbre->info.type = est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A->gauche.A);
        if (arbre->info.type != est_valide_arbre_syntaxique(decl_classes, decl_vars, arbre->droit.A->droit.A))
        {
          printf("Les parties else et then d'une expression conditionnelles doivent être de type compatible.\n");
          exit(EXIT_FAILURE);
        }
        return arbre->info.type;
      
      default:
        printf("Erreur dans la vérification contextuelle : cas %c (%d) non pris en compte.\n", arbre->op, arbre->op);
        exit(EXIT_FAILURE);
    }
  }
  
  return type;
}
