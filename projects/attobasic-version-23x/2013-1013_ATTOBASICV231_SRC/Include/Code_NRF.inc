.if NRF
;****************************************************************************
; RFIcommand: Initialize the nRF24L01
;****************************************************************************
RFIcommand:
	GoSub	SPIinit				;init the SPI interface
	SETB	NRFPORT-1,NRFCE		;set CE pin to output
	CLRB	NRFPORT,NRFCE		;clear CE pin

	ldi		U,1					;preset mode 0 (+1)
	GoSub	pushU				;save on dstack
	GoSub	pushU				;save on dstack
	GoSub	pushU				;save on dstack

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	breq	RFIcommand4			;0 parameters on dstack, process as 3
	cpi		cmdparms,1			;is there 1 parameter on dstack?
	breq	RFIcommand1			;1 parameters on dstack, process
	cpi		cmdparms,3			;are there 3 parameters on dstack?
	breq	RFIcommand4			;3 parameters on dstack, process
	rjmp	NRFerr1D			;wrong number of arguments on dstack

RFIcommand1:	;1 parameter on the stack
	GoSub	popU				;fetch the mode
	tst		U					;U = 0?
	breq	RFIcommand3			;yes, branch
	cpi		U,3					;U = 3?
	breq	RFIcommand2			;yes, branch
	rjmp	NRFerr16			;error, invalid range

RFIcommand2: ;U = 3, so process set the nRF24L01 register defaults
	SetZPtr	(2*RFIdeftbl)		;point Z to Register default table  in PROGMEM

RFIcommand2a:
	lpm		U,Z+				;fetch the register to U
	lpm		bytecntr,Z+			;fetch the register's depth to bytecntr
	
	tst		bytecntr			;end of table?
	breq	RFIcommand5			;yes, exit

	ori		U,NRF24_CMD_W_REGISTER;command write to register
	GoSub	SPI_write			;write the data

RFIcommand2b:
	lpm		U,Z+				;read data
	GoSub	SPI_write			;write the data
	dec		bytecntr			;decrement byte counter
	brne	RFIcommand2b		;loop till zero
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	adiw	ZH:ZL,1				;increment Z to next table element
	rjmp	RFIcommand2a		;loop for next register

RFIcommand3:	;U = 0, config as TX w/ default values	
	ldi		U,( NRF24_R00_CFG | \
				NRF24_CMD_W_REGISTER);write reg 0
	GoSub	SPI_write			;write the data

	ldi		U,NRF24_REG_00_DEF	;enter standby mode
	rcall	NRFwr				;write data, end command

;	CLRB	GPIOR2,NRF_EN,temp	;clear NRF flag
	rjmp	NRFstat				;fetch status and exit
;
RFIcommand4:	;U = 1 or 2, set TX or RX mode
	;set the data rate
	ldi		U,(	NRF24_R06_RF_SETUP | \
				NRF24_CMD_W_REGISTER);write reg 6
	GoSub	SPI_write			;write the data
	GoSub	popU				;fetch the bitrate
	swap	U					;shift to proper bit position
	lsr		U
	ori		U,NRF24_PWR_0dBm	;add in TX PA bits
	rcall	NRFwr				;write data, emd command

	;set the channel
	ldi		U,( NRF24_R05_RF_CH | \
				NRF24_CMD_W_REGISTER);write reg 5
	GoSub	SPI_write			;write the data
	GoSub	popU				;fetch the channel
	andi	U,NRF24_RF_CH		;mask channel [0..127]
	rcall	NRFwr				;write data, end command

	;set the transceiver's mode
	ldi		U,( NRF24_R00_CFG | \
				NRF24_CMD_W_REGISTER);write reg 0
	GoSub	SPI_write			;write the data
	GoSub	popU				;fetch the mode
	dec		U					;subtract 1 for mode inclusive
	ori		U,( NRF24_REG_00_DEF	| \
				NRF24_PWR_UP )	;add in Reg00 default bits
	rcall	NRFwr				;write data, end command

