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
.device ATtiny2313

    
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

