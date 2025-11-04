#include "file_items.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

ItemArr new_item_arr() {
  ItemArr arr = {
      .items = malloc(sizeof(Item)), .items_length = 0, .items_size = 1};
  if (arr.items == NULL) {
    fprintf(stderr, "Couldn't allocate memory for Item!\n");
    errno = 12;
    exit(ENOMEM);
  }
  return arr;
}

Item new_item(char *name, bool is_dir, __mode_t type, intmax_t size,
              char *timestamp) {
  Item item = {.name = name,
               .is_dir = is_dir,
               .type = type,
               .size = size,
               .timestamp = timestamp};
  return item;
}

ItemArr resize_arr(ItemArr items) {
  items.items_size += 1;
  items.items_size = items.items_size * 2;
  items.items = realloc(items.items, (sizeof(Item) * items.items_size));
  if (items.items == NULL) {
    fprintf(stderr, "Couldn't reallocate memory for Item array!\n");
    errno = 12;
    exit(ENOMEM);
  }
  return items;
}

ItemArr iterate_items(DIR *d, char *root_path, bool all_mode, bool list_mode,
                      bool recursive) {
  ItemArr items = new_item_arr();

  errno = 0;
  for (struct dirent *dir = readdir(d); dir != NULL; dir = readdir(d)) {
    if (errno) {
      fprintf(stderr, "Error while reading directory %s\n", root_path);
      exit(errno);
    }
    if (dir->d_name[0] == '.' && !all_mode) {
      continue;
    }
    if (items.items_length == items.items_size) {
      items = resize_arr(items);
    }

    int full_path_size = sizeof(char) * 1024 + 1;
    char *full_path = malloc(full_path_size);
    if (full_path == NULL) {
      fprintf(stderr, "Couldn't allocate memory for full_path char buffer!\n");
      errno = 12;
      exit(ENOMEM);
    }

    full_path[0] = 0;
    snprintf(full_path + strlen(full_path), full_path_size - strlen(full_path),
             "%s/%s", root_path, dir->d_name);

    struct stat path_stat;
    if (stat(full_path, &path_stat) != 0) {
      fprintf(stderr, "Couldn't access 'stat()' for path: %s. Error: %d\n",
              full_path, errno);
      continue;
    }

    const char *format = "%b %d %H:%M";
    size_t MAX_SIZE = 64;
    char *time_fmted = malloc(sizeof(char) * MAX_SIZE);
    if (time_fmted == NULL) {
      fprintf(stderr,
              "Couldn't allocate memory for time format char buffer!\n");
      errno = 12;
      exit(ENOMEM);
    }
    int ret_buffer =
        strftime(time_fmted, MAX_SIZE, format, localtime(&path_stat.st_mtime));
    if (ret_buffer > 0) {
      Item item =
          new_item(dir->d_name, S_ISDIR(path_stat.st_mode), path_stat.st_mode,
                   (intmax_t)path_stat.st_size, time_fmted);
      items.items[items.items_length] = item;
      items.items_length++;
    } else {
      fprintf(stderr,
              "Buffer exceeded for strftime() for timestamp of path: %s\n",
              full_path);
    }
    free(full_path);
  }

  qsort(items.items, items.items_length, sizeof(Item), comp);

  char *string = print_items(items, list_mode);
  puts(string);

  if (recursive) {
    for (int i = 0; i < items.items_length; i++) {
      Item item = items.items[i];
      if (!item.is_dir) {
        continue;
      }
      if (item.is_dir && strlen(item.name) == 1 && item.name[0] == '.') {
        continue;
      }
      if (item.is_dir && strlen(item.name) == 2 &&
          strncmp(item.name, "..", 2) == 0) {
        continue;
      }
      int full_path_size = sizeof(char) * 1024 + 1;
      char *full_path = malloc(full_path_size);
      if (full_path == NULL) {
        fprintf(stderr, "Couldn't allocate memory for full_path char buffer!");
        errno = 12;
        exit(ENOMEM);
      }
      full_path[0] = 0;
      snprintf(full_path + strlen(full_path),
               full_path_size - strlen(full_path), "%s/%s", root_path,
               item.name);

      DIR *d = opendir(full_path);
      if (d == NULL) {
        fprintf(stderr, "Could not access directory: %s %s", item.name,
                root_path);
        continue;
      }

      struct stat path_stat;
      if (stat(full_path, &path_stat) != 0) {
        fprintf(stderr,
                "Couldn't access 'stat()' for path: %s. Root path: %s; "
                "Error: %d\n",
                full_path, root_path, errno);
        continue;
      }

      printf("%s:\n", full_path);
      iterate_items(d, full_path, all_mode, list_mode, recursive);
      closedir(d);
      free(full_path);
    }
  }

  for (int i = 0; i < items.items_length; i++) {
    free(items.items[i].timestamp);
  }

  free(items.items);
  free(string);
  return items;
}

char *print_items(ItemArr items, bool list_mode) {
  int output_size = sizeof(char) * 1024 + 1;
  char *output_data = malloc(output_size);
  output_data[0] = 0;
  int largest_int = 0;
  for (int i = 0; i < items.items_length; i++) {
    Item item = items.items[i];
    if (items.items[i].size > largest_int) {
      largest_int = item.size;
    }
  }

  for (int i = 0; i < items.items_length; i++) {
    Item item = items.items[i];
    format_item_string(item, output_data, output_size, list_mode,
                       largest_size_length(largest_int));
  }

  return output_data;
}

int largest_size_length(int large_int) {
  int length = 0;
  int x = large_int;
  while (x > 0) {
    x = x / 10;
    length++;
  }
  return length;
}

char *format_item_string(Item item, char *output_data, int output_size,
                         bool list_mode, int longest_size) {
  if (list_mode) {
    snprintf(output_data + strlen(output_data),
             output_size - strlen(output_data), "%*jd %s ", longest_size,
             item.size, item.timestamp);
    if (item.is_dir) {
      snprintf(output_data + strlen(output_data),
               output_size - strlen(output_data), "\033[96m%s\033[0m\n",
               item.name);
    } else {
      if (is_executable_file(permissions_mask(item))) {
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), "\033[92m%s\033[0m\n",
                 item.name);
      } else {
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), "%s\n", item.name);
      }
    }

  } else {
    if (item.is_dir) {
      snprintf(output_data + strlen(output_data),
               output_size - strlen(output_data), "\033[96m%s\033[0m  ",
               item.name);
    } else {
      if (is_executable_file(permissions_mask(item))) {
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), "\033[92m%s\033[0m  ",
                 item.name);
      } else {
        snprintf(output_data + strlen(output_data),
                 output_size - strlen(output_data), "%s  ", item.name);
      }
    }
  }
  return output_data;
}

int comp(const void *a, const void *b) {
  Item *x = (Item *)a;
  Item *y = (Item *)b;

  return (strcmp(x->name, y->name));
}

unsigned short int permissions_mask(Item item) { return item.type & 0777; }

bool is_executable_file(unsigned short int permission_mask) {
  while (permission_mask > 0) {
    int mask = 0;
    mask = permission_mask % 10;
    if (mask == 7 || mask == 3 || mask == 1) {
      return true;
    }
    permission_mask = permission_mask / 10;
  }

  return false;
}
