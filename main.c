#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int get_int_len(int value) {
  int l = 1;
  while (value > 9) {
    l++;
    value /= 10;
  }
  return l;
}

typedef struct {
  char *name;
  bool is_dir;
} Item;

int comp(const void *a, const void *b) {
  Item *x = (Item *)a;
  Item *y = (Item *)b;

  return (strcmp(x->name, y->name));
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    DIR *d = opendir(".");
    if (d == NULL) {
      return -1;
    } else {

      Item *items = malloc(sizeof(Item));
      if (items == NULL) {
        return -1;
      }

      int items_in = 0;
      int items_size = 1;
      struct dirent *dir = readdir(d);
      while (dir != NULL) {
        if (items_in == items_size) {
          items_size += 1;
          items_size = (int)items_size * 2;
          items = realloc(items, (sizeof(Item) * items_size));
          if (items == NULL) {
            return -1;
          }
        }

        Item item = {.name = dir->d_name, .is_dir = false};
        items[items_in] = item;
        items_in++;
        dir = readdir(d);
      }

      qsort(items, items_in, sizeof(Item), comp);

      for (int i = 0; i < items_in; i++) {
        struct stat path_stat;
        stat(items[i].name, &path_stat);
        if S_ISDIR (path_stat.st_mode) {
          printf("\033[96m%s\t%jd\033[0m\n", items[i].name,
                 (intmax_t)path_stat.st_size);
        } else {
          printf("%s\t%jd\n", items[i].name, (intmax_t)path_stat.st_size);
        }
      }

      free(d);
      free(items);
    }
  }

  return 0;
}
