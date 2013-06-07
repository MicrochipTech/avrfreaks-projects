;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Wed May 22 10:54:30 2013
;--------------------------------------------------------
	.module getline
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _GetLine
	.globl _printf
	.globl _InputBuffer
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
_InputBuffer::
	.ds 30
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
;getline.c:29: void GetLine(char *Buffer, char len)
;	---------------------------------
; Function GetLine
; ---------------------------------
_GetLine_start::
_GetLine:
	push	ix
	ld	ix,#0
	add	ix,sp
;getline.c:32: unsigned short pBuffer = (unsigned short)((char *)&Buffer[0]);
	ld	e,4 (ix)
	ld	d,5 (ix)
;getline.c:41: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = len;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,6 (ix)
	ld	(bc),a
;getline.c:42: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;getline.c:43: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
	ld	iy,#0x0004
	ld	bc,(_CX80_System_FNC_EntryPoint)
	add	iy, bc
	ld	0 (iy), d
;getline.c:46: *CX80_System_FNC_EntryPoint = CX80_FNC_GET_INPUT_STRING;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x26
;getline.c:49: CX80_DO_SYSTEM_CALL;
	call #0x0007
	pop	ix
	ret
_GetLine_end::
;getline.c:52: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;getline.c:56: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;getline.c:59: printf("Example of getline system call\n\r");
	ld	hl,#__str_0
	push	hl
	call	_printf
;getline.c:60: printf("Type a string, confirm with Return\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_printf
;getline.c:61: printf("->");
	ld	hl, #__str_2
	ex	(sp),hl
	call	_printf
	pop	af
;getline.c:63: GetLine(InputBuffer, 30);
	ld	de,#_InputBuffer
	ld	a,#0x1E
	push	af
	inc	sp
	push	de
	call	_GetLine
	pop	af
	inc	sp
;getline.c:65: printf("\n\rTyped:%s\n\r", InputBuffer); 
	ld	hl,#__str_3
	ld	bc,#_InputBuffer
	push	bc
	push	hl
	call	_printf
	pop	af
	pop	af
;getline.c:68: return 0;
	ld	hl,#0x0000
	ret
_main_end::
__str_0:
	.ascii "Example of getline system call"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_1:
	.ascii "Type a string, confirm with Return"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_2:
	.ascii "->"
	.db 0x00
__str_3:
	.db 0x0A
	.db 0x0D
	.ascii "Typed:%s"
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
