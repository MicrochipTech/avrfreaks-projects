#include "types.h"
#include "wgCheckBox.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//---------------------------------------------------------------------------
const UCHAR aucCheck[8] = {0xC3,0xE7,0x7E,0x3C,0x3C,0x7E,0xE7,0xC3};


//---------------------------------------------------------------------------
void WgCheckBox_Init(void *pstThis_)
{
	WIDGET_CHECKBOX *pstWidget = (WIDGET_CHECKBOX*)pstThis_;

	pstWidget->szCaption = "CheckBox";
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
	pstWidget->cBGColor = COLOR_RED;
    pstWidget->cCheckColor = COLOR_GREY25;
	pstWidget->cBoxColor = COLOR_WHITE;
	pstWidget->cTextColor = COLOR_WHITE;
    pstWidget->cActiveColor = COLOR_GREY25;
    pstWidget->bChecked = FALSE;
}
//---------------------------------------------------------------------------
void WgCheckBox_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_CHECKBOX *pstWidget = (WIDGET_CHECKBOX*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstBase->pstOwner);

	DRAW_TEXT_STRUCT stText;
	DRAW_RECTANGLE_STRUCT stRect;
    DRAW_STAMP_STRUCT stStamp;

	USHORT usXOffset = 0;
	USHORT usYOffset = 0;

	// Get the location of the control relative to elements higher in the heirarchy
	Fooey_GetControlOffsets(pstBase, &usXOffset, &usYOffset);

    // Clear the bounding rectangle
    stRect.usLeft = pstBase->usLeft + usXOffset;
    stRect.usRight = pstBase->usRight + usXOffset;
    stRect.usTop = pstBase->usTop + usYOffset;
    stRect.usBottom = pstBase->usBottom + usYOffset;
    stRect.bFill = TRUE;

    if (pstBase->bHasFocus == TRUE)
    {
        stRect.uLineColor = pstWidget->cActiveColor;
    }
    else
    {
        stRect.uFillColor = pstWidget->cBGColor;
    }
    stRect.uFillColor = pstWidget->cBGColor;

    Draw_Rectangle(pstManager->pstDisplay, &stRect);

    // Draw the check box (10x10 box 1 pixel inside the bounding box)

    stRect.usLeft++;
    stRect.usRight = stRect.usLeft + 9;
    stRect.usTop = stRect.usTop + ((pstBase->usBottom - pstBase->usTop) - 10) / 2;
    stRect.usBottom = stRect.usTop + 9;
    stRect.uFillColor = pstWidget->cBoxColor;
    stRect.uLineColor = pstWidget->cCheckColor;

    Draw_Rectangle(pstManager->pstDisplay, &stRect);

    // Draw the check (if set)
    if (pstWidget->bChecked == TRUE)
    {
        stStamp.usX = stRect.usLeft + 1;
        stStamp.usY = stRect.usTop + 1;
        stStamp.uColor = pstWidget->cCheckColor;
        stStamp.usWidth = 8;
        stStamp.usHeight = 8;
        stStamp.pucData = (UCHAR*)&aucCheck[0];
        Draw_Stamp(pstManager->pstDisplay, &stStamp);
    }

	// Draw the Text
	stText.pstFont = pstWidget->pstFont;
	stText.pucString = pstWidget->szCaption;
	stText.uColor = pstWidget->cTextColor;
	stText.usX = stRect.usRight + 2;
	stText.usY = pstBase->usTop;

	Draw_Text(pstManager->pstDisplay, &stText);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgCheckBox_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	switch (pstEvent_->usEventID)
    {
    	case FOOEY_EVENT_TYPE_KEYBOARD:
        {
        }
        	break;
        case FOOEY_EVENT_TYPE_MOUSE:
        {
        	WIDGET_CHECKBOX *pstWidget = (WIDGET_CHECKBOX*)pstThis_;

            // Fresh event
            if (pstEvent_->usEventTime == 0)
            {
                // Mouse down
                if (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.bLeftState == 1)
                {
                    if (pstWidget->bChecked == FALSE)
                    {
                        pstWidget->bChecked = TRUE;
                    }
                    else
                    {
                        pstWidget->bChecked = FALSE;
                    }

                    // set the control's focus
                    Fooey_ResetFocus((GUI_MANAGER_STRUCT*)pstWidget->stWidget.pstOwner);
                    pstWidget->stWidget.bHasFocus = TRUE;
                    pstWidget->stWidget.bRedraw = TRUE;
                }
            }
        }
        	break;
        default:
        	break;
    }
}
//---------------------------------------------------------------------------
void WgCheckBox_Destroy(void *pstThis_)
{

}

