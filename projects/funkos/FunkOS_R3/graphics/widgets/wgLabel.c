#include "types.h"
#include "wgLabel.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//--------------------------------------------------------------------------- 
void WgLabel_Init(void *pstThis_)
{
	WIDGET_LABEL *pstWidget = (WIDGET_LABEL*)pstThis_;
	pstWidget->szCaption = "Label";
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
	pstWidget->cBGColor = COLOR_GREY75;
	pstWidget->cTextColor = COLOR_BLACK;
	pstWidget->bUseBG = FALSE;					
}
//--------------------------------------------------------------------------- 
void WgLabel_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_LABEL *pstWidget = (WIDGET_LABEL*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstBase->pstOwner);
	DRAW_TEXT_STRUCT stText;
	USHORT usXOffset = 0;
	USHORT usYOffset = 0;

	// Get the location of the control relative to elements higher in the heirarchy
	Fooey_GetControlOffsets(pstBase, &usXOffset, &usYOffset);

	// Draw a rectangle before the text if the BG is specified.
	if (pstWidget->bUseBG == TRUE)
	{
		DRAW_RECTANGLE_STRUCT stRect;
		stRect.usLeft = pstBase->usLeft + usXOffset;
		stRect.usRight = pstBase->usRight + usXOffset;
		stRect.usTop = pstBase->usTop + usYOffset;
		stRect.usBottom = pstBase->usBottom + usYOffset;
		stRect.bFill = TRUE;
		stRect.uLineColor = pstWidget->cBGColor;
		stRect.uFillColor = pstWidget->cBGColor;

		Draw_Rectangle(pstManager->pstDisplay, &stRect);
	}

	// Draw the Text
	stText.usX = pstBase->usLeft + usXOffset;
	stText.usY = pstBase->usTop + usYOffset;
	stText.pstFont = pstWidget->pstFont;
	stText.pucString = pstWidget->szCaption;
	stText.uColor = pstWidget->cTextColor;
	Draw_Text(pstManager->pstDisplay, &stText);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgLabel_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
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
void WgLabel_Destroy(void *pstThis_)
{

}

