/*

	file:	background.h
	desc:	BACKGROUND tool header.

	note:	This module provides basic block-reference drawing functions.
		  	Designed for drawing game maps, etc.

	todo: 	Implement version for multi-block reference. (for example: one addres for 16*16 block)

	author: Jaromir Dvorak (md@unicode.cz)

*/


/* ########## BEGIN OF USER CONFIGURABLE SECTION ########## */

//
//Define which versions you want to compile.
//
#define BACKGROUND_DRAW
#define BACKGROUND_DRAW_RLE
//#define BACKGROUND_DRAW_SIMPLE

	
/* ########### END OF USER CONFIGURABLE SECTION ########### */
 

/*
 *
 * Function prototypes.
 *
 */

//
//Draws a raw (uncompressed) block image map img to window wnd with start left top block x, y in the selected window.
//Simplest version. does not handle any overflows.
//
void background_draw_simple   ( unsigned char x, unsigned char y, PGM_P img, unsigned char width, unsigned char height );

//
//Draws a raw (uncompressed) block image map img to window wnd with start left top block x, y in the selected window.
//Handles overflow - window and positioned image intersection is drawn only.
//Negative coords accepted.
//
unsigned char background_draw ( signed char x, signed char y, PGM_P img, unsigned char width, unsigned char height );

//
//Draws a RLE-compressed block image map img to window wnd with start left top block x, y in the selected window.
//Handles overflow - window and positioned image intersection is drawn only.
//Negative coords accepted.
//
void background_draw_RLE      ( signed char x, signed char y, PGM_P img, unsigned char width, unsigned char  height );

