;Note that the stack pointer is init'ed upon hardware reset
;
Init:
	cli

	rcall	WDDisable			;disable watchdog timer

	rcall	SYSCLKSET			;set system clock prescaler

	clr		r0					;zero some registers
	clr		r1					;zero some registers

#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
;	GoSub	usb_configured		;wait till configed
;;	LOAD	inbytel,usb_configuration;wait till configed	
;	tst		inbytel				;test inbytel for zero
;	breq	Init1				;branch if not configured yet
	GoSub	usb_shutdown		;insure USB interface has been terminated
								; if we were restarted by the bootloader,
								; we may not have had a proper USB disconnect
  #if !TEST	;bypass delay in TEST mode
	GoSub	Delay100mS			;delay 100 mS
  #endif
#endif

Init1:	;init USB if enabled
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	GoSub	usb_init			;init USB interface
								;sei set by usb_serial_init routine

	ldi		temp,USBConTmr		;load USB connection timeout		
	mov		bytecntr,temp
	GoSub	Delay1S				;delay 1 Second

Init1a:	;init USB if enabled
	CLRB	GPIOR0,WDTIRQ		;insure WDTIRQ flag is clear
;enable watchdog timer for 8 seconds IRQ
	STOREI	WDTCSR,(1<<WDCE|1<<WDE)
	STOREI	WDTCSR,((1<<WDIE)|(1<<WDP3)|(1<<WDP0)|(0<<WDE))
	wdr							;reset watchdog

Init1b:
#if !TEST	;bypass delay in TEST mode
	GoSub	Delay100mS			;delay 100 mS
#endif
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	Init1d				;WDT tripped, no connection
	GoSub	usb_configured		;wait till configed
;	LOAD	inbytel,usb_configuration;load config status
	tst		inbytel				;if zero, we have a connection
	breq	Init1b				;loop till not zero


Init1c:	;we wait here for the user's terminal to connect and set DTR
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	Init1d				;loop till DTR set

	GoSub	usb_serial_get_control	;get DTR/RTS status
	LOAD	temp,cdc_line_rtsdtr;get value into register
	sbrs	temp,USB_SERIAL_RTS	;skip if USB_SERIAL_RTS bit is set
	rjmp	Init1c				;loop till DTR set

	GoSub	usb_serial_flush_input 	;flush any junk input
	GoSub	usb_serial_flush_output ;flush output buffer
	rjmp	Init1e				;continue

Init1d:	;we arrived here because of a good connection or a WDT timeout
	dec		bytecntr			;decrement timeout counter
	brne	Init1a				;loop till 0
	GoSub	WDDisable			;disable watchdog timer
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	GoSub	usb_shutdown		;insure USB interface has been terminated

Init1e:	;we either got a USB connection or didn't, clear the WDT flag anyhow
	CLRB	GPIOR0,WDTIRQ		;insure WDTIRQ flag is clear

#else	;init USART
;enable TIMx in PRRx
	CLRB	UART_PRR,UART_PRUART,temp	;enable UART in PRR
	ldi     temp,high(((f_clk/16)/baudrate)-1)     
	STORE	UBRRH,temp     	;Load UART baud rate
	ldi     temp,low(((f_clk/16)/baudrate)-1)
	STORE	UBRRL,temp     	;Load UART baud rate

; USART0: RX Interupts, enable TX and RX
	ldi		temp,(1<<RXCIE|0<<TXCIE|0<<UDRIE| \
					1<<RXEN|1<<TXEN|0<<UCSZ2)
	STORE	UCSRB,temp

	sei							;enable global interrupts otherwise no serial I/O
#endif

Init2:
;Port setup for Power-Up
;------------------------
.ifdef PORTA
	ldi		temp,PrtAdir		;port A DDR
	STORE	DDRA,temp
	ldi		temp,PrtAPup
	STORE	PORTA,temp
