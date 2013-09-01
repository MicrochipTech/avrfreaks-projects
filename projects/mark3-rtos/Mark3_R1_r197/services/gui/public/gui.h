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
    \file gui.h
    \brief Graphical User Interface classes and data structure declarations.
*/

#ifndef __GUI_H__
#define __GUI_H__

#include "kerneltypes.h"
#include "ll.h"
#include "driver.h"
#include "graphics.h"

#include "message.h"

#include "keycodes.h"

#define GUI_DEBUG            (0)

#if GUI_DEBUG
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define GUI_DEBUG_PRINT        printf
#else
#define GUI_DEBUG_PRINT(...)
#endif


//---------------------------------------------------------------------------
/*!
    Event state defintions, used for determining whether or not a button or
    key is in the "up" or "down" contact state.
*/
#define EVENT_STATE_UP            (0)
#define EVENT_STATE_DOWN        (1)

//---------------------------------------------------------------------------
#define MAX_WINDOW_CONTROLS        (251) //!< Maximum number of controls per window

#define TARGET_ID_BROADCAST_Z   (252) //!< Broadcast event to all controls in the topmost window
#define TARGET_ID_BROADCAST        (253) //!< Send event to all controls in all windows
#define TARGET_ID_FOCUS            (254) //!< Send event to the in-focus control
#define TARGET_ID_HIGH_Z        (255) //!< Send event to the highest Z-order control


//---------------------------------------------------------------------------
/*!
    Enumeration defining the various UI event codes.
*/
typedef enum
{
    EVENT_TYPE_KEYBOARD,    //!< Keypress event
    EVENT_TYPE_MOUSE,        //!< Mouse movement or click event
    EVENT_TYPE_TOUCH,        //!< Touchscreen movement event
    EVENT_TYPE_JOYSTICK,    //!< Joystick event
    EVENT_TYPE_TIMER,        //!< Timer event
//---
    EVENT_TYPE_COUNT        //!< Count of different event types supported
} GuiEventType_t;

//---------------------------------------------------------------------------
/*!
    Keyboard UI event structure definition.
*/
typedef struct
{
    K_UCHAR ucKeyCode;        //!< 8-bit value representing a keyboard scan code
    union
    {
        K_UCHAR ucFlags;    //!< Flags indicating modifiers to the event
        struct
        {
            unsigned int bKeyState:1;        //!< Key is being pressed or released
            unsigned int bShiftState:1;        //!< Whether or not shift is pressed
            unsigned int bCtrlState:1;        //!< Whether or not CTRL is pressed
            unsigned int bAltState:1;        //!< Whether or not ALT it pressed
            unsigned int bWinState:1;        //!< Whether or not the Window/Clover key is pressed
            unsigned int bFnState:1;        //!< Whether or not a special function key is pressed
        };
    };
} KeyEvent_t;

//---------------------------------------------------------------------------
/*!
    Mouse UI event structure
*/
typedef struct
{
    K_USHORT usX;        //!< absolute X location of the mouse (pixel)
    K_USHORT usY;        //!< absolute Y location of the mouse (pixel)

    union
    {
        K_UCHAR ucFlags;    //!< modifier flags for the event
        struct
        {
            unsigned int bLeftState:1;        //!< State of the left mouse button
            unsigned int bRightState:1;        //!< State of the right mouse button
            unsigned int bMiddleState:1;    //!< State of the middle mouse button
            unsigned int bScrollUp:1;        //!< State of the scroll wheel (UP)
            unsigned int bScrollDown:1;        //!< State of the scroll wheel (DOWN)
        };
    };
} MouseEvent_t;

//---------------------------------------------------------------------------
/*!
    Touch UI event structure
*/
typedef struct
{
    K_USHORT usX;        //!< Absolute touch location (pixels)
    K_USHORT usY;        //!< Absolute touch location (pixels)

    union
    {
        K_USHORT ucFlags;        //!< Modifier flags
        struct
        {
            unsigned int bTouch:1;    //!< Whether or not touch is up or down
        };
    };
} TouchEvent_t;

