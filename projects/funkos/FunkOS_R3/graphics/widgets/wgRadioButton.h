#ifndef __WIDGET_RADIOBUTTON_H_
#define __WIDGET_RADIOBUTTON_H_

#include "types.h"
#include "fooey.h"
#include "font.h"

//---------------------------------------------------------------------------
typedef struct __Widget_RadioButton
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

    struct __Widget_RadioButton *pstNext;   //!< Next button in the array
    struct __Widget_RadioButton *pstPrev;   //!< Previous button in the array

} WIDGET_RADIOBUTTON;
//--------------------------------------------------------------------------- 
void WgRadioButton_Init(void *pstThis_);
void WgRadioButton_Draw(void *pstThis_);
void WgRadioButton_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgRadioButton_Destroy(void *pstThis_);

#endif