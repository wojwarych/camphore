#include "file_items.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Item new_item(char *name, bool is_dir, __mode_t type, intmax_t size,
              char *timestamp) {
  Item item = {.name = name,
               .is_dir = is_dir,
               .type = type,
               .size = size,
               .timestamp = timestamp};
  return item;
}

ItemArr *resize_arr(ItemArr *items) {
  items->items_size += 1;
  items->items_size = items->items_size * 2;
  items->items = realloc(items->items, (sizeof(Item) * items->items_size));
  return items;
}

ItemArr *iterate_items(DIR *d, char *root_path, bool all_mode, bool list_mode,
                       bool recursive) {
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

      int full_path_size = sizeof(char) * 1024 + 1;
      char *full_path = malloc(full_path_size);
      full_path[0] = 0;
      snprintf(full_path + strlen(full_path),
               full_path_size - strlen(full_path), "%s/%s", root_path,
               dir->d_name);
      struct stat path_stat;
      stat(full_path, &path_stat);

      const char *format = "%b %d %H:%M";
      size_t MAX_SIZE = 64;
      char *time_fmted = malloc(sizeof(char) * MAX_SIZE);
      strftime(time_fmted, MAX_SIZE, format, localtime(&path_stat.st_mtime));

      Item item =
          new_item(dir->d_name, S_ISDIR(path_stat.st_mode), path_stat.st_mode,
                   (intmax_t)path_stat.st_size, time_fmted);
      items->items[items->items_length] = item;
      items->items_length++;
      if (recursive && item.is_dir && strcmp(item.name, "..") != 0) {
        DIR *d = opendir(full_path);
        iterate_items(d, full_path, all_mode, list_mode, recursive);
        free(full_path);
        closedir(d);
      }
      dir = readdir(d);
    }

    qsort(items->items, items->items_length, sizeof(Item), comp);

    if (items != NULL) {

      char *string = print_items(*items, list_mode);
      puts(string);

      for (int i = 0; i < items->items_length; i++) {
        free(items->items[i].timestamp);
      }
      free(items->items);
      free(items);
      free(string);
    }
    return items;
  }
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
    mask = permission_mask % 8;
    if (mask == 7 || mask == 3 || mask == 1) {
      return true;
    }
    permission_mask = permission_mask / 8;
  }

  return false;
}
