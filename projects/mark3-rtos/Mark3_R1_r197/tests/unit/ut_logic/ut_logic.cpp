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

//---------------------------------------------------------------------------

#include "kerneltypes.h"
#include "kernel.h"
#include "../ut_platform.h"

//===========================================================================
// Local Defines
//===========================================================================

//===========================================================================
// Define Test Cases Here
//===========================================================================
TEST(ut_logic)
{
    // Test the built-in unit-test logic to ensure our base assumptions are
    // correct.

    // 1 == true, ensure that this test passes
    EXPECT_TRUE(1);

    // 0 == false, ensure that this test passes
    EXPECT_FALSE(0);

    // 1 != false, ensure that this test fails
    EXPECT_FAIL_FALSE(1);

    // 0 != true, ensure that this test fails
    EXPECT_FAIL_TRUE(0);

    // Ensure that various 8-32 bit values meet equality conditions
    EXPECT_EQUALS(0, 0);

    // signed 8-bit values
    EXPECT_EQUALS(-128, -128);
    EXPECT_EQUALS(127, 127);

    // unsigned 8-bit values
    EXPECT_EQUALS(255, 255);

    // signed 16-bit values
    EXPECT_EQUALS(32767, 32767);
    EXPECT_EQUALS(-32768, -32768);

    // unsigned 16-bit values
    EXPECT_EQUALS(65535, 65535);

    // unsigned 32-bit values
    EXPECT_EQUALS(-214783648, -214783648);
    EXPECT_EQUALS(214783647, 214783647);

    // signed 32-bit values
    EXPECT_EQUALS(4294967295, 4294967295);

    // Ensure that various 8-32 bit values meet equality conditions
    EXPECT_FAIL_EQUALS(0, -1);

    // signed 8-bit values
    EXPECT_FAIL_EQUALS(-128, -1);
    EXPECT_FAIL_EQUALS(127, -1);

    // unsigned 8-bit values
    EXPECT_FAIL_EQUALS(255, 0);

    // signed 16-bit values
    EXPECT_FAIL_EQUALS(32767, -1);
    EXPECT_FAIL_EQUALS(-32768, -1);

    // unsigned 16-bit values
    EXPECT_FAIL_EQUALS(65535, 0);

    // unsigned 32-bit values
    EXPECT_FAIL_EQUALS(-214783648, -1);
    EXPECT_FAIL_EQUALS(214783647, 1);

    // signed 32-bit values
    EXPECT_FAIL_EQUALS(4294967295, 0);

}
TEST_END

//===========================================================================
// Test Whitelist Goes Here
//===========================================================================
TEST_CASE_START
  TEST_CASE(ut_logic),
TEST_CASE_END