//---------------------------------------------------------------------------
/*!
    Joystick UI event structure
*/
typedef struct
{
    union
    {
        K_USHORT usRawData;                //!< Raw joystick data
        struct
        {
            unsigned int bUp:1;            //!< D-pad UP state
            unsigned int bDown:1;        //!< D-pad DOWN state
            unsigned int bLeft:1;        //!< D-pad LEFT state
            unsigned int bRight:1;        //!< D-pad RIGHT state

            unsigned int bButton1:1;    //!< Joystick Button1 state
            unsigned int bButton2:1;    //!< Joystick Button2 state
            unsigned int bButton3:1;    //!< Joystick Button3 state
            unsigned int bButton4:1;    //!< Joystick Button4 state
            unsigned int bButton5:1;    //!< Joystick Button5 state
            unsigned int bButton6:1;    //!< Joystick Button6 state
            unsigned int bButton7:1;    //!< Joystick Button7 state
            unsigned int bButton8:1;    //!< Joystick Button8 state
            unsigned int bButton9:1;    //!< Joystick Button9 state
            unsigned int bButton10:1;    //!< Joystick Button10 state

            unsigned int bSelect:1;        //!< Start button state
            unsigned int bStart:1;        //!< Select button state
        };
    };
} JoystickEvent_t;

//---------------------------------------------------------------------------
/*!
    Timer UI event structure
*/
typedef struct
{
    K_USHORT usTicks;    //!< Number of clock ticks (arbitrary) that have elapsed
} TimerEvent_t;

//---------------------------------------------------------------------------
/*!
    Composite UI event structure.  Depending on the event type, can contain
    either a keyboard, mouse, touch, joystick, timer event, etc.
*/
typedef struct
{
    K_UCHAR ucEventType;        //!< GuiEventType_t event type
    K_UCHAR ucTargetID;         //!< Control index that this event is targeted towards
    union
    {
        KeyEvent_t        stKey;      //!< Keyboard data
        MouseEvent_t    stMouse;    //!< Mouse data
        TouchEvent_t    stTouch;    //!< Touchscreen data
        JoystickEvent_t stJoystick; //!< Joystick data
        TimerEvent_t    stTimer;    //!< Timer data
    };

} GuiEvent_t;

//---------------------------------------------------------------------------
typedef enum
{
    GUI_EVENT_OK = 0,       //!< No problem
    GUI_EVENT_CONSUMED,     //!< Event was consumed
    GUI_EVENT_CANCEL,       //!< Event processing canceled
    GUI_EVENT_RETRY,        //!< Retry processing the event
//---
    GUI_EVENT_COUNT
} GuiReturn_t;

class GuiControl;

//---------------------------------------------------------------------------
/*!
    \brief Basic Window Class

    A Window is loosely defined as a container of controls, all sharing a
    coordinate reference coordinate frame.  Events are managed on a per-window
    basis, and each window is isolated from eachother.
*/
class GuiWindow : public LinkListNode
{

public:
    /*!
        Initialize the GUI Window object prior to use.  Must be called before
        calling other methods on this object
    */
    void Init()
    {
        m_ucControlCount = 0;
        m_pclDriver = NULL;
        m_szName = "";
        ClearNode();
    }

    /*!
        Set the graphics driver to use for rendering controls on the window.

        \fn void SetDriver( GraphicsDriver *pclDriver_ )
        \param pclDriver_ Pointer to the graphics driver
    */
    void SetDriver( GraphicsDriver *pclDriver_ ) { m_pclDriver = pclDriver_; }

    /*!
        Set the graphics driver to use for rendering controls on the window.

        \fn GraphicsDriver *GetDriver()
        \return Pointer to the Window's graphics driver
    */
    GraphicsDriver *GetDriver() { return m_pclDriver; }

