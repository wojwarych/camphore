#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct {
  char *name;
  bool is_dir;
} Item;

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
        struct stat path_stat;
        stat(dir->d_name, &path_stat);
        if S_ISDIR (path_stat.st_mode) {
          printf("\033[96m%s\t%jd\033[0m\n", dir->d_name,
                 (intmax_t)path_stat.st_size);
        } else {
          printf("%s\t%jd\n", dir->d_name, (intmax_t)path_stat.st_size);
        }

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
      free(d);
      free(items);
    }
  }

  return 0;
}
