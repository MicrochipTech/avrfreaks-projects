;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Thu May 23 10:07:19 2013
;--------------------------------------------------------
	.module waitkey
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
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
;waitkey.c:26: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;waitkey.c:30: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;waitkey.c:32: printf("Press a key...");
	ld	hl,#__str_0
	push	hl
	call	_printf
	pop	af
;waitkey.c:35: *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x24
;waitkey.c:38: CX80_DO_SYSTEM_CALL;
	call #0x0007
;waitkey.c:40: printf("\n\rKey pressed = %c\n\r", *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR));
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	ld	a,(hl)
	ld	e,a
	rla
	sbc	a, a
	ld	d,a
	ld	hl,#__str_1
	push	de
	push	hl
	call	_printf
	pop	af
	pop	af
;waitkey.c:42: return 0;
	ld	hl,#0x0000
	ret
_main_end::
__str_0:
	.ascii "Press a key..."
	.db 0x00
__str_1:
	.db 0x0A
	.db 0x0D
	.ascii "Key pressed = %c"
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
