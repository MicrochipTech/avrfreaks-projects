.ifdef TWDR
.if TWI
;*******************************************************************
;	TWI interface routines
;****************************************************************************/
;  TWIcommand: TWI must be called first to initialize the TWI interface. 
;	X = 0 for 400Kbps and x = 1 for 100Kbps clock.  Without [x] is same 
;	as x = 0. Defaults to Master @ 400Kbps with PORT pull-ups enabled.  
;	[Note: If it is desired to use alternate pull-ups, disable the PORT 
;	pull-ups by clearing the SCL/SDA pins in the PORT register]
;****************************************************************************/
TWIcommand:
.ifdef PRR
	CLRB	PRR,PRTWI,temp				;enable TWI in PRR
.endif
	clr		U						;default to 400K
	Gosub	pushU					;save it on the stack
	GoSub	Get1stParm				;interpret line and fetch 1st variable
	ldi		temp,$01				;only values of [0..1] allowed
	cp		temp,U
	brmi	TWIcommand9				;branch if allowable exceeded 

;Is 400K TWI bit rate available?
.if (TWBCLK400K > 9 && TWBCLK400K <255)
	ldi		temp,TWBCLK400K			;Clock = 400K
.else
	ldi		temp,TWBCLK				;Clock = maximum determined
.endif

	sbrc	U,0						;skip next if bit 0 clear

;Is 100K TWI bit rate available?
.if (TWBCLK100K > 9 && TWBCLK100K <255)
	ldi		temp,TWBCLK100K			;Clock = 100K
.else
	ldi		temp,TWBCLK				;Clock = maximum determined
.endif

	STORE	TWBR,temp				;save bit rate

	SETB	PORTC,TWI_SCL 			;SCK pin is output
	SETB	PORTC,TWI_SDA 			;SDA pin is input

	clr		temp
 	STORE   TWSR, temp				;set prescaler
	com		temp					;Default content = SDA released
	STORE	TWDR,temp
	ldi		temp,((1<<TWEN)|(0<<TWIE)|(0<<TWINT)| \
				 (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC))
									;Enable TWI and release TWI pins,
									;Disable Interupt, 
									;No Signal requests.
 	STORE	TWCR,temp
	TWISndStop						;reset bus by sending a STOP condition

	rcall	TWIgetstat		;Get the current bus status

	ret

TWIcommand9:
	rjmp	TWIErr0C		;error code "0C"
