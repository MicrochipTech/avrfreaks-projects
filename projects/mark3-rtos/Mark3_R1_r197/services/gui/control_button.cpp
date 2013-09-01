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

#include "control_button.h"
#include "gui.h"


void ButtonControl::Init()
{
    m_szCaption = "Button";
    m_pstFont = NULL;
    m_uBGColor = COLOR_GREY50;
    m_uActiveColor = COLOR_GREY25;
    m_uLineColor = COLOR_GREY62;
    m_uTextColor = COLOR_WHITE;
    m_bState = false;
    m_pfCallback = NULL;
    m_pvCallbackData = NULL;
    SetAcceptFocus(true);
}
//---------------------------------------------------------------------------
void ButtonControl::Draw()
{
    DrawText_t stText;
    DrawLine_t stLine;

    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();

    K_USHORT usXOffset = 0;
    K_USHORT usHalfWidth = 0;
    K_USHORT usYOffset = 0;

    // Get the location of the control relative to elements higher in the heirarchy
    GetControlOffset(&usXOffset, &usYOffset);

    // Draw the rounded-off rectangle
    stLine.usX1 = GetLeft() + usXOffset;
    stLine.usX2 = stLine.usX1 + GetWidth() - 1;
    stLine.usY1 = GetTop() + usYOffset;
    stLine.usY2 = stLine.usY1;
    stLine.uColor = m_uLineColor;
    pclDriver->Line(&stLine);

    stLine.usY1 = GetTop() + GetHeight() + usYOffset - 1;
    stLine.usY2 = stLine.usY1;
    pclDriver->Line(&stLine);

    stLine.usX1 = GetLeft() + usXOffset;
    stLine.usX2 = stLine.usX1;
    stLine.usY1 = GetTop() + usYOffset + 1;
    stLine.usY2 = GetTop() + GetHeight() - 2;
    pclDriver->Line(&stLine);

    stLine.usX1 = GetLeft() + GetWidth() + usXOffset - 1;
    stLine.usX2 = stLine.usX1;
    pclDriver->Line(&stLine);

    // Draw a rectangle before the text if the BG is specified.
    {
        DrawRectangle_t stRect;
        stRect.usLeft = GetLeft() + usXOffset + 1;
        stRect.usRight = GetLeft() + GetWidth() + usXOffset - 2;
        stRect.usTop = GetTop() + usYOffset + 1;
        stRect.usBottom = GetTop() + GetHeight() + usYOffset - 2;
        stRect.bFill = true;

        if (m_bState)
        {
            stRect.uFillColor = m_uActiveColor;
        }
        else
        {
            stRect.uFillColor = m_uBGColor;
        }

        if (GetParentWindow()->IsInFocus(this))
        {
            stRect.uLineColor = m_uLineColor;
        }
        else
        {
            stRect.uLineColor = m_uFillColor;
        }

        pclDriver->Rectangle(&stRect);
    }

    // Draw the Text
    stText.pstFont = m_pstFont;
    stText.pcString = m_szCaption;
    stText.uColor = m_uTextColor;
    usHalfWidth = pclDriver->TextWidth(&stText);
    usHalfWidth >>= 1;
    stText.usLeft = GetLeft() + (GetWidth()>>1) - usHalfWidth + usXOffset;
    stText.usTop = GetTop() + usYOffset;
    pclDriver->Text(&stText);
}

//---------------------------------------------------------------------------
GuiReturn_t ButtonControl::ProcessEvent( GuiEvent_t *pstEvent_ )
{
    K_USHORT usXOffset, usYOffset;

    GetControlOffset(&usXOffset, &usYOffset);

    GUI_DEBUG_PRINT("ButtonControl::ProcessEvent\n");

    switch (pstEvent_->ucEventType)
    {
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
                    if (m_pfCallback)
                    {
                        m_pfCallback(m_pvCallbackData);
                    }
                }
                SetStale();
            }
        }
            break;
        case EVENT_TYPE_MOUSE:
        {
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
                        SetStale();
                        if (m_pfCallback)
                        {
                            m_pfCallback(m_pvCallbackData);
                        }
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
void ButtonControl::Activate( bool bActivate_ )
{
    // When we de-activate the control, simply disarm the control and force
    // a redraw
    if (!bActivate_)
    {
        m_bState = false;
    } 
    SetStale();
}
