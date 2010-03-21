#ifndef __WIDGET_LABEL_H_
#define __WIDGET_LABEL_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

//--------------------------------------------------------------------------- 
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	CHAR *szCaption;				//!< Pointer to the text to display
	FONT_STRUCT *pstFont;			//!< Font to use
	COLOR cBGColor;					//!< Background color	
	COLOR cTextColor;				//!< Text color
	BOOL bUseBG;					//!< Whether or not to draw the BG
} WIDGET_LABEL;
//--------------------------------------------------------------------------- 
void WgLabel_Init(void *pstThis_);
void WgLabel_Draw(void *pstThis_);
void WgLabel_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgLabel_Destroy(void *pstThis_);

#endif