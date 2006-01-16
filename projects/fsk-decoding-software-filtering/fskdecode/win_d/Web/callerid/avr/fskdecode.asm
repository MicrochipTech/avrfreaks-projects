
;callerID.asm
;
;neil barnes / nailed_barnacle 2003

;this is intended to run on the AVRMega8 at 16MHz


	.include "m8def.inc"


;***************************************************************************
;* most variables live in the registers full time
;***************************************************************************

	.CSEG

					;r1 and r0 are needed by multiply
					;r0 has low byte, r1 high byte
					;r2 used as temp store for 16*16 mul
					;r3 used as temp store


	.def    result0 =	r16     ;partial result for 16*16 mult summed here
	.def	result1 = 	r17	;also accumulator for fir filter
	.def    result2 =	r18     ;
	.def    result3 =	r19     ;high byte

	.def    main0 =		r20     ;Temp variable used by main program
	.def    main1 =		r21     ; "      "      "       "      "
	.def    main2 =		r22     ; "      "      "       "      "
	.def    main3 =		r23     ; "      "      "       "      "

	

;***************************************************************************
;* Port Pin Assignements
;***************************************************************************


	


;***************************************************************************
;* Interrupt vectors
;***************************************************************************


	.org 0
	rjmp	RESET			;reset
	rjmp	RESET			;external int 0
	rjmp	RESET			;external int 1
	rjmp	RESET			;timer 2 compare
	rjmp	RESET			;timer 2 overflow
	rjmp	RESET			;timer 1 capture
	rjmp	sample			;timer 1 compare a
	rjmp	RESET			;timer 1 compare b
	rjmp	RESET			;timer 1 overflow
	rjmp	RESET			;timer 0 overflow
	rjmp	RESET			;serial xfer complete
	rjmp	RESET			;usart rx complete
	rjmp	RESET			;usart data register empty
	rjmp	RESET			;usart tx complete
	rjmp	RESET			;adc conversion complete
	rjmp	RESET			;eeprom ready
	rjmp	RESET			;analogue comparator
	rjmp	RESET			;two-wire serial
	rjmp	RESET			;store program memory ready



	.equ length = 21		;size of buffer
	
	; bit defines for the UART status register
	.equ idle = 1
	.equ rxflag = 2
	.equ start = 3
	.equ run = 4
	.equ framerr = 5
	.equ stop = 6

	.dseg

input:	.byte 2
output:	.byte 2
result:	.byte 1
rx:	.byte 1
status:	.byte 1
clock:	.byte 1
bit:	.byte 1
q:	.byte 1

buffer:	.byte (length*2)	; room here for 21 samples
delay:	.byte 10
;offset:	.byte 2		; this says where we're reading the data offset
nextwrite:	.byte 2	; and this is where the next write occurs

	.cseg


;***************************************************************************
;*
;* MAIN PROGRAM
;*
;***************************************************************************
;* INITAILIZATION
;***************************************************************************


RESET:  

		;we need to set the stack pointer to top of ram
	ldi	Main1,$04
	out	SPH, Main1
	ldi	Main1,$5f
	out	SPL, Main1	;top of ram is $045f
		

;***************************************************************************
;* MAIN LOOP
;***************************************************************************

	; set up an interrupt to look at a port so we can use
	; a pre-fudged test data suite
	; in the real world we will read the adc port
	; using a 16MHz clock and with a 9600 per second sample rate
	; it needs an interrupt every 1667 clocks (strictly 1666.6666...)
	; so we will use timer 1 with no prescale

	ldi     main1,0b11111111        ;set port D bits to outputs
	out     DDRD,main1
	ldi     main1,0b00000000        ;preset  output state
	out     PortD,main1
	ldi     main1,0b00000000        ;set port B to inputs
	out     DDRB,main1
	ldi     main1,0b11111111       	;turn on pullups on inputs
	out     PortB,main1

	ldi	main0,low(1667)
	ldi	main1,high(1667)
	out	ocr1ah,main1
	out	ocr1al,main0		;set timer values for 9600

	ldi     main0,0b00001001
	out     TCCR1B,main0        	;don't use timer prescaler, use CTC mode

	ldi	main0,0b00010000	;M8 uses bit 5 to 
	out     TIMSK,main0             ;enable comparator a interrupt 
	
	;clear the delay and main buffer
	ldi	main0,10
	ldi	ZH,high(delay)
	ldi	ZL,low(delay)
	clr	main1
init_01:
	st	Z+,main1
	dec	main0
	brne	init_01

	ldi	main0,42
	ldi	ZH,high(buffer)
	ldi	ZL,low(buffer)
