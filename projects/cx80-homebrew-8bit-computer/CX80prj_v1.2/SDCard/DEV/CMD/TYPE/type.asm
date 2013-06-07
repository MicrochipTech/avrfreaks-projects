;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.2.0 #8008 (Jul  6 2012) (MINGW32)
; This file was generated Tue May 28 09:23:23 2013
;--------------------------------------------------------
	.module type
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _GetCh
	.globl _MoveCursor
	.globl _CloseFile
	.globl _GetString
	.globl _OpenFile
	.globl _GetLine
	.globl _CX80PrintString
	.globl _strlen
	.globl _strchr
	.globl _FileID
	.globl _ReadLine
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
_ReadLine::
	.ds 38
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
;type.c:18: void CX80PrintString(char *Txt)
;	---------------------------------
; Function CX80PrintString
; ---------------------------------
_CX80PrintString_start::
_CX80PrintString:
	push	ix
	ld	ix,#0
	add	ix,sp
;type.c:20: unsigned short pBuffer = (unsigned short)((char *)&Txt[0]);
	ld	c,4 (ix)
	ld	b,5 (ix)
;type.c:22: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = CX80_LoByte(pBuffer);
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	a, c
	ld	(de),a
;type.c:23: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = CX80_HiByte(pBuffer);
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	inc	de
	ld	a,b
	ld	(de),a
;type.c:25: *CX80_System_FNC_EntryPoint = CX80_FNC_PRINT_STRING;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x00
;type.c:27: CX80_DO_SYSTEM_CALL;
	call #0x0007
	pop	ix
	ret
_CX80PrintString_end::
;type.c:30: void GetLine(char *Buffer, char len)
;	---------------------------------
; Function GetLine
; ---------------------------------
_GetLine_start::
_GetLine:
	push	ix
	ld	ix,#0
	add	ix,sp
;type.c:32: unsigned short pBuffer = (unsigned short)((char *)&Buffer[0]);
	ld	e,4 (ix)
	ld	d,5 (ix)
;type.c:34: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = len;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,6 (ix)
	ld	(bc),a
;type.c:35: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;type.c:36: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
	ld	iy,#0x0004
	ld	bc,(_CX80_System_FNC_EntryPoint)
	add	iy, bc
	ld	0 (iy), d
;type.c:38: *CX80_System_FNC_EntryPoint = CX80_FNC_GET_INPUT_STRING;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x26
;type.c:40: CX80_DO_SYSTEM_CALL;
	call #0x0007
	pop	ix
	ret
_GetLine_end::
;type.c:43: unsigned char OpenFile(char *FName)
;	---------------------------------
; Function OpenFile
; ---------------------------------
_OpenFile_start::
_OpenFile:
	push	ix
	ld	ix,#0
	add	ix,sp
;type.c:45: unsigned short pFileName = (unsigned short)((char *)&FName[0]);
	ld	e,4 (ix)
	ld	d,5 (ix)
;type.c:47: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = FA_READ;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,#0x01
	ld	(bc),a
;type.c:49: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pFileName);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;type.c:50: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pFileName);
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	a,0 (iy)
	add	a, #0x04
	ld	c,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	b,a
	ld	a, d
	ld	(bc),a
;type.c:52: *CX80_System_FNC_EntryPoint = CX80_FNC_OPEN_CREATE_FILE;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x17
;type.c:54: CX80_DO_SYSTEM_CALL;
	call #0x0007
;type.c:56: return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	inc	hl
	ld	l,(hl)
	pop	ix
	ret
_OpenFile_end::
;type.c:59: unsigned char GetString(char fID, char *buffer, unsigned short len)
;	---------------------------------
; Function GetString
; ---------------------------------
_GetString_start::
_GetString:
	push	ix
	ld	ix,#0
	add	ix,sp
;type.c:61: unsigned short pBuffer = (unsigned short)((char *)&buffer[0]);
	ld	e,5 (ix)
	ld	d,6 (ix)
;type.c:63: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	ld	a,4 (ix)
	ld	(bc),a
;type.c:64: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2L_ADDR) = CX80_LoByte(pBuffer);
	ld	bc,(_CX80_System_FNC_EntryPoint)
	inc	bc
	inc	bc
	inc	bc
	ld	a, e
	ld	(bc),a
;type.c:65: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG2H_ADDR) = CX80_HiByte(pBuffer);
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	a,0 (iy)
	add	a, #0x04
	ld	c,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	b,a
	ld	a, d
	ld	(bc),a
