#include <stdio.h>
#include <stdlib.h>
#include <ilc/dynarray.h>

static void item_print(void *item) {
    int i = *((int *) item);
    printf("%d", i);
}

static void double_item(void *item) {
    int *int_item = (int *)item;
    *int_item *= 2;
}

static int item_equal(const void *item1, const void *item2) {
    int i1 = *((int *) item1);
    int i2 = *((int *) item2);

    return i1 == i2;
}

static void *item_sum_left(void *psum, void *px) {
    int sum = *((int *) psum);
    int x = *((int *) px);

    int *result = malloc(sizeof(int));
    *result = sum + x;

    free(psum);

    return (void *) result;
}

static void *item_sum_right(void *px, void *psum) {
    return item_sum_left(psum, px);  /* addition is commutative :) */
}

static void *item_diff_left(void *pdiff, void *px) {
    int diff = *((int *) pdiff);
    int x = *((int *) px);

    int *result = malloc(sizeof(int));
    *result = diff - x;

    free(pdiff);

    return result;
}

static void *item_diff_right(void *px, void *pdiff) {
    int x = *((int *) px);
    int diff = *((int *) pdiff);

    int *result = malloc(sizeof(int));
    *result = x - diff;

    free(pdiff);

    return result;
}

void show_dynarray(const char *msg, const DynArray *arr) {
    printf("%s", msg);
    dynarray_debug_print(arr, item_print);
    printf("\n---\n");
}

int main() {
    DynArray *arr = create_dynarray(sizeof(int));

    show_dynarray("empty dynamic array: ", arr);
    
    int i;
    for (i = 1; i <= 10; i++) {
        dynarray_append(arr, (void *) &i);
    }

    show_dynarray("added numbers 1-10: ", arr);

    dynarray_map(arr, double_item);

    show_dynarray("used map with double_item: ", arr);

    dynarray_remove_at(arr, 0);
    show_dynarray("removed first item: ", arr);

    dynarray_remove_at(arr, 2);
    show_dynarray("removed third item: ", arr);

    int item_to_remove = 18;
    dynarray_remove(arr, (void *) &item_to_remove, item_equal, 0);
    show_dynarray("removed 18: ", arr);

    int item_to_insert = 8;
    dynarray_insert(arr, (void *) &item_to_insert, 3);
    dynarray_insert(arr, (void *) &item_to_insert, 5);
    dynarray_insert(arr, (void *) &item_to_insert, 7);
    show_dynarray("inserted 8 at indices 3, then 5, then 7: ", arr);

    dynarray_remove(arr, (void *) &item_to_insert, item_equal, 1);
    show_dynarray("removed all 8s: ", arr);

    int *initial = malloc(sizeof(int));
    *initial = 0;
    void *sum = dynarray_lfold(arr, item_sum_left, initial);
    printf("lfold (sum): %d\n", *((int *) sum));

    free(sum);

    initial = malloc(sizeof(int));
    *initial = 0;
    sum = dynarray_rfold(arr, item_sum_right, initial);
    printf("rfold (sum): %d\n", *((int *) sum));

    free(sum);

    initial = malloc(sizeof(int));
    *initial = 0;
    void *diff = dynarray_lfold(arr, item_diff_left, initial);
    printf("lfold (diff): %d\n", *((int *) diff));

    free(diff);

    initial = malloc(sizeof(int));
    *initial = 0;
    diff = dynarray_rfold(arr, item_diff_right, initial);
    printf("rfold (diff): %d\n", *((int *) diff));

    free(diff);

    free_dynarray(arr);
    return 0;
}
