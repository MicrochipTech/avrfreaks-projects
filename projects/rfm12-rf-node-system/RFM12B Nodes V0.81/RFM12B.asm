; antenna lengths
;433 1/4 wave = 164.7mm
;433 1/2 wave = 329.4mm
;433 full wave = 692.7mm

;868 1/4 wave = 82.2mm
;868 1/2 wave = 164.3mm
;868 full wave = 345.5mm

;915 1/4 wave = 77.9mm
;915 1/2 wave = 155.9mm
;915 full wave = 327.8mm

;=====================================================================
;STATUS BITS. HIGH BYTE READ OUT FIRST (copied from the data sheet)
;
;bit15-	RGIT (TX register is ready to receive the next byte) or..
;	FFIT (RX mode. Bits in the RX FIFO reached the programmed limit)
;bit14-	POR (Power-on reset)
;bit13-	RGUR (TX reg under run, reg over write) or..
;	FFOV (RX FIFO overflow)
;bit12-	WKUP (Wake-up timer overflow)
;bit11-	EXT (Logic level on interrupt pin)
;bit10-	LBD (Low battery detect, voltage is below the programmed limit)
;bit9-	FFEM (FIFO is empty)
;BIT8-	ATS (detected strong enough RF signal) when enable receiver (er) is OFF OR...
;	RSSI (strength of the incoming signal is above programmed limit) with (er) ON

;bit7-	DQD (Data quality detector output)
;bit6-	CRL (Clock recovery locked)
;bit5-	ATGL (Toggling in each AFC cycle)
;bit4-	OFFS-6 (MSB of the measured frequency offset (sign of the offset value))
;bits 3..0 OFFS(3..0)  Offset value to be added to the value of the frequency-
;	  control parameter (Four LSB bits)
;Note: In order to get accurate values the AFC has to be disabled during the
;read by clearing the "en" bit in the AFC Control Command (bit 0).


.equ	RF_RX_ON	= 0x8281	; RX on, low-bat detect off, no clk out
.equ	RF_TX_ON	= 0x8229	; TX on, low-bat detect off, no clk out
.equ	RF_IDLE_MODE    = 0x8259	; xtal on, RX & TX all off low-bat detect off, no clk out
.equ	RF_WAKEUP_MODE  = 0x8203	; xtal, RX, TX, low-bat, clk out all off, wakeup ON
.equ	RF_TIMER_LEN    = 0xE47D	; WAKE-UP timer set for 2-sec
.equ	RF_TIMER_1      = 0xE2FA	; WAKE-UP timer set for 1-sec
.equ	RF_TIMER_4      = 0xE4FA	; WAKE-UP timer set for 4-sec
.equ	RF_TIMER_59     = 0xE97c	; WAKE-UP timer set for 59-sec
.equ	RF_SLEEP_MODE   = 0x8203	; xtal, RX, TX all off low-bat off, no clk out

;.equ	maxdata		= 66		; maximum PAYLOAD size.
;.equ	maxdata		= 100		; maximum PAYLOAD size. Total PACKET length must be < 255
.equ	maxdata		= 200		; maximum PAYLOAD size. Total PACKET length must be < 255
					; .. since only using 8-bit values
;



; now header-byte values (packet types)
.equ	cmdpack		= 0	; command mode packet. terminal mode to a remote node
; a command response from the 'remote' node has bit-7 set ie cmdpack+0x80

.equ	beaconpack	= 1	; beacon packet. Just to indicate existance
.equ	fwdpack		= 2	; for forwarding. to reach indirect destination
.equ	timepack	= 3	; time packet. from hub (or node) to node
.equ	datapack	= 4	; 'normal' data packet. Whatever
.equ	ackpack		= 6	; acknowlege packet. as it says
.equ	configpack	= 7	; to remotely configure a node
.equ	digipack	= 8	; read/set digital I/O
.equ	ananpack	= 9	; read/set analog I/O
.equ	pinpack		= 10	; combination of 8 & 9
.equ	pwmpack		= 11	; read/sets PWM output
.equ	pingpack	= 12	; 'are you there' packet
.equ	connectpack	= 13	; request 'command connection'
.equ	temppack	= 14	; temperature reading

