#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
  char *name;
  bool is_dir;
  __mode_t type;
  intmax_t size;
} Item;

typedef struct {
  Item *items;
  int items_length;
  int items_size;
} ItemArr;

Item new_item(char *, bool, __mode_t, intmax_t);
ItemArr *resize_arr(ItemArr *);
ItemArr *iterate_items(DIR *d, bool all_mode);
char *print_items(ItemArr, bool);
int comp(const void *a, const void *b);
unsigned short int permissions_mask(Item);
bool is_executable_file(unsigned short int);

#endif
