;*****************************************************************
;***	Strings stored in program memory
;*****************************************************************
; Tokens are used for enhanced printing as follows:
;	0xf8 before a string will center it using the constant LNLEN
;		as the maximum line length.
;	0xf9 followed by a number then a printable character will
;		repeat that character the number of times specified.  
;	0xfa emits a CR/LF combination.
;	0xfb emits the string "Error ".
;
; !!!! WARNING: do not allow the assembler to pad these !!!!
;		strings with 0x00, fill with a space at the end.
;*****************************************************************
;
.equ	TOK_CTR		= 0xF8		;the "center line" token
.equ	TOK_REP		= 0xF9		;the "repeat character" token
.equ	TOK_CRLF	= 0xFA		;the CR/LF token
.equ	TOK_ERR		= 0xFB		;the "Error" string token
;
#if ( (FCLK/10000000) == 0 )
  #define CLOCKRATE	$20,(FCLK/1000000)+$30
#else
  #define CLOCKRATE	(FCLK/10000000)+$30,((FCLK/1000000)%10)+$30
#endif

signonmessage:
#if defined(__ATmega88__) && TEST
	.db     "AttoBASIC ATmega88 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30," ",TOK_CRLF
#elif defined(__ATmega88PA__) && TEST
	.db     "AttoBASIC ATmega88PA Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30," ",TOK_CRLF
#elif defined(__ATmega168__) && TEST
	.db     "AttoBASIC ATmega168 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30,TOK_CRLF
#elif defined(__ATmega168PA__) && TEST
	.db     "AttoBASIC ATmega168PA Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30,TOK_CRLF
#elif defined(__ATmega328__) && TEST
	.db     "AttoBASIC ATmega328 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30,TOK_CRLF
#elif defined(__ATmega328PA__) && TEST
	.db     "AttoBASIC ATmega328PA Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30,TOK_CRLF
#elif defined(__ATmega32U4__) && TEST
	.db     "AttoBASIC ATmega32U4 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30," ",TOK_CRLF
#elif defined(__AT90USB1286__) && TEST
	.db     "AttoBASIC AT90USB1286 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30," ",TOK_CRLF
#elif defined(__ATmega2560__) && TEST
	.db     "AttoBASIC ATmega2560 Ver: ", \
		(Version/100)+$30,'.',\
		((Version%100)/10)+$30, \
		((Version%100)%10)+$30," ",TOK_CRLF
#endif
;
#if !TEST
	.db		TOK_REP,LNLEN,"=",TOK_CRLF
;	.db     TOK_CTR,"AttoBASIC Ver: ", \
;		(Version/100)+$30,'.',\
;		((Version%100)/10)+$30, \
;		((Version%100)%10)+$30," ",TOK_CRLF
;	.db     TOK_CTR,"AttoBASIC ",TOK_CRLF
#endif
;
#if defined(__ATmega88__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega88 @",CLOCKRATE,"MHz ",TOK_CRLF
#elif defined(__ATmega88PA__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega88PA @",CLOCKRATE,"MHz ",TOK_CRLF
#elif defined(__ATmega168__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega168 @",CLOCKRATE,"MHz",TOK_CRLF
#elif defined(__ATmega168PA__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega168PA @",CLOCKRATE,"MHz",TOK_CRLF
#elif defined(__ATmega328__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega328P @",CLOCKRATE,"MHz ",TOK_CRLF
#elif defined(__ATmega328PA__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega328PA @",CLOCKRATE,"MHz",TOK_CRLF
#elif defined(__ATmega32U4__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega32U4 @",CLOCKRATE,"MHz ",TOK_CRLF
#elif defined(__AT90USB1286__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for AT90USB1286 @",CLOCKRATE,"MHz",TOK_CRLF
#elif defined(__ATmega2560__) && !TEST && !DREC
	.db     TOK_CTR,"AttoBASIC for ATmega2560 @",CLOCKRATE,"MHz ",TOK_CRLF
#elif !TEST && DREC
	.db		TOK_CTR,"AVR Data Recorder ",TOK_CRLF
	.db		TOK_CTR,"Using AttoBASIC for ATmega328P @",CLOCKRATE,"MHz ",TOK_CRLF
