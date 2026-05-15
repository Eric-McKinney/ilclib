#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0  /* while opposite of C's true/false, it fits process exit codes which run_test_suite uses */
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

TestSuite *create_test_suite(const char *name);
void free_test_suite(TestSuite *suite);
void suite_add_test(TestSuite *suite, const char *test_name,
                    int (*run)(const void *), const void *input);
void run_test_suite(TestSuite *suite, int verbose);

int VERBOSE = 0;

#define RUN_TESTS(...) \
    Test tests[] = { __VA_ARGS__ }; \
    size_t ntests = sizeof(tests) / sizeof(Test); \
    __ilc_test_main__(tests, ntests); \

#define TEST_SUITE(suite_name) static void __ilc_test_main__(Test *tests, size_t ntests) { \
    TestSuite *suite = create_test_suite(#suite_name); \
    size_t i; \
    for (i = 0; i < ntests; i++) { \
        suite_add_test(suite, tests[i].name, tests[i].run, tests[i].input); \
    } \
    run_test_suite(suite, VERBOSE); \
    free_test_suite(suite); \
}

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

#endif
