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
    \file control_groupbox.cpp
    \brief GUI GroupBox Control Implementation
*/

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "graphics.h"
#include "control_slickgroupbox.h"

#define TEXT_X_OFFSET           (8)
#define TEXT_Y_OFFSET           (0)
#define TEXT_BANNER_HEIGHT      (5)


//---------------------------------------------------------------------------
void SlickGroupBoxControl::Draw()
{
    GUI_DEBUG_PRINT( "GroupBoxControl::Draw()\n");
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();
    K_USHORT usX, usY;
    K_USHORT usTextWidth;

    GetControlOffset(&usX, &usY);

        // Draw the header rectangle
    {
        DrawRectangle_t stRect;
        stRect.usTop = GetTop() + usY;
        stRect.usBottom = stRect.usTop + GetHeight() - 1;
        stRect.usLeft = GetLeft() + usX;
        stRect.usRight = stRect.usLeft + GetWidth() - 1;
        stRect.bFill = true;
        stRect.uFillColor = m_uBGColor;
        stRect.uLineColor = m_uBGColor;
        pclDriver->Rectangle(&stRect);
    }

    {
        DrawLine_t stLine;

        // Draw the rounded-off rectangle
        stLine.usX1 = GetLeft() + usX + 1;
        stLine.usX2 = stLine.usX1 + GetWidth() - 3;
        stLine.usY1 = GetTop() + usY;
        stLine.usY2 = stLine.usY1;
        stLine.uColor = COLOR_GREY50;
        pclDriver->Line(&stLine);

        stLine.usY1 = GetTop() + usY + GetHeight() - 1;
        stLine.usY2 = stLine.usY1;
        pclDriver->Line(&stLine);

        stLine.usY1 = GetTop() + usY + m_pstFont->ucSize + TEXT_BANNER_HEIGHT;
        stLine.usY2 = stLine.usY1;
        pclDriver->Line(&stLine);

        // Draw the rounded-off rectangle
        stLine.usX1 = GetLeft() + usX ;
        stLine.usX2 = stLine.usX1;

        stLine.usY1 = GetTop() + usY + 1;
        stLine.usY2 = stLine.usY1 + GetHeight() - 3;
        pclDriver->Line(&stLine);

        // Draw the rounded-off rectangle
        stLine.usX1 = GetLeft() + usX + GetWidth() - 1;
        stLine.usX2 = stLine.usX1;
        pclDriver->Line(&stLine);

    }

    // Draw the header rectangle
    {
        DrawRectangle_t stRect;
        stRect.usTop = GetTop() + usY + 1;
        stRect.usBottom = GetTop() + usY + ((m_pstFont->ucSize + TEXT_BANNER_HEIGHT) >> 1);
        stRect.usLeft = GetLeft() + usX + 1;
        stRect.usRight = GetLeft() + GetWidth() - 2;
        stRect.bFill = true;
        stRect.uFillColor = RGB_COLOR((K_ULONG)(MAX_RED * 0.20),(K_ULONG)(MAX_RED * 0.20),(K_ULONG)(MAX_BLUE * 0.75) );
        stRect.uLineColor = stRect.uFillColor;
        pclDriver->Rectangle(&stRect);

        stRect.usTop = GetTop() + usY + ((m_pstFont->ucSize + TEXT_BANNER_HEIGHT) >> 1);
        stRect.usBottom = GetTop() + usY + (m_pstFont->ucSize + TEXT_BANNER_HEIGHT) - 1;
        stRect.uFillColor = RGB_COLOR((K_ULONG)(MAX_RED * 0.10),(K_ULONG)(MAX_RED * 0.10),(K_ULONG)(MAX_BLUE * 0.65) );
        stRect.uLineColor = stRect.uFillColor;
        pclDriver->Rectangle(&stRect);

    }

    // Draw the caption
    {
        DrawText_t stText;
        stText.usLeft = usX + GetLeft() + TEXT_X_OFFSET;
        stText.usTop = usY + GetTop() + TEXT_Y_OFFSET;
        stText.uColor = COLOR_WHITE;
        stText.pstFont = m_pstFont;
        stText.pcString = m_pcCaption;

        usTextWidth = pclDriver->TextWidth(&stText);
        pclDriver->Text(&stText);
    }


}