#endif

#if !TEST	; generate the build code
	.db		TOK_CTR,"Ver: ", \
			(Version/100)+$30,'.',\
			((Version%100)/10)+$30, \
			((Version%100)%10)+$30," ", \
			"Build code: ", \
			(__CENTURY__/10)+0x30, \
			(__CENTURY__%10)+0x30, \
			(__YEAR__/10)+0x30,(__YEAR__%10)+0x30, \
			(__MONTH__/10)+0x30,(__MONTH__%10)+0x30, \
			(__DAY__/10)+0x30,(__DAY__%10)+0x30, \
			".",(__HOUR__/10)+0x30, \
			(__HOUR__%10)+0x30,(__MINUTE__/10)+0x30, \
			(__MINUTE__%10)+0x30,TOK_CRLF,TOK_REP,LNLEN,"-"
#endif
	.dw	0
;
hellomessage:
#if !TEST && DREC
	.db		TOK_CTR,"Original AttoBASIC (C)2002 by Richard Cappels ",TOK_CRLF
	.db		TOK_CTR,"http://projects.cappels.org  projects@cappels.org ",TOK_CRLF 
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
	.db		TOK_CTR,"Data Recorder & Enhancements (C)2011, 2012",TOK_CRLF
	.db		TOK_CTR," by K. Scott Vitale (ksv_prj@gmx.com) ",TOK_CRLF
	.db		TOK_REP,LNLEN,"=",TOK_CRLF
#elif !TEST && !DREC
	.db		TOK_CTR,"Original code (C)2002 by Richard Cappels",TOK_CRLF
	.db		TOK_CTR,"http://projects.cappels.org  projects@cappels.org ",TOK_CRLF 
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
	.db		TOK_CTR,"Enhancements by K. Scott Vitale (ksv_prj@gmx.com) ",TOK_CRLF
#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) ); reduce sign-on message size, be specific
	.db		TOK_CTR,"ATmega88/168/328 port (C)2011 ",TOK_CRLF
#elif defined(__ATmega32U4__)	; reduce sign-on message size, be specific
	.db		TOK_CTR,"ATmega32U4 port (C)2012 ",TOK_CRLF
#elif defined(__ATmega2560__)	; reduce sign-on message size, be specific
	.db		TOK_CTR,"ATmega2560 port (C)2013 ",TOK_CRLF
#elif defined(__AT90USB1286__); reduce sign-on message size, be specific
	.db		TOK_CTR,"AT90USB1286 port (C)2013",TOK_CRLF
#endif
;
#if ( BTLDR && defined(__AT90USB1286__) ); reduce sign-on message size, be specific
	.db		TOK_CTR,"AT90USB1286 USB & DFU Bootloader support (C)2013",TOK_CRLF
#elif ( BTLDR && defined(__ATmega32U4__) ); reduce sign-on message size, be specific
	.db		TOK_CTR,"Mega32U4 USB & DFU Bootloader support (C)2012 ",TOK_CRLF
#endif
.if OWR					; reduce sign-on message size, be specific
	.db		TOK_CTR,"1-Wire(R) support (C)2013 ",TOK_CRLF
.endif
.if NRF					; reduce sign-on message size, be specific
	.db		TOK_CTR,"nRF24L01(+) RF Transceiver support (C)2013",TOK_CRLF
.endif
.if DHT					; reduce sign-on message size, be specific
	.db		TOK_CTR,"DHT Humidity and Temperature support (C)2013",TOK_CRLF
.endif
.if EFS					; reduce sign-on message size, be specific
	.db		TOK_CTR,"EEP File System support (C)2013 ",TOK_CRLF
.endif
.if (RTC || LPMD || DATA); reduce sign-on message size, be specific
	.db		TOK_CTR,"Sleep, RTC and Data-file support (C)2012",TOK_CRLF
.endif
.if (TWI || SPI || DDS || ICP); reduce sign-on message size, be specific
	.db		TOK_CTR,"TWI/SPI/DDS & Input Capture (C)2011 ",TOK_CRLF
