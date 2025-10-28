#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
  char *name;
  bool is_dir;
  __mode_t type;
  intmax_t size;
} Item;

int comp(const void *a, const void *b);
Item new_item(char *, bool, __mode_t, intmax_t);
unsigned short int permissions_mask(Item);
bool is_executable_file(unsigned short int);

#endif
