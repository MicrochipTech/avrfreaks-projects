#ifndef __WIDGET_EDITBOX_H_
#define __WIDGET_EDITBOX_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

#define EDITBOX_SIZE        (24)
//---------------------------------------------------------------------------
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	CHAR szCaption[EDITBOX_SIZE];   //!< Pointer to the caption text
	FONT_STRUCT *pstFont;			//!< Font to use
	COLOR cBGColor;					//!< Background color
	COLOR cLineColor;				//!< Border color
	COLOR cActiveColor;				//!< Border color (selected)
	COLOR cTextColor;				//!< Font color
} WIDGET_EDITBOX;
//--------------------------------------------------------------------------- 
void WgEditBox_Init(void *pstThis_);
void WgEditBox_Draw(void *pstThis_);
void WgEditBox_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgEditBox_Destroy(void *pstThis_);

#endif