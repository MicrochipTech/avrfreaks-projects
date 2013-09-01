
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
    \file control_slickbutton.h
    \brief GUI Button Control, with a flare

    Basic pushbutton control with an up/down state, and Mark3 visual style
*/

#ifndef __CONTROL_SLICKBUTTON_H__
#define __CONTROL_SLICKBUTTON_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "font.h"

typedef void (*ButtonCallback)( void *pvData_ );

class SlickButtonControl : public GuiControl
{
public:

    virtual void Init();
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ );
    virtual void Activate( bool bActivate_ );

    void SetFont( Font_t *pstFont_ )        { m_pstFont = pstFont_; }

    void SetCaption( const K_CHAR *szCaption_ )     { m_szCaption = szCaption_;}

    void SetCallback( ButtonCallback pfCallback_, void *pvData_ )
        { m_pfCallback = pfCallback_; m_pvCallbackData = pvData_; }
private:

    const K_CHAR *m_szCaption;
    Font_t *m_pstFont;
    bool    m_bState;
    K_UCHAR m_ucTimeout;

    void *m_pvCallbackData;
    ButtonCallback m_pfCallback;
};


#endif

