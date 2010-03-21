//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  fooey.c

	Description:
	The Fooey GUI library - panel and event management
*/

#include "fooey.h"
#include "widgets.h"

//---------------------------------------------------------------------------
#define CS_ENTER()  {} //! TODO: Include either protect.h or taskport.h
#define CS_EXIT()   {}

//---------------------------------------------------------------------------
/*!
    Initializes an instance of a Fooey GUI.

    \fn void Fooey_InitDisplay(	GUI_MANAGER_STRUCT *pstManager_,
						DISPLAY_DRIVER_STRUCT *pstDisplay_,
						FOOEY_EVENT_STRUCT *pstEvents_,
						UCHAR ucNumEvents_,
						GUI_BASE_STRUCT *astWidgets_[],
						USHORT usNumWidgets_)
    \param pstManager_ - Pointer to the GUI manager base struct
	\param pstDisplay_ - pointer to the display driver to use with this GUI instance
    \param pstEvents_ - pointer to the events buffer for the UI
    \param ucNumEvents_ - Number of events in the events buffer
    \param astWidgets_ - Array of pointers to the widgets managed in this UI
    \param usNumWidgets_ - Number of elements in this instance
*/

void Fooey_InitDisplay(	GUI_MANAGER_STRUCT *pstManager_,
						DISPLAY_DRIVER_STRUCT *pstDisplay_,
						FOOEY_EVENT_STRUCT *pstEvents_,
						UCHAR ucNumEvents_,
						GUI_BASE_STRUCT *astWidgets_[],
						USHORT usNumWidgets_)
{
	USHORT i;

	// Set the display driver associated with the UI panel
	pstManager_->pstDisplay = pstDisplay_;

	// Clear the widget list
	for (i = 0; i < usNumWidgets_; i++)
	{
		astWidgets_[i] = NULL;
	}

	// Add the widget list to the manager
	pstManager_->pastScene = astWidgets_;

	// Set the max number of widgets.
	pstManager_->usNumWidgets = usNumWidgets_;

	// Initialize the event queue
	pstManager_->stEvents.pstEvents = pstEvents_;
	pstManager_->stEvents.ucSize = ucNumEvents_;
	pstManager_->stEvents.ucHead = 0;
	pstManager_->stEvents.ucTail = 0;
}
//---------------------------------------------------------------------------
/*!
    Adds a widget to a Fooey GUI, initilizing and specializing the control
    based on the type of widget specified.  Once associated with the GUI, it is managed
    automatcially.

    \fn BOOL Fooey_AddWidget(	GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_,
						UCHAR ucGroup_,
						UCHAR eType_)
    \param pstManager_ - Pointer to the GUI manager base struct
    \param pstWidget_ - Pointer to the widget to add to the managed GUI
    \param ucGroup_ - The control group priority level for drawing (high = front, low = back)
    \param eType_ - The control's type
    \return TRUE on success, FALSE if the GUI manager is full.
*/

BOOL Fooey_AddWidget(	GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_,
						UCHAR ucGroup_,
						UCHAR eType_)
{
	USHORT i;
	BOOL bReturn = FALSE;
	// find the first vacant element in the list, and add the control
	// at that index.
	for (i = 0; i < pstManager_->usNumWidgets; i++)
	{
		// empty element...
		if (pstManager_->pastScene[i] == NULL)
		{
			// Assign the relevant data.
			pstManager_->pastScene[i] = pstWidget_;
			pstManager_->pastScene[i]->ucGroup = ucGroup_;
			if (ucGroup_ > pstManager_->ucMaxGroup)
			{
				pstManager_->ucMaxGroup = ucGroup_;
			}
			pstManager_->pastScene[i]->bRedraw = TRUE;
			pstManager_->pastScene[i]->bHasFocus = FALSE;
			pstManager_->pastScene[i]->pfInit = g_apfInit[eType_];
			pstManager_->pastScene[i]->pfDraw = g_apfDraw[eType_];
			pstManager_->pastScene[i]->pfControl = g_apfControl[eType_];
			pstManager_->pastScene[i]->pfDestroy = g_apfDestroy[eType_];
			pstManager_->pastScene[i]->pstOwner = pstManager_;
			pstManager_->pastScene[i]->eType = eType_;
			bReturn = TRUE;
			break;
		}
	}
	return bReturn;
}
//---------------------------------------------------------------------------
/*!
    Removes a widget from a Fooey GUI.  Once a control is removed, it is no
    longer managed by the GUI.

    \fn BOOL Fooey_RemoveWidget(GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_)
    \param pstManager_ - Pointer to the GUI manager base struct
    \param pstWidget_ - Pointer to the widget to remove from the managed GUI
    \return TRUE on success, FALSE if the widget is not found
*/

