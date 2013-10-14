;*****************************************************************
;*	file: macros.inc
;*
;*	Description:
;*	Source file for application note AVR001 - Conditional Assembly
;*	and Portability Macros.
;*
;*	Defines a number of macros that makes it easier to access
;*	IO registers and extended IO registers (or SRAM locations up
;*  to adress $FF if applicable).
;*	The macros can be used to produce code that assembles to
;*	any target AVR, without considering if the accessed IO
;*	registers are located in low, standard or extended IO space
;*
;* $Revision: 2.2 $	
;* $Author: jllassen $
;* $Date: Wednesday, January 26, 2005 10:55:18 UTC $
;*****************************************************************

;*********************************************************
;*	BIT access anywhere in IO or lower $FF of data space
;*	SETB - SET Bit in IO of data space
;*	CLRB - CLeaR Bit in IO of data space
;*********************************************************

.MACRO SETB 		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		sbr  @2, (1<<@1)
		sts  @0, @2
	.elif @0>0x1F
		in   @2, @0
		sbr  @2, (1<<@1)
		out  @0, @2
	.else
		sbi  @0, @1
	.endif
.ENDMACRO

.MACRO CLRB 		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		cbr  @2, (1<<@1)
		sts  @0, @2
	.elif @0>0x1F
		in   @2, @0
		cbr  @2, (1<<@1)
		out  @0, @2
	.else
		cbi  @0, @1
	.endif
.ENDMACRO

;*********************************************************
;*	Bit test anywhere in IO or in lower $FF of data space
;*  SKBS : SKip if Bit Set
;*  SKBC : SKip if Bit Cleared
;*********************************************************
.MACRO SKBS  		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		sbrs @2, @1
	.elif @0>0x1F
		in   @2, @0
		sbrs @2, @1
	.else
		sbis @0, @1
	.endif
.ENDMACRO

.MACRO SKBC  		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds	 @2, @0
		sbrc @2, @1
	.elif @0>0x1F
		in	 @2, @0
		sbrc @2, @1
	.else
		sbic @0, @1
	.endif
.ENDMACRO

;*********************************************************
;*	Byte access anywhere in IO or lower $FF of data space
;* 	STORE - Store register in IO or data space
;* 	LOAD  - Load register from IO or data space
;*********************************************************

.MACRO STORE 		;Arguments: Address, Register
	.if	@0>0x3F
		sts	@0, @1
	.else
		out	@0, @1
	.endif
.ENDMACRO

.MACRO STOREI	;Arguments: Address, Immediate_Value
	ldi	r16,@1
	.if	@0>0x3F
		sts	@0, r16
	.else
		out	@0, r16
	.endif
.ENDMACRO

.MACRO STORE16 		;Arguments: Address, RegH, RegL
	.if	@0>0x3F
		sts	@0+1,@1
		sts	@0,@2
	.else
		out	@0+1,@1
		out	@0,@2
	.endif
.ENDMACRO

.MACRO STOREI16 		;Arguments: Address (H/L), Immediate, temp_reg
	.if	@0>0x3F
		ldi	@2,LOW(@1)
		sts	@0+1, @2
		ldi	@2,HIGH(@1)
		sts	@0, @2
	.else
		ldi	@2,LOW(@1)
		out	@0, @2
		ldi	@2,HIGH(@1)
		out	@0, @2
	.endif
.ENDMACRO

.MACRO LOAD 		;Arguments: Register, Address
	.if	@1>0x3F
		lds	@0,@1
	.else
		in	@0,@1
	.endif
.ENDMACRO

.MACRO LOAD16 		;Arguments: RegH,RegL, Address
	.if	@2>0x3F
		lds	@1, @2
		lds	@0, @2+1
	.else
		in	@1, @2
		in	@0, @2+1
	.endif
.ENDMACRO

.MACRO LOADI16 		;Arguments: RegH, RegL, Immediate
		ldi	@0, HIGH(@2)
		ldi	@1, LOW(@2)
.ENDMACRO

;*********************************************************
;*	16-bit IO Register Operations
;*		OUTW	io_adr, reg_high, reg_low
;*		INW		reg_high, reg_low, io_adr
;*********************************************************
;
.MACRO OUTW
	out @0, @1
	out @0-1, @2
.ENDMACRO

.MACRO	INW
	in @1, @2-1
	in @0, @2
.ENDMACRO

;*********************************************************
;*	16-bit Pointer Register load
;*********************************************************

.MACRO LDPTR 		;Arguments: Register, Data
	ldi		@0H, HIGH(2*@1)			;set rH to high byte 
	ldi		@0L, LOW(2*@1)			;set rL to low byte 
.ENDMACRO
;
;****************************************************************************/
;MACRO TWI_Start: used to initiate a START condition and start the TWI 
;					state machine.
;****************************************************************************/
.MACRO TWI_START
	ldi		Wreg1,((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE))
	STORE	TWCR,Wreg1					;Set START condition
.ENDMACRO
;