.endif
;------------------------
.ifdef PORTB
	ldi		temp,PrtBdir		;port B DDR
  #if DREC	;enable/disable AVR Data Recorder features
	ori		temp,(1<<DIGOUT)	;port bit for digital outpuy
  #endif
	STORE	DDRB,temp
	ldi		temp,PrtBPup
  #if DREC	;enable/disable AVR Data Recorder features
	ori		temp,(0<<DIGOUT)	;port bit for digital output
  #endif
	STORE	PORTB,temp
.endif
;------------------------
.ifdef PORTC
	ldi		temp,PrtCdir		;port C DDR
  #if DREC	;enable/disable AVR Data Recorder features
	ori		temp,(  (1<<AD536CH) | \
					(1<<AD536OS) )	;port bits for AD536
  #endif
	STORE	DDRC,temp
	ldi		temp,PrtCPup
	STORE	PORTC,temp
	
	ldi		temp,PrtDdir		;port D DDR
  #if DREC	;enable/disable AVR Data Recorder features
	ori		temp,(  (1<<ADC0_R0) | \
					(1<<ADC0_R1) | \
					(1<<ADC1_R0) | \
					(1<<ADC1_R1) )	;port bits for DREC gain
  #endif
.endif
;------------------------
.ifdef PORTD
	ldi		temp,PrtDdir		;port D DDR
	STORE	DDRD,temp
	ldi		temp,PrtDPup
	STORE	PORTD,temp
.endif
;------------------------
.ifdef PORTE
	ldi		temp,PrtEdir		;port E DDR
	STORE	DDRE,temp
	ldi		temp,PrtEPup
	STORE	PORTE,temp
.endif
;------------------------
.ifdef PORTF
	ldi		temp,PrtFdir		;port F DDR
	STORE	DDRF,temp
	ldi		temp,PrtFPup
	STORE	PORTF,temp
.endif

;------------------------
.if DSI	;initialize the DS pins
	LOAD	temp,DS_PORT-1		
	andi	temp,~( 1<<DS_SDA | \
					1<<DS_SDD )
	STORE	DS_PORT-1,temp		;set DS port bits as inputs

  .if DS_PUPS
  .message "Enabling Port-Pin Pullups for DS interface."
	LOAD	temp,DS_PORT-2
	ori		temp,(  1<<DS_SDA | \
					1<<DS_SDD )
	STORE	DS_PORT,temp		;set pullups on DS port bits
  .endif
.endif
;
;------------------------
.if (DHT && RTC)	;initiate the DHT pin as input w/ pullup
	CLRB	DHPORT-1,DHTPIN		;set DATA pin to output
	SETB	DHPORT,DHTPIN		;enable DATA pin pullup

	LOAD	temp,GPIOR1			;get DHT flags
	andi	temp,~((1<<DHTSign) | (1<<DHTUnit))	
	STORE	GPIOR1,temp			;clear flags to default

	STOREI	RT_NRC,3			;preset no-response counter

	SetZPtr	HTRdTmr 			;seed DHT read timeout
	STORE16	RT_TMR,ZH,ZL
.endif

Init3:
.if ICP
	STOREI	ICPCfg,1,temp		;init ICPCfg RAM variable
.endif
;
Init4:
.if EFS	
;====================================================================
;File system support.  Check for erased EEP and initialize the EEP 
;  with all file handles and sizes being "0".
;NOTE: does not check for valid programs stored at each file handle.
;====================================================================
	ldi		U,EFSBLKSZ			;load byte counter
	SETYPTR	EFSIDX0				;Y = 1st byte of file index pointers
	set							;set T-Flag to wait for EEP write 
Init4a:
	GoSub	ReadEEP				;get the byte
	cpi		temp,0xFF			;is it blank?
	brne	Init4b				;no, branch
	clr		temp				;zero the byte
	GoSub	WriteEEP			;write it
Init4b:
	ADDI16	YH,YL,1				;increment EEP pointer
	dec		U					;decrement loop counter
	brne	Init4a				;loop 
