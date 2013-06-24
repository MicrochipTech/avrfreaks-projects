
;==============================================================================
;
; ***************  Y-reg reserved for use in ext0 int routine  ****************
;
;==============================================================================

; the initialise routine only sets up for  -ve edge trigger.
init_ext0:
	ldi	temp1,0b01000000	; bit to clear any pending ext0 int
	out	gifr, temp1		; 

	in	temp1,mcucr
	andi	temp1,0b11111100
	ori	temp1,0b00000010	; falling edge on int0 & 1
	out	mcucr,temp1
	ret




; The interrupt oroutine works by jumping to the section dictated to by the value
; of a variable called 'state'.
; The receive software is broken into more states than the transmit. Receive data
; is checked byte by byte (almost) as it arrives and the local checksum assembled so
; by the time the received checksum arrives, it can be checked immidiately
; Transmit is simpler in the interrupt sense. The complete message is built and
; the checksum calculated as a foreground task, the interrupt routine (once triggered
; for transmit) simply feeds the data into the transmitter register.

; external int-0 routine
EXT_INT0:
	sbrc	bitflags,7	; test if just woken from power-down mode
	rjmp	wakeup

	push	temp1		; save reg and status
	load	temp1,SREG
	push	temp1
	push	temp2
	push	temp3
	push	accL
	push	accH

; first, check if TX'ing and go straight there if appropriate
	jeq	state,state10,TX10


; so must be in RX mode. Read the RFM12B
RXstuff:
	rcall	RFMread		; combined status & fifo read (if data is available)
	brcs	got		; C=got data in temp1
	rjmp	I0exit		; NC=no data

; data byte in temp1
got:	st	y+,temp1	; save in ram
	cpi	state,state5	; see if receiving CRC bytes so dont add into CRC
	brne	gotC
	rjmp	rxD		; skip fwd so dont add RX'ed CRC bytes into local CRC

gotC:	rcall	addtoCRC

	jeq	state,4,rxC	; receiving data
	jeq	state,3,rxB	; receiving data
	jeq	state,2,rxA	; initiated RX. On 1st RX byte, start timeout timer.
	jeq	state,1,er	; error state. stop int and exit
	jeq	state,0,idle	; nothing in progress. Stop int and exit
	error	1

; falls thru to exit
I0exit:
	pop	accH
	pop	accL
	pop	temp3
	pop	temp2
	pop	temp1
	store	SREG,temp1
	pop	temp1
	reti

er:
idle:
finished:
	ext0_OFF		; turn off int
	RFcommand RF_IDLE_MODE	; xtal on, (RX, TX, low-bat detect, clk out) all off
	rjmp	I0exit

;==============================================================================
;The 1st byte to activate the RX interrupt routine is the HEADER byte. (RFM12B.asm
;for packet structure) .The system is using TWO sync-bytes to trigger the
;fifo fill. The 1st sync byte is fixed in the silicon of the RFM12B.
;The 2nd sync-byte is being uses as the GROUP byte. It can be changed in software.
;Reception of the GROUP byte is implied since the only way the FIFO fill would
;be triggered is by receiving both sync-bytes in sequence.
;The GROUP byte is include in the CRC. The begin_RX routine adds it to the CRC
;So, 1st byte is the HEADER, 2nd is payload (data) length, 3rd is payload
;
;==============================================================================
; State2
; 1st received byte of a block gets us here. HEADER byte.
rxA:
	ldi	state,state3	; change state to show rx in progress
	rjmp	I0exit		; exit

;******************************************************************************
; state3
; now looking for data (payload) count
rxB:
; this should be the actual payload count. It is from 0 to maxdata
	mov	datacount,temp1	; save here also
	jeq	temp1,0,Zdata	; if zero length, skip to state5 (CRC)
	cpi	temp1,maxdata+1
	brsh	YY		; bad length
; count good so continue
	ldi	state,state4	; for data receive
	rjmp	I0exit

YY:	error	1		; bad length byte
XY:	ldi	state,state1	; set error state
	rjmp	finished	; stop int and exit



;******************************************************************************
; state4
; the actual payload gets received here (if any). state4 Skipped if no payload
rxC:	dec	datacount
	brne	I0exit
; payload finished, change state so next time get CRC bytes
Zdata:	ldi	state,state5	; data completed
	ldi	temp1,2		; for 2 CRC bytes reception
	mov	datacount,temp1
	rjmp	I0exit

;******************************************************************************
; state5
; this should be the CRC bytes
rxD:	dec	datacount
	brne	I0exit		; on 1st CRC byte, just exit

; after 2nd byte, falls through to the CRC testing
; now both CRC bytes received. High byte is still in temp1
	lds	temp3,crc_high
	cp	temp1,temp3
	brne	bad
	ld	temp1,-y	; Y is now pointing AFTER the CRC so -1 (now at MSB)
	ld	temp1,-y	; back again to point at LSB
	lds	temp3,crc_low
	cp	temp1,temp3
	brne	bad
	ldi	state,state6	; complete packet tested and good
	error	0		; no errors
	rjmp	finished	; turn OFF int and exit

bad:	error	2		; bad checksum
	ldi	state,state1	; error
	rjmp	finished	; exits with error




;******************************************************************************
;******************************************************************************
; state 10
; TX interrupt section
; just straight TX from ram. The complete message (with checksums) has been constructed
; in ram before the transmission process is started
TX10:	ld	temp1,y+		; get data to TX
	rcall	spi_send		; SS is stil low (see intTXdata routine)

	lds	temp1,TX_txcount	; adjust TX downcount
	dec	temp1
	sts	TX_txcount,temp1

	breq	TXdun			; if all sent, jump
	rjmp	I0exit			; else exit till next byte to TX


; the last byte is a dummy one, just to give enough time for the bytes in
; the TX buffers to be sent before turning-off the transmitter
TXdun:
	ext0_OFF		; switch off int
; RFM12B into idle mode
	RFcommand RF_IDLE_MODE	; RX,TX OFF

	ldi	state,state11	; show done
	sbi	SSpin		; finish the write to the TX buffers
	rjmp	I0exit		; and exit


; here on wake-up from power-down.
wakeup:
	push	temp1
	in	temp1,sreg
	push	temp1

	in	temp1,gimsk
	andi	temp1,0b10111111	; turn off ext0 int
	out	gimsk,temp1

	cb	bitflags,7		; clear indicator bit

	in	temp1,mcucr
	andi	temp1,0b11011100	; clear sleep bit and int types
	ori	temp1,0b00000010	; set falling edge on int0 & 1
	out	mcucr,temp1		; sleep off, falling edge for ext0

	ldi	temp1,0b01000000	; bit to clear any pending ext0 int
	out	gifr, temp1		; clear any pending INT0 interrupt flag

	pop	temp1
	out	sreg,temp1
	pop	temp1

	reti


	

