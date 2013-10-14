;****************************************************************************/
;TWISndStart: initiate a START condition
;****************************************************************************/
.MACRO TWISndStart
;TWI Interface enabled, Enable TWI Interupt and clear the flag and
; initiate a START condition.
	ldi		r16,((1<<TWEN)|(1<<TWINT)|(1<<TWSTA))
	STORE	TWCR,r16					;Set START condition
.ENDMACRO
;
;****************************************************************************/
;TWISndStop: insure a STOP condition
;****************************************************************************/
.MACRO TWISndStop
	ldi		r16,((1<<TWEN)|(1<<TWINT)|(1<<TWSTO))
										;Enable TWI-interface, Disable TWI 
										; Interrupt and Initiate a STOP 
										; condition.
 	STORE	TWCR,r16						;Send STOP condition
.ENDMACRO
;
;****************************************************************************/
;  MACRO Definition: Loops until TWI interrupt flag is set
;****************************************************************************/
.MACRO WAIT_TWI_int
 	SKBS	TWCR,TWINT,r16				;skip next if TWINT flag set
 	rjmp	PC-0x0003					;loop
.ENDMACRO
;
;****************************************************************************/
;  MACRO Definition: Loops until TWI STA flag is set
;****************************************************************************/
.MACRO WAIT_TWI_start
 	SKBS	TWCR,TWSTA,r16				;skip next if TWSTA flag set
 	rjmp	PC-0x0003					;loop
.ENDMACRO
;
;****************************************************************************/
;  MACRO Definition: Loops until TWI STO flag is set
;****************************************************************************/
.MACRO WAIT_TWI_stop
 	SKBS	TWCR,TWSTO,r16				;skip next if TWSTO flag set
 	rjmp	PC-0x0003					;loop
.ENDMACRO
;

