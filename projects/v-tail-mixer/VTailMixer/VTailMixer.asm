;***************************************************************************
;* V - T A I L   M I X E R   F I R M W A R E   F O R   A T T I N Y 1 3      
;* 
;* File Name            :"VTailMixer.asm"
;* Title                :V-Tail Mixer Source
;* Date                 :08.31.2007
;* Version              :1.0
;* Support email        :support@gadgetparadise.com
;* Target MCU           :ATtiny13
;* Source TAB size      :2
;* Compiler             :AVRAssembler2
;*
;* DESCRIPTION
;*
;* This program is V-tail mixer. It measures the standard R/C servo 
;* pulses (1-2ms) on two channels with 1200 steps resolution.
;* And it generates the sum and differential pulse length
;* on two servo output channels.
;* 
;* WARRANTY DISCLAIMER:
;* THIS PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTY, EXPRESSED OR 
;* IMPLIED, INCLUDING BUT NOT LIMITED TO FITNESS FOR A PARTICULAR 
;* PURPOSE. THE AUTHOR ASSUME NO LIABILITY FOR DAMAGES, DIRECT OR 
;* CONSEQUENTIAL, WHICH MAY RESULT FROM THE USE OF TTF2FNA SOFTWARE.
;* 
;* SOFTWARE LICENSE AND DISTRIBUTION:
;* 
;* The V-Tail mixer software is exclusively owned by Laszlo Arvai. 
;* This software is released as a freeware. You may freely use or 
;* distribute this program for any non-commercial purposes. 
;* Distribution is permitted only for the whole software package 
;* without any modifications and without any fee of charge. All 
;* accompanying files must be included.
;* The source code is published under the GNU General Public 
;* License. You can freely modify it or integrate it into your 
;* projects even if your project is commercial.
;* But you can't use the hex files or the 
;* source files in their original form for commercial 
;* purpose, without a written permission from the 
;* software author.
;*
;***************************************************************************

.include "tn13def.inc"

;***** Macros
#define RAMINDEX( x ) Z + (x - SRAM_START)
#define RAMINDEX_LOW( x ) Z + (x - SRAM_START)
#define RAMINDEX_HIGH( x ) Z + (x - SRAM_START + 1)

;***** We don't need index registers (except Z)
.undef	XH
.undef	XL
.undef	YH
.undef	YL

;***** Constants
.equ	CHANNEL_RESOLUTION = 1200				; resolution of channels
.equ	MIDDLE_VALUE = CHANNEL_RESOLUTION / 2	; middle position value
.equ	CHANNEL_MIN_PULSE_LENGTH = 1200			; min pulse length measured in timer increments
.equ	CHANNEL_PULSE_REPEAT_RATIO = 24000		; PPM pulse repeat ratio measured in timer increments
.equ	GLITCH_TOLERANCE = 600					; If the pulse length is lower or higher by this value than expected the pulse considered as glitch
.equ	PULSE_REPEAT_TIMEOUT = 200				; Servo pulse repeat timeout value for pulse timeout (in 256x timer increment)
.equ	PULSE_OUT_DISABLED_STATUS = 0xff		; Pulse output disabled status code

; default input output values
.equ	DEFAULT_CHANNEL_OUT_VALUE = MIDDLE_VALUE + CHANNEL_MIN_PULSE_LENGTH - 256;
.equ	DEFAULT_CHANNEL_IN_VALUE = MIDDLE_VALUE;

;***** Pin definitions

; input pins
.equ CH0_INPUT_PIN = PB3
.equ CH1_INPUT_PIN = PB4

; output pins
.equ CH0_OUTPUT_PIN = PB0
.equ CH1_OUTPUT_PIN = PB1

;***** Reserved memory address

.dseg
ch0_out_status:		.byte 1 	; Output Channel0 status register
ch0_out_value:		.byte 2		; Output Channel0 value
ch0_out_counter:	.byte 2 	; Output Channel0 time counter

ch1_out_status:		.byte 1		; Output Channel1 status register
ch1_out_value:		.byte 2		; Output Channel1 value
ch1_out_counter:	.byte 2		; Output Channel1 time counter

ch0_in_status:		.byte 1		; Input Channel0 status
ch0_time_stamp:		.byte 2		; Input Channel0 time stamp
ch0_in_value:		.byte 2		; Input Channel0 pulse length

ch1_in_status:		.byte 1		; Input Channel1 status
ch1_time_stamp:		.byte 2		; Input Channel1 time stamp
ch1_in_value:		.byte 2		; Input Channel1 pulse length


;***** Global Register Variables
.def it_timer_high = R15
.def it_pulse_timeout = r10

;***** EEPROM data
.eseg
ee_fail_save_settings:
	.db 0xff


