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



FILE *fd = nil(FILE);
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



/* parcours recursif de l'arbre representant une expression. Les valeurs
 * des identificateurs situes aux feuilles de l'arbre sont a rechercher
 * dans la liste lv
 *
 * ATTENTION: tous les cas ne sont pas traites dans l'arbre !!!
 */
int evalue(PARBRE arbre, PVAR lv) {
  int g, d;

  switch (arbre->op) {
  case Id:
    while (lv) {
      if (! strcmp(lv->nom, arbre->gauche.S)) return(lv->val);
      else lv = lv->suiv;
    }
    fprintf(stderr, "Variable non declaree: %s\n", arbre->gauche.S);
    exit(3);
  case Cste:
    return(arbre->gauche.E);
  case EQ:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g == d);
  case NEQ:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g != d);
  case GT:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g > d);
  case GE:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g >= d);
  case LT:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g < d);
  case LE:
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g <= d);
  case '+':
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g + d);
  case '*':
    g = evalue(arbre->gauche.A, lv);
    d = evalue(arbre->droit.A, lv);
    return (g * d);
  case ITE: /* if_then_else_ */
    /* le fils gauche represente la condition, le fils droit un arbre dont
     * les deux fils correspondent respectivement aux parties then et else.
     * Attention a n'evaluer que la partie necessaire !
     */
    if (evalue(arbre->gauche.A, lv)) 
       return evalue(arbre->droit.A->gauche.A, lv);
    else return evalue(arbre->droit.A->droit.A, lv);
  default: fprintf(stderr, "Cas non prevu dans Evaluation: %c\n", arbre->op);
    exit(4);
  }
}


/* juste pour faire joli: imprime toutes les paires (variable, valeur) */
void imprime(PVAR lv) {
  printf("Liste des couples variable/valeur:\n");
  while (lv) { 
    printf("%s = %d\n", lv->nom, lv->val);
    lv = lv->suiv;
  }
}



/* Construit un couple (variable, valeur) apres avoir verifie que la variable
 * n'apparait pas deja dans la liste des variables declarees (lv) puisque
 * c'etait une contrainte de l'enonce.
 * Ajoute le nouveau couple en tete de liste et renvoie le tout.
 */
PVAR MakeVar(char *nom, int val, PVAR lv) {
  PVAR l = lv;
  while(l) {
    if (! strcmp(nom, l->nom)) {
      fprintf(stderr, "Erreur! double declaration de variable: %s\n", nom);
      exit(3);
    }
    else l = l->suiv;
  }
  PVAR res = NEW(1, VAR);
  res->nom = nom; res->val = val; res->suiv = lv;
  return(res);
}


/* Constructeur de feuille de type ID pour l'arbre de syntaxe abstraite: 
 * on stocke l'id.
 */
PARBRE MakeId(char *var) {
  PARBRE res = NEW(1, ARBRE);
  res->op = Id; res->gauche.S = var; res->droit.A = nil(ARBRE);
  return(res);
}


/* Idem pour une feuille de type CSTE: on stocke la valeur */
PARBRE MakeCste(int val) {
  PARBRE res = NEW(1, ARBRE);
  res->op = Cste; res->gauche.E = val; res->droit.A = nil(ARBRE);
  return(res);
}


/* Constructeur pour un noeud interne: une etiquette pour l'operateur, et deux
 * sous-arbres.
 */
PARBRE MakeNoeud(char op, PARBRE g, PARBRE d) {
 PARBRE res = NEW(1, ARBRE);
 res->op = op; res->gauche.A = g; res->droit.A = d;
 return(res);
}


/* Constructeur pour le if_then_else qui oblige a coder un arbre ternaire avec
 * des arbres binaires. On cree un arbre racine d'etiquette ITE, de sous-arbre
 * gauche la condition et de sous-arbre droit un autre arbre d'etiquette
 * arbitraire (NOP) et dont les fils sont les arbres des parties then et else.
 */
PARBRE MakeITE(PARBRE pCond, PARBRE pThen, PARBRE pElse) {
  PARBRE p = MakeNoeud(NOP, pThen, pElse);
  return MakeNoeud(ITE, pCond, p); 
}


/* lecture dynamique d'une valeur, indiquee par le GET dans une expression.
 * Exemple: x:= 3 + GET; begin x end
 * Voir comment GET est traite dans evalue()
 */
int lire() {
  char buf[50]; int res;

  if (fd == nil(FILE)) {
    fprintf(stderr, "Fichier de donnees manquant\n");
    exit(1);
  }
  fgets(buf, 20, fd); 	       /* OK, y a mieux que fgets, je sais ! */
  sscanf(buf, "%d", &res);
  return(res);
}
