#include <stdio.h>
#include <assert.h>
#include <ilc/string.h>
#include <ilc/test.h>

static int create_string_test_helper(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);

    if (cstr == NULL) {
        return str == NULL;
    }

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

static int string_copy_test_helper(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);
    String *copy = string_copy(str);

    if (cstr == NULL) {
        return copy == NULL;
    }

    int len_same = str->len == copy->len;
    int chars_same = check_mem_equal(str->chars, copy->chars, str->len);
    int memory_location_different = str != copy && str->chars != copy->chars;

    if (verbose) {
        printf("   given string: \"%s\", length: %lu\n", cstr, len);
        if (!len_same) {
            printf(
                "    " COLOR_TEXT(RED, "copy's length is different") "(copy's length: %lu, orignal length: %lu)\n",
                copy->len,
                len
            );
        }
        if (!chars_same) {
            printf("    " COLOR_TEXT(RED, "copy's chars are different") "\n");
        }
        if (!memory_location_different) {
            printf("    " COLOR_TEXT(RED, "copy shares same memory address as original") "\n");
        }
    }

    free_string(str);
    free_string(copy);

    return len_same && chars_same && memory_location_different;
}

static int string_copy_test(int verbose) {
    int t1 = string_copy_test_helper(NULL, 1, verbose);
    int t2 = string_copy_test_helper("", 0, verbose);
    int t3 = string_copy_test_helper("I <3 C", 6, verbose);
    int t4 = string_copy_test_helper("should be simple", 16, verbose);

    return (t1 && t2 && t3 && t4) ? SUCCESS : FAILURE;
}

int main() {
    TestSuite *string_tests = create_test_suite("string tests");
    suite_add_test(string_tests, "create string", create_string_test);
    suite_add_test(string_tests, "copy string", string_copy_test);
    run_test_suite(string_tests, 0);
    free_test_suite(string_tests);

    return 0;
}
