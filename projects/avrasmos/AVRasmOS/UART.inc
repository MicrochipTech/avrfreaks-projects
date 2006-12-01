;**********************************************************
; для "кривых" кварцев значения, возможно, будут неверные (ошибка связана с "не округлением")
;	UART definition
.EQU	BOUD_1200		=((SYSTEM_CLOCK/1200)/16)-1 
.EQU	BOUD_2400		=((SYSTEM_CLOCK/2400)/16)-1 
.EQU	BOUD_4800		=((SYSTEM_CLOCK/4800)/16)-1 
.EQU	BOUD_9600		=((SYSTEM_CLOCK/9600)/16)-1 
.EQU	BOUD_14400		=((SYSTEM_CLOCK/14400)/16)-1 
.EQU	BOUD_19200		=((SYSTEM_CLOCK/19200)/16)-1 
.EQU	BOUD_28800		=((SYSTEM_CLOCK/28800)/16)-1 
.EQU	BOUD_33600		=((SYSTEM_CLOCK/33600)/16)-1 
.EQU	BOUD_57600		=((SYSTEM_CLOCK/57600)/16)-1 
.EQU	BOUD_76800		=((SYSTEM_CLOCK/76800)/16)-1 
.EQU	BOUD_115200		=((SYSTEM_CLOCK/115200)/16)-1 
//---------------------------------------------------------
.EQU	TOut_1200		=1+(1000000/SYSTEM_TICK)/((1200/10)/4)  
.EQU	TOut_2400		=1+(1000000/SYSTEM_TICK)/((2400/10)/4) 
.EQU	TOut_4800		=1+(1000000/SYSTEM_TICK)/((4800/10)/4) 
.EQU	TOut_9600		=1+(1000000/SYSTEM_TICK)/((9600/10)/4) 
.EQU	TOut_14400		=1+(1000000/SYSTEM_TICK)/((14400/10)/4) 
.EQU	TOut_19200		=1+(1000000/SYSTEM_TICK)/((19200/10)/4) 
.EQU	TOut_28800		=1+(1000000/SYSTEM_TICK)/((28800/10)/4) 
.EQU	TOut_33600		=1+(1000000/SYSTEM_TICK)/((33600/10)/4) 
.EQU	TOut_57600		=1+(1000000/SYSTEM_TICK)/((57600/10)/4) 
.EQU	TOut_76800		=1+(1000000/SYSTEM_TICK)/((76800/10)/4) 
.EQU	TOut_115200		=1+(1000000/SYSTEM_TICK)/((115200/10)/4) 
//---------------------------------------------------------
.EQU	CONST_DEFAULT_CONFIG	=10
.EQU	CONST_SPEED_TABLE_SIZE	=11

;**********************************************************
; SETs of MACRO
//---------------------------------------
.MACRO ____UART_INIT		;портит к24 к25, v r1:r0 - TIMEOUT
	lds	r24,@0
	RCALL	UART_INIT
.ENDMACRO
//---------------------------------------
.MACRO ____UART_READ
#if defined(__ATmega16__)
	IN	@0,UDR
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	LDS	@0,UDR0
#endif
.ENDMACRO

//---------------------------------------
.MACRO ____UART_WRITE
#if defined(__ATmega16__)
	OUT	UDR,@0
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	STS	UDR0,@0
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_DRE_ON		
#if defined(__ATmega16__)
	in	@0,UCSRB
	sbr	@0,(1<<UDRIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	sbr	@0,(1<<UDRIE0)
	sts	UCSR0B,@0	
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_DRE_OFF		
#if defined(__ATmega16__)
	in	@0,UCSRB
	cbr	@0,(1<<UDRIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	cbr	@0,(1<<UDRIE0)
	sts	UCSR0B,@0	
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_TXC_ON			
#if defined(__ATmega16__)
	in	@0,UCSRB
	sbr	@0,(1<<TXCIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	sbr	@0,(1<<TXCIE0)
	sts	UCSR0B,@0
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_TXC_OFF			
#if defined(__ATmega16__)
	in	@0,UCSRB
	cbr	@0,(1<<TXCIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	cbr	@0,(1<<TXCIE0)
	sts	UCSR0B,@0	
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_RXC_ON			
#if defined(__ATmega16__)
	in	@0,UCSRB
	sbr	@0,(1<<RXCIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	sbr	@0,(1<<RXCIE0)
	sts	UCSR0B,@0	
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_RXC_OFF			
#if defined(__ATmega16__)
	in	@0,UCSRB
	cbr	@0,(1<<RXCIE)
	out	UCSRB,@0	
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	@0,UCSR0B
	cbr	@0,(1<<RXCIE0)
	sts	UCSR0B,@0	
#endif
.ENDMACRO

//---------------------------------------
.MACRO ____UART_CONFIGURE
#if defined(__ATmega16__)
	ldi	@0,(1<<TXC)	;сбросить прерывание на всякий случай
	out	UCSRA,@0
	ldi	@0,(1<<RXEN)+(1<<TXEN)
	out	UCSRB,@0
	ldi	@0,(1<<URSEL)+(1<<UCSZ1)+(1<<UCSZ0)
	out	UCSRC,@0

#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	ldi	@0,(1<<TXC0)
	sts	UCSR0A,@0
	ldi	@0,(1<<RXEN0)+(1<<TXEN0)
	sts	UCSR0B,@0
	ldi	@0,(1<<UCSZ01)+(1<<UCSZ00)
	sts	UCSR0C,@0
#endif
.ENDMACRO

//---------------------------------------
.MACRO	____UART_SET_SPEED
#if defined(__ATmega16__)
	out	UBRRL,r24
	out	UBRRH,r25
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	sts	UBRR0L,r24
	sts	UBRR0H,r25
#endif
.ENDMACRO

;***********************************************
.EXIT
