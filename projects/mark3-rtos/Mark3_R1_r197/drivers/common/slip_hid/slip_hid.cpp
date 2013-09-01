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
    \file slip_hid.cpp
    
    \brief HID device abstraction over FunkenSlip channel    
*/
#include "kerneltypes.h"
#include "driver.h"
#include "gui.h"
#include "message.h"
#include "system_heap.h"
#include "slip.h"
#include "slip_mux.h"
#include "slip_hid.h"
#include "driver.h"

//---------------------------------------------------------------------------
typedef enum
{
    SLIP_HID_MOUSE,
    SLIP_HID_KEYBOARD,
    SLIP_HID_JOYSTICK
} SlipHidEvents_t;

//---------------------------------------------------------------------------
static GuiEventSurface *g_pclSurface;

//---------------------------------------------------------------------------
void SlipHidHandler(Driver *pclDriver_, K_UCHAR ucChannel_, K_UCHAR *pucData_, K_USHORT usLen_ )
{    
    K_UCHAR ucEventType = pucData_[0];
    GuiEvent_t stGuiEvent;
    bool bSend = true;
    GuiEvent_t *pstEvent = (GuiEvent_t*)&pucData_[1];

    if (!pucData_ || !usLen_)
    {
        return;
    }
    
    stGuiEvent.ucEventType = pstEvent->ucEventType;
    stGuiEvent.ucTargetID = pstEvent->ucTargetID;

    switch (ucEventType)
    {
        case SLIP_HID_MOUSE:
        {
            MouseEvent_t *pstMouse = (MouseEvent_t*)&pucData_[3];            
            
            stGuiEvent.stMouse.ucFlags = pstMouse->ucFlags;
            stGuiEvent.stMouse.usX = pstMouse->usX;
            stGuiEvent.stMouse.usY = pstMouse->usY;
        }            
            break;
        case SLIP_HID_JOYSTICK:
        {
            JoystickEvent_t *pstJoystick = (JoystickEvent_t*)&pucData_[3];            
            stGuiEvent.stJoystick.usRawData = pstJoystick->usRawData;
        }        
            break;
        case SLIP_HID_KEYBOARD:
        {
            KeyEvent_t *pstKeyboard = (KeyEvent_t*)&pucData_[3];            
            stGuiEvent.stKey.ucFlags = pstKeyboard->ucFlags;
            stGuiEvent.stKey.ucKeyCode = pstKeyboard->ucKeyCode;
        }        
            break;
        default:
            bSend = false;
        break;
    }
    
    if (bSend)
    {
        g_pclSurface->SendEvent(&stGuiEvent);
    }    
}

//---------------------------------------------------------------------------
K_USHORT SlipHid::Control( K_USHORT usEvent_, void *pvDataIn_,
                            K_USHORT usSizeIn_, void *pvDataOut_, K_USHORT usSizeOut_ )
{
    switch (usEvent_)    
    {
        case CMD_SET_SLIPMUX:
        {
            SlipMux *pclSlipMux = (SlipMux *)pvDataIn_;
            pclSlipMux->InstallHandler(SLIP_CHANNEL_HID, SlipHidHandler);
        }
            break;
        case CMD_SET_SURFACE:
        {
            g_pclSurface = (GuiEventSurface *)pvDataIn_;
        }
        default:
            return 0;
    }
    return 0;
}    
