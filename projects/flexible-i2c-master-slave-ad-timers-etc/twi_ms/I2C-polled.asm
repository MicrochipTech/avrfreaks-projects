
; These are the simple I2C Master access API's.
;	The specific master cycle type routines simply use the defined registers to fill in
;	the buffer and byte count as needed by the ISR, waiting for the bus and starting the cycle.
;
; It seems to me that the period between the Master_Wait routine and the Master_Send routine that
;	sends the START is the most dangerous part of the implementation.  It seems impossible so far to
;	achieve valid slave responses in all cases when doing master cycles, and this is the purpose for
;	raising this project to others with more knowledge.
;
;I2C (TWI) defs (assuming 8MHz clk)
.equ twbr_init = 34		;this is the master clock rate value, and results in a 100KHz clk
.equ twcr_init = 0x04	;set the enable bit
.equ twsr_init = 0		; no multipliers
.equ twar_init = 0xFE	;disable slave more or less


.equ TWI_RW_mask = 0x01
.equ TWI_RW_bit = 0

;
;	Wait for the the bus to be available for master, then grab the lock.
;	Note that this enters with irq's enabled, and exits with them turned off. ** changed to reduce irq latency **
;
I2C_Master_Wait:
	lds	Temp, TWI_statusReg
	sbrc	Temp, TWI_Stat_Master_bit
	rjmp	I2C_Master_Wait					;wait for previous operations to end
	lds	Temp, TWCR
	sbrc	Temp, TWSTO		;make sure the stop bit is also cleared before issuing another access
	rjmp	I2C_Master_Wait

;***** this stuff is not necessary since there is no I2C stuff allowed in ISRs
;	cli
	lds	Temp, TWI_statusReg							;with irq's disabled, make sure there is no stuff again
;	sbrc	Temp, TWI_Stat_Master_bit		;if someone sneeked in, wait longer
;	rjmp	I2C_MW_1
	sbrc	Temp, TWI_Stat_Slave_bit
	rjmp	I2C_Master_Wait

	ldi	T2, 0x05
	sts	TWCR, T2

I2C_MW_1:
;	sei
;	rjmp I2C_Master_Wait
I2C_MW_0:
	sbr	Temp, TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp
;	sei		;
	ret


;------------------WRITE I2C Adr-Data---------------------------
;
; This writes a single data byte to a device assuming the device addr,
; register addr (byte), and data are in I2C_DevAddr, I2C_Addr, and I2C_Data
;

Wr_I2C_Adr_Data:

	st	-Y, XH
	st	-Y, XL

	rcall	I2C_Master_Wait

	ldi	Temp, 3			;this is a 3 byte operation
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	cbr	I2C_DevAddr, TWI_RW_mask		;make it a write
	st	X+, I2C_DevAddr
	st	X+, I2C_Addr
	st	X, I2C_Data

	ld	XL, Y+
	ld	XH, Y+

	rjmp	TWI_Master_Send

		
;------------------WRITE I2C-Data---------------------------
;
; This writes a single data byte to a device assuming the device addr,
; and data are in I2C_DevAddr and I2C_Data.
;
; Note that this can be used to set the register address for subseqeunt reads
; as long as there is a single master talking to the device.
;

Wr_I2C_Data:

	rcall	I2C_Master_Wait

	st	-Y, XH				;save X to avoid trashing the calling routine
	st	-Y, XL

	ldi	Temp, 2		;this is a 2 byte operation
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	cbr	I2C_DevAddr, TWI_RW_mask		;make it a write
	st	X+, I2C_DevAddr
	st	X, I2C_Data

	ld	XL, Y+
	ld	XH, Y+

	rjmp	TWI_Master_Send


;------------------WRITE I2C-Word---------------------------
;
; This writes a two bytes to a device assuming the device addr, register addr,
; and data are in I2C_DevAddr, I2C_Addr, and I2C_Data (MSB or first byte) and I2C_DataLB (lsb or second byte).
;

Wr_I2C_Word:

	rcall	I2C_Master_Wait

	st	-Y, XH
	st	-Y, XL

	ldi	Temp, 4			;this is a 4 byte operation
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	cbr	I2C_DevAddr, TWI_RW_mask		;make it a write
	st	X+, I2C_DevAddr
	st	X+, I2C_Addr
	st	X+, I2C_Data
	st	X, I2C_DataLB

	ld	XL, Y+
	ld	XH, Y+

	rjmp	TWI_Master_Send


