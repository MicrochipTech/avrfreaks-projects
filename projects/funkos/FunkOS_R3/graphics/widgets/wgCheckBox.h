#ifndef __WIDGET_CHECKBOX_H_
#define __WIDGET_CHECKBOX_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

//---------------------------------------------------------------------------
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	CHAR *szCaption;				//!< Pointer to the caption text
	FONT_STRUCT *pstFont;			//!< Font to use
	COLOR cBGColor;					//!< Background color
    COLOR cCheckColor;              //!< Color of the check
    COLOR cBoxColor;                //!< BG Color of the box
	COLOR cTextColor;				//!< Font color
    COLOR cActiveColor;             //!< Color when activef
    BOOL bChecked;                  //!< Whether or not the button is clicked or not
} WIDGET_CHECKBOX;
//--------------------------------------------------------------------------- 
void WgCheckBox_Init(void *pstThis_);
void WgCheckBox_Draw(void *pstThis_);
void WgCheckBox_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgCheckBox_Destroy(void *pstThis_);

#endif