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
    \file control_notification.cpp
    \brief Notification pop-up control

    A pop-up control that can be used to present the user with information
    about system state changes, events, etc.
*/

#include "control_notification.h"
#include "kerneltypes.h"

//---------------------------------------------------------------------------
void NotificationControl::Draw()
{
    if (!m_bVisible)
    {
        return;
    }

    DrawRectangle_t stRect;
    DrawLine_t stLine;
    DrawText_t stText;

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
    stLine.uColor = COLOR_WHITE;
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

    stRect.usTop = GetTop() + usYOffset + 1;
    stRect.usBottom = stRect.usTop + GetHeight() - 3;
    stRect.usLeft = GetLeft() + usXOffset + 1;
    stRect.usRight = stRect.usLeft + GetWidth() - 3;
    stRect.bFill = true;
    stRect.uFillColor = COLOR_BLACK;
    stRect.uLineColor = COLOR_BLACK;
    pclDriver->Rectangle(&stRect);

    // Draw the Text
    stText.pstFont = m_pstFont;
    stText.pcString = m_szCaption;
    stText.uColor = COLOR_WHITE;
    usHalfWidth = pclDriver->TextWidth(&stText);
    usHalfWidth >>= 1;
    stText.usLeft = GetLeft() + (GetWidth()>>1) - usHalfWidth + usXOffset;
    stText.usTop = GetTop() + usYOffset;
    pclDriver->Text(&stText);
}

//---------------------------------------------------------------------------
GuiReturn_t NotificationControl::ProcessEvent( GuiEvent_t *pstEvent_ )
{

    switch (pstEvent_->ucEventType)
    {
        case EVENT_TYPE_TIMER:
        {
            if (m_bTrigger && m_usTimeout)
            {
                m_usTimeout--;

                if (!m_usTimeout)
                {
                    m_bVisible = false;
                    m_bTrigger = false;
                    SetStale();

                    K_USHORT usX, usY;
                    GetControlOffset(&usX, &usY);

                    GetParentWindow()->InvalidateRegion( GetLeft() + usX, GetTop() + usY, GetWidth(), GetHeight());
                }
            }

            break;
        }
        default:
            break;
    }
}
