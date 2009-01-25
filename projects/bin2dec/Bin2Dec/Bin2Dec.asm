

;	Purpose:		; This routine takes converts a 1 to 4 byte binary number and converts it
					; to a formated decimal number.
					; Bin2Dec <NumLSB>, <BytesInNum>, <OutBufPtr>, <MaskPtr>
					; ex: Bin2Dec r16, 2, OutBufLen, MaskLen
					; where:
					; 		r17:r16 is number to format (r16 = LSB)
					;		2 is the number of bytes to format
					; OutBufLen and MaskLen are declared elsewhere as (for example):
					;
					;.DSEG
					;OutBuf:		.BYTE 12
					;OutBufLen:		.BYTE 1
					;.CSEG
					;Mask:			.db "#,##0.00vpp"
					;MaskLen:		.db 11
					;
					;sts		OutBufLen, 12
					;
;	Notes:			; As each digit is output, from right to left, it is placed in the buffer according
					; to the mask.  if the character in the mask is '#' then the digit is placed there. The
					; mask character '0' is used to force leading zeros.  Any other character is simply
					; inserted into the buffer

					;core binary-to-decimal algorithm from: http://elm-chan.org/docs/avrlib/mk_decu.txt


;	Examples:		  with an input value of 1234:
					;
					;	Mask			Output
					; ============	============
					;    "###,###"		   1,234
					;    "###,##0"		   1,234
					;	 "000,000"		 001,234
					;	   "#.###"		   1.234
					;	  ".#####"		  . 1234
					;	  ".00000"		  .01234
					;   "#0.000v"		  1.234v
					; "Out: ##.##v"  Out: 12.34v
					;  "# # xx # #"   1 2 xx 3 4

.cseg

.def	SFTemp	=	r16
.def	OutLen	=	r17
.def	OutPos	=	r18
.def	MskLen	=	r19
.def	MskPos	=	r20
.def	CurMsk	=	r21
.def	var0	=	r10
.def	var1	=	r11
.def	var2	=	r12
.def	var3	=	r13
.def	CurChar	=	r22
.def	BitCnt	=	r23

.MACRO Bin2Dec		;Bin2Dec <NumLSB>, <BytesInNum>, <OutBufPtr>, <MaskPtr>
	PushX
	PushY
	PushZ
	push	SFTemp
	ldi		XH, high(@0+1)	;Input LSB
	ldi		XL, low(@0+1)
	ldi		SFTemp, @1
	ldi		YH, high(@2)	;OutLenPtr
	ldi		YL, low(@2)
	ldi		ZH, high(@3*2)	;MskLenPtr
	ldi		ZL, low(@3*2)
	rcall	_B2DStart
	pop		SFTemp
	PopZ
	PopY
	PopX
.ENDM



_B2DStart:
	push	OutLen
	push	OutPos
	push	MskLen
	push	MskPos
	push	CurMsk
	push	var0
	push	var1
	push	var2
	push	var3
	push	CurChar
	push	BitCnt
	ld		OutLen, Y		;Load OutLen
	ldi		OutPos, 1
	ldi		MskPos, 1
	
	push	SFTemp	
	clr		SFTemp
	cpi		ZL, 0
	cpc		ZH, SFTemp
	breq	_B2DNoMask
	lpm		MskLen, Z		;Load MskLen
	sbiw	Z, 1			;Decrement Z
	sbrc	MskLen, 0		; if MskLen is odd, subtrack padding byte
	sbiw	Z, 1			;Decrement Z
	rjmp	_B2DMskCkDn
_B2DNoMask:
	clr		MskLen
_B2DMskCkDn:
	pop		SFTemp	
	clr		var1			;set up input
	clr		var2
	clr		var3
	ld		var0, -X		;load b0
	cpi		SFTemp, 1
	breq	_B2DL0
	ld		var1, -X		;load b1
	cpi		SFTemp, 2
	breq	_B2DL0
	ld		var2, -X		;load b2
	cpi		SFTemp, 3
	breq	_B2DL0
	ld		var3, -X		;load b0
_B2DL0:
	clr     CurChar         ;var1 /= 10;
	ldi     BitCnt, 32      ;
_B2DL1:
	lsl     var0            ;shift
	rol     var1
	rol		var2
	rol		var3
	rol     CurChar
	cpi     CurChar, 10
	brcs    _B2DL2
	subi    CurChar, 10
	inc     var0
_B2DL2:
	dec     BitCnt
	brne    _B2DL1
	subi    CurChar, -'0'    ;Convert remainder to decimal
_B2DL3:
	cpi		MskLen, 0
	brne	_SF_GetMsk
	ldi		CurMsk, '#'
	rjmp	_SF_NoMsk
_SF_GetMsk:
	cp		MskLen, MskPos
	brlo	_SF_MaskDone
	rjmp	_SF_MaskGood
_SF_MaskDone:
	
	ldi		CurMsk, ' '
	rjmp	_SF_NoMsk
_SF_MaskGood:
	lpm		CurMsk, Z				; load it
	sbiw	Z, 1					; Z points to current mask char
	inc		MskPos					; adjust mask position
_SF_NoMsk:
	cp		OutLen, OutPos
	brlo	_B2DExit
	cpi		CurMsk, '#'				; is it a '#'
	breq	_SF_WasNum				; yep, jump to routine
	cpi		CurMsk, '0'				; is it a '0'
	breq	_SF_Was0				; yep, jump to routine
	cpi		CurMsk, ','				; is it a ','
	breq	_SF_WasComma			; jump to routine
	st		-Y, CurMsk				; not any of the above, store it in output buffer
	inc		OutPos					; adjust position
	rjmp	_B2DL3					; get next mask character
_SF_WasComma:
	cpi		CurChar, ' '				; if input byte is ' ' then we don't want a comma
	brne	_SF_AddComma			; i.e. no leading comma
	st		-Y, CurChar				; otherwise, store input byte to buffer
	inc		OutPos
	rjmp	_SF_Done				; process another
_SF_AddComma:
	ldi		SFTemp, ','				; add comma to output buffer
	st		-Y, SFTemp
	inc		OutPos
	sbiw	Z, 1					; Z points to current mask char
	inc		MskPos					; adjust mask position
	cp		OutLen, OutPos			; did it cause us to reach the end of the buffer
	brlo	_B2DExit				; yep, we're done
	st		-Y, CurChar
	inc		OutPos
	rjmp	_SF_Done				; process another
_SF_WasNum:
	st		-Y, CurChar				; store it
	inc		OutPos
	rjmp	_SF_Done				; and process another
_SF_Was0:
	cpi		CurChar, ' '				; is current char a space?
	breq	_SF_Add0				; yep, add leading zero
	st		-Y, CurChar
	inc		OutPos
	rjmp	_SF_Done				; process another
_SF_Add0:
	ldi		SFTemp, '0'				; add leading zero
	st		-Y, SFTemp
	inc		OutPos
_SF_Done:
	cp      var0, BitCnt       		;  see if done
	cpc     var1, BitCnt       		; 
	cpc		var2, BitCnt
	cpc		var3, BitCnt
	breq	_B2DDone
	rjmp	_B2DL0
_B2DDone:
	ldi		CurChar, ' '
	cp		OutLen, OutPos
	brsh	_B2DL3
_B2DExit:	
	pop		BitCnt				
	pop		CurChar
	pop		var3
	pop		var2
	pop		var1
	pop		var0
	pop		CurMsk
	pop		MskPos
	pop		MskLen
	pop		OutPos
	pop		OutLen
	ret
