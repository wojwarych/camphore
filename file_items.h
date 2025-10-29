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
  char *timestamp;
} Item;

typedef struct {
  Item *items;
  int items_length;
  int items_size;
} ItemArr;

Item new_item(char *, bool, __mode_t, intmax_t, char *);
ItemArr *resize_arr(ItemArr *);
ItemArr *iterate_items(DIR *d, char *, bool);
char *print_items(ItemArr, bool);
char *format_item_string(Item, char *, int, bool, int);
int comp(const void *a, const void *b);
unsigned short int permissions_mask(Item);
bool is_executable_file(unsigned short int);
int largest_size_length(int);

#endif