BOOL Fooey_RemoveWidget(GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_)
{
	USHORT i;
	BOOL bReturn = FALSE;

	// If the widget matches one in the list, remove it.
	for (i = 0; i < pstManager_->usNumWidgets; i++)
	{
		if (pstManager_->pastScene[i] == pstWidget_)
		{
			pstManager_->pastScene[i] = NULL;
			bReturn = TRUE;
			break;
		}
	}
	return bReturn;
}
//---------------------------------------------------------------------------
/*!
    Renders (draws) all GUI elements associated with a UI from lowest group to highest
    group.  Resets the "bRedraw" property of each control afterward.

    \fn void Fooey_RenderAll(GUI_MANAGER_STRUCT *pstManager_)
    \param pstManager_ - pointer to the GUI manager containing the controls to render
*/

void Fooey_RenderAll(GUI_MANAGER_STRUCT *pstManager_)
{
	USHORT i;

	// Set the draw flags for each control
	for (i = 0; i < pstManager_->usNumWidgets; i++)
	{
		if (pstManager_->pastScene[i] != NULL)
		{
			pstManager_->pastScene[i]->bRedraw = TRUE;
		}
	}
	// Render all controls
	Fooey_RenderChanged(pstManager_);
}
//---------------------------------------------------------------------------
/*!
    Renders (draws) all of the GUI elements associated with the UI that have
    their bRedraw property set to TRUE.

    \fn void Fooey_RenderChanged(GUI_MANAGER_STRUCT *pstManager_)
    \param pstManager_ - pointer to the GUI manager containing the controls to render
*/

void Fooey_RenderChanged(GUI_MANAGER_STRUCT *pstManager_)
{
	USHORT i;
	USHORT usCtrlsToDraw;
	USHORT usCtrlsDrawn;
	UCHAR ucGroup;

	usCtrlsToDraw = 0;
	usCtrlsDrawn = 0;

	// Figure out how many controls are scheduled for update
	for (i = 0; i < pstManager_->usNumWidgets; i++)
	{
		if (pstManager_->pastScene[i] != NULL)
		{
			if (pstManager_->pastScene[i]->bRedraw == TRUE)
			{
				usCtrlsToDraw++;
			}
		}
	}
	
	// Go through the group list from low->high until we've drawn
	// every control in the update list
	for (ucGroup = 0; ucGroup <= pstManager_->ucMaxGroup; ucGroup++)
	{
		for (i = 0; i < pstManager_->usNumWidgets; i++)
		{
			// Only draw controls that aren't null, are in the current
			// group and have their redraw flag set TRUE.		
			if (pstManager_->pastScene[i] != NULL)
			{
				if (pstManager_->pastScene[i]->ucGroup == ucGroup)
				{
					if (pstManager_->pastScene[i]->bRedraw == TRUE)
					{
						pstManager_->pastScene[i]->pfDraw(pstManager_->pastScene[i]);
						pstManager_->pastScene[i]->bRedraw = FALSE;
						usCtrlsDrawn++;
					}
				}
			}
		}
		// If all controls have been drawn, bail out.
		if (usCtrlsToDraw == usCtrlsDrawn)
		{
			break;
		}
	}
}
//---------------------------------------------------------------------------
/*!
    Buffers an event to the GUI manager's event queue, which will be processed
    by a call to Fooey_ProcessEvents().

    \fn BOOL Fooey_SendEvent(GUI_MANAGER_STRUCT *pstManager_, FOOEY_EVENT_STRUCT *pstEvent_)
    \param pstManager_ - pointer to the GUI manager containing the controls to render
    \param pstEvent_ - pointer to the event to queue.  This is copied in, so the
                        event need not be persistent.
    \return TRUE on successful buffering, FALSE on buffer full
*/

BOOL Fooey_SendEvent(GUI_MANAGER_STRUCT *pstManager_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	UCHAR ucNextHead;
	BOOL bReturn = FALSE;

	// Protected block
	CS_ENTER();

	// What's the next head index?
	ucNextHead = pstManager_->stEvents.ucHead + 1;
	if (ucNextHead >= pstManager_->stEvents.ucSize)
	{
		ucNextHead = 0;
	}

	// If the next head index is the tail index, we're full, so bail
	if (ucNextHead == pstManager_->stEvents.ucTail)
	{
		bReturn = FALSE;
	}
	else
	{
		// We have a free spot - fill data and return TRUE
		pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucHead].usEventID = pstEvent_->usEventID;
		pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucHead].usEventTime = pstEvent_->usEventTime;
		pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucHead].EVENT_DATA.ulRawData = pstEvent_->EVENT_DATA.ulRawData;
  		pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucHead].usDest = pstEvent_->usDest;
		pstManager_->stEvents.ucHead = ucNextHead;
		bReturn = TRUE;
	}

	//Protected block
	CS_EXIT();

	return bReturn;
}
//---------------------------------------------------------------------------
/*!
    Processes all queued events in the GUI manager's event buffer.

    \fn void Fooey_ProcessEvents(GUI_MANAGER_STRUCT *pstManager_)
    \param pstManager_ - pointer to the GUI manager containing the controls
*/

