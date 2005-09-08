
;-----------------------------------------------------------------------------
; avrblink.asm - very simple test program for AT90S2313.
;   Blinks a LED on pin 16.
;-----------------------------------------------------------------------------
.include "avr2313.inc"

.macro  set_led
	cbi  portd, 5
	sbi  portd, 6
.endm

.macro  clr_led
	cbi  portd, 6
	sbi  portd, 5
.endm

.macro  set_X
	ldi temp2,@0
	st X+,temp2
.endm

.macro SetM_Z
	ldi	ZL,low(@0*2)
	ldi	ZH,high(@0*2)
.endm

.macro rev_dw
	.db high(@0), low(@0)
.endm

.macro  push_X
	push xl
	push xh
.endm

.macro  pop_X
	pop xh
	pop xl
.endm

.equ	MAXTIME = 65535

;------- Global Register Variables

.def  res  = r16
.def  temp3  = r17
.def  temp2  = r18
.def  temp1  = r19  


;------- Registers used by timer
.def  rout  = r20
.def  _sreg  = r22  
;.def  divisor  = r23  
.def  case  = r24
.def  led_div  = r25  



.DSEG
pwm:	
	.dw 0,0,0,0,0,0,0,0,0,0,0

buffer:	.db "hoooooooooooooooooooooooooooooooooooooooooooooooola"
 
;-------- Interrupt Vectors
.CSEG
  rjmp  RESET    ; 0 - Reset Handle
  reti           ; 1 - External Interrupt 0
  reti           ; 2 - External Interrupt 1
  reti           ; 3 - Timer 1 Capture Event
  reti           ; 4 - Timer 1 Compare Match
  rjmp TIMER1_OVERFLOW           ; 5 - Timer 1 Overflow
  reti           ; 6 - Timer 0 Overflow
  reti           ; 7 - Uart Rx Complete
  reti           ; 8 - Uart Data Reg. empty
  reti           ; 9 - Uart Tx Complete
  reti           ; 10 - Analog Comparator



TIMER1_OVERFLOW:
	in _sreg,SREG

	ld rout,Y+
	out TCNT1H, rout
	ld rout,Y+
	out TCNT1L, rout

	out  PORTB,case

	;next servo
	lsl case

	andi rout,1
	brne timer_exit

	ldi YL,LOW(pwm)
	ldi YH,HIGH(pwm)
	ldi case,1		

		
timer_exit:
	out  SREG, _sreg
	reti
	
;-----------------------------------------------------------------
; RESET -
;-----------------------------------------------------------------
RESET:
  clr  temp1        ; temp = 0(clear reg)
  out  GIMSK,temp1  ; disable external interrupt

     ; setup direction of port pins
     ; make sure all ports are set as input, tri-state
  out  PORTB,temp1  ; PORTB = 0
  out  PORTD,temp1  ; PORTD = 0

  ldi  temp1,0xff
  out  DDRB, temp1  ; DDRB = 0
  out  DDRD, temp1  ; DDRD = 0

	; 2313 has 128 bytes of ram between 3DH - DFH,
	; some of the other variants don't have this ram, or have more.
	; we will setup our stack at the top of this.
	ldi  temp1,low(RAMEND)
	out  SPL, temp1
	ldi  temp1,high(RAMEND)
	out  SPL+1, temp1

	;enable timer
	ldi  temp1, 1<<TOIE1
	out TIMSK, temp1

	ldi  temp1, 0  
	out TCCR1A , temp1

	ldi  temp1, 1  
	out TCCR1B , temp1


	;
	;	SET SERIAL PORT
	;
.equ	BAUD = 9600
.equ	Speed = 10

	ldi	temp1, ((Speed * 1000000) / (16 * BAUD)) - 1
	out	UBRR,temp1

	; setup the UART control register to enable receive interrupts
	; and enable both the receive and transmit sides of the UART

	ldi	temp1, (1<<RXCIE)+(1<<RXEN)+(1<<TXEN)
	out	UCR,temp1

	ldi	temp1,(1<<ACD)		; turn off the analog comparator
	out	ACSR,temp1		; to minimize current draw


	;init servo registers
	ldi YL,LOW(pwm)
	ldi YH,HIGH(pwm)
	ldi case,1

	cli

	;source
	SetM_Z s_hello
	rcall print

	SetM_Z  init_values
	;dest
	ldi XL,LOW(pwm)
	ldi XH,HIGH(pwm)
	ldi temp1,(init_values_end-init_values)*2
	rcall mem_cpy

/*
	SetM_Z  init_string

	ldi XL,LOW(buffer)
	ldi XH,HIGH(buffer)

	rcall str_cpy
	rcall decode_X
*/
	sei
	

next1:
	ldi XL,LOW(buffer)
	ldi XH,HIGH(buffer)

	rcall serial_input_loop_X

	rcall print_X
	rcall decode_X

  rjmp  next1

