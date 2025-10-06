#ifndef TEST_H
#define TEST_H

#include <stddef.h>

#define SUCCESS 0  /* while opposite of C's true/false, it fits process exit codes which run_test_suite uses */
#define FAILURE 1

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE_HL "\033[44m"
#define BLUE "\033[0;36m"
#define END_COLOR "\033[0m"
#define COLOR_TEXT(color, text) color text END_COLOR

typedef struct _test_suite TestSuite;

TestSuite *create_test_suite(const char *name);
void free_test_suite(TestSuite *suite);
void suite_add_test(TestSuite *suite, const char *test_name, int (*run)(int));
void run_test_suite(const TestSuite *suite, int verbose);
int check_mem_equal(const void *actual, const void *expected, size_t size);

#endif