init_02:
	st	Z+,main1
	dec	main0
	brne	init_02

	sei				;and allow interrupts
	


        ;setup stuff for the software uart
	;it occurs to me that one could pass the data out
	;on a pin and back to the usart as an input
	;but where's the fun in that?      

	clr	main0
	sts	nextwrite,main0
	sts	nextwrite+1,main0
	sts	result,main0

	clr r15

	sts	bit,main0
	sts	clock,main0
	ldi	main0,exp2(idle)
	sts	status,main0
FOREVER: 

	lds	r24,status
	andi	r24,exp2(rxflag)
	breq	forever			;watch for the rxflag to be set
	lds	r24,rx
	out	portd,r24		;output the data
forever_1:
	lds	r24,status
	andi	r24,exp2(rxflag)
	brne	forever_1		;wait for rxflag to reset

	jmp    FOREVER                 ;loop forever



sample:		;we get here when there's a timer1 tick
		;every 104uS or thereabouts
		;read the B port and feed it to the filter
		;routine
		;the main program will look after what's happening
		;to the data itself
	
	in 	main1,pinb
	rcall 	filter
	inc	r15
	reti





	
filter:
	; this is translated from C code
	; it can probably be improved :)
	;
	; called every time a new sample is taken
	; i.e. every 9600th second
	;
	; other frequencies could be used; but the sample
	; rate must be chosen to give an even whole number of
	; samples per bit period e.g. 7200, 9600, 12000
	;
	; 7200 samples/sec = 6 samples/bit and is the lowest that 
	; will reliably work
	;
	; 'thingy' is the sample received from the ADC, though we could
	; get the sample directly in finished code
	; 
	; the system decodes the codec tones by delaying
	; each sample by half a bit length, multiplying the 
	; delayed sample by the inverse of the current sample,
	; and low-pass filtering the result
	

;	input = ((((short)thingy)-0x80)<<8);	// upscale the sample
	
	; this assumes 8-bit unipolar data is being received
	; i.e. maximum negative is 0, maximum positive is 0xff,
	; and zero is 0x80
	; the data is expected to arrive in the main1 register

	ldi	main0,0x80
	sub	main1,main0
	clr	r2
	sts	input,r2
	sts	input+1,main1		; and save the result
;
;	// we delay the input samples_per_bit/2 samples (4 samples)
;	for (q=0; q<5; q++)
;		delay[q] = delay[q+1];

	ldi	main0,4

	ldi	r30,low(delay)
	ldi	r31,high(delay)		;point Z at delay[0]
	ldi	r28,low(delay+2)
	ldi	r29,high(delay+2)	;point Y at delay[1]

fi_0001:
	ld	r0,Y+
	st	Z+,r0
	ld	r0,Y+
	st	Z+,r0			;copy one word
	dec	main0
	brne	fi_0001			;and repeat four times

;
;	// save new sample
;		delay[spb/2] = input;

	;Z should be pointing at delay[4]
	lds	r0,input
	lds	r1,input+1
	st	Z+,r0
	st	Z+,r1
;
;	// now we multiply todays sample with the delayed one
;	ac = (int)delay[0]*(int)input*-1;

	movw	main1:main0,r1:r0
	lds	r0,delay
	lds	r1,delay+1
	movw	main3:main2,r1:r0
	rcall	mul16			;result in result0-3
	ldi	main0,0xff
	ldi	main1,0
	eor	result0,main0		;now multiply by -1
	eor	result1,main0		;the fast way
	eor	result2,main0
	eor	result3,main0		
	inc	main0
	inc	main0
	add	result0,main0		
	adc	result1,main1
	adc	result2,main1
	adc	result3,main1		;should be a touch quicker than
					;calling mul16 again

