
.EQU	DATARAM_START=$60
.EQU	DATARAM_END=$FF


.DSEG

;*************************************************************************
;*************************************************************************
;   SOFT TIMERS SRAM ADDRESSES
;*************************************************************************
;*************************************************************************

RAM_STIMER1_CV:	.BYTE	1
RAM_STIMER1_PV:	.BYTE	1



RAM_DISPLAY_BUFFER_LINE1:	.BYTE	20
RAM_DISPLAY_BUFFER_LINE2:	.BYTE	20

RAM_LCD_POINTER:			.BYTE	2
RAM_CHRONOMETER:			.BYTE	4