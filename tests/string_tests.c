#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <ilc/string.h>
#include <ilc/test.h>


static int check_null_property_upheld(const String *ret_val, int errno_val, size_t len, int verbose) {
    int ret_val_ok = ret_val == NULL;
    int errno_ok = errno_val == EFAULT;

    if (verbose) {
        const char *result = ret_val_ok && errno_ok ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("    properties %s (given NULL, length: %lu)\n", result, len);
        if (!ret_val_ok) {
            printf("        " COLOR_TEXT(RED, "return value != NULL") "\n");
        }
        if (!errno_ok) {
            printf("        " COLOR_TEXT(RED, "errno not set properly") " (errno = %d)\n",
                   errno_val);
        }
    }

    return ret_val_ok && errno_ok;
}

static int create_string_test_properties(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len, verbose);
    }

    int len_ok = str->len == len;
    int chars_ok = check_mem_equal(str->chars, cstr, len);

    if (verbose) {
        const char *result = len_ok && chars_ok ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("    properties %s (given string: \"%s\", length: %lu)\n", result, cstr, len);
        if (!len_ok) {
            printf("        " COLOR_TEXT(RED, "lengths differ")
                   " (expected %lu, actual %lu)\n", len, str->len);
        }
        if (!chars_ok) {
            printf("        " COLOR_TEXT(RED, "chars differ") " (actual chars: \"");
            string_print(str);
            printf("\")\n");
        }
    }

    free_string(str);

    return len_ok && chars_ok;
}

static int string_copy_test_properties(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);
    String *copy = string_copy(str);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len, verbose);
    }

    int len_same = str->len == copy->len;
    int chars_same = check_mem_equal(str->chars, copy->chars, str->len);
    int memory_location_different = str != copy && str->chars != copy->chars;

    if (verbose) {
        const char *result = len_same && chars_same && memory_location_different ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("   properties %s (given string: \"%s\", length: %lu)\n", result, cstr, len);
        if (!len_same) {
            printf(
                "        " COLOR_TEXT(RED, "copy's length is different")
                "(copy's length: %lu, orignal length: %lu)\n",
                copy->len, len
            );
        }
        if (!chars_same) {
            printf("        " COLOR_TEXT(RED, "copy's chars are different") " (copy's chars: \"");
            string_print(copy);
            printf("\")\n");
        }
        if (!memory_location_different) {
            printf("        " COLOR_TEXT(RED, "copy shares same memory address as original") "\n");
        }
    }

    free_string(str);
    free_string(copy);

    return len_same && chars_same && memory_location_different;
}

static int string_reverse_test_properties(const char *cstr, size_t len, int verbose) {
    String *str = create_string(cstr, len);
    String *reverse = string_reverse(str);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len, verbose);
    }

    int len_same = str->len == reverse->len;
    int chars_reversed = 1;

    size_t i;
    if (len_same) {
        for (i = 0; i < str->len; i++) {
            if (reverse->chars[i] != str->chars[str->len - i - 1]) {
                chars_reversed = 0;
                break;
            }
        }
    }

    if (verbose) {
        const char *result = len_same && chars_reversed ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("    properties %s (given string: \"%s\", length: %lu)\n", result, cstr, len);
        if (!len_same) {
            printf("        " COLOR_TEXT(RED, "reversed string length is different")
                   " (original length: %lu, reversed length: %lu)\n", str->len, reverse->len);
            printf("    note: didn't check if string was reversed properly due to length difference\n");
        }
        if (!chars_reversed) {
            printf("        " COLOR_TEXT(RED, "string was not reversed properly")
                   " (reverse's chars: \"");
            string_print(reverse);
            printf("\")\n");
        }
    }

    free_string(str);
    free_string(reverse);

    return len_same && chars_reversed;
}

static int string_reverse_test_examples(const char *cstr, const char *cstr_reverse, size_t len, int verbose) {
    /* not the point of this function (and NULL case already tested in properties) */
    assert(cstr != NULL && cstr_reverse != NULL);

    String *str = create_string(cstr, len);
    String *expected_reverse = create_string(cstr_reverse, len);
    String *reverse = string_reverse(str);

    /* test is meaningless if strings can't be created */
    assert(str != NULL && expected_reverse != NULL && reverse != NULL);

    int expectation_met = string_equal(expected_reverse, reverse);

    if (verbose) {
        const char *result = expectation_met ?
            COLOR_TEXT(GREEN, "ok") :
            COLOR_TEXT(RED, "not ok");
        printf("    example %s (given: \"%s\", expected: \"%s\", given len: %lu)\n",
               result, cstr, cstr_reverse, len);
        if (!expectation_met) {
            printf("        string_reverse(\"%s\") => \"", cstr);
            string_print(reverse);
            printf("\" != \"%s\"\n", cstr_reverse);
        }
    }

    free_string(str);
    free_string(expected_reverse);
    free_string(reverse);

    return expectation_met;
}

static int string_copy_test(int verbose) {
    int test_results[] = {
        string_copy_test_properties(NULL, 1, verbose),
        string_copy_test_properties("", 0, verbose),
        string_copy_test_properties("I <3 C", 6, verbose),
        string_copy_test_properties("should be simple", 16, verbose),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    int final_result = 1;
    int i;
    for (i = 0; i < num_tests; i++) {
        final_result = final_result && test_results[i];
    }

    return final_result ? SUCCESS : FAILURE;
}

static int create_string_test(int verbose) {
    int test_results[] = {
        create_string_test_properties(NULL, 1, verbose),
        create_string_test_properties("", 0, verbose),
        create_string_test_properties("I <3 C", 6, verbose),
        create_string_test_properties("123456789", 9, verbose),
        create_string_test_properties("shorter", 5, verbose),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    int final_result = 1;
    int i;
    for (i = 0; i < num_tests; i++) {
        final_result = final_result && test_results[i];
    }

    return final_result ? SUCCESS : FAILURE;
}

static int string_reverse_test(int verbose) {
    int test_results[] = {
        string_reverse_test_properties(NULL, 1, verbose),
        string_reverse_test_properties("", 0, verbose),
        string_reverse_test_properties("abc", 3, verbose),
        string_reverse_test_properties("another day", 11, verbose),
        string_reverse_test_properties("even", 4, verbose),
        string_reverse_test_properties("123456789123456789123456789", 27, verbose),

        string_reverse_test_examples("", "", 0, verbose),
        string_reverse_test_examples("abc", "cba", 3, verbose),
        string_reverse_test_examples("another day", "yad rehtona", 11, verbose),
        string_reverse_test_examples("even", "neve", 4, verbose),
        string_reverse_test_examples("123456789123456789123456789", "987654321987654321987654321", 27, verbose),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    int final_result = 1;
    int i;
    for (i = 0; i < num_tests; i++) {
        final_result = final_result && test_results[i];
    }

    return final_result ? SUCCESS : FAILURE;
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
