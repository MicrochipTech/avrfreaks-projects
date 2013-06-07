;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Thu May 23 11:08:44 2013
;--------------------------------------------------------
	.module file
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _WriteBufferToFile
	.globl _CloseFile
	.globl _CreateFile
	.globl _puts
	.globl _FileID
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
_FileID::
	.ds 1
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
;file.c:28: unsigned char CreateFile(char *FileName)
;	---------------------------------
; Function CreateFile
; ---------------------------------
_CreateFile_start::
_CreateFile:
	push	ix
	ld	ix,#0
	add	ix,sp
;file.c:31: unsigned short pFileName = (unsigned short)((char *)&FileName[0]);
	ld	e,4 (ix)
	ld	d,5 (ix)
;file.c:35: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = FA_WRITE | FA_CREATE_ALWAYS;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,#0x0A
	ld	(bc),a
;file.c:37: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pFileName);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;file.c:38: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pFileName);
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	a,0 (iy)
	add	a, #0x04
	ld	c,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	b,a
	ld	a, d
	ld	(bc),a
;file.c:41: *CX80_System_FNC_EntryPoint = CX80_FNC_OPEN_CREATE_FILE;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x17
;file.c:44: CX80_DO_SYSTEM_CALL;
	call #0x0007
;file.c:50: return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	inc	hl
	ld	l,(hl)
	pop	ix
	ret
_CreateFile_end::
;file.c:53: void CloseFile(char fID)
;	---------------------------------
; Function CloseFile
; ---------------------------------
_CloseFile_start::
_CloseFile:
;file.c:56: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	iy,#2
	add	iy,sp
	ld	a,0 (iy)
	ld	(de),a
;file.c:59: *CX80_System_FNC_EntryPoint = CX80_FNC_CLOSE_FILE;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x1A
;file.c:62: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_CloseFile_end::
;file.c:65: unsigned char WriteBufferToFile(char fID, char *wBuffer, unsigned short len)
;	---------------------------------
; Function WriteBufferToFile
; ---------------------------------
_WriteBufferToFile_start::
_WriteBufferToFile:
	push	ix
	ld	ix,#0
	add	ix,sp
;file.c:68: unsigned short pBuffer = (unsigned short)((char *)&wBuffer[0]);
	ld	e,5 (ix)
	ld	d,6 (ix)
;file.c:71: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,4 (ix)
	ld	(bc),a
;file.c:73: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;file.c:74: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	a,0 (iy)
	add	a, #0x04
	ld	c,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	b,a
	ld	a, d
	ld	(bc),a
;file.c:76: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3L_ADDR) = CX80_LoByte(len);
	ld	a,0 (iy)
	add	a, #0x05
	ld	e,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	d,a
	ld	a, 7 (ix)
	ld	(de),a
;file.c:77: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3H_ADDR) = CX80_HiByte(len);
	ld	a,0 (iy)
	add	a, #0x06
	ld	e,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	d,a
	ld	a, 8 (ix)
	ld	(de),a
;file.c:80: *CX80_System_FNC_EntryPoint = CX80_FNC_WRITE_FILE_BUFFER;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x1D
;file.c:83: CX80_DO_SYSTEM_CALL;
	call #0x0007
;file.c:89: return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	inc	hl
	ld	l,(hl)
	pop	ix
	ret
_WriteBufferToFile_end::
;file.c:92: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;file.c:96: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;file.c:98: puts("Create ftest.txt ...\n\r");
	ld	hl,#__str_0
	push	hl
	call	_puts
;file.c:99: FileID = CreateFile("ftest.txt");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_CreateFile
	pop	af
	ld	iy,#_FileID
	ld	0 (iy),l
;file.c:101: if (FileID < 3)
	ld	a,0 (iy)
	sub	a, #0x03
	jr	NC,00105$
;file.c:105: if (WriteBufferToFile(FileID, "CX80 FILE TEST\n\r", 16) != 0)
	ld	hl,#__str_2
	ld	bc,#0x0010
	push	bc
	push	hl
	ld	a,(_FileID)
	push	af
	inc	sp
	call	_WriteBufferToFile
	pop	af
	pop	af
	inc	sp
	ld	a,l
	or	a, a
	jr	Z,00102$
;file.c:106: puts("Error while write on file!\n\r");
	ld	hl,#__str_3
	push	hl
	call	_puts
	pop	af
	jr	00103$
00102$:
;file.c:108: puts("Write ok\n\r");
	ld	hl,#__str_4
	push	hl
	call	_puts
	pop	af
00103$:
;file.c:110: CloseFile(FileID);
	ld	a,(_FileID)
	push	af
	inc	sp
	call	_CloseFile
	inc	sp
	jr	00106$
00105$:
;file.c:113: puts("Error on file creation!\n\r");
	ld	hl,#__str_5
	push	hl
	call	_puts
	pop	af
00106$:
;file.c:115: return 0;
	ld	hl,#0x0000
	ret
_main_end::
__str_0:
	.ascii "Create ftest.txt ..."
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_1:
	.ascii "ftest.txt"
	.db 0x00
__str_2:
	.ascii "CX80 FILE TEST"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_3:
	.ascii "Error while write on file!"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_4:
	.ascii "Write ok"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_5:
	.ascii "Error on file creation!"
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
