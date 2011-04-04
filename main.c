#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "main.h"


extern int yyparse();

/* Principes de l'interprete:
 * 1. Pendant le traitement des declarations de la forme x := E, on construit
 * une liste de paires (variable, valeur) au fur et a mesure qu'on rencontre
 * une nouvelle declaration. Pour cela, lors de l'analyse syntaxique de E
 * on construit un arbre de syntaxe abstraite (en fait un arbre
 * operateur-operandes) pour E. La construction de l'arbre se fait dans les
 * actions semantiques (partie bison) par appels aux fonctions Make...
 * ci-apres en passant en parametre l'etiquette a utiliser et les arbres
 * representant les operandes.
 * Quand E est reconnue (i.e. quand on effectue la reduction pour le E le plus
 * externe de la construction courante), on dispose de la racine de l'arbre,
 * qu'on passe a la fonction Evalue, accompagnee de la liste courante des paires
 * (variable, valeur). On obtient une valeur en resultat, qu'on associe a la
 * variable x pour faire une nouvelle paire.
 * Une fois evalue l'arbre representant E, on n'en n'a plus besoin
 * et on l'oublie (formellement, on aurait du le desallouer !).
 * On ne cree donc jamais l'arbre syntaxique correspondant a tout le programme, 
 * mais seulement l'arbre d'une expression arithmetique.
 * Le 'contexte' de l'evaluation est resume par la liste des paires
 * variable/valeurs
 *
 * 2. Le traitement de l'expression finale est identique: construction de
 * l'arbre de l'expression puis evaluation dans le contexte des declarations
 * passees
 *
 * La fonction Evalue n'est qu'un grand switch qui teste l'etiquette du
 * noeud courant de l'arbre et fait l'operation demandee, en s'appelant
 * recursivement pour evaluer les operandes, si besoin.
 *
 * Remarque: les appels initiaux aux fonctions de ce fichier apparaissent donc
 * dans les actions semantiques associees aux regles de grammaire.
 */
 
extern int yylineno;

/* yyerror:  fonction importee par Bison et a fournir explicitement. Elle
 * est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *s) {
  printf("Ligne %d: %s\n", yylineno, s);
}



FILE *fd = NIL(FILE);
char *fname;

/* appel:
 *   tp2 fichier-programme
 * ou
 *   tp2 fichier-programme fichier-donnees
 * si le programme contient des GET qui seront lus dynamiquement dans le
 * fichier de donnees (pas de lecture au clavier)
 */
int main(int argc, char **argv) {
  int fi;

  if (argc < 2) { 
    fprintf(stderr, "Fichier programme manquant\n");
    exit(1);
  }
  if ((fi = open(argv[1], O_RDONLY)) == -1) {
    fprintf(stderr, "Je n'arrive pas a ouvrir: %s\n", argv[1]);
    exit(1);
  }

  /* redirige l'entree standard sur le fichier... */
  close(0); dup(fi); close(fi);

  if (argc >= 3) { /* fichier dans lequel lire les valeurs pour GET */
    fname = argv[2];
    if ((fd = fopen(fname,  "r")) == NULL)
      { perror(fname); exit(1); }
  }

  /* yyparse: appel à l'analyseur syntaxique. Lance l'analyse syntaxique de
   * tout le source, en appelant yylex au fur et a mesure. Execute les
   * actions semantiques en parallele avec les mouvements de type REDUCE de
   * l'analyseur syntaxique.
   * yyparse renvoie 0 si le source est syntaxiquement correct, une valeur
   * differente de 0 en cas d'erreur lexicale ou syntaxique.
   * Comme l'interpretation globale est automatiquement lancee par les actions
   * associees aux mouvements REDUCE, une fois que yyparse a termine il n'y
   * a plus rien a faire.
   */
  if (yyparse()) {
    fprintf(stderr, "Programme Incorrect \n");  return 2; 
  } else return 0;
}