;	SETB	GPIOR2,NRF_EN,temp	;set NRF flag

RFIcommand5:
	clr		U					;preset to "0"
	GoSub	pushU				;save on dstack
	rjmp	RFFcommand1			;clear FIFO and flags
;
RFIdeftbl:	;factory defaults table; reg, depth, data
	.db		0x00,1,NRF24_REG_00_DEF,0
	.db		0x01,1,NRF24_REG_01_DEF,0
	.db		0x02,1,NRF24_REG_02_DEF,0
	.db		0x03,1,NRF24_REG_03_DEF,0
	.db		0x04,1,NRF24_REG_04_DEF,0
	.db		0x05,1,NRF24_REG_05_DEF,0
	.db		0x06,1,NRF24_REG_06_DEF,0
	.db		0x07,1,(NRF24_RX_DR	| \
					NRF24_TX_DS	| \
					NRF24_MAX_RT ),0
	.db		0x0A,5,RX_ADDR_P0,RX_ADDR_P0,RX_ADDR_P0,RX_ADDR_P0,RX_ADDR_P0,0
	.db		0x0B,5,RX_ADDR_P1,RX_ADDR_P1,RX_ADDR_P1,RX_ADDR_P1,RX_ADDR_P1,0
	.db		0x0C,1,RX_ADDR_P2,0
	.db		0x0D,1,RX_ADDR_P3,0
	.db		0x0E,1,RX_ADDR_P4,0
	.db		0x0F,1,RX_ADDR_P5,0
	.db		0x10,5,TX_ADDR,TX_ADDR,TX_ADDR,TX_ADDR,TX_ADDR,0
	.db		0x11,1,NRF24_R11_RX_PW_P0_DEF,0
	.db		0x12,1,NRF24_R12_RX_PW_P1_DEF,0
	.db		0x13,1,NRF24_R13_RX_PW_P2_DEF,0
	.db		0x14,1,NRF24_R14_RX_PW_P3_DEF,0
	.db		0x15,1,NRF24_R15_RX_PW_P4_DEF,0
	.db		0x16,1,NRF24_R16_RX_PW_P5_DEF,0
	.db		0x1C,1,NRF24_REG_1C_DEF,0
	.db		0x1d,1,NRF24_REG_1D_DEF,0
	.dw		0					;end of table
;
;****************************************************************************
; RFXcommand: Transfer the nRF24L01's RX buffer to the DATA statement buffer
;****************************************************************************
RFXcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	;fetch the status to see if RX data available
	GoSub	SPI_read			;fetch status register
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command

	sbrc	U,LOG2(NRF24_RX_DR)	;skip next if RX_DR bit is clear
	rjmp	RFXcommand1			;jump if data available
	ser		U					;U=255
	GoSub	pushU				;save on dstack
	ret							;return to caller

RFXcommand1:	;check for command line parameters
	andi	U,NRF24_RX_P_NO		;mask RX_P_NO bits
	lsr		U					;right justify to get pipe number
	mov		H,U					;save a copy in H

	;fetch the pipe's payload size
	inc		U					;increment U for proper register address
	ori		U,NRF24_R10_TX_ADDR	;read reg 0x1[p]
	rcall	NRFwrrd				;write data, read data, end command
	mov		bytecntr,U			;move the payload size to T
	clr		r1					;clear a register
	PUSHY						;save Y
	SetYPtr	(READSTOR+READSTKSZ);Y = top of DATA buffer
	STORE	DATAptr,r1			;clear DATA read pointer
	STORE	DATAptr+1,U			;save payload size as # of DATA elements

	CLRB	NRFPORT,NRFCE		;insure CE pin is low
	ldi		U,NRF24_CMD_R_RX_PAYLOAD;read RX payload
	GoSub	SPI_write			;write the data

