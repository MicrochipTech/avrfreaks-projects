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
#include "control_panel.h"

//---------------------------------------------------------------------------
void PanelControl::Draw()
{
    GUI_DEBUG_PRINT( "PanelControl::Draw()\n");
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();
    DrawRectangle_t stRectangle;    
    K_USHORT usX, usY;
    
    GetControlOffset(&usX, &usY);
    
    stRectangle.usTop = GetTop() + usY;
    stRectangle.usBottom = stRectangle.usTop + GetHeight() -1;
    stRectangle.usLeft = GetLeft() + usX;
    stRectangle.usRight = stRectangle.usLeft + GetWidth() -1;
    stRectangle.bFill = true;
    stRectangle.uLineColor = m_uColor;
    stRectangle.uFillColor = m_uColor;
    
    pclDriver->Rectangle(&stRectangle);
}