.equ	nakpack		= 21	; NAK response. Receiver got it but does not understand the request/command



;************************ -------------------------- **************************
;*********************** configuration data in eeprom *************************
;************************ -------------------------- **************************
.eseg
config_start:

node_address:		.byte	1	; this nodes address
node_group:		.byte	1	; this nodes group
node_channel:		.byte	1	; this nodes channel (frequency)
node_rate:		.byte	1	; over-the-air data rate
node_retries:		.byte	1	; how many send attempts
node_beacon:		.byte	1	; 0=off, otherwise time in minuits
node_dest:		.byte	1	; default send (destination) address
node_e:			.byte	1	; not used yet...
node_f:			.byte	1	; not used yet...
node_g:			.byte	1	; not used yet...
node_h:			.byte	1	; not used yet...
node_i:			.byte	1	; not used yet...
node_j:			.byte	1	; not used yet...

config_end:

config_data_crc_low:	.byte	1	; crcL of config data
config_data_crc_high:	.byte	1	; crcH byte
;******************************************************************************



; *****************************************************************************
; ************************* TX block format ***********************************
; *****************************************************************************
; 0xAA	x3 is the preamble to allow the receiver RF stage to lock up (0xAA,0xAA,0xAA)
; 0x2D	sync-byte (built into the silicon of the receiver) for "fill fifo on sync-byte/s"
; 0xD4	2nd sync-byte.(groupbyte) This is changeable and can be used as a 'group select'
; XX	header. (headerbyte) Defines packet type
; XX	Length of payload (DATA) block. Max of 66
; XX...	data payload (1st two bytes can be SOURCE addr, DEST addr so then XX bytes of DATA)
; XX	CRC16, little-endian (LS-byte is 1st in stream, then MS-byte) X-modem CRC16
; XX	X2 post-amble (0xAA, 0xAA). These bytes are fed into the TX stream but because
; there is a 2-byte TX buffer they only serve to enable the CRC bytes to actualy
; get transmitted. After the last CRC byte, the 1st of the post-amble bytes is about
; to be shifted out into the TX stage. One of the post-amble bytes is sent then
; the transmitter is switched off. Switching off before this byte causes (on my
; desktop testing) about a 2.5% error with the last bit transmitted.
; Once I started to send (actualy transmit) ONE postamble byte these errors stopped.

;------------------------------------------------------------------------------
	.dseg
;************************ -------------------------- **************************
;************************ transmitter message buffer **************************
;************************ -------------------------- **************************

TX_base:
TX_txcount:		.byte	1	; byte-downcount as being sent by int routine
TX_total_length:	.byte	1	; calculated before 1st transmission. TX_datasize+12
;TX_datasize+(3xpreamble)+(2xpostamble)+(2xcrc)+(2xsync)+(1xheader)=(1xdatasize)+1
; the +1 at the end does not get sent to RFM12B chip. The interrupt is used to switch OFF transmit mode

TX_pream:		.byte	3	; fixed at 0xAA. Preamble
TX_sync_code:		.byte	1	; Always 0x2d. Fixed in receiver silicon
TX_group_code:		.byte	1	; actualy 2nd sync-byte. (redefinable in software)
TX_header_code:		.byte	1	; defines 'type' of packet

TX_datasize:		.byte	1	; how many actual DATA (payload) bytes in the buffer
; 1st 2 bytes in DATA can be used as source/dest adresses so
; TX_data_buffer address is the same as TX_sourcenode
TX_data_buffer:
TX_sourcenode:		.byte	1
TX_destnode:		.byte	1+maxdata+2	; space for CRC16 & 2xpostamble bytes



