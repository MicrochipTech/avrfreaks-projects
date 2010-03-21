#include "types.h"
#include "wgEditBox.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//---------------------------------------------------------------------------
void WgEditBox_Init(void *pstThis_)
{
	WIDGET_EDITBOX *pstWidget = (WIDGET_EDITBOX*)pstThis_;
	pstWidget->szCaption[0] = '\0';
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
    pstWidget->cBGColor = COLOR_WHITE;
    pstWidget->cLineColor = COLOR_GREY50;
    pstWidget->cTextColor = COLOR_BLACK;
    pstWidget->cActiveColor = COLOR_GREY25;
}
//---------------------------------------------------------------------------
void WgEditBox_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_EDITBOX *pstWidget = (WIDGET_EDITBOX*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstBase->pstOwner);

	DRAW_TEXT_STRUCT stText;
	DRAW_RECTANGLE_STRUCT stRect;

	USHORT usXOffset = 0;
	USHORT usYOffset = 0;

	// Get the location of the control relative to elements higher in the heirarchy
	Fooey_GetControlOffsets(pstBase, &usXOffset, &usYOffset);

    stRect.usLeft = pstBase->usLeft + usXOffset;
    stRect.usRight = pstBase->usRight + usXOffset;
    stRect.usTop = pstBase->usTop + usYOffset;
    stRect.usBottom = pstBase->usBottom + usYOffset;
    stRect.uFillColor = pstWidget->cBGColor;
    stRect.bFill = TRUE;
    if (pstBase->bHasFocus == TRUE)
    {
        stRect.uLineColor = pstWidget->cActiveColor;
    }
    else
    {
        stRect.uLineColor = pstWidget->cLineColor;
    }

    Draw_Rectangle(pstManager->pstDisplay, &stRect);


	// Draw the Text
	stText.pstFont = pstWidget->pstFont;
	stText.pucString = pstWidget->szCaption;
	stText.uColor = pstWidget->cTextColor;

	stText.usX = pstBase->usLeft + usXOffset + 1;
	stText.usY = pstBase->usTop + usYOffset + 1;
	Draw_Text(pstManager->pstDisplay, &stText);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgEditBox_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	switch (pstEvent_->usEventID)
    {
    	case FOOEY_EVENT_TYPE_KEYBOARD:
        {
        }
        	break;
        case FOOEY_EVENT_TYPE_MOUSE:
        {
        }
        	break;
        default:
        	break;
    }

}
//---------------------------------------------------------------------------
void WgEditBox_Destroy(void *pstThis_)
{

}

