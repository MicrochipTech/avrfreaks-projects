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
    \file joystick.cpp
    
    \brief Joystick driver base-class implementation
*/
#include "kerneltypes.h"
#include "joystick.h"

//---------------------------------------------------------------------------
void JoystickDriver::Scan()
{
    m_stLastReport.usDigital = m_stCurrentReport.usDigital;
    m_stLastReport.usAnalogX1 = m_stCurrentReport.usAnalogX1;
    m_stLastReport.usAnalogY1 = m_stCurrentReport.usAnalogY1;
    m_stLastReport.usAnalogX2 = m_stCurrentReport.usAnalogX2;
    m_stLastReport.usAnalogY2 = m_stCurrentReport.usAnalogY2;
    Control(JOYSTICK_SCAN, 0, 0, (void*)&m_stCurrentReport, 0);    
}

//---------------------------------------------------------------------------
void JoystickDriver::ReadCurrent( JoystickReport *pstReport_ )
{
    pstReport_->usDigital = m_stCurrentReport.usDigital;
    pstReport_->usAnalogX1 = m_stCurrentReport.usAnalogX1;
    pstReport_->usAnalogY1 = m_stCurrentReport.usAnalogY1;
    pstReport_->usAnalogX2 = m_stCurrentReport.usAnalogX2;
    pstReport_->usAnalogY2 = m_stCurrentReport.usAnalogY2;
}

//---------------------------------------------------------------------------
void JoystickDriver::ReadPrevious( JoystickReport *pstReport_ )
{
    pstReport_->usDigital = m_stLastReport.usDigital;
    pstReport_->usAnalogX1 = m_stLastReport.usAnalogX1;
    pstReport_->usAnalogY1 = m_stLastReport.usAnalogY1;
    pstReport_->usAnalogX2 = m_stLastReport.usAnalogX2;
    pstReport_->usAnalogY2 = m_stLastReport.usAnalogY2;
}
