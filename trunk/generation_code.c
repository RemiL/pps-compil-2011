#include <stdio.h>
#include <string.h>

#include "generation_code.h"

/**
 * Calcule l'index des différentes variables, attributs et méthodes.
 */
int calculer_index(liste_classes_t classes, arbre_t* prog_principal)
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
  
  return calculer_index_variables_locales_arbre(prog_principal, 0);
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
    methode->nb_variables_locales = calculer_index_variables_locales_arbre(methode->bloc, 0);
    
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
void generer_code(FILE* fichier, liste_classes_t classes, arbre_t* prog_principal, int nb_var_prog_principal)
{
  fprintf(fichier, "-- On ne veut pas exécuter le code des fonctions donc\n"
                   "-- on saute à l'initialisation des tables des sauts\n"
                   "JUMP init_tables_sauts\n");
  
  generer_code_classes_predefinies(fichier);
  
  generer_code_classes(fichier, classes);
  
  generer_code_tables_sauts(fichier, classes);
  
  generer_code_prog_principal(fichier, prog_principal, nb_var_prog_principal);
}

/**
 * Génère le code correspondant aux types fournis.
 * Le paramètre de la fonction est le destinataire
 * à savoir l'entier ou la chaine à imprimer, il
 * est dépilé lors de l'appel.
 */
void generer_code_classes_predefinies(FILE* fichier)
{
  /* Type Entier */
  fprintf(fichier, "-- Début code classe prédéfinie : Entier\n");
  
  fprintf(fichier, "-- Début code Entier::imprimer()\n"
                   "Entier_imprimer: WRITEI\n"
                   "\tRETURN\n"
                   "-- Fin code Entier::imprimer()\n");
  
  fprintf(fichier, "-- Fin code classe prédéfinie : Entier\n\n");
  
  /* Type Chaine */
  fprintf(fichier, "-- Début code classe prédéfinie : Chaine\n");
  
  fprintf(fichier, "-- Début code Chaine::imprimer()\n"
                   "Chaine_imprimer: WRITES\n"
                   "\tRETURN\n"
                   "-- Fin code Chaine::imprimer()\n");
  
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
    
  fprintf(fichier, "%s_alloc: ALLOC %d\n", classe->nom, classe->nb_attributs_non_statiques + 1);
  fprintf(fichier, "-- Initialisation du champ correspondant à la table des sauts\n"
                   "\tDUPN 1\n"
                   "\tPUSHG %d\n"
                   "\tSTORE 0\n", classe->decalage_table_sauts);
  fprintf(fichier, "-- Appel au constructeur pour la suite des initialisations\n"
                   "\tPUSHA %s_const\n", classe->nom);
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
    
    fprintf(fichier, "%s_%s: PUSHN %d\n", classe->nom, methode->nom, methode->nb_variables_locales);
    
    fprintf(fichier, "\tPOPN %d\n", methode->nb_variables_locales);
    fprintf(fichier, "\tRETURN\n");
    
    fprintf(fichier, "-- Fin code %s::%s()\n", classe->nom, methode->nom);
    
    methode = methode->suiv;
  }
}

void generer_code_tables_sauts(FILE* fichier, liste_classes_t classes)
{
  classe_t* classe = classes.tete;
  
  fprintf(fichier, "-- Début initialisation des tables des sauts\n");
  fprintf(fichier, "init_tables_sauts: NOP\n");
  
  while (classe != NIL(classe_t))
  {
    generer_code_table_sauts(fichier, classe);
    
    classe = classe->suiv;
  }
  
  fprintf(fichier, "-- Fin initialisation des tables des sauts\n\n");
  fprintf(fichier, "JUMP prog_principal\n");
}

void generer_code_table_sauts(FILE* fichier, classe_t* classe)
{
  /* Tableaux de chaines de caractères initialement
   * toutes NULL.
   * Indiquent le nom de la classe et de la méthode
   * d'index considéré. */
  char** nom_classe = NEW(classe->nb_methodes, char*);
  char** nom_methode = NEW(classe->nb_methodes, char*);
  classe_t* c;
  methode_t* methode;
  int i;
  
  fprintf(fichier, "\n-- Début table des sauts de %s\n", classe->nom);
  
  fprintf(fichier, "\tALLOC %d\n", classe->nb_methodes);
  
  c = classe;
  while (c != NIL(classe_t))
  {
    methode = c->methodes.tete;
    
    while (methode != NIL(methode_t))
    {
      if (nom_classe[methode->index] == NULL)
      {
        nom_classe[methode->index] = c->nom;
        nom_methode[methode->index] = methode->nom;
      }
      
      methode = methode->suiv;
    }
    
    c = c->classe_mere;
  }
  
  for (i=0; i<classe->nb_methodes; i++)
  {
    fprintf(fichier, "\tDUPN 1\n");
    fprintf(fichier, "\tPUSHA %s_%s\n", nom_classe[i], nom_methode[i]);
    fprintf(fichier, "\tSTORE %d\n", i);
  }
  
  fprintf(fichier, "-- Fin table des sauts de %s\n", classe->nom);
}

