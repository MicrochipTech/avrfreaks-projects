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
    \file graphics_uoled.h
    
    \brief Graphics driver implementation on 4D systems uOLED displays
    
*/

#include "driver.h"
#include "draw.h"
#include "graphics.h"
#include "driver.h"

#ifndef __GRAPHICS_UOLED_H__
#define __GRAPHICS_UOLED_H__

//---------------------------------------------------------------------------
typedef struct
{
    struct
    {
        union
        {
            K_USHORT usData;
            K_UCHAR ucData;
        };
    };
    K_UCHAR ucLen;
} DataVector_t;


/*!
    Graphics driver implementation based on the generic GraphicsDriver class
*/
class GraphicsUOLED : public GraphicsDriver
{
public:
//---------------------------------------------------------
    virtual void Init() {}
    virtual K_UCHAR Open() { return 0; }
    virtual K_UCHAR Close() { return 0; }
    virtual K_USHORT Read( K_USHORT usBytes_, K_UCHAR *pucData_ )
        { return 0; }
    virtual K_USHORT Write( K_USHORT usBytes_, K_UCHAR *pucData_ )
        { return 0; }
    virtual K_USHORT Control( K_USHORT usEvent_, void *pvDataIn_, K_USHORT usSizeIn_, void *pvDataOut_, K_USHORT usSizeOut_ )
        { return 0; }        
        
//---------------------------------------------------------
    virtual void DrawPixel(DrawPoint_t *pstPoint_) { Point(pstPoint_); }

//---------------------------------------------------------
/*
    Raster operations defined using per-pixel rendering.
    Can be overridden in inheriting classes.
*/
    virtual void ClearScreen();
    virtual void Point(DrawPoint_t *pstPoint_);
    virtual void Line(DrawLine_t *pstLine_);
    virtual void Rectangle(DrawRectangle_t *pstRectangle_);
    virtual void Circle(DrawCircle_t *pstCircle_);
    virtual void Ellipse(DrawEllipse_t *pstEllipse_) {}
    virtual void Bitmap(DrawBitmap_t *pstBitmap_) {}
    virtual void Stamp(DrawStamp_t *pstStamp_) {}
    virtual void Move(DrawMove_t *pstMove_ ) {}
    virtual void TriangleWire(DrawPoly_t *pstPoly_);
    virtual void TriangleFill(DrawPoly_t *pstPoly_) {}
    virtual void Polygon(DrawPoly_t *pstPoly_);

    virtual void Text(DrawText_t *pstText_);
    virtual K_USHORT TextWidth(DrawText_t *pstText_);
    void SetDriver( Driver *pclDriver_ ) { m_pclDriver = pclDriver_; }
private:


    void WriteByte( K_UCHAR ucByte_ );
    void WriteWord( K_USHORT usWord_ );
    void WriteVector( DataVector_t *pstVector_, K_UCHAR ucCount_);

    K_UCHAR ReadByte( void );
    K_UCHAR WaitAck( void );

    void MoveCursor(K_USHORT usX_, K_USHORT usY_);
    void MoveOrigin( K_USHORT usX_, K_USHORT usY_ );

    void SetOutlineColor( COLOR uColor_ );
    void SetFontFGColor(COLOR uColor_);
    void SetFontBGColor(COLOR uColor_);
    void SetTextOpacity(bool bOpaque_);

    Driver *m_pclDriver;
    
    COLOR m_uTextColor;
};

#endif
