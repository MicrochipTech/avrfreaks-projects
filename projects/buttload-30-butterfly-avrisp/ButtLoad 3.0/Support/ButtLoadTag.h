/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

/*
   ButtTag.H: This file can be included in user projects to
   allow the embedding of “ButtTags”, small strings of text
   which may be read by ButtLoad (if the program is stored in
   ButtLoad's non-volatile memory) or by other C management
   programs. Each ButtTag must have a unique name and may be
   of any length, however ButtLoad will only display the first
   20 characters of each tag's contents.
*/

#ifndef BLTAG_H
#define BLTAG_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	// STRUCTS:
	struct ButtLoadData
	{
		char MagicString[4];
		char TagData[];
	};
	
	// DEFINES:
	#define BT_TAGHEADER          {'@','(','#',')'}
	#define BUTTLOADTAG(id, data) const struct ButtLoadData BUTTTAG_##id PROGMEM = {MagicString: BT_TAGHEADER, TagData: data}

#endif
