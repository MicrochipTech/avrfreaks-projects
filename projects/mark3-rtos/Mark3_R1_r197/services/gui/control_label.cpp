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
    \file control_panel.cpp
    \brief GUI Panel Control Implementation
*/

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "graphics.h"
#include "control_label.h"

//---------------------------------------------------------------------------
void LabelControl::Draw()
{
    GUI_DEBUG_PRINT( "LabelControl::Draw()\n");
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();

    DrawWindow_t stWindow;

    K_USHORT usX, usY;

    GetControlOffset(&usX, &usY);

    DrawText_t stText;
    DrawRectangle_t stRect;
    K_USHORT usTextWidth;

    stText.usTop = GetTop() + usY;
    stText.usLeft = GetLeft() + usX;
    stText.uColor = m_uFontColor;
    stText.pcString = m_pcCaption;
    stText.pstFont = m_pstFont;
    usTextWidth = pclDriver->TextWidth(&stText);

    stRect.usLeft = GetLeft() + usX;
    stRect.usRight = stRect.usLeft + GetWidth() - 1;
    stRect.usTop = GetTop() + usY;
    stRect.usBottom = stRect.usTop + GetHeight() - 1;
    stRect.uLineColor = m_uBackColor;
    stRect.uFillColor = m_uBackColor;
    stRect.bFill = true;
    pclDriver->Rectangle(&stRect);

    stWindow.usLeft = stText.usLeft;
    stWindow.usRight = stText.usLeft+ GetWidth() - 1;
    stWindow.usTop = stText.usTop;
    stWindow.usBottom = stText.usTop + GetHeight() - 1;

    if (!stText.pstFont)
    {
        return;
    }

    pclDriver->SetWindow(&stWindow);
    pclDriver->Text(&stText);
    pclDriver->ClearWindow();
}
