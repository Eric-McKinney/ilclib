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
    int test_result;

    DynArray *da = create_dynarray(args->item_size);
    if (da == NULL) {
        return FAILURE;
    }

    for (int i = 0; i < args->num_items; i++) {
        if (dynarray_append(da, (char *) args->items + i * args->item_size) != 0) {
            if (VERBOSE) {
                fprintf(stderr, "    append failed\n");
            }
            break;
        }
    }

    free_dynarray(da);

    test_result = SUCCESS;
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
        {sizeof(int),  nums_len,   nums,  0},
        {sizeof(char), chars_len,  chars, 0},
        {sizeof(char *), strs_len, strs,  0},
        {sizeof(int), nums_len, NULL, EFAULT}
    };

    RUN_TESTS(
        {"append ints",    append_test, &inputs[0]},
        {"append chars",   append_test, &inputs[1]},
        {"append strs",    append_test, &inputs[2]},
        {"invalid append", append_test, &inputs[3]},
    )
}