    /*!
        Assign a GUI Control to this window object.  Adding an object to a
        window ensures that the object will be drawn on the specific window
        surface, and ensures that events directed to this window will be
        forwarded to the controls appropriately.

        \fn AddControl( GuiControl *pclControl_, GuiControl *pclParent_ )

        \param pclControl_ Pointer to the control object to add
        \param pclParent_  Pointer to the control's "parent" object (or NULL)
    */
    void AddControl( GuiControl *pclControl_, GuiControl *pclParent_ );

    /*!
        Removes a previously-added control from the Window.

        \fn RemoveControl( GuiControl *pclControl_ )

        \param pclControl_ Pointer to the control object to remove
    */
    void RemoveControl( GuiControl *pclControl_ );

    /*!
        Returns the highest Z-Order of all controls attached to this window.

        \fn K_UCHAR GetMaxZOrder()

        \return The highest Z-Order used by controls in this window
    */
    K_UCHAR GetMaxZOrder();

    /*!
        Redraw objects in the window.  Typically, only the affected controls
        will need to be redrawn, but in some cases (such as window
        initialization), the entire window will need to be redrawn cleanly.
        This behavior is defined by the value of the bRedrawAll_ parameter.

        \fn void Redraw( K_BOOL bRedrawAll_ )
    */
    void Redraw( K_BOOL bRedrawAll_ );

    /*!
        Process an event sent to this window.  This method handles all
        of the plumbing required to target the event towards specific
        controls, or all controls in the window depending on the
        event payload.
    */
    void ProcessEvent( GuiEvent_t *pstEvent_ );

    /*!
        Set the control used to accept "focus" events.  Such events include
        keyboard events.

        \fn void SetFocus( GuiControl *pclControl_ )

        \param pclControl_ Pointer to the control object to set focus on.
    */
    void SetFocus( GuiControl *pclControl_ );

    /*!
        Return whether or not the selected control is in focus or not.

        \fn K_BOOL IsInFocus( GuiControl *pclControl_ )

        \param pclControl_ Pointer to the control object to evaluate

        \return true - the selected control is the active control on the window
                false - otherwise
    */
    K_BOOL IsInFocus( GuiControl *pclControl_ )
    {
        if (m_pclInFocus == pclControl_)
        {
            return true;
        }
        return false;
    }

    /*!
        Set the location of the topmost pixel of the window

        \param usTop_ Topmost pixel of the window
    */
    void SetTop( K_USHORT usTop_ )          { m_usTop = usTop_; }

    /*!
        Set the location of the leftmost pixel of the window

        \param usLeft_ Leftmost pixel of the window
    */
    void SetLeft( K_USHORT usLeft_ )      { m_usLeft = usLeft_; }

    /*!
        Set the height of the window (in pixels)

        \param usHeight_ Height of the window in pixels
    */
    void SetHeight( K_USHORT usHeight_ ) { m_usHeight = usHeight_; }

    /*!
        Set the width of the window (in pixels)

        \param usWidth_ Width of the window in pixels
    */
    void SetWidth( K_USHORT usWidth_ )      { m_usWidth = usWidth_; }

    /*!
        Return the topmost pixel of the window

        \return Topmost pixel of the window
    */
    K_USHORT GetTop()             { return m_usTop; }

    /*!
        Return the leftmost pixel of the window

        \return Leftmost pixel of the window
    */
    K_USHORT GetLeft()             { return m_usLeft; }

    /*!
        Get the height of the window in pixels

        \return Height of the window in pixels
    */
    K_USHORT GetHeight()         { return m_usHeight; }

    /*!
        Get the width of the window in pixels

        \return Width of the window in pixels
    */
    K_USHORT GetWidth()         { return m_usWidth; }

    /*!
        Get the Z-order of the window on the event surface
    */
    K_UCHAR GetZOrder()         { return m_ucZ; }

    /*!
        Set the Z-order of the window on the event surface
    */
    void SetZOrder( K_UCHAR ucZ_ ) { m_ucZ = ucZ_; }

