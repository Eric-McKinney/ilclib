#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* while opposite of C's true/false, these align with process exit codes which
 * run_test_suite uses because each test runs in a child process
 */
#define SUCCESS 0
#define FAILURE 1

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE_HL "\033[44m"
#define BLUE "\033[0;36m"
#define END_COLOR "\033[0m"
#define COLOR_TEXT(color, text) color text END_COLOR


typedef struct {
    const char *name;
    int (*run)(const void *);
    const void *input;
} Test;

typedef struct _test_suite TestSuite;

/* Is set by handle_cmdline_args (if you use it) when -v,--verbose flags used.
 * Only used within this header file. Intended use is for tests to be able to
 * check if they should provide more verbose output.
 */
int VERBOSE = 0;

/****************************/
/* FUNCTION QUICK REFERENCE */
/****************************/

/*
 * TestSuite *create_test_suite(const char *name);
 * void free_test_suite(TestSuite *suite);
 * void suite_add_test(TestSuite *suite, const char *test_name,
 *                     int (*run)(const void *), const void *input);
 * void run_test_suite(TestSuite *suite, int verbose);
 * void handle_cmdline_args(int argc, char **argv);
 */

/*************************/
/* MACRO QUICK REFERENCE */
/*************************/

/*
 * TEST_SUITE(test_suite_name)
 * RUN_TESTS(tests...)
 */

/******************************************/
/* FUNCTION DECLARATIONS AND DESCRIPTIONS */
/******************************************/

/*
 * Allocates and initializes a test suite with the given name.
 *
 * Errors (errno values):
 *   EFAULT: name was NULL (the pointer, not the string)
 *
 * Returns: a pointer to the test suite on success and NULL on error.
 */
TestSuite *create_test_suite(const char *name);

/*
 * Frees the resources allocated for the given test suite. Do not call this
 * function on the same test suite more than once. Do not use a test suite
 * after it has been freed. It is safe to call this function on a NULL pointer.
 *
 * Errors: none.
 *
 * Returns: nothing.
 */
void free_test_suite(TestSuite *suite);

/*
 * Adds a test to the suite with given name, run function, and pointer to its
 * input. Tests are not required to have unique names, run functions, or inputs.
 * The run function is expected to return SUCCESS or FAILURE (macros defined at
 * the top of this file). Other than that, what the run function does and how it
 * uses the input are free to choose.
 *
 * Errors (errno values):
 *   EFAULT: name, run, or both were NULL
 *
 * Returns: nothing.
 */
void suite_add_test(TestSuite *suite, const char *test_name,
                    int (*run)(const void *), const void *input);

/*
 * Runs all of the tests within the given test suite. Each test is run in a
 * child process so that segfaults or anything that would terminate the program
 * does not prematurely end the test suite. The value returned by each test run
 * function should use the SUCCESS or FAILURE macros or otherwise adhere to
 * process exit code conventions so that this function can differentiate normal
 * execution from things like segfaults.
 *
 * Errors (errno values):
 *   EFAULT: suite was NULL
 *
 * Returns: nothing.
 */
void run_test_suite(TestSuite *suite, int verbose);

/*
 * Default handling of commandline arguments for only --help, -v/--verbose flags
 * including error messages for too many arguments and invalid arguments.
 *
 * Errors: just pass argc and argv untouched and nothing bad can happen :)
 *
 * Returns: nothing.
 */
void handle_cmdline_args(int argc, char **argv) {
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
}

/*
 * Given a comma-separated list of Test structs, run them in children processes.
 *
 * Notes:
 *   Must use the TEST_SUITE macro (defined below) above this macro in the file.
 *   Must use this macro within a function (e.g. your main function).
 *   Do not put a semicolon after the macro (see example below).
 *   When listing Test structs, the last one can have a comma too (see example).
 *
 * Additional notes for the curious:
 *   The do-while is for scoping, so RUN_TESTS can be used like a function call.
 *   This macro doesn't "swallow the semi-colon" to be consistent w/TEST_SUITE.
 *
 * ex:
 *
 * #include <ilc/test.h>
 *
 * TEST_SUITE("my awesome tests")
 *
 * int test_is_four(const void *input) {
 *     int arg = *((int *) input);
 *     return (arg == 4) ? SUCCESS : FAILURE;
 * }
 *
 * int main() {
 *     int x = 4;
 *     int y = 7;
 *
 *     RUN_TESTS(
 *         {"x is four", test_is_four, &x},
 *         {"y is four", test_is_four, &y},
 *     )
 * }
 */
#define RUN_TESTS(...) \
do { \
    Test tests[] = { __VA_ARGS__ }; \
    size_t ntests = sizeof(tests) / sizeof(Test); \
    __ilc_test_main__(tests, ntests); \
} while(0);

/*
 * Declares a test suite with the given name (wrapped in quotes).
 *
 * Notes:
 *   Use this macro at the top of the file or at least above RUN_TESTS.
 *   Must use this macro outside of a function.
 *   Do not use this macro more than once in a file (even with different names).
 *   Do not put a semicolon after the macro (see example below).
 *
 * ex:
 *
 * #include <ilc/test.h>
 *
 * TEST_SUITE("my awesome tests")
 */
#define TEST_SUITE(suite_name) \
static void __ilc_test_main__(Test *tests, size_t ntests) { \
    TestSuite *suite = create_test_suite(suite_name); \
    size_t i; \
    for (i = 0; i < ntests; i++) { \
        suite_add_test(suite, tests[i].name, tests[i].run, tests[i].input); \
    } \
    run_test_suite(suite, VERBOSE); \
    free_test_suite(suite); \
}

#endif
