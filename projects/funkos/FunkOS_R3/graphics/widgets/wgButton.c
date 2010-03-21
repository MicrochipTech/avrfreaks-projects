#include "types.h"
#include "wgButton.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//---------------------------------------------------------------------------
void WgButton_Init(void *pstThis_)
{
	WIDGET_BUTTON *pstWidget = (WIDGET_BUTTON*)pstThis_;
	pstWidget->szCaption = "Button";
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
    pstWidget->cBGColor = COLOR_GREY50;
    pstWidget->cBGActiveColor = COLOR_GREY25;
    pstWidget->cLineColor = COLOR_WHITE;
    pstWidget->cTextColor = COLOR_WHITE;
    pstWidget->bState = FALSE;
}
//---------------------------------------------------------------------------
void WgButton_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_BUTTON *pstWidget = (WIDGET_BUTTON*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstBase->pstOwner);

	DRAW_TEXT_STRUCT stText;
	DRAW_LINE_STRUCT stLine;

	USHORT usXOffset = 0;
    USHORT usHalfWidth = 0;
	USHORT usYOffset = 0;

	// Get the location of the control relative to elements higher in the heirarchy
	Fooey_GetControlOffsets(pstBase, &usXOffset, &usYOffset);

	// Draw the rounded-off rectangle
	stLine.usX1 = pstBase->usLeft + usXOffset + 1;
	stLine.usX2 = pstBase->usRight + usXOffset - 1;
    stLine.usY1 = pstBase->usTop + usYOffset;
    stLine.usY2 = pstBase->usTop + usYOffset;
    stLine.uColor = pstWidget->cLineColor;
    Draw_Line(pstManager->pstDisplay, &stLine);

    stLine.usY1 = pstBase->usBottom + usYOffset;
    stLine.usY2 = pstBase->usBottom + usYOffset;
    Draw_Line(pstManager->pstDisplay, &stLine);

	stLine.usX1 = pstBase->usLeft + usXOffset;
	stLine.usX2 = pstBase->usLeft + usXOffset;
    stLine.usY1 = pstBase->usTop + usYOffset + 1;
    stLine.usY2 = pstBase->usBottom + usYOffset - 1;
    Draw_Line(pstManager->pstDisplay, &stLine);

	stLine.usX1 = pstBase->usRight + usXOffset;
	stLine.usX2 = pstBase->usRight + usXOffset;
    Draw_Line(pstManager->pstDisplay, &stLine);

	// Draw a rectangle before the text if the BG is specified.
	{
		DRAW_RECTANGLE_STRUCT stRect;
		stRect.usLeft = pstBase->usLeft + usXOffset + 1;
		stRect.usRight = pstBase->usRight + usXOffset - 1;
		stRect.usTop = pstBase->usTop + usYOffset + 1;
		stRect.usBottom = pstBase->usBottom + usYOffset - 1;
		stRect.bFill = TRUE;

        if (pstWidget->bState)
        {
            stRect.uFillColor = pstWidget->cBGActiveColor;
        }
        else
        {
            stRect.uFillColor = pstWidget->cBGColor;
        }

        if (pstBase->bHasFocus)
        {
            stRect.uLineColor = pstWidget->cLineColor;
        }
        else
        {
            stRect.uLineColor = stRect.uFillColor;
        }

		Draw_Rectangle(pstManager->pstDisplay, &stRect);
	}

	// Draw the Text
	stText.pstFont = pstWidget->pstFont;
	stText.pucString = pstWidget->szCaption;
	stText.uColor = pstWidget->cTextColor;
    usHalfWidth = Draw_TextWidth(pstManager->pstDisplay, &stText);
    usHalfWidth >>= 1;
	stText.usX = pstBase->usLeft + ((pstBase->usRight - pstBase->usLeft)>>1) - usHalfWidth + usXOffset;
	stText.usY = pstBase->usTop + usYOffset;
	Draw_Text(pstManager->pstDisplay, &stText);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgButton_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	switch (pstEvent_->usEventID)
    {
    	case FOOEY_EVENT_TYPE_KEYBOARD:
        {
        }
        	break;
        case FOOEY_EVENT_TYPE_MOUSE:
        {
            WIDGET_BUTTON *pstWidget = (WIDGET_BUTTON*)pstThis_;

            // Fresh event
            if (pstEvent_->usEventTime == 0)
            {
                if (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.bLeftState == 1)
                {
                    // Mouse down - set the visual state
                    pstWidget->bState = TRUE;
                }
                else
                {
                    // mouse up - run the callback
                    pstWidget->bState = FALSE;
                    if (pstWidget->pfCallback != NULL)
                    {
                        pstWidget->pfCallback();
                    }
                }
                // set the control's focus
                Fooey_ResetFocus((GUI_MANAGER_STRUCT*)pstWidget->stWidget.pstOwner);
                pstWidget->stWidget.bHasFocus = TRUE;
                pstWidget->stWidget.bRedraw = TRUE;
            }
        }
        	break;
        default:
        	break;
    }

}
//---------------------------------------------------------------------------
void WgButton_Destroy(void *pstThis_)
{

}

