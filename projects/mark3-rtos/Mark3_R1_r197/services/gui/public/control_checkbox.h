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
    \file control_checkbox.h
    \brief Checkbox Control

    A binary On/Off switch control
*/

#ifndef __CONTROL_CHECKBOX_H__
#define __CONTROL_CHECKBOX_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "font.h"

class CheckBoxControl : public GuiControl
{
public:
    virtual void Init();
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ );
    virtual void Activate( bool bActivate_ )  { SetStale(); }

    void SetFont( Font_t *pstFont_ )          { m_pstFont    = pstFont_; }
    void SetCaption( const char *szCaption_ ) { m_szCaption = szCaption_; }
    void SetCheck( bool bChecked_ )              { m_bChecked   = bChecked_; }
    void SetFontColor( COLOR uFontColor_ )    { m_uFontColor = uFontColor_; }
    void SetBoxColor( COLOR uBoxColor_ )      { m_uBoxColor  = uBoxColor_; }
    void SetBackColor( COLOR uBackColor_ )      { m_uBackColor = uBackColor_; }
    bool IsChecked( void )                       { return m_bChecked; }

private:
    const char *m_szCaption;
    COLOR m_uBackColor;
    COLOR m_uBoxColor;
    COLOR m_uFontColor;
    Font_t *m_pstFont;
    bool m_bChecked;
};

#endif

