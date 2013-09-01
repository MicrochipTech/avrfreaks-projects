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
    \file gui.cpp
    \brief Graphical User Interface classes and data structure definitions.
*/

#include "message.h"
#include "kerneltypes.h"
#include "gui.h"
#include "system_heap.h"
#include "fixed_heap.h"
#include "memutil.h"

//---------------------------------------------------------------------------
void GuiWindow::AddControl( GuiControl *pclControl_, GuiControl *pclParent_ )
{
    GUI_DEBUG_PRINT("GuiWindow::AddControl\n");

    m_clControlList.Add(static_cast<LinkListNode*>(pclControl_));
    m_pclInFocus = pclControl_;
    m_ucControlCount++;

    pclControl_->SetParentWindow(this);
    pclControl_->SetParentControl(pclParent_);
}

//---------------------------------------------------------------------------
void GuiWindow::RemoveControl( GuiControl *pclControl_ )
{
    GUI_DEBUG_PRINT("GuiWindow::RemoveControl\n");

    if (pclControl_->GetPrev())
    {
        m_pclInFocus = static_cast<GuiControl*>(pclControl_->GetPrev());
    }
    else if (pclControl_->GetNext())
    {
        m_pclInFocus = static_cast<GuiControl*>(pclControl_->GetNext());
    }
    else
    {
        m_pclInFocus = NULL;
    }
    m_clControlList.Remove(static_cast<LinkListNode*>(pclControl_));
    m_ucControlCount--;
}

//---------------------------------------------------------------------------
K_UCHAR GuiWindow::GetMaxZOrder()
{
    GUI_DEBUG_PRINT("GuiWindow::GetMaxZOrder\n");

    LinkListNode *pclTempNode;
    K_UCHAR ucZ = 0;
    K_UCHAR ucTempZ;

    pclTempNode = m_clControlList.GetHead();

    while (pclTempNode)
    {
        ucTempZ = (static_cast<GuiControl*>(pclTempNode))->GetZOrder();
        if (ucTempZ > ucZ)
        {
            ucZ = ucTempZ;
        }
        pclTempNode = pclTempNode->GetNext();
    }

    return ucZ;
}

//---------------------------------------------------------------------------
void GuiWindow::Redraw( K_BOOL bRedrawAll_ )
{
    GUI_DEBUG_PRINT("GuiWindow::Redraw\n");

    K_UCHAR ucControlsLeft = m_ucControlCount;
    K_UCHAR ucCurrentZ = 0;
    K_UCHAR ucMaxZ;

    ucMaxZ = GetMaxZOrder();

    // While there are still controls left to process (and we're less than
    // the maximum Z-order, just a sanity check.), redraw each object that
    // has its stale flag set, or all controls if the bRedrawAll_ parameter
    // is true.
    while (ucControlsLeft && (ucCurrentZ <= ucMaxZ))
    {
        LinkListNode *pclTempNode;

        pclTempNode = m_clControlList.GetHead();
        while (pclTempNode)
        {
            GuiControl* pclTempControl = static_cast<GuiControl*>(pclTempNode);
            if (pclTempControl->GetZOrder() == ucCurrentZ)
            {
                if ((bRedrawAll_) || (pclTempControl->IsStale()))
                {
                    pclTempControl->Draw();
                    pclTempControl->ClearStale();
                }

                ucControlsLeft--;
            }

            pclTempNode = pclTempNode->GetNext();
        }
        ucCurrentZ++;
    }
    GUI_DEBUG_PRINT("  Current Z: %d\n", ucCurrentZ);
    GUI_DEBUG_PRINT("  Controls Left: %d\n", ucControlsLeft);
}