RFXcommand3:
	GoSub	SPI_read			;read the data
	st		-Y,U				;store in DATA space
	dec		bytecntr			;decrement payload counter
	brne	RFXcommand3			;loop till zero
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command

	;read the STATUS register
	GoSub	SPI_read			;fetch status register
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	mov		T,U					;save STATUS
	andi	U,NRF24_RX_P_NO		;strip RX_P_NO flags - more FIFO data?
	cpi		U,NRF24_RX_P_NO		;is RX_FIFO empty?
	brne	RFXcommand3a		;no, still more data in the FIFO

	;no more data in the RX FIFO so clear the NRF24_RX_DR bit
	andi	T,NRF24_RX_DR		;strip RX_DR flag and clear it
	ldi		U,( NRF24_R07_STAT | \
				NRF24_CMD_W_REGISTER);write reg 7
	GoSub	SPI_write			;write the data
	mov		U,T					;move the new data to U
	rcall	NRFwr				;write data, end command

RFXcommand3a:
	POPY						;restore Y
	mov		U,H					;retrieve the pipe number
	GoSub	pushU				;save the pipe number on the data stack

	ret							;return to caller
;
;****************************************************************************
; RFTcommand: Transmit data.  The payload size is determined by the
;	number of data elements provided to the command.
;	Uses: 
;****************************************************************************
RFTcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	push	DStkPtr				;save current dstkptr value
	push	DStkPtr				;save current dstkptr value again
	GoSub	ChkCmdParms			;interpret line, tst for zero parms

	;insure that TX FIFO is not clogged with MAX_RT bit set
	GoSub	SPI_read			;fetch the STATUS register
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command

	sbrs	U,LOG2(NRF24_MAX_RT);is MAX_RT bit set?
	rjmp	RFTcommand1			;is okay, continue

	ldi		U,( NRF24_R07_STAT | \
				NRF24_CMD_W_REGISTER);write reg 7
	GoSub	SPI_write			;write the data
	ldi		U,NRF24_MAX_RT		;clear the U,NRF24_MAX_RT bit
	rcall	NRFwr				;write data, end command

RFTcommand1:	;FIFO enabled, check for parms
	CLRB	NRFPORT,NRFCE		;clear CE pin
	tst		cmdparms			;check for no parameters supplied
	breq	RFTcommand3			;jump to pulse the CE pin

RFTcommand2:	;pop the data off the stack and write it to the nRF24L01.
	;must bypass "popU" routine to send MSB to LSB
	mov		bytecntr,cmdparms	;set up loop counter
	clr		r1					;clear a register
	pop		r0					;restore DStkPtr into r0
	PUSHZ						;save Z-Pointer
	SetZPtr	(DATASTK+DATASTKSZ)	;point Z to TOS
	SUB16	ZH,ZL,r1,r0			;adjust for new parameters added
	ldi		U,NRF24_CMD_W_TX_PAYLOAD;sending TX data
	GoSub	SPI_write			;write the data

RFTcommand2a:
	ld		U,-Z				;fetch data, post dec Z
	GoSub	SPI_write			;write the data
	dec		bytecntr			;decrement loop counter
	brne	RFTcommand2a		;loop till zero
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command

	POPZ						;restore Z pointer

RFTcommand3:	;pulse the CE pin to initiate TX 
	SETB	NRFPORT,NRFCE		;set CE pin
	GoSub	Delay10uS			;delay 20uS
	GoSub	Delay10uS			
	CLRB	NRFPORT,NRFCE		;clear CE pin

	ldi		U,NRFARDLY			;delay to allow for RX to ACK
RFTcommand3a:
	GoSub	Delay1mS			;delay 1mS
	dec		U					;decrement counter
	brne	RFTcommand3a		;loop till zero
