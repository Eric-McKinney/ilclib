#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ilc/test.h>

typedef struct {
    const char *name;
    int (*run)(int);
} Test;

struct _test_suite {
    const char *name;
    Test **tests;
    unsigned int num_tests;
};


TestSuite *create_test_suite(const char *name) {
    if (name == NULL) {
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

    for (i = 0; i < suite->num_tests; i++) {
        free(suite->tests[i]);
    }

    free(suite->tests);
    suite->tests = NULL;
    suite->num_tests = 0;
    free(suite);
}

static Test *create_test(const char *name, int (*run)(int)) {
    if (name == NULL || run == NULL) {
        return NULL;
    }

    Test *test = malloc(sizeof(Test));
    test->name = name;  /* name should be a string literal in scope as long as the test suite it belongs to */
    test->run = run;
    return test;
}

void suite_add_test(TestSuite *suite, const char *test_name, int (*run)(int)) {
    suite->num_tests++;
    suite->tests = realloc(suite->tests, suite->num_tests * sizeof(Test *));

    if (suite->tests == NULL) {
        fprintf(stderr, "suite_add_test: failed to add test %s (realloc failed)\n", test_name);
        exit(EXIT_FAILURE);
    }

    suite->tests[suite->num_tests - 1] = create_test(test_name, run);
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
                test_result = FAILURE;
            }
        } else {  /* child */
            test_result = suite->tests[i]->run(verbose);
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

int check_mem_equal(const void *actual, const void *expected, size_t size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        if (((char *)actual)[i] != ((char *)expected)[i]) {
            return 0;
        }
    }

    return 1;
}
