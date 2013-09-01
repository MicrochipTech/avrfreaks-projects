/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/

#ifndef __UT_PLATFORM_H__
#define __UT_PLATFORM_H__

#include "kerneltypes.h"
#include "kernel.h"
#include "unit_test.h"

//---------------------------------------------------------------------------
#define STACK_SIZE_APP		(256)	//!< Size of the main app's stack
#define STACK_SIZE_IDLE		(192)	//!< Size of the idle thread stack

//---------------------------------------------------------------------------
#define UART_SIZE_RX		(12)	//!< UART RX Buffer size
#define UART_SIZE_TX		(12)	//!< UART TX Buffer size

//---------------------------------------------------------------------------
class MyUnitTest : public UnitTest
{
public:
    void PrintTestResult();
};

//---------------------------------------------------------------------------
typedef void (*TestFunc)(void);

//---------------------------------------------------------------------------
typedef struct
{
    const K_CHAR *szName;
    MyUnitTest *pclTestCase;
    TestFunc pfTestFunc;
} MyTestCase;

//---------------------------------------------------------------------------
#define TEST(x)        \
static MyUnitTest TestObj_##x;   \
static void TestFunc_##x(void) \
{ \
    MyUnitTest *pclCurrent = &TestObj_##x;

#define TEST_END \
    pclCurrent->Complete(); \
    pclCurrent->PrintTestResult(); \
}

//---------------------------------------------------------------------------
#define EXPECT_TRUE(x)      pclCurrent->Start(); pclCurrent->ExpectTrue(x)
#define EXPECT_FALSE(x)     pclCurrent->Start(); pclCurrent->ExpectFalse(x)
#define EXPECT_EQUALS(x,y)  pclCurrent->Start(); pclCurrent->ExpectEquals((K_LONG)(x), (K_LONG)(y))
#define EXPECT_GT(x,y)      pclCurrent->Start(); pclCurrent->ExpectGreaterThan((K_LONG)(x), (K_LONG)(y))
#define EXPECT_LT(x,y)      pclCurrent->Start(); pclCurrent->ExpectLessThan((K_LONG)(x), (K_LONG)(y))
#define EXPECT_GTE(x,y)     pclCurrent->Start(); pclCurrent->ExpectGreaterThanEquals((K_LONG)(x), (K_LONG)(y))
#define EXPECT_LTE(x,y)     pclCurrent->Start(); pclCurrent->ExpectLessThanEquals((K_LONG)(x), (K_LONG)(y))

//---------------------------------------------------------------------------
#define EXPECT_FAIL_TRUE(x)      pclCurrent->Start(); pclCurrent->ExpectFailTrue(x)
#define EXPECT_FAIL_FALSE(x)     pclCurrent->Start(); pclCurrent->ExpectFailFalse(x)
#define EXPECT_FAIL_EQUALS(x,y)  pclCurrent->Start(); pclCurrent->ExpectFailEquals((K_LONG)(x), (K_LONG)(y))
#define EXPECT_FAIL_GT(x,y)      pclCurrent->Start(); pclCurrent->ExpectFailGreaterThan((K_LONG)(x), (K_LONG)(y))
#define EXPECT_FAIL_LT(x,y)      pclCurrent->Start(); pclCurrent->ExpectFailLessThan((K_LONG)(x), (K_LONG)(y))
#define EXPECT_FAIL_GTE(x,y)     pclCurrent->Start(); pclCurrent->ExpectFailGreaterThanEquals((K_LONG)(x), (K_LONG)(y))
#define EXPECT_FAIL_LTE(x,y)     pclCurrent->Start(); pclCurrent->ExpectFailLessThanEquals((K_LONG)(x), (K_LONG)(y))

//---------------------------------------------------------------------------
#define TEST_NAME_EVALUATE(name)       #name
#define TEST_NAME_STRINGIZE(name)      TEST_NAME_EVALUATE(name)

//---------------------------------------------------------------------------
#define TEST_CASE_START MyTestCase astTestCases[] = {
#define TEST_CASE(x)    { TEST_NAME_STRINGIZE(x), &TestObj_##x, TestFunc_##x }
#define TEST_CASE_END   { 0, 0, 0 } };

//---------------------------------------------------------------------------
extern MyTestCase astTestCases[];
extern void run_tests();

//---------------------------------------------------------------------------
void PrintString(const K_CHAR *szStr_);


#endif //__UT_PLATFORM_H__

