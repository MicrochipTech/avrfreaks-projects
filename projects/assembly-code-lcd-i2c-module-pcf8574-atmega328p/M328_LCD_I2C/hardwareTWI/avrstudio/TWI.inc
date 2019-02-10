;------------------------------------
; Set TWIBitRate for fclk=16Mhz
;100Khz=16MHz/(16+2*TWBR*PRESCALER)
I2CInit:
	ldi temp,75		;100KHz
	sts TWBR,temp
	ldi temp,0x00	;TWPS0, TWPS1 both are 0 (prescaler = 1)
	sts TWSR,temp
	ret
;------------------------------------
WriteI2C:
	call I2CSTART
	ldi I2WData,SLA+W
	call I2CPUT

	mov	I2WData,LCD_Reg
	call	I2CPUT
	call	I2CStop
	ret
;---------SEND I2C START------------------
;This will send a I2WData out as the address
;sends start condition and address
;
I2CSTART:
	ldi temp, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
	sts TWCR,temp

;wait for start condition to be sent.
;When TWINT in TWCR is cleared, it is sent
;
WAIT_START:
	lds temp,TWCR
	sbrs temp,TWINT
	rjmp WAIT_START

;check TWSR for bus status.
;mask last three bits (prescaler value)

	lds temp,TWSR
	andi temp,0b11111000
	cpi temp,START
	breq PC+2
	jmp errloop
	ret
;
;---------------PUT I2C-------------------------
;byte to send is stored in I2WData
;then this function will disable the TW int, write data to TWDR, 
;and wait for an ack
;
I2CPUT:
	ldi temp,(0<<TWINT) | (1<<TWEN)
	sts TWCR,temp
	sts TWDR,I2WData
	ldi temp,(1<<TWINT) | (1<<TWEN)
	sts TWCR,temp
;
WAIT_DONE:
	lds temp,TWCR
	sbrs temp,TWINT
	rjmp WAIT_DONE
	ret
;
;-------------SEND I2C Stop-------------------
I2CSTOP:
	ldi temp,(1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
	sts TWCR,temp
;
;check TWCR to see if there is still a transmission
;if not, stop bit has been sent
;
Check1:
	lds temp,TWCR
	andi temp,0b00010000
	brne Check1
	ret
;----------------------------------------------------------------
errloop:
;no error checking here
	rjmp	errloop
