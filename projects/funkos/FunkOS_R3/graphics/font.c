//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  font.c
	
	Description:
		Functions related to loading font tables
*/
//--------------------------------------------------------------------------- 
#include "types.h"
#include "font_table.h"

//---------------------------------------------------------------------------
/*!
	Call this function at the end of main() to start the scheduler.
	\fn const FONT_STRUCT *Font_Load(CHAR *szFontName_, UCHAR ucFontWidth_, UCHAR ucFlags_)
	\param szFontName_ - the name of the font family (alphanumeric)
	\param ucFontWidth_ - the point size of the font
	\param ucFlags_ - font flags (bold, italic, underline, etc.)
*/
const FONT_STRUCT *Font_Load(CHAR *szFontName_, UCHAR ucFontWidth_, UCHAR ucFlags_)
{
	UCHAR ucFontIndex;
	BOOL bNameMatch;
	UCHAR ucTempWidth;
	UCHAR ucTempFlags;
	UCHAR ucStrIndex;
	UCHAR ucTemp;
	// Check the font table to see if there's a matching font
	for (ucFontIndex = 0; ucFontIndex < 3; ucFontIndex++)
	{
		ucStrIndex = 0;
		bNameMatch = TRUE;
	
		// Ghetto strcmp
		ucTemp = g_astFonts[ucFontIndex].szFontName[ucStrIndex];		
		while ((ucTemp != 0) && (szFontName_[ucStrIndex] != 0))
		{
			if (ucTemp != szFontName_[ucStrIndex])
			{
				bNameMatch = FALSE;
				break;
			}
			ucStrIndex++;
			ucTemp = g_astFonts[ucFontIndex].szFontName[ucStrIndex];
		}

		// Make sure the strings both end at the same place...
		if (ucTemp != szFontName_[ucStrIndex])
		{
            continue;
    	}

		// If the name matched entirely, see if the flags match, too.
		if (bNameMatch == TRUE)
		{
			ucTempWidth = g_astFonts[ucFontIndex].ucSize;
			ucTempFlags = g_astFonts[ucFontIndex].ucFlags;

			// Check for parameter match
			if ((ucTempWidth == ucFontWidth_) && (ucFlags_ == ucTempFlags))
			{
				// Copy the font out.
				return (&g_astFonts[ucFontIndex]);
			}
		}
	}
	// Copy the first font in the table out if no matches made.	
	return (&g_astFonts[0]);
}
//---------------------------------------------------------------------------
