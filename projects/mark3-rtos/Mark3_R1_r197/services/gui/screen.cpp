/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file screen.cpp
    \brief Higher level window management framework
*/

#include "kerneltypes.h"
#include "screen.h"
#include "gui.h"
#include "memutil.h"

//---------------------------------------------------------------------------
void Screen::SetManager( ScreenManager *pclScreenManager_ )
{
    m_pclScreenManager = pclScreenManager_;
}

//---------------------------------------------------------------------------
void Screen::SetWindowAffinity( const K_CHAR *szWindowName_ )
{
    m_pclWindow = m_pclScreenManager->FindWindowByName( szWindowName_ );
}

//---------------------------------------------------------------------------
GuiWindow *ScreenManager::FindWindowByName( const K_CHAR *m_szName_ )
{
    return m_pclSurface->FindWindowByName( m_szName_ );
}

//---------------------------------------------------------------------------
Screen *ScreenManager::FindScreenByName( const K_CHAR *szName_ )
{
    LinkListNode *pclTempNode = static_cast<LinkListNode*>(m_clScreenList.GetHead());

    while (pclTempNode)
    {
        if (MemUtil::CompareStrings(szName_, static_cast<Screen*>(pclTempNode)->GetName()))
        {
             return static_cast<Screen*>(pclTempNode);
        }
        pclTempNode = pclTempNode->GetNext();
    }

    return NULL;
}

