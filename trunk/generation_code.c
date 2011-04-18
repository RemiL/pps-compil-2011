#include <stdio.h>

#include "generation_code.h"

/**
 * Calcule l'index des différentes variables, attributs et méthodes.
 */
void calculer_index(liste_classes_t classes, arbre_t* prog_principal)
{
  /* Décalage courant par rapport au fond de pile */
  int decalage_bg = 0;
  
  classe_t* classe = classes.tete;
  
  while (classe != NIL(classe_t))
  {
    classe->decalage_table_sauts = decalage_bg++;
    
    decalage_bg = calculer_index_attributs(classe, decalage_bg);
    decalage_bg = calculer_index_methodes(classe, decalage_bg);
    
    /* Constructeur */
    calculer_index_params(classe->constructeur);
    calculer_index_variables_locales_arbre(classe->constructeur->bloc, 0);
    
    classe = classe->suiv;
  }
  
  calculer_index_variables_locales_arbre(prog_principal, decalage_bg);
}

/**
 * Calcule les index des attributs : pour les attributs non statiques,
 * il s'agit du décalage dans la zone mémoire allouée à l'objet dans
 * le tas, pour les attributs statiques, il s'agit du décalage par
 * rapport au fond de pile.
 * Retourne le nouveau décalage à partir du fond de pile à utiliser
 * pour la suite de la génération de code.
 */
int calculer_index_attributs(classe_t* classe, int decalage_bg)
{
  /* Pour les attributs non statiques, l'index commence à 1 puisque
   * l'index 0 correspond au renvoie vers la table des sauts. */
  int index = 1;
  
  var_t* attribut = classe->attributs.tete;
  
  /* S'il y a des classes parentes, on laisse la place à leurs attributs. */
  if (classe->classe_mere != NIL(classe_t))
    index += classe->classe_mere->nb_attributs_non_statiques;
    
  while (attribut != NIL(var_t))
  {
    attribut->index = (attribut->statique) ? decalage_bg++ : index++;
    
    attribut = attribut->suiv;
  }
  
  classe->nb_attributs_non_statiques = index - 1;
  
  return decalage_bg;
}

/**
 * Calcule l'index des méthodes dans la table des sauts.
 * Lance également le calcul des index pour leurs paramètres et pour
 * les variables locales utilisées dans le corps des méthodes.
 * Retourne le nouveau décalage à partir du fond de pile à utiliser
 * pour la suite de la génération de code.
 */
int calculer_index_methodes(classe_t* classe, int decalage_bg)
{
  methode_t* methode = classe->methodes.tete;
  methode_t* methode_redefinie;
  
  classe->nb_methodes = (classe->classe_mere != NIL(classe_t)) ? classe->classe_mere->nb_methodes : 0;
  
  while (methode != NIL(methode_t))
  {
    if (methode->type_methode != REDEFINIE)
      methode->index = classe->nb_methodes++;
    else
    {
      methode_redefinie = chercher_methode_arborescence_classe(classe->classe_mere, methode->nom);
      methode->index = methode_redefinie->index;
    }
    
    calculer_index_params(methode);
    calculer_index_variables_locales_arbre(methode->bloc, 0);
    
    methode = methode->suiv;
  }
  
  return decalage_bg;
}

/**
 * Calcule l'index des paramètres d'une méthode dans la pile
 * par rapport au frame pointer (fp).
 */
void calculer_index_params(methode_t* methode)
{
  /* Les paramètres se trouvent juste avant fp, le décalage est
   * donc négatif. On place les paramètres dans la pile dans 
   * l'ordre utilisé lors de l'appel de la méthode. */
  int index = - methode->params.nb;
  
  param_t* param = methode->params.tete;
  
  while (param != NIL(param_t))
  {
    param->index = index++;
    
    param = param->suiv;
  }
}

/**
 * Calcule l'index des variables locales utilisées dans un arbre syntaxique.
 * Le premier index utilisé est celui fourni en paramètre.
 * Retourne le prochain index à utiliser lors de la poursuite du calcul des index.
 */
int calculer_index_variables_locales_arbre(arbre_t* arbre, int index)
{
  if (arbre != NIL(arbre_t))
  {
    switch (arbre->op)
    {
      /* Les nouvelles variables locales sont ajoutées au niveau des blocs. */
      case Bloc:
        index = calculer_index_variables_locales(arbre->gauche.vars, index);
        return calculer_index_variables_locales_arbre(arbre->droit.A, index);
      
      /* On peut avoir des blocs dans les listes d'expression */
      case ';':
        index = calculer_index_variables_locales_arbre(arbre->gauche.A, index);
        return calculer_index_variables_locales_arbre(arbre->droit.A, index);
      
      /* et au niveau des expressions if-then-else. */
      case ITE:
        index = calculer_index_variables_locales_arbre(arbre->droit.A->gauche.A, index);
        return calculer_index_variables_locales_arbre(arbre->droit.A->droit.A, index);
      
      default:
        return index;
    }
  }
  else
    return index;
}

/**
 * Calcule l'index des variables locales de la liste fourni.
 * Le premier index utilisé est celui fourni en paramètre.
 * Retourne le prochain index à utiliser lors de la poursuite du calcul des index.
 */
int calculer_index_variables_locales(liste_vars_t variables, int index)
{
  var_t* var = variables.tete;
  
  while (var != NIL(var_t))
  {
    var->index = index++;
    
    var = var->suiv;
  }
  
  return index;
}
