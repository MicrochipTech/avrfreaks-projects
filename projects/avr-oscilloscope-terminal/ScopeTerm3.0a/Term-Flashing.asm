; Term-Flashing.asm	-- Included source for Dutchtronix Oscilloscope Terminal
;
;  Copyright © 2008 Jan P.M. de Rie
;
;  All Rights Reserved
;
;  This file is part of the Dutchtronix Oscilloscope Clock Distribution.
;  Use, modification, and re-distribution is permitted subject to the
;  terms in the file named "LICENSE.TXT", which contains the full text
;  of the legal notices and should always accompany this Distribution.
;
;  This software is provided "AS IS" with NO WARRANTY OF ANY KIND.
;
;  This notice (including the copyright and warranty disclaimer)
;  must be included in all copies or derivations of this software.
;
; Terminal program for Dutchtronix AVR Oscilloscope Clock hardware
;
; Flash Control Table: allow displayed fields to flash/blink
;
; Each entry describes a ScanTbl entry that needs to flash:
;
;	ScanTbl ptr, Vector Table ptr, On/Off status, FlashCnt,
;		Original VectorTbl ptr,  AlternateOriginal Vector Table ptr (review)
;
; Add an item to flash
; Arguments:
;	X: Scantbl Ptr
;	Z: Vector Tbl to use (Scantbl may be NULL)
;	r16: FlashCnt (0xff means forever)
;	r0, r1 Alternate Vector Tbl.
; Return:
;	X: FlashTblPtr
;
; Registers used: r17,r18,r19, Z
;
AddFlashItem:
	pushw	X				;save arg 1
	ldiw	X,FlashTbl		;beginning of table
	ldi		r17,MaxFlashTblEntries
AddFlashL100:
	ld		r18,X+			;get ScanTbl ptr
	ld		r19,X+
	or		r18,r19			;zero means available
	breq	AddFlashL300	;yes
;
; entry is in use. skip to next entry
;
	adiw	X,FlashTblEntrySize-2	;autoincremented twice
	dec		r17				;table cnt
	brne	AddFlashL100	;more to scan
;
; finished scanning the table, no free entry found
; should not happen but we'll return the last (unused entry) to continue
;
#if DEBUG
	rjmp	PC				;loop
#else
	popw	Z				;cleanup the stack
	rjmp	AddFlashL900
#endif
AddFlashL300:
	st		X+,ZL			;store VectorTbl Ptr
	st		X+,ZH
	st		X+,_1			;mark as on
	st		X+,r16			;store FlashCnt
	popw	Z				;retrieve ScanTbl ptr
	ld		r16,Z+			;save original VectorTbl ptr
	st		X+,r16
	ld		r16,Z+
	st		X+,r16
	st		X+,r0			;store Alternate VectorTbl ptr
	st		X+,r1			;X points beyond end of entry
	sbiw	X,FlashTblEntrySize	;back to start of entry
	sbiw	Z,2				;back to original ScanTbl 
	st		X+,ZL			;store ScanTbl Ptr
	st		X,ZH
	sbiw	X,1				;back to start of entry
AddFlashL900:
	ret
;
; Remove an item from the FlashTbl
; Arguments:
;	X: FlashTblPtr
;
; Registers used: r16, r17
;
RemoveFlashItem:
	mov		r0,XL					;ignore if not active
	or		r0,XH
	breq	RemoveFlashL100
;
; restore the original vector tbl in the scan tbl
;
	pushw	Z
	ld		ZL,X+			;get ScanTbl ptr
	ld		ZH,X+
	adiw	X,4				;move to Original VectorTbl ptr
	ld		r16,X+			;get Original VectorTbl ptr
	ld		r17,X+			;X points to alternate entry
	st		Z+,r16			;restore ScanTbl
	st		Z+,r17
RmFlashL100:
	sbiw	X,FlashTblEntrySize-2 ;back to start of entry
	rcall	EmptyFlashItem	;Empty FlashTbl Entries
	popw	Z
RemoveFlashL100:
	ret
;
; Empty an item in the FlashTbl
; Arguments:
;	X: FlashTblPtr
;
; Registers used: r16
;
EmptyFlashItem: 
	ldi		r16,FlashTblEntrySize
EFIL100:
	st		X+,_0
	dec		r16
	brne	EFIL100
	ret
;
; Initialize the FlashTbl
; Arguments:
;
; Registers used: r16, r17, X
;
InitFlashTbl:
	ldi		r17,MaxFlashTblEntries+1	;also init the last dummy entry
	ldiw	X,FlashTbl		;beginning of table
InitFlashTblL100:
	rcall	EmptyFlashItem
	dec		r17
	brne	InitFlashTblL100
	ret
;
; Process Flash Table
; Arguments:
;	Z: ptr to function to call for every active entry
;
; Registers used: r16, r17, r18
;
ProcessFlashTbl:
	ldiw	X,FlashTbl		;beginning of table
	ldi		r18,MaxFlashTblEntries
ProcessFlashL100:
	pushw	X				;save
	ld		r16,X+			;get ScanTbl ptr
	ld		r17,X+
	or		r16,r17			;zero means not-active
	breq	ProcessFlashL300
;
; entry is active
;
	sbiw	X,2				;autoincremented twice
	push	r18				;save
	rcall	ProcessFlashItem
	pop		r18				;restore
ProcessFlashL300:
	popw	X				;retrieve	
	adiw	X,FlashTblEntrySize	;next entry
	dec		r18				;table cnt
	brne	ProcessFlashL100 ;more to scan
	ret
;
; sample ProcessFlashTblItem function
; Arguments:
;	X: FlashTblPtr
;
; Registers used: r16,r17,r18,r19
;
ProcessFlashItem:
	ld		r16,X+			;get ScanTbl ptr
	ld		r17,X+
#if DEBUG
	mov		r18,r16			;check for valid ptr
	or		r18,r17
	brne	PFIL200
	rjmp	PC
PFIL200:
#endif
	adiw	X,2				;skip VectorTbl Ptr
	ld		r18,X+			;get current status
	ld		r19,X			;get FlashCnt
	cpi		r19,0xff		;means no count
	breq	PFIL250
	dec		r19				;update
	st		X,r19			
	breq	PFIL800			;done, exit
PFIL250:
	mov		r19,r18			;update status
	eor		r19,_1			;Toggle
	st		-X,r19
	tst		r18				;current flash status
	breq	PFIL300			;off, go turn on
;
; field was on, Turn off
;
	adiw	X,4				;advance to Alternate VectorTbl Ptr
	rjmp	PFIL400
PFIL300:
	sbiw	X,2				;back to VectorTbl Ptr
PFIL400:
	ld		r18,X+			;get VectorTbl
	ld		r19,X+
	movw	XL,r16			;get ScanTbl Ptr
	st		X+,r18			;restore VectorTbl
	st		X+,r19
	rjmp	PFIL900
;
; Countdown reached 0. Remove item
;
PFIL800:
	sbiw	X,5				;5 increments
	pushw	Z
	rcall	RemoveFlashItem
	popw	Z
PFIL900:
	ret
