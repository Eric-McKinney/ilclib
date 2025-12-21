#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <ilc/string.h>
#include <ilc/test.h>


int VERBOSE = 0;


static int check_null_property_upheld(const String *ret_val, int errno_val, size_t len) {
    int ret_val_ok = ret_val == NULL;
    int errno_ok = errno_val == EFAULT;

    if (VERBOSE) {
        const char *result = ret_val_ok && errno_ok ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("    null property %s (given NULL, length: %lu)\n", result, len);
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

static int create_string_test_properties(const char *cstr, size_t len) {
    String *str = create_string(cstr, len);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len);
    }

    int len_ok = str->len == len;
    int chars_ok = check_mem_equal(str->chars, cstr, len);

    if (VERBOSE) {
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

static int string_copy_test_properties(const char *cstr, size_t len) {
    String *str = create_string(cstr, len);
    String *copy = string_copy(str);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len);
    }

    int len_same = str->len == copy->len;
    int chars_same = check_mem_equal(str->chars, copy->chars, str->len);
    int memory_location_different = str != copy && str->chars != copy->chars;

    if (VERBOSE) {
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

static int string_reverse_test_properties(const char *cstr, size_t len) {
    String *str = create_string(cstr, len);
    String *reverse = string_reverse(str);

    if (cstr == NULL) {
        return check_null_property_upheld(str, errno, len);
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

    if (VERBOSE) {
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

static int string_reverse_test_examples(const char *cstr, const char *cstr_reverse, size_t len) {
    /* not the point of this function (and NULL case already tested in properties) */
    assert(cstr != NULL && cstr_reverse != NULL);

    String *str = create_string(cstr, len);
    String *expected_reverse = create_string(cstr_reverse, len);
    String *reverse = string_reverse(str);

    /* test is meaningless if strings can't be created */
    assert(str != NULL && expected_reverse != NULL && reverse != NULL);

    int expectation_met = string_equal(expected_reverse, reverse);

    if (VERBOSE) {
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

static int substring_invalid_range_prop(const String *str, size_t len) {
    /* if start > end, substr = NULL and errno = EINVAL */
    String *substr;
    int prop_upheld = 1;
    if (len > 0) {  /* impossible to test on empty string */
        substr = substring(str, str->len, 0);
        prop_upheld = prop_upheld && substr == NULL && errno == EINVAL;
    }

    if (len > 1) {  /* doesn't quite work with string of length 1 since -1 becomes 0 */
        errno = 0;
        substr = substring(str, -1, 0);
        prop_upheld = prop_upheld && substr == NULL && errno == EINVAL;
    }

    if (VERBOSE) {
        const char *result = prop_upheld ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("        invalid range property %s\n", result);
    }

    return prop_upheld;
}

static int substring_oob_prop(const String *str) {
    /* if start or end are out of bounds, substr = NULL and errno = EDOM */
    String *substr;
    long i;
    int prop_upheld = 1;
    long oob[] = {
        -1 * str->len - 1,
        -2 * str->len - 1,
        str->len + 1,
        2 * str->len + 1
    };
    long num_oob = 4;

    /* start out of bounds */
    for (i = 0; i < num_oob; i ++) {
        errno = 0;
        substr = substring(str, oob[i], str->len);
        prop_upheld = prop_upheld && substr == NULL && errno == EDOM;
    }

    /* end out of bounds */
    for (i = 0; i < num_oob; i++) {
        errno = 0;
        substr = substring(str, 0, oob[i]);
        prop_upheld = prop_upheld && substr == NULL && errno == EDOM;
    }

    /* start and end out of bounds */
    for (i = 0; i < num_oob; i++) {
        errno = 0;
        substr = substring(str, oob[i], oob[i]);
        prop_upheld = prop_upheld && substr == NULL && errno == EDOM;
    }

    if (VERBOSE) {
        const char *result = prop_upheld ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("        out of bounds property %s\n", result);
    }

    return prop_upheld;
}

static int substring_empty_str_prop(const String *str) {
    /* if start == end and start, end >= 0, substr should always return empty string */
    String *substr;
    size_t i;
    int prop_upheld = 1;
    for (i = 0; i < str->len; i++) {
        substr = substring(str, i, i);
        assert(substr != NULL);
        prop_upheld = prop_upheld && substr->len == 0;
        free_string(substr);
    }

    if (VERBOSE) {
        const char *result = prop_upheld ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("        empty string property %s\n", result);
    }

    return prop_upheld;
}

static int substring_char_at_prop(const String *str) {
    /* substring(i, i+1) yields str->chars[i] for i in [0, str->len) */
    /* substring(i, i) yields str->chars[str->len + i] for i in [-str->len, -1]*/
    String *substr;
    long i;
    int prop_upheld = 1;
    for (i = 0; i < (long) str->len; i++) {
        substr = substring(str, i, i + 1);
        assert(substr != NULL);
        prop_upheld = prop_upheld && substr->len == 1
            && substr->chars[0] == str->chars[i];
        free_string(substr);
    }

    for (i = -1 * str->len; i <= -1; i++) {
        substr = substring(str, i, i);
        assert(substr != NULL);
        prop_upheld = prop_upheld && substr->len == 1
            && substr->chars[0] == str->chars[str->len + i];
        free_string(substr);
    }

    if (VERBOSE) {
        const char *result = prop_upheld ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("        char at property %s\n", result);
    }

    return prop_upheld;
}

static int substring_identity_prop(const String *str) {
    /* if start = 0 and end = str->len, then susbtr = str */
    String *substr;
    int prop_upheld;
    substr = substring(str, 0, str->len);
    prop_upheld = substr->len == str->len && check_mem_equal(substr->chars, str->chars, str->len);
    free_string(substr);

    if (VERBOSE) {
        const char *result = prop_upheld ?
            COLOR_TEXT(GREEN, "upheld") :
            COLOR_TEXT(RED, "violated");
        printf("        identity property %s\n", result);
    }

    return prop_upheld;
}

static int substring_test_properties(const char *cstr, size_t len) {
    String *str = create_string(cstr, len);

    if (cstr == NULL) {
        String *substr = substring(str, 0, 1);  /* start and end values are arbitrary and irrelevant here */
        return check_null_property_upheld(substr, errno, len);
    }

    if (VERBOSE) {
        printf("    testing properties for \"%s\" length %lu\n", cstr, len);
    }

    int invalid_range_prop_upheld = substring_invalid_range_prop(str, len);
    int out_of_bounds_prop_upheld = substring_oob_prop(str);
    int empty_string_prop_upheld = substring_empty_str_prop(str);
    int char_at_prop_upheld = substring_char_at_prop(str);
    int identity_prop_upheld = substring_identity_prop(str);

    free_string(str);
    return (invalid_range_prop_upheld
        && out_of_bounds_prop_upheld
        && empty_string_prop_upheld
        && char_at_prop_upheld
        && identity_prop_upheld);
}

static int substring_test_examples(const char *cstr, size_t len,
                                   long start, long end,
                                   const char *csubstr, size_t substr_len) {
    /* not the point of this function (and NULL case already tested in properties) */
    assert(cstr != NULL && csubstr != NULL);

    String *str = create_string(cstr, len);
    String *expected_substr = create_string(csubstr, substr_len);
    String *substr = substring(str, start, end);

    /* test is meaningless if strings can't be created */
    assert(str != NULL && expected_substr != NULL && substr != NULL);

    int expectation_met = string_equal(expected_substr, substr);

    if (VERBOSE) {
        const char *result = expectation_met ?
            COLOR_TEXT(GREEN, "ok") :
            COLOR_TEXT(RED, "not ok");
        printf("    example %s (given: \"%s\" w/len %lu, expected: \"%s\" w/len %lu)\n",
               result, cstr, len, csubstr, substr_len);
        if (!expectation_met) {
            printf("        substr(\"%s\", %ld, %ld) => \"", cstr, start, end);
            string_print(substr);
            printf("\" != \"%s\"\n", csubstr);
        }
    }

    free_string(str);
    free_string(expected_substr);
    free_string(substr);

    return expectation_met;
}

static int string_contains_prop_helper(const char *prop_name,
                                       const String *str, const String *test,
                                       int expected_ret, int expected_errno) {
    errno = 0;
    int contains = string_contains(str, test);
    int errno_val = errno;

    int ret_val_ok = contains == expected_ret;
    int errno_ok = errno_val == expected_errno;
    int test_ok = ret_val_ok && errno_ok;

    if (VERBOSE) {
        if (!test_ok) {
            printf("    %s " COLOR_TEXT(RED, "violated") "\n", prop_name);
        }
        if (!ret_val_ok) {
            printf("        " COLOR_TEXT(RED, "returned %d, expected %d") "\n",
                   contains, expected_ret);
        }
        if (!errno_ok) {
            printf("        " COLOR_TEXT(RED, "errno is %d, expected %d") "\n",
                   errno_val, expected_errno);
        }
    }

    return test_ok;
}
static int string_contains_null_prop(const String *str) {
    /* when given NULL instead of a string, return 0 and set errno = EFAULT */
    return string_contains_prop_helper("null property", str, NULL, 0, EFAULT);
}

static int string_contains_empty_prop(const String *str) {
    /* every string contains the empty string */
    String *empty = create_string("", 0);
    assert(empty != NULL);

    int result = (str != NULL) ?
        string_contains_prop_helper("empty property", str, empty, 1, 0) :
        string_contains_prop_helper("empty property", str, empty, 0, EFAULT);

    free_string(empty);
    return result;
}

static int string_contains_reflexive_prop(const String *str) {
    /* every string contains itself */
    return (str != NULL) ?
        string_contains_prop_helper("reflexive property", str, str, 1, 0) :
        string_contains_prop_helper("reflexive property", str, str, 0, EFAULT);
}

static int string_contains_length_prop(const String *str) {
    /* a string cannot contain a string with a length larger than its own */
    if (str == NULL) {
        return 1;  /* skip NULL case */
    }

    String *larger = create_string(str->chars, str->len + 1);
    assert(larger != NULL);

    int result = string_contains_prop_helper("length property", str, larger, 0, 0);

    free_string(larger);
    return result;
}

static int string_contains_test_properties(const char *cstr, size_t len) {
    String *str = create_string(cstr, len);

    if (VERBOSE) {
        printf("    testing properties for \"%s\"\n", cstr);
    }

    int null_prop_upheld = string_contains_null_prop(str);
    int empty_prop_upheld = string_contains_empty_prop(str);
    int reflexive_prop_upheld = string_contains_reflexive_prop(str);
    int length_prop_upheld = string_contains_length_prop(str);

    free_string(str);
    int props_upheld = (null_prop_upheld
        && empty_prop_upheld
        && reflexive_prop_upheld
        && length_prop_upheld);

    if (VERBOSE && props_upheld) {
        printf("        properties " COLOR_TEXT(GREEN, "upheld") "\n");
    }

    return props_upheld;
}

static int string_contains_test_examples(const char *cstr, size_t len,
                                         const char *cstr2, size_t len2,
                                         int expected_ret) {
    /* not the point of this function (and NULL case already tested in properties) */
    assert(cstr != NULL && cstr2 != NULL);

    String *str = create_string(cstr, len);
    String *str2 = create_string(cstr2, len2);
    int expected_errno = 0;

    /* test is meaningless if strings can't be created */
    assert(str != NULL && str2 != NULL);

    int contains = string_contains(str, str2);
    int errno_val = errno;

    int ret_val_ok = contains == expected_ret;
    int errno_ok = errno_val == expected_errno;
    int test_ok = ret_val_ok && errno_ok;

    if (VERBOSE) {
        const char *result = test_ok ?
            COLOR_TEXT(GREEN, "passed") :
            COLOR_TEXT(RED, "failed");
        printf("    string_contains(\"%s\", \"%s\") %s\n", cstr, cstr2, result);

        if (!ret_val_ok) {
            printf("        " COLOR_TEXT(RED, "returned %d, expected %d") "\n",
                   contains, expected_ret);
        }
        if (!errno_ok) {
            printf("        " COLOR_TEXT(RED, "errno is %d, expected %d") "\n",
                   errno_val, expected_errno);
        }
    }

    free_string(str);
    free_string(str2);
    return test_ok;
}

static int tally_test_results(int *results, int num_tests) {
    int final_result = 1;
    int i;
    for (i = 0; i < num_tests; i++) {
        final_result = final_result && results[i];
    }

    return final_result ? SUCCESS : FAILURE;
}

static int string_copy_test() {
    int test_results[] = {
        string_copy_test_properties(NULL, 1),
        string_copy_test_properties("", 0),
        string_copy_test_properties("I <3 C", 6),
        string_copy_test_properties("should be simple", 16),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    return tally_test_results(test_results, num_tests);
}

static int create_string_test() {
    int test_results[] = {
        create_string_test_properties(NULL, 1),
        create_string_test_properties("", 0),
        create_string_test_properties("I <3 C", 6),
        create_string_test_properties("123456789", 9),
        create_string_test_properties("shorter", 5),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    return tally_test_results(test_results, num_tests);
}

static int string_reverse_test() {
    int test_results[] = {
        string_reverse_test_properties(NULL, 1),
        string_reverse_test_properties("", 0),
        string_reverse_test_properties("abc", 3),
        string_reverse_test_properties("another day", 11),
        string_reverse_test_properties("even", 4),
        string_reverse_test_properties("123456789123456789123456789", 27),

        string_reverse_test_examples("", "", 0),
        string_reverse_test_examples("abc", "cba", 3),
        string_reverse_test_examples("another day", "yad rehtona", 11),
        string_reverse_test_examples("even", "neve", 4),
        string_reverse_test_examples("123456789123456789123456789", "987654321987654321987654321", 27),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    return tally_test_results(test_results, num_tests);
}

static int substring_test() {
    int test_results[] = {
        substring_test_properties(NULL, 0),
        substring_test_properties("", 0),
        substring_test_properties("a", 1),
        substring_test_properties("ab", 2),
        substring_test_properties("abc", 3),
        substring_test_properties("a few words", 11),

        substring_test_examples("", 0, 0, 0, "", 0),
        substring_test_examples("abc", 3, 0, -1, "abc", 3),
        substring_test_examples("abc", 3, 0, 3, "abc", 3),
        substring_test_examples("abc", 3, 1, -1, "bc", 2),
        substring_test_examples("abc", 3, 1, 3, "bc", 2),
        substring_test_examples("abc", 3, 1, 2, "b", 1),
        substring_test_examples("a few words", 11, 2, 5, "few", 3),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    return tally_test_results(test_results, num_tests);
}

static int string_contains_test() {
    int test_results[] = {
        string_contains_test_properties(NULL, 0),
        string_contains_test_properties("", 0),
        string_contains_test_properties("abc", 3),
        string_contains_test_properties("I <3 C", 6),

        string_contains_test_examples("", 0, "", 0, 1),
        string_contains_test_examples("abc", 3, "", 0, 1),
        string_contains_test_examples("abc", 3, "ab", 2, 1),
        string_contains_test_examples("abc", 3, "bc", 2, 1),
        string_contains_test_examples("abc", 3, "ac", 2, 0),
        string_contains_test_examples("abc", 3, "ba", 2, 0),
        string_contains_test_examples("abc", 3, "cb", 2, 0),
        string_contains_test_examples("abc", 3, "a", 1, 1),
        string_contains_test_examples("abc", 3, "b", 1, 1),
        string_contains_test_examples("abc", 3, "c", 1, 1),
        string_contains_test_examples("I <3 C", 6, "<3", 2, 1),
        string_contains_test_examples("I <3 C", 6, "c", 1, 0),
    };

    int num_tests = sizeof(test_results) / sizeof(int);
    return tally_test_results(test_results, num_tests);
}

int main(int argc, char **argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0) {
            VERBOSE = 1;
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
    suite_add_test(string_tests, "substring", substring_test);
    suite_add_test(string_tests, "string contains", string_contains_test);
    run_test_suite(string_tests, VERBOSE);
    free_test_suite(string_tests);

    return 0;
}
