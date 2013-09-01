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
    \file control_notification.h
    \brief Notification pop-up control

    A pop-up control that can be used to present the user with information
    about system state changes, events, etc.
*/

#ifndef __CONTROL_NOTIFICATION_H__
#define __CONTROL_NOTIFICATION_H__

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"

class NotificationControl : public GuiControl
{
public:
    virtual void Init()
    {
        SetAcceptFocus(false);
        m_szCaption = "";
        m_pstFont = NULL;
        m_bVisible = true;
        m_bTrigger = false;
    }

    virtual void Draw();
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ );
    virtual void Activate( bool bActivate_ ) {}

    void SetFont( Font_t *pstFont_ ) { m_pstFont = pstFont_; }
    void SetCaption( const K_CHAR *szCaption_ ) { m_szCaption = szCaption_; }

    void Trigger( K_USHORT usTimeout_ )
    {
        m_usTimeout = usTimeout_;
        m_bTrigger = true;
        m_bVisible = true;
        SetStale();
    }

private:
    const K_CHAR * m_szCaption;
    Font_t *m_pstFont;
    K_USHORT m_usTimeout;
    bool m_bTrigger;
    bool m_bVisible;
};

#endif

