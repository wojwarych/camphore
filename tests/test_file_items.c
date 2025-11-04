#include "../file_items.h"
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <stdio.h>

struct long_integers {
  int long_int;
  int length;
};

struct executable_file_permission_mask {
  unsigned short int bitmask;
  bool is_executable;
};

Test(file_items, new_item) {
  Item new_item = {"some name", false, 234, 1234567, "just some timestamp"};

  cr_assert(eq(new_item.name, "some name"));
  cr_assert(eq(new_item.is_dir, false));
  cr_assert(eq(new_item.type, 234));
  cr_assert(eq(new_item.size, 1234567));
  cr_assert(eq(new_item.timestamp, "just some timestamp"));
}

Test(file_items, resize_arr) {
  ItemArr arr = new_item_arr();
  int prev_size = arr.items_size;
  cr_assert(eq(arr.items_size, 1));
  cr_assert(eq(arr.items_length, 0));

  arr = resize_arr(arr);

  cr_assert(eq(arr.items_size, (prev_size + 1) * 2));
  cr_assert(eq(arr.items_length, 0));
}

ParameterizedTestParameters(file_items, largest_size_length) {
  static struct long_integers params[] = {
      {2137, 4}, {4, 1}, {982456, 6}, {22, 2}};
  size_t nb_params = sizeof(params) / sizeof(struct long_integers);
  return cr_make_param_array(struct long_integers, params, nb_params);
}

ParameterizedTest(struct long_integers *param, file_items,
                  largest_size_length) {
  cr_assert(eq(largest_size_length(param->long_int), param->length));
}

ParameterizedTestParameters(file_items, is_executable_file) {
  static struct executable_file_permission_mask params[] = {
      {1, true},   {10, true}, {100, true}, {101, true},  {110, true},
      {111, true}, {3, true},  {30, true},  {300, true},  {330, true},
      {303, true}, {0, false}, {20, false}, {200, false}, {640, false}};
  size_t nb_params =
      sizeof(params) / sizeof(struct executable_file_permission_mask);
  return cr_make_param_array(struct executable_file_permission_mask, params,
                             nb_params);
}

ParameterizedTest(struct executable_file_permission_mask *param, file_items,
                  is_executable_file) {
  cr_assert(eq(int, is_executable_file(param->bitmask), param->is_executable));
}
