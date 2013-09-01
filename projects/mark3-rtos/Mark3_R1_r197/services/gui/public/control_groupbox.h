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

#ifndef __CONTROL_GROUPBOX_H__
#define __CONTROL_GROUPBOX_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"

class GroupBoxControl : public GuiControl
{
public:
    virtual void Init() { m_uLineColor = COLOR_BLACK;
                          m_uFontColor = COLOR_GREY25;
                          m_uPanelColor = COLOR_GREY75;
                          SetAcceptFocus(false); }
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ ) {};
    virtual void Activate( bool bActivate_ ) {}

    void SetPanelColor( COLOR eColor_ ) { m_uPanelColor = eColor_; }
    void SetLineColor( COLOR eColor_ ) { m_uLineColor = eColor_; }
    void SetFontColor( COLOR eColor_ ) { m_uFontColor = eColor_; }
    void SetFont( Font_t *pstFont_ ) { m_pstFont = pstFont_; }
    void SetCaption( const K_CHAR *pcCaption_ ) { m_pcCaption = pcCaption_; }
private:
    COLOR m_uPanelColor;
    COLOR m_uLineColor;
    COLOR m_uFontColor;

    Font_t *m_pstFont;
    const K_CHAR *m_pcCaption;
};

#endif