;--------------------------------------------------------
;
;  Decode Commands in X
;
;--------------------------------------------------------
decode_X:
decode_loop:

	ld temp2,X+
	cpi temp2,'.'
	breq decode_done		; yup, exit the loop

	cpi temp2,'s'
	brne error

		ldi ZL,LOW(pwm)
		ldi ZH,HIGH(pwm)

		; get servo number
		rcall number_decode_X

		; Z = Z+s*2
		lsl r0
		rol r1

		add ZL,r0
		adc ZH,r1


		ld temp2,X+
		cpi temp2,'p'
		brne error

			rcall number_decode_X

			rcall mul_by_10

			; store 65535-number
			ldi temp1,0xff
			ldi temp2,0xff
			sub temp1,r0
			sbc temp2,r1

			; high part first
			st Z+,temp2
			st Z+,temp1


	rjmp	decode_loop		; keep sending characters

decode_done:
	push_x
	SetM_Z s_ok
	rcall print
	pop_x
	ret
error:
	push_x
	SetM_Z s_error
	rcall print
	pop_x
	ret
;--------------------------------------------------------
;
;  Get number from X and put it in r1:r0
;
;--------------------------------------------------------
number_decode_X:
	clr r0
	clr r1

number_loop:

	;get next char from buffer
	ld res,X+
	rcall convert_number
	cpi res,255
	breq number_done

	add r0,res
	brbc 0,number_go
	inc r1	
number_go:

	ld res,X
	rcall convert_number
	cpi res,255
	breq number_done


	;temp=temp*10  
	rcall mul_by_10


	rjmp number_loop

number_done:
	ret


;--------------------------------------------------------
;
;  Mul r1:r0 by 10
;
;--------------------------------------------------------
mul_by_10:
	;temp = temp*(8+2) = temp*(4+1)*2
	lsl r0
	rol r1

	mov temp1, r0
	mov temp2, r1

	lsl temp1
	rol temp2

	lsl temp1
	rol temp2

	add r0,temp1
	adc r1,temp2

	ret
;--------------------------------------------------------
;
;  conver res into a number
;
;--------------------------------------------------------
convert_number:
	cpi res,'0'
	brlo is_number_not
	cpi res,'9'+1
	brsh is_number_not

	subi res,'0'
	ret
is_number_not:
	ldi res,255
	ret


;--------------------------------------------------------
;
;  get string from serial port and put it in X
;
;--------------------------------------------------------
serial_input_loop_X:
	push XL
	push XH
get_char:
	sbis USR,RXC
	rjmp get_char

	in temp2,UDR
	st X+,temp2
	cpi temp2,'.'
	brne get_char

	;add a zero at the end
	clr temp2
	st X+,temp2

	pop XH
	pop XL
	ret

;--------------------------------------------------------
;
; transmit a message
;
;--------------------------------------------------------
print_X:
	push XL
	push XH

mloop:	; send the data out

	ld r0,X+
	tst	r0		; is the data zero?
	breq	mdone		; yup, exit the loop

uwait:
	sbis	USR,UDRE	; is uart  ready to xmit?
	rjmp	uwait

	out	UDR,R0		; send the data out serial
	rjmp	mloop		; keep sending characters

mdone:	; wait 1 second before restarting
	pop XH
	pop XL

	ret
	

;--------------------------------------------------------
;
; transmit a message
;
;--------------------------------------------------------
print:
	push XL
	push XH

print_loop:	; send the data out
	lpm			; data at Z => r0
	inc	ZL		; ready Z for next read
	tst	R0		; is the data zero?
	breq print_done		; yup, exit the loop

print_wait:
	sbis	USR,UDRE	; is uart  ready to xmit?
	rjmp	print_wait

	out	UDR,R0		; send the data out serial
	rjmp	print_loop		; keep sending characters

print_done:	
	pop XH
	pop XL

	ret

;---------------------------------------------------------
;	source Z, dest X, count temp1
;---------------------------------------------------------
str_cpy:	

	push XL
	push XH
	push ZL
	push ZH

str_mloop:	; send the data out

	lpm			; data at Z => r0
	st X+,r0
	inc	ZL		; ready Z for next read
	brbc 1,str_caca
	inc zh
str_caca:	;ld r1,Z+	; ready Z for next read

	tst	R0		; is the data zero?
	brne str_mloop		; yup, exit the loop

	pop ZH
	pop ZL
	pop XH
	pop XL

	ret


;---------------------------------------------------------
;	source Z, dest X
;---------------------------------------------------------
mem_cpy:	; transmit a hello world message

	push XL
	push XH
	push ZL
	push ZH

mem_mloop:	; send the data out

	lpm			; data at Z => r0
	st X+,r0

	inc zl
	brbc 1,caca
	inc zh
caca:	;ld r1,Z+	; ready Z for next read
	dec temp1		; is the data zero?
	brne mem_mloop		; yup, exit the loop

	pop ZH
	pop ZL
	pop XH
	pop XL

	ret

/*
init_string:
.db "s0p1000s1p1500s2p2000s3p500s4p2500s5p1500s6p15s7p10.",0
*/

s_hello:
	.db "XServo ready.",0x0a,0x0d,0
s_ok: 
	.db "Ok!",0xa,0xd,0
s_error: 
	.db "Error",0xa,0xd,0




init_values:
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw MAXTIME - ( 15000 ) 
rev_dw 1    ; so it waits 6.55ms
rev_dw 1    ; so it waits 6.55ms
rev_dw 0    ; so it waits 6.55ms and starts from the beginning
init_values_end:

;**** End of File ****