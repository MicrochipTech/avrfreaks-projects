;*****************************************************************************
;	This small assembler code changes PIN state on PORTB.
;	Just for practice I wanted to use interrupt from TCNT0.
;	The time for PORTB toggling is set for 0,999sek. This can be changed
;	using PRESCALER and OVF_NR values.
;	In my case Timer/Counter is increased each 32us <1/(CLK/256)>, that causes 
;	timer overflow each 8,192ms (32us*256). Each overflow is counted. 
;	Having exactly 1sec for LEDs state changing, the overflow must happened 
;	122times (0x7A). It gives exactly 0,999424sec.
;
;	This is my first AVR assembler code, so please for understanding
;	Would be appreciate to receive any comments on this or suggestions 
;	to improve. For instance how to write this code more effectively. 
;	
;	=================
;	uC:		AT90S2313
;	CLK:	8MHz
;	=================
;
;	e-mail: jdebs@tlen.pl
;*****************************************************************************

  .nolist   				; 
  .include  "2313def.inc" 	; 
  .list   					; 
  .listmac   				;

  .equ PRESCALER	= 0x04	; Set prescaler CK/256
  .equ OVF_NR		= 0x7A	; The number of Counter overflows

  .cseg   					;	 
  .org 0 					; 
  
  rjmp Reset		 		;

;----------------------------- Interrupt Vectors -------------------------------

  .org INT0addr 			; External Interrupt0 Vector 
  reti   					; Address 
  .org INT1addr 			; External Interrupt1 Vector 
  reti   					; Address 
  .org ICP1addr 			; Input Capture1 Interrupt 
  reti   					; Vector Address 
  .org OC1addr 				; Output Compare1A 
  reti   					; Interrupt Vector Address 
  .org OVF1addr 			; Overflow1 Interrupt Vector 
  reti   					; Address 
  
  .org OVF0addr 			; Overflow0 Interrupt Vector 
  rjmp Interrupt;			; Address 
  
  .org URXCaddr 			; UART Receive Complete 
  reti   					; Interrupt Vector Address 
  .org UDREaddr 			; UART Data Register Empty 
  reti   					; Interrupt Vector Address 
  .org UTXCaddr 			; UART Transmit Complete 
  reti   					; Interrupt Vector Address 
  .org ACIaddr 				; Analog Comparator 
  reti   					; Interrupt Vector Address 
;--------------------------------------------------------------------------------------------- 

Reset:   					; 
  cli   					; stop interrupts
  ldi r16,LOW(RAMEND) 		; stack initiation
  out SPL,r16 				; 
  ldi r16,0xFF 				; PORTB as OUTPUT
  out DDRB,r16 				; all PINs are high "1" = LEDs OFF
  out PORTB, r16		 	; 
  rjmp SetUp				;

;---------------------------- Interrupt handling --------------------------------------------- 

Interrupt:
  push r16					; 
  inc r17					; increment interrupts counter
  cpi r17, OVF_NR			; compare counter value with number of counter overloads
  breq toggleLED			; if equal change PINs state
  pop r16					; if not return from Interrupt procedure
  reti						;	 

toggleLED:					; procedure makes EXOR with actual PIN PORTB state and 0xFF
  ldi r18, 0xFF				; this causes PORTB negation
  in r16, PORTB				;
  eor r16, r18  			;
  out PORTB, r16			;
  clr r17					; clear interrupts counter 
  pop r16					;
  reti						;

SetUp:						;
  sei						; Global interrupt enable
  ldi r16, 0x02				; Timer interrupts unlocked
  out TIMSK, r16			;
  clr r16					; clear r16
  ldi r16, PRESCALER		; set prescaler value
  out TCCR0, r16			; start internal counter

Loop:						;
  nop						; 
  rjmp Loop					; 


