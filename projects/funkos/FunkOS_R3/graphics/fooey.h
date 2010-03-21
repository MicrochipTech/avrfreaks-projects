//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  fooey.h

	Description:
	The Fooey GUI library - structure and function definitions
*/
//---------------------------------------------------------------------------

#ifndef __FOOEY_H__
#define __FOOEY_H__

#include "types.h"
#include "display.h"
#include "colorspace.h"
#include "fooey_event.h"

//---------------------------------------------------------------------------
#define WIDGET_DEST_BROADCAST       (0xFFFF)
#define WIDGET_DEST_ACTIVE          (0xFFFE)
#define WIDGET_DEST_AUTO            (0xFFFD)

//---------------------------------------------------------------------------
// Base function types used in the UI heirarchy
typedef void (*WIDGET_INIT)(void *pstThis_);
typedef void (*WIDGET_DRAW)(void *pstThis_);
typedef void (*WIDGET_CONTROL)(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
typedef void (*WIDGET_DESTROY)(void *pstThis_);

//--------------------------------------------------------------------------- 
typedef struct __gui_base_struct
{
	USHORT 		usLeft;			//!< Define the position and size of a control
	USHORT 		usTop;
	USHORT 		usRight;
	USHORT 		usBottom;

	USHORT 		usCtrlID;		//!< Define a unique ID for each control

	UCHAR 		ucGroup;		//!< Depth grouping
	UCHAR   	eType;			//!< Define the behaviour of the control when specialized

	BOOL 		bRedraw;		//!< Whether or not the target needs to be redrawn
	BOOL		bHasFocus;		//!< Whether or not this is the active control

	WIDGET_INIT		pfInit;		//!< Initializer function
	WIDGET_DRAW 	pfDraw;		//!< Function to render the control
	WIDGET_CONTROL	pfControl;	//!< Event handler for the control (universal input behaviour)
	WIDGET_DESTROY  pfDestroy;	//!< Destructor function

	void    *pstOwner;	        //!< Pointer back to the GUI manager
	struct __gui_base_struct *pstParent; //!< pointer to the direct parent

} GUI_BASE_STRUCT;

//--------------------------------------------------------------------------- 
typedef struct
{
	FOOEY_EVENT_STRUCT *pstEvents;	//!< pointer to array of events
	UCHAR ucSize;					//!< size of event data array
	UCHAR ucHead;					//!< Head index of circular buffer
	UCHAR ucTail;					//!< Tail index of circular buffer
} GUI_EVENT_QUEUE_STRUCT;

//---------------------------------------------------------------------------
typedef struct
{
	DISPLAY_DRIVER_STRUCT *pstDisplay;	//!< Video device associated with the user interface
	GUI_EVENT_QUEUE_STRUCT stEvents;	//!< Pointer to an event struct
	GUI_BASE_STRUCT **pastScene;		//!< Pointer to a list of widgets
	USHORT usNumWidgets;				//!< Number of widgets allowed in this manager
	ULONG ulCurrentID;					//!< Index of control with focus
	UCHAR ucMaxGroup;					//!< Max group
} GUI_MANAGER_STRUCT;

//---------------------------------------------------------------------------
void Fooey_InitDisplay(	GUI_MANAGER_STRUCT *pstManager_, 
						DISPLAY_DRIVER_STRUCT *pstDisplay_,
						FOOEY_EVENT_STRUCT *pstEvents_,
						UCHAR ucNumEvents_,
						GUI_BASE_STRUCT *astWidgets_[],						
						USHORT usNumWidgets_);

BOOL Fooey_AddWidget(	GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_,
                        UCHAR ucGroup_,
						UCHAR eType_);

BOOL Fooey_RemoveWidget(GUI_MANAGER_STRUCT *pstManager_,
						GUI_BASE_STRUCT *pstWidget_);

void Fooey_RenderAll(GUI_MANAGER_STRUCT *pstManager_);
void Fooey_RenderChanged(GUI_MANAGER_STRUCT *pstManager_);

BOOL Fooey_SendEvent(GUI_MANAGER_STRUCT *pstManager_, FOOEY_EVENT_STRUCT *pstEvent_);
void Fooey_ProcessEvents(GUI_MANAGER_STRUCT *pstManager_);

void Fooey_GetControlOffsets(GUI_BASE_STRUCT *pstWidget_, USHORT *pusX_, USHORT *pusY_);
void Fooey_InitWidget(GUI_BASE_STRUCT *pstWidget_);
void Fooey_ResetFocus(GUI_MANAGER_STRUCT *pstManager_);

//---------------------------------------------------------------------------

#endif 