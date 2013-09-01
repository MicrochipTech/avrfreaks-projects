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
/*!
    \file unit_test.cpp
    \brief Unit test class definition
*/

#include "kerneltypes.h"
#include "unit_test.h"

//---------------------------------------------------------------------------
UnitTest::UnitTest()
{
    m_bIsActive = false;
    m_usIterations = 0;
    m_usPassed = 0;
    m_bComplete = false;
}

//---------------------------------------------------------------------------
void UnitTest::Pass() 
{ 
    if (m_bComplete)
    {
        return;
    }
    
    if (m_bIsActive)
    { 
        m_bIsActive = false; 
        m_usIterations++;
        m_usPassed++;
        m_bStatus = true;
    }        
}    

//---------------------------------------------------------------------------
void UnitTest::Fail()
{
    if (m_bComplete)
    {
        return;
    }
    
    if (m_bIsActive)
    {
        m_bIsActive = false;
        m_usIterations++;            
        m_bStatus = false;
    }
}
