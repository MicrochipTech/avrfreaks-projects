#ifndef __WIDGETS_H_
#define __WIDGETS_H_

#include "widgetcfg.h"
//---------------------------------------------------------------------------
//!! User defined controls
#include "wgSurface.h"
#include "wgLabel.h"
#include "wgButton.h"
#include "wgEditBox.h"
#include "wgProgressBar.h"
#include "wgCheckBox.h"
#include "wgRadioButton.h"
//---------------------------------------------------------------------------
typedef enum
{
    WIDGET_SURFACE_ID,
    WIDGET_LABEL_ID,
    WIDGET_BUTTON_ID,
    WIDGET_EDITBOX_ID,
    WIDGET_PROGRESSBAR_ID,
    WIDGET_CHECKBOX_ID,
    WIDGET_RADIOBUTTON_ID,
	//!! ADD YOUR CUSTOM WIDGET SET ID's HERE
	WIDGET_NUM_WIDGETS
} WIDGET_ID;

//---------------------------------------------------------------------------
// function tables for the widget handlers
extern const WIDGET_INIT g_apfInit[];
extern const WIDGET_DRAW g_apfDraw[];
extern const WIDGET_CONTROL g_apfControl[];
extern const WIDGET_DESTROY g_apfDestroy[];

#endif 