    /*!
        Cycle the focus to the next active control in the window

        \fn void CycleFocus( bool bForward_ )
        \param bForward_ - Cycle to the next control when true,
                           previous control when false
    */
    void CycleFocus( bool bForward_ );

    /*!
        Set the name for this window
    */
    void SetName( const K_CHAR *szName_ ) { m_szName = szName_; }

    /*!
        Return the name of this window
    */
    const K_CHAR *GetName() { return m_szName; }

    /*!
        Invalidate a region of the window, specified by the bounding box.  The
        coordinates specified in the parameters (top and left) refer to absolute
        display coordinates, and are not relative to coordinates within a window.
    */
    void InvalidateRegion( K_USHORT usLeft_, K_USHORT usTop_, K_USHORT usWidth_, K_USHORT usHeight_ );

private:
    K_USHORT m_usTop;        //!< Topmost pixel of the window on the event surface
    K_USHORT m_usLeft;        //!< Leftmost pixel of the window on the event surface
    K_USHORT m_usHeight;    //!< Height of the window in pixels
    K_USHORT m_usWidth;        //!< Width of the window in pixels

    K_UCHAR  m_ucZ;         //!< Z-order of the window on the event surface
    const K_CHAR  *m_szName;      //!< Name applied to this window

    DoubleLinkList m_clControlList;    //!< List of controls managed by this window
    GuiControl *m_pclInFocus;        //!< Pointer to the control in event focus
    K_UCHAR m_ucControlCount;        //!< Number of controls in this window
    GraphicsDriver *m_pclDriver;    //!< Graphics driver for this window.
};

//---------------------------------------------------------------------------
/*!
    \brief GUI Event Surface Object

    An event surface is the lowest-level UI object.  It maintains a list of
    windows which are associated with it, and manages the transmission and
    routing of events to each window, and their appropriate controls

    All windows located on the event surface are assumed to share a common
    display, and coordinate frame.  In this way, multiple GUIs can be
    implemented in the system, each tied to separate physical or virtual
    displays.
*/
class GuiEventSurface
{
public:
    /*!
        Initialize an event surface before use.  Must be called prior to
        any other object methods.
    */
    void Init() { m_clMessageQueue.Init(); }

    /*!
        Add a window to the event surface.

        \param pclWindow_ Pointer to the window object to add to the sruface
    */
    void AddWindow( GuiWindow *pclWindow_ );

    /*!
        Remove a window from the event surface.

        \param pclWindow_ Pointer to the window object to remove from the surface
    */
    void RemoveWindow( GuiWindow *pclWindow_ );

    /*!
        Send an event to this window surface.  The event will be forwraded to
        all windows managed by this service.

        \param pstEvent_ Pointer to an event to send
        \return true on success, false on failure
    */
    K_BOOL SendEvent( GuiEvent_t *pstEvent_ );

    /*!
        Process an event in the event queue.  If no events are pending, the
        call will block until an event is available.
    */
    K_BOOL ProcessEvent();

    /*!
        Get the count of pending events in the event surface's queue.
    */
    K_UCHAR GetEventCount() { return m_clMessageQueue.GetCount(); }

    /*!
        Return a pointer to a window by name, or NULL on failure
    */
    GuiWindow *FindWindowByName( const K_CHAR *szName_ );

    /*!
        Invalidate a region of the window, specified by the bounding box.  The
        coordinates specified in the parameters (top and left) refer to absolute
        display coordinates, and are not relative to coordinates within a window.
    */
    void InvalidateRegion( K_USHORT usLeft_, K_USHORT usTop_, K_USHORT usWidth_, K_USHORT usHeight_ );

private:
    /*!
        Copy the contents of one message structure to another.

        \param pstDst_ Destination event pointer
        \param pstSrc_ Source event pointer
    */
    void CopyEvent( GuiEvent_t *pstDst_, GuiEvent_t *pstSrc_ );

private:
    /*!
        List of windows managed on this event surface
    */
    DoubleLinkList m_clWindowList;

    /*!
        Message queue used to manage window events
    */
    MessageQueue   m_clMessageQueue;
};

