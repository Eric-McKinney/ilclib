#include <errno.h>
#include <ilc/test.h>
#include <ilc/dynarray.h>

TEST_SUITE("dynarray tests")

typedef struct {
    size_t item_size;
    int num_items;
    const void *items;
    int expected_errno;
} DArrTestInput;

static int append_test(const void *input) {
    DArrTestInput *args = (DArrTestInput *) input;
    int test_result = SUCCESS;
    errno = 0;

    DynArray *da = create_dynarray(args->item_size);
    if (da == NULL) {
        if (VERBOSE) {
            printf("    create_dynarray " COLOR_TEXT(RED, "returned NULL") "\n");
        }
        return FAILURE;
    }

    for (int i = 0; i < args->num_items; i++) {
        const void *item = (char *) args->items + i * args->item_size;
        int ret_val = dynarray_append(da, item);
        int errno_val = errno;
        size_t len = dynarray_length(da);

        int ret_val_ok = ret_val == args->expected_errno;
        int errno_ok = errno_val == args->expected_errno;
        int length_ok = (ret_val == 0) ? (len == (size_t) i + 1) : (len == (size_t) i);
        int item_ok = (ret_val == 0) ?
            memcmp(dynarray_item_at(da, i), item, args->item_size) == 0
            : 1;
        int append_ok = ret_val_ok
                     && errno_ok
                     && length_ok
                     && item_ok;

        if (VERBOSE && !append_ok) {
            printf("    dynarray_append(da, item #%d):\n", i);

            if (!ret_val_ok) {
                printf("        dynarray_append " COLOR_TEXT(RED, "returned %d") "\n",
                       ret_val);
            }
            if (!errno_ok) {
                printf("        dynarray_append " COLOR_TEXT(RED, "set errno to %d") "\n",
                       errno_val);
            }
            if (!length_ok) {
                printf("        dynarray_length " COLOR_TEXT(RED, "returned %lu")
                       " after append #%d\n", len, i + 1);
            }
            if (!item_ok) {
                printf("        dynarray_item_at "
                       COLOR_TEXT(RED, "differs from actual item") "\n");
            }
        }

        if (!append_ok) {
            test_result = FAILURE;
            break;
        }
    }

    free_dynarray(da);
    return test_result;
}

int main(int argc, char **argv) {
    int nums[] = {1, 2, 3, 5, 6, 7, 10, 9};
    size_t nums_len = sizeof(nums) / sizeof(int);

    char chars[] = {'g', 'e', 'f', 'd', 'v', 'k', 'a'};
    size_t chars_len = sizeof(chars) / sizeof(char);

    const char *strs[] = {"I <3 C", "a string", "words go here", "example"};
    size_t strs_len = sizeof(strs) / sizeof(char *);

    handle_cmdline_args(argc, argv);

    DArrTestInput inputs[] = {
        {sizeof(int),    nums_len,  nums,  0},
        {sizeof(char),   chars_len, chars, 0},
        {sizeof(char *), strs_len,  strs,  0},
        {sizeof(int),    1,         NULL,  EFAULT}
    };

    RUN_TESTS(
        {"append ints",    append_test, &inputs[0]},
        {"append chars",   append_test, &inputs[1]},
        {"append strs",    append_test, &inputs[2]},
        {"invalid append", append_test, &inputs[3]},
    )
}
