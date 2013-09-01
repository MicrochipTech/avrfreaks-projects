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
    \file control_gamepanel.cpp
    \brief GUI Panel Control Implementation with joystick control and 
           tick-based state machine updates.
*/

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "graphics.h"
#include "control_gamepanel.h"

//---------------------------------------------------------------------------
void GamePanelControl::Draw()
{
   // Game state machine goes here.
}

//---------------------------------------------------------------------------
GuiReturn_t GamePanelControl::ProcessEvent( GuiEvent_t *pstEvent_ )
{
    K_USHORT usXOffset, usYOffset;

    switch (pstEvent_->ucEventType)
    {
        case EVENT_TYPE_TIMER:
            // Every tick, call Draw().  This is used to kick the state
            // machine
            SetStale();
            break;
        case EVENT_TYPE_KEYBOARD:
            break;
        case EVENT_TYPE_MOUSE:
            break;
        case EVENT_TYPE_JOYSTICK:
            m_stLastJoy.usRawData = m_stCurrentJoy.usRawData;
            m_stCurrentJoy.usRawData = pstEvent_->stJoystick.usRawData;
            break;
    }
    return GUI_EVENT_OK;
}