;
RFTcommand4:	;fetch the STATUS register
	GoSub	SPI_read			;fetch the STATUS register
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	sbrs	U,LOG2(NRF24_MAX_RT);is MAX_RT bit set?
	rjmp	RFTcommand4a		;good xmit, continue

	ldi		U,3					;U=3
	rjmp	RFTcommand4b		;save on dstack and return
;
RFTcommand4a:	;return the status of the FIFO register
	ldi		U,NRF24_R17_FIFO_STAT;read reg 0x17
	rcall	NRFwrrd				;write data, read data, end command

	andi	U,( NRF24_TX_FULL | \
				NRF24_TX_EMPTY );strip off TX FIFO flags
	swap	U					;swap nibbles
;
RFTcommand4b:	;return the status of the FIFO register
	pop		DStkPtr				;restore previous dstkptr value
	GoSub	pushU				;save on dstack
	ret							;return to caller
;
;****************************************************************************
; RFAcommand: Set the nRF24L01 addresses
; Uses: H, T, U, temp, inbyte, r1, r0, Z and Y
;****************************************************************************
RFAcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled
	PUSHZ						;save Z-Pointer
	PUSHY						;save Y-Pointer

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFAcommand1			;parameters on dstack, continue
	rjmp	NRFerr1D			;no parameters on dstack, error

RFAcommand1:	;parse the parameters, must bypass the "popU" routine
	mov		T,cmdparms			;copy in T
	dec		T					;adjust parameter count

	;fetch the pipe number from the datastack
	SetYPtr	(DATASTK+DATASTKSZ)	;point Y to TOS in RAM
	ld		inchar,-Y			;fetch pipe #

	;fetch the max address width for the selected pipe
	clr		r1					;clear register
	SetZPtr	(2*RFApipetbl)		;point Z to MAC LUT in PROGMEM
	mov		r0,inchar			;copy pipe # in r0
	lsl		r0					;multiply by 2
	ADD16	ZH,ZL,r1,r0			;add as index to Z
	lpm		r0,Z+				;fetch min MAC size in r0
	lpm		r1,Z				;fetch max MAC size in r1
	tst		r0					;is it a good pipe number?
	brne	RFAcommand1a		;yes, continue
	rjmp	NRFerr16			;no, error!

RFAcommand1a:
	;fetch the current address width from the nRF24L01
	ldi		U,NRF24_R03_SETUP_AW;read reg 03
	rcall	NRFwrrd				;write data, read data, end command
	subi	U,-2				;+ 2 for actual size
	mov		H,U					;make a copy in H

	cp		r0,r1				;min = max ?
	brne	RFAcommand1b		;no, branch
	mov		H,r0				;must be RX[2:5] @ 1 byte so set current

RFAcommand1b:	;check supplied width against device max width
	cp		r1,T				;params greater than max width?
	brmi	RFAcommand9			;too many params, error
	cp		T,r0				;params less than min width?
	brmi	RFAcommand9			;too few params, error
	cp		T,H					;params = current width?
	brne	RFAcommand9			;no, error

	;proper number of parameters included, start the MAC programming
	ldi		U,( NRF24_R10_TX_ADDR | \
				NRF24_CMD_W_REGISTER );preload for TX pipe register
	cpi		inchar,7			;is it the TX pipe?
	breq	RFAcommand2			;yes, branch

	mov		U,inchar			;copy desired pipe to U
	subi	U,-(NRF24_R0A_RX_ADDR_P0);add register offset
	ori		U,NRF24_CMD_W_REGISTER;register write command

RFAcommand2:
	GoSub	SPI_write			;write to register command
RFAcommand2a:
	GoSub	popU				;fetch MAC byte (LSB to MSB)
	GoSub	SPI_write			;write the data
	dec		T					;decrement needed parameter count
	brne	RFAcommand2a		;loop till finished

	SETB	SPI_PORT,SPI_SS		;raise SS line to end command

	POPY						;restore Y-Pointer
	POPZ						;restore Z-Pointer

	rjmp	NRFstat				;fetch status and exit
