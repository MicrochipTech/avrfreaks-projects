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
#include "control_groupbox.h"

#define BORDER_OFFSET           (4)
#define TEXT_X_OFFSET           (8)
#define TEXT_Y_OFFSET           (0)

//---------------------------------------------------------------------------
void GroupBoxControl::Draw()
{
    GUI_DEBUG_PRINT( "GroupBoxControl::Draw()\n");
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();
    K_USHORT usX, usY;
    K_USHORT usTextWidth;

    GetControlOffset(&usX, &usY);

    // Draw the background panel
    {
        DrawRectangle_t stRectangle;
        stRectangle.usTop = GetTop() + usY;
        stRectangle.usBottom = stRectangle.usTop + GetHeight() -1;
        stRectangle.usLeft = GetLeft() + usX;
        stRectangle.usRight = stRectangle.usLeft + GetWidth() -1;
        stRectangle.bFill = true;
        stRectangle.uLineColor = m_uPanelColor;
        stRectangle.uFillColor = m_uPanelColor;

        pclDriver->Rectangle(&stRectangle);
    }

    // Draw the caption
    {
        DrawText_t stText;
        stText.usLeft = usX + TEXT_X_OFFSET;
        stText.usTop = usY + TEXT_Y_OFFSET;
        stText.uColor = m_uFontColor;
        stText.pstFont = m_pstFont;
        stText.pcString = m_pcCaption;

        usTextWidth = pclDriver->TextWidth(&stText);
        pclDriver->Text(&stText);
    }

    // Draw the lines surrounding the panel
    {
        DrawLine_t stLine;

        stLine.uColor = m_uLineColor;
        stLine.usY1 = GetTop() + usY + BORDER_OFFSET;
        stLine.usY2 = GetTop() + usY + GetHeight() - BORDER_OFFSET - 1;
        stLine.usX1 = usX + BORDER_OFFSET;
        stLine.usX2 = usX + BORDER_OFFSET;
        pclDriver->Line(&stLine);

        stLine.usY1 = GetTop() + usY + BORDER_OFFSET;
        stLine.usY2 = GetTop() + usY + GetHeight() - BORDER_OFFSET - 1;
        stLine.usX1 = usX + GetWidth() - BORDER_OFFSET - 1;
        stLine.usX2 = usX + GetWidth() - BORDER_OFFSET - 1;
        pclDriver->Line(&stLine);

        stLine.usY1 = GetTop() + usY + GetHeight() - BORDER_OFFSET - 1;
        stLine.usY2 = GetTop() + usY + GetHeight() - BORDER_OFFSET - 1;
        stLine.usX1 = usX + BORDER_OFFSET;
        stLine.usX2 = usX + GetWidth() - BORDER_OFFSET - 1;
        pclDriver->Line(&stLine);

        stLine.usY1 = GetTop() + BORDER_OFFSET - 1;
        stLine.usY2 = GetTop() + BORDER_OFFSET - 1;
        stLine.usX1 = usX + BORDER_OFFSET;
        stLine.usX2 = usX + TEXT_X_OFFSET - 2;
        pclDriver->Line(&stLine);

        stLine.usX1 = usX + TEXT_X_OFFSET + usTextWidth;
        stLine.usX2 = usX + GetWidth() - BORDER_OFFSET - 1;
        pclDriver->Line(&stLine);
    }


}
