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
    \file drvPSXjoy.cpp
    
    \brief Playstation (PSX/PS2) Joystick driver implementaiton for AVR
*/

#include "kerneltypes.h"
#include "driver.h"
#include "joystick.h"
#include "drvPSXJoy.h"

#include <avr/io.h>

//---------------------------------------------------------------------------
void PSXJoystick::Init()
{
    m_ucType = 0;
    m_bConnected = false;
}

//---------------------------------------------------------------------------
K_UCHAR PSXJoystick::Open()
{
    PSX_DDR  &= ~PSX_DATA_BIT;  // We receive data (act like PSX)    
    PSX_PORT |= PSX_DATA_BIT;   // Set internal pullup

    PSX_PORT |= PSX_CLK_BIT;    // Clock output (we control?)
    PSX_DDR  |= PSX_CLK_BIT;    // Clock output high
    
    PSX_DDR  |= PSX_CMD_BIT;    // We send commands (act like PSX)
    PSX_PORT |= PSX_CMD_BIT;    // Hold line high
        
    PSX_DDR  |= PSX_ATT_BIT;    // Attention bit (master ctrld)
    PSX_PORT |= PSX_ATT_BIT;    // Attention bit high until CS

    PSX_PORT |= PSX_ACK_BIT;    // Set internal pullup    
    PSX_DDR  &= ~PSX_ACK_BIT;   // Ack controlled by ctrlr    
    
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT PSXJoystick::Control( K_USHORT usEvent_, void *pvIn_, K_USHORT usSizeIn_,
                                                  void *pvOut_, K_USHORT usSizeOut_ )
{    
    switch(usEvent_)
    {
        // Main event - poll joypad.
        case JOYSTICK_SCAN:
        {
            ScanInternal();
        }
            break;
        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------
void PSXJoystick::JoyDelay(K_USHORT usTime_)
{
    volatile K_USHORT usTime = 0;
    while (usTime < (usTime_ << 4))
    {
        usTime++;
    }
}

//---------------------------------------------------------------------------
void PSXJoystick::ScanInternal()
{
    K_UCHAR ucBytesLeft;
    K_UCHAR ucJoyMode;
    K_UCHAR ucJoyIndex;
    
    K_UCHAR aucData[MAX_PSX_DATA_BYTES];
    
    // Signal attention - time to read out data
    PSX_PORT |= PSX_CMD_BIT;
    PSX_PORT |= PSX_CLK_BIT;
    PSX_PORT &= ~PSX_ATT_BIT;    
    
    JoyDelay(2);    
    
    // Send request for data
    CmdByte(PSX_CMD_START, true); 
        
    // Get the joypad mode
    ucJoyMode = CmdByte(PSX_CMD_POLL, false);
    m_aucRaw[0] = ucJoyMode;

    // Idle - Joypad ready to talk...
    m_aucRaw[1] = CmdByte(PSX_CMD_IDLE, false);

    // # bytes remaining based on Joypad mode:
    switch (ucJoyMode)
    {
        case PSX_TYPE_DIGITAL:    // Digital Mode
        {
            ucBytesLeft = PSX_SIZE_DIGITAL;
        }
            break;
        case PSX_TYPE_ANALOG_GREEN:    // Analog Green Mode
        {
            ucBytesLeft = PSX_SIZE_ANALOG;
        }
            break;
        case PSX_TYPE_ANALOG_RED:    // Analog Red Mode
        {
            ucBytesLeft = PSX_SIZE_ANALOG;
        }
            break;
        default:
        {
            // Disconnect.
            ucBytesLeft = 0;
            m_bConnected = false;
        }
    }
    
    // Array index for the raw PSX joypad input
    ucJoyIndex = 0;
    
    // Read the rest of the bytes.
    while (ucBytesLeft && m_bConnected)
    {
        if (ucBytesLeft != 1)
        {
            aucData[ucJoyIndex] = CmdByte(PSX_CMD_IDLE, false);
            m_aucRaw[3] = aucData[ucJoyIndex];
        }
        else
        {
            aucData[ucJoyIndex] = CmdByte(PSX_CMD_IDLE, true);
            m_aucRaw[4] = aucData[ucJoyIndex];
        }
        
        // Update indexes.
        ucBytesLeft--;
        ucJoyIndex++;
    }
    
    // Set attention high.
    PSX_PORT |= PSX_ATT_BIT;
    if (m_bConnected)
    {
        // Decode the joypad data if it's connected.
        Decode(ucJoyMode, aucData);
    }
}

//---------------------------------------------------------------------------
K_UCHAR PSXJoystick::CmdByte(K_UCHAR ucCmd_, K_BOOL bWaitAck_)
{
    K_UCHAR ucReturnVal = 0;
    K_UCHAR ucBitMask = 0x01;
    K_UCHAR ucSpin = 0;
    
    // Wait for ack to go high...
    while ((PSX_PORT & PSX_ACK_BIT) == 0)
    {
        ucSpin++;
        JoyDelay(1);
        if (ucSpin > POLL_SPIN_COUNT)
        {
            m_bConnected = false;
            return 0;
        }        
    }

    m_bConnected = true;
    JoyDelay(2);
    
    while (ucBitMask)
    {
        if ((ucCmd_ & ucBitMask) != 0)
        {
            // Clock command out.
            PSX_PORT |= PSX_CMD_BIT;
        }
        else
        {
            PSX_PORT &= ~PSX_CMD_BIT;
        }

        // data clocked in on falling edge
        PSX_PORT &= ~PSX_CLK_BIT;    
        JoyDelay(5);

        // Read the input data pin.
        if ((PSX_PIN & PSX_DATA_BIT) != 0)
        {
            ucReturnVal    |= ucBitMask;
        }
        
        // data high phase
        PSX_PORT |= PSX_CLK_BIT;    
        JoyDelay(5);
        
        // Next bit
        ucBitMask <<= 1;
    }

    // If this current byte requires acknowledgement
    if (bWaitAck_)
    {
        K_UCHAR ucCounter = 0;
        
        // Retry loop for waiting on ACK
        while(ucCounter < POLL_SPIN_COUNT)
        {
            // Poll for ACK...
            if ((PSX_PIN & PSX_ACK_BIT) == 0)
            {                
                break;
            }            
            ucCounter++;
            JoyDelay(1);
        }    
    }

    return ucReturnVal;
}
//---------------------------------------------------------------------------
void PSXJoystick::Decode(K_UCHAR ucJoyMode_, K_UCHAR *pucData_)
{
    // Common to all supported modes...
    m_stCurrentReport.bSelect   = ((pucData_[0] & 0x01) == 0);
    m_stCurrentReport.bButton10 = ((pucData_[0] & 0x02) == 0);    //L3
    m_stCurrentReport.bButton9  = ((pucData_[0] & 0x04) == 0);    //R3
    m_stCurrentReport.bStart    = ((pucData_[0] & 0x08) == 0);
    m_stCurrentReport.bUp         = ((pucData_[0] & 0x10) == 0);
    m_stCurrentReport.bRight     = ((pucData_[0] & 0x20) == 0);
    m_stCurrentReport.bDown     = ((pucData_[0] & 0x40) == 0);
    m_stCurrentReport.bLeft     = ((pucData_[0] & 0x80) == 0);
    
    // Decode the message data into the appropriate joypad report format
    switch (ucJoyMode_)
    {
        case PSX_TYPE_DIGITAL:    // Digital Mode
        {            
            m_stCurrentReport.bButton6  = ((pucData_[1] & 0x01) == 0);    //L1
            m_stCurrentReport.bButton5  = ((pucData_[1] & 0x02) == 0);    //R1
            m_stCurrentReport.bButton8  = ((pucData_[1] & 0x04) == 0);    //L2
            m_stCurrentReport.bButton7  = ((pucData_[1] & 0x08) == 0);    //R2
            m_stCurrentReport.bButton1  = ((pucData_[1] & 0x10) == 0);    //Triangle
            m_stCurrentReport.bButton2  = ((pucData_[1] & 0x20) == 0);    //O
            m_stCurrentReport.bButton3  = ((pucData_[1] & 0x40) == 0);  //X
            m_stCurrentReport.bButton4  = ((pucData_[1] & 0x80) == 0);  //[]
        }
            break;

        case PSX_TYPE_ANALOG_GREEN:    // Analog Green Mode
        {           
            m_stCurrentReport.bButton8 = ((pucData_[1] & 0x01) == 0);
            m_stCurrentReport.bButton7 = ((pucData_[1] & 0x02) == 0);
            m_stCurrentReport.bButton4 = ((pucData_[1] & 0x04) == 0);
            m_stCurrentReport.bButton1 = ((pucData_[1] & 0x08) == 0);
            m_stCurrentReport.bButton5 = ((pucData_[1] & 0x10) == 0);
            m_stCurrentReport.bButton2 = ((pucData_[1] & 0x20) == 0);
            m_stCurrentReport.bButton3 = ((pucData_[1] & 0x40) == 0);
            m_stCurrentReport.bButton7 = ((pucData_[1] & 0x80) == 0);

            m_stCurrentReport.usAnalogX2 = pucData_[2];
            m_stCurrentReport.usAnalogY2 = pucData_[3];
            m_stCurrentReport.usAnalogX1 = pucData_[4];
            m_stCurrentReport.usAnalogY1 = pucData_[5];
        }
            break;
            
        case PSX_TYPE_ANALOG_RED:    // Analog Red Mode
        {
            m_stCurrentReport.bButton6  = ((pucData_[1] & 0x01) == 0);    //L1
            m_stCurrentReport.bButton5  = ((pucData_[1] & 0x02) == 0);    //R1
            m_stCurrentReport.bButton8  = ((pucData_[1] & 0x04) == 0);    //L2
            m_stCurrentReport.bButton7  = ((pucData_[1] & 0x08) == 0);    //R2
            m_stCurrentReport.bButton1  = ((pucData_[1] & 0x10) == 0);    //Triangle
            m_stCurrentReport.bButton2  = ((pucData_[1] & 0x20) == 0);    //O
            m_stCurrentReport.bButton3  = ((pucData_[1] & 0x40) == 0);  //X
            m_stCurrentReport.bButton4  = ((pucData_[1] & 0x80) == 0);  //[]
        
            m_stCurrentReport.usAnalogX2 = pucData_[2];
            m_stCurrentReport.usAnalogY2 = pucData_[3];
            m_stCurrentReport.usAnalogX1 = pucData_[4];
            m_stCurrentReport.usAnalogY1 = pucData_[5];
        }
            break;
        default:
            break;
    }
}

