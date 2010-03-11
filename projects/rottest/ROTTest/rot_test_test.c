/*
 * rot_test_test.c
 *
 */
#include "rot_test.h"
#include "stdio.h"

static void setUp(void){
	;
}

static void tearDown(void){
	;
}

ROTTestSuite(testSuite1,setUp,tearDown);

ROTTest(test1) {
	ROTAssert(1);
}

ROTTest(test2) {
	int toBeExecuted = 0;
	int notToBeExecuted = 1;
	ROTAssert(toBeExecuted);
	ROTAssert(notToBeExecuted);
}

ROTTest(test3) {
	int x = 1,y = 1;
	ROTAssertEquals(x,y);
}

ROTTest(test4) {
	int x = 0, y = 1;
	ROTAssertEquals(x,y);
}

ROTTest(test5){
	ROTFail("Should fail");
}

