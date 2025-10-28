#include "file_items.h"
#include <dirent.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum { LIST = 'l', ALL = 'a', RECURSIVE = 'R' } MODE;

typedef struct {
  Item *items;
  int items_length;
  int items_size;
} ItemArr;

ItemArr *resize_arr(ItemArr *);

char *print_items(ItemArr, bool);

ItemArr *iterate_items(DIR *d, bool all_mode) {
  if (d == NULL) {
    return NULL;
  } else {

    ItemArr *items = malloc(sizeof(ItemArr));
    Item *item = malloc(sizeof(Item));
    items->items = item;
    if (items == NULL) {
      return NULL;
    }
    items->items_size = 1;
    items->items_length = 0;

    struct dirent *dir = readdir(d);
    while (dir != NULL) {
      if ((strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) &&
          !all_mode) {
        dir = readdir(d);
        continue;
      }
      if (items->items_length == items->items_size) {
        items = resize_arr(items);
        if (items == NULL) {
          return NULL;
        }
      }

      struct stat path_stat;
      stat(dir->d_name, &path_stat);
      Item item = new_item(dir->d_name, S_ISDIR(path_stat.st_mode),
                           path_stat.st_mode, (intmax_t)path_stat.st_size);
      items->items[items->items_length] = item;
      items->items_length++;
      dir = readdir(d);
    }

    qsort(items->items, items->items_length, sizeof(Item), comp);

    return items;
  }
}

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

ItemArr *resize_arr(ItemArr *items) {
  items->items_size += 1;
  items->items_size = items->items_size * 2;
  items->items = realloc(items->items, (sizeof(Item) * items->items_size));
  return items;
}

char *print_items(ItemArr items, bool list_mode) {
  int output_size = sizeof(char) * 1024 + 1;
  char *output_data = malloc(output_size);
  output_data[0] = 0;

  for (int i = 0; i < items.items_length; i++) {
    Item item = items.items[i];
    if (item.is_dir && list_mode) {
      snprintf(output_data + strlen(output_data),
               output_size - strlen(output_data), "%jd\t\033[96m%s\033[0m\n",
               item.size, item.name);
    } else if (item.is_dir && !list_mode) {
      snprintf(output_data + strlen(output_data),
               output_size - strlen(output_data), "\033[96m%s\033[0m  ",
               item.name);
    } else if (!item.is_dir && list_mode) {
      if (is_executable_file(permissions_mask(item))) {
        const char fmt[] = "%jd\t\033[92m%s\033[0m\n";
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), fmt, item.size, item.name);
      } else {
        const char fmt[] = "%jd\t%s\n";
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), fmt, item.size, item.name);
      }
    } else {
      if (is_executable_file(permissions_mask(item))) {
        const char fmt[] = "\033[92m%s\033[0m ";
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), fmt, item.name);
      } else {
        const char fmt[] = "%s ";
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), fmt, item.name);
      }
    }
  }

  return output_data;
}
