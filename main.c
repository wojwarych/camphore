#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  char*  name;
  bool   is_dir;
} Item;


int main(int argc, char* argv[]) {
  if (argc == 1) {
    DIR* d = opendir(".");
    if (d == NULL) {
      return -1;
    } else {

      Item* items = malloc(sizeof(Item));
      if (items == NULL) {
        return -1;
      }

      int items_in = 0;
      int items_size = 1;
      struct dirent* dir = readdir(d);
      while (dir != NULL) {
        printf("%s\n", dir->d_name);

        if (items_in == items_size) {
          items_size += 1;
          items_size = (int)items_size * 2;
          items = realloc(items, (sizeof(Item) * items_size));
          if (items == NULL) {
            return -1;
          }
        }

        Item item = { .name = dir->d_name, .is_dir = false };
        items[items_in] = item;
        items_in++;
        dir = readdir(d);
      }
      free(items);
    }
  }

  return 0;
}