//---------------------------------------------------------------------------
/*!
    \brief GUI Control Base Class

    This class is the common ancestor to all GUI control elements.  It
    defines a base set of properties common to all controls, as well as
    methods for initialization, event handling, and redrawing.  Controls are
    directly related to Windows, which are used to manage and organize
    controls.
*/
class GuiControl : public LinkListNode
{
public:
    /*!
        Initiailize the control - must be called before use.
        Implementation is subclass specific.

        \fn void Init()
    */
    virtual void Init() = 0;

    /*!
        Redraw the control "cleanly".  Subclass specific.

        \fn void Draw()
    */
    virtual void Draw() = 0;

    /*!
        Process an event sent to the control.  Subclass specific
        implementation.

        \fn GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ )
        \param pstEvent_ Pointer to a struct containing the event data
    */
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ ) = 0;

    /*!
        Set the location of the topmost pixel of the control

        \param usTop_ Topmost pixel of the control
    */
    void SetTop( K_USHORT usTop_ )          { m_usTop = usTop_; }

    /*!
        Set the location of the leftmost pixel of the control

        \param usLeft_ Leftmost pixel of the control
    */
    void SetLeft( K_USHORT usLeft_ )      { m_usLeft = usLeft_; }

    /*!
        Set the height of the control (in pixels)

        \param usHeight_ Height of the control in pixels
    */
    void SetHeight( K_USHORT usHeight_ ) { m_usHeight = usHeight_; }

    /*!
        Set the width of the control (in pixels)

        \param usWidth_ Width of the control in pixels
    */
    void SetWidth( K_USHORT usWidth_ )      { m_usWidth = usWidth_; }

    /*!
        Set the Z-order (depth) of the control

        \param ucZ_ Z order of the control
    */
    void SetZOrder( K_UCHAR ucZ_ )          { m_ucZOrder = ucZ_; }

    /*!
        Set the index of the control, used for cycling through focus (ala
        tab order in VB).

        \param ucIdx_ Focus index of the control
    */
    void SetControlIndex( K_UCHAR ucIdx_ ) { m_ucControlIndex = ucIdx_; }

    /*!
        Return the topmost pixel of the control

        \return Topmost pixel of the control
    */
    K_USHORT GetTop()             { return m_usTop; }

    /*!
        Return the leftmost pixel of the control

        \return Leftmost pixel of the control
    */
    K_USHORT GetLeft()             { return m_usLeft; }

    /*!
        Get the height of the control in pixels

        \return Height of the control in pixels
    */
    K_USHORT GetHeight()         { return m_usHeight; }

    /*!
        Get the width of the control in pixels

        \return Width of the control in pixels
    */
    K_USHORT GetWidth()         { return m_usWidth; }

    /*!
        Return the Z-order of the control

        \return Z-order of the control
    */
    K_UCHAR  GetZOrder()        { return m_ucZOrder; }

    /*!
        Return the Control Index of the control

        \return The control index of the control
    */
    K_UCHAR  GetControlIndex()     { return m_ucControlIndex; }

    /*!
        Return whether or not the control needs to be redrawn or not

        \return true - control needs redrawing, false - control is intact.
    */
    K_BOOL     IsStale()            { return m_bStale; }

    /*!
        Return the absolute offset of the control within an event surface.
        This function will traverse through all of the object's parents,
        and their parents, until the root control and root window are
        identified.  The absolute pixel locations of the Topmost (Y)
        and Leftmost (X) pixels are populated in the

        \fn void GetControlOffset( K_USHORT *pusX_, K_USHORT *pusY_ )
        \param pusX_ Pointer to the K_USHORT containing the leftmost pixel
        \param pusY_ Pointer to the K_USHORT containing the topmost pixel
    */
    void GetControlOffset( K_USHORT *pusX_, K_USHORT *pusY_ );

    /*!
        Return whether or not the current control has the focus in the window

        \fn K_BOOL  IsInFocus()

        \return true if this control is in focus, false otherwise
    */
    K_BOOL  IsInFocus()
    {
        return m_pclParentWindow->IsInFocus(this);
    }

    /*!
        Activate or deactivate the current control - used when switching
        from one active control to another.

        \fn void Activate( bool bActivate_ )
        \param bActivate_ - true to activate, false to deactivate
    */
    virtual void Activate( bool bActivate_ ) = 0;