void Fooey_ProcessEvents(GUI_MANAGER_STRUCT *pstManager_)
{
    FOOEY_EVENT_STRUCT stEvent;
	UCHAR ucNextTail;
    USHORT i;

	// While there are events to process...
	while (pstManager_->stEvents.ucTail != pstManager_->stEvents.ucHead)
	{
		// Get the oldest item in the queue
		stEvent.usEventID = pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucTail].usEventID;
        stEvent.usEventTime = pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucTail].usEventTime;
   		stEvent.EVENT_DATA.ulRawData = pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucTail].EVENT_DATA.ulRawData;

		stEvent.usDest = pstManager_->stEvents.pstEvents[pstManager_->stEvents.ucTail].usDest;

		// Figure out who it's for (if not for everyone)
		if (stEvent.usDest == WIDGET_DEST_BROADCAST)
		{
            // it's for everyone
            for (i = 0; i < pstManager_->usNumWidgets; i++)
            {
                if (pstManager_->pastScene[i] != NULL)
                {
                    pstManager_->pastScene[i]->pfControl(pstManager_->pastScene[i], &stEvent);
                }
            }
 		}
		else if (stEvent.usDest == WIDGET_DEST_ACTIVE)
		{
			// Send to the active control
            for (i = 0; i < pstManager_->usNumWidgets; i++)
            {
                if (pstManager_->pastScene[i] != NULL)
                {
                    if (pstManager_->pastScene[i]->bHasFocus == TRUE)
                    {
                        pstManager_->pastScene[i]->pfControl(pstManager_->pastScene[i], &stEvent);
                        break;
                    }
                }
            }
		}
        else if (stEvent.usDest == WIDGET_DEST_AUTO)
        {
            // Send to the surface control - rely on its logic to determine what
            // control ends up getting the event.
            for (i = 0; i < pstManager_->usNumWidgets; i++)
            {
                if (pstManager_->pastScene[i] != NULL)
                {
                    if (pstManager_->pastScene[i]->eType == WIDGET_SURFACE_ID)
                    {
                        pstManager_->pastScene[i]->pfControl(pstManager_->pastScene[i], &stEvent);
                        break;
                    }
                }
            }
        }
		else
		{
            for (i = 0; i < pstManager_->usNumWidgets; i++)
            {
                if (pstManager_->pastScene[i] != NULL)
                {
                    if (pstManager_->pastScene[i]->usCtrlID == stEvent.usDest)
                    {
                        pstManager_->pastScene[i]->pfControl(pstManager_->pastScene[i], &stEvent);
                        break;
                    }
                }
            }
		}

		ucNextTail = pstManager_->stEvents.ucTail + 1;
		if (ucNextTail >= pstManager_->stEvents.ucSize)
		{
			ucNextTail = 0;
		}
		pstManager_->stEvents.ucTail = ucNextTail;
	}
}
//---------------------------------------------------------------------------
/*!
    Determine the X/Y offsets of a widget relative to the root node in the
    display (usually a surface widget).  Scans continuously until the base
    node is found.

    \fn void Fooey_GetControlOffsets(GUI_BASE_STRUCT *pstWidget_, USHORT *pusX_, USHORT *pusY_)
    \param pstWidget_ - pointer to the current widget
    \param pusX_ - pointer where the X offset of the control will be returned
    \param pusY_ - pointer where the Y offset of the control will be returned
*/

void Fooey_GetControlOffsets(GUI_BASE_STRUCT *pstWidget_, USHORT *pusX_, USHORT *pusY_)
{
    GUI_BASE_STRUCT *pstMyParent;
    USHORT usX, usY;
    *pusX_ = 0;
    *pusY_ = 0;
    usX = 0;
    usY = 0;
    // Go through the heirarchy and find the sum total of all X/Y Offsets.
    // (The top/bottom/left/right values are relative to the parent's X/Y locations).
    pstMyParent = pstWidget_->pstParent;
    while(pstMyParent != NULL)
    {
        usX += pstMyParent->usLeft;
        usY += pstMyParent->usTop;
        pstMyParent = pstMyParent->pstParent;
    }

    // Return the X/Y offsets to the control.
    *pusX_ = usX;
    *pusY_ = usY;
}
//---------------------------------------------------------------------------
/*!
    Initialize a widget that has been added to a GUI manager

    \fn void Fooey_InitWidget(GUI_BASE_STRUCT *pstWidget_)
    \param pstWidget_ - pointer to the current widget
*/

void Fooey_InitWidget(GUI_BASE_STRUCT *pstWidget_)
{
    if (pstWidget_ != NULL)
    {
        pstWidget_->pfInit(pstWidget_);
    }
}
//---------------------------------------------------------------------------
/*!
    Resets the focus to false for all controls managed by a current UI manager
    struct.

    \fn void Fooey_ResetFocus(GUI_MANAGER_STRUCT *pstManager_)
    \param pstManager_ - pointer to the GUI manager struct
*/

void Fooey_ResetFocus(GUI_MANAGER_STRUCT *pstManager_)
{
    USHORT i;
    for (i = 0; i < pstManager_->usNumWidgets; i++)
    {
        if (pstManager_->pastScene[i] != NULL)
        {
            if (pstManager_->pastScene[i]->bHasFocus == TRUE)
            {
                pstManager_->pastScene[i]->bHasFocus = FALSE;
                pstManager_->pastScene[i]->bRedraw = TRUE;
            }
        }
    }
}
//----------------------------------------------------------------------------