;type.c:66: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3L_ADDR) = CX80_LoByte(len);
	ld	a,0 (iy)
	add	a, #0x05
	ld	e,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	d,a
	ld	a, 7 (ix)
	ld	(de),a
;type.c:67: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG3H_ADDR) = CX80_HiByte(len);
	ld	a,0 (iy)
	add	a, #0x06
	ld	e,a
	ld	a,1 (iy)
	adc	a, #0x00
	ld	d,a
	ld	a, 8 (ix)
	ld	(de),a
;type.c:69: *CX80_System_FNC_EntryPoint = CX80_FNC_GET_STRING_FROM_FILE;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x2A
;type.c:71: CX80_DO_SYSTEM_CALL;
	call #0x0007
;type.c:73: return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR));
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	inc	hl
	ld	l,(hl)
	pop	ix
	ret
_GetString_end::
;type.c:76: void CloseFile(char fID)
;	---------------------------------
; Function CloseFile
; ---------------------------------
_CloseFile_start::
_CloseFile:
;type.c:78: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = fID;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	iy,#2
	add	iy,sp
	ld	a,0 (iy)
	ld	(de),a
;type.c:80: *CX80_System_FNC_EntryPoint = CX80_FNC_CLOSE_FILE;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x1A
;type.c:82: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_CloseFile_end::
;type.c:85: void MoveCursor(char row, char column)
;	---------------------------------
; Function MoveCursor
; ---------------------------------
_MoveCursor_start::
_MoveCursor:
;type.c:87: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR) = row;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	ld	hl, #2+0
	add	hl, sp
	ld	a, (hl)
	ld	(de),a
;type.c:88: *(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1H_ADDR) = column;
	ld	de,(_CX80_System_FNC_EntryPoint)
	inc	de
	inc	de
	ld	iy,#3
	add	iy,sp
	ld	a,0 (iy)
	ld	(de),a
;type.c:90: *CX80_System_FNC_EntryPoint = CX80_FNC_MOVE_CURSOR;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x0B
;type.c:92: CX80_DO_SYSTEM_CALL;
	call #0x0007
	ret
_MoveCursor_end::
;type.c:95: unsigned char GetCh(void)
;	---------------------------------
; Function GetCh
; ---------------------------------
_GetCh_start::
_GetCh:
;type.c:97: *CX80_System_FNC_EntryPoint = CX80_FNC_WAIT_FOR_KEYPRESSED;
	ld	hl,(_CX80_System_FNC_EntryPoint)
	ld	(hl),#0x24
;type.c:99: CX80_DO_SYSTEM_CALL;
	call #0x0007
;type.c:101: return (*(CX80_System_FNC_EntryPoint + CX80_SYSTEM_REG1L_ADDR)); 
	ld	hl,(_CX80_System_FNC_EntryPoint)
	inc	hl
	ld	l,(hl)
	ret
_GetCh_end::
;type.c:104: int main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
	push	ix
	ld	ix,#0
	add	ix,sp
	dec	sp
;type.c:109: CX80_System_FNC_EntryPoint = (char *)0x0000;
	ld	iy,#_CX80_System_FNC_EntryPoint
	ld	0 (iy),#0x00
	ld	1 (iy),#0x00
;type.c:111: CX80PrintString("CX80 Type Command.\n\r");
	ld	hl,#__str_0
	push	hl
	call	_CX80PrintString
;type.c:112: CX80PrintString("Show text file on video.\n\r");
	ld	hl, #__str_1
	ex	(sp),hl
	call	_CX80PrintString
;type.c:113: CX80PrintString("Filename (empty to exit)\n\r");
	ld	hl, #__str_2
	ex	(sp),hl
	call	_CX80PrintString
;type.c:114: CX80PrintString(":");
	ld	hl, #__str_3
	ex	(sp),hl
	call	_CX80PrintString
	pop	af
;type.c:116: GetLine(ReadLine, 38);
	ld	de,#_ReadLine
	ld	a,#0x26
	push	af
	inc	sp
	push	de
	call	_GetLine
;type.c:118: CX80PrintString("\n\r");
	inc	sp
	ld	hl,#__str_4
	ex	(sp),hl
	call	_CX80PrintString
;type.c:120: if (strlen(ReadLine) > 0)
	ld	hl, #_ReadLine
	ex	(sp),hl
	call	_strlen
	pop	af
	ld	b,h
	ld	a,b
	or	a,l
	jp	Z,00120$
