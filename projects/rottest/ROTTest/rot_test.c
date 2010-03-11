/*
 * rot_test.c
 *
 */
#include <stdarg.h>
#include "rot_test.h"
#include "stdio.h"
#include "setjmp.h"

static rot_test_type *currentTest;
static jmp_buf env;
static void (*reportFunction)(char *) = NO_REPORT_FUNCTION_REGISTRED;

void incrementAssertPassedForCurrentTest(void){
	++currentTest->assertCounts->passed;
}

void incrementAssertFailedForCurrentTest(void){
	++currentTest->assertCounts->failed;
	longjmp(env, -1);
}

static void reportFormat(char const* format, ...)
{
	va_list args;
	char message[128];
	if(reportFunction != NO_REPORT_FUNCTION_REGISTRED) {
		va_start(args, format);
		vsnprintf(message, sizeof(message), format, args);
		reportFunction(message);
		va_end(args);
	}
}

void reportStartingTests()
{
	reportFormat("Start Tests\n\r");
}

void reportAllTestsDone(struct passedFailedCounter testCount)
{
	reportFormat("Completed Tests: Passed: %d Failed: %d\n\r", testCount.passed, testCount.failed);
}

void reportStartingTestSuite(rot_test_suite_type* test_suite)
{
	reportFormat("\tStart TestSuite: Name: %s\n\r", test_suite->testSuiteName);
}

void reportFinishedTestSuite(rot_test_suite_type* test_suite)
{
	reportFormat("\tCompleted TestSuite: Name: %s Tests Passed: %d Tests Failed: %d Asserts Passed: %d Asserts Failed: %d\n\r",
		test_suite->testSuiteName,
		test_suite->testCounts.passed, test_suite->testCounts.failed,
		test_suite->assertCounts.passed, test_suite->assertCounts.failed);
}

void reportStartingTest(rot_test_type* test)
{
	reportFormat("\t\tStart Test: Name: %s\n\r", test->testName);
}

void reportFinishedTest(rot_test_type* test)
{
	reportFormat("\t\tCompleted Test: Name: %s Asserts Passed: %d Asserts Failed: %d\n\r", test->testName, test->assertCounts->passed, test->assertCounts->failed);
}


void reportTestPassed(char * file,int line,char *expression)
{
	reportFormat("\t\t\tAssert Passed\n\r");
	incrementAssertPassedForCurrentTest();
}

void reportTestDidNotPass(char * file,int line,char *expression)
{
	reportFormat("\t\t\tAssert Failed: Filename: %s Line: %d Expression: %s\n\r", file, line, expression);
	incrementAssertFailedForCurrentTest();
}

void registerReportFunction(void (*function)(char *)){
	reportFunction = function;
}

void doTests(void){
	struct passedFailedCounter testCount = {0, 0};
	rot_test_suite_type* current_test_suite;
	rot_test_suite_type* prev_test_suite = NO_SUITE;

	currentTest = &rot_tests_section_start;

	if(&rot_tests_section_start < &rot_tests_section_end){
		reportStartingTests();

		do{
			current_test_suite = currentTest->testSuite;
			if(current_test_suite != prev_test_suite) {
				if(prev_test_suite != NO_SUITE){
					reportFinishedTestSuite(prev_test_suite);
				}
				reportStartingTestSuite(current_test_suite);
				prev_test_suite = current_test_suite;
			}

			(current_test_suite->setUp)();
			reportStartingTest(currentTest);
			if(setjmp(env) == 0){
				(currentTest->testFunction)();
				++current_test_suite->testCounts.passed;
				++testCount.passed;
			} else {
				++current_test_suite->testCounts.failed;
				++testCount.failed;
			}
			reportFinishedTest(currentTest);

			current_test_suite->assertCounts = addPassFailedCounters(current_test_suite->assertCounts,
																	*(currentTest->assertCounts));
			(current_test_suite->tearDown)();
			currentTest++;
		}while(currentTest < &rot_tests_section_end);

		reportFinishedTestSuite(current_test_suite);
		reportAllTestsDone(testCount);
	}
}