;************************ ----------------------- *****************************
;************************ receiver message buffer *****************************
;************************ ----------------------- *****************************

; If the receive buffer is purely used for receiving, some of the bytes, ie
; RX_total_length, RX_pream, RX_sync_code, RX_group_code can be deleted. They are
; present here so a received message can be easily forwarded
RX_base:
RX_txcount:		.byte	1	; byte-upcount as being sent
RX_total_length:	.byte	1	; calculated before 1st transmission. RX_datasize+12
;RX_datasize+(3xpreamble)+(2xpostamble)+(2xcrc)+(2xsync)+(1xheader)=(1xdatasize)+1
; the +1 at the end does not get sent to RFM12B chip. The interrupt is used to switch OFF transmit mode
RX_pream:		.byte	3	; fixed at 0xAA
RX_sync_code:		.byte	1	; fixed
RX_group_code:		.byte	1	; actualy 2nd sync-byte. (redefinable in software)
RX_header_code:		.byte	1	; defines 'type' of packet

RX_datasize:		.byte	1	; how many actual DATA bytes in the buffer
; 1st 2 bytes in data buffer can be used as source/dest adresses so
; RX_data_buffer address is the same as RX_sourcenode
RX_data_buffer:
RX_sourcenode:		.byte	1
RX_destnode:		.byte	1+maxdata+2	; space for CRC16 & 2xpostamble bytes




;*****************************************************************************
; some buffer pointers
pointer1:		.byte	2	; TX pointer store
pointer2:		.byte	2	; RX pointer store


;************************ ------------------------ ****************************
;***********************  Config bytes from EEPROM  ***************************
;************************ ------------------------ ****************************
; these are read from eeprom into ram at startup
config_data_ram:	.byte	config_end-config_start ; space to copy config data into

; these are the addresses of the individual config items. Not all are used yet.
.equ	config_node_address	= config_data_ram	; node address
.equ	config_node_group	= config_data_ram+1	; node	group
.equ	config_node_channel	= config_data_ram+2	; node channel (frequency)
.equ	config_node_rate	= config_data_ram+3	; node over-air data rate
.equ	config_node_dest	= config_data_ram+4	; where to send to
.equ	config_node_retries	= config_data_ram+5
.equ	config_node_beacon	= config_data_ram+6
.equ	config_node_e		= config_data_ram+7
.equ	config_node_f		= config_data_ram+8
.equ	config_node_g		= config_data_ram+9
.equ	config_node_h		= config_data_ram+10
.equ	config_node_i		= config_data_ram+11
.equ	config_node_j		= config_data_ram+12


;************************ ------------------------ ****************************
;***********************          code              ***************************
;************************ ------------------------ ****************************

.cseg

; *****************************************************************************
; DESCR: initial setup of RFM12B.
; ENTRY: Some items are fixed, others are from config set in eeprom
; EXIT: RFM12 initialised and in idle (tx-rx-OFF, xtal ON)
; CHANGES: 
; USES: 
; *****************************************************************************
init_RFM12B:
	rcall	wait_powerup			; if RFM12B just powered on, pause briefly till its ready

	RFcommand	0x80D7			;//EL,EF,433band,11.5pF
	RFcommand	RF_IDLE_MODE		;idle, TC & RX off, xtal ON

	lds	temp1,config_node_channel	; read channel (frequency) from eeprom
	rcall	set_channel			; write it

	lds	temp1,config_node_rate		; read data rate from eeprom
	rcall	set_datarate			; write it

	RFcommand	0x94A2			; VDI,FAST,134kHz,0dBm,
	RFcommand	0xC2AC			; AL,!ml,DIG,DQD4

	lds	temp1,config_node_group		; read group from eeprom
	rcall	set_group			; write it

	RFcommand	0xCA83	; FIFO8,SYNC,ff,DR.  FIFO depth is in BITS, not BYTES
	RFcommand	0xC48A	; @PWR,NO RSTRIC,!st,!fi,O
	RFcommand	0x9850	; pos shift, 90Khz dev, max out
	RFcommand	0xCC77	; PLL settings
	RFcommand	0xE000	; NOT USE
	RFcommand	0xC800	; NOT USE
	RFcommand	0xC400	; 1.66MHz,2.2V
	ret



