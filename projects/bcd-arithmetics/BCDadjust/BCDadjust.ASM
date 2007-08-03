
;*****  ValYor,01 *****

.def	RX	= r16
.def	RY	= r17


;*****  DECIMAL - ADJUST RESULT FOR SUBTRACTION * RX = RX-RY *****

DASUB:	in   RY,SREG	;store carry (bit0) in RY
	brhc PC+2		;skip if half carry is cleared
	subi RX,$06		;LSD = LSD - 6
	sbrc RY,0		;skip if previous carry is cleared
	subi RX,$60		;MSD = MSD - 6
	ror  RY		;restore carry in SREG
	ret



;*****  DECIMAL - ADJUST RESULT FOR ADDITION * RX = RX+RY *****

DAADD:	in   RY,SREG	;store carry (bit0) & half carry (bit5) in RY
	cpi  RX,$9A		;check if LSD > 9 and MSD + half carry > 9
	brcs PC+2		;skip if carry is set (MSD + half carry <= 9 )
	inc  RY		;set carry (bit0) in RY (previous carry is 0)
	sbrs RY,5		;skip if previous half carry is set
	brhs PC+2		;skip if half carry is set (LSD <= 9)
	subi RX,-$06	;LSD = LSD + 6
	sbrc RY,0		;skip if bit carry in RY is cleared
	subi RX,-$60	;MSD = MSD + 6
	ror  RY		;store new carry in SREG
	ret



;*****  Unknown author, Tested  ******

;*****  CONVERT 1 BYTE PACKED BCD TO HEX(BIN) *****

RXHEX:	sbrc RX,4
	subi RX,6	;(-$10+10)*2^0
	sbrc RX,5
	subi RX,12	;(-$10+10)*2^1
	sbrc RX,6
	subi RX,24	;(-$10+10)*2^2
	sbrc RX,7
	subi RX,48	;(-$10+10)*2^3
	ret