;***** Interrupt table

	.cseg

	.org	$0000
	rjmp	RESET_vect      	; Reset handle

	.org	INT0addr
	reti

	.org	PCI0addr
	rjmp	PCINT0_vect

	.org	OVF0addr
	rjmp	TIM0_OVF_vect

	.org	ERDYaddr
	reti

	.org	ACIaddr
	reti

	.org	OC0Aaddr
	rjmp	TIM0_COMPA_vect		; Timer0 CompareA handle

	.org	OC0Baddr
	rjmp	TIM0_COMPB_vect		; Timer0 CompareB handle

	.org	WDTaddr
	reti

	.org	ADCCaddr
	reti


;***** Code


;****************************************************************************
;*
;* Main Program
;*
;* Folowing functions are implemented:
;*  - system init
;*  - load fail safe settings from EEPROM
;*  - check for jupmer on servo outputs and check fail save settings if jumper detected
;*  - calculate servo pulse length
;* 
;* All other functions are implementend in irq functions
;*
;***************************************************************************

;***** Main Program Register Variables

.def	m_temp	= r16					; Main program temp variable
.def	m_low	= r17
.def	m_high 	= r18
.def	m_low2	= r8
.def	m_high2	= r9
.def 	m_fail_safe_settings = r11

;***** Code

RESET_vect:

	;** Init stack
	ldi 	m_temp,low(RAMEND)			; Load low byte address of end of RAM into register R16
	out		SPL,m_temp					; Initialize stack pointer to end of internal RAM

	;** Init RAM address pointer (indirect memory access is used to avoid 32 length of lds and sts instruction)
	ldi		ZL, low( SRAM_START )
	ldi		ZH, high( SRAM_START )

	; init SRAM
	movw	Y, Z
	clr		m_low
	ldi		m_temp, SRAM_SIZE

clr_sram:
	st		Y+, m_low
	dec		m_temp
	brne	clr_sram

	;** Load fail safe settigs from the EEPROM
	ldi		m_temp, ee_fail_save_settings		; set address
	out		EEARL, m_temp

	sbi		EECR, EERE							; start read

	in		m_fail_safe_settings, EEDR			; read data

	;** Check for jumper connection between two output pins

	; setup port (OUT0 - output, OUT1 - input)
	ldi		m_temp, (1<<(CH0_OUTPUT_PIN))
	out		DDRB, m_temp

	; activate weak pull up
	ldi		m_temp, (1<<(CH1_OUTPUT_PIN))
	out		PORTB, m_temp

	rcall	short_delay

	; check if input pin is 0
	sbic	PINB, CH1_OUTPUT_PIN
	rjmp	normal_start

	; change output to 1
	sbi		PORTB, CH0_OUTPUT_PIN

	rcall	short_delay

	; check if input pin is 1
	sbis	PINB, CH1_OUTPUT_PIN
	rjmp	normal_start

	; change output to 0
	cbi		PORTB, CH0_OUTPUT_PIN

	rcall	short_delay

	; check if input pin is 0
	sbic	PINB, CH1_OUTPUT_PIN
	rjmp	normal_start

	; change output to 1
	sbi		PORTB, CH0_OUTPUT_PIN

	rcall	short_delay

	; check if input pin is 1
	sbis	PINB, CH1_OUTPUT_PIN
	rjmp	normal_start

	; change output to 0
	cbi		PORTB, CH0_OUTPUT_PIN

	rcall	short_delay

	; check if input pin is 0
	sbic	PINB, CH1_OUTPUT_PIN
	rjmp	normal_start

settings_change:
	; invert settings
	com		m_fail_safe_settings

	; store settings in EEPROM
	ldi		m_temp, (0<<EEPM1) | (0<<EEPM0)		; erase&write mode
	out		EECR, m_temp

	ldi		m_temp, ee_fail_save_settings		; address
	out		EEARL, m_temp

	out		EEDR, m_fail_safe_settings			; data

	sbi		EECR, EEMPE							; enable program

	sbi		EECR, EEPE							; program

	; infinite loop
sc_loop:
	rjmp	sc_loop

	;** Init variables
