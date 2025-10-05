#ifndef TEST_H
#define TEST_H

typedef struct TestSuite TestSuite;

void init_test_suite(TestSuite *suite, const char *name);
void destruct_test_suite(TestSuite *suite);
void suite_add_test(TestSuite *suite, char *test_name, int (*run)());
void run_test_suite(const TestSuite *suite, int verbose);

#endif