;
.endif
;
.if DATA	;data file variables
	clr		temp				;clear register
	STORE	DFLaddr,temp		;zero the internal address counter
	STORE	DFLaddr+1,temp
	STOREI	DFDfmt,1			;set data dump format to "1"
	STOREI	DFIbyte,DFI_BYTE	;set the bulk erase value
.endif

.if ADConv	;preload ADC registers
	STOREI	ADMUX,ADMUX_L,temp
	STOREI	ADCSRB,ADCSRB_L,temp
	STOREI	ADCSRA,ADCSRA_L,temp
.endif

#if DREC	;enable AVR Data Recorder features
	ldi		temp,(  (1<<ADCH0) | \
					(1<<ADCH1) | \
					(1<<ADCH2) | \
					(1<<ADCH3) )	;port bits for DREC DIDR0
	STORE	DIDR0,temp				;disable digital inputs
#endif

.if RTC	;Real-time Counter init
	Gosub	RTC_TIMINIT			;enable real-time counter
	STOREI	RTCMul,RTCMul_0		;save default multiplier value in RAM
.endif

	;init flags
	STOREI	GPIOR0,0			;clear all GPIOR0 flags
	STOREI	GPIOR1,(1<<ACOCh1)	;clear all GPIOR1 flags

	;init random # seed
	GoSub	MkRndSeed			;generate random seed before clearing RAM

Init4c:
	STOREI	MCUSR,0				;clear MCUSR flags
.ifdef RAMPZ					;need RAMPZ to access >64KW FLASH
	STOREI	RAMPZ,0				;clear RAMPZ register
.endif
	STOREI	CurrLine,0			;clear current line counter
;
;------------------------------------------------
;set sleep mode to idle
	ldi		temp,(0<<SM2|0<<SM0|1<<SE)
	STORE	SMCR,temp

#if !TEST
.if SStst
	SETB	SS_PORT+2,SS_PIN	;enable port pull-up
	GoSub	Delay10uS			;delay 10uS
	LOAD	temp,SS_PORT		;get input port
	CLRB	SS_PORT+2,SS_PIN	;disable port pull-up
	sbrc	temp,SS_PIN			;If SS_PORT,SS_PIN is grounded then 
								; execute from EEPROM
	rjmp	readytorun

.if !EFS
	GoSub	loadcommand			;load program (no EFS)
.else
	INITDSTACK					;Initialize data stack pointer
	clr		U					;default to program '0'
	sbr		XH,(1<<RUNACT)		;set flag to indicate we are executing a command
	GoSub	LOADcommanda		;load program (EFS)
.endif
	GoSub   runcommand

	GoSub	crlf				;print two lines
	GoSub	crlf
	SETZPTR (2*signonmessage)	;Print 1st part of sign-on message
	GoSub	sendlromline		;sent it.
	ret							;return to caller
.endif
#endif

readytorun:
	GoSub	newprogram			;Initialize program memory
								; also spits out short sign-on
	ret
;
;*************************************************************************************
; 	WDDisable:  Disabled watch-dog timer.
;*************************************************************************************
WDDisable:
	PushStat					;save status register (preserve I flag)
	wdr							;reset WDR
	LOAD	temp,MCUSR			;clear WDRF flag
 	andi	temp,~(1<<WDRF)
	STORE	MCUSR,temp

	; Write logical one to WDCE and WDE
	; Keep old prescaler setting to prevent unintentional time-out
	cli							;disable interrupts to set WDT
	LOAD	temp,WDTCSR			;get WDTCSR
	ori		temp,(1<<WDCE) | (1<<WDE)
	STORE	WDTCSR,temp
 
	; Turn off WDT
	andi	temp,0<<WDE
	STORE	WDTCSR,temp

	PopStat						;restore I flag
	ret
;
;*************************************************************************
; SYSCLKSET - Sets the system clock presecaler
;*************************************************************************
;
SYSCLKSET:
	ldi		U,(1<<CLKPCE)			;load CLKPS enable bit
	ldi		T,LOG2(FCLK_PS)			;load CLKPS divisor
	STORE	CLKPR,U
	STORE	CLKPR,T					;set it
	ret
;
;***************************************************************