; *****************************************************************************
; DESCR: calculates CRC16 of the memory block for transmission
; ENTRY: datacount has PAYLOAD count, payload in ram at [pointer1]-see TXbase/RXbase
; EXIT: CRC16 at end of data, postamble added, (RX)(TX)_total_length filled in
; CHANGES: memory block, X, CRC bytes, (RX)(TX)_total_length
; USES: init_crc, addtocrc
; called from prepare_TX
; *****************************************************************************
build_crc:
	push	temp1
	push	temp2

	loadX	pointer1		; get base ddress of data block
	adiw	xh:xl,6			; point at 1st byte to add to CRC (group)
	rcall	init_crc		; zero it
	mov	loopcounter,datacount	; payload count
	inc	loopcounter		; add extras pre-data (see TX block format above)
	inc	loopcounter
	inc	loopcounter
addloop:
	ld	temp1,x+
	rcall	addtoCRC
	dec	loopcounter
	brne	addloop
; crc done. now store it at the end of the payload
	st	x+,crc_low
	st	x+,crc_high
; and finaly, fill in the ending postamble
	ldi	temp1,preamble	; 2x postamble bytes
	st	x+,temp1
	st	x,temp1

	loadX	pointer1	; get base of ram block
	adiw	xh:xl,1		; point at (RX)(TX)_total_length
	mov	temp1,datacount	; get payload length
	ldi	temp2,11+1	; get overhead. 11 bytes overhead, the extra 1 is to stop the TX int
	add	temp1,temp2	; add payload & overhead
	st	x,temp1		; store at (RX)(TX)_total_length

	pop	temp2
	pop	temp1
	ret			; message in ram completed & ready to send



; *****************************************************************************
; DESCR: sends 16-bit data from SPI port to device
; ENTRY: high byte in accH, low byte in accL
; EXIT: -
; CHANGES: temp1.
; USES: spi_send
; *****************************************************************************
sendword:
	cbi	SSpin		; select chip
	mov	temp1,accH
	rcall	spi_send	; data preserved over call
	mov	temp1,accL	; actual data byte
	rcall	spi_send	; data preserved over call
	sbi	SSpin		; de-select chip
	ret


; *****************************************************************************
; DESCR: returns all 16-bits of status reg
; ENTRY: -
; EXIT: accH=high byte of status, accL=low byte
; CHANGES: temp1, accL, accH
; USES: spi_send
; *****************************************************************************
RFMstatus:
	cbi	SSpin		; select chip
	clr	temp1
	rcall	spi_send
	in	accH,spdr	; copy read-in high byte
	clr	temp1
	rcall	spi_send
	in	accL,spdr	; copy read-in low byte
	sbi	SSpin
	ret



; ****************************************************************************
; DESCR: gets the 1st 8 bits (b15..b8) of status (The interrupt bits)
; ENTRY: -
; EXIT: 8-bits of status in accH
; CHANGES: accH, temp1
; USES: spi_send
; *****************************************************************************
RFMintbits:
	cbi	SSpin		; select chip
	clr	temp1
	rcall	spi_send
	in	accH,spdr
	sbi	SSpin
	ret



; *****************************************************************************
; DESCR: checks RFM12B status flags for newly powered-on.
; ENTRY: -
; EXIT: If new power-on, pause for 200ms. If not, just exit
; CHANGES: accH, accL
; USES: msdelay function
; *****************************************************************************
wait_powerup:
	rcall	RFMstatus
	sbrc	accH,6		; see if a power-on reset
	rjmp	POD		; power-on-delay
	ret
