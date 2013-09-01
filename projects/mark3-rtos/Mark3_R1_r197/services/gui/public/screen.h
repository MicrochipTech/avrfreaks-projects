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
    \file screen.h
    \brief Higher level window management framework
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "kerneltypes.h"
#include "gui.h"
#include "ll.h"

//---------------------------------------------------------------------------
class ScreenList;
class ScreenManager;

//---------------------------------------------------------------------------
class Screen : public LinkListNode
{
public:
    Screen() { ClearNode(); }

    /*!
        This is called when a new screen needs to be created.  This calls the
        underlying virtual "create" method, which performs all control object
        initialization and allocation.  Calling a redraw(true) on the bound
        window will result in the new window being rendered to display.
    */
    void Activate()             { Create(); }

    /*!
        This is called when a screen is torn-down.  Essentially removes the
        controls from the named window and deallocates any memory used to
        build up the screen.
    */
    void Deactivate()           { Destroy(); }

    /*!
        Indicate by name which window this screen is to be bound.
    */
    void SetWindowAffinity( const K_CHAR *szWindowName_ );

    /*!
        Set the name of the current screen.
    */
    void SetName( const K_CHAR *szName_ )           { m_szName = szName_; }

    /*!
        Return the name of the current screen.
    */
    const K_CHAR *GetName()                         { return m_szName; }

protected:
    friend class ScreenManager;

    /*!
        Function called by the ScreenManager to set the screen affinity.
    */
    void SetManager( ScreenManager *pclScreenManager_ );

    const K_CHAR    *m_szName;
    ScreenManager   *m_pclScreenManager;
    GuiWindow       *m_pclWindow;

private:

    virtual void Create() = 0;
    virtual void Destroy() = 0;

};

//---------------------------------------------------------------------------
class ScreenList
{
public:
    ScreenList()                        { m_clList.Init(); }

    /*!
        Add a screen to the screen list
    */
    void Add( Screen *pclScreen_ )      { m_clList.Add(pclScreen_); }

    /*!
        Remove a screen from the screen list
    */
    void Remove( Screen *pclScreen_)    { m_clList.Remove(pclScreen_); }

    /*!
        Get the beginning of the screen list
    */
    Screen *GetHead()                   { return static_cast<Screen*>(m_clList.GetHead()); }

private:
    DoubleLinkList  m_clList;           //!< Double link-list used to manage screen objects
};

//---------------------------------------------------------------------------
class ScreenManager
{
public:

    ScreenManager() { m_pclSurface = NULL; }

    /*!
        Add a new screen to the screen manager.
    */
    void AddScreen( Screen *pclScreen_ )        { m_clScreenList.Add(pclScreen_);
                                                  pclScreen_->SetManager(this); }

    /*!
        Remove an existing screen from the screen manager.
    */
    void RemoveScreen( Screen *pclScreen_)      { m_clScreenList.Remove(pclScreen_);
                                                  pclScreen_->SetManager(NULL); }

    /*!
        Set the event surface on which this screen manager's screens will be displayed
    */
    void SetEventSurface( GuiEventSurface *pclSurface_ ) { m_pclSurface = pclSurface_; }

    /*!
        Return a pointer to a window by name
    */
    GuiWindow *FindWindowByName( const K_CHAR *m_szName_ );

    /*!
        Return a pointer to a screen by name
    */
    Screen *FindScreenByName( const K_CHAR *m_szName_ );

private:

    ScreenList m_clScreenList;      //!< Screen list object used to manage individual screens
    GuiEventSurface *m_pclSurface;  //!< Pointer to the GUI Event Surface on which the screens are displayed
};

#endif
