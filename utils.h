#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define VRAI 1
#define FAUX 0

/* deux macros pratiques, utilisées dans les allocations */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define NIL(type) (type *) 0

#endif
