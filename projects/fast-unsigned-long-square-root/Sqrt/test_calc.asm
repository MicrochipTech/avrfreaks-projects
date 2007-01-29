.nolist
.include "m88def.inc"
.list



/********************* variables RAM *****************************/
	.dseg

carre:		.byte	4	//utilisé pour le calcul


/*************** Programme : vecteurs d'interruptions ************/
	.cseg

/**************** INTERRUPT VECTORS ******************************/
	.org	0X0000
	rjmp RESET
	.org	INT0addr	// External Interrupt Request 0
	rjmp default_int
	.org	INT1addr	// External Interrupt Request 1
	rjmp default_int
	.org	PCI0addr	// Pin Change Interrupt Request 0
	rjmp default_int
	.org	PCI1addr	// Pin Change Interrupt Request 0
	rjmp default_int
	.org	PCI2addr	// Pin Change Interrupt Request 1
	rjmp default_int
	.org	WDTaddr		// Watchdog Time-out Interrupt
	rjmp default_int
	.org	OC2Aaddr	// Timer/Counter2 Compare Match A
	rjmp default_int
	.org	OC2Baddr	// Timer/Counter2 Compare Match A
	rjmp default_int
	.org	OVF2addr	// Timer/Counter2 Overflow
	rjmp default_int
	.org	ICP1addr	// Timer/Counter1 Capture Event
	rjmp default_int
	.org	OC1Aaddr	// Timer/Counter1 Compare Match A
	rjmp default_int
	.org	OC1Baddr	// Timer/Counter1 Compare Match B
	rjmp default_int
	.org	OVF1addr	// Timer/Counter1 Overflow
	rjmp default_int
	.org	OC0Aaddr	// TimerCounter0 Compare Match A
	rjmp default_int
	.org	OC0Baddr	// TimerCounter0 Compare Match B
	rjmp default_int
	.org	OVF0addr	// Timer/Couner0 Overflow
	rjmp default_int
	.org	SPIaddr		// SPI Serial Transfer Complete
	rjmp default_int
	.org	URXCaddr	// USART Rx Complete
	rjmp default_int
	.org	UDREaddr	// USART, Data Register Empty
	rjmp default_int
	.org	UTXCaddr	// USART Tx Complete
	rjmp default_int
	.org	ADCCaddr	// ADC Conversion Complete
	rjmp default_int
	.org	ERDYaddr	// EEPROM Ready
	rjmp default_int
	.org	ACIaddr		// Analog Comparator
	rjmp default_int
	.org	TWIaddr		// Two-wire Serial Interface
	rjmp default_int
	.org	SPMRaddr	// Store Program Memory Read
	rjmp default_int

//.equ	INT_VECTORS_SIZE	= 26	// size in words	.org 0x0000		// Reset Handler

default_int:
	reti
/******************* valeurs de test ***********************/
v_0:		.dd		0x00000000		//   27 cycles
v_1:		.dd		0x00000001		//  384 cycles
v_10000:	.dd		0x00010000		//  553 cycles
v_20000:	.dd		0x00020000		//  952 cycles
v_1000000:	.dd		0x01000000		//  653 cycles
v_2000000:	.dd		0x02000000		// 1746 cycles
v_12345678:	.dd		0x12345678		// 1961 cycles; mauvais arrondi
v_FFFFFFFF:	.dd 	0xFFFFFFFF		// 3449 cycles
v_FFFEFFFF:	.dd 	0xFFFEFFFF		// 2774 cycles
v_87654321:	.dd 	0x87654321		// 2032 cycles
v_FEDCBA98:	.dd 	0xFEDCBA98		// 2725 cycles
/******************* utilitaires de calcul *****************/
.include "sqrt.asm"
/***********************************************************/
load_value:
	lpm r0,Z+
	sts carre+0,r0
	lpm r0,Z+
	sts carre+1,r0
	lpm r0,Z+
	sts carre+2,r0
	lpm r0,Z+
	sts carre+3,r0
	ret

/******************* Programme Principal *******************/

RESET:

/******************* Initialisations ***********************/

/************ init stack **********************/

	ldi	zl,low(ramend)	//init stack pointer
	out	SPL,zl
	ldi	zl,high(ramend)	//init stack pointer
	out	SPH,zl

			// etc.... 

/************* tests ***************************************/

	ldi zl,low(v_0<<1)
	ldi zh,high(v_0<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_1<<1)
	ldi zh,high(v_1<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_10000<<1)
	ldi zh,high(v_10000<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_20000<<1)
	ldi zh,high(v_20000<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_1000000<<1)
	ldi zh,high(v_1000000<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_2000000<<1)
	ldi zh,high(v_2000000<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_12345678<<1)
	ldi zh,high(v_12345678<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_FFFFFFFF<<1)
	ldi zh,high(v_FFFFFFFF<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_FFFEFFFF<<1)
	ldi zh,high(v_FFFEFFFF<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_87654321<<1)
	ldi zh,high(v_87654321<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt

	ldi zl,low(v_FEDCBA98<<1)
	ldi zh,high(v_FEDCBA98<<1)
	rcall load_value
	ldi zl,low(carre)
	ldi zh,high(carre)
	rcall sqrt


toto:
	rjmp toto
