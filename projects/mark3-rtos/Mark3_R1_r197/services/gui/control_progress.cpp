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
    \file control_progress.cpp
    \brief GUI Progress Bar Control

    A simple progress bar control using lines and rectangles to display
    the status of an operation from initialization to completion
*/
#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "control_progress.h"

//---------------------------------------------------------------------------
void ProgressControl::Init()
{
    m_uBackColor = COLOR_BLACK;
    m_uBorderColor = COLOR_GREY75;
    m_uProgressColor = COLOR_GREEN;
    SetAcceptFocus(false);
}

//---------------------------------------------------------------------------
void ProgressControl::Draw()
{
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();
    DrawRectangle_t stRect;
    DrawLine_t stLine;

    K_USHORT usX, usY;
    K_USHORT usProgressWidth;

    GetControlOffset(&usX, &usY);

    // Draw the outside of the progress bar region
    stLine.uColor = m_uBorderColor;
    stLine.usX1 = usX + GetLeft() + 1;
    stLine.usX2 = usX + GetLeft() + GetWidth() - 2;
    stLine.usY1 = usY + GetTop();
    stLine.usY2 = usY + GetTop();
    pclDriver->Line(&stLine);

    stLine.usY1 = usY + GetTop() + GetHeight() - 1;
    stLine.usY2 = usY + GetTop() + GetHeight() - 1;
    pclDriver->Line(&stLine);

    stLine.usY1 = usY + GetTop() + 1;
    stLine.usY2 = usY + GetTop() + GetHeight() - 2;
    stLine.usX1 = usX + GetLeft();
    stLine.usX2 = usX + GetLeft();
    pclDriver->Line(&stLine);

    stLine.usX1 = usX + GetLeft() + GetWidth() - 1;
    stLine.usX2 = usX + GetLeft() + GetWidth() - 1;
    pclDriver->Line(&stLine);

    // Draw the "completed" portion
    usProgressWidth = (K_USHORT)( ( ( ( (K_ULONG)m_ucProgress) * (GetWidth()-2) ) + 50 ) / 100);
    stRect.usTop = usY + GetTop() + 1;
    stRect.usBottom = usY + GetTop() + GetHeight() - 2;
    stRect.usLeft = usX + GetLeft() + 1;
    stRect.usRight = stRect.usLeft + usProgressWidth - 1;
    stRect.bFill = true;
    stRect.uLineColor = m_uProgressColor;
    stRect.uFillColor = m_uProgressColor;
    pclDriver->Rectangle(&stRect);

    // Draw the "incomplete" portion
    stRect.usLeft = stRect.usRight + 1;
    stRect.usRight = usX + GetLeft() + GetWidth() - 2;
    stRect.bFill = true;
    stRect.uLineColor = m_uBackColor;
    stRect.uFillColor = m_uBackColor;
    pclDriver->Rectangle(&stRect);

}

//---------------------------------------------------------------------------
void ProgressControl::SetProgress( K_UCHAR ucProgress_ )
{
    m_ucProgress = ucProgress_;
    if (m_ucProgress > 100)
    {
        m_ucProgress;
    }
    SetStale();
}

//---------------------------------------------------------------------------
GuiReturn_t ProgressControl::ProcessEvent( GuiEvent_t *pstEvent_)
{
    return GUI_EVENT_OK;
}
