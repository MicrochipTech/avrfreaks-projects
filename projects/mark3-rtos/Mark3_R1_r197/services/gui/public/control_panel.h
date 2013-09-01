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
    \file control_panel.h
    \brief GUI Panel Control

    The "panel" is probably the simplest control that can be implemented in a
    GUI.  It serves as a dock for other controls, and also as an example
    for implementing more complex controls.

    A panel is essentially a flat rectangle, specified by a control's typical
    top/left/height/width parameters, and a color value.
*/

#ifndef __CONTROL_PANEL_H__
#define __CONTROL_PANEL_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"

class PanelControl : public GuiControl
{
public:
    virtual void Init() { m_uColor = COLOR_BLACK; SetAcceptFocus(false); }
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ ) {};
    virtual void Activate( bool bActivate_ ) {}

    void SetColor( COLOR eColor_ ) { m_uColor = eColor_; }

private:
    COLOR m_uColor;

};

#endif