POD:	msdelay	200		; power-on delay. Commands not accepted before 100ms
	ret			; so we wait 200ms just to be extra sure....


; *****************************************************************************
; DESCR: restarts the search for sync-bytes for fifo-fill in the receive chain
; ENTRY: none
; EXIT: none
; CHANGES: accL, accH
; USES: sendword
; *****************************************************************************
resync_RX:
	RFcommand	0xCA81		; bit ff off
	RFcommand	0xCA83		; and back on
	ret


; *****************************************************************************
; DESCR: receive-message started as a background task. Flags set when complete
; or errors.
; ENTRY: groupbyte(reg, 2nd sync-byte) to receive desired packets
; EXIT: message hunt started.
; CHANGES: many things...
; USES: sendword, init_CRC, addtoCRC, resync_RX
; *****************************************************************************
begin_RX:
	ext0_OFF			; make sure int0 is OFF
	error	0			; start off with no error conditions
	RFcommand 0x8281		; ensure RX chain ON.
	ldi	state,state2		; show that just beginning
	rcall	init_CRC		; initialise CRC total (ie to zero)
	ldy	ram,RX_group_code	; where to start saving data

	lds	temp1,config_node_group	; from config data
	st	y+,temp1
	rcall	addtoCRC		; start of CRC. GROUPBYTE added 1st

	call	RFMstatus		; read to clear any int bits
	rcall	resync_rx		; restart sync-bytes hunting process
	ext0_ON				; clear any pending & enable INT0
	ret
; The groupbyte reception is implied since fifo fill would never start unless
; both of the sync-bytes are received. This is why it is added to the receive
; CRC here. It never gets passed to the 'outside world' as received data, its
; just a trigger internaly to the RFM12B to start the fifo-fill process, this is
; the 'data' that gets passed on to the micro, not the trigger bytes.




; *****************************************************************************
; DESCR: reads RFM12B status bytes ( both-bytes). If RX data is available,
; keeps reading another 8 bits which reads the RX fifo
; ENTRY: -
; EXIT: C=data in temp1, NC=no data
; CHANGES: accL, accH, temp1
; USES: spi_send
; *****************************************************************************
RFMread:
	cbi	SSpin		; select chip
	clr	temp1
	rcall	spi_send
	in	accH,spdr	; copy read-in high byte
	clr	temp1
	rcall	spi_send
	in	accL,spdr	; copy read-in low byte

	clc			; clear carry
	sbrs	accH,7		; testing for 'rx-ready' bit
	rjmp	nodata		; if no data ready, exit
	clr	temp1
	rcall	spi_send	; now reads in fifo
	in	temp1,spdr	; data into temp1
	sec			; to show got data
nodata:	sbi	SSpin		; transaction finished
	ret




; *****************************************************************************
; DESCR: formats the ram-block before intiating transmission
; ENTRY: data block base at X, datacount is payload size, payload at
; (RX)(TX)_data_buffer (inside ram block)
; EXIT: transmission started
; CHANGES: ram-block at X, X,INT0 state, pointer1
; USES: build_CRC, start_TX
; *****************************************************************************
prepare_TX:
	ext0_OFF			; make sure int0 is OFF
	storeX	pointer1		; save pointer. (TX)(RX)_base

	adiw	xh:xl,2			; point at 1st preamble byte location
	ldi	temp1,preamble		; set-up 3Xpreamble bytes
	st	x+,temp1
	st	x+,temp1
	st	x+,temp1

	ldi	temp1,syncbyte		; get 1st syncbyte. This is a fixed value
	st	x+,temp1		; save into buffer

	lds	temp1,config_node_group	; from eeprom config set
	st	x+,temp1		; changeable in software (2nd sync byte)
	st	x+,headerbyte		; frame type
	st	x,datacount		; data payload count

	rcall	build_CRC		; fills in the CRC bytes and TX_total_length