;
RFAcommand9:	;too few
	rjmp	NRFerr1D			;error code "$1D"
;
RFApipetbl:	; min and max MAC sizes per pipe
	.db		3,5					;RX0
	.db		3,5					;RX1
	.db		1,1					;RX2
	.db		1,1					;RX3
	.db		1,1					;RX4
	.db		1,1					;RX5
	.db		0,0					;invalid
	.db		3,5					;TX
;
;.include	"Include/Data_NRF_MAC.inc"	;include the MAC address table
;
;****************************************************************************
; RFBcommand: Set the nRF24L01's RX pipe payload sizes
;	Uses: H,T,Um temp, bytecntr, r1 and Z
;****************************************************************************
RFBcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFBcommand1			;parameters on dstack, continue

	rjmp	NRFstat				;fetch status and exit
;
RFBcommand1:
	cpi		cmdparms,2			;are there 2 parameters on dstack?
	breq	RFBcommand2			;yes, branch

	;one parameter provided, return payload size of pipe 0x11 to 0x16
	rcall	NRFchkpipenmbr		;fetch and check the pipe number in U
	inc		U					;add 1 to pipe #
	ori		U,NRF24_R10_TX_ADDR ;read reg 0x1[p]
	rcall	NRFwrrd				;write data, read data, end command
	GoSub	pushU				;save value on dstack
	ret							;return to caller
;
RFBcommand2:	;two parameters provided
	rcall	NRFchkpipesz		;fetch and check the payload size in U
	mov		H,U					;save in H

	rcall	NRFchkpipenmbr		;fetch and check the pipe number in U
	mov		inchar,U			;copy pipe number to inchar
	mov		bytecntr,U			;copy pipe number to bytecntr register
	inc		bytecntr			;adjust for bit-inclusive
	clr		T					;clear T register
	sec							;set carry bit

RFBcommand2a:
	rol		T					;shift carry bit into T
	dec		bytecntr			;decrement pipe bit counter
	brne	RFBcommand2a		;shift bit till zero
	
	;fetch the current register value
	ldi		U,NRF24_R02_EN_RXADDR;read reg 02
	rcall	NRFwrrd				;write data, read data, end command

	tst		H					;is buffer request 0?
	breq	RFBcommand2c		;yes, clear bit

RFBcommand2b:	;enable the pipe
	or		T,U					;set the pipe enable bit
	rjmp	RFBcommand2d		;write the pipe enable bits

RFBcommand2c:	;disable pipe
	com		T					;invert the bits
	andi	T,NRF24_ERX_MASK	;mask the proper bits
	and		T,U					;set the pipe enable bit

RFBcommand2d:	;write the pipe enable register
	ldi		U,( NRF24_R02_EN_RXADDR | \
				NRF24_CMD_W_REGISTER );write reg 02
	GoSub	SPI_write			;write the data
	mov		U,T					;move bit mask to U
	rcall	NRFwr				;write data, end command

RFBcommand3:	;write the pipe payload size
	mov		U,inchar			;restore the pipe number
	inc		U					;add 1 to pipe #
	ori		U,( NRF24_R10_TX_ADDR | \
				NRF24_CMD_W_REGISTER );write reg 0x1[p]
	GoSub	SPI_write			;write the data
	mov		U,H					;move the payload size to U
	rcall	NRFwr				;write data, end command
	rjmp	NRFstat				;fetch status and exit

RFBcommand9:
	rjmp	NRFerr16			;error code "$16"
;
;****************************************************************************
; RFEcommand: Enable/disable the nRF24L01 CE pin
;****************************************************************************
RFEcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled
	clr		U					;default CE off
	GoSub	PushU				;save on dstack
	GoSub	Get1stParm			;get the last parameter
	ldi		temp,2				;only values of [0..2] allowed
	cp		temp,U
	brmi	RFEcommand9			;branch if allowable exceeded 
	sbrc	U,1					;skip next if U < 1
	rjmp	RFEcommand1			;U < 1
	sbrs	U,0					;skip next if U = 1
	rjmp	RFEcommand2			;U = 0

