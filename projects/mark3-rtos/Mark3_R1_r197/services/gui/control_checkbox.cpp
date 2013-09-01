/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file control_checkbox.cpp
    \brief Checkbox Control

    A binary On/Off switch control
*/

#include "gui.h"
#include "kerneltypes.h"
#include "draw.h"
#include "font.h"
#include "control_checkbox.h"

//---------------------------------------------------------------------------
#define TEXT_X_OFFSET        (13)

//---------------------------------------------------------------------------
static const K_UCHAR aucBox[] =
{ 0x7E,
  0x81,
  0x81,
  0x81,
  0x81,
  0x81,
  0x81,
  0x7E };

//---------------------------------------------------------------------------
static const K_UCHAR aucCheck[] =
{ 0,
  0,
  0x3C,
  0x3C,
  0x3C,
  0x3C,
  0,
  0 };

//---------------------------------------------------------------------------
void CheckBoxControl::Init()
{
    SetAcceptFocus(true);
}

//---------------------------------------------------------------------------
void CheckBoxControl::Draw()
{
    GraphicsDriver *pclDriver = GetParentWindow()->GetDriver();
    K_USHORT usX, usY;
    K_USHORT usTextWidth;

    GetControlOffset(&usX, &usY);

    // Draw the box, (and check, if necessary)
    {
        DrawRectangle_t stRect;

        if (GetParentWindow()->IsInFocus(this))
        {
            stRect.uLineColor = m_uActiveColor;
        }
        else
        {
            stRect.uLineColor = m_uBackColor;
        }

        stRect.uFillColor = m_uBackColor;
        stRect.usTop = usY + GetTop();
        stRect.usLeft = usX + GetLeft();
        stRect.usRight = stRect.usLeft + GetWidth() - 1;
        stRect.usBottom = stRect.usTop + GetHeight() - 1;
        stRect.bFill = true;
        pclDriver->Rectangle(&stRect);

        stRect.uLineColor = m_uBoxBGColor;
        stRect.uFillColor = m_uBoxBGColor;
        stRect.usTop = usY + GetTop() + ((GetHeight() - 5) >> 1) - 1;
        stRect.usLeft = usX + GetLeft() + 2;
        stRect.usRight = stRect.usLeft + 7;
        stRect.usBottom = stRect.usTop + 7;
        stRect.bFill = true;
        pclDriver->Rectangle(&stRect);
    }

    {
        DrawStamp_t stStamp;
        stStamp.uColor = m_uBoxColor;
        stStamp.usY = usY + GetTop() + ((GetHeight() - 5) >> 1) - 1;
        stStamp.usX = usX + GetLeft() + 2;
        stStamp.usWidth = 8;
        stStamp.usHeight = 8;
        stStamp.pucData = (K_UCHAR*)aucBox;
        pclDriver->Stamp(&stStamp);

        if (m_bChecked)
        {
            stStamp.pucData = (K_UCHAR*)aucCheck;
            pclDriver->Stamp(&stStamp);
        }
    }

    // Draw the caption
    {
        DrawText_t stText;
        stText.usLeft = usX + GetLeft() + TEXT_X_OFFSET;
        stText.usTop = usY + GetTop();
        stText.uColor = m_uFontColor;
        stText.pstFont = m_pstFont;
        stText.pcString = m_szCaption;

        usTextWidth = pclDriver->TextWidth(&stText);
        pclDriver->Text(&stText);
    }
}

//---------------------------------------------------------------------------
GuiReturn_t CheckBoxControl::ProcessEvent( GuiEvent_t *pstEvent_ )
{
    K_USHORT usXOffset, usYOffset;

    GetControlOffset(&usXOffset, &usYOffset);

    GUI_DEBUG_PRINT("ButtonControl::ProcessEvent\n");

    switch (pstEvent_->ucEventType)
    {
        case EVENT_TYPE_KEYBOARD:
        {
            // If this is a space bar or an enter key, behave like a mouse click.
            if ((KEYCODE_SPACE == pstEvent_->stKey.ucKeyCode) ||
                (KEYCODE_RETURN == pstEvent_->stKey.ucKeyCode))
            {
                if (pstEvent_->stKey.bKeyState)
                {
                    m_bChecked = true;
                }
                else
                {
                    m_bChecked = false;
                }
                SetStale();
            }
        }
            break;
        case EVENT_TYPE_MOUSE:
        {
            // Is this control currently in the "active"/pressed state?
            if (m_bChecked)
            {
                // Check to see if the movement is out-of-bounds based on the coordinates.
                // If so, de-activate the control
                if (pstEvent_->stMouse.bLeftState)
                {
                    if ((pstEvent_->stMouse.usX >= GetLeft() + usXOffset) &&
                        (pstEvent_->stMouse.usX < GetLeft() + usXOffset + GetWidth()-1) &&
                        (pstEvent_->stMouse.usY >= GetTop() + usYOffset) &&
                        (pstEvent_->stMouse.usY < GetTop() + usYOffset + GetHeight() - 1))
                    {
                        m_bChecked = false;
                        SetStale();
                    }
                }
            }
            else if (!m_bChecked)
            {
                // If we registered a down-click in the bounding box, set the state of the
                // control to activated.
                if (pstEvent_->stMouse.bLeftState)
                {
                    if ((pstEvent_->stMouse.usX >= GetLeft() + usXOffset) &&
                        (pstEvent_->stMouse.usX < GetLeft() + usXOffset + GetWidth()-1) &&
                        (pstEvent_->stMouse.usY >= GetTop() + usYOffset) &&
                        (pstEvent_->stMouse.usY < GetTop() + usYOffset + GetHeight() - 1))
                    {
                        m_bChecked = true;
                        SetStale();
                    }
                }
            }

            if (!IsInFocus())
            {
                GetParentWindow()->SetFocus(this);
                SetStale();
            }
        }
            break;
    }
}
