;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Tue May 28 10:02:43 2013
;--------------------------------------------------------
	.module screen
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _ScreenDemo
	.globl _MoveCursor
	.globl _ClearScreen
	.globl _WaitKeypressed
	.globl _puts
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
;screen.c:29: void WaitKeypressed(void)
;	---------------------------------
; Function WaitKeypressed
; ---------------------------------
_WaitKeypressed_start::
_WaitKeypressed:
;screen.c:32: *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x24
;screen.c:35: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_WaitKeypressed_end::
;screen.c:38: void ClearScreen(void)
;	---------------------------------
; Function ClearScreen
; ---------------------------------
_ClearScreen_start::
_ClearScreen:
;screen.c:41: *CX80_System_FNC_EntryPoint = CX80_FNC_CLEAR_SCREEN;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x06
;screen.c:44: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_ClearScreen_end::
;screen.c:47: void MoveCursor(char row, char column)
;	---------------------------------
; Function MoveCursor
; ---------------------------------
_MoveCursor_start::
_MoveCursor:
;screen.c:55: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = row;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	hl, #2+0
	add	hl, sp
	ld	a, (hl)
	ld	(de),a
;screen.c:56: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = column;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	inc	de
	ld	iy,#3
	add	iy,sp
	ld	a,0 (iy)
	ld	(de),a
;screen.c:59: *CX80_System_FNC_EntryPoint = CX80_FNC_MOVE_CURSOR;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0B
;screen.c:62: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_MoveCursor_end::
;screen.c:65: void ScreenDemo(void)
;	---------------------------------
; Function ScreenDemo
; ---------------------------------
_ScreenDemo_start::
_ScreenDemo:
;screen.c:67: ClearScreen();
	call	_ClearScreen
;screen.c:69: puts("Move cursor to 18, 14\n\r");
	ld	hl,#__str_0
	push	hl
	call	_puts
;screen.c:70: puts("Press key...\n\r"); 
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
;screen.c:72: MoveCursor(14, 18);
	ld	hl, #0x120E
	ex	(sp),hl
	call	_MoveCursor
	pop	af
;screen.c:74: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:76: ClearScreen();
	call	_ClearScreen
;screen.c:77: puts("Move cursor to home\n\r");
	ld	hl,#__str_2
	push	hl
	call	_puts
;screen.c:78: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:81: *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_HOME;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x07
;screen.c:84: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:86: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:88: ClearScreen();
	call	_ClearScreen
;screen.c:89: puts("Cursor in block shape\n\r");
	ld	hl,#__str_3
	push	hl
	call	_puts
;screen.c:90: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:93: *CX80_System_FNC_EntryPoint = CX80_FNC_BLOCK_CURSOR;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x14
;screen.c:96: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:98: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:100: ClearScreen();
	call	_ClearScreen
;screen.c:101: puts("Cursor in underline shape\n\r");
	ld	hl,#__str_4
	push	hl
	call	_puts
;screen.c:102: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:105: *CX80_System_FNC_EntryPoint = CX80_FNC_UNDERLINE_CURSOR;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x15
;screen.c:108: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:110: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:112: ClearScreen();
	call	_ClearScreen
;screen.c:113: puts("Hidden the cursor\n\r");
	ld	hl,#__str_5
	push	hl
	call	_puts
;screen.c:114: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:117: *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_SHOW;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0E
;screen.c:118: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 0;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a,#0x00
	ld	(de),a
;screen.c:121: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:123: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:125: ClearScreen();
	call	_ClearScreen
;screen.c:126: puts("Show the cursor\n\r");
	ld	hl,#__str_6
	push	hl
	call	_puts
;screen.c:127: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:130: *CX80_System_FNC_EntryPoint = CX80_FNC_CURSOR_SHOW;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0E
;screen.c:131: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 1;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a,#0x01
	ld	(de),a
;screen.c:134: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:136: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:138: ClearScreen();
	call	_ClearScreen
;screen.c:139: puts("Change line dimension\n\r");
	ld	hl,#__str_7
	push	hl
	call	_puts
;screen.c:140: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
;screen.c:142: MoveCursor(14, 0);
	ld	hl, #0x000E
	ex	(sp),hl
	call	_MoveCursor
	pop	af
;screen.c:145: *CX80_System_FNC_EntryPoint = CX80_FNC_SET_LINE_DIMENSION;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0C
;screen.c:146: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = '1';
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a,#0x31
	ld	(de),a
;screen.c:149: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:151: puts("NEW LINE");
	ld	hl,#__str_8
	push	hl
	call	_puts
	pop	af
;screen.c:153: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:155: ClearScreen();
	call	_ClearScreen
;screen.c:156: puts("Invert video signal\n\r");
	ld	hl,#__str_9
	push	hl
	call	_puts
;screen.c:157: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:160: *CX80_System_FNC_EntryPoint = CX80_FNC_INVERT_VIDEO_OUTPUT;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x16
;screen.c:161: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 1;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a,#0x01
	ld	(de),a
;screen.c:164: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:166: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:168: ClearScreen();
	call	_ClearScreen
;screen.c:169: puts("Restore video signal\n\r");
	ld	hl,#__str_10
	push	hl
	call	_puts
;screen.c:170: puts("Press key...\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:173: *CX80_System_FNC_EntryPoint = CX80_FNC_INVERT_VIDEO_OUTPUT;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x16
;screen.c:174: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = 0;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a,#0x00
	ld	(de),a
;screen.c:177: CX80_DO_SYSTEM_CALL;
	call #0x0007
;screen.c:179: WaitKeypressed();
	jp	_WaitKeypressed
_ScreenDemo_end::
__str_0:
	.ascii "Move cursor to 18, 14"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_1:
	.ascii "Press key..."
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_2:
	.ascii "Move cursor to home"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_3:
	.ascii "Cursor in block shape"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_4:
	.ascii "Cursor in underline shape"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_5:
	.ascii "Hidden the cursor"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_6:
	.ascii "Show the cursor"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_7:
	.ascii "Change line dimension"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_8:
	.ascii "NEW LINE"
	.db 0x00
__str_9:
	.ascii "Invert video signal"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_10:
	.ascii "Restore video signal"
	.db 0x0A
	.db 0x0D
	.db 0x00
;screen.c:182: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;screen.c:186: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;screen.c:188: ClearScreen();
	call	_ClearScreen
;screen.c:190: puts("CX80 Screen Function Demo\n\r");
	ld	hl,#__str_11
	push	hl
	call	_puts
;screen.c:191: puts("Press key...\n\r");
	ld	hl, #__str_12
	ex	(sp),hl
	call	_puts
	pop	af
;screen.c:193: WaitKeypressed();
	call	_WaitKeypressed
;screen.c:195: ScreenDemo();
	call	_ScreenDemo
;screen.c:197: return 0;
	ld	hl,#0x0000
	ret
_main_end::
__str_11:
	.ascii "CX80 Screen Function Demo"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_12:
	.ascii "Press key..."
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
