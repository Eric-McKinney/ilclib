#include <stdio.h>
#include <assert.h>
#include <ilc/string.h>
#include <ilc/test.h>

static int create_string_test_helper(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);

    if (cstr == NULL) {
        return str == NULL;
    }

    assert(str->chars != NULL);

    int len_ok = str->len == len;
    int chars_ok = check_mem_equal(str->chars, cstr, len);

    if (verbose) {
        printf("    given string: \"%s\", length: %lu\n", cstr, len);
        if (!len_ok) {
            printf("    " COLOR_TEXT(RED, "lengths differ") " (expected %lu, actual %lu)\n", len, str->len);
        }
        if (!chars_ok) {
            printf("    " COLOR_TEXT(RED, "chars differ") "\n");
        }
        if (len_ok && chars_ok) {
            printf("    test " COLOR_TEXT(GREEN, "passed") "\n");
        }
        printf("\n");
    }

    free_string(str);

    return len_ok && chars_ok;
}

static int create_string_test(int verbose) {
    int t1 = create_string_test_helper(NULL, 1, verbose);
    int t2 = create_string_test_helper("", 0, verbose);
    int t3 = create_string_test_helper("I <3 C", 6, verbose);
    int t4 = create_string_test_helper("123456789", 9, verbose);
    int t5 = create_string_test_helper("shorter", 5, verbose);

    return (t1 && t2 && t3 && t4 && t5) ? SUCCESS : FAILURE;
}

int main() {
    TestSuite *string_tests = create_test_suite("string tests");
    suite_add_test(string_tests, "create string", create_string_test);
    run_test_suite(string_tests, 0);
    free_test_suite(string_tests);

    return 0;
}
