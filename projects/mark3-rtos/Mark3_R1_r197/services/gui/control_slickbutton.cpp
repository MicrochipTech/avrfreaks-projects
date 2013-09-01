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
    \file control_button.cpp
    \brief GUI Button Control Implementation

    Basic pushbutton control with an up/down state.
*/

#include "control_slickbutton.h"
#include "gui.h"
#include <stdio.h>
#include <stdlib.h>


/* Control looks somthing like this:

   ----------------------------
  |         tone 1             |
  |____________________________|
  |         tone 2             |
  |                            |
   ----------------------------

   Button with rounded corners,
   two-tone.
*/

void SlickButtonControl::Init()
{
    m_szCaption = "Button";
    m_pstFont = NULL;
    m_bState = false;
    m_pfCallback = NULL;
    m_pvCallbackData = NULL;
    m_ucTimeout = 0;
    SetAcceptFocus(true);
}
//---------------------------------------------------------------------------
void SlickButtonControl::Draw()
{
    DrawText_t stText;
    DrawLine_t stLine;
    DrawRectangle_t stRect;

    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();

    K_USHORT usXOffset = 0;
    K_USHORT usHalfWidth = 0;
    K_USHORT usYOffset = 0;

    // Get the location of the control relative to elements higher in the heirarchy
    GetControlOffset(&usXOffset, &usYOffset);

    // Draw the rounded-off rectangle
    stLine.usX1 = GetLeft() + usXOffset + 1;
    stLine.usX2 = stLine.usX1 + GetWidth() - 3;
    stLine.usY1 = GetTop() + usYOffset;
    stLine.usY2 = stLine.usY1;
    stLine.uColor = COLOR_GREY50;
    pclDriver->Line(&stLine);

    stLine.usY1 = GetTop() + usYOffset + GetHeight() - 1;
    stLine.usY2 = stLine.usY1;
    pclDriver->Line(&stLine);

    // Draw the rounded-off rectangle
    stLine.usX1 = GetLeft() + usXOffset ;
    stLine.usX2 = stLine.usX1;

    stLine.usY1 = GetTop() + usYOffset + 1;
    stLine.usY2 = stLine.usY1 + GetHeight() - 3;
    pclDriver->Line(&stLine);

    // Draw the rounded-off rectangle
    stLine.usX1 = GetLeft() + usXOffset + GetWidth() - 1;
    stLine.usX2 = stLine.usX1;
    pclDriver->Line(&stLine);

    // Draw the two-tone button

    stRect.usTop = GetTop() + usYOffset + 1;
    stRect.usBottom = stRect.usTop + ((GetHeight() - 1) / 2) - 1;
    stRect.usLeft = GetLeft() + usXOffset + 1;
    stRect.usRight = stRect.usLeft + GetWidth() - 3;
    stRect.bFill = true;

    if (m_ucTimeout)
    {
        if (m_ucTimeout & 1)
        {
            stRect.uLineColor = COLOR_WHITE;
        }
        else
        {
            stRect.uLineColor = COLOR_BLACK;
        }
    }
    else
    {
        if (IsInFocus())
        {
            stRect.uLineColor = RGB_COLOR((K_ULONG)(MAX_RED * 0.10),(K_ULONG)(MAX_RED * 0.10),(K_ULONG)(MAX_BLUE * 0.65) );
        }
        else
        {
            stRect.uLineColor = RGB_COLOR( (K_ULONG)(MAX_RED * 0.10),(K_ULONG)(MAX_GREEN * 0.10),(K_ULONG)(MAX_BLUE * 0.10));;
        }
    }
    stRect.uFillColor = stRect.uLineColor;
    pclDriver->Rectangle(&stRect);

    stRect.usTop = stRect.usBottom + 1;
    stRect.usBottom = stRect.usTop + ((GetHeight() - 1) / 2 ) - 1;

    if (m_ucTimeout)
    {
        if (m_ucTimeout & 1)
        {
            stRect.uLineColor = COLOR_WHITE;
        }
        else
        {
            stRect.uLineColor = COLOR_BLACK;
        }
    }
    else
    {
        if (IsInFocus())
        {
            stRect.uLineColor = RGB_COLOR((K_ULONG)(MAX_RED * 0.20),(K_ULONG)(MAX_RED * 0.20),(K_ULONG)(MAX_BLUE * 0.75) );
        }
        else
        {
            stRect.uLineColor = RGB_COLOR( (K_ULONG)(MAX_RED * 0.20),(K_ULONG)(MAX_GREEN * 0.20),(K_ULONG)(MAX_BLUE * 0.20));
        }
    }

    stRect.uFillColor = stRect.uLineColor;
    pclDriver->Rectangle(&stRect);
    // Draw the Text
    stText.pstFont = m_pstFont;
    stText.pcString = m_szCaption;
    stText.uColor = COLOR_GREY75;
    usHalfWidth = pclDriver->TextWidth(&stText);
    usHalfWidth >>= 1;
    stText.usLeft = GetLeft() + (GetWidth()>>1) - usHalfWidth + usXOffset;
    stText.usTop = GetTop() + usYOffset;
    pclDriver->Text(&stText);
}

