;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number           :AVR000
;* File Name        :"tn2313def.inc"
;* Title            :Register/Bit Definitions for the ATtiny2313
;* Date             :03.06.17
;* Version          :1.00
;* Support E-mail   :avr@atmel.com
;* Target MCU       :ATtiny2313
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register  
;* names and I/O register bit names appearing in the data book can be used.
;* In addition, the two registers forming the data pointer Z have been 
;* assigned names ZL - ZH. 
;*
;* The Register names are represented by their hexadecimal address.
;* 
;* The Register Bit names are represented by their bit number (0-7).
;* 
;* Please observe the difference in using the bit names with instructions
;* such as "sbr"/"cbr" (set/clear bit in register) and "sbrs"/"sbrc" 
;* (skip if bit in register set/cleared). The following example illustrates
;* this:
;* 
;* in   r16,PORTB       ;read PORTB latch
;* sbr  r16,(1<<PB6)+(1<<PB5)   ;set PB6 and PB5 (use masks, not bit#)
;* out  PORTB,r16       ;output to PORTB
;*
;* in   r16,TIFR        ;read the Timer Interrupt Flag Register
;* sbrc r16,TOV0        ;test the overflow flag (use bit#)
;* rjmp TOV0_is_set     ;jump if set
;* ...                  ;otherwise do something else
;*  
;***************************************************************************

;***** Specify Device
;.device ATtiny2313

    
;*****************************************************************************
; I/O Register Definitions
;*****************************************************************************

.equ    SREG    = 0x3F
.equ    SPL     = 0x3D
.equ    OCR0B   = 0x3C
.equ    GIMSK   = 0x3B
.equ    EIFR    = 0x3A
.equ    GIFR    = 0x3A      ; for compatibility purpose
.equ    TIMSK   = 0x39       
.equ    TIFR    = 0x38
.equ    SPMCSR  = 0x37
.equ    OCR0A   = 0x36
.equ    MCUCR   = 0x35
.equ    MCUSR   = 0x34
.equ    TCCR0B  = 0x33
.equ    TCCR0   = 0x33      ; for compatibility purpose
.equ    TCNT0   = 0x32
.equ    OSCCAL  = 0x31
.equ    TCCR0A  = 0x30
.equ    TCCR1A  = 0x2F
.equ    TCCR1B  = 0x2E
.equ    TCNT1H  = 0x2D
.equ    TCNT1L  = 0x2C
.equ    OCR1AH  = 0x2B
.equ    OCR1AL  = 0x2A
.equ    OCR1BH  = 0x29
.equ    OCR1BL  = 0x28
.equ    CLKPR   = 0x26
.equ    ICR1H   = 0x25
.equ    ICR1L   = 0x24
.equ    GTCCR   = 0x23     
.equ    SFIOR   = 0x23      ; for compatibility purpose
.equ    TCCR1C  = 0x22
.equ    WDTCR   = 0x21
.equ    PCMSK   = 0x20
.equ    EEAR    = 0x1E      ; for compatibility purpose
.equ    EEARL   = 0x1E  
.equ    EEDR    = 0x1D
.equ    EECR    = 0x1C
.equ    PORTA   = 0x1B
.equ    DDRA    = 0x1A
.equ    PINA    = 0x19
.equ    PORTB   = 0x18
.equ    DDRB    = 0x17
.equ    PINB    = 0x16
.equ    GPIOR2  = 0x15
.equ    GPIOR1  = 0x14
.equ    GPIOR0  = 0x13
.equ    PORTD   = 0x12
.equ    DDRD    = 0x11
.equ    PIND    = 0x10  
.equ    USIDR   = 0x0F
.equ    USISR   = 0x0E
.equ    USICR   = 0x0D
.equ    UDR     = 0x0C
.equ    UCSRA   = 0x0B
.equ    USR     = 0x0B      ; for compatibility purpose
.equ    UCSRB   = 0x0A
.equ    UCR     = 0x0A      ; for compatibility purpose
.equ    UBRRL   = 0x09
.equ    UBRR    = 0x09      ; for compatibility purpose
.equ    ACSR    = 0x08
.equ    UCSRC   = 0x03
.equ    UBRRH   = 0x02
.equ    DIDR    = 0x01

    
;*****************************************************************************
; Bit Definitions
;*****************************************************************************

;***** SREG *******
.equ I          = 7 
.equ T          = 6
.equ H          = 5
.equ S          = 4
.equ V          = 3
.equ N          = 2
.equ Z          = 1
.equ C          = 0

;***** SPL ********
.equ SP7        = 7 
.equ SP6        = 6
.equ SP5        = 5
.equ SP4        = 4
.equ SP3        = 3
.equ SP2        = 2
.equ SP1        = 1
.equ SP0        = 0

;***** GIMSK ******
.equ INT1       = 7 
.equ INT0       = 6  
.equ PCIE       = 5

;***** EIFR *******
.equ INTF1      = 7
.equ INTF0      = 6  
.equ PCIF       = 5

;***** TIMSK ******
.equ TOIE1      = 7
.equ OCIE1A     = 6
.equ OCIE1B     = 5
.equ ICIE1      = 3  
.equ OCIE0B     = 2  
.equ TOIE0      = 1
.equ OCIE0A     = 0
.equ TICIE      = 3     ; for compatibility purpose

;***** TIFR *******
.equ TOV1       = 7
.equ OCF1A      = 6
.equ OCF1B      = 5
.equ ICF1       = 3  
.equ OCF0B      = 2  
.equ TOV0       = 1
.equ OCF0A      = 0

;***** SPMCSR *****
.equ CTPB       = 4
.equ RFLB       = 3
.equ PGWRT      = 2
.equ PGERS      = 1
.equ SPMEN      = 0

;***** MCUCR ******
.equ PUD        = 7
.equ SM1        = 6
.equ SE         = 5
.equ SM0        = 4
.equ ISC11      = 3
.equ ISC10      = 2
.equ ISC01      = 1
.equ ISC00      = 0
.equ SM         = 4     ; for compatibility purpose

;***** MCUSR ******
.equ WDRF       = 3
.equ BORF       = 2
.equ EXTRF      = 1
.equ PORF       = 0

;***** TCCR0B *****
.equ FOC0A      = 7
.equ FOC0B      = 6
.equ WGM02      = 3
.equ CS02       = 2
.equ CS01       = 1
.equ CS00       = 0

;***** OSCCAL *****
.equ CAL6       = 6
.equ CAL5       = 5
.equ CAL4       = 4
.equ CAL3       = 3
.equ CAL2       = 2
.equ CAL1       = 1
.equ CAL0       = 0

;***** TCCR0A *****
.equ COM0A1     = 7
.equ COM0A0     = 6
.equ COM0B1     = 5
.equ COM0B0     = 4
.equ WGM01      = 1
.equ WGM00      = 0

;***** TCCR1A *****
.equ COM1A1     = 7
.equ COM1A0     = 6
.equ COM1B1     = 5
.equ COM1B0     = 4
.equ WGM11      = 1
.equ WGM10      = 0
.equ PWM11      = 1     ; for compatibility purpose
.equ PWM10      = 0     ; for compatibility purpose

;***** TCCR1B *****
.equ ICNC1      = 7
.equ ICES1      = 6
.equ WGM13      = 4
.equ WGM12      = 3
.equ CS12       = 2
.equ CS11       = 1
.equ CS10       = 0
.equ CTC1       = 3     ; for compatibility purpose

;***** CLKPR ******
.equ CLKPCE     = 7 
.equ CLKPS3     = 3
.equ CLKPS2     = 2
.equ CLKPS1     = 1
.equ CLKPS0     = 0

;***** SFIOR ******
.equ PSR10      = 0 

;***** TCCR1C *****
.equ FOC1A      = 7
.equ FOC1B      = 6

;***** WDTCSR *****
.equ WDIF       = 7
.equ WDIE       = 6
.equ WDP3       = 5
.equ WDCE       = 4
.equ WDE        = 3
.equ WDP2       = 2
.equ WDP1       = 1
.equ WDP0       = 0
.equ WDTOE      = 4     

;***** PCMSK ******
.equ PCINT7     = 7
.equ PCINT6     = 6
.equ PCINT5     = 5  
.equ PCINT4     = 4
.equ PCINT3     = 3
.equ PCINT2     = 2
.equ PCINT1     = 1
.equ PCINT0     = 0

;***** EECR *******
.equ EEPM1      = 5
.equ EEPM0      = 4
.equ EERIE      = 3
.equ EEMPE      = 2
.equ EEPE       = 1
.equ EERE       = 0
; Kept for backward compatibility
.equ EEMWE      = 2
.equ EEWE       = 1


;***** PORTA ******
.equ PORTA2     = 2
.equ PORTA1     = 1
.equ PORTA0     = 0

;***** DDRA *******
.equ DDA2       = 2
.equ DDA1       = 1
.equ DDA0       = 0
    
;***** PINA *******
.equ PINA2      = 2
.equ PINA1      = 1
.equ PINA0      = 0
    
;***** PORTB ******
.equ PORTB7     = 7
.equ PORTB6     = 6
.equ PORTB5     = 5
.equ PORTB4     = 4
.equ PORTB3     = 3
.equ PORTB2     = 2
.equ PORTB1     = 1
.equ PORTB0     = 0

;***** DDRB *******
.equ DDB7       = 7
.equ DDB6       = 6
.equ DDB5       = 5 
.equ DDB4       = 4
.equ DDB3       = 3
.equ DDB2       = 2
.equ DDB1       = 1
.equ DDB0       = 0
    
;***** PINB *******
.equ PINB7      = 7
.equ PINB6      = 6
.equ PINB5      = 5 
.equ PINB4      = 4
.equ PINB3      = 3
.equ PINB2      = 2
.equ PINB1      = 1
.equ PINB0      = 0

;***** PORTD ******
.equ PORTD6     = 6
.equ PORTD5     = 5
.equ PORTD4     = 4
.equ PORTD3     = 3
.equ PORTD2     = 2
.equ PORTD1     = 1
.equ PORTD0     = 0

;***** DDRD *******
.equ DDD6       = 6
.equ DDD5       = 5 
.equ DDD4       = 4
.equ DDD3       = 3
.equ DDD2       = 2
.equ DDD1       = 1
.equ DDD0       = 0

;***** PIND *******
.equ PIND6      = 6
.equ PIND5      = 5 
.equ PIND4      = 4
.equ PIND3      = 3
.equ PIND2      = 2
.equ PIND1      = 1
.equ PIND0      = 0 

;***** USISR ******
.equ USISIF     = 7
.equ USIOIF     = 6
.equ USIPF      = 5
.equ USIDC      = 4
.equ USICNT3    = 3
.equ USICNT2    = 2
.equ USICNT1    = 1
.equ USICNT0    = 0

;***** USICR ******
.equ USISIE     = 7
.equ USIOIE     = 6
.equ USIWM1     = 5
.equ USIWM0     = 4
.equ USICS1     = 3
.equ USICS0     = 2
.equ USICLK     = 1
.equ USITC      = 0

;***** UCSRA ******
.equ RXC        = 7
.equ TXC        = 6
.equ UDRE       = 5
.equ FE         = 4
.equ DOR        = 3
.equ UPE	= 2
.equ PE         = 2	; for compatibility purpose
.equ U2X        = 1
.equ MPCM       = 0
.equ OR         = 3     ; for compatibility purpose

;***** UCSRB ******
.equ RXCIE      = 7 
.equ TXCIE      = 6
.equ UDRIE      = 5
.equ RXEN       = 4
.equ TXEN       = 3
.equ UCSZ2      = 2
.equ RXB8       = 1
.equ TXB8       = 0
.equ CHR9       = 2     ; for compatibility purpose
    
;***** ACSR *******
.equ ACD        = 7  
.equ ACBG       = 6
.equ ACO        = 5
.equ ACI        = 4
.equ ACIE       = 3
.equ ACIC       = 2
.equ ACIS1      = 1
.equ ACIS0      = 0

;***** UCSRC ******
.equ UMSEL      = 6 
.equ UPM1       = 5
.equ UPM0       = 4
.equ USBS       = 3
.equ UCSZ1      = 2
.equ UCSZ0      = 1
.equ UCPOL      = 0

;***** DIDR ******
.equ AIN1D      = 1
.equ AIN0D      = 0

;*****************************************************************************
; CPU Register Declarations
;*****************************************************************************

.def    XL      = r26       ; X pointer low
.def    XH      = r27       ; X pointer high
.def    YL      = r28       ; Y pointer low
.def    YH      = r29       ; Y pointer high
.def    ZL      = r30       ; Z pointer low
.def    ZH      = r31       ; Z pointer high


;*****************************************************************************
; Data Memory Declarations
;*****************************************************************************

.equ    RAMEND      = 0xDF  ; Highest internal data memory (SRAM) address.
                            ;(128 Bytes RAM + IO + REG)
.equ    EEPROMEND   = 0x7F  ; Highest EEPROM address.
                            ;(128 Bytes)
.equ    EEADRBITS   =   7   ; no. of bits in EEPROM address register

.equ    RAM_SIZE    = 128


;*****************************************************************************
; Program Memory Declarations
;*****************************************************************************

.equ    FLASHEND    = 0x3FF ;  Highest program memory (flash) address
                            ; (When addressed as 16 bit words)
                            ; ( 1024 words , 2K byte ) 

;**** Page Size ****
.equ    PAGESIZE    = 16    ;number of WORDS in a Flash page
.equ    EEPAGESIZE  =  2    ;number of WORDS in an EEPROM page

;*****************************************************************************
;**** Interrupt Vectors ****        
;*****************************************************************************

.equ    INT0addr        = 0x001     ;External Interrupt0
.equ    INT1addr        = 0x002     ;External Interrupt1
.equ    ICP1addr        = 0x003     ;Input capture interrupt 1
.equ    OC1Aaddr        = 0x004     ;Timer/Counter1 Compare Match A     
.equ    OVF1addr        = 0x005     ;Overflow1 Interrupt 
.equ    OVF0addr        = 0x006     ;Overflow0 Interrupt
.equ    URXC0addr       = 0x007     ;USART0 RX Complete Interrupt
.equ    UDRE0addr       = 0x008     ;USART0 Data Register Empty Interrupt
.equ    UTXC0addr       = 0x009     ;USART0 TX Complete Interrupt 
.equ    ACIaddr         = 0x00A     ;Analog Comparator Interrupt 
.equ    PCINTaddr       = 0x00B     ;Pin Change Interrupt
.equ    OC1Baddr        = 0x00C     ;Timer/Counter1 Compare Match B
.equ    OC0Aaddr        = 0x00D     ;Timer/Counter0 Compare Match A     
.equ    OC0Baddr        = 0x00E     ;Timer/Counter0 Compare Match B     
.equ    USI_STARTaddr   = 0x00F     ;USI start interrupt
.equ    USI_OVFaddr     = 0x010     ;USI overflow interrupt
.equ    ERDYaddr        = 0x011     ;EEPROM write complete
.equ    WDTaddr         = 0x012     ;Watchdog Timer Interrupt
		; for compatibility purpose
.equ	URXCaddr		= 0x007		
.equ	UDREaddr		= 0x008		
.equ	UTXCaddr		= 0x009		

;*****************************************************************************
;*****************************************************************************