normal_start:

	; init pulse timeout
	ldi		m_temp, PULSE_REPEAT_TIMEOUT
	mov		it_pulse_timeout, m_temp

	; init ch0_out_value
	ldi		m_temp, LOW(DEFAULT_CHANNEL_OUT_VALUE)
	std		RAMINDEX_LOW(ch0_out_value), m_temp

	ldi		m_temp, HIGH(DEFAULT_CHANNEL_OUT_VALUE)
	std		RAMINDEX_HIGH(ch0_out_value), m_temp

	; init ch1_out_value
	ldi		m_temp, LOW(DEFAULT_CHANNEL_OUT_VALUE)
	std		RAMINDEX_LOW(ch1_out_value), m_temp

	ldi		m_temp, HIGH(DEFAULT_CHANNEL_OUT_VALUE)
	std		RAMINDEX_HIGH(ch1_out_value), m_temp

	; init ch0_in_value
	ldi		m_temp, low(DEFAULT_CHANNEL_IN_VALUE)
	std		RAMINDEX_LOW(ch0_in_value), m_temp

	ldi		m_temp, high(DEFAULT_CHANNEL_IN_VALUE)
	std		RAMINDEX_HIGH(ch0_in_value), m_temp

	; init ch1_in_value
	ldi		m_temp, low(DEFAULT_CHANNEL_IN_VALUE)
	std		RAMINDEX_LOW(ch1_in_value), m_temp

	ldi		m_temp, high(DEFAULT_CHANNEL_IN_VALUE)
	std		RAMINDEX_HIGH(ch1_in_value), m_temp

	;** Setup port
	ldi		m_temp, (1<<(CH0_OUTPUT_PIN)) + (1<<(CH1_OUTPUT_PIN))
	out		DDRB, m_temp

	clr		m_temp
	out		PORTB, m_temp

	;** Enable timer inerrupts
	ldi		m_temp, (1<<OCIE0A) + (1<<OCIE0B) + (1<<TOIE0)
	out		TIMSK0, m_temp

	;** Setup timer1
	clr		m_temp
	out		TCCR0A, m_temp

	ldi		m_temp, 1<<CS01
	out		TCCR0B, m_temp

	;** Setup pin change interrupt
	ldi		m_temp, 1<<PCIE
	out		GIMSK, m_temp

	ldi		m_temp, (1<<(CH0_INPUT_PIN)) + (1<<(CH1_INPUT_PIN))
	out		PCMSK, m_temp

	;** Enable interrupts
	sei

forever:
	; ** calculate channel 0 output value
	cli

	; load channel 0 length
	ldd		m_low, RAMINDEX_LOW( ch0_in_value )
	ldd		m_high, RAMINDEX_HIGH( ch0_in_value )

	; load channel 1 length
	ldd		m_low2, RAMINDEX_LOW( ch1_in_value )
	ldd		m_high2, RAMINDEX_HIGH( ch1_in_value )

	sei

	; channel0_out = channel0_in + channel1_in - MIDDLE_VALUE
	add		m_low, m_low2
	adc		m_high, m_high2

	subi	m_low, low( MIDDLE_VALUE )
	sbci	m_high, high( MIDDLE_VALUE )

	; limit output pulse length
	rcall	limit_output_pulse_length

	; update output pulse length
	cli
	std		RAMINDEX_LOW( ch0_out_value ), m_low
	std		RAMINDEX_HIGH( ch0_out_value ), m_high
	sei

	; ** calculate channel 1 output value
	cli

	; load channel 0 length
	ldd		m_low, RAMINDEX_LOW( ch0_in_value )
	ldd		m_high, RAMINDEX_HIGH( ch0_in_value )

	; load channel 1 length
	ldd		m_low2, RAMINDEX_LOW( ch1_in_value )
	ldd		m_high2, RAMINDEX_HIGH( ch1_in_value )

	sei

	; channel0_out = channel0_in - channel1_in + MIDDLE_VALUE
	sub		m_low, m_low2
	sbc		m_high, m_high2

	subi	m_low, low( -MIDDLE_VALUE )
	sbci	m_high, high( -MIDDLE_VALUE )

	; limit output pulse length
	rcall	limit_output_pulse_length

	; update output pulse length
	cli
	std		RAMINDEX_LOW( ch1_out_value ), m_low
	std		RAMINDEX_HIGH( ch1_out_value ), m_high
	sei

	rjmp	forever		;Loop forever

;****************************************************************************
;* Short delay
;*
;* Short delay for jumper detection
;***************************************************************************
short_delay:
	clr		m_temp

sd_loop:
	dec		m_temp
	brne	sd_loop

	ret

;****************************************************************************
;* Limit output value
;*
;* Limits output pulse length (m_low, m_high contains pulse length)
;***************************************************************************
limit_output_pulse_length:
	; check for negative value
	tst		m_high
	brmi	lopl_set_low

	; check for the upper limit
	ldi		m_temp, high(CHANNEL_RESOLUTION)
	cpi		m_low, low(CHANNEL_RESOLUTION)
	cpc		m_high, m_temp
	brsh	lopl_set_high

lopl_add_min_pulse_length:
	; add min pulse length
	subi	m_low, low(-CHANNEL_MIN_PULSE_LENGTH)
	sbci	m_high, high(-CHANNEL_MIN_PULSE_LENGTH)

	; substract 256 (because the first timer wraparound is not counted)
	dec		m_high

	ret

lopl_set_high:
	; set pulse length to the maximum value
	ldi		m_high, high(CHANNEL_RESOLUTION)
	ldi		m_low, low(CHANNEL_RESOLUTION)
	rjmp	lopl_add_min_pulse_length

lopl_set_low:
	clr		m_low
	clr		m_high
	rjmp	lopl_add_min_pulse_length

;****************************************************************************
;* Timer0 CompareA interupt
;*
;* This interrupt generates servo pulse for channel0
;***************************************************************************

;***** Timer0 CompareA Register Variables