//---------------------------------------------------------------------------
void GuiWindow::InvalidateRegion( K_USHORT usLeft_, K_USHORT usTop_, K_USHORT usWidth_, K_USHORT usHeight_ )
{
    LinkListNode *pclTempNode;
    K_USHORT usLeft1, usLeft2, usRight1, usRight2, usTop1, usTop2, usBottom1, usBottom2;

    pclTempNode = m_clControlList.GetHead();

    usLeft1 = usLeft_;
    usRight1 = usLeft_ + usWidth_ - 1;
    usTop1 = usTop_;
    usBottom1 = usTop_ + usHeight_ - 1;

    while (pclTempNode)
    {
        GuiControl *pclControl = static_cast<GuiControl*>(pclTempNode);
        K_USHORT usX, usY;

        bool bMatch = false;

        // Get the absolute display coordinates
        pclControl->GetControlOffset(&usX, &usY);


        usLeft2 = pclControl->GetLeft() + usX;
        usRight2 = usLeft2 + pclControl->GetWidth() - 1;
        usTop2 = pclControl->GetTop() + usY;
        usBottom2 = usTop2 + pclControl->GetHeight() - 1;

        // If the control has any pixels in the bounding box.
        if (
                (
                    (
                        (usLeft1 >= usLeft2) &&
                        (usLeft1 <= usRight2)
                    ) ||
                    (
                        (usRight1 >= usLeft2) &&
                        (usRight1 <= usRight2)
                    ) ||
                    ((usLeft1 <= usLeft2) && (usRight1 >= usRight2))
                ) &&
                (
                    (
                        (usTop1 >= usTop2) &&
                        (usTop1 <= usBottom2)
                    ) ||
                    (
                        (usBottom1 >= usTop2) &&
                        (usBottom1 <= usBottom2)
                    ) ||
                    ((usTop1 <= usTop2) && (usBottom1 >= usBottom2))
                )
            )
        {
            bMatch = true;            
        }
        else if(
                (
                    (
                        (usLeft2 >= usLeft1) &&
                        (usLeft2 <= usRight1)
                    ) ||
                    (
                        (usRight2 >= usLeft1) &&
                        (usRight2 <= usRight1)
                    ) ||
                    ((usLeft2 <= usLeft1) && (usRight2 >= usRight1))
                ) &&
                (
                    (
                        (usTop2 >= usTop1) &&
                        (usTop2 <= usBottom1)
                    ) ||
                    (
                        (usBottom2 >= usTop1) &&
                        (usBottom2 <= usBottom1)
                    ) ||
                    ((usTop2 <= usTop1) && (usBottom2 >= usBottom1))
                )
            )
        {
            bMatch = true;            
        }


        if (bMatch)
        {
            pclControl->SetStale();

            // Invalidate all child controls as well (since redrawing a parent could cause them to disappear)
            GuiControl *pclChild = static_cast<GuiControl*>(m_clControlList.GetHead());

            // Go through all controls and check for parental ancestry
            while (pclChild)
            {
                GuiControl *pclParent = static_cast<GuiControl*>(pclChild->GetParentControl());

                // If this control is a descendant of the current control at some level
                while (pclParent)
                {
                    if (pclParent == pclControl)
                    {
                        // Set the control as stale
                        pclChild->SetStale();
                        break;
                    }
                    pclParent = pclParent->GetParentControl();
                }

                pclChild = static_cast<GuiControl*>((static_cast<LinkListNode*>(pclChild))->GetNext());
            }
        }

        pclTempNode = pclTempNode->GetNext();
    }
}