; message now complete in ram
; (TX)(RX)_total_length/preamble/sync/group/header/count/data/checksum/postamble

free:	rcall	test_listen		; wait till the airwaves are available
	brcs	free			; loop waiting till no-carrier detected

do_tx:	rcall	start_TX		; loads the TX count and sets TX mode

	ret

; *****************************************************************************
; DESCR: re/transmit a packet from ram after prepare_TX has constructed it
; ENTRY: prepare_TX must have been called for the data. pointer1 must be valid
; EXIT: packet transmission started
; CHANGES: temp1, X, state, Y, enables INT0
; USES: intTXdata, sendword
; *****************************************************************************
start_TX:
	ldi	state,state10
	loadX	pointer1		; base of data block
	adiw	xh:xl,1			; point at total length
	ld	temp1,x			; total length location
	dec	temp1			; because we are just going to send the 1st byte
	st	-x,temp1		; int routine downcount location
	adiw	xh:xl,2			; mow points to 1st preamble byte
	mov	yl,xl			; copy this location to Y pair
	mov	yh,xh
	RFcommand RF_TX_ON		; switch TX chain ON
	ld	temp1,y+		; get 1st preamble byte
	rcall	intTXdata		; send it, but keep SS low
	ext0_ON				; turn on ext0 int for (TX buffer ready)
	ret


; *****************************************************************************
; DESCR: loads test string into ram buffer (TX buffer)
; ENTRY: -
; EXIT: test string loaded, datacount=payload length
; CHANGES: Z,Y,loopcounter,temp1,headerbyte
; USES: -
; *****************************************************************************
get_test_string:
	ldz	rom,testdata		; test string in flash
	ldy	ram,TX_data_buffer	; data payload area in ram
	clr	loopcounter		; init upcounter

xfer:	lpm	temp1,z+		; get source data
	cpi	temp1,0			; is it end of string?
	breq	dun			; if yes, jmp
	st	y+,temp1		; No, so save in buffer
	inc	loopcounter		; inc byte count
	rjmp	xfer			; and back to get the next byte

dun:	mov	datacount,loopcounter	; actual DATA payload
	ldi	headerbyte,'x'
	ret

; MAX payload
testdata:
	.db	"012345678901234567890123456789012345678901234567890123456789012345",0

; just a message
;testdata:
	.db	"This is a test string so we can see if it works ok.",0
; zero payload
;testdata:
	.db	0	; NO payload




; *****************************************************************************
; DESCR: transmits the FIRST byte of a prepared message. The rest are all sent
; from within the interrupt routine (INT0)
; ENTRY: complete message assembled in ram ready to send
; EXIT: 1st databyte is transmitted
; CHANGES: accH, accL, temp1
; USES: RFMintbits, spi_send
; *****************************************************************************
intTXdata:
	push	temp1		; 1st, save the data byte
wtTX:	rcall	RFMintbits	; get module int status bits
	sbrs	accH,7		; testing for 'TX ready' bit
	rjmp	wtTX		; loop till unit ready

; here when ready to accept data.
	pop	temp1		; restore data to send

	ldi	accH,0xb8	; write to TX register command
	mov	accL,temp1	; data to write

	cbi	SSpin		; select chip (RFM12B module)
	mov	temp1,accH	; send command
	rcall	spi_send	;
	mov	temp1,accL	; send data byte to TX
	rcall	spi_send	; 
	ret
; The SS signal is kept lowered on exit from this routine so the next write
; (the next byte to TX) will go direct to the TX register as an 8-bit transfer
; because the COMMAND (0xB8XX) is still active (write to TX register).
; Once the SS line is brought high, the command is finished.
; Every time the TX register is available, the int0 line is activated so the
; 8-bit transfer can happen in the interrupt routine as a background task



