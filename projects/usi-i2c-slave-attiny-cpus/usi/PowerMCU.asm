;****************************************************************************
;****************************************************************************
;* I2C slave CPU							 c2012 by Vladislav Shapovalov	*
;* CPU:   ATtiny2313      					   								*
;* Clock: INT 8 MHz			    	              		 	3dfilm@mail.ru  *
;!!!!!!!!!!!!!   SCL DDR and PORT are ALWAYS =1	 !!!!!!!!!!!!!!!!!			*
;!!!!!!!!!!!!!   SDA PORT are ALWAYS =1 		 !!!!!!!!!!!!!!!!!			*
;****************************************************************************
;****************************************************************************

.include "tn2313def.inc"
.include "avr.inc"

.equ MySlaveAddress	= 0xAC

.equ DDR_I2C	= DDRB
.equ PORT_I2C	= PORTB
.equ PIN_I2C	= PINB
.equ SCL		= 7				;!!!!!!!!!!!!!SCL DDR and PORT are ALWAYS =1	 !!!!!!!!!!!!!!!!!
.equ SDA 		= 5				;!!!!!!!!!!!!!SDA PORT are ALWAYS =1 			!!!!!!!!!!!!!!!!!

.equ	Flag			= GPIOR1
.equ	I2Cadr			= 7
.equ	I2Crw			= 6			;Read=1
.equ	I2CsubAddr		= 5			;WriteSubAddr=1
.equ	I2Cask			= 4

;=========================================================;
.dseg
.org	SRAM_START					;RAMTOP
I2CsubAddrBuf:	 	.byte 1

;****************************************************************************************
;----------------------------------------------------------;
.cseg
.ORG 0
	RJMP RESET
.ORG USI_STARTaddr		;USI_STRaddr
	rjmp StartCondition
.ORG USI_OVFaddr
	rjmp I2CdataRX
;----------------------------------------------------------;
StartCondition:
	push AL
	in AL,SREG
	push AL

 	cbi DDR_I2C,SDA
	outi Flag,0

	sbic PIN_I2C,SCL	;Wait for SCL to go low to ensure the "Start Condition" has completed.
	rjmp PC-1

	outi USICR,(1<<USISIE)|(1<<USIOIE)|(1<<USIWM1)|(1<<USIWM0)|(1<<USICS1)   
   	outi USISR,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)
  
	pop	AL
	out	SREG,AL
	pop AL
	reti
;----------------------------------------------------------;
I2CdataRX:
	pushw Y
	pushw A
	in AL,SREG
	push AL

	in AH,USIDR

	sbis Flag,I2Cadr
	rjmp CheckAddress

	sbis Flag,I2Crw
	rjmp WriteData
;------------------------------------
;ReadData:
	sbis Flag,I2Cask
	rjmp ReadData1

	sbis Flag,I2CsubAddr
	rjmp Ask0
	cpi AH,0
	brne ResetUSI			;NACK
PutData:
	cbi Flag,I2Cask
	lds YL,I2CsubAddrBuf
	clr YH
	addiw Y,SRAM_START
	ld AL,Y+
	out USIDR,AL
	subiw Y,SRAM_START
	sts I2CsubAddrBuf,YL

	sbi DDR_I2C,SDA			;Set SDA as output
   	ldi AL,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)
	rjmp I2CdataRX_outAL
Ask0:
	sbi Flag,I2CsubAddr
	rjmp PutData
ReadData1:
	sbi Flag,I2Cask
	cbi DDR_I2C,SDA			;Set SDA as input
	outi USIDR,0			;Prepare ACK 
	rjmp I2CdataRX_out7E
;------------------------------------
WriteData:
	sbis Flag,I2Cask
	rjmp WriteData0
	cbi Flag,I2Cask 
 	cbi DDR_I2C,SDA
   	ldi AL,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)
	rjmp I2CdataRX_outAL
ResetUSI:
 	cbi DDR_I2C,SDA
	outi Flag,0
	outi USICR,(1<<USISIE)|(1<<USIWM1)|(0<<USIWM0)|(1<<USICS1); ;Start Condition Interrupt;Set USI in Two-wire mode;Clock Source = External, positive edge        */  \
    ldi AL,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)		;Clear all flags, except Start Cond 
	rjmp I2CdataRX_outAL
WriteData0:
	sbic Flag,I2CsubAddr
	rjmp DoData
;Save SubAddress
	sts I2CsubAddrBuf,AH
	sbi Flag,I2CsubAddr
	sbi Flag,I2Cask
	rjmp Send_ASK
DoData:
	lds YL,I2CsubAddrBuf
	clr YH
	addiw Y,SRAM_START
	st Y+,AH
	subiw Y,SRAM_START
	sts I2CsubAddrBuf,YL
	sbi Flag,I2Cask
	rjmp Send_ASK
;---------------------------------
CheckAddress:
	bst AH,0
	cbr AH,1
	cpi AH,MySlaveAddress
	brne ResetUSI
	ldi AL,(1<<I2Cadr)|(1<<I2Cask)
	bld AL,I2Crw
	out Flag,AL
Send_ASK:
	outi USIDR,0
	sbi DDR_I2C,SDA
I2CdataRX_out7E:
  	ldi AL,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(0x0E<<USICNT0)
I2CdataRX_outAL:
   	out USISR,AL

	pop	AL
	out	SREG,AL
	popw A
	popw Y
	reti
;----------------------------------------------------------;
RESET:
	outi SPL,low(RAMEND)

	outi DDRD, 0xFF
	outi PORTD,0
	outi DDRB, 0b00000000+(1<<SCL)
	outi PORTB,0b00000000+(1<<SCL)+(1<<SDA)			;Port 1=OUTputs

	sbi ACSR,ACD					;disable AnalogCoparator
	
	outi MCUCR,(1<<SE)				;Sleep On///Power-Down Mode

	ldiw X,I2CsubAddrBuf
	ldi AL,1
	st X+,AL
	inc AL
	cpi AL,120
	brlo PC-3


;I2Cinit:
	outi USICR,(1<<USISIE)|(1<<USIWM1)|(0<<USIWM0)|(1<<USICS1)	;| ;Start Condition Interrupt;Set USI in Two-wire mode;Clock Source = External, positive edge        */  \
    outi USISR,(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)	;Clear all flags
	outi Flag,0

	sei

MainLoop:
	sleep
	WDR
	rjmp MainLoop



