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

unsigned short int permissions_mask(Item item) { return item.type & 0777; }

bool is_executable_file(unsigned short int permission_mask) {
  while (permission_mask > 0) {
    int mask = 0;
    mask = permission_mask % 8;
    if (mask == 7 || mask == 3 || mask == 1) {
      return true;
    }
    permission_mask = permission_mask / 8;
  }

  return false;
}
