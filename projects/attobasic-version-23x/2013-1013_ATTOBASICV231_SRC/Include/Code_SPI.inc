.if SPI
;*************************************************************
;	SPM: set the SPI mode [0..2].
SPMcommand:
.ifdef PRR
	CLRB	PRR,PRSPI,temp		;enable SPI in PRR
.endif
	ldi		U,2					;default to Mode 2
	Gosub	pushU				;save it on the stack
	rcall	Get1stParm			;get the last parameter
	ldi		temp,$03			;only values of [0..3] allowed
	cp		temp,U
	brmi	SPMcommand9			;branch if allowable exceeded 
	lsl		U					;shift value 2 bits left
	lsl		U

	rcall	SPIinit				;init the SPI
	LOAD	temp,SPCR			;get SPCR
	andi	temp,0xF3			;zero the CPOL and CPHA bits
	or		temp,U				;OR new value
	STORE	SPCR,temp			;save SPCR
	ret

SPMcommand9:
	STOREI	ERRCode,$0D			;error code "D"
	Goto	error
;
;*************************************************************
;	SPO: set the data order (SPO 0/1) 0 = MSB, 1 = LSB.
SPOcommand:
	rcall	Get1stParm		;get the last parameter
	ldi		temp,$1				;only values of [0..1] allowed
	cp		temp,U
	brmi	SPOcommand9			;branch if allowable exceeded 
	SETB	SPCR,DORD,temp		;set LSB mode
	sbrc	U,0					;skip next if bit 0 set
	rjmp	SPOcommand1
	CLRB	SPCR,DORD,temp		;set LSB mode
SPOcommand1:
	ret

SPOcommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error
;
;*************************************************************
;	SPC: set the SPI clock rate.
SPCKcommand:
	rcall	Get1stParm		;get the last parameter
	ldi		temp,$7				;only values of [0..7] allowed
	cp		temp,U
	brmi	SPCKcommand9		;branch if allowable exceeded 

	mov		r0,U				;copy of U in r0
	andi	U,0x03				;strip off bit 2 (SPI2X)
	LOAD	temp,SPCR			;get SPCR
	andi	temp,0xF8			;clear clock bits
	or		temp,U				;OR new clock bits
	STORE	SPCR,temp			;save SPCR

	SETB	SPSR,SPI2X,temp		;set 2X clock mode			
	sbrc	r0,2				;skip next if bit 2 clear
	rjmp	SPCKcommand1
	CLRB	SPSR,SPI2X,temp		;set 1X clock mode
SPCKcommand1:
	ret

SPCKcommand9:
	STOREI	ERRCode,$0A			;error code "A"
	Goto	error

;*************************************************************
;	SPW: write a value to and read a value from the SPI port
SPWcommand:
	rcall	ChkSPIEn			;is SPI enabled?
	rcall	Get1stParm		;get the last parameter

	CLRB	SPI_PORT,SPI_SS		;drop SS line low
	nop							;delay
	nop
	STORE	SPDR,U				;send data
SPWcommand1:
	SKBS	SPSR,SPIF,romreg	;skip next if SPIF set
	rjmp	SPWcommand1			;loop till data shifted out
	LOAD	U,SPDR				;get return byte
	ret
;
;*************************************************************
;	SPR: read a value from the SPI port
SPRcommand:
	rcall	ChkSPIEn			;is SPI enabled?
	CLRB	SPI_PORT,SPI_SS		;drop SS line low
	nop							;delay
	nop
	STORE	SPDR,U				;send dummy data
SPRcommand1:
	SKBS	SPSR,SPIF,romreg	;skip next if SPIF set
	rjmp	SPRcommand1			;loop till data shifted out
	LOAD	U,SPDR				;get return byte
	Gosub	pushU
	ret
;
;*************************************************************
;	SPS: set SPI_SS pin to level to U where U = [0..1].  Defaults
;		 to '1' when Gosubed without a value.
SPScommand:
	rcall	ChkSPIEn			;is SPI enabled?
	ldi		U,1					;Put default level on stack			
	GoSub	pushU				;save on the dstack
	rcall	Get1stParm			;get the last parameter
	SETB	SPI_PORT,SPI_SS		;drop SS line low
	sbrc	U,0					;skip next if bit 0 set
	rjmp	SPScommand1
	CLRB	SPI_PORT,SPI_SS		;drop SS line low
SPScommand1:
	ret
;
.endif
;
.if (SPI || DATA)
;*************************************************************
; ChkSPIEn: checks to see if the SPI is enabled.  Error if not
;*************************************************************
ChkSPIEn:
	SKBC	SPCR,SPE,temp		;skip next if SPI enabled
	ret							;return to caller
	STOREI	ERRCode,$1A			;error code "0x1A"
	Goto	error
	ret
;
;***********************************************************************
; SPIinit: SPI port initialization routine.  Defaults to SPI Master in
;	mode 0 with fck/16.
; Destroys: temp
;***********************************************************************
SPIinit:
	;Set MOSI, SCK and SS output, all others input
	LOAD	temp,SPI_PORT-1
	ori		temp,(	(1 << SPI_MOSI) | \
					(1 << SPI_SCK) | \
					(1 << SPI_SS) )
	STORE	SPI_PORT-1,temp

	;make SS output high
	SETB	SPI_PORT,SPI_SS		;insure SS is high

	;Enable SPI, Master, set clock rate fck/16, mode 0
	ldi		temp,(	(1 << SPE) | \
					(1 << MSTR) | \
					(0 << CPOL) | \
					(0 << CPHA) | \
					(1 << SPR0) )
	STORE	SPCR,temp
	CLRB	SPSR,SPI2X,temp			;insure 1X clock

	ret
;
;***************************************************************
; SPI_write: Local SPI write data routine.  Writes value in "U"
;	to SPDR, waits for xmit to finish before returning.
; Destroys: temp
;***************************************************************
SPI_write:
	CLRB	SPI_PORT,SPI_SS		;drop SS line low
	nop							;delay
	nop
	STORE	SPDR,U				;send data
SPI_write1:
	SKBS	SPSR,SPIF,temp		;skip next if SPIF set
	rjmp	SPI_write1			;loop till data shifted out
	LOAD	temp,SPDR			;read SPDR to clear SPIF flag
	ret							;return to caller
;
;*************************************************************
; SPI_read: read a value from the SPI port into "U"
; Destroys: temp
;*************************************************************
SPI_read:
	CLRB	SPI_PORT,SPI_SS		;drop SS line low
	nop							;delay
	nop
	STORE	SPDR,U				;send dummy data
SPI_read1:
	SKBS	SPSR,SPIF,temp		;skip next if SPIF set
	rjmp	SPI_read1			;loop till data shifted out
	LOAD	U,SPDR				;get return byte
	ret							;return to caller
;
.endif