;
;****************************************************************************/
;  TWAcommand: TWA [x] - TWA sends the slave address to the bus.  Returns 
;	with the bus status on the stack. Ex 1: TWA $A0 selects slave address 
;	$A0 for writing.  Ex 2: A:= TWA $A0 selects slave address $A0 for writing 
;	and returns the bus status in variable A.  [Note: This command should 
;	be used after issuing a START condition to send the desired slave address.  
;	The user must insure bit 0 of the slave address contains the R(ead) 
;	or W(rite) indicator bit AND'ed or OR'ed with the 7-bit slave address 
;	before sending.  The address may need to be left-shifted one bit position]
;****************************************************************************/
TWAcommand:
	LOAD	temp,TWCR		;get TWCR
	sbrs 	temp,TWEN		;skip next if TWEN set
	rjmp	TWAcommand9		;TWI not enabled, notify user

	GoSub	Get1stParm				;interpret line and fetch 1st variable
	STORE	TWDR,U			;save the address in TWI data reg

 	ldi		temp,((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	;Enable 
							; TWI-interface and clear the flag to send 
							; byte
 	STORE	TWCR,temp		;init the xfer

	WAIT_TWI_int			;wait for command to finish
	
	LOAD 	temp,TWSR			;get bus status
	andi	temp,$F8			;strip all but status
	cpi   	temp,TW_MTX_ADR_ACK ;slave+W ACK?
	breq  	TWAcommand1		;branch if TW_MTX_ADR_ACK

	cpi   	temp,TW_MRX_ADR_ACK ;slave+R ACK?
	brne  	TWAcommand2		;branch if not TW_MRX_ADR_ACK

TWAcommand1:
	rcall	TWIgetstat		;Get the current bus status
;	Gosub	PushU			;save on stack
	ret

TWAcommand2:
	cpi   	temp,TW_MTX_ADR_NACK;slave+W NACK?
	breq  	TWAcommand8		;branch if TW_MTX_ADR_NACK
	cpi   	temp,TW_MRX_ADR_NACK;slave+R NACK?
	breq  	TWAcommand8		;branch if TW_MRX_ADR_NACK

TWAcommand7:	;Bus error
	rjmp	TWIErr12		;error code "12"

TWAcommand8:	;NACK
	rjmp	TWIErr14		;error code "14"

TWAcommand9:	;TWI not enabled
	rjmp	TWIErr13		;error code "13"
;
;****************************************************************************/
;  TWWcommand: TWW [x] - TWW sends a byte to the bus.  Returns with 
;	the bus status on the stack.  Ex 1: TWW B eend the data held in 
;	variable B to the previously selected slave for a write operation.  
;	Ex 2: A:= TWW $A0 sends $A0 to the slave for writing and returns 
;	the bus status in variable A.  .  
;	[Note issue this command after a "TWA [x]" (SLA+W) has been issued 
;	and acknowledged by the slave].
;****************************************************************************/
TWWcommand:
	LOAD	temp,TWCR		;get TWCR
	sbrs 	temp,TWEN		;skip next if TWEN set
	rjmp	TWWcommand9		;TWI not enabled, notify user

	GoSub	Get1stParm				;interpret line and fetch 1st variable
	STORE	TWDR,U			;save the address in TWI data reg

 	ldi		temp,((1<<TWINT)|(1<<TWEN)|(1<<TWIE))	;Enable 
							; TWI-interface and clear the flag to send 
							; byte
 	STORE	TWCR,temp		;init the xfer

	WAIT_TWI_int			;wait for command to finish
	
	LOAD 	U,TWSR			;get bus status
	andi	U,$F8			;strip all but status
	cpi   	U,TW_MTX_DATA_ACK;ACK?
	brne  	TWWcommand1		;branch if not TW_MTX_DATA_ACK
	rcall	TWIgetstat		;Get the current bus status
	ret

TWWcommand1:
	cpi   	U,TW_MTX_DATA_NACK ;NACK received
	breq  	TWWcommand8		;branch if TW_MTX_DATA_NACK

TWWcommand7:	;Bus error
	rjmp	TWIErr12		;error code "12"

TWWcommand8:	;NACK
	rjmp	TWIErr14		;error code "14"

TWWcommand9:	;TWI not enabled
	rjmp	TWIErr13		;error code "13"
;
;****************************************************************************/
;  TWRcommand: Receives a byte from the TWI bus and places it onto the 
;	stack..  x = 0 to signal to the slave that this is the last byte to 
;	receive, x = 1 to signal to the slave there is more data to receive. 
;	Without [x] is same as x = 1.  Ex: A:= TWR 0 receives a byte, 
;	signal to the slave that no further data is requested and returns 
;	the data in variable A. [Note issue this command after a 
;	"TWA [x]" (SLA+R) has been issued and acknowledged by the slave].
;****************************************************************************/
TWRcommand:
	LOAD	temp,TWCR		;get TWCR
	sbrs 	temp,TWEN		;skip next if TWEN set
	rjmp	TWRcommand9		;TWI not enabled, notify user

 	ldi		U,1				;default read another byte
	Gosub	pushU			;save it on the stack
	GoSub	Get1stParm		;interpret line and fetch 1st variable
	ldi		temp,$01		;only values of [0..1] allowed
	cp		temp,U
	brmi	TWRcommand10	;branch if allowable exceeded 

							;TWI enabled, Send ACK after reception
	ldi		temp,((1<<TWEN)|(1<<TWINT)|(1<<TWEA))
	sbrs	U,0				;skip next if bit 0 is set (issue ACK)
							;TWI enabled, Send NACK after reception
	ldi		temp,((1<<TWEN)|(1<<TWINT)|(0<<TWEA))
	STORE	TWCR,temp		;init the xfer

	;set up register T to test for proper status response
	ldi   	T,TW_MRX_DATA_ACK;ACK for "N" data bytes
	sbrs	U,0				;skip next if bit 0 is set (issue ACK)
	ldi   	T,TW_MRX_DATA_NACK;NACK for last data byte


	WAIT_TWI_int			;wait for command to finish

	LOAD 	temp,TWSR		;get bus status
	andi	temp,$F8		;strip all but status

	cp		temp,T			;test for proper response
	brne  	TWRcommand7		;branch if not TW_MRX_DATA_ACK/NACK

;	cpi   	temp,TW_MRX_DATA_ACK	;ACK?
;	brne  	TWRcommand1		;branch if not TW_MRX_DATA_ACK

	LOAD	U,TWDR			;get the data byte
	Gosub	PushU			;save on stack
	ret

;TWRcommand1:
;	cpi   	temp,TW_MRX_DATA_ACK	;NACK received
;	breq  	TWRcommand8		;branch if TW_MRX_DATA_ACK

TWRcommand7:	;Bus error
	rjmp	TWIErr12		;error code "12"

TWRcommand8:	;NACK
	rjmp	TWIErr14		;error code "14"

TWRcommand9:	;TWI not enabled
	rjmp	TWIErr13		;error code "13"

TWRcommand10:	;TWI not enabled
	rjmp	TWIErr0C		;error code "0C"
;
;;****************************************************************************/
;  TWScommand: Assert a START condition on the bus.  When the TWI interface 
;	is initialized, a START condition is asserted.  Returns with the bus status 
;	on the stack.  However, the user must re-assert a START condition after a 
;	STOP condition to ready the bus for the next message sequence.
;****************************************************************************/
TWScommand:
	LOAD	temp,TWCR		;get TWCR
	sbrs 	temp,TWEN		;skip next if TWEN set
	rjmp	TWIErr13		;TWI not enabled, notify user
	TWISndStart				;assert a STOP condition


;LDS       R16,0x00BC     Load direct from data space
;SBRS      R16,7          Skip if bit in register set
;RJMP      PC-0x0003      Relative jump

	WAIT_TWI_int			;wait for command to finish

	LOAD 	U,TWSR			;get bus status
	andi	U,$F8			;strip all but status
	cpi   	U,TW_START
	breq  	TWScommand1		;branch if TW_START
	rjmp	TWIErr12		;TW_START not detected, error code "12"

TWScommand1:
;	rcall	TWIgetstat		;Get the current bus status
	Gosub	PushU			;save on stack
	ret
;
;****************************************************************************/
;  TWPcommand: Assert a STOP condition on the bus.  The user must assert 
;	a STOP condition after the last message byte has been sent to or received 
;	from the slave or to abort a transfer in progress.
;****************************************************************************/
TWPcommand:
	LOAD	temp,TWCR		;get TWCR
	sbrs 	temp,TWEN		;skip next if TWEN set
	rjmp	TWPcommand9		;TWI not enabled, notify user

	TWISndStop				;assert a STOP condition

	ldi		temp,$FF		;write data reg ro release SDA
	STORE	TWDR,temp
	rcall	TWIgetstat		;Get the current bus status
	ret						;return to caller

TWPcommand8:	;WRITE COLLISION error, notify user
	rjmp	TWIErr12		;error code "12"

TWPcommand9:	;TWI not enabled
	rjmp	TWIErr13		;error code "13"
;
;****************************************************************************/
;  TWBcommand: Queries the TWI status register for the last detected 
;	condition of the bus.  [Note: the byte returned is right-
;	shifted 3 bit positions. If a STOP condition has been detected, 
;	$01 is returned to indicate so. Ex: A:= TWB (if A = 3 then SLA+W 
;	has been transmitted and an ACK received).
;****************************************************************************/
TWBcommand:
	LOAD	U,TWCR			;get TWCR
	sbrs 	U,TWEN			;skip next if TWEN set
	rjmp	TWBcommand9		;TWI not enabled, notify user
;	LOAD	temp,TWCR
	sbrs 	U,TWSTO			;check for STOP condition
	rjmp 	TWBcommand1		;STOP flag clear, check TWINT
	ldi		U,1				;return a bit 0 = 1 to indicate STOP
	Gosub	pushU			;Push the data onto the stack
	ret

TWBcommand1:
	WAIT_TWI_int			;wait for command to finish
	rcall	TWIgetstat		;Get the current bus status
	ret						;return to caller

TWBcommand9:	;TWI not enabled
	rjmp	TWIErr13		;error code "13"
;
;*****************************************************************
;TWIgetstat: fetch the TWI status register and right-justify it
;*****************************************************************
TWIgetstat:
	LOAD	U,TWSR			;Get current bus status
	andi	U,$F8			;strip all but status
;	lsr		U				;shift status right 3 bit positions
;	lsr		U
;	lsr		U
	GoSub	pushU			;save on stack
	ret						;return to caller
;
;*****************************************************************
; TWI error processing.
;*****************************************************************
TWIErr12:	;Bus error
	STOREI	ERRCode,$12		;error code "12"
	Goto	error

TWIErr14:	;NACK
	STOREI	ERRCode,$14		;error code "14"
	Goto	error

TWIErr13:	;TWI not enabled
	STOREI	ERRCode,$13		;error code "13"
	Goto	error

TWIErr0C:	;Exceeded range of 0:1
	STOREI	ERRCode,$0C		;error code "C"
	Goto	error
;
;****************************************************************
.endif
.endif
