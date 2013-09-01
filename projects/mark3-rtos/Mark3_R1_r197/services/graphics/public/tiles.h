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
#include "mark3cfg.h"
#include "kerneltypes.h"
#include "colordepth.h"
#include "colorspace.h"
#include "graphics.h"
#include "draw.h"

#ifndef __TILES_H__
#define __TILES_H__

//---------------------------------------------------------------------------
/*!
    Define the pixel format used by a given tile.  This is used to greatly
    reduce the memory footprint of tiles, by using color-indexing with
    fewer bits-per-pixel than the native pixel-format of the display.
*/
typedef enum
{
    TILE_1BPP = 0,	// Monochrome
    TILE_2BPP = 1,	// 4-color
    TILE_3BPP = 2,	// 8-color
    TILE_4BPP = 3,	// 16-color
    TILE_5BPP = 4,	// 32-color
    TILE_6BPP = 5,	// 64-color
    TILE_7BPP = 6,	// 128-color
    TILE_8BPP = 7,	// 256-color
// --
    TILE_NUM_FORMATS
} TileFormat_t;

//---------------------------------------------------------------------------
/*!
  * \brief TileDef_t Structure defining parameters for a color-indexed tile
  */
typedef struct
{
    TileFormat_t m_eFormat;     //!< Color-indexing of the tile (bits-per-pixel)

    K_UCHAR *m_pucData;         //!< Pointer to color-indexed tile data
    COLOR   *m_puPalette;       //!< Pointer to a palette assigned to this tile

    K_UCHAR m_ucHeight;         //!< Height of the tile (in pixels)
    K_UCHAR m_ucWidth;          //!< Width of the tile (in pixels)
} TileDef_t;

//---------------------------------------------------------------------------
// Class for managing an 8x8, 16bpp-output tile with arbitrary bpp.
#define TILE_8x8_BUFFER_SIZE	(8 * 8) // 128 "Color".

class Tile_8x8
{
public:
    /*!
     * \brief LoadTile
     *
     * Load the tile specified by pstTileDef_ into memory.  This takes some
     * time as it parses the indexed colors, does a lookup, and then writes
     * to the local tile buffer.  Once a tile has been loaded, it can be
     * rendered any number of times.

     * \param pstTileDef_ Pointer to a struct containing configuration
     *                    data for the tile to be loaded.
     */
    void LoadTile (TileDef_t *pstTileDef_);

    /*!
     * \brief Render
     *
     * Render loaded tile data to a specific location on a specified
     * display.
     *
     * \param pclDriver_ Pointer to the graphics driver to render with
     *
     * \param usX_ Leftmost pixel index
     * \param usY_ Topmost pixel index
     */
    void Render(GraphicsDriver *pclDriver_, K_USHORT usX_, K_USHORT usY_);

private:
    /*!
     * \brief m_auTileBuffer Object's local storage for tile data
     */
    COLOR m_auTileBuffer[ TILE_8x8_BUFFER_SIZE ];

    /*!
     * \brief m_ucWidth Width of the tile (may be smaller than width of buffer)
     */
    K_UCHAR m_ucWidth;

    /*!
     * \brief m_ucHeight Height of the tile (may be smaler than the height of buffer)
     */
    K_UCHAR m_ucHeight;
};

#endif // __TILES_H__
