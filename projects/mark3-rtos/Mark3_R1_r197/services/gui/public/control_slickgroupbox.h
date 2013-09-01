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
    \file control_groupbox.h
    \brief GUI Group Box Control

    A groupbox control is essentially a panel with a text caption, and a
    lined border.
*/

#ifndef __CONTROL_SLICKGROUPBOX_H__
#define __CONTROL_SLICKGROUPBOX_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"

class SlickGroupBoxControl : public GuiControl
{
public:
    virtual void Init() { SetAcceptFocus(false); m_uBGColor = COLOR_BLACK; }
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ ) { return GUI_EVENT_OK; };
    virtual void Activate( bool bActivate_ ) {}

    void SetFont( Font_t *pstFont_ ) { m_pstFont = pstFont_; }
    void SetCaption( const K_CHAR *pcCaption_ ) { m_pcCaption = pcCaption_; }
    void SetBGColor( COLOR uColor_ ) { m_uBGColor = uColor_; }
private:

    Font_t *m_pstFont;
    const K_CHAR *m_pcCaption;
    COLOR m_uBGColor;
};

#endif

