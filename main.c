#include "file_items.h"
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { LIST = 'l', ALL = 'a', RECURSIVE = 'R' } MODE;

int main(int argc, char *argv[]) {
  int opt;
  bool list_mode = false;
  bool all_mode = false;
  bool recursive = false;
  while ((opt = getopt(argc, argv, "lRa")) != -1) {
    switch (opt) {
    case 'l':
      list_mode = true;
      break;
    case 'a':
      all_mode = true;
      break;
    case 'R':
      recursive = true;
      break;
    default:
      printf("None valid option provided!\n");
      break;
    }
  }

  char *dirpath;
  if (argv[optind] == NULL) {
    dirpath = ".";
  } else {
    dirpath = argv[optind];
  }

  DIR *d = opendir(dirpath);
  ItemArr *items = iterate_items(d, all_mode);
  if (items != NULL) {

    char *string = print_items(*items, list_mode);
    puts(string);

    free(d);
    free(items->items);
    free(items);
    free(string);
  }
  return 0;
}