;
;	// and finally, low pass filter the result
;	b[0] = ac>>15;

	; if we look at the result of a 16*16 signed multiply
	; it turns out there are two sign bits
	; we only want one of them
	; so we'll shift up one rather than down 15

	rol	result1
	rol	result2
	rol	result3			;our result is in result2,3

	; work out where to put it
	; see below - we're emulating a circular buffer

	; we are trying to synchronise two buffers here
	; the buffer itself is in ram
	; the coefficients are in program rom

	; what we are implementing is this:
	; a standard FIR filter sums the products of the filter coefficients
	; and the data, and then shifts the data along one place
	; the next item of data gets added in at the beginning of the data buffer
	;
	; we don't want to do the shift if we can avoid it
	; as it costs 11uS and we don't have time to spare
	;
	; a real DSP chip would have a circular buffer, we emulate it
	; we have two copies of the filter coefficients butted to each other
	;
	; P  Q  R  S  T  P' Q' R' S' T'
	;
	; and one copy of the data buffer
	;
	; A  B  C  D  E
	;
	; we start with a pointer to P', and do the summation
	; AP'+BQ'+CR'+DS'+ET'
	;
	; next time we add new data to replace A
	;
	; F  B  C  D  E 
	;
	; decrement the pointer to T, and do the summation
	;
	; FT+BP'+CQ'+DR'+ES'
	;
	; and so on. When the coefficient filter passes P we aim it at P'
	; and in this way need not move any data

	; buffer[nextwrite] = ac>>15;
 	lds	main0,nextwrite
	clr	main1			;will always be 0

	push	main1
	push	main0			;save nextwrite

	ldi	YL,low(buffer)
	ldi	YH,high(buffer)
	add	YL,main0
	adc	YH,main1		;Y points to buffer[nextwrite]

	st	Y+,result2
	st	Y+,result3		

	; offset = 21-nextwrite;
	ldi	ZL,42			;21 words
	clr	ZH
	sub	ZL,main0
	sbc	ZH,main1		;Z now has offset
		
	; ac = 0;
	clr	result0
	clr	result1
	clr	result2
	clr	result3

	; for (q=0; q<21; q++)
	ldi	main3,20
	mov	r3,main3

	;	ac+= (int)buffer[q]*(int)hi1k2[q+offset];
	ldi	YL,low(buffer)
	ldi	YH,high(buffer)
	ldi	main2,low(hi1k2*2)
	ldi	main3,high(hi1k2*2)
	add	ZL,main2
	adc	ZH,main3

	clr	r2			;this needs to be zero
					;for the multiply

fi_0002:
	ld	main0,Y+
	ld	main1,Y+		;buffer[q]
	lpm	main2,Z+
	lpm	main3,Z+		;hi1k2[q+offset]
					;and both pointers incremented	
	; 16*16->32 bit multiply and accumulate
	; result0-3 += main0,1 * main2,3
	; inlined for speed

	muls	main3,main1
	add	result2,r0
	adc	result3,r1
	mul	main2,main0
	add	result0,r0
	adc	result1,r1
	adc	result2,r2
	adc	result3,r2
	mulsu	main3,main0
	sbc	result3,r2
	add	result1,r0
	adc	result2,r1
	adc	result3,r2
	mulsu	main1,main2
	sbc	result3,r2
	add	result1,r0
	adc	result2,r1
	adc	result3,r2

	dec	r3
	brge	fi_0002			;and do this 21 times
		
	; nextwrite++;
	pop	main0
	pop	main1			;get it back
	inc	main0
	inc	main0			;increment to next word - won't roll over
		
	; if (nextwrite > 20)
	cpi	main0,42
	brne	PC+2

	;	nextwrite = 0;
	clr	main0
	sts	nextwrite,main0
	;sts	nextwrite+1,main1

	; if (ac>0)
	;	result = 1;
	;else
	;	result = 0;
	
	; at this point, r2 is still cleared
	;
	; we need to do a 16-bit compare to set the final result

	
	subi	result2,1
	sbc	result3,r2
	brlt	PC+2
	
	inc	r2		;then make r2 positive

	; from here r2 holds the output of the filter stage
    ; do other stuff as required...
    
    ret

mul16:
	; 16*16->32 bit multiply, signed
	; code from Atmel appnote AVR201
	; multiplicands are in main1/2 and main3/4

	clr 	r2
	muls 	main3,main1
	movw 	result3:result2, r1:r0
	mul 	main2,main0
	movw 	result1:result0, r1:r0
	mulsu 	main3,main0
	sbc 	result3,r2
	add 	result1,r0
	adc 	result2,r1
	adc 	result3,r2
	mulsu	main1,main2
	sbc	result3,r2
	add	result1,r0
	adc	result2,r1
	adc	result3,r2

	; which should leave a nice 32-bit product in result
	ret

mac16:
	; 16*16->32 bit multiply and accumulate
	; result0-3 += main0,1 * main2,3
	clr	r2
	muls	main3,main1
	add	result2,r0
	adc	result3,r1
	mul	main2,main0
	add	result0,r0
	adc	result1,r1
	adc	result2,r2
	adc	result3,r2
	mulsu	main3,main0
	sbc	result3,r2
	add	result1,r0
	adc	result2,r1
	adc	result3,r2
	mulsu	main1,main2
	sbc	result3,r2
	add	result1,r0
	adc	result2,r1
	adc	result3,r2

	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; the static data for the filter coefficients
;
; these are 16-bit binary fractions of the coefficients for a low pass filter
; there are two copies of the filter so we can emulate a circular buffer
; without having to test for end conditions

hi1k2:
	.dw	0,84,0,-284,-691,-796,0,1992,4756,7209,8191,7209,4756,1992,0,-796,-691,-284,0,84,0
	.dw	0,84,0,-284,-691,-796,0,1992,4756,7209,8191,7209,4756,1992,0,-796,-691,-284,0,84,0


