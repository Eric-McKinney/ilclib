#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ilc/string.h>
#include <ilc/test.h>

static int create_string_test_properties(const char *cstr, size_t len, int verbose) {
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
    int t1 = create_string_test_properties(NULL, 1, verbose);
    int t2 = create_string_test_properties("", 0, verbose);
    int t3 = create_string_test_properties("I <3 C", 6, verbose);
    int t4 = create_string_test_properties("123456789", 9, verbose);
    int t5 = create_string_test_properties("shorter", 5, verbose);

    return (t1 && t2 && t3 && t4 && t5) ? SUCCESS : FAILURE;
}

static int string_copy_test_properties(const char *cstr, size_t len, int verbose) {
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
    int t1 = string_copy_test_properties(NULL, 1, verbose);
    int t2 = string_copy_test_properties("", 0, verbose);
    int t3 = string_copy_test_properties("I <3 C", 6, verbose);
    int t4 = string_copy_test_properties("should be simple", 16, verbose);

    return (t1 && t2 && t3 && t4) ? SUCCESS : FAILURE;
}

static int string_reverse_test_properties(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);
    String *reverse = string_reverse(str);

    if (cstr == NULL) {
        return reverse == NULL;
    }

    int len_same = str->len == reverse->len;
    int chars_reversed = 1;

    unsigned int i;
    if (len_same) {
        for (i = 0; i < str->len; i++) {
            if (reverse->chars[i] != str->chars[str->len - i - 1]) {
                chars_reversed = 0;
                break;
            }
        }
    }

    if (verbose) {
        printf("    given string: \"%s\", length: %lu\n", cstr, len);
        if (!len_same) {
            printf("    " COLOR_TEXT(RED, "reversed string length is different")
                   " (original length: %lu, reversed length: %lu)\n", str->len, reverse->len);
            printf("    note: didn't check if string was reversed properly due to length difference\n");
        }
        if (!chars_reversed) {
            printf("    " COLOR_TEXT(RED, "string was not reversed properly")
                   " (reverse[%u] is %c while original[%lu - %u - 1] is %c)\n", i, reverse->chars[i],
                   str->len, i, str->chars[str->len - i - 1]);
        }
    }

    free_string(str);
    free_string(reverse);

    return len_same && chars_reversed;
}

static int string_reverse_test_examples(const char *cstr, const char *cstr_reverse, size_t len, int verbose) {
    String *str = create_string(cstr, len);
    String *expected_reverse = create_string(cstr_reverse, len);

    /* not the point of this function (and NULL case already tested in properties) */
    assert(cstr != NULL && cstr_reverse != NULL);

    String *reverse = string_reverse(str);

    /* test is meaningless if strings can't be created */
    assert(str != NULL && expected_reverse != NULL && reverse != NULL);

    int expectation_met = string_equal(expected_reverse, reverse);

    if (verbose && !expectation_met) {
        printf("    " COLOR_TEXT(RED, "string_reverse(\"%s\") != \"%s\"") "\n", cstr, cstr_reverse);
    }

    free_string(str);
    free_string(expected_reverse);
    free_string(reverse);

    return expectation_met;
}

static int string_reverse_test(int verbose) {
    int t1 = string_reverse_test_properties(NULL, 1, verbose);
    int t2 = string_reverse_test_properties("", 0, verbose);
    int t3 = string_reverse_test_properties("abc", 3, verbose);

    int t4 = string_reverse_test_examples("abc", "cba", 3, verbose);
    int t5 = string_reverse_test_examples("another day", "yad rehtona", 11, verbose);

    return (t1 && t2 && t3 && t4 && t5) ? SUCCESS : FAILURE;
}

int main(int argc, char **argv) {
    int verbose = 0;

    if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[1], "--help") == 0) {
            printf(
                "Usage: %s [-v|--verbose|--help]\n"
                "    -v, --verbose\n"
                "        Show more details about each test\n"
                "    --help\n"
                "        Print this help message and exit\n",
                argv[0]
            );
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "%s: Invalid argument \"%s\"\n", argv[0], argv[1]);
            exit(EXIT_FAILURE);
        }
    } else if (argc > 2) {
        fprintf(stderr, "%s: Too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    TestSuite *string_tests = create_test_suite("string tests");
    suite_add_test(string_tests, "create string", create_string_test);
    suite_add_test(string_tests, "copy string", string_copy_test);
    suite_add_test(string_tests, "reverse string", string_reverse_test);
    run_test_suite(string_tests, verbose);
    free_test_suite(string_tests);

    return 0;
}
