;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Tue May 28 09:30:35 2013
;--------------------------------------------------------
	.module wvideo
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _CX80_System_FNC_EntryPoint
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_CX80_System_FNC_EntryPoint::
	.ds 2
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;wvideo.c:10: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;wvideo.c:12: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;wvideo.c:14: CX80_System_FNC_EntryPoint[CX80_SYSTEM_FNC_REG_ADDR] = CX80_FNC_INVERT_VIDEO_OUTPUT;
	ld	hl,#0x0000
	ld	(hl),#0x16
;wvideo.c:15: CX80_System_FNC_EntryPoint[CX80_SYSTEM_REG1L_ADDR] = 1;
	ld	l, #0x01
	ld	(hl),#0x01
;wvideo.c:17: CX80_DO_SYSTEM_CALL;
	call #0x0007
;wvideo.c:19: return 0;
	ld	l, #0x00
	ret
_main_end::
	.area _CODE
	.area _CABS (ABS)