.def	ch0_out_temp = r19					; Channel0 output temp variable
.def	ch0_out_temp2 = r20					; Channel0 output temp2 variable

;***** Code

TIM0_COMPA_vect:

	;** Interrupt init

	; store status register
	in		ch0_out_temp, SREG
	push	ch0_out_temp

	; enable other interrupts
	sei

	; get status
	ldd		ch0_out_temp, RAMINDEX( ch0_out_status )
	cpi		ch0_out_temp, 0
	brne	oca_st1

	;** if status is 0
	;(servo pulse start)

	; stabilize pin level (turning off pin chage will modify otherwise)
	sbi		PORTB, CH0_OUTPUT_PIN

	; turn off pin change interrupt enable
	cli
	in		ch0_out_temp, TCCR0A
	andi	ch0_out_temp, ~( (1<<COM0A1) | (1<<COM0A0) )
	out		TCCR0A, ch0_out_temp
	sei

	; check for timeout
	tst		it_pulse_timeout
	brne	oca_normal_pulse_handling

	; check for timeout mode
	tst		m_fail_safe_settings
	brne	oca_normal_pulse_handling

	; timeout occured and timeout mode is middle servo position
	ldi		ch0_out_temp, LOW( CHANNEL_MIN_PULSE_LENGTH + MIDDLE_VALUE - 256 )	; load low byte
	ldi		ch0_out_temp2, HIGH( CHANNEL_MIN_PULSE_LENGTH + MIDDLE_VALUE - 256 )	; load high byte

	rjmp	oca_store_pulse_length

oca_normal_pulse_handling:
	; load pulse length
	ldd		ch0_out_temp, RAMINDEX_LOW( ch0_out_value	)		; load low byte
	ldd		ch0_out_temp2, RAMINDEX_HIGH(ch0_out_value )		; load high byte

oca_store_pulse_length:
	; store pulse length
	std		RAMINDEX_LOW( ch0_out_counter ), ch0_out_temp
	std		RAMINDEX_HIGH(ch0_out_counter), ch0_out_temp2

	rjmp	oca_increment_status

oca_st1:
	cpi		ch0_out_temp, 1
	breq	oca_stat1
	cpi		ch0_out_temp, 4
	brne	oca_st2

oca_stat1:
	;** if status is 1 or 4
	;(servo pulse high or low level)

	; check if counter > 512
	ldd		ch0_out_temp, RAMINDEX_HIGH(ch0_out_counter); load counter high
	cpi		ch0_out_temp, 2
	brlo	oca_counter_lower	

	dec		ch0_out_temp						; decrement counter by 256
	std		RAMINDEX_HIGH(ch0_out_counter), ch0_out_temp

	rjmp	oca_it_cleanup

oca_counter_lower:
	; divide remaing count into two equal parts
	ldd		ch0_out_temp, RAMINDEX_LOW(ch0_out_counter)
	ldd		ch0_out_temp2, RAMINDEX_HIGH(ch0_out_counter)

	lsr		ch0_out_temp2
	ror		ch0_out_temp

	adc		ch0_out_temp2, ch0_out_temp

	; store counter
	std		RAMINDEX_LOW(ch0_out_counter), ch0_out_temp2
	clr		ch0_out_temp2
	std		RAMINDEX_HIGH(ch0_out_counter), ch0_out_temp2

	; setup compare value
	in		ch0_out_temp2, OCR0A
	add		ch0_out_temp2, ch0_out_temp
	out		OCR0A, ch0_out_temp2

	rjmp	oca_increment_status

oca_st2:
	cpi		ch0_out_temp, 2
	brne	oca_st3

	;** if status is 2
	;(preparation for falling edge)

	; set last part of timing
	ldd		ch0_out_temp, RAMINDEX(ch0_out_counter)
	in		ch0_out_temp2, OCR0A
	add		ch0_out_temp, ch0_out_temp2
	out		OCR0A, ch0_out_temp

	; enable pin change
	cli
	in		ch0_out_temp, TCCR0A
	ori		ch0_out_temp, 1<<COM0A1
	out		TCCR0A, ch0_out_temp
	sei

	rjmp	oca_increment_status

oca_st3:
	cpi		ch0_out_temp, 3
	brne 	oca_st5

	;** if status is 3
	;(start pulse delay)

	; stabilize pin level (turning off pin chage will modify otherwise)
	cbi		PORTB, CH0_OUTPUT_PIN

	; turn off pin change
	cli
	in		ch0_out_temp, TCCR0A
	andi	ch0_out_temp, ~( (1<<COM0A1) | (1<<COM0A0) )
	out		TCCR0A, ch0_out_temp
	sei

	; check for timeout
	tst		it_pulse_timeout
	brne	oca_normal_pulse_low

	; check for timeout mode
	tst		m_fail_safe_settings
	breq	oca_normal_pulse_low

	; disable servo output
	ldi		ch0_out_temp, PULSE_OUT_DISABLED_STATUS
	std		RAMINDEX(ch0_out_status), ch0_out_temp
	 
	rjmp	oca_it_cleanup

