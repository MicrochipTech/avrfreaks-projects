#include "types.h"
#include "wgRadioButton.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"
#include "font.h"

//---------------------------------------------------------------------------
const UCHAR aucCircle[8] = {0x3C,0x7E,0xFF,0xFF,0xFF,0xFF,0x7E,0x3C};
const UCHAR aucFilled[8] = {0x00,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00};
                                                              
//---------------------------------------------------------------------------
void WgRadioButton_Init(void *pstThis_)
{
	WIDGET_RADIOBUTTON *pstWidget = (WIDGET_RADIOBUTTON*)pstThis_;

	pstWidget->szCaption = "RadioBtn";
	pstWidget->pstFont = (FONT_STRUCT*)Font_Load("Tahoma", 12, 0);
	pstWidget->cBGColor = COLOR_RED;
    pstWidget->cCheckColor = COLOR_GREY25;
	pstWidget->cBoxColor = COLOR_WHITE;
	pstWidget->cTextColor = COLOR_WHITE;
    pstWidget->cActiveColor = COLOR_GREY25;
    pstWidget->bChecked = FALSE;
}
//---------------------------------------------------------------------------
void WgRadioButton_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstBase = (GUI_BASE_STRUCT*)pstThis_;
	WIDGET_RADIOBUTTON *pstWidget = (WIDGET_RADIOBUTTON*)pstThis_;
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
        stRect.uLineColor = pstWidget->cBGColor;
    }
    stRect.uFillColor = pstWidget->cBGColor;

    Draw_Rectangle(pstManager->pstDisplay, &stRect);

    // Draw the check box (10x10 box 1 pixel inside the bounding box)
    stStamp.usX = stRect.usLeft + 1;
    stStamp.usY = stRect.usTop + ((pstBase->usBottom - pstBase->usTop) - 10) / 2;
    stStamp.uColor = pstWidget->cBoxColor;
    stStamp.usWidth = 8;
    stStamp.usHeight = 8;
    stStamp.pucData = (UCHAR*)&aucCircle[0];
    Draw_Stamp(pstManager->pstDisplay, &stStamp);

    // Draw the check (if set)
    if (pstWidget->bChecked == TRUE)
    {
        stStamp.uColor = pstWidget->cCheckColor;
        stStamp.pucData = (UCHAR*)&aucFilled[0];
        Draw_Stamp(pstManager->pstDisplay, &stStamp);
    }

	// Draw the Text
	stText.pstFont = pstWidget->pstFont;
	stText.pucString = pstWidget->szCaption;
	stText.uColor = pstWidget->cTextColor;
	stText.usX = stStamp.usX + 12;
	stText.usY = pstBase->usTop;

	Draw_Text(pstManager->pstDisplay, &stText);

    pstBase->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgRadioButton_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	switch (pstEvent_->usEventID)
    {
    	case FOOEY_EVENT_TYPE_KEYBOARD:
        {
        }
        	break;
        case FOOEY_EVENT_TYPE_MOUSE:
        {
        	WIDGET_RADIOBUTTON *pstWidget = (WIDGET_RADIOBUTTON*)pstThis_;
            WIDGET_RADIOBUTTON *pstNext = pstWidget->pstNext;
           	WIDGET_RADIOBUTTON *pstPrev = pstWidget->pstPrev;

            // Fresh event
            if (pstEvent_->usEventTime == 0)
            {
                // Mouse down
                if (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.bLeftState == 1)
                {
                    if (pstWidget->bChecked == FALSE)
                    {
                        // De-select all other buttons in the group
                        while (pstNext != NULL)
                        {
                            pstNext->bChecked = FALSE;
                            pstNext->stWidget.bRedraw = TRUE;
                            pstNext->stWidget.bHasFocus = FALSE;
                            pstNext = pstNext->pstNext;
                        }
                        while (pstPrev != NULL)
                        {
                            pstPrev->bChecked = FALSE;
                            pstPrev->stWidget.bRedraw = TRUE;
                            pstPrev->stWidget.bHasFocus = FALSE;
                            pstPrev = pstPrev->pstPrev;
                        }
                        // And check the control that was just clicked
                        pstWidget->bChecked = TRUE;
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
void WgRadioButton_Destroy(void *pstThis_)
{

}

