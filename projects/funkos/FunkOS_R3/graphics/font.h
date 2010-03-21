//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  font.h
	
	Description:
		Functions related to accessing fonts.
*/
//--------------------------------------------------------------------------- 
#ifndef __FONT_H__
#define __FONT_H__

#include "types.h"
#include "font_port.h"

//---------------------------------------------------------------------------
// Font library configuration (determines the size of the font table)
#define FONT_PRINTABLE_CHARS	(256)
#define FONT_ASCII_OFFSET		(0)

//---------------------------------------------------------------------------
// Font flags
#define FONT_FLAG_NORMAL		(0x00)
#define FONT_FLAG_BOLD			(0x01)
#define FONT_FLAG_ITALIC		(0x02)
#define FONT_FLAG_UNDERLINE		(0x04)
#define FONT_FLAG_SUBSCRIPT		(0x08)
#define FONT_FLAG_SUPERSCRIPT	(0x10)
#define FONT_FLAG_STRIKETHROUGH	(0x20)

//---------------------------------------------------------------------------
// Font structure definition
typedef struct
{
	CHAR *szFontName;	//!< Name of the font (i.e. windows TTF name)
	UCHAR ucSize;		//!< Size of the font (i.e. windows TTF font size)
	UCHAR ucFlags;		//!< Font properties (Bold, italic, underline, etc.)
	
	//-- Values below are used to calculate the size of the font table and font indexing.
	UCHAR ucFontWidth;	//!< Width of the font characters in pixels (max of the whole font)
	UCHAR ucFontHeight; //!< Height of the font characters in pixels (max of the whole font)
	UCHAR ucStartChar;	//!< Starting character (ASCII char index)
	UCHAR ucNumChars;	//!< Number of chars	
	const FONT_RAW_TYPE *pucCharData;	//!< Pointer to characters used in the font 
} FONT_STRUCT;

//---------------------------------------------------------------------------
const FONT_STRUCT *Font_Load(CHAR *szFontName_, UCHAR ucFontWidth_, UCHAR ucFlags_);

#endif