oca_normal_pulse_low:
	; set low level length (CHANNEL_PULSE_REPEAT_RATIO - ch0_out_value - 512)
	ldi		ch0_out_temp, LOW(CHANNEL_PULSE_REPEAT_RATIO)
	ldd		ch0_out_temp2, RAMINDEX_LOW(ch0_out_value)
	sub		ch0_out_temp, ch0_out_temp2
	std		RAMINDEX_LOW(ch0_out_counter), ch0_out_temp

	ldi		ch0_out_temp, HIGH(CHANNEL_PULSE_REPEAT_RATIO)
	ldd		ch0_out_temp2, RAMINDEX_HIGH(ch0_out_value)
	sbc		ch0_out_temp, ch0_out_temp2
	subi	ch0_out_temp, 2
	std		RAMINDEX_HIGH(ch0_out_counter), ch0_out_temp

	rjmp	oca_increment_status

oca_st5:
	cpi		ch0_out_temp, 5
	brne	oca_disabled_status

	;** if status is 5
	;(preparation for rising edge)

	; set last part of timing
	ldd		ch0_out_temp, RAMINDEX(ch0_out_counter)
	in		ch0_out_temp2, OCR0A
	add		ch0_out_temp, ch0_out_temp2
	out		OCR0A, ch0_out_temp

	; enable pin change
	cli
	in		ch0_out_temp, TCCR0A
	ori		ch0_out_temp, (1<<COM0A0) | (1<<COM0A1)
	out		TCCR0A, ch0_out_temp
	sei
	
	; invalid status
oca_invalid_status:
	; clear status
	clr		ch0_out_temp
	std		RAMINDEX(ch0_out_status), ch0_out_temp

	rjmp	oca_it_cleanup

	; check for disabled status
oca_disabled_status:
	cpi		ch0_out_temp, PULSE_OUT_DISABLED_STATUS
	brne	oca_invalid_status

	; if no more timeout restart output
	tst		it_pulse_timeout
	brne	oca_invalid_status

	rjmp	oca_it_cleanup

oca_increment_status:

	; increment status
	ldd		ch0_out_temp, RAMINDEX(ch0_out_status)
	inc		ch0_out_temp
	std		RAMINDEX(ch0_out_status), ch0_out_temp

	;** Interrupt cleanup
oca_it_cleanup:

	; restore status register
	pop		ch0_out_temp
	out		SREG, ch0_out_temp

	reti

;******************************************************************************
;* Timer0 CompareB interupt
;*
;* This interrupt generates servo pulse for channel1
;******************************************************************************

;***** Timer0 CompareB Register Variables

.def	ch1_out_temp = r21					; Channel0 output temp variable
.def	ch1_out_temp2 = r22					; Channel0 output temp2 variable

;***** Code

TIM0_COMPB_vect:

	;** Interrupt init

	; store status register
	in		ch1_out_temp, SREG
	push	ch1_out_temp

	; enable other interrupts
	sei

	; get status
	ldd		ch1_out_temp, RAMINDEX(ch1_out_status)
	cpi		ch1_out_temp, 0
	brne	ocb_st1

	;** if status is 0
	;(servo pulse start)

	; stabilize pin level (turning off pin chage will modify otherwise)
	sbi		PORTB, CH1_OUTPUT_PIN

	; turn off pin change interrupt enable
	cli
	in		ch1_out_temp, TCCR0A
	andi	ch1_out_temp, ~( (1<<COM0B1) | (1<<COM0B0) )
	out		TCCR0A, ch1_out_temp
	sei

	; check for timeout
	tst		it_pulse_timeout
	brne	ocb_normal_pulse_handling

	; check for timeout mode
	tst		m_fail_safe_settings
	brne	ocb_normal_pulse_handling

	; timeout occured and timeout mode is middle servo position
	ldi		ch1_out_temp, LOW( CHANNEL_MIN_PULSE_LENGTH + MIDDLE_VALUE - 256 )	; load low byte
	ldi		ch1_out_temp2, HIGH( CHANNEL_MIN_PULSE_LENGTH + MIDDLE_VALUE - 256 )	; load high byte

	rjmp	ocb_store_pulse_length

ocb_normal_pulse_handling:
	; load pulse length
	ldd		ch1_out_temp, RAMINDEX_LOW( ch1_out_value	)		; load low byte
	ldd		ch1_out_temp2, RAMINDEX_HIGH(ch1_out_value )		; load high byte

ocb_store_pulse_length:
	; store pulse length
	std		RAMINDEX_LOW( ch1_out_counter ), ch1_out_temp
	std		RAMINDEX_HIGH(ch1_out_counter), ch1_out_temp2

	rjmp	ocb_increment_status

ocb_st1:
	cpi		ch1_out_temp, 1
	breq	ocb_stat1
	cpi		ch1_out_temp, 4
	brne	ocb_st2

