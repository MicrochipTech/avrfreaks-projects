


; the default config-set is  slightly different whether the node is created as RX or TX
; so if you make one of each, the demo code will let the TX unit send its temp
; readings to the correctly addressed RX node.

	#ifdef	RX_

.equ	default_address 	= 255	; default receiver node address
.equ	default_group		= 0xD4	; dec-212 default group
.equ	default_channel		= 0x06	; default channel =433Mhz. CH6
.equ	default_rate		= 11	; default rate=49.2K. rate 9
.equ	default_retries		= 0	; 
.equ	default_beacon		= 0	; 
.equ	default_dest		= 1	; default destination


	#elif	TX_

.equ	default_address 	= 0x01	; default node address
.equ	default_group		= 0xD4	; dec-212 default group
.equ	default_channel		= 0x06	; default channel =433Mhz. CH6
.equ	default_rate		= 11	; default rate=49.2K. rate 9
.equ	default_retries		= 0	; 
.equ	default_beacon		= 0	; 
.equ	default_dest		= 255	; default destination

	#endif



.dseg

EEadr:		.db	2	; EEprom addr start, Low byte then high


.cseg

; *****************************************************************************
; DESCR: copies the eeprom config data into ram
; ENTRY: config data must exist
; EXIT: data copied
; CHANGES: ram
; USES: eeprom_read
; *****************************************************************************
configset_to_ram:
	ldX	ram,config_start			; get eeprom data start
	storeX	EEadr
	ldi	loopcounter,config_end-config_start	; byte count
	ldZ	ram,config_data_ram
copi:	rcall	eeprom_read
	adiw	xh:xl,1
	storeX	EEadr
	st	z,temp1
	dec	loopcounter
	brne	copi
	ret



set_config:
	ldX	ram,node_address
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_address,temp1

	ldX	ram,node_group
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_group,temp1
	call	set_group

	ldX	ram,node_channel
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_channel,temp1
	call	set_channel

	ldX	ram,node_rate
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_rate,temp1
	call	set_datarate

	ldX	ram,node_retries
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_retries,temp1

	ldX	ram,node_beacon
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_beacon,temp1

	ldX	ram,node_dest
	storeX	EEadr
	rcall	EEPROM_read
	sts	config_node_dest,temp1
	ret





test_config:
	call	init_crc		; zero CRC
	ldX	ram,config_start	; start of config data
	storeX	EEadr			; set pointer

; get length of data
	ldi	loopcounter,config_end-config_start
check1:	rcall	EEPROM_read		; get byte
	call	addtoCRC		; add to CRC
	loadX	EEadr
	adiw	xh:xl,1			; inc pointer
	storeX	EEadr			; save it
	dec	loopcounter
	brne	check1			; loop till read all config data
; crc now complete so compare to stored one in eeprom
	rcall	EEPROM_read		; get byte (crcL)
	cp	temp1,crc_low
	brne	config_fail
	adiw	xh:xl,1			; inc pointer
	storeX	EEadr			; save it
	rcall	EEPROM_read		; get byte (crcH)
	cp	temp1,crc_high		; compare next
	brne	config_fail
	sec				; shows data valid
	ret

config_fail:
	stringout	"config CRC failure"
	clc
	ret



default_config:
	call	init_crc		; zero CRC
	ldX	ram,config_start	; start of config data
	storeX	EEadr			; set pointer
; get length of data
	ldi	loopcounter,config_end-config_start

set1:	ldi	temp1,default_address		; default chan=1
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter

	ldi	temp1,default_group		; default group=0xd4
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter

	ldi	temp1,default_channel		; default channel=6 (433Mhz)
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter

	ldi	temp1,default_rate		; rate = 49.2k
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter

	ldi	temp1,default_retries
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter

	ldi	temp1,default_beacon
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter


	ldi	temp1,default_dest		; =255
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter




set2:	clr	temp1			; fill the rest with zeros
	call	addtoCRC			; add to crc
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	dec	loopcounter
	brne	set2

; now write CRC low
	mov	temp1,crc_low
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	
; now write CRC high
	mov	temp1,crc_high
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
	ret




; *****************************************************************************
; DESCR: reads the EEprom data and updates the CRC to match
; ENTRY: -
; EXIT: CRC in eeprom updated
; CHANGES: CRC in eeprom
; USES: rcall	EEPROM_read, EEPROM_write
; *****************************************************************************
update_CRC:
	call	init_crc		; zero CRC
	ldX	ram,config_start	; start of config data
	storeX	EEadr			; set pointer

; get length of data
	ldi	loopcounter,config_end-config_start
check4:	rcall	EEPROM_read		; get byte
	call	addtoCRC		; add to CRC
	loadX	EEadr
	adiw	xh:xl,1			; inc pointer
	storeX	EEadr			; save it
	dec	loopcounter
	brne	check4			; loop till read all config data

; crc now complete so write to EEprom
	mov	temp1,crc_low
	rcall	EEPROM_write
	loadX	EEadr
	adiw	xh:xl,1
	storeX	EEadr
; now write CRC high
	mov	temp1,crc_high
	rcall	EEPROM_write
	ret




; *****************************************************************************
; DESCR: writes a byte to eeprom (erase then write cycle)
; ENTRY: byte to write in temp1, addr to write in EEadr (ram)
; EXIT: byte written
; CHANGES: eeprom
; USES: -
; *****************************************************************************
EEPROM_write:
	sbic	EECR,EEPE	; Wait for completion of previous write
	rjmp	EEPROM_write	; loop till ready
	push	temp1
	lds	temp1,EEadr	; get low byte
	out	EEARL,temp1
	lds	temp1,EEadr+1	; get high byte
	out	EEARH, temp1	; Set up address (r18:r17) in address register
	pop	temp1
	out	EEDR,temp1	; Write data (r16) to Data Register
	cli			; disable ints for timed access
	sbi	EECR,EEMPE	; Write logical one to EEMPE
	sbi	EECR,EEPE	; Start eeprom write by setting EEPE
	sei			; re-enable ints
	ret




; *****************************************************************************
; DESCR: reads a byte from EEprom
; ENTRY: address to read in EEadr (ram)
; EXIT: data in temp1
; CHANGES: temp1
; USES: -
; *****************************************************************************
EEPROM_read:
	sbic	EECR,EEPE	; Wait for completion of previous write
	rjmp	EEPROM_read		

	lds	temp1,EEadr	; get low byte
	out	EEARL,temp1
	lds	temp1,EEadr+1	; get high byte
	out	EEARH, temp1	; Set up address (r18:r17) in address register
	cli
	sbi	EECR,EERE	; Start eeprom read by writing EERE
	sei
	in	temp1,EEDR	; Read data from Data Register
	ret































