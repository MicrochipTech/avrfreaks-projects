#ifndef __WIDGET_BUTTON_H_
#define __WIDGET_BUTTON_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

//---------------------------------------------------------------------------
typedef void (*WG_BUTTON_CALLBACK)(void);

//--------------------------------------------------------------------------- 
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	CHAR *szCaption;				//!< Pointer to the caption text
	FONT_STRUCT *pstFont;			//!< Font to use
	COLOR cBGColor;					//!< Background color	
	COLOR cBGActiveColor;			//!< Color when active
	COLOR cLineColor;				//!< Line Color
	COLOR cTextColor;				//!< Font color
	WG_BUTTON_CALLBACK pfCallback;	//!< Button callback function
    BOOL bState;                    //!< Whether or not the button is clicked or not
} WIDGET_BUTTON;
//--------------------------------------------------------------------------- 
void WgButton_Init(void *pstThis_);
void WgButton_Draw(void *pstThis_);
void WgButton_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgButton_Destroy(void *pstThis_);

#endif