//---------------------------------------------------------------------------
void GuiWindow::ProcessEvent( GuiEvent_t *pstEvent_ )
{
    GUI_DEBUG_PRINT("GuiWindow::ProcessEvent\n");

    // If the event is for broadcast - send it to all controls,
    // without regard to order.
    if ((TARGET_ID_BROADCAST == pstEvent_->ucTargetID)
        || (TARGET_ID_BROADCAST_Z == pstEvent_->ucTargetID))
    {
        GUI_DEBUG_PRINT("  TARGET_ID_BROADCAST(_Z)\n");

        LinkListNode *pclTempNode;
        pclTempNode = m_clControlList.GetHead();

        while (pclTempNode)
        {
            GuiReturn_t eRet;
            eRet = (static_cast<GuiControl*>(pclTempNode))->ProcessEvent(pstEvent_);
            if (GUI_EVENT_CONSUMED == eRet)
            {
                break;
            }
            pclTempNode = pclTempNode->GetNext();
        }
    }
    // Send the event only to the currently-selected object.
    else if (TARGET_ID_FOCUS == pstEvent_->ucTargetID)
    {
        GUI_DEBUG_PRINT("  TARGET_ID_FOCUS\n");
        GuiReturn_t eReturn = GUI_EVENT_OK;

        // Try to let the control process the event on its own
        if (m_pclInFocus)
        {
            eReturn = m_pclInFocus->ProcessEvent(pstEvent_);
        }

        // If the event was not consumed, use default logic to process the event
        if (GUI_EVENT_CONSUMED != eReturn)
        {
            if (EVENT_TYPE_KEYBOARD == pstEvent_->ucEventType)
            {
                if (KEYCODE_TAB == pstEvent_->stKey.ucKeyCode)
                {
                    if (pstEvent_->stKey.bKeyState)
                    {
                        CycleFocus(true);
                    }
                }
            }
            else if (EVENT_TYPE_JOYSTICK == pstEvent_->ucEventType)
            {
                if (pstEvent_->stJoystick.bUp || pstEvent_->stJoystick.bLeft)
                {
                    // Cycle focus *backwards*
                    CycleFocus(false);
                }
                else if (pstEvent_->stJoystick.bRight || pstEvent_->stJoystick.bDown)
                {
                    // Cycle focus *forewards*
                    CycleFocus(true);
                }
            }
        }
    }
    else if (TARGET_ID_HIGH_Z == pstEvent_->ucTargetID)
    {
        GUI_DEBUG_PRINT("  TARGET_ID_HIGH_Z\n");

        K_USHORT usTargetX, usTargetY;
        K_USHORT usOffsetX, usOffsetY;
        K_UCHAR ucMaxZ = 0;

        LinkListNode *pclTempNode;
        pclTempNode = m_clControlList.GetHead();

        switch (pstEvent_->ucEventType)
        {
            case EVENT_TYPE_MOUSE:
            case EVENT_TYPE_TOUCH:
            {
                GuiControl *pclTargetControl = NULL;

                // Read the target X/Y coordinates out of the event struct
                if (EVENT_TYPE_TOUCH == pstEvent_->ucEventType)
                {
                    usTargetX = pstEvent_->stTouch.usX;
                    usTargetY = pstEvent_->stTouch.usY;
                }
                else
                {
                    usTargetX = pstEvent_->stMouse.usX;
                    usTargetY = pstEvent_->stMouse.usY;
                }

                // Go through every control on the window, checking to see if the
                // event falls within the bounding box
                   while (pclTempNode)
                {
                    GuiControl *pclControl = (static_cast<GuiControl*>(pclTempNode));

                    pclControl->GetControlOffset(&usOffsetX, &usOffsetY);

                    // Compare event coordinates to bounding box (with offsets)
                    if ( ((usTargetX >= (usOffsetX + pclControl->GetLeft()) &&
                          (usTargetX <= (usOffsetX + pclControl->GetLeft() + pclControl->GetWidth() - 1)))) &&
                         ((usTargetY >= (usOffsetY + pclControl->GetTop()) &&
                          (usTargetY <= (usOffsetY + pclControl->GetTop() + pclControl->GetHeight() - 1)))) )
                    {
                        // If this control is higher in Z-Order, set this as the newest
                        // candidate control to accept the event
                        if (pclControl->GetZOrder() >= ucMaxZ)
                        {
                            pclTargetControl = pclControl;
                            ucMaxZ = pclControl->GetZOrder();
                        }
                    }

                    pclTempNode = pclTempNode->GetNext();
                }

                // If a suitable control was found on the event surface, pass the event off
                // for processing.
                if (pclTargetControl)
                {
                    // If the selected control is different from the current in-focus
                    // control, then deactive that control.
                    if (m_pclInFocus && (m_pclInFocus != pclTargetControl))
                    {
                        m_pclInFocus->Activate(false);
                        m_pclInFocus = NULL;
                    }
                    (static_cast<GuiControl*>(pclTargetControl))->ProcessEvent(pstEvent_);
                }
            }
                break;
            default:
                break;
        }
    }
}
//---------------------------------------------------------------------------
void GuiWindow::SetFocus( GuiControl *pclControl_ )
{
    GUI_DEBUG_PRINT("GuiWindow::SetFocus\n");

    m_pclInFocus = pclControl_;
}

