;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: LCD/USART texts
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************



;************************************************************************************************
;	0x00-terminated text strings (0x0D - break line (for LCD always goto 2. line)) 
;************************************************************************************************

	.if	LING==0
	;english
TDIFail:	.db	"No IDE device found!",0
TWDInit:	.db	"Waiting for IDE dev.",0
TFTnotMbr:	.db	"Missing master boot",0x0D,"record!",0
TFTnotFAT16:	.db	"Not FAT16 file",0x0D,"system!",0
TFTnotFBR:	.db	"Missing FAT16 boot",0x0D,"record!",0
TSRchk:	.db	"External RAM check:",0x0D,0
TSRfail:	.db	"test failed!",0
TUkB:	.db	"kB",0
TUMB:	.db	"MB",0
TRData:	.db	"Send data ... ",0
TCPmod:	.db	"From COM port:",0
TRDataD:	.db	"done",0x0D,0
TModN:	.db	"Title: ",0
TSngs:	.db	" songs",0


	.elif	LING==1
	;cesky
TDIFail:	.db	"Nenalezeno IDE zar.!",0
TWDInit:	.db	"Cekam na IDE zariz.",0
TFTnotMbr:	.db	"Nenalezen master",0x0D,"boot record!",0
TFTnotFAT16:	.db	"1. partition neni",0x0D,"typu FAT16!",0
TFTnotFBR:	.db	"Nenalezen FAT16 boot",0x0D,"record!",0
TSRchk:	.db	"Test externi RAM:",0x0D,0
TSRfail:	.db	"chyba!",0
TUkB:	.db	"kB",0
TUMB:	.db	"MB",0
TRData:	.db	"Posli data ... ",0
TCPmod:	.db	"Z COM portu:",0
TRDataD:	.db	"ok",0x0D,0
TModN:	.db	"Nazev: ",0
TSngs:	.db	" skladeb",0
	.endif