RFEcommand1:
	SETB	NRFPORT,NRFCE		;set CE pin
	sbrs	U,1					;skip next if U > 1
	rjmp	NRFstat				;fetch status and exit
	GoSub	Delay10uS			;delay 20uS
	GoSub	Delay10uS			
	;fall through to clear port pin

RFEcommand2:
	CLRB	NRFPORT,NRFCE		;clear CE pin
	rjmp	NRFstat				;fetch status and exit
;;
RFEcommand9:
	rjmp	NRFerr16			;error code "16"
;
;****************************************************************************
; RFCcommand: Set the nRF24L01's TX/RX channel or returns the current channel
;****************************************************************************
RFCcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFCcommand1			;parameters on dstack, set channel

	;fetch RF channel register 
	ldi		U,NRF24_R05_RF_CH	;RF channel register
	GoSub	SPI_write			;write the data
	rjmp	NRFstat				;fetch the data then exit
;;
RFCcommand1:
	Gosub	popU				;fetch desrired channel
	sbrc	U,7					;skip next if U > 127
	rjmp	NRFerr16			;error, allowable exceeded 
	push	U					;save on stack

	;set the transceiver's channel
	ldi		U,( NRF24_R05_RF_CH | \
				NRF24_CMD_W_REGISTER);write reg 5
	GoSub	SPI_write			;write the data
	pop		U					;restore channel from stack
	rcall	NRFwr				;write data, end command
	rjmp	NRFstat				;fetch status and exit
;
;****************************************************************************
; RFPcommand: set the nRF24L01's TX PA
;****************************************************************************
RFPcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFPcommand1			;parameters on dstack, set TX PA level

	;fetch current RF setup
	ldi		U,NRF24_R06_RF_SETUP;read reg 6
	rcall	NRFwrrd				;write data, read data, end command
	andi	U,NRF24_PWR			;strip the PA bits
	lsr		U					;shift right 1 bit
	GoSub	pushU				;save on dstack
	ret							;return to caller
;
RFPcommand1:	;Set the TX PA level
	GoSub	popU				;fetch desired value from dstack
	ldi		temp,3				;only values of [0..3] allowed
	cp		U,temp
	brmi	RFPcommand2			;branch if okay
	rjmp	NRFerr0D			;error

RFPcommand2:
	lsl		U					;shift U 1 bit position
	mov		T,U					;move to T				

	;fetch current RF setup
	ldi		U,NRF24_R06_RF_SETUP;read reg 6
	rcall	NRFwrrd				;write data, read data, end command

	andi	U,~(NRF24_PWR)		;strip TX PA bits
	or		T,U					;mask the TX PA bits, leave in T

	;set the RF TX PA level
	ldi		U,( NRF24_R06_RF_SETUP | \
				NRF24_CMD_W_REGISTER);write reg 6
	GoSub	SPI_write			;write the data
	mov		U,T					;move new value to U
	rcall	NRFwr				;write data, end command
	rjmp	NRFstat				;fetch status and exit
;
;****************************************************************************
; RFDcommand: set or return the nRF24L01's data rate
;****************************************************************************
RFDcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFDcommand1			;parameters on dstack, set data rate

	;fetch current data rate
	ldi		U,NRF24_R06_RF_SETUP;read reg 6
	rcall	NRFwrrd				;write data, read data, end command
	andi	U,NRF24_RF_DR_HIGH	;mask the data rate bit
	lsr		U					;shift right 3 bit positions
	lsr		U
	lsr		U
	GoSub	pushU				;save on dstack
	ret							;return to caller
