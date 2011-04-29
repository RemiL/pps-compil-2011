#include <stdio.h>
#include <string.h>

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
    /* Les types prédéfinis sont traités différement */
    if (strcmp(classe->nom, "Entier") && strcmp(classe->nom, "Chaine"))
    {
      classe->decalage_table_sauts = decalage_bg++;
      
      decalage_bg = calculer_index_attributs(classe, decalage_bg);
      decalage_bg = calculer_index_methodes(classe, decalage_bg);
      
      /* Constructeur */
      calculer_index_params(classe->constructeur);
      calculer_index_variables_locales_arbre(classe->constructeur->bloc, 0);
    }
    
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

/**
 * Lance la génération du code correspondant aux classes et au programme
 * principal fournis. Le code généré sera écrit dans le fichier fourni.
 */
void generer_code(FILE* fichier, liste_classes_t classes, arbre_t* prog_principal)
{
  generer_code_classes_predefinies(fichier);
  
  generer_code_classes(fichier, classes);
}

/**
 * Génère le code correspondant aux types fournis.
 */
void generer_code_classes_predefinies(FILE* fichier)
{
  /* Type Entier */
  fprintf(fichier, "-- Début code classe prédéfinie : Entier\n");
  
  fprintf(fichier, "-- Début code Entier::imprimer()\n");
  fprintf(fichier, "Entier_imprimer: NOP\n");
  fprintf(fichier, "-- Fin code Entier::imprimer()\n");
  
  fprintf(fichier, "-- Fin code classe prédéfinie : Entier\n\n");
  
  /* Type Chaine */
  fprintf(fichier, "-- Début code classe prédéfinie : Chaine\n");
  
  fprintf(fichier, "-- Début code Chaine::imprimer()\n");
  fprintf(fichier, "Chaine_imprimer: NOP\n");
  fprintf(fichier, "-- Fin code Chaine::imprimer()\n");
  
  fprintf(fichier, "-- Fin code classe prédéfinie : Chaine\n\n");
}

/**
 * Génère le code correspondant aux classes fournis sauf les types
 * prédéfinies.
 */
void generer_code_classes(FILE* fichier, liste_classes_t classes)
{
  classe_t* classe = classes.tete;
  
  while (classe != NIL(classe_t))
  {
    /* Les types prédéfinis sont traités différement */
    if (strcmp(classe->nom, "Entier") && strcmp(classe->nom, "Chaine"))
    {
      fprintf(fichier, "-- Début code classe : %s\n", classe->nom);
      
      generer_code_constructeur(fichier, classe);
      generer_code_methodes(fichier, classe);
      
      fprintf(fichier, "-- Fin code classe : %s\n\n", classe->nom);
    }
    
    classe = classe->suiv;
  }
}

/**
 * Génère le code du constructeur de la classe fournie.
 * Il est séparé en deux étiquettes pour plus de simpliciter
 * en cas d'héritage :
 * - <nom_classe>_alloc pour la partie d'allocation de la
 *   mémoire.
 * - <nom_classe>_const pour la partie constructeur proprement
 *   dite.
 */
void generer_code_constructeur(FILE* fichier, classe_t* classe)
{
  /* Allocation de l'espace mémoire */
  fprintf(fichier, "-- Début code allocation classe %s\n", classe->nom);
    
  fprintf(fichier, "%s_alloc: ALLOC %d\n", classe->nom, classe->nb_attributs_non_statiques);
  fprintf(fichier, "\tPUSHA %s_const\n", classe->nom);
  fprintf(fichier, "\tCALL\n");
  
  fprintf(fichier, "\tRETURN\n");
  
  fprintf(fichier, "-- Fin code allocation classe %s\n", classe->nom);
  
  /* Constructeur en lui-même */
  fprintf(fichier, "-- Début code constructeur %s(", classe->nom);
  afficher_liste_params(fichier, classe->constructeur->params);
  fprintf(fichier, ")\n");
  
  fprintf(fichier, "%s_const: NOP\n", classe->nom);
  
  /* Appel aux constructeurs des classes parentes */
  generer_code_appel_constructeur_classes_parentes(fichier, classe);
  
  fprintf(fichier, "\tRETURN\n");
  
  fprintf(fichier, "-- Fin code constructeur %s()\n", classe->nom);
}

void generer_code_appel_constructeur_classes_parentes(FILE* fichier, classe_t* classe)
{
  if (classe->classe_mere != NIL(classe_t))
  {
    generer_code_appel_constructeur_classes_parentes(fichier, classe->classe_mere);
    fprintf(fichier, "-- Appel du constructeur de la classe %s()\n", classe->classe_mere->nom);
    fprintf(fichier, "\tPUSHA %s_const\n", classe->classe_mere->nom);
    fprintf(fichier, "\tCALL\n");
  }
}

void generer_code_methodes(FILE* fichier, classe_t* classe)
{
  methode_t* methode = classe->methodes.tete;
  
  while (methode != NIL(methode_t))
  {
    fprintf(fichier, "-- Début code %s::%s(", classe->nom, methode->nom);
    afficher_liste_params(fichier, methode->params);
    fprintf(fichier, ")\n");
    
    fprintf(fichier, "%s_%s: NOP\n", classe->nom, methode->nom);
    
    fprintf(fichier, "\tRETURN\n");
    
    fprintf(fichier, "-- Fin code %s::%s()\n", classe->nom, methode->nom);
    
    methode = methode->suiv;
  }
}
