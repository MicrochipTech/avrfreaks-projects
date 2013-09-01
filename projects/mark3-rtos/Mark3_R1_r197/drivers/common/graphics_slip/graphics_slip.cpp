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
    \file graphics_slip.h
    \brief SLIP Serial graphics driver
*/

#include "kerneltypes.h"
#include "driver.h"
#include "graphics.h"
#include "graphics_slip.h"
#include "slip.h"
#include "slip_mux.h"
#include "font.h"

//---------------------------------------------------------------------------
K_BOOL GraphicsSlip::FindStamp(K_UCHAR *pucStamp_)
{
    K_UCHAR i;
    // iterate through to see if the stamp already exists
    for (i=0; i < STAMP_CACHE_SIZE; i++)
    {
        if (m_astStampCache[i].pucData == pucStamp_)
        {
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------
void GraphicsSlip::AddStamp(K_UCHAR *pucStamp_)
{
    // Update the stamp cache with the new stuff
    m_astStampCache[m_ucStampCacheIndex].pucData = pucStamp_;
        
    // 32 element cache.
    m_ucStampCacheIndex = (m_ucStampCacheIndex + 1);
    if (m_ucStampCacheIndex >= STAMP_CACHE_SIZE)
    {
        m_ucStampCacheIndex = 0;
    }
}

//---------------------------------------------------------------------------
void GraphicsSlip::AddIfNecessary(K_UCHAR *pucStamp_, K_UCHAR ucSize_)
{
    if (!FindStamp(pucStamp_))
    {
        // Send a copy of the stamp to the host
        m_pclSlip->WriteData(SLIP_CHANNEL_GRAPHICS, (K_CHAR*)pucStamp_, ucSize_);

        // Update the cache
        AddStamp(pucStamp_);
    }
}

//---------------------------------------------------------------------------
void GraphicsSlip::Init(void)
{
    m_usResX = 240;
    m_usResY = 192;
    m_ucBPP = 16;
    m_usLeft = 0;
    m_usRight = 239;
    m_usTop = 0;
    m_usBottom = 191;
    
    m_pclSlip->SetDriver( DriverList::FindByPath("/dev/tty" ) );                            
}

//---------------------------------------------------------------------------
void GraphicsSlip::DrawPixel(DrawPoint_t *pstPoint_)
{
    SlipDataVector astSlipData[2];
    K_UCHAR ucEventType = DISPLAY_EVENT_SET_PIXEL;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstPoint_;
    astSlipData[1].ucSize = sizeof(DrawPoint_t);
    
    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 2);
}

//---------------------------------------------------------------------------
void GraphicsSlip::ClearScreen(void)
{
    K_UCHAR ucData;
    ucData = DISPLAY_EVENT_CLEAR;
    
    m_pclSlip->WriteData(SLIP_CHANNEL_GRAPHICS, (K_CHAR*)&ucData, 1);
}

//---------------------------------------------------------------------------
void GraphicsSlip::Line(DrawLine_t *pstLine_)
{
    SlipDataVector astSlipData[2];
    K_UCHAR ucEventType = DISPLAY_EVENT_LINE;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstLine_;
    astSlipData[1].ucSize = sizeof(DrawLine_t);
    
    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 2);
}

//---------------------------------------------------------------------------
void GraphicsSlip::Rectangle(DrawRectangle_t *pstRectangle_)
{
    SlipDataVector astSlipData[2];
    K_UCHAR ucEventType = DISPLAY_EVENT_RECTANGLE;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstRectangle_;
    astSlipData[1].ucSize = sizeof(DrawRectangle_t);
    
    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 2);
}

//---------------------------------------------------------------------------
void GraphicsSlip::Circle(DrawCircle_t *pstCircle_)
{
    SlipDataVector astSlipData[2];
    K_UCHAR ucEventType = DISPLAY_EVENT_CIRCLE;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstCircle_;
    astSlipData[1].ucSize = sizeof(DrawCircle_t);
    
    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 2);
}

//---------------------------------------------------------------------------
void GraphicsSlip::Ellipse(DrawEllipse_t *pstEllipse_)
{
    SlipDataVector astSlipData[2];
    K_UCHAR ucEventType = DISPLAY_EVENT_ELLIPSE;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstEllipse_;
    astSlipData[1].ucSize = sizeof(DrawEllipse_t);
    
    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 2);
}

//---------------------------------------------------------------------------
void GraphicsSlip::Stamp(DrawStamp_t *pstStamp_)
{
    SlipDataVector astSlipData[3];
    
    K_UCHAR ucEventType = DISPLAY_EVENT_STAMP;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstStamp_;
    astSlipData[1].ucSize = sizeof(DrawStamp_t) - 2;
    astSlipData[2].pucData = pstStamp_->pucData;
    astSlipData[2].ucSize = (K_UCHAR)(((pstStamp_->usWidth + 7) / 8) 
                                            * pstStamp_->usHeight);

    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 3);
}

//---------------------------------------------------------------------------
K_UCHAR GraphicsSlip::Strlen(K_CHAR *szData_)
{
    K_UCHAR ucRetval = 0;
    while(*szData_++)
    {
        ucRetval++;
    }
    return ucRetval + 1;
}

#if 0
//---------------------------------------------------------------------------
void GraphicsSlip::Text(DrawText_t *pstText_)
{
    SlipDataVector astSlipData[5];
    
    K_UCHAR ucEventType = DISPLAY_EVENT_TEXT;
    
    astSlipData[0].pucData = &ucEventType;
    astSlipData[0].ucSize = 1;
    astSlipData[1].pucData = (K_UCHAR*)pstText_;
    astSlipData[1].ucSize = sizeof(DrawText_t) - 4;
    astSlipData[2].pucData = &(pstText_->pstFont->ucSize);
    astSlipData[2].ucSize = 1;
    astSlipData[3].pucData = (K_UCHAR*) pstText_->pstFont->szName;
    astSlipData[3].ucSize = Strlen((K_CHAR*)pstText_->pstFont->szName);
    astSlipData[4].pucData = (K_UCHAR*)pstText_->pcString;
    astSlipData[4].ucSize = Strlen((K_CHAR*)pstText_->pcString);

    m_pclSlip->WriteVector(SLIP_CHANNEL_GRAPHICS, astSlipData, 5);
}

#endif