;
RFDcommand1:	;Set the data rate
	GoSub	popU				;fetch desired value from dstack
	ldi		temp,1				;only values of [0..1] allowed
	cp		U,temp
	brmi	RFDcommand2			;branch if okay
	rjmp	NRFerr0C			;error

RFDcommand2:
	lsl		U					;shift left 3 bit positions
	lsl		U
	lsl		U
	mov		T,U					;move to T				

	;fetch current RF setup
	ldi		U,NRF24_R06_RF_SETUP;read reg 6
	rcall	NRFwrrd				;write data, read data, end command

	andi	U,~(NRF24_RF_DR_HIGH);strip the data rate bit
	or		T,U					;mask the data rate bits, leave in T

	;set the data rate
	ldi		U,( NRF24_R06_RF_SETUP | \
				NRF24_CMD_W_REGISTER);write reg 6
	GoSub	SPI_write			;write the data
	mov		U,T					;move new value to U
	rcall	NRFwr				;write data, end command
	rjmp	NRFstat				;fetch status and exit
;
;****************************************************************************
; RFWcommand: Write data to a register in the nRF24L01
;****************************************************************************
RFWcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled

	GoSub	ChkCmdParms			;interpret line, tst for zero pars
	brne	RFWcommand1			;parameters on dstack, continue

	STOREI	ERRCode,$1D			;no parameters, error code "0x1D"
	Goto	error

RFWcommand1:
	GoSub	popU				;fetch the value to write
	mov		T,U					;copy in T

	GoSub	popU				;fetch the register to write to
	ldi		temp,NRF24_R1D_FEAT	;max register allowed
	cp		temp,U
	brmi	RFWcommand9			;branch if allowable exceeded 

	ori		U,NRF24_CMD_W_REGISTER;write reg 0xRR
	GoSub	SPI_write			;write the data
	mov		U,T					;move new value to U
	rcall	NRFwr				;write data, end command
	ret
;
RFWcommand9:
	STOREI	ERRCode,1			;no parameters, error code "1"
	Goto	error
;
;****************************************************************************
; RFRcommand: Fetch the value of a nRF24L01 registers.
;	Wihtout any parameters returns FIFO status
;****************************************************************************
RFRcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled
	ldi		U,0x17				;default to register 0x17
	Gosub	pushU				;save it on the stack
	GoSub	Get1stParm			;get the last parameter
	ldi		temp,NRF24_R1D_FEAT	;max register allowed
	cp		U,temp
	brmi	RFRcommand1			;branch if good 
	rjmp	NRFerr0C			;error!

RFRcommand1:	;fetch register 
	GoSub	SPI_write			;write the data
	rjmp	NRFstat				;fetch the data then exit
;
;****************************************************************************
; RFFcommand: Flush the nRF24L01's TX/RX buffers
;****************************************************************************
RFFcommand:
	GoSub	ChkSPIEn			;be sure SPI interface is enabled
	clr		U					;preset to "0"
	GoSub	pushU				;save on dstack

	GoSub	Get1stParm			;get the last parameter
	mov		H,U					;save a copy in H
	tst		U					;U = 0?
	brne	RFFcommand1			;no, branch

	;default "0" means clear both FIFO's
	ldi		U,NRF24_CMD_FLUSH_TX;flush TX buffer
	rcall	NRFwr				;write data, end command

	ldi		U,NRF24_CMD_FLUSH_RX;flush RX buffer
	rcall	NRFwr				;write data, end command
	
	ldi		H,0x3				;set to clear both flags
	rjmp	RFFcommand3

RFFcommand1:	;check for a single parameter
	ldi		temp,2				;max allowed is "2"
	cp		temp,U
	brpl	RFFcommand2			;branch if good 
	rjmp	NRFerr16			;error

RFFcommand2:	;process the parameter
	ldi		U,(NRF24_CMD_FLUSH_TX & 0xF0);load base command
	or		U,H					;add in the TX or RX select bit
	rcall	NRFwr				;write data, end command

