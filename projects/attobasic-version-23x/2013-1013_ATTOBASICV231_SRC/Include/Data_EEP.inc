;******************************************************************************
; EEPROM Storage
;******************************************************************************
	.eseg
;******************************************************************************
.if EFS	;support for file system
;==============================================================
;STRUCTURE: The entire EEP is divided into allocation 
;	units of "EFSBLKSZ" bytes.
;
;FILE INDEX:
; The file index records (BPHNDL * HANDLES_MAX) contain
;	the address of the file's 1st allocation unit
;	The number of available file index records is determined 
;	by the "HANDLES_MAX" equate.
;
;ALLOCATION UNIT: Each allocation unit (32 bytes) consists of:
; Byte:	
;	 0:	Record Type
;	 1:	Start of data payload
;	...
;	30:	End of data payload
;	31:	Next Block
;
; Where "Record Type" is:
;		0xFF = Unused Record (erased EEP byte)
;		0x01 = Data Record
;
;==============================================================
.endif
;
	.org	E2END	;top of EEPROM
 E2PPROGTOP:		;always defined
;
.if EFS	;support for file system
	.org	0	;bottom of EEPROM
  EFSIDX0:	.db		EFSFILE0/EFSBLKSZ		;file 0
  EFSIDX1:	.db		EFSFILE1/EFSBLKSZ		;file 1
;  EFSIDX2:	.db		EFSFILE2/EFSBLKSZ		;file 2
  EFSIDX2:	.db		0						;file 2
  EFSIDX3:	.db		EFSFILE3/EFSBLKSZ		;file 3
  EFSIDX4:	.db		0						;file 4
  EFSIDX5:	.db		0						;file 5
  EFSIDX6:	.db		0						;file 6
  EFSIDX7:	.db		0						;file 7
  EFSIDX8:	.db		0						;file 8
  EFSIDX9:	.db		0						;file 9
  EFSIDX10:	.db		0						;file 10
  EFSIDX11:	.db		0						;file 11
  EFSIDX12:	.db		0						;file 12
  EFSIDX13:	.db		0						;file 13
  EFSIDX14:	.db		0						;file 14
  EFSIDX15:	.db		0						;file 15
;
			.dd		0		;reserve 16 more bytes of EEP
			.dd		0
			.dd		0
			.dd		0
;	.org	(E2END-(BPHNDL*HANDLES_MAX))	;preserved 4 bytes per handle
.endif

.if EFS
.equ	EFS_RECORD = 1		;"1" to generate dummy EEPROM record for testing.
;
  .if EFS_RECORD
EFSFILE0:
			.db		1
			.db		"10 REM This is program 0",CR
			.db		"20 PR"
			.db		(EFSFILE0a/EFSBLKSZ)
EFSFILE0a:
			.db		1
			.db		"I 10",CR
			.db		"30 PRI 100",CR
			.db		"40 PRI 200",CR
			.db		"50 "
			.db		(EFSFILE0b/EFSBLKSZ)
EFSFILE0b:
			.db		1
			.db		"END",CR
			.db		0,0,0,0,0,0,0,0
			.db		0,0,0,0,0,0,0,0
			.db		0,0,0,0,0,0,0,0
			.db		0,0
			.db		0
;-------
EFSFILE1:
			.db		1
			.db		"10 REM This is program 1",CR
			.db		"20 FO"
			.db		(EFSFILE1a/EFSBLKSZ)
EFSFILE1a:
			.db		1
			.db		"R N=0 10",CR
			.db		"30 PRX N",CR
			.db		"40 NEX",CR
			.db		"50 EN"
			.db		(EFSFILE1b/EFSBLKSZ)
EFSFILE1b:
			.db		1
			.db		"D",CR
			.db		0,0,0,0,0,0,0,0
			.db		0,0,0,0,0,0,0,0
			.db		0,0,0,0,0,0,0,0
			.db		0,0,0,0,0
EFSFILE2:
			.db		0xFF,0x00
			.dw		0
			.dd		0
			.dd		0
			.dd		0
			.dd		0
			.dd		0
			.dd		0
			.dd		0
EFSFILE3:
			.db		1
			.db		"10 REM This is program 3",CR
			.db		"20 RT"
			.db		(EFSFILE3a/EFSBLKSZ)
EFSFILE3a:
			.db		1
			.db		"P ; SLP 5 ; RTP",CR
			.db		"30 SLP 6 ; RTP"
			.db		(EFSFILE3b/EFSBLKSZ)
EFSFILE3b:
			.db		1
			.db		CR
			.db		"30 FOR I=0 100",CR
			.db		"40 PRI ",'"',"The va"
			.db		(EFSFILE3c/EFSBLKSZ)
EFSFILE3c:
			.db		1
			.db		"lue of I is ",'"',' ',';'," PRI I",CR
			.db		"50 NEX",CR
			.db		"6"
			.db		(EFSFILE3d/EFSBLKSZ)
EFSFILE3d:
			.db		1
			.db		"0 PRI ",'"',"Hello",'"',CR
			.db		"70 RTP",CR
			.db		"80 END",CR
			.db		0,0,0



  .endif
.endif