ocb_stat1:
	;** if status is 1 or 4
	;(servo pulse high or low level)

	; check if counter > 512
	ldd		ch1_out_temp, RAMINDEX_HIGH(ch1_out_counter)	; load counter high
	cpi		ch1_out_temp, 2
	brlo	ocb_counter_lower	

	dec		ch1_out_temp						; decrement counter by 256
	std		RAMINDEX_HIGH(ch1_out_counter), ch1_out_temp

	rjmp	ocb_it_cleanup

ocb_counter_lower:
	; divide remaing count into two equal parts
	ldd		ch1_out_temp, RAMINDEX_LOW(ch1_out_counter)
	ldd		ch1_out_temp2, RAMINDEX_HIGH(ch1_out_counter)

	lsr		ch1_out_temp2
	ror		ch1_out_temp

	adc		ch1_out_temp2, ch1_out_temp

	; store counter
	std		RAMINDEX_LOW(ch1_out_counter), ch1_out_temp2
	clr		ch1_out_temp2
	std		RAMINDEX_HIGH(ch1_out_counter), ch1_out_temp2

	; setup compare value
	in		ch1_out_temp2, OCR0B
	add		ch1_out_temp2, ch1_out_temp
	out		OCR0B, ch1_out_temp2

	rjmp	ocb_increment_status

ocb_st2:
	cpi		ch1_out_temp, 2
	brne	ocb_st3

	;** if status is 2
	;(preparation for falling edge)

	; set last part of timing
	ldd		ch1_out_temp, RAMINDEX(ch1_out_counter)
	in		ch1_out_temp2, OCR0B
	add		ch1_out_temp, ch1_out_temp2
	out		OCR0B, ch1_out_temp

	; enable pin change
	cli
	in		ch1_out_temp, TCCR0A
	ori		ch1_out_temp, 1<<COM0B1
	out		TCCR0A, ch1_out_temp
	sei

	rjmp	ocb_increment_status


ocb_st3:
	cpi		ch1_out_temp, 3
	brne ocb_st5

	;** if status is 3
	;(start pulse delay)

	; stabilize pin level (turning off pin chage will modify otherwise)
	cbi		PORTB, CH1_OUTPUT_PIN

	; turn off pin change
	cli
	in		ch1_out_temp, TCCR0A
	andi	ch1_out_temp, ~( (1<<COM0B1) | (1<<COM0B0) )
	out		TCCR0A, ch1_out_temp
	sei

	; check for timeout
	tst		it_pulse_timeout
	brne	ocb_normal_pulse_low

	; check for timeout mode
	tst		m_fail_safe_settings
	breq	ocb_normal_pulse_low

	; disable servo output
	ldi		ch1_out_temp, PULSE_OUT_DISABLED_STATUS
	std		RAMINDEX(ch1_out_status), ch1_out_temp
	 
	rjmp	ocb_it_cleanup

ocb_normal_pulse_low:
	; set low level length (CHANNEL_PULSE_REPEAT_RATIO - ch1_out_value - 512)
	ldi		ch1_out_temp, LOW(CHANNEL_PULSE_REPEAT_RATIO)
	ldd		ch1_out_temp2, RAMINDEX_LOW(ch1_out_value)
	sub		ch1_out_temp, ch1_out_temp2
	std		RAMINDEX_LOW(ch1_out_counter), ch1_out_temp

	ldi		ch1_out_temp, HIGH(CHANNEL_PULSE_REPEAT_RATIO)
	ldd		ch1_out_temp2, RAMINDEX_HIGH(ch1_out_value)
	sbc		ch1_out_temp, ch1_out_temp2
	subi	ch1_out_temp, 2
	std		RAMINDEX_HIGH(ch1_out_counter), ch1_out_temp

	rjmp	ocb_increment_status

ocb_st5:
	cpi		ch1_out_temp, 5
	brne	ocb_disabled_status

	;** if status is 5
	;(preparation for rising edge)

	; set last part of timing
	ldd		ch1_out_temp, RAMINDEX_LOW(ch1_out_counter)
	in		ch1_out_temp2, OCR0B
	add		ch1_out_temp, ch1_out_temp2
	out		OCR0B, ch1_out_temp

	; enable pin change
	cli
	in		ch1_out_temp, TCCR0A
	ori		ch1_out_temp, (1<<COM0B0) | (1<<COM0B1)
	out		TCCR0A, ch1_out_temp
	sei

ocb_invalid_status:
	; clear status
	clr		ch1_out_temp
	std		RAMINDEX(ch1_out_status), ch1_out_temp

	rjmp	ocb_it_cleanup

	; check for disabled status
ocb_disabled_status:
	cpi		ch1_out_temp, PULSE_OUT_DISABLED_STATUS
	brne	ocb_invalid_status

	; if no more timeout restart output
	tst		it_pulse_timeout
	brne	ocb_invalid_status

	rjmp	ocb_it_cleanup

