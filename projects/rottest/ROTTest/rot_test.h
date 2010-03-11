/*
 * rot_test.h
 *
 */

#ifndef ROT_TEST_H_
#define ROT_TEST_H_

#include "_ansi.h"

#define NO_REPORT_FUNCTION_REGISTRED (void *) 0
#define NO_SUITE (void *) -1

struct passedFailedCounter {
	unsigned int passed;
	unsigned int failed;
};

static inline struct passedFailedCounter addPassFailedCounters(struct passedFailedCounter l, struct passedFailedCounter r)
{
	struct passedFailedCounter result = {
		.passed = l.passed + r.passed,
		.failed = l.failed + r.failed
	};
	return result;
}

typedef struct {
	char const* testSuiteName;
	void (*setUp) (void);
	void (*tearDown) (void);

	//statistics that are collected during the tests:
	struct passedFailedCounter assertCounts;
	struct passedFailedCounter testCounts;
}rot_test_suite_type;

typedef struct {
	char const* testName;
	void (*testFunction) (void);
	rot_test_suite_type *testSuite;
	struct passedFailedCounter *assertCounts;
}rot_test_type;

extern rot_test_type rot_tests_section_start, rot_tests_section_end;

#define ROTTestSuite(suiteName, setupFunction, tearDownFunction) \
    static rot_test_suite_type rot_current_test_suite = {        \
	   .testSuiteName = #suiteName,								 \
	   .setUp = setupFunction,									 \
	   .tearDown = tearDownFunction   							 \
	}

#define ROTTest(test) \
	void test(void); \
	rot_test_set_up(test);\
	void test(void)

#define rot_test_set_up(test) \
	static struct passedFailedCounter rot_test_assertCounts_##test;			\
	static rot_test_type rot_test_##test set_rot_test_attribute = {			\
			.testName = #test, 									   			\
			.testFunction = test, 											\
			.testSuite = &rot_current_test_suite,							\
			.assertCounts = &rot_test_assertCounts_##test					\
	}

#define set_rot_test_attribute __attribute__ ((unused,__section__ (".rot_tests")))

#define ROTAssert(expr)\
	(expr) ? reportTestPassed(__FILE__, __LINE__,#expr) : reportTestDidNotPass(__FILE__,__LINE__,#expr)
#define ROTAssertEquals(expr1,expr2)\
	((expr1) == (expr2)) ? reportTestPassed(__FILE__, __LINE__,toString(expr1 == expr2)) : reportTestDidNotPass(__FILE__,__LINE__,toString(expr1 == expr2))
#define ROTFail(expr)\
	reportTestDidNotPass(__FILE__,__LINE__,#expr)

#define toString(s) #s

void _EXFUN(doTests, (void));
void _EXFUN(reportTestPassed,(char *,int,char *));
void _EXFUN(reportTestDidNotPass,(char *,int,char *));
void _EXFUN(registerReportFunction,(void (*) (char *)));
#endif /* TEST_H_ */
