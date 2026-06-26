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
 * check if they should provide more detailed output.
 *
 * Free to modify at run time.
 */
int VERBOSE = 0;

/* Number of spaces to indent by in one level of indentation.
 *
 * Not modified by any function in this library. Used by some wrapper macros
 * below to control output formatting.
 *
 * Free to modify at run time.
 */
int INDENT = 4;

/* Number of indents.
 *
 * Not modified by any function in this library. Used by some wrapper macros
 * below to control output formatting.
 *
 * Free to modify during run time.
 */
int INDENT_LEVEL = 1;

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
 * int test_mem_equal(const void *expected, const void *result, size_t nbytes,
 *                    const char *fail_msg, void (*print_fn)(const void *),
 *                    int verbose, int indent, int indent_level);
 * int test_int_equal(int expected, int actual, const char *fail_msg,
 *                    int verbose,  int indent, int indent_level);
 * int test_condition(int condition, const char *fail_msg, int verbose,
 *                    int indent, int indent_level);
 */

/*************************/
/* MACRO QUICK REFERENCE */
/*************************/

/*
 * TEST_SUITE(test_suite_name)
 * RUN_TESTS(tests...)
 * MEM_EQUAL(expected, result, nbytes, fail_msg, print_fn)
 * INT_EQUAL(expected, actual, fail_msg)
 * ERRNO_EQUAL(expected_errno, actual_errno)
 * CONDITION(condition, fail_msg)
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
 * Tests if the regions of memory pointed to by expected and result are
 * identical for nbytes number of bytes. If the regions differ and verbose is
 * given as true (not 0), then the fail_msg is printed on its own line, followed
 * by a line each for the result of calling print_fn on expected and result. The
 * indentation is controlled by indent which is the number of spaces per each
 * level of indentation and indent_level which is the starting level of indent
 * for all printed output generated by this function. If either fail_msg or
 * print_fn are NULL then they will simply not be included in the output. If
 * both are NULL then a default message will be printed.
 *
 * Parameters:
 *   expected: pointer to region of memory with expected output/result
 *   result: pointer to region of memory with actual output/result
 *   nbytes: size of memory pointed to by expected and result (in bytes)
 *   fail_msg: message to print when memory regions aren't equal
 *   printf_fn: function to print info about expected and result
 *   verbose: boolean which enables fail_msg and print_fn when mem not equal
 *   indent: how many spaces in each indent
 *   indent_level: how many indentations to start with
 *
 * Errors:
 *   None
 *
 * Returns:
 *   1 if the expected and result memory regions are equal, 0 if they differ
 */
int test_mem_equal(const void *expected, const void *result, size_t nbytes,
                   const char *fail_msg, void (*print_fn)(const void *),
                   int verbose, int indent, int indent_level);

/*
 * Test if actual is the same as expected. Prints fail_msg if they aren't equal
 * and verbose is true.
 *
 * Parameters:
 *   expected: "correct" number
 *   actual: number to compare against expected
 *   fail_msg: message to print when ints differ and verbose is true
 *   verbose: boolean which enables error message when ints differ
 *   indent: how many spaces in each indent
 *   indent_level: how many indentations to start with
 *
 * Errors:
 *   None
 *
 * Returns:
 *   1 if the ints are equal, 0 if they are different
 */
int test_int_equal(int expected, int actual, const char *fail_msg,
                   int verbose,  int indent, int indent_level);

/*
 * Test if a condition is true. Prints fail_msg when condition is false and
 * verbose is true.
 *
 * Parameters:
 *   condition: condition to test
 *   fail_msg: string to print on failure (if verbose is true)
 *   verbose: boolean which enables printing fail_msg when condition is false
 *   indent: how many spaces in each indent
 *   indent_level: how many indentations to start with
 *
 * Errors:
 *   None
 *
 * Returns:
 *   condition (literally the value of the condition parameter)
 */
int test_condition(int condition, const char *fail_msg, int verbose, int indent,
                   int indent_level);

/**************************************/
/* MACRO DEFINITIONS AND DESCRIPTIONS */
/**************************************/

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
 *   The do-while is to avoid potential scoping issues
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

/*
 * A wrapper to the test_mem_equal function. Provides default values for
 * parameters that control the output style. See test_mem_equal in this file
 * for any additional information such as usage, parameters, or behavior.
 *
 * Notes:
 *   Use like any other function call.
 *
 * ex:
 *
 * typedef struct {
 *     int x;
 *     int y;
 * } Point;
 *
 * void print_point(const void *p) {
 *     Point *point = (Point *) p;
 *     printf("(%d, %d)", point->x, point->y);
 * }
 *
 * void shift(Point *point, int dx, int dy) {
 *     point->x += dx;
 *     point->y += dy;
 * }
 *
 * ... (in test function) ...
 *
 * Point p1 = {0,0};
 * Point expected = {4,5};
 *
 * shift(&p1, 4, 5);
 *
 * return MEM_EQUAL(&expected, &p1, sizeof(Point), "points are not equal", print_point);
 */
#define MEM_EQUAL(expected, result, nbytes, fail_msg, print_fn) \
    test_mem_equal(expected, result, nbytes, fail_msg, print_fn, VERBOSE, INDENT, \
        INDENT_LEVEL)

/*
 * A wrapper to the test_int_equal function. Provides some default values for
 * parameters that control the output style. See test_int_equal in this file
 * for any additional information such as usage, parameters, or behavior.
 *
 * Notes:
 *   Use like any other function call.
 *
 * ex:
 *
 * const int expected_apples = 20;
 * int apples = collect_apples();
 *
 * INT_EQUAL(expected_apples, apples, "incorrect number of apples collected");
 */
#define INT_EQUAL(expected, actual, fail_msg) \
    test_int_equal(expected, actual, fail_msg, VERBOSE, INDENT, INDENT_LEVEL)

/*
 * A wrapper to the test_int_equal function to compare errno values.
 * See test_int_equal in this file for any additional information.
 *
 * Notes:
 *   Use like any other function call.
 *
 * ex:
 *
 * #include <errno.h>
 * some_func_that_might_change_errno();
 * int errno_val = errno;
 *
 * return ERRNO_EQUAL(0, errno_val);
 */
#define ERRNO_EQUAL(expected_errno, actual_errno) \
    test_int_equal(expected_errno, actual_errno, COLOR_TEXT(RED, "incorrect errno"), \
        VERBOSE, INDENT, INDENT_LEVEL)

/*
 * A wrapper to the test_condition function. Provides some default values for
 * parameters that control the output style. See test_condition in this file
 * for any additional information such as usage, parameters, or behavior.
 *
 * Notes:
 *   Use like any other function call.
 *
 * ex:
 *
 * #define MAX_APPLES 200
 *
 * int num_apples = count_apples();
 * return (CONDITION(num_apples >= 0, "negative number of apples detected")
 *      && CONDITION(num_apples <= MAX_APPLES, "maximum number of apples exceeded"));
 */
#define CONDITION(condition, fail_msg) \
    test_condition(condition, fail_msg, VERBOSE, INDENT, INDENT_LEVEL)
