#include "types.h"
#include "fooey.h"
#include "widgetcfg.h"
#include "widgets.h"

const WIDGET_INIT g_apfInit[] =
{
    WgSurface_Init,
    WgLabel_Init,
    WgButton_Init,
    WgEditBox_Init,
    WgProgressBar_Init,
    WgCheckBox_Init,
    WgRadioButton_Init,
    NULL,
};

const WIDGET_DRAW g_apfDraw[] =
{
    WgSurface_Draw,
    WgLabel_Draw,
    WgButton_Draw,
    WgEditBox_Draw,
    WgProgressBar_Draw,
    WgCheckBox_Draw,
    WgRadioButton_Draw,
    NULL
};

const WIDGET_CONTROL g_apfControl[] =
{
    WgSurface_Control,
    WgLabel_Control,
    WgButton_Control,
    WgEditBox_Control,
    WgProgressBar_Control,
    WgCheckBox_Control,
    WgRadioButton_Control,
    NULL
};

const WIDGET_DESTROY g_apfDestroy[] =
{
    WgSurface_Destroy,
    WgLabel_Destroy,
    WgButton_Destroy,
    WgEditBox_Destroy,
    WgProgressBar_Destroy,
    WgCheckBox_Destroy,
    WgRadioButton_Destroy,
    NULL
};

