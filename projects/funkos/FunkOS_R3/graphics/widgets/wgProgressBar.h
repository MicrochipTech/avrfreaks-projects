#ifndef __WIDGET_PROGRESSBAR_H_
#define __WIDGET_PROGRESSBAR_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

//---------------------------------------------------------------------------
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	FONT_STRUCT *pstFont;			//!< Font to use
    UCHAR ucProgress;               //!< Percent complete
	COLOR cBGColor;					//!< Background color
	COLOR cLineColor;				//!< Border color
	COLOR cFillColor;				//!< Border color
	COLOR cTextColor;				//!< Font color
} WIDGET_PROGRESSBAR;
//---------------------------------------------------------------------------
void WgProgressBar_Init(void *pstThis_);
void WgProgressBar_Draw(void *pstThis_);
void WgProgressBar_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgProgressBar_Destroy(void *pstThis_);

#endif