//---------------------------------------------------------------------------
void GuiWindow::CycleFocus( bool bForward_ )
{
    GUI_DEBUG_PRINT("GuiWindow::CycleFocus\n");

    // Set starting point and cached copy of current nodes
    LinkListNode *pclTempNode = static_cast<GuiControl*>(m_clControlList.GetHead());
    LinkListNode *pclStartNode = m_pclInFocus;

    if (bForward_)
    {
        // If there isn't a current focus node, set the focus to the beginning
        // of the list
        if (!m_pclInFocus)
        {
            m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
            if (!m_pclInFocus)
            {
                return;
            }
            pclTempNode = static_cast<GuiControl*>(m_pclInFocus);
            pclStartNode = NULL;
        }
        else
        {
            // Deactivate the control that's losing focus
            static_cast<GuiControl*>(m_pclInFocus)->Activate(false);

            // Otherwise start with the next node
            pclStartNode = pclStartNode->GetNext();
        }

        // Go through the whole control list and find the next one to accept
        // the focus
        while (pclTempNode && pclTempNode != pclStartNode)
        {
            if (static_cast<GuiControl*>(pclTempNode)->AcceptsFocus())
            {
                m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
                m_pclInFocus->Activate(true);
                SetFocus(m_pclInFocus);
                return;
            }
            pclTempNode = pclTempNode->GetNext();
        }

        pclTempNode = static_cast<GuiControl*>(m_clControlList.GetHead());
        while (pclTempNode && pclTempNode != pclStartNode)
        {
            if (static_cast<GuiControl*>(pclTempNode)->AcceptsFocus())
            {
                m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
                m_pclInFocus->Activate(true);
                SetFocus(m_pclInFocus);
                return;
            }
            pclTempNode = pclTempNode->GetNext();
        }
    }
    else
    {
        pclTempNode = static_cast<GuiControl*>(m_clControlList.GetTail());
        pclStartNode = m_pclInFocus;

        // If there isn't a current focus node, set the focus to the end
        // of the list
        if (!m_pclInFocus)
        {
            m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
            if (!m_pclInFocus)
            {
                return;
            }
            pclTempNode = static_cast<GuiControl*>(m_pclInFocus);
            pclStartNode = NULL;
        }
        else
        {
            // Deactivate the control that's losing focus
            static_cast<GuiControl*>(m_pclInFocus)->Activate(false);

            // Otherwise start with the previous node
            pclStartNode = pclStartNode->GetPrev();
        }

        // Go through the whole control list and find the next one to accept
        // the focus
        while (pclTempNode && pclTempNode != pclStartNode)
        {
            if (static_cast<GuiControl*>(pclTempNode)->AcceptsFocus())
            {
                m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
                m_pclInFocus->Activate(true);
                SetFocus(m_pclInFocus);
                return;
            }
            pclTempNode = pclTempNode->GetPrev();
        }

        pclTempNode = static_cast<GuiControl*>(m_clControlList.GetTail());
        while (pclTempNode && pclTempNode != pclStartNode)
        {
            if (static_cast<GuiControl*>(pclTempNode)->AcceptsFocus())
            {
                m_pclInFocus = static_cast<GuiControl*>(pclTempNode);
                m_pclInFocus->Activate(true);
                SetFocus(m_pclInFocus);
                return;
            }
            pclTempNode = pclTempNode->GetPrev();
        }
    }
}
//---------------------------------------------------------------------------
GuiWindow *GuiEventSurface::FindWindowByName( const K_CHAR *szName_ )
{
    LinkListNode *pclTempNode = static_cast<LinkListNode*>(m_clWindowList.GetHead());

    while (pclTempNode)
    {
        if (MemUtil::CompareStrings(szName_, static_cast<GuiWindow*>(pclTempNode)->GetName()))
        {
             return static_cast<GuiWindow*>(pclTempNode);
        }
        pclTempNode = pclTempNode->GetNext();
    }

    return NULL;
}

//---------------------------------------------------------------------------
void GuiEventSurface::AddWindow( GuiWindow *pclWindow_ )
{
    GUI_DEBUG_PRINT("GuiEventSurface::AddWindow\n");

    m_clWindowList.Add(static_cast<LinkListNode*>(pclWindow_));
}

//---------------------------------------------------------------------------
void GuiEventSurface::RemoveWindow( GuiWindow *pclWindow_ )
{
    GUI_DEBUG_PRINT("GuiEventSurface::RemoveWindow\n");

    m_clWindowList.Remove(static_cast<LinkListNode*>(pclWindow_));
}

//---------------------------------------------------------------------------
K_BOOL GuiEventSurface::SendEvent( GuiEvent_t *pstEvent_ )
{
    GUI_DEBUG_PRINT("GuiEventSurface::SendEvent\n");

    // Allocate a message from the global message pool
    Message *pclMessage = GlobalMessagePool::Pop();

    // No messages available? Return a failure
    if (!pclMessage)
    {
        return false;
    }

    // Allocate a copy of the event from the heap
    GuiEvent_t *pstEventCopy = static_cast<GuiEvent_t*>(SystemHeap::Alloc(sizeof(GuiEvent_t)));

    // If the allocation fails, push the message back to the global pool and bail
    if (!pstEventCopy)
    {
        GlobalMessagePool::Push(pclMessage);
        return false;
    }

    // Copy the source event into the destination event buffer
    CopyEvent(pstEventCopy, pstEvent_);

    // Set the new event as the message payload
    pclMessage->SetData(static_cast<void*>(pstEventCopy));

    // Send the event to the message queue
    m_clMessageQueue.Send(pclMessage);

    return true;
}

//---------------------------------------------------------------------------
K_BOOL GuiEventSurface::ProcessEvent()
{
    GUI_DEBUG_PRINT("GuiEventSurface::ProcessEvent\n");

    // read the event from the queue (blocking call)
    Message *pclMessage = m_clMessageQueue.Receive();
    GuiEvent_t stLocalEvent;

    // If we failed to get something from the queue,
    // bail out
    if (!pclMessage)
    {
        return false;
    }

    // Copy the event data from the message into a local copy
    CopyEvent(&stLocalEvent,
        static_cast<GuiEvent_t*>(pclMessage->GetData()));

    // Free the message and event as soon as possible, since
    // they are shared system resources
    SystemHeap::Free(pclMessage->GetData());
    GlobalMessagePool::Push(pclMessage);

    // Special case check - target ID is the highest Z-ordered window(s) ONLY.
    if (stLocalEvent.ucTargetID == TARGET_ID_BROADCAST_Z)
    {
        LinkListNode* pclTempNode = m_clWindowList.GetHead();
        K_UCHAR ucMaxZ = 0;

        while (pclTempNode)
        {
            if (ucMaxZ < (static_cast<GuiWindow*>(pclTempNode))->GetZOrder() )
            {
                ucMaxZ = static_cast<GuiWindow*>(pclTempNode)->GetZOrder();
            }
            pclTempNode = pclTempNode->GetNext();
        }

        // Iterate through all windows again - may have multiple windows
        // at the same z-order.
        pclTempNode = m_clWindowList.GetHead();
        while (pclTempNode)
        {
            if (ucMaxZ == (static_cast<GuiWindow*>(pclTempNode))->GetZOrder())
            {
                (static_cast<GuiWindow*>(pclTempNode))->ProcessEvent(&stLocalEvent);
            }
            pclTempNode = pclTempNode->GetNext();
        }
    }
    // Broadcast the event - sending it to *all* windows.  Let the individual
    // windows figure out what to do with the events.
    else
    {
        LinkListNode* pclTempNode = m_clWindowList.GetHead();
        while (pclTempNode)
        {
            (static_cast<GuiWindow*>(pclTempNode))->ProcessEvent(&stLocalEvent);
            pclTempNode = pclTempNode->GetNext();
        }
    }

    // Return out
    return true;
}

//---------------------------------------------------------------------------
void GuiEventSurface::CopyEvent( GuiEvent_t *pstDst_, GuiEvent_t *pstSrc_ )
{
    GUI_DEBUG_PRINT("GuiEventSurface::CopyEvent\n");
    K_UCHAR *pucDst_ = (K_UCHAR*)pstDst_;
    K_UCHAR *pucSrc_ = (K_UCHAR*)pstSrc_;
    K_UCHAR i;
    for (i = 0; i < sizeof(GuiEvent_t); i++)
    {
        *pucDst_++ = *pucSrc_++;
    }
}

//---------------------------------------------------------------------------
void GuiEventSurface::InvalidateRegion( K_USHORT usLeft_, K_USHORT usTop_, K_USHORT usWidth_, K_USHORT usHeight_ )
{
    LinkListNode* pclTempNode = m_clWindowList.GetHead();
    while (pclTempNode)
    {
        (static_cast<GuiWindow*>(pclTempNode))->InvalidateRegion(usLeft_, usTop_, usWidth_, usWidth_);
        pclTempNode = pclTempNode->GetNext();
    }
}

//---------------------------------------------------------------------------
void GuiControl::GetControlOffset( K_USHORT *pusX_, K_USHORT *pusY_ )
{
    GUI_DEBUG_PRINT("GuiControl::GetControlOffset\n");
    GuiControl *pclTempControl = m_pclParentControl;
    *pusX_ = 0;
    *pusY_ = 0;
    while (pclTempControl)
    {
        *pusX_ += pclTempControl->GetLeft();
        *pusY_ += pclTempControl->GetTop();
        pclTempControl = pclTempControl->GetParentControl();
    }

    if (m_pclParentWindow)
    {
        *pusX_ += m_pclParentWindow->GetLeft();
        *pusY_ += m_pclParentWindow->GetTop();
    }
}
