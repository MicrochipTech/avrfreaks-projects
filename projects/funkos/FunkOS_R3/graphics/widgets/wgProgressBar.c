#include "types.h"
#include "wgProgressBar.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//---------------------------------------------------------------------------
void WgProgressBar_Init(void *pstThis_)
{
	WIDGET_PROGRESSBAR *pstWidget = (WIDGET_PROGRESSBAR*)pstThis_;
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
    pstWidget->cBGColor = COLOR_WHITE;
    pstWidget->cLineColor = COLOR_GREY50;
    pstWidget->cTextColor = COLOR_BLACK;
    pstWidget->cFillColor = COLOR_GREEN;
    pstWidget->ucProgress = 0;
}
//---------------------------------------------------------------------------
void WgProgressBar_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_PROGRESSBAR *pstWidget = (WIDGET_PROGRESSBAR*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstBase->pstOwner);

	DRAW_RECTANGLE_STRUCT stRect;

	USHORT usXOffset = 0;
	USHORT usYOffset = 0;
    USHORT usProgressWidth;

	// Get the location of the control relative to elements higher in the heirarchy
	Fooey_GetControlOffsets(pstBase, &usXOffset, &usYOffset);

    // First, draw the background rectangle
    stRect.usLeft = pstBase->usLeft + usXOffset;
    stRect.usRight = pstBase->usRight + usXOffset;
    stRect.usTop = pstBase->usTop + usYOffset;
    stRect.usBottom = pstBase->usBottom + usYOffset;
    stRect.uFillColor = pstWidget->cBGColor;
    stRect.bFill = TRUE;
    stRect.uLineColor = pstWidget->cLineColor;

    Draw_Rectangle(pstManager->pstDisplay, &stRect);

    // Draw the progress rectangle
    stRect.usLeft = pstBase->usLeft + usXOffset + 1;
    stRect.usTop = pstBase->usTop + usYOffset + 1;
    stRect.usBottom = pstBase->usBottom + usYOffset - 1;

    stRect.uFillColor = pstWidget->cFillColor;
    stRect.uLineColor = pstWidget->cFillColor;
    stRect.bFill = TRUE;

    usProgressWidth = (pstBase->usRight - pstBase->usLeft) - 2;
    usProgressWidth = (usProgressWidth * (USHORT)pstWidget->ucProgress) / 100;
    stRect.usRight = stRect.usLeft + usProgressWidth;

    Draw_Rectangle(pstManager->pstDisplay, &stRect);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgProgressBar_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
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
void WgProgressBar_Destroy(void *pstThis_)
{

}