void generer_code_prog_principal(FILE* fichier, arbre_t* prog_principal, int nb_var_prog_principal)
{
  fprintf(fichier, "\n-- Programme principal\n"
                   "prog_principal: START\n");
  
  fprintf(fichier, "-- Allocation de l'espace mémoire pour les variables locales\n"
                   "\tPUSHN %d\n", nb_var_prog_principal);
  
  fprintf(fichier, "-- Début code programme principal\n");
  generer_code_arbre(fichier, prog_principal);
  fprintf(fichier, "-- Fin code programme principal\n");
  
  fprintf(fichier, "-- Libération de l'espace mémoire des variables locales\n"
                   "\tPOPN %d\n", nb_var_prog_principal);
  
  fprintf(fichier, "STOP\n");
}

void generer_code_arbre(FILE* fichier, arbre_t* arbre)
{
  /* étiquettes pour le if-then-else */
  int eti_else, eti_fin;
  
  if (arbre != NIL(arbre_t))
  {
    switch (arbre->op)
    {
      case ';':
        generer_code_arbre(fichier, arbre->gauche.A);
      
      case Bloc:
        generer_code_arbre(fichier, arbre->droit.A);
        break;
      
      case Cste:
        fprintf(fichier, "\tPUSHI %d\n", arbre->gauche.E);
        break;
      
      case Chaine:
        fprintf(fichier, "\tPUSHS %s\n", arbre->gauche.S);
        break;
      
      case Id:
        generer_code_identifiant(fichier, arbre);
        break;
      
      case Appel:
      case AppelStatique:
        generer_code_appel(fichier, arbre);
        break;
      
      case '+':
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tADD\n");
        break;
      
      case '-':
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tSUB\n");
        break;
      
      case '*':
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tMUL\n");
        break;
      
      case '/':
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tDIV\n");
        break;
      
      case EQ:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tEQUAL\n");
        break;
      
      case NEQ:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tEQUAL\n");
        fprintf(fichier, "\tNOT\n");
        break;
      
      case GT:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tSUP\n");
        break;
      
      case GE:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tSUPEQ\n");
        break;
      
      case LT:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tINF\n");
        break;
      
      case LE:
        fprintf(fichier, "-- Code opérande gauche\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "-- Code opérande droite\n");
        generer_code_arbre(fichier, arbre->droit.A);
        fprintf(fichier, "\tINFEQ\n");
        break;
      
      case ITE:
        eti_else = generer_etiquette_ITE();
        eti_fin = generer_etiquette_ITE();
        fprintf(fichier, "-- Code condition\n");
        generer_code_arbre(fichier, arbre->gauche.A);
        fprintf(fichier, "\tJZ ite%d -- si faux, on saute à else\n", eti_else);
        fprintf(fichier, "-- Code then\n");
        generer_code_arbre(fichier, arbre->droit.A->gauche.A);
        fprintf(fichier, "\tJUMP ite%d -- on saute le else\n", eti_fin);
        fprintf(fichier, "ite%d: NOP -- Code else\n", eti_else);
        generer_code_arbre(fichier, arbre->droit.A->droit.A);
        fprintf(fichier, "ite%d: NOP -- Fin du if-then-else\n", eti_fin);
    }
  }
}

void generer_code_identifiant(FILE* fichier, arbre_t* arbre)
{
  switch (arbre->type_var)
  {
    case VARIABLE:
      fprintf(fichier, "\tPUSHL %d -- variable locale %s\n", arbre->info.var->index, arbre->info.var->nom);
      break;
    
    default:
      break;
  }
}

void generer_code_appel(FILE* fichier, arbre_t* arbre)
{
  fprintf(fichier, "-- Destinataire appel\n");
  generer_code_arbre(fichier, arbre->gauche.A);
  fprintf(fichier, "-- Appel\n");
  /* Si le destinataire n'est pas une variable donc
   * soit une constante soit un nom de classe dans
   * le cas d'un appel statique. On peut s'intéresser
   * directement à la table des sauts correspond à
   * la classe du destinataire. */
  if (arbre->gauche.A->type_var == NON_VAR)
    fprintf(fichier, "\tPUSHG %d"
                     " -- adresse de la TS en pile\n", arbre->gauche.A->info.type->decalage_table_sauts);
  /* Sinon, l'appel doit prendre en compte le type
   * réel de l'objet et donc utiliser sa table des
   * sauts plutôt que celle de son type visible. */
  else
  {
    fprintf(fichier, "\tDUPN 1 -- duplique l'addresse de l'objet\n"
                     "\tLOAD 0 -- champ 0 = adresse de la TS\n");
  }
  fprintf(fichier, "\tLOAD %d -- index de la méthode\n", arbre->info.methode->index);
  fprintf(fichier, "\tCALL\n");
}

/*
Génère un nombre entier tel que s'il est concaténé à la
chaine "ite", le résultat est une étiquette unique.
*/
int generer_etiquette_ITE()
{
  /* On déclare la variable static de telle sorte à éviter
   * une variable globale ou un passage par paramètre. */
  static int prochaine_etiquette = 0;
  /* On incrémente pour le prochain appel. */
  return prochaine_etiquette++;
}
