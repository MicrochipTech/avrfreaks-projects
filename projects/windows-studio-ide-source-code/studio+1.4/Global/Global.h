// Global.h
// Global functions & definitions

#ifndef GLOBAL_H
#define GLOBAL_H

// Default (fixed) tab size
#define DEFAULTTABCHARS 4

// Common Strings
#define STRING_VERSION "Version 1.4"
#define STRING_VERSIONSHORT "1.4"
#define STRING_NAME "Studio+"
#define STRING_FLASH "Flash Microsystems"
#define STRING_COPYRIGHT "Copyright © 2001-2002 Flash Microsystems Ltd"
#define STRING_FLASHURL "www.flash.co.uk"
#define STRING_FLASHEMAIL "fdsales@attglobal.net"

// Registry Keys
#define COMPANYKEY STRING_FLASH
#define PRODUCTKEY STRING_NAME
#define VERSIONKEY STRING_VERSIONSHORT

// Global Colours
#define WHITE 0x00ffffff
#define BLACK 0x00000000

// Syntax hilighting text colours
#define TEXTCOLOUR BLACK				// Black
#define COMMENTCOLOUR 0x00008000		// Dark Green
#define KEYWORDCOLOUR 0x00ff0000		// Blue
#define DIRECTIVECOLOUR 0x00AA0088		// Dark Magenta
#define STRINGCOLOUR 0x00606060			// Dark Gray
#define CHARCOLOUR 0x00606060			// Dark Gray
//#define BRACKETCOLOUR 0x00000080		// Dark Red

// Breakpoint types
#define NOBREAKPOINT 0
#define SOFTBREAKPOINT 1
#define HARDBREAKPOINT 2

// Variable (& Symbol) types
#define VARIABLE_REGISTER 0x0001
#define VARIABLE_LOCAL 0x0002
#define VARIABLE_STATIC 0x0004
#define VARIABLE_SYMBOL 0x0008
#define VARIABLE_TYPE 0x0010
#define VARIABLE_ASSEMBLER 0x8000
#define VARIABLE_TYPEMASK 0x7FFF
#define VARIABLE_ASSEMBLERMASK 0x8000

// Context menu IDs
#define CONTEXTMENUID_PROJECT 14000
#define CONTEXTMENUID_SOURCEFILE 14001
#define CONTEXTMENUID_FUNCTION 14002
#define CONTEXTMENUID_VARIABLE 14003

// Error codes
#define SPERROR_SUCCESS 0
#define SPERROR_NOTINSTALLED 1
#define SPERROR_MISSINGIDE 2
#define SPERROR_IDENOTLICENSED 3
#define SPERROR_MISSINGFAMILY 4
#define SPERROR_FAMILYNOTLICENSED 5
#define SPERROR_BADPROJECT 6
#define SPERROR_BADLISTING 7
#define SPERROR_BADDEBUG 8
#define SPERROR_BADSOURCELIST 9
#define SPERROR_MISSINGSOURCE 10
#define SPERROR_NOISDVECTOR 11
#define SPERROR_NOISD 12
#define SPERROR_NOCODE 13
#define SPERROR_PROJECTNOTASSEMBLER 14

#define SPERROR_2KLIMIT 255

// ISD Signature Offsets
#define ISD_SIGNATURE_START 2
#define ISD_SIGNATURE_END 6

#endif // GLOBAL_H
