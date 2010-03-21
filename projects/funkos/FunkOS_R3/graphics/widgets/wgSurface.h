#ifndef __WIDGET_SURFACE_H_
#define __WIDGET_SURFACE_H_

#include "types.h"
#include "fooey.h"

//--------------------------------------------------------------------------- 
typedef struct
{
	GUI_BASE_STRUCT stWidget;		//!< Always first (inheritence)
	COLOR cBGColor;					//!< Background color of the surface
} WIDGET_SURFACE;
//--------------------------------------------------------------------------- 
void WgSurface_Init(void *pstThis_);
void WgSurface_Draw(void *pstThis_);
void WgSurface_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_);
void WgSurface_Destroy(void *pstThis_);

#endif