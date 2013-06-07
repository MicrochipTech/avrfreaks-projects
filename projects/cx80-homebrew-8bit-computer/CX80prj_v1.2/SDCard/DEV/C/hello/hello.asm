;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Tue May 21 11:10:07 2013
;--------------------------------------------------------
	.module hello
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _MoveCursor
	.globl _ClearScreen
	.globl _printf
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
;hello.c:27: void ClearScreen(void)
;	---------------------------------
; Function ClearScreen
; ---------------------------------
_ClearScreen_start::
_ClearScreen:
;hello.c:30: *CX80_System_FNC_EntryPoint = CX80_FNC_CLEAR_SCREEN;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x06
;hello.c:33: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_ClearScreen_end::
;hello.c:36: void MoveCursor(char row, char column)
;	---------------------------------
; Function MoveCursor
; ---------------------------------
_MoveCursor_start::
_MoveCursor:
;hello.c:44: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = row;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	hl, #2+0
	add	hl, sp
	ld	a, (hl)
	ld	(de),a
;hello.c:45: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = column;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	inc	de
	ld	iy,#3
	add	iy,sp
	ld	a,0 (iy)
	ld	(de),a
;hello.c:48: *CX80_System_FNC_EntryPoint = CX80_FNC_MOVE_CURSOR;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0B
;hello.c:51: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_MoveCursor_end::
;hello.c:54: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;hello.c:58: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;hello.c:60: ClearScreen();
	call	_ClearScreen
;hello.c:62: MoveCursor(2, 13);
	ld	hl,#0x0D02
	push	hl
	call	_MoveCursor
;hello.c:65: printf("Hello World!\n\r");
	ld	hl, #__str_0
	ex	(sp),hl
	call	_printf
	pop	af
;hello.c:68: return 0;
	ld	hl,#0x0000
	ret
_main_end::
__str_0:
	.ascii "Hello World!"
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
