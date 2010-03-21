//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file font_port.h

	Description:
		Hardware-specific module for reading bytes from the font table.  
		Abstracting access out at this level allows for different storage 
		classes and methods to be used without requiring the drawing functions
		to be rewritten.  This example uses directly-addressible storage.
*/
//---------------------------------------------------------------------------
#ifndef __FONT_PORT_H__
#define __FONT_PORT_H__

//---------------------------------------------------------------------------
// Definition for the storage type of the font character data.
#define FONT_RAW_TYPE		UCHAR

//---------------------------------------------------------------------------
// Macro for reading a byte from the font table
#define Font_ReadByte(usAddress, pucFont)  (pucFont[usAddress])

#endif

