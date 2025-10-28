#include "file_items.h"
#include <string.h>

int comp(const void *a, const void *b) {
  Item *x = (Item *)a;
  Item *y = (Item *)b;

  return (strcmp(x->name, y->name));
}

Item new_item(char *name, bool is_dir, __mode_t type, intmax_t size) {
  Item item = {.name = name, .is_dir = is_dir, .type = type, .size = size};
  return item;
}
