/**
 * Projet compilation - Polytech' Paris-Sud 4ième année
 * Février - Mai 2011
 * 
 * Rémi Lacroix, Aliénor Latour, Nathanaël Masri, Loic Ramboanasolo
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

extern int yyparse();
extern void yylex_destroy();

extern int yylineno;

/* yyerror:  fonction importee par Bison et a fournir explicitement. Elle
 * est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *s)
{
  printf("Ligne %d: %s\n", yylineno, s);
}

FILE *fichier_code_genere = NIL(FILE);

/* appel:
 *   comp fichier-programme-source
 * ou
 *   comp fichier-programme-source fichier-à-générer
 * Dans le premier cas, le fichier à générer sera nommé automatiquement
 * à partir du nom du fichier source.
 */
int main(int argc, char **argv)
{
  int fi;
  int ret;
  char *nom_fichier_code_genere;
  
  if (argc < 2)
  { 
    fprintf(stderr, "Fichier programme manquant\n");
    exit(1);
  }
  if ((fi = open(argv[1], O_RDONLY)) == -1)
  {
    fprintf(stderr, "Je n'arrive pas a ouvrir: %s\n", argv[1]);
    exit(1);
  }

  /* redirige l'entree standard sur le fichier... */
  close(0); dup(fi); close(fi);

  if (argc == 2)
  {
    char* point = strrchr(argv[1], '.');
    int l = (point != NULL) ? point-argv[1] : strlen(argv[1]);
    nom_fichier_code_genere = NEW(strlen(argv[1])+5, char);
    
    strncpy(nom_fichier_code_genere, argv[1], l);
    strcpy(nom_fichier_code_genere+l, "_gen");
    if (point != NULL)
      strcpy(nom_fichier_code_genere+l+4, argv[1]+l);
    nom_fichier_code_genere[strlen(argv[1])+4] = '\0';
  }
  else
    nom_fichier_code_genere = strdup(argv[2]);
  
  if ((fichier_code_genere = fopen(nom_fichier_code_genere,  "w+")) == NULL)
  {
    perror(nom_fichier_code_genere);
    exit(1);
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
  ret = yyparse();
  /* Libération de la mémoire utilisé par flex */
  yylex_destroy();
  
  fclose(fichier_code_genere);
  free(nom_fichier_code_genere);
  
  if (ret)
  {
    fprintf(stderr, "Programme Incorrect\n");
    return 2;
  }
  else
    return 0;
}