.endif
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
  #if USB && !DREC	; reduce sign-on message size, be specific
	.db		TOK_CTR,"USB Serial I/O (C)2008,2010 PJRC.COM, LLC ",TOK_CRLF
	.db		TOK_CTR,"http://www.pjrc.com/teensy/ ",TOK_CRLF 
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
  #endif
  #if BTLDR && ( defined(__ATmega168__) || \
  		defined(__ATmega168PA__) || \
		defined(__ATmega328__) || \
		defined(__ATmega328PA__) )	; reduce sign-on message size, be specific
  	.db		TOK_CTR,"OptiBoot Bootloader based on stk500boot.c by",TOK_CRLF
	.db		TOK_CTR,"Jason Kyle, Peter Knight, Bill Westfield & Ladyada",TOK_CRLF
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
  #elif BTLDR && defined(__ATmega2560__); reduce sign-on message size, be specific
	.db     TOK_CTR,"STK500v2 Bootloader Peter Fleury <pfleury@gmx.ch> ",TOK_CRLF
	.db     TOK_CTR," and Worapoht Kornkaewwattanakul <dev@avride.com> ",TOK_CRLF 
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
  #elif BTLDR &&  ( defined(__ATmega32U4__) || \
  					defined(__AT90USB1286__) ); reduce sign-on message size, be specific
	.db     TOK_CTR,"LUFA Bootloader Written by Dean Camera",TOK_CRLF
	.db     TOK_CTR,"dean@fourwalledcubicle.com - http://www.lufa-lib.org",TOK_CRLF 
	.db		TOK_REP,LNLEN,"-",TOK_CRLF
  #endif
	.db     TOK_CTR,"All rights and remedies expressly reserved",TOK_CRLF
	.db		TOK_CTR,"Not for commercial use",TOK_CRLF
	.db		TOK_REP,LNLEN,"=",TOK_CRLF
#endif
	.dw     0
;
perferate:	;for the dump commands
	.db		TOK_REP,((DUMPBYTES * 3) + 5),"-",TOK_CRLF
	.dw		0
;
.if DEBUG
MSG_HEADER:
	.db		TOK_REP,6," ","00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ",TOK_CRLF
	.dw		0

MSG_VRAM:
	.db		"Variables in RAM:",TOK_CRLF
	.dw		0
;
MSG_E2PROM:
	.db		"EEP Contents:",TOK_CRLF
	.dw		0
;
MSG_PROG:
	.db		"PROGRAM Memory Contents:",TOK_CRLF,0
	.dw		0
;
pc_nextavail:
	.db		"Next available location: ",0
	.dw		0
.endif
;
MSG_E2PROMSAVE2BIG:
	.db		TOK_ERR,": Program too large for EEP!",TOK_CRLF
	.dw		0
;
MSG_EEPERASE:
	.db		"Erasing EEP "
	.dw		0

MSG_E2PROMSAVE:
.if !EFS
	.db		"Saving to EEP.",TOK_CRLF,0
.endif
	.dw		0
;
MSG_SAVEPROGNUM:
.if EFS
	.db		"Saving File # "
.endif
	.dw		0
;
MSG_E2PROMLOAD:
.if !EFS
	.db		"Loading from EEP",TOK_CRLF,0
.endif
	.dw		0
;
MSG_LOADPROGNUM:
.if EFS
	.db		"Loading File # ",0
.endif
	.dw		0
;
MSG_CATEMPTY:
	.db		"Empty",0
	.dw		0
;
MSG_BYTES:
.if EFS
	.db		" bytes > ",0
.endif
	.dw		0
;
breakmsgtxt:
	.db		"Break",TOK_CRLF
	.dw		0
;
usedmem:
	.db		" bytes used.  "
	.dw		0
;	
freemem:
	.db		" bytes free. ",TOK_CRLF
	.dw		0
;	
invokebldr1:
	.db		TOK_CRLF,"Exit your terminal program."
	.dw		0
invokebldr2:
	.db		TOK_CRLF,"Invoking bootloader (in 8 secs) at 0x"
	.dw		0

#if ( defined(__ATmega328__) || \
	  defined(__ATmega328PA__) || \
	  defined(__AT90USB1286__)  || \
	  defined(__ATmega2560__) || \
	  defined(__ATmega32U4__) )
