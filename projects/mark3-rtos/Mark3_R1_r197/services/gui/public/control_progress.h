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
    \file control_progress.h
    \brief GUI Progress Bar Control

    A simple progress bar control using lines and rectangles to display
    the status of an operation from initialization to completion
*/

#ifndef __CONTROL_PROGRESS_H__
#define __CONTROL_PROGRESS_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "font.h"

class ProgressControl : public GuiControl
{
public:
    virtual void Init();
    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ );
    virtual void Activate( bool bActivate_ ) {}

    void SetBackColor( COLOR eColor_ )     { m_uBackColor = eColor_; }
    void SetProgressColor( COLOR eColor_ )  { m_uProgressColor = eColor_; }
    void SetBorderColor( COLOR eColor_ )    { m_uBorderColor = eColor_; }

    void SetProgress( K_UCHAR ucProgress_ );

private:
    COLOR m_uBackColor;
    COLOR m_uProgressColor;
    COLOR m_uBorderColor;
    K_UCHAR m_ucProgress;
};

#endif

