#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <stdbool.h>

typedef struct {
  char *name;
  bool is_dir;
} Item;

int comp(const void *a, const void *b);

#endif