protected:
    friend class GuiWindow;
    friend class GuiEventSurface;

    /*!
        Set the parent control of this control.  When a control has its parent
        set, it is considered "nested" within that control.  Moving the control
        will thus result in all of its child controls to become invalidated,
        thus requiring redraws.  The control's object offsets (Top, Bottom,
        Height, and Width) also become relative to the origin of the parent
        control.

        \fn void SetParentControl( GuiControl *pclParent_ )
        \param pclParent_ Pointer to the control's parent control
    */
    void SetParentControl( GuiControl *pclParent_ ) { m_pclParentControl = pclParent_; }

    /*!
        Set the parent window of this control.  All controls within the same window
        are all associated together, and share events targetted towards a specific
        window.  Event tabbing, focus, and Z-ordering is also shared between controls
        within a window.

        \fn void SetParentWindow( GuiWindow *pclWindow_ )
        \param pclWindow_ Pointer to the control's parent window.
    */
    void SetParentWindow( GuiWindow *pclWindow_ )    { m_pclParentWindow  = pclWindow_; }

    /*!
        Return the pointer to the control's currently-assigned parent control.

        \fn GuiControl *GetParentControl()
        \return Pointer to the Control's currently assigned parent control.
    */
    GuiControl *GetParentControl()                     { return m_pclParentControl; }

    /*!
        Get the parent window of this control

        \fn GuiWindow *GetParentWindow()
        \return Pointer to the control's window
    */
    GuiWindow *GetParentWindow()                     { return m_pclParentWindow; }

    /*!
        Clear the stale flag for this control.  Should only be done after a
        redraw has been completed

        \fn    void ClearStale()
    */
    void ClearStale()                                 { m_bStale = false; }

    /*!
        Signal that the object needs to be redrawn.
    */
    void SetStale()                                 { m_bStale = true; }

    /*!
        Tell the control whether or not to accept focus.
    */
    void SetAcceptFocus( bool bFocus_ )             { m_bAcceptsFocus = bFocus_; }

    /*!
        Returns whether or not this control accepts focus.
    */
    bool AcceptsFocus()                             { return m_bAcceptsFocus; }
private:
    /*! true if the control is stale and needs to be redrawn, false otherwise*/
    K_BOOL     m_bStale;

    /*! Whether or not the control accepts focus or not*/
    K_BOOL   m_bAcceptsFocus;

    /*! The Z-Order (depth) of the control. Only the highest order controls
        are visible at any given location */
    K_UCHAR  m_ucZOrder;

    /*! Index of the control in the window.  This is used for setting focus
        when transitioning from control to control on a window */
    K_UCHAR  m_ucControlIndex;

    /*! Topmost location of the control on the window */
    K_USHORT m_usTop;

    /*! Leftmost location of the control on the window */
    K_USHORT m_usLeft;

    /*! Width of the control in pixels */
    K_USHORT m_usWidth;

    /*! Height of the control in pixels */
    K_USHORT m_usHeight;

    /*! Pointer to the parent control */
    GuiControl *m_pclParentControl;

    /*! Pointer to the parent window associated with this control */
    GuiWindow  *m_pclParentWindow;
};

//---------------------------------------------------------------------------
/*!
    Stub control class, used for testing out the GUI framework where certain
    controls have not yet been implemented.
*/
class StubControl : public GuiControl
{
public:
    virtual void Init() {  }
    virtual void Draw() {  }
    virtual GuiReturn_t ProcessEvent( GuiEvent_t *pstEvent_ ) { return GUI_EVENT_OK; }
    virtual void Activate( bool bActivate_ ) { }
};

#endif

