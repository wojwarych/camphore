#include "../file_items.h"
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <stdio.h>

int FOO = 0;
int *BAR = &FOO;
Item ITEM = {.name = "",
             .is_dir = false,
             .type = 0,
             .size = 4096,
             .timestamp = "Oct 28 13:44"};
Item *ITEM_P = &ITEM;
int *SIZE_P;
char *OUTPUT;
char *EXPECTED;

void setup_regular_files() {
  int size_p = 1024;
  SIZE_P = &size_p;
  OUTPUT = malloc(sizeof(char) * size_p + 1);
  OUTPUT[0] = 0;
  EXPECTED = malloc(sizeof(char) * size_p + 1);
  EXPECTED[0] = 0;
  return;
}

void teardown_regular_files() {
  free(OUTPUT);
  free(EXPECTED);
}

struct list_mode_struct {
  bool list_mode;
  bool is_dir;
  int type_bits;
};

ParameterizedTestParameters(format_item_string,
                            format_item_string_regular_files) {
  static struct list_mode_struct params[] = {{false, false, 0x81a4},
                                             {true, true, 0x41ed},
                                             {false, true, 0x41ed},
                                             {true, false, 0x81a4}};
  size_t nb_params = sizeof(params) / sizeof(struct list_mode_struct);
  return cr_make_param_array(struct list_mode_struct, params, nb_params);
}

ParameterizedTest(struct list_mode_struct *param, format_item_string,
                  format_item_string_regular_files, .init = setup_regular_files,
                  .fini = teardown_regular_files) {
  ITEM_P->name = "test.txt";
  ITEM_P->is_dir = param->is_dir;
  ITEM_P->type = param->type_bits;

  format_item_string(*ITEM_P, OUTPUT, *SIZE_P, param->list_mode, 4);

  if (!param->list_mode) {
    if (!ITEM_P->is_dir) {
      sprintf(EXPECTED, "%s  ", ITEM_P->name);
    } else {
      sprintf(EXPECTED, "\033[96m%s\033[0m  ", ITEM_P->name);
    }
  } else {
    if (!ITEM_P->is_dir) {
      sprintf(EXPECTED, "%*jd %s %s\n", 4, ITEM_P->size, ITEM_P->timestamp,
              ITEM_P->name);
    } else {
      sprintf(EXPECTED, "%*jd %s \033[96m%s\033[0m\n", 4, ITEM_P->size,
              ITEM_P->timestamp, ITEM_P->name);
    }
  }

  cr_assert(eq(str, OUTPUT, EXPECTED));
}