//---------------------------------------------------------------------------
GuiReturn_t SlickButtonControl::ProcessEvent( GuiEvent_t *pstEvent_ )
{
    K_USHORT usXOffset, usYOffset;

    GetControlOffset(&usXOffset, &usYOffset);

    GUI_DEBUG_PRINT("ButtonControl::ProcessEvent\n");

    switch (pstEvent_->ucEventType)
    {
        case EVENT_TYPE_TIMER:
        {
            if (m_ucTimeout)
            {
                SetStale();
                m_ucTimeout--;
                if (!m_ucTimeout)
                {
                    if (m_pfCallback)
                    {
                        m_pfCallback(m_pvCallbackData);
                    }

                    SetAcceptFocus(true);
                }
            }

            break;
        }
        case EVENT_TYPE_KEYBOARD:
        {
            // If this is a space bar or an enter key, behave like a mouse click.
            if ((KEYCODE_SPACE == pstEvent_->stKey.ucKeyCode) ||
                (KEYCODE_RETURN == pstEvent_->stKey.ucKeyCode))
            {
                if (pstEvent_->stKey.bKeyState)
                {
                    m_bState = true;
                }
                else
                {
                    m_bState = false;
                    m_ucTimeout = 10;
                    //SetAcceptFocus(false);
                    //GetParentWindow()->SetFocus(NULL);
                    SetStale();

                }
                SetStale();
            }
        }
            break;
        case EVENT_TYPE_MOUSE:
        {
            if (m_ucTimeout)
            {
                break;
            }
            // Is this control currently in the "active"/pressed state?
            if (m_bState)
            {
                // Check to see if the movement is out-of-bounds based on the coordinates.
                // If so, de-activate the control
                if (pstEvent_->stMouse.bLeftState)
                {
                    if ((pstEvent_->stMouse.usX < GetLeft() + usXOffset) ||
                        (pstEvent_->stMouse.usX >= GetLeft() + usXOffset + GetWidth()-1) ||
                        (pstEvent_->stMouse.usY < GetTop() + usYOffset) ||
                        (pstEvent_->stMouse.usY >= GetTop() + usYOffset + GetHeight() - 1))
                    {
                        m_bState = false;
                        SetStale();
                    }
                }
                // left button state is now up, and the control was previously active.
                // Run the event callback for the mouse, and go from there.
                else
                {
                    if ((pstEvent_->stMouse.usX >= GetLeft() + usXOffset) &&
                        (pstEvent_->stMouse.usX < GetLeft() + usXOffset + GetWidth()-1) &&
                        (pstEvent_->stMouse.usY >= GetTop() + usYOffset) &&
                        (pstEvent_->stMouse.usY < GetTop() + usYOffset + GetHeight() - 1))
                    {
                        m_bState = false;
                        m_ucTimeout = 10;
                        SetAcceptFocus(false);
                        GetParentWindow()->SetFocus(NULL);
                        SetStale();

                    }
                }
            }
            else if (!m_bState)
            {
                // If we registered a down-click in the bounding box, set the state of the
                // control to activated.
                if (pstEvent_->stMouse.bLeftState)
                {
                    if ((pstEvent_->stMouse.usX >= GetLeft() + usXOffset) &&
                        (pstEvent_->stMouse.usX < GetLeft() + usXOffset + GetWidth()-1) &&
                        (pstEvent_->stMouse.usY >= GetTop() + usYOffset) &&
                        (pstEvent_->stMouse.usY < GetTop() + usYOffset + GetHeight() - 1))
                    {
                        m_bState = true;
                        SetStale();
                    }
                }
            }

            if (!IsInFocus())
            {
                GetParentWindow()->SetFocus(this);
                SetStale();
            }

        }
            break;
    }

}

//---------------------------------------------------------------------------
void SlickButtonControl::Activate( bool bActivate_ )
{
    // When we de-activate the control, simply disarm the control and force
    // a redraw
    if (!bActivate_)
    {
        m_bState = false;
    }
    SetStale();
}
