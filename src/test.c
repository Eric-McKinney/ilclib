#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ilc/test.h>


struct _test_suite {
    const char *name;
    Test **tests;
    unsigned int num_tests;
};


TestSuite *create_test_suite(const char *name) {
    if (name == NULL) {
        errno = EFAULT;
        return NULL;
    }

    TestSuite *suite = malloc(sizeof(TestSuite));
    suite->name = name;  /* name should be a string literal in scope as long as the test suite */
    suite->tests = NULL;
    suite->num_tests = 0;

    return suite;
}

void free_test_suite(TestSuite *suite) {
    unsigned int i;

    if (suite == NULL) {
        return;
    }

    for (i = 0; i < suite->num_tests; i++) {
        free(suite->tests[i]);
    }

    free(suite->tests);
    suite->tests = NULL;
    suite->num_tests = 0;
    free(suite);
}

static Test *create_test(const char *name, int (*run)(const void *), const void *input) {
    if (name == NULL || run == NULL) {
        errno = EFAULT;
        return NULL;
    }

    Test *test = malloc(sizeof(Test));
    test->name = name;  /* name should be a string literal in scope as long as the test suite it belongs to */
    test->run = run;
    test->input = input;
    return test;
}

void suite_add_test(TestSuite *suite, const char *test_name, int (*run)(const void *),
                    const void *input) {
    suite->num_tests++;
    suite->tests = realloc(suite->tests, suite->num_tests * sizeof(Test *));

    if (suite->tests == NULL) {
        fprintf(stderr, "suite_add_test: failed to add test %s (realloc failed)\n", test_name);
        exit(EXIT_FAILURE);
    }

    suite->tests[suite->num_tests - 1] = create_test(test_name, run, input);
}

void run_test_suite(TestSuite *suite, int verbose) {
    unsigned int i, num_passed = 0;

    printf("=== " COLOR_TEXT(BLUE_HL, "%s") " ===\n", suite->name);

    for (i = 0; i < suite->num_tests; i++) {
        int test_result;
        pid_t fork_result;

        if (verbose) {
            printf("\n");
        }
        printf(COLOR_TEXT(BLUE, "%s"), suite->tests[i]->name);
        if (verbose) {
            printf(":\n");
        } else {
            printf("...");
        }

        fflush(stdout);
        fork_result = fork();

        if (fork_result < 0) {
            fprintf(stderr, "run_test_suite: failed to create test environment (fork returned %d)\n", fork_result);
            exit(EXIT_FAILURE);
        } else if (fork_result != 0) {  /* parent */
            int status;
            wait(&status);

            if (WIFEXITED(status)) {
                test_result = WEXITSTATUS(status);
            } else {
                if (verbose && WIFSIGNALED(status) && WTERMSIG(status) == 11) {  /* 11 is SIGSEGV */
                    printf("    test had a " COLOR_TEXT(RED, "segmentation fault") "\n");
                }

                test_result = FAILURE;
            }
        } else {  /* child */
            const void *test_input = suite->tests[i]->input;
            test_result = suite->tests[i]->run(test_input);
            free_test_suite(suite);  /* not strictly necessary, but makes finding real memory leaks easier */
            exit(test_result);
        }

        int test_passed = test_result == SUCCESS ? 1 : 0;
        num_passed += test_passed;
        const char *result_text = (test_passed ? COLOR_TEXT(GREEN, "PASSED") : COLOR_TEXT(RED, "FAILED"));

        if (verbose) {
            printf("Result: %s\n", result_text);
        } else {
            printf("%s\n", result_text);
        }
    }

    printf("\n" COLOR_TEXT(BLUE_HL, "%s") ": %d of %d tests passed\n", suite->name, num_passed, suite->num_tests);
}

static __inline__ void print_indent(int indent, int indent_level) {
    for (int i = 0; i < indent * indent_level; i++) {
        printf(" ");
    }
}

int test_mem_equal(const void *expected, const void *result, size_t nbytes,
                   const char *fail_msg, void (*print_fn)(const void *),
                   int verbose, int indent, int indent_level) {
    if (memcmp(expected, result, nbytes) == 0) {
        return 1;
    }

    if (verbose) {
        print_indent(indent, indent_level);

        if (fail_msg != NULL) {
            printf("%s\n", fail_msg);
        }

        if (print_fn != NULL) {
            print_indent(indent, indent_level + 1);
            printf("expected:\n");
            print_indent(indent, indent_level + 2);
            print_fn(expected);
            print_indent(indent, indent_level + 1);
            printf("but got:\n");
            print_indent(indent, indent_level + 2);
            print_fn(result);
        }
    }

    return 0;
}

int test_errno_equal(int expected_errno, int actual_errno, int verbose,
                     int indent, int indent_level) {
    if (expected_errno == actual_errno) {
        return 1;
    }

    if (verbose) {
        print_indent(indent, indent_level);
        printf(COLOR_TEXT(RED, "incorrect errno") ": ");
        printf("(expected: %d", expected_errno);
        printf(", but got: %d)\n", actual_errno);
    }

    return 0;
}

int test_condition(int condition, const char *fail_msg, int verbose, int indent,
                   int indent_level) {
    if (!condition && verbose) {
        print_indent(indent, indent_level);
        printf("%s\n", fail_msg);
    }

    return condition;
}
