;
; A Knight rider light... the 1001nth??????????
;
; Just for fun...  
;
; Written by J.J. Proveniers, 22 June 2003
;
; 8 LEDs connected to PORTA
; They are not simply turned on and off, but also fade out
; Done with 16 step PWM
; To prevent flicker, at least 50Hz update rate is needed times 16=800 ints/seconds, a piece of cake for an AVR :)
; R0..7 contain the current intensity level for each LED
;
; The main loop updates these, and the timer interrupt handler does the PWM and interval timer
;
; It was tested on a 8515/8MHz on a board of another AVR project and this project is just a break. This code should work on a 2313 if you use Port B
; and maybe also on a 1200, but that one doesn't have stack, so a bit of regsiter fiddling is neccesary.(which could also be done of course 
; for the 8515 :))

		.include	"8515def.inc"

		.equ	FADEDELAY=50		; Define how fast fadeouts go, the higher the number the slower it goes
		.equ	MOVETIME=7		; Number of fadeout cycles before next move
		
		.def	FadeCounter=r11		; Fade interval counter
		.def	MoveCounter=r9		; Move Interval counter, clocked by rollover of Fade Interval
		.def	PWM=r10			; Current PWM step
		.def	LedStep=r12		; Current LED step
		.def	Dir=r13			; Current direction (either 1 or -1)

		;

		.org	$0000			; Do vector stuff
		rjmp	reset

		.org	INT0addr		; External Interrupt 0
		reti

		.org	INT1addr		; External Interrupt 1
		reti

		.org	ICP1addr		; Input Capture1 
		reti

		.org	OC1Aaddr		; Output Compare1A 
		reti

		.org	OC1Baddr		; Output Compare1B 
		reti

		.org	OVF1addr		; Overflow1 
		reti

		.org	OVF0addr		; Overflow0 
		rjmp	timer0

		.org	SPIaddr 		; SPI 
		reti

		.org	URXCaddr		; UART Receive Complete 
		reti

		.org	UDREaddr		; UART Data Register Empty  
		reti

		.org	UTXCaddr		; UART Transmit Complete
		reti

		.org	ACIaddr 		; Analog Comparator
		reti

		; Main code starts here

reset:		ldi	r16,high(RAMEND)	; Setup stack
		out	SPH,r16

		ldi	r16,low(RAMEND)
		out	SPL,r16

		ldi	r16,4			; Timer 0 prescaler /256
		out	TCCR0,r16

		ldi	r16,1<<TOIE0		; Enable timer 0 overflow ints
		out	TIMSK,r16

		ldi	r16,255			; Set Port A to all outputs
		out	DDRA,r16

		ldi	r16,14			; Set all LEDs to on
		mov	r0,r16
		mov	r1,r16
		mov	r2,r16
		mov	r3,r16
		mov	r4,r16
		mov	r5,r16
		mov	r6,r16
		mov	r7,r16

		clr	LedStep			; Initialise 

		ldi	r16,1
		mov	Dir,r16

		ldi	r16,16
		mov	MoveCounter,r16

		ldi	r16,FADEDELAY
		mov	fadeCounter,r16		; Load fade out interval counter

		sei				; Now enable int and go

mainloop:	tst	FadeCounter		; Check if it's time to do fadeouts
		brne	mainloop		; 		

		ldi	r16,FADEDELAY
		mov	fadeCounter,r16		; Reload fade out interval counter

		clr	ZL
		clr	ZH
		ldi	r16,8		; Loop trough LED intensity levels
mainloop5:	ld	r17,Z		
		cpi	r17,15		; Is it at max??
		breq	mainloop6	; If not, then don't dec
		tst	r17
		breq	mainloop6	; If off, don't dec either
		dec	r17
mainloop6:	st	Z+,r17		; Store new intensity level, inc pointer
		dec	r16
		brne	mainloop5



mainloop1:	dec	MoveCounter	; Time to move?		
		brne	mainloop	; No, loop back

		ldi	r16,MOVETIME
		mov	MoveCounter,r16
		
		mov	ZL,LedStep	; Turn off current LED		
		clr	ZH
		ldi	r16,10				
		st	Z,r16		; Store new intensity value for this LED which will cause it to fade
	
		add	LedStep,Dir	; Move to next LED (add 1 or -1 stored in Dir)
		mov	r16,LedStep
		
		cpi	r16,7		; Either end reached?
		breq	mainloop3
		cpi	r16,0
		brne	mainloop4

mainloop3:	neg	Dir		; Switch direction		

mainloop4:	mov	ZL,LedStep	; Turn on next LED		
		clr	ZH
		ldi	r16,15		; Max intensity level
		st	Z,r16		; Store new intensity value for this LED which will cause it to fade

		rjmp	mainloop

;
; Timer 0 interrupt handler, called approx 800 times a second
;

timer0:		push	r16
		push	r17
		push	r18	
		push	ZH
		push	ZL

		in	r16,SREG
		push	r16	
	
		ldi	r16,255-10	; Reload counter
		out	TCNT0,r16

		tst	FadeCounter
		breq	timer0_2
		dec	FadeCounter	; Decrement fade counter

timer0_2:	dec	PWM		; Decrement main PWM step counter
		ldi	r16,15
		and	PWM,r16		; Only 16 levels

		clr	ZL		; Loop through all LED intensity levels
		clr	ZH
		
		ldi	r17,1		; Bit mask
		clr	r18		; Current output status for all LEDs

timer0_0:	ld	r16,Z+		; Get intensity level for LED

		tst	r16
		breq	timer0_1	

		cp	r16,PWM		; Compare to current PWM step
		brlo	timer0_1	; If lower turn it on
		or	r18,r17

timer0_1:	lsl	r17		; Shift bit mask
		brne	timer0_0	; When bit mask becomes zero, all LEDs done
		
		com	r18
		out	PORTA,r18	; Update PORT

		pop	r16
		out	SREG,r16

		pop	ZL
		pop	ZH		
		pop	r18
		pop	r17
		pop	r16

		reti
