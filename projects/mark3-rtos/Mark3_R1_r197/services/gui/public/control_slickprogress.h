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
    \file control_slickprogress.h
    \brief GUI Progress Bar Control, with flare.

    A simple progress bar control using lines and rectangles to display
    the status of an operation from initialization to completion
*/

#ifndef __CONTROL_SLICKPROGRESS_H__
#define __CONTROL_SLICKPROGRESS_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "font.h"

class SlickProgressControl : public GuiControl
{
public:
    virtual void Init();
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ );
    virtual void Activate( bool bActivate_ ) {}

    void SetProgress( K_UCHAR ucProgress_ );

private:
    K_UCHAR m_ucProgress;
};

#endif