ocb_increment_status:

	; increment status
	ldd		ch1_out_temp, RAMINDEX(ch1_out_status)
	inc		ch1_out_temp
	std		RAMINDEX(ch1_out_status), ch1_out_temp

	;** Interrupt cleanup
ocb_it_cleanup:

	; restore status register
	pop		ch1_out_temp
	out		SREG, ch1_out_temp

	reti

;******************************************************************************
;* Timer0 Overflow interupt
;*
;* This interrupt increases timer counter high byte and handles
;* pulse time out counter
;******************************************************************************

;***** Timer0 Overflow Register Variables

.def	t0owf_temp = r3						; Timer0 overflow temp variable


TIM0_OVF_vect:
	; store SREG
	in		t0owf_temp, SREG

	; increment counter high
	inc		it_timer_high

	; decrement pulse timeout if it's higher than zero
	tst		it_pulse_timeout
	breq	to_exit

	dec 	it_pulse_timeout

to_exit:
	out	SREG, t0owf_temp

	reti

;****************************************************************************
;* Calculate Pulse Length Function
;*
;* Calculates and limits pulse length using time stamps of pin changes
;***************************************************************************

;***** Calculate Pulse Length Register Variables

.def	cpl_low1 = r24				; CPL low temp1 variable
.def	cpl_high1 = r25				; CPL high temp1 variable
.def	cpl_low2 = r6				; CPL low temp2 variable
.def	cpl_high2 = r7				; CPL high temp2 variable
.def	cpl_temp = r26				; CPL temp variable
.def	cpl_result_low = r27		; CPL result low variable
.def	cpl_result_high = r28		; CPL result high variable

calculate_pulse_length:
	;**calculate length
	;(falling_edge_time_stamp - rising_edge_time_stamp)
	sub		cpl_low1, cpl_low2
	sbc		cpl_high1, cpl_high2

	;** limit pulse length

	; check for glitch length
	cpi		cpl_low1, low( CHANNEL_MIN_PULSE_LENGTH - GLITCH_TOLERANCE )
	ldi		cpl_temp, high( CHANNEL_MIN_PULSE_LENGTH - GLITCH_TOLERANCE )
	cpc		cpl_high1, cpl_temp

	brlo	cpl_return					; if this is a glitch use previous value

	; check for low pulse length
	cpi		cpl_low1, low(CHANNEL_MIN_PULSE_LENGTH)
	ldi		cpl_temp, high(CHANNEL_MIN_PULSE_LENGTH)
	cpc		cpl_high1, cpl_temp

	brlo	cpl_low_value

	; substact min pulse length
	subi	cpl_low1, low(CHANNEL_MIN_PULSE_LENGTH)
	ldi		cpl_temp, high(CHANNEL_MIN_PULSE_LENGTH)
	sbc		cpl_high1, cpl_temp

	; check for max value
	cpi		cpl_low1, low(CHANNEL_RESOLUTION )
	ldi		cpl_temp, high(CHANNEL_RESOLUTION )
	cpc		cpl_high1, cpl_temp

	brsh	cpl_high_value

	; check calue for glitch limit
	cpi		cpl_low1, low(CHANNEL_RESOLUTION + GLITCH_TOLERANCE )
	ldi		cpl_temp, high(CHANNEL_RESOLUTION + GLITCH_TOLERANCE )
	cpc		cpl_high1, cpl_temp

	brsh	cpl_return

	; make average of this value
	add		cpl_low1, cpl_result_low
	adc		cpl_high1, cpl_result_high

	lsr		cpl_high1									; divide by 2
	ror		cpl_low1

	; store result
	mov		cpl_result_low, cpl_low1
	mov		cpl_result_high, cpl_high1

	rjmp	cpl_return

cpl_high_value:
	; set output value to maximum value
	ldi		cpl_result_low, low( CHANNEL_RESOLUTION )
	ldi		cpl_result_high,  high( CHANNEL_RESOLUTION )

	rjmp	cpl_return

cpl_low_value:
	; set output value to minimum value
	ldi		cpl_result_low, low( CHANNEL_MIN_PULSE_LENGTH )
	ldi		cpl_result_high, high( CHANNEL_MIN_PULSE_LENGTH )

	rjmp	cpl_return

cpl_return:
	ret

;****************************************************************************
;* PIN Change interrupt
;*
;* This interrupt measures input pulse length
;***************************************************************************

;***** PIN Change Register Variables

.def	pc_low = r4						; PIN Change low temp variable
.def	pc_high = r5					; PIN Change high temp variable
.def	pc_temp = r23					; PIN Change temp variable

;***** Code

PCINT0_vect:
	; save SREG
	in		pc_temp, SREG
	push	pc_temp

	; disable pin change interrupt
	in		pc_temp, GIMSK
	andi	pc_temp, ~(1<<PCIE)
	out		GIMSK, pc_temp

	; get timestamp
	mov		pc_high, it_timer_high
	in		pc_low, TCNT0
		
	; handle the case when owerflow occurs
	; between reading low and high bytes
	in		pc_temp, TIFR0
	sbrs	pc_temp, TOV0
	rjmp	pc_l1

	in		pc_low, TCNT0
	inc		pc_high