;------------------Read I2C-BYTE---------------------------
;
; This reads a single data byte from a device assuming the device addr is
; setup and the internal device register pointer is good.  Data is returned
; in I2C_Data.
;

Rd_I2C_Byte:

	rcall	I2C_Master_Wait

	st	-Y, XH
	st	-Y, XL

	ldi	Temp, 2			;this is a 2 byte operation
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	sbr	I2C_DevAddr, TWI_RW_mask		;make it a read
	st	X+, I2C_DevAddr

	rcall	TWI_Master_Send		;call so that we come back here

	rcall	I2C_Master_Wait		;wait for it to complete and hold it so that the data is not overwritten

	ldi	Temp, 0xc5
	sts	TWCR, Temp

	ld	I2C_Data, X				;get the data byte

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp		;give the bus back

	sei

	ld	XL, Y+
	ld	XH, Y+

	ret

;------------------Read I2C_Adr-BYTE---------------------------
;
; This reads a single data byte from a device after setting the internal register
; address.  Data is returned in I2C_Data.
;	Note that repeated starts are not done here so there is no device lock to prevent
;	the addr being changed on the device, so if multiple masters are accessing the same device
;	there may be problems.
;

Rd_I2C_Adr_Byte:

	rcall	I2C_Master_Wait

	st	-Y, XH
	st	-Y, XL

	ldi	Temp, 2			;Start with a 2 byte write to set the addr
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	cbr	I2C_DevAddr, TWI_RW_mask		;make it a write
	st	X+, I2C_DevAddr
	st	X, I2C_Addr

	rcall	TWI_Master_Send		;call so that we come back here

	rcall	I2C_Master_Wait		;wait for it to complete and hold it so that the data is not overwritten

	sbiw	XH:XL, 1					;now do the read, still 2 byte, set the read bit
	sbr	I2C_DevAddr, TWI_RW_mask		;make it a read
	st	X+, I2C_DevAddr

	rcall	TWI_Master_Send		;call so that we come back here

	rcall	I2C_Master_Wait		;wait for it to complete and hold it so that the data is not overwritten

	ldi	Temp, 0xc5
	sts	TWCR, Temp

	ld	I2C_Data, X				;get the data byte

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp		;give the bus back

	sei

	ld	XL, Y+
	ld	XH, Y+

	ret

;------------------Read I2C_Adr-Word---------------------------
;
; This reads two data bytes from a device after setting the internal register
; address.  Data is returned in I2C_Data (MSB or first byte) and I2C_DataLB (LSB or second byte).
;	Note that as above, repeated starts are not done here so there is no device lock to prevent
;	the addr being changed on the device, so if multiple masters are accessing the same device
;	there may be problems.
;

Rd_I2C_Adr_Word: 

	rcall	I2C_Master_Wait

	st	-Y, XH
	st	-Y, XL

	ldi	Temp, 2			;Start with a 2 byte write to set the addr
	sts	TWI_msgSize, Temp

	ldi	XL, low(TWI_Master_buf)
	ldi	XH, high(TWI_Master_buf)

	cbr	I2C_DevAddr, TWI_RW_mask		;make it a write
	st	X+, I2C_DevAddr
	st	X, I2C_Addr

	rcall	TWI_Master_Send		;call so that we come back here

	rcall	I2C_Master_Wait		;get the bus back after the read completes

	ldi	Temp, 3
	sts	TWI_msgSize, Temp
	sbiw	XH:XL, 1					;do the read, now 3 byte, set the read bit
	sbr	I2C_DevAddr, TWI_RW_mask		;make it a read
	st	X+, I2C_DevAddr

	rcall	TWI_Master_Send		;call so that we come back here

	rcall	I2C_Master_Wait		;wait for it to complete and hold it so that the data is not overwritten

	ldi	Temp, 0xc5
	sts	TWCR, Temp
	
	ld	I2C_Data, X+			;get the first data byte
	ld	I2C_DataLB, X			;and second

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp		;give the bus back

	sei

	ld	XL, Y+
	ld	XH, Y+

	ret
