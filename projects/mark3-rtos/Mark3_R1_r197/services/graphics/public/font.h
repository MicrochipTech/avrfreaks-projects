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
    \file font.h
    \brief Font structure definitions
*/

#ifndef __FONT_H__
#define __FONT_H__

#include "kerneltypes.h"
#include "fontport.h"

//---------------------------------------------------------------------------
typedef struct
{
    K_UCHAR ucWidth;        //!< Width of this font glyph in pixels
    K_UCHAR ucHeight;        //!< Height of this font glyph in pixels
    K_UCHAR ucVOffset;        //!< Vertical offset of this glyph
    K_UCHAR aucData[1];        //!< Glyph data array
} Glyph_t;

//---------------------------------------------------------------------------
/*!
    The size of the glyph is the width*height (in bytes), plus the overhead 
    of the struct parameters.
*/
#define GLYPH_SIZE(x) \
    (((K_USHORT)((x->ucWidth + 7) >> 3) * (K_USHORT)(x->ucHeight)) + sizeof(Glyph_t) - 1)

//---------------------------------------------------------------------------
typedef struct
{
    K_UCHAR ucSize;
    K_UCHAR ucFlags;
    K_UCHAR ucStartChar;
    K_UCHAR ucMaxChar;
    const K_CHAR *szName;
    const FONT_STORAGE_TYPE *pucFontData;    
} Font_t;

#endif

