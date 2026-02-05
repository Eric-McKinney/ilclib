#ifndef DYNARRAY_H
#define DYNARRAY_H

typedef struct dynarray DynArray;
typedef void (*map_fn)(void *);
typedef void *(*fold_fn)(void *, void *);
typedef int (*compare_fn)(const void *, const void *);

DynArray *create_dynarray(size_t item_size);
DynArray *create_dynarray_sized(size_t item_size, size_t initial_capacity);
void free_dynarray(DynArray *arr);
int dynarray_append(DynArray *arr, const void *item);
int dynarray_insert(DynArray *arr, const void *item, size_t index);
int dynarray_remove(DynArray *arr, const void *item, compare_fn equal, int remove_all);
int dynarray_remove_at(DynArray *arr, size_t index);
int dynarray_replace(DynArray *arr, const void *to_replace, const void *new_item,
                     compare_fn equal, int replace_all);
int dynarray_replace_at(DynArray *arr, size_t index, const void *new_item);
size_t dynarray_length(const DynArray *arr);
void *dynarray_item_at(const DynArray *arr, size_t index);
void dynarray_print(const DynArray *arr, map_fn item_print);
void dynarray_debug_print(const DynArray *arr, map_fn item_debug_print);
void dynarray_map(DynArray *arr, map_fn fn);
void *dynarray_lfold(const DynArray *arr, fold_fn fn, void *initial_value);
void *dynarray_rfold(const DynArray *arr, fold_fn fn, void *initial_value);

// TODO: sort, filter, replace, replace_at

#endif
