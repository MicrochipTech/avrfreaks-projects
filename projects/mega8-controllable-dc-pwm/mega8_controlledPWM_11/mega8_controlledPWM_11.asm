; Controlled D/C PWM
; ================================================================
; Fix output frequency of 10kHz at OCA1 (PB1) via FAST mode PWM
; User adjustable duty cycle based on UP/DN pushbutton input:
;  - adjustable from ~10-99%, in 10% increments
; Startup duty cycle=50%
; Will ramp between level/DC changes.
; ---------------------
; ExtCLK = 8Mhz
;   ____________________________
;  |  Rev 1.1	   8-26-06		|
;  |  Jamie K.					|
;  |  slip0n0fall@gmail.com		|
;   ----------------------------

; __Future Improvements__
;   - Display current dyty cycle on 10-segment bar graph
;   - Store PrevLevel in EEPROM, to recall after power cycle
;   - Add button keypress 'repeat'

; ===========================================
;   Rev 1.1 additions:
; ===========================================
; -- Typo on WGM bits; changed from '(1<<WGM13 | 1<<WGM13)' to '(1<<WGM12 | 1<<WGM13)'
;    (Was actually setting mode to PHASE correct, rather than intended FAST)
; -- Added crude sweep, and now updates D/C register based on previous D/C



.include "m8def.inc"
.include "_delays.asm"

.DEF TrashReg  = R16
.DEF TempRegL  = R17
.DEF TempRegH  = R18
.DEF OutLevel  = R19
.DEF PrevLevel = R20

;.DEF _waist_US uses R22

.DEF DutyCycL  = R24
.DEF DutyCycH  = R25

.EQU PWM_PER  = 0x031F
.EQU DUTY_INC = 0x4F		; 10% of PWM_PER

;================================================
; Interupt Vectors
;================================================
.CSEG
.ORG 0x0000
 RJMP Reset				; MAIN
.ORG INT0addr
 RJMP ButtonUP_ISR			; External Interrupt 0
.ORG INT1addr
 RJMP ButtonDN_ISR			; External Interrupt 1
;================================================

Reset:
 		LDI 	TrashReg, HIGH(RAMEND) 		; STACK setup, upper byte
 		OUT 	SPH, TrashReg				; to stack pointer
 		LDI 	TrashReg, LOW(RAMEND)		; lower byte
 		OUT 	SPL, TrashReg

 		RCALL 	PWM_Init					; Initialize first PWM values

 		LDI 	TrashReg, ( 1<<DDB1 )		; ENable PWM output pin
 		OUT 	DDRB, TrashReg				

	  ; Push buttons
		LDI		TrashReg, ( 1<<PD2 ) | ( 1<<PD3 )			; Pins are configured as INPUTS,
		OUT		PORTD, TrashReg								;  write to port to ENable pull-ups
		LDI		TrashReg, ( 1<<ISC01 ) | ( 1<<ISC11 )		; int. on falling edges
		OUT 	MCUCR, TrashReg
		LDI		TrashReg, ( 1<<INT0 ) | ( 1<<INT1 )			; enable ext. inter.
		OUT		GICR, TrashReg

		LDI		OutLevel, 5					; Set our initial starting value
		LDI		PrevLevel, 5
		SEI									; GLOBAL INTERRUPT ENable

Main:
	   	CP		OutLevel, PrevLevel			; Has a change of Level occured?
		BRNE	ChangeLvl					; (br !=0) If so calculate new PWM duty 
		RJMP 	Main

;================================================
; Functions
;================================================
PWM_Init:
 		LDI 	TrashReg, ((1<<COM1A1) | (1<<COM1B1) | (1<<WGM11))	; Set for FAST mode, non-inverting
 		OUT 	TCCR1A, TrashReg									

	 	LDI 	TrashReg, ( 1<<WGM12 | 1<<WGM13 | 1<<CS10 )			; PREscale = 1
	 	OUT 	TCCR1B, TrashReg

	 	LDI 	TempRegH, HIGH(PWM_PER)								; this defines our TOP, or period
	 	LDI 	TempRegL, LOW(PWM_PER)								; NON-ATOMIC WRITE!!
	 	OUT 	ICR1H, TempRegH		 
	 	OUT 	ICR1L, TempRegL        								; defined freq: Mclk/(Scale*(top+1))
	
	 	LDI 	DutyCycH, 0x01	  		
	 	LDI 	DutyCycL, 0x8F	  									; 50% of PWM_PER ~= 0x018F

	 	OUT 	OCR1AH, DutyCycH									; set our start-up D/C
	 	OUT 	OCR1AL, DutyCycL

		RET

ChangeLvl:	
		LDI		TrashReg, DUTY_INC				; We'll use the Duty_Inc to loop
		CLI										; DISable interrupts for safe OCR writes
		CP		OutLevel, PrevLevel				; Compare to determine sweep direction			
		BRSH	SweepUP							; branch if OutLvl >= Prev
 SweepDN:
		_Waiste_us 6000		
		SBIW	DutyCycH:DutyCycL, 1			; Subtract immed from word	
		OUT 	OCR1AH, DutyCycH				; Update new PWM D/C		 
	 	OUT 	OCR1AL, DutyCycL        	
		DEC		TrashReg						; Decrement our counter
		CPI		TrashReg, 0						; Have we hit zero yet?
		BRNE	SweepDN							; If not, keep looping
		DEC		PrevLevel						;  Otherwise, update PrevLevel so we don't sweep again
		SEI										; ENable interrupts
		RJMP 	Main							; Sweep completed
 SweepUP:
		_Waiste_us 6000		
		ADIW	DutyCycH:DutyCycL, 1			; Add immed to word
		OUT 	OCR1AH, DutyCycH				; Update PWM D/C
	 	OUT 	OCR1AL, DutyCycL        		
		DEC		TrashReg						; Decrement our counter
		CPI		TrashReg, 0						; Have we hit zero yet?
		BRNE	SweepUp							; If not, keep looping
		INC		PrevLevel						;  Otherwise, update PrevLevel so we don't sweep again
		SEI										; ENable interrupts
		RJMP 	Main							; Sweep completed




;================================================
; Interupt Service Routines
;================================================
ButtonUP_ISR:
		PUSH 	TrashReg				; Who saves their trash??
		IN  	TrashReg, SREG			; Read SREG
		PUSH	TrashReg				; Store SREG

		CPI		OutLevel, 10			; would an increment = 11?
		BREQ	ExitUP					; if so, skip incrememnt
		INC		OutLevel

 ExitUP:				
		POP		TrashReg				; Retrieve SREG
		OUT		SREG, TrashReg			; Restore SREG
        POP		TrashReg				; Restore Trash
		RETI

;================================================
ButtonDN_ISR:
		PUSH 	TrashReg				
		IN  	TrashReg, SREG			
		PUSH	TrashReg				

		CPI		OutLevel, 1				; would a decrement equal 0?
		BREQ	ExitDN					;   if so ignore, exit
		DEC		OutLevel				; otherwise go ahead and decrement
		RJMP	ExitDN

 ExitDN:
		POP		TrashReg				
		OUT		SREG, TrashReg			
        POP		TrashReg				
		RETI

