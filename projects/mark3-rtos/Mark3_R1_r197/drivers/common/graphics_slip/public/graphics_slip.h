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
    
    \brief Graphics driver implementation on-top of SLIP serial.
    
    Graphics driver implementation requires a host capable of interpreting
    the FunkenSLIP graphics protocol and able to render the results.
*/

#include "driver.h"
#include "draw.h"
#include "graphics.h"
#include "slip.h"

#ifndef __GRAPHICS_SLIP_H__
#define __GRAPHICS_SLIP_H__

//---------------------------------------------------------------------------
#define STAMP_CACHE_SIZE            (16)

//---------------------------------------------------------------------------
/*!
    This data structure is used to provide a quick lookup table for 
    stamps, so that they can be cached instead of redrawn pixel-for-pixel on
    every single redraw.
*/
typedef struct
{
    K_UCHAR ucAccess;
    K_UCHAR *pucData;
} SlipStampCache_t;
//---------------------------------------------------------------------------

/*!
    Graphics driver implementation based on the generic GraphicsDriver class
*/
class GraphicsSlip : public GraphicsDriver
{
public:
//---------------------------------------------------------
    virtual void Init();
    virtual K_UCHAR Open() { return 0; }
    virtual K_UCHAR Close() { return 0; }
    virtual K_USHORT Read( K_USHORT usBytes_, K_UCHAR *pucData_ )
        { return 0; }
    virtual K_USHORT Write( K_USHORT usBytes_, K_UCHAR *pucData_ )
        { return 0; }
    virtual K_USHORT Control( K_USHORT usEvent_, void *pvDataIn_, K_USHORT usSizeIn_, void *pvDataOut_, K_USHORT usSizeOut_ )
        { return 0; }        
        
//---------------------------------------------------------
    virtual void DrawPixel(DrawPoint_t *pstPoint_);

//---------------------------------------------------------
/*
    Raster operations defined using per-pixel rendering.
    Can be overridden in inheriting classes.
*/
    virtual void ClearScreen();
    // virtual void Point(DrawPoint_t *pstPoint_);
    virtual void Line(DrawLine_t *pstLine_);
    virtual void Rectangle(DrawRectangle_t *pstRectangle_);
    virtual void Circle(DrawCircle_t *pstCircle_);
    virtual void Ellipse(DrawEllipse_t *pstEllipse_);
    // virtual void Bitmap(DrawBitmap_t *pstBitmap_);
    virtual void Stamp(DrawStamp_t *pstStamp_);
    // virtual void Move(DrawMove_t *pstMove_ );
    // virtual void TriangleWire(DrawPoly_t *pstPoly_);
    // virtual void TriangleFill(DrawPoly_t *pstPoly_);
    // virtual void Polygon(DrawPoly_t *pstPoly_);

    //virtual void Text(DrawText_t *pstText_);
    //virtual K_USHORT TextWidth(DrawText_t *pstText_) {return 0;}
    void SetSlip( Slip* pclSlip_ ) { m_pclSlip = pclSlip_; }
private:
    K_BOOL FindStamp(K_UCHAR *pucStamp_);
    void AddStamp(K_UCHAR *pucStamp_);
    void AddIfNecessary(K_UCHAR *pucStamp_, K_UCHAR ucSize_);
    K_UCHAR Strlen(K_CHAR *szData_);
    
    SlipStampCache_t m_astStampCache[ STAMP_CACHE_SIZE ];
    Slip *m_pclSlip;
    K_UCHAR m_ucStampCacheIndex;
};

#endif
