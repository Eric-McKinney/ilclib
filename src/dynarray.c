#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ilc/dynarray.h>

struct dynarray {
    size_t length;  /* number of items in contents */
    size_t capacity;  /* allocated size (in number of items) */
    size_t item_size;
    void *contents;
};

DynArray *create_dynarray_sized(size_t item_size, size_t initial_capacity) {
    DynArray *arr = malloc(sizeof(DynArray));

    if (arr == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    arr->length = 0;
    arr->capacity = initial_capacity;
    arr->item_size = item_size;
    arr->contents = malloc(initial_capacity * item_size);

    if (arr->contents == NULL) {
        errno = ENOMEM;
        free(arr);
        return NULL;
    }

    return arr;
}

DynArray *create_dynarray(size_t item_size) {
    return create_dynarray_sized(item_size, 8);
}

void free_dynarray(DynArray *arr) {
    if (arr == NULL) {
        return;
    }

    free(arr->contents);
    free(arr);
}

int dynarray_append(DynArray *arr, const void *item) {
    if (arr == NULL || item == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    if (arr->length == arr->capacity) {
        void *p = realloc(arr->contents, arr->capacity * 2 * arr->item_size);
        if (p == NULL) {
            errno = ENOMEM;  /* should be set by realloc, but set again to be sure */
            return ENOMEM;
        }

        arr->contents = p;
        arr->capacity *= 2;
    }

    memcpy((char *)arr->contents + arr->length * arr->item_size, item, arr->item_size);
    arr->length++;

    return 0;
}

int dynarray_insert(DynArray *arr, const void *item, size_t index) {
    if (arr == NULL || item == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    if (index > arr->length) {
        errno = EINVAL;
        return EINVAL;
    }

    if (arr->length == arr->capacity) {
        void *p = realloc(arr->contents, arr->capacity * 2 * arr->item_size);
        if (p == NULL) {
            errno = ENOMEM;
            return ENOMEM;
        }

        arr->contents = p;
        arr->capacity *= 2;
    }

    /* shift contents down one and insert item */
    void *target = (char *)arr->contents + index * arr->item_size;
    void *rest = (char *)target + arr->item_size;
    size_t num_items = arr->length - index;
    memmove(rest, target, num_items * arr->item_size);

    memcpy(target, item, arr->item_size);
    arr->length++;

    return 0;
}

int dynarray_remove(DynArray *arr, const void *item, compare_fn equal, int remove_all) {
    if (arr == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    size_t i;
    for (i = 0; i < arr->length; i++) {
        void *curr_item = (char *)arr->contents + i * arr->item_size;
        if (equal(item, curr_item)) {
            dynarray_remove_at(arr, i);

            if (!remove_all) {
                return 0;
            }
        }
    }

    return 0;
}

int dynarray_remove_at(DynArray *arr, size_t index) {
    if (arr == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    if (index >= arr->length) {
        errno = EINVAL;
        return EINVAL;
    }

    /* shift everything down */
    void *item = (char *)arr->contents + index * arr->item_size;
    void *rest = (char *)item + arr->item_size;
    size_t num_rest = arr->length - index - 1;
    memcpy(item, rest, num_rest * arr->item_size);

    arr->length--;

    return 0;
}

int dynarray_replace(DynArray *arr, const void *to_replace, const void *new_item,
                     compare_fn equal, int replace_all) {
    if (arr == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    size_t i;
    for (i = 0; i < arr->length; i++) {
        void *curr = (char *)arr->contents + i * arr->item_size;
        if (equal(curr, item)) {
            memcpy(curr, item, arr->item_size);

            if (!replace_all) {
                return 0;
            }
        }
    }

    return 0;
}

int dynarray_replace_at(DynArray *arr, size_t index, const void *new_item) {
    if (arr == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    if (index >= arr->length) {
        errno = EINVAL;
        return EINVAL;
    }

    void *to_replace = arr->contents + index * arr->item_size;
    memcpy(to_replace, new_item, arr->item_size);

    return 0;
}

size_t dynarray_length(const DynArray *arr) {
    if (arr == NULL) {
        errno = EFAULT;
        return 0;
    }

    return arr->length;
}

void *dynarray_item_at(const DynArray *arr, size_t index) {
    if (arr == NULL) {
        errno = EFAULT;
        return NULL;
    }

    return (char *)arr->contents + index * arr->item_size;
}

void dynarray_print(const DynArray *arr, map_fn item_print) {
    if (arr == NULL) {
        printf("(null)");
        return;
    }

    printf("[");
    size_t i;
    for (i = 0; i < arr->length; i++) {
        if (i != 0) {
            printf(", ");
        }
        item_print((char *)arr->contents + i * arr->item_size);
    }
    printf("]");
}

void dynarray_debug_print(const DynArray *arr, map_fn item_debug_print) {
    if (arr == NULL) {
        printf("(null)");
        return;
    }

    printf("{length: %lu, contents: [", arr->length);

    size_t i;
    for (i = 0; i < arr->length; i++) {
        if (i != 0) {
            printf(", ");
        }

        item_debug_print((char *)arr->contents + i * arr->item_size);
    }
    printf("]}");
}

void dynarray_map(DynArray *arr, map_fn fn) {
    if (arr == NULL) {
        errno = EFAULT;
        return;
    }

    size_t i;
    for (i = 0; i < arr->length; i++) {
        void *item = (char *)arr->contents + i * arr->item_size;
        fn(item);
    }
}

static void *lfold(size_t length, size_t item_size, fold_fn fn, void *a, void *b) {
    if (length == 0) {
        return a;
    }

    return lfold(length - 1, item_size, fn, fn(a, b), (char *)b + item_size);
}

void *dynarray_lfold(const DynArray *arr, fold_fn fn, void *initial_value) {
    if (arr == NULL) {
        errno = EFAULT;
        return NULL;
    }

    return lfold(arr->length, arr->item_size, fn, initial_value, arr->contents);
}

static void *rfold(size_t length, size_t item_size, fold_fn fn, void *a, void *b) {
    if (length == 0) {
        return a;
    }

    return fn(b, rfold(length - 1, item_size, fn, a, (char *)b + item_size));
}

void *dynarray_rfold(const DynArray *arr, fold_fn fn, void *initial_value) {
    if (arr == NULL) {
        errno = EFAULT;
        return NULL;
    }

    return rfold(arr->length, arr->item_size, fn, initial_value, arr->contents);
}