pc_l1:

	; enable other interrupts
	sei

	;** check input channel0 low to high transition
	ldd		pc_temp, RAMINDEX( ch0_in_status )
	cpi		pc_temp, 0
	brne	pc_ch0_check_for_falling_edge

	sbis	PINB, CH0_INPUT_PIN
	rjmp	pc_ch0_check_for_falling_edge
	
	; rising edge detected

	; store time stamp
	std		RAMINDEX_LOW(ch0_time_stamp), pc_low			; store low byte
	std		RAMINDEX_HIGH(ch0_time_stamp), pc_high		; store high byte

	; increment status
	ldd		pc_temp, RAMINDEX(ch0_in_status)
	inc		pc_temp
	std		RAMINDEX(ch0_in_status), pc_temp

	rjmp	pc_ch1_check_for_rising_edge

	;** check input channel0 high to low transition
pc_ch0_check_for_falling_edge:
	ldd		pc_temp, RAMINDEX(ch0_in_status)
	cpi		pc_temp, 1
	brne	pc_ch1_check_for_rising_edge

	sbic	PINB, CH0_INPUT_PIN
	rjmp	pc_ch1_check_for_rising_edge

	; store data for pulse length calculation
	mov		cpl_low1, pc_low													; falling edge time stamp
	mov		cpl_high1, pc_high

	ldd		cpl_low2, RAMINDEX_LOW(ch0_time_stamp); rising edge time stamp
	ldd		cpl_high2, RAMINDEX_HIGH(ch0_time_stamp)

	ldd		cpl_result_low, RAMINDEX_LOW(ch0_in_value); previous pulse length
	ldd		cpl_result_high, RAMINDEX_HIGH(ch0_in_value)

	rcall	calculate_pulse_length										; call pulse length calculations

	std		RAMINDEX_LOW(ch0_in_value), cpl_result_low					; store result
	std		RAMINDEX_HIGH(ch0_in_value), cpl_result_high

	; restart pulse timeout
	ldi		pc_temp, PULSE_REPEAT_TIMEOUT
	mov		it_pulse_timeout, pc_temp

	; reset status
	clr		pc_temp
	std		RAMINDEX(ch0_in_status), pc_temp

	rjmp	pc_reenable_pin_change_interrupt	


pc_ch1_check_for_rising_edge:
	;** check input channel1 low to high transition
	ldd		pc_temp, RAMINDEX(ch1_in_status)
	cpi		pc_temp, 0
	brne	pc_ch1_check_for_falling_edge

	sbis	PINB, CH1_INPUT_PIN
	rjmp	pc_ch1_check_for_falling_edge
	
	; rising edge detected

	; store time stamp
	std		RAMINDEX_LOW(ch1_time_stamp), pc_low			; store low byte
	std		RAMINDEX_HIGH(ch1_time_stamp), pc_high		; store high byte

	; increment status
	ldd		pc_temp, RAMINDEX(ch1_in_status)
	inc		pc_temp
	std		RAMINDEX(ch1_in_status), pc_temp

	rjmp	pc_reenable_pin_change_interrupt

	;** check input channel1 high to low transition
pc_ch1_check_for_falling_edge:
	ldd		pc_temp, RAMINDEX( ch1_in_status )
	cpi		pc_temp, 1
	brne	pc_reenable_pin_change_interrupt

	sbic	PINB, CH0_INPUT_PIN
	rjmp	pc_reenable_pin_change_interrupt

	; store data for pulse length calculation
	mov		cpl_low1, pc_low													; falling edge time stamp
	mov		cpl_high1, pc_high

	ldd		cpl_low2, RAMINDEX_LOW(ch1_time_stamp)				; rising edge time stamp
	ldd		cpl_high2, RAMINDEX_HIGH(ch1_time_stamp)

	ldd		cpl_result_low, RAMINDEX_LOW(ch1_in_value); previous pulse length
	ldd		cpl_result_high, RAMINDEX_HIGH(ch1_in_value)

	rcall	calculate_pulse_length										; call pulse length calculations

	std		RAMINDEX_LOW(ch1_in_value), cpl_result_low					; store result
	std		RAMINDEX_HIGH(ch1_in_value), cpl_result_high

	; restart pulse timeout
	ldi		pc_temp, PULSE_REPEAT_TIMEOUT
	mov		it_pulse_timeout, pc_temp

	; reset status
	clr		pc_temp
	std		RAMINDEX(ch1_in_status), pc_temp

pc_reenable_pin_change_interrupt:
	in		pc_temp, GIMSK
	ori		pc_temp, 1<<PCIE
	out		GIMSK, pc_temp

	; restore status register
	pop		pc_temp
	out		SREG, pc_temp

	reti