;type.c:123: cnt = 0;
	ld	c,#0x00
;type.c:125: FileID = OpenFile(ReadLine);
	ld	hl,#_ReadLine
	push	bc
	push	hl
	call	_OpenFile
	pop	af
	pop	bc
	ld	iy,#_FileID
	ld	0 (iy),l
;type.c:127: if (FileID < 3)
	ld	a,0 (iy)
	sub	a, #0x03
	jp	NC,00117$
;type.c:129: while (1)
00112$:
;type.c:131: ret = GetString(FileID, ReadLine, 38);
	ld	hl,#_ReadLine
	push	bc
	ld	de,#0x0026
	push	de
	push	hl
	ld	a,(_FileID)
	push	af
	inc	sp
	call	_GetString
	pop	af
	pop	af
	inc	sp
	pop	bc
	ld	-1 (ix),l
;type.c:133: if (cnt == MAX_LINE_ON_SCREEN)
	ld	a,c
	sub	a, #0x18
	jr	NZ,00106$
;type.c:135: MoveCursor(24, 0);
	ld	hl,#0x0018
	push	hl
	call	_MoveCursor
;type.c:136: CX80PrintString("More? (Y/N)");
	ld	hl, #__str_5
	ex	(sp),hl
	call	_CX80PrintString
	pop	af
;type.c:138: pCh = GetCh();
	call	_GetCh
;type.c:140: if ((pCh == 'y') || (pCh == 'Y'))
	ld	a,l
	cp	a,#0x79
	jr	Z,00101$
	sub	a, #0x59
	jr	NZ,00113$
00101$:
;type.c:142: cnt = 0;
	ld	c,#0x00
;type.c:144: MoveCursor(24, 0);
	push	bc
	ld	hl,#0x0018
	push	hl
	call	_MoveCursor
	pop	af
	pop	bc
;type.c:146: CX80PrintString("           ");
	ld	hl,#__str_6
	push	bc
	push	hl
	call	_CX80PrintString
	ld	hl, #0x0018
	ex	(sp),hl
	call	_MoveCursor
	pop	af
	pop	bc
;type.c:151: break;
00106$:
;type.c:154: CX80PrintString(ReadLine);
	ld	hl,#_ReadLine
	push	bc
	push	hl
	call	_CX80PrintString
	pop	af
	pop	bc
;type.c:156: cnt++;
	inc	c
;type.c:158: pntch = strchr(ReadLine, '\n');
	ld	hl,#_ReadLine
	push	bc
	ld	de,#0x000A
	push	de
	push	hl
	call	_strchr
	pop	af
	pop	af
	pop	bc
;type.c:160: if (pntch == NULL)
	ld	a,h
	or	a,l
	jr	NZ,00108$
;type.c:161: CX80PrintString("\n\r");
	ld	hl,#__str_4
	push	bc
	push	hl
	call	_CX80PrintString
	pop	af
	pop	bc
00108$:
;type.c:163: if (ret != 0)
	ld	a,-1 (ix)
	or	a, a
	jp	Z,00112$
;type.c:164: break;
00113$:
;type.c:167: CloseFile(FileID);
	ld	a,(_FileID)
	push	af
	inc	sp
	call	_CloseFile
	inc	sp
;type.c:169: if (ret == 0xAA)
	ld	a,-1 (ix)
	sub	a, #0xAA
	jr	NZ,00120$
;type.c:170: CX80PrintString("Error while read file!\n\r");
	ld	hl,#__str_7
	push	hl
	call	_CX80PrintString
	pop	af
	jr	00120$
00117$:
;type.c:173: CX80PrintString("Error while open file!\n\r");
	ld	hl,#__str_8
	push	hl
	call	_CX80PrintString
	pop	af
00120$:
;type.c:176: return 0;
	ld	hl,#0x0000
	ld	sp,ix
	pop	ix
	ret
_main_end::
__str_0:
	.ascii "CX80 Type Command."
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_1:
	.ascii "Show text file on video."
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_2:
	.ascii "Filename (empty to exit)"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_3:
	.ascii ":"
	.db 0x00
__str_4:
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_5:
	.ascii "More? (Y/N)"
	.db 0x00
__str_6:
	.ascii "           "
	.db 0x00
__str_7:
	.ascii "Error while read file!"
	.db 0x0A
	.db 0x0D
	.db 0x00
__str_8:
	.ascii "Error while open file!"
	.db 0x0A
	.db 0x0D
	.db 0x00
	.area _CODE
	.area _CABS (ABS)
