#include "types.h"
#include "wgSurface.h"
#include "colorspace.h"
#include "display.h"
#include "draw.h"

//--------------------------------------------------------------------------- 
void WgSurface_Init(void *pstThis_)
{
	WIDGET_SURFACE *pstWidget = (WIDGET_SURFACE*)pstThis_;
	pstWidget->cBGColor = COLOR_RED;
}
//--------------------------------------------------------------------------- 
void WgSurface_Draw(void *pstThis_)
{
	GUI_BASE_STRUCT *pstWidget = (GUI_BASE_STRUCT*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstWidget->pstOwner);
	DISPLAY_DRIVER_STRUCT *pstDisplay = pstManager->pstDisplay;
	DRAW_RECTANGLE_STRUCT stRect;

	stRect.usLeft = pstWidget->usLeft;
	stRect.usRight = pstWidget->usRight;
	stRect.usTop = pstWidget->usTop;
	stRect.usBottom = pstWidget->usBottom;
	stRect.bFill = TRUE;
	stRect.uLineColor = ((WIDGET_SURFACE*)pstThis_)->cBGColor;
	stRect.uFillColor = ((WIDGET_SURFACE*)pstThis_)->cBGColor;

	Draw_Rectangle(pstDisplay, &stRect);
    pstWidget->bRedraw = FALSE;
}
//---------------------------------------------------------------------------
void WgSurface_Control(void *pstThis_, FOOEY_EVENT_STRUCT *pstEvent_)
{
	GUI_BASE_STRUCT *pstWidget = (GUI_BASE_STRUCT*)pstThis_;
	GUI_MANAGER_STRUCT *pstManager = (GUI_MANAGER_STRUCT*)(pstWidget->pstOwner);
    USHORT i;

    if (pstEvent_->usDest == WIDGET_DEST_AUTO)
    {

        switch (pstEvent_->usEventID)
        {
            //--------------------------------------------------------------------
            case FOOEY_EVENT_TYPE_KEYBOARD:
            {
                // Try the event with the surface's currently selected component
                // Forward the event to the bounding box of all non-null controls that aren't the surface
                for (i = 0; i < pstManager->usNumWidgets; i++)
                {
                    if ((pstManager->pastScene[i] != NULL) && (pstManager->pastScene[i] != pstThis_))
                    {
                        if (pstManager->pastScene[i]->bHasFocus == TRUE)
                        {
                            // Forward the event to the control
                            pstManager->pastScene[i]->pfControl(pstManager->pastScene[i], pstEvent_);
                        }
                    }
                }
            }
                break;
            //--------------------------------------------------------------------
            case FOOEY_EVENT_TYPE_MOUSE:
            {
                USHORT usLeft;
                USHORT usRight;
                USHORT usTop;
                USHORT usBottom;

                USHORT usXOffset;
                USHORT usYOffset;

                // Forward the event to the bounding box of all non-null controls that aren't the surface
                for (i = 0; i < pstManager->usNumWidgets; i++)
                {
                    if ((pstManager->pastScene[i] != NULL) &&
                        (pstManager->pastScene[i] != pstThis_))
                    {
                        // Get the bounding box of each control
                        usLeft = pstManager->pastScene[i]->usLeft;
                        usTop = pstManager->pastScene[i]->usTop;
                        usRight = pstManager->pastScene[i]->usRight;
                        usBottom = pstManager->pastScene[i]->usBottom;

                        Fooey_GetControlOffsets(pstWidget, &usXOffset, &usYOffset);

                        usLeft += usXOffset;
                        usRight += usXOffset;

                        usTop += usYOffset;
                        usBottom += usYOffset;

                        // Compare to the X/Y location of the event
                        if ((pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.usX >= usLeft) &&
                            (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.usX <= usRight) &&
                            (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.usY >= usTop) &&
                            (pstEvent_->EVENT_DATA.MOUSE_EVENT_DATA.usY >= usBottom))
                        {
                            // Forward the event to the control
                            pstManager->pastScene[i]->pfControl(pstManager->pastScene[i], pstEvent_);
                        }
                    }
                }
            }
                break;
            default:
                break;
        }
    }
}
//---------------------------------------------------------------------------
void WgSurface_Destroy(void *pstThis_)
{

}