; *****************************************************************************
; DESCR: waits till a TX is finished
; ENTRY: transmission in progress
; EXIT: transmission completed. RF=idle, state=11, ext0=OFF
; CHANGES: -
; USES: -
; *****************************************************************************
wait_TXend:
	jne	state,state11,wait_TXend	; wait till TX finished
	ret





; Switch on RX and wait 500uSec for receiver to stabilise, then test RX level
; to see if allready a carrier present (for listen before send, collision avoidance)
; *****************************************************************************
; DESCR: checks for carrier present, ie another node transmitting
; ENTRY: 
; EXIT: CS=no carrier detected. CC=carrier detected
; CHANGES: 
; USES: 
; *****************************************************************************
test_listen:
	rfcommand RF_RX_ON	; turn receiver ON, no interrupt enabled
	cydelay	3000		; a 500uSec delay
	rcall	RFMintbits	; read first 8 status bits
	push	accH		; save status bits
	RFcommand RF_IDLE_MODE	; back to idle
	pop	accH		; restore status bits
	lsr	accH		; bit0 into C. CS=carrier CC=no carrier
	ret			; 



; 
; *****************************************************************************
; DESCR: waits up to 12.5ms for NO detected CARRIER receive.
; ENTRY: none
; EXIT: CC=no carrier, CS=carrier found and timeout
; CHANGES: none
; USES: test_listen
; *****************************************************************************
findgap:
	rcall	test_listen	; 500uSec delay built-in
	brcc	nocar
	msdelay 2
	rcall	test_listen
	brcc	nocar
	msdelay 2
	rcall	test_listen
	brcc	nocar
	msdelay 2
	rcall	test_listen
	brcc	nocar
	msdelay 2
	rcall	test_listen
	brcc	nocar
	msdelay 2
	rcall	test_listen
	brcc	nocar
	sec
	ret
nocar:	clc
	ret




; *****************************************************************************
; DESCR: sets tx/rx group code in the RFM12B
; ENTRY: group in temp1 (0..0xFF)
; EXIT: RFM12B register updated
; CHANGES: 
; USES: 
; *****************************************************************************
set_group:
	mov	accL,temp1
	ldi	accH,0xCE
	rcall	sendword
	sec
	ret


; =============================================================================

; channel spacing at 0.5Mhz steps starting at
; 1, F=200 430.5M
; 2, F=400 431
; 3, F=600 431.5M
; 4, F=800 432
; 5, F=1000 432.5M
; 6, F=1200 433
; 7, F=1400 433.5M
; 8, F=1600 434
; 9, F=1800 434.5M
; 10, F=2000 435
; 11, F=2200 435.5M
; 12, F=2400 436
; 13, F=2600 436.5M
; 14, F=2800 437
; 15, F=3000 437.5M
; 16, F=3200 438
; 17, F=3400 438.5M
; 18, F=3600 439
; 19, F=3800 439.5M
; *****************************************************************************
; DESCR: setx the channel (frequency) for TX/RX
; ENTRY: channel number in temp1 (1...19) at 0.5Mhz steps.
; EXIT: C=good, NC=error
; CHANGES: accH, accL, temp1, temp2, r0, r1
; USES: 
; *****************************************************************************
set_channel:
	cpi	temp1,20
	brlo	Setc1		; check if too high
	rjmp	badval		; bad exit

setc1:	tst	temp1		; check if zero
	brne	Setc2
	rjmp	badval		; bad exit

; good value so continue
setc2:	push	temp1		; save value
	ldi	temp2,200	; fixed to define channel spacing (0.5Mhz)
	mul	temp1,temp2	; temp1 can be 1....19
	ldi	temp2,0b10100000	; high nibble is command
	or	r1,temp2	; merge bits

	cli
	mov	accL,r0
	mov	accH,r1
	rcall	sendword
	sei
	pop	temp1		; restore entry value
	sec			; all good!
	ret