RFFcommand3:	;clear the STATUS flags
	ldi		U,( NRF24_R07_STAT );read the status register
	rcall	NRFwrrd				;write data, read data, end command

	mov		T,U					;save STATUS in T

	;manipulate the flags and write the STATUS register
	ldi		U,( NRF24_R07_STAT | \
				NRF24_CMD_W_REGISTER);write reg 7
	GoSub	SPI_write			;write the data
	sbrc	H,bit0				;skip next if not TX
	ori		T,( NRF24_TX_DS | \
				NRF24_MAX_RT )	;clear TX flag bits
	sbrc	H,bit1				;skip next if not RX
	ori		T,( NRF24_RX_DR )	;clear RX flag bits
	mov		U,T					;copy to U
	rcall	NRFwr				;write data, end command
	rjmp	NRFstat				;fetch status and exit
;
;****************************************************************************
; NRFstat: Fetches the status register and returns to caller.
;****************************************************************************
NRFstat:
	GoSub	SPI_read			;fetch status register
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	GoSub	PushU				;save on dstack
	ret							;return to caller
;
;****************************************************************************
; NRFwr:	Write data in U, raise SS line to end command
;****************************************************************************
NRFwr:
	GoSub	SPI_write			;write the data
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	ret
;
;****************************************************************************
; NRFrd:	Read data into U, raise SS line to end command
;****************************************************************************
;NRFrd:
;	GoSub	SPI_read			;write the data
;	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
;	ret
;
;****************************************************************************
; NRFwrrd:	Write data in U, return data read in U, 
;	raise SS line to end command
;****************************************************************************
NRFwrrd:
	GoSub	SPI_write			;write the data
	GoSub	SPI_read			;write the data
	SETB	SPI_PORT,SPI_SS		;raise SS line to end command
	ret
;
;****************************************************************************
; NRFchkparms: Zeros cmdparm register, calls line interpreter and tests
;	for zero parameters passed.
;****************************************************************************
;NRFchkparms:
;	clr		cmdparms			;clear relative counter
;	GoSub	interpretlinev		;process line parameters
;	tst		cmdparms			;any new data on stack?
;	ret	
;
;****************************************************************************
; NRFNRFchkpipenmbr: Checks that the pipe number held in U is valid.  If not,
;	an error is issued.
;****************************************************************************
NRFchkpipenmbr:		;fetch and check the pipe number
	GoSub	popU				;fetch the pipe number from dstack
	ldi		temp,( NRF24_R16_RX_PW_P5 - \
				   NRF24_R11_RX_PW_P0 );only 0..5 for pipe number
	cp		temp,U
	brmi	NRFerr16			;to high, error!
	ret							;return to caller

;****************************************************************************
; NRFNRFchkpipesz: Checks that the size of the pipe specified is not greater 
;	that the maximum size allowed. If not, an error is issued.
;****************************************************************************
NRFchkpipesz:		;fetch and check the buffer size
	GoSub	popU				;fetch the buffer size from the dstack
	ldi		temp,NRF24_MAX_MSG_LEN;only MAX_MSG_LEN allowed
	cp		temp,U
	brmi	NRFerr16			;to high, error!
	ret							;return to caller
;
;****************************************************************************
; Common Errors
;****************************************************************************
NRFerr0C:	;Exceeded range of 0:1
	STOREI	ERRCode,$0C			;error code "0x0C"
	Goto	error
;
NRFerr0D:	;Exceeded range of 0:3
	STOREI	ERRCode,$0D			;error code "0X0D"
	Goto	error
;
NRFerr16:	;Specified value exceeded range
	STOREI	ERRCode,$16			;error code "0x16"
	Goto	error
;
NRFerr1D:	;wrong number of arguments
	STOREI	ERRCode,$1D			;error code "$1D"
	Goto	error
;
.endif