WTFmsg1:
	.db		TOK_CTR,"42 is the Answer to the Ultimate Question ",TOK_CRLF
	.db		TOK_CTR,"of Life, the Universe, and Everything ",TOK_CRLF
	.dw		0
;
WTFmsg2:
	.db		TOK_CTR,"So Long, and Thanks for All the Fish",TOK_CRLF
	.dw		0
#endif
;
;==================================================================================
;ERROR messages
;==================================================================================
errlinenum:
	.db		"Line # ",0
	.dw		0

emessage:
	.db		"Error "
	.dw		0

emessage00:
	.db		TOK_ERR,"0: Unknown command or operator."
	.dw		0
;
emessage01:
	.db		TOK_ERR,"1: Value not between 0:255."
	.dw		0
;
emessage02:
	.db		TOK_ERR,"2: Program memory is full"
	.dw		0
;
emessage03:
	.db		TOK_ERR,"3: Character not a recognized type."
	.dw		0
;
emessage04:
	.db		TOK_ERR,"4:internal error - buffer limit exceeded."
	.dw		0
;
emessage05:
	.db		TOK_ERR,"5: Bad variable name."
	.dw		0
;
emessage06:
	.db		TOK_ERR,"6: Too many arguments"
	.dw		0
;
emessage07:
	.db		TOK_ERR,"7: Value was not available."
	.dw		0
;	
emessage08:
	.db		TOK_ERR,"8: Machine stack error",TOK_CRLF
	.db		TOK_REP,9," ","Functions too complex or calls nested too deeply",0
	.dw		0
;	
emessage09:
	.db		TOK_ERR,"9: GOTO or GOSUB destination not found."
	.dw		0
;
emessage0A:
	.db		TOK_ERR,"A: Exceeded range of 0:7",0
	.dw		0
;
emessage0B:
	.db		TOK_ERR,"B: Line number > 3 digits"
	.dw		0
;
emessage0C:
	.db		TOK_ERR,"C: Exceeded range of 0:1",0
	.dw		0
;
emessage0D:
	.db		TOK_ERR,"D: Exceeded range of 0:3",0
	.dw		0
;
emessage0E:
.if ICP
	.db		TOK_ERR,"E: ICP register overflow."
.endif;
	.dw		0
;
emessage0F:
	.db		TOK_ERR,"F: Arithmetic overflow."
	.dw		0
;
emessage10:
	.db		TOK_ERR,"10: Arithmetic underflow."
	.dw		0
;
emessage11:
	.db		TOK_ERR,"11: Divide by zero."
	.dw		0
;
emessage12:
.if TWI
	.db		TOK_ERR,"12: TWI bus error"
.endif;
	.dw		0
;
emessage13:
.if TWI
	.db		TOK_ERR,"13: TWI not enabled"
.endif;
	.dw		0
;
emessage14:
.if TWI
	.db		TOK_ERR,"14: TWI Slave replied with NAK."
.endif;
	.dw		0
;
emessage15:
	.db		TOK_ERR,"15: No Boot-loader found."
	.dw		0
;
emessage16:
	.db		TOK_ERR,"16: Specified value exceeded range."
	.dw		0
;
emessage17:
	.db		TOK_ERR,"17: Set AC Ref > 0."
	.dw		0
;
emessage18:
.if ( DHT || OWR || DSI )
	.db		TOK_ERR,"18: Device non-response. "
.endif
	.dw		0

emessage19:
.if ( DHT || OWR || DSI )
	.db		TOK_ERR,"19: Device checksum. "
.endif
	.dw		0
;
emessage1A:
.if SPI
	.db		TOK_ERR,"1A: SPI interface not enabled"
.endif;
	.dw		0

emessage1B:
	.db		TOK_ERR,"1B:"
	.dw		0

emessage1C:
	.db		TOK_ERR,"1C: Not a binary number"
	.dw		0

emessage1D:
	.db		TOK_ERR,"1D: Invalid number of arguments"
	.dw		0

emessage1E:
	.db		TOK_ERR,"1E:"
	.dw		0

emessage1F:
;	.db		TOK_ERR,"1F: "
	.dw		0
