/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2014 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
#include "tiles.h"
#include "bitstream.h"
#include "mark3cfg.h"
#include "kerneltypes.h"
#include "colordepth.h"
#include "colorspace.h"
#include "graphics.h"
#include "draw.h"

//---------------------------------------------------------------------------
void Tile_8x8::LoadTile (TileDef_t *pstTileDef_)
{
    BitStreamer clStream;

    m_ucWidth = pstTileDef_->m_ucWidth;
    m_ucHeight = pstTileDef_->m_ucHeight;

    // Tile size = ((width of the tile in pixels * bits-per-pixel encoded) / 8) rounded to the next byte * number of rows.
    K_USHORT usTileSize = (((K_USHORT)pstTileDef_->m_ucWidth * ((K_USHORT)pstTileDef_->m_eFormat + 1)) + 7)  >> 3;
    COLOR *puBufPtr = m_auTileBuffer;

    usTileSize *= (K_USHORT)(pstTileDef_->m_ucHeight);

    clStream.Init( pstTileDef_->m_pucData, usTileSize );
    for (K_UCHAR i = 0; i < pstTileDef_->m_ucHeight; i++)
    {
        for (K_UCHAR j = 0; j < pstTileDef_->m_ucWidth; j++)
        {
            K_UCHAR ucColorIndex = clStream.ReadBits((K_UCHAR)pstTileDef_->m_eFormat + 1);
            *puBufPtr++ = pstTileDef_->m_puPalette[ucColorIndex];
        }
        // Each row starts with a fresh byte
        clStream.AdvanceByte();
    }
}

//---------------------------------------------------------------------------
void Tile_8x8::Render(GraphicsDriver *pclDriver_, K_USHORT usX_, K_USHORT usY_)
{
    DrawBitmap_t stBitmap;

    // Create a bitmap object that references our object's tile buffer
    stBitmap.pucData = (K_UCHAR*)m_auTileBuffer;
    stBitmap.ucBPP = 16;
    stBitmap.usHeight = m_ucHeight;
    stBitmap.usWidth = m_ucWidth;
    stBitmap.usX = usX_;
    stBitmap.usY = usY_;

    // use the supplied driver to render the tile as a bitmap
    pclDriver_->Bitmap(&stBitmap);
}