; *****************************************************************************
; DESCR: checks 'state' condition for [idle/tx-complete/RX-complete] states
; ENTRY: -
; EXIT: CS=no TX/RX, CC=TX/RX in progress
; CHANGES: -
; USES: -
; *****************************************************************************
test_state:
	jeq	state,0,stat1		; idle
	jeq	state,6,stat1		; idle with RX complete
	jeq	state,11,stat1		; idle with TX complete
badval:	clc				; RF NOT idle
	ret
stat1:	sec	; good status value, RF=idle
	ret



; *****************************************************************************
; DESCR: sets the over-the-air data rate
; ENTRY: value in temp1 (0..15). 'state' must be ok to allow change
; EXIT: CS=success, CC=failed (bad state)
; CHANGES: 
; USES: 
; *****************************************************************************
set_datarate:
	cpi	temp1,16
	brsh	badval		; if >15 then bad

rt2:	push	ZL
	push	ZH
	push	temp1
	ldz	rom,rate_table
	andi	temp1,0b00001111
	add	ZL,temp1
	brcc	rt1
	inc	ZH
rt1:	lpm	temp1,z		; value for data rate
	mov	accL,temp1
	ldi	accH,0xc6	; data rate command
	rcall	sendword	; writes the rate data to RFM12B
	pop	temp1
	pop	ZH
	pop	ZL
	sec
	ret

rate_table:
	.db	0xc6,0xa2,0x91,0x88,0x30,0x23,0x17,0x11
	.db	0x0B,0x08,0x07,0x06,0x05,0x04,0x03,0x02


; over the air rates in Kb
;0x02	; 114.943	15
;0x03	; 86.207	14
;0x04	; 68.966	13
;0x05	; 57.471	12
;0x06	; 49.261	11
;0x07	; 43.103	10
;0x08	; 38.314	9
;0x0B	; 28.736	8
;0x11	; 19.157	7
;0x17	; 14.368	6
;0x23	; 9.579		5
;0x30	; 7.037		4
;0x88	; 4.789		3
;0x91	; 2.395		2
;0xa2	; 1.232		1
;0xc6	; 0.607		0






; *****************************************************************************
; DESCR: just a dirty way to dump (in hex) 16 bytes of buffer 
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
showbuf:
	newline
	ldi	loopcounter,16
	ldx	ram,TX_base+6
shi:	ld	temp1,x+
	rcall	hexout 		;(uses current stream-destination)
	send	' '
	dec	loopcounter
	brne	shi
	ret



; *****************************************************************************
; DESCR: sets up for a sleep period
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
enter_sleep1:
	RFCOMMAND RF_TIMER_4	; WAKE-UP in 4 sec
	rjmp	comm

enter_sleep2:
	RFCOMMAND RF_TIMER_1	; WAKE-UP in 1-sec
	rjmp	comm

enter_sleep3:
	RFCOMMAND RF_TIMER_59	; WAKE-UP in 59 sec

comm:	RFCOMMAND RF_SLEEP_MODE	; shuts chip RF system down

	clr	temp1
	sts	EICRA,temp1		; enable external INT0 low-level instead of edge
	ext0_on

	ori	bitflags,0b10000000	; set bit 7 to indicate sleep

	cbi	portd,5
	in	temp1,smcr
	ori	temp1,(1<<se)
	out	smcr,temp1
	sleep			; enter power-down mode
; a LOW LEVEL on the int0 pin wakes from sleep and causes the int0 interrupt routine to be jumped to

	nop
	nop
	nop
	nop


; now we have just exited from the int0 routine after wakeup
	in	temp1,smcr
	andi	temp1,~(1<<se)
	out	smcr,temp1		; disable sleep
	andi	bitflags,0b01111111	; clear sleep flag bit

	rcall	RFMread			; read the int bits to cancel the int req
	stoprf				; sets RF to idle mode and int off
	ret



