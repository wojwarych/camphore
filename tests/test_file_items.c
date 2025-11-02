#include "../file_items.h"
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <stdio.h>

struct long_integers {
  int long_int;
  int length;
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
