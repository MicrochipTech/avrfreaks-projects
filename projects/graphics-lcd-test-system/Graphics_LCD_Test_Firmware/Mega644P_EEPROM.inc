;********** ATmega644P EEPROM Subroutines **************************************
;Mega644P_EEPROM.inc
;9-1-2010 CFS

;for ATmega644P with 2K  EEPROM
;all EEPROM writes include atomic erase operation
;same routines as ATMega324P version


;*******************************************************************************
;* ROUTINE
;*	EE_Checksum
;* DESCRIPTION
;*	Calculates checksum on EEPROM region lastEEPROM:firstEEPROM.
;* USAGE
;*	temp <- checksum(lastEEPROM:firstEEPROM)
;*	firstEEPROM and lastEEPROM are equates in project def file
;* STATISTICS
;*	Cycles:		varies with length of EEPROM region
;*	Words:		11 + ret
;*	Register usage: r16 to r18, temp, and temp1
;*
;*******************************************************************************


					;firstEEPROM is starting address
					;lastEEPROM is last address checked
					;returns checksum in temp register
EE_Checksum:	clr	temp		;clear temp register
		ldi	r17,low(lastEEPROM)	;load last address
		ldi	r18,high(lastEEPROM)
EE_Chkslp:	rcall	EERead		;read data
		add	temp,r16	;add data to checksum
		subi	r17,1		;decrement address
		sbci	r18,0
		cpi	r17,low(firstEEPROM)	;check against starting address
		ldi	temp1,high(firstEEPROM)
		cpc	r18,temp1
		brcc	EE_Chkslp	;loop if greater
		ret


;*******************************************************************************
;* ROUTINE
;*	EEWrite
;* DESCRIPTION
;*	Write byte to EEPROM.
;* USAGE
;*	EEPROM(r18:r17) <- r16
;* STATISTICS
;*	Cycles:		Depends on EEPROM
;*	Words:		14 + ret
;*	Register usage: r16 to r18
;* NOTE
;*	EEWrite waits until the EEPROM is ready to be programmed, then programs
;*	the EEPROM address given by r18:r17 with the data byte in r16.
;*	Interrupts are disabled during the write routine and the state of the
;*	global interrupt enable bit is restored when the routine is done.
;*	Address in r18:r17 is not changed
;*
;*******************************************************************************



EEWrite:	wdr			;clear watchdog timer
		sbic	EECR,EEPE	;if EEPE not clear
		rjmp	EEWrite		;wait more
		out	EEARL,r17	;output address
		out	EEARH,r18
		out	EEDR,r16	;output data

					;save interrupt enable bit to T
		clt
		brid	EEWrt_1
		set

EEWrt_1:	cli			;disable interrupts
		sbi	EECR,EEMPE	;set EEPROM master write bit
		sbi	EECR,EEPE	;set EEPROM write strobe

		brtc	EEWrt_Ret	;enable interrupts if T set
		sei

EEWrt_Ret:	ret



;*******************************************************************************
;* ROUTINE
;*	EEWrite1
;* DESCRIPTION
;*	Write byte to EEPROM.
;* USAGE
;*	EEPROM(r18:r17) <- r16
;* STATISTICS
;*	Cycles:		Depends on EEPROM
;*	Words:		14 + ret
;*	Register usage: r16 to r18
;* NOTE
;*	EEWrite1 waits until the EEPROM is ready to be programmed, then programs
;*	the EEPROM address given by r18:r17 with the data byte in r16.
;*	Interrupts are disabled during the write routine and always re-enabled
;*	when the routine is done.
;*	Address in r18:r17 is not changed
;*
;*******************************************************************************



EEWrite1:	wdr			;clear watchdog timer
		sbic	EECR,EEPE	;if EEPE not clear
		rjmp	EEWrite1		;wait more
		out	EEARL,r17	;output address
		out	EEARH,r18
		out	EEDR,r16	;output data

		cli			;disable interrupts
		
		sbi	EECR,EEMPE	;set EEPROM master write bit
		sbi	EECR,EEPE	;set EEPROM write strobe

		sei

		ret




;*******************************************************************************
;* ROUTINE
;*	EERead
;* DESCRIPTION
;*	Read byte from EEPROM.
;* USAGE
;*	r16 <- EEPROM(r18:r17)
;* STATISTICS
;*	Cycles:		Depends on EEPROM
;*	Words:		10 + ret
;*	Register usage: r16 to r18 and temp
;* NOTE
;*	EEWrite waits until the EEPROM is ready (to be programmed), then reads
;*	the EEPROM address given by r18:r17 and places the data byte in r16.
;*	Returns EEPROM address register to $0000 when done to help prevent
;*	EEPROM corruption. Address in r18:r17 is not changed.
;*
;*******************************************************************************


EERead:		wdr			;clear watchdog timer
		sbic	EECR,EEPE	;if EEPE not clear
		rjmp	EERead		;wait more
		out	EEARL,r17	;output address
		out	EEARH,r18
		sbi	EECR,EERE	;set EEPROM read strobe
		in	r16,EEDR	;get data

		clr	temp		;set EEPROM address register to $0000
		out	EEARL,temp	;(helps prevent corruption)
		out	EEARH,temp
		ret
