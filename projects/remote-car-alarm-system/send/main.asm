		.include "tn15def.inc"

.equ cnt		=	57	;57		; T: 1000000/(1024*256*cnt) 
.equ cntj		=	5   ;5
.equ TX433_PORT	=	0			;Transmit pin is PB0

.equ	TxD		=	0			;Transmit pin is PB0

.equ	SIGNATURE_0 = '*'-' '	;
.equ	SIGNATURE_1 = '*'-' '	;	change '*' to aproppriate symbols
.equ	SIGNATURE_2 = '*'-' '	;
.equ	SIGNATURE_3 = '*'-' '	;
.equ	SIGNATURE_4 = '*'-' '	;
.equ	CARNUM_0 = '*'-' '
.equ	CARNUM_1 = '*'-' '
.equ	CARNUM_2 = '*'-' '
.equ	CARNUM_3 = '*'-' '
.equ	CARNUM_4 = '*'-' '
.equ	CARNUM_5 = '*'-' '

;***** Global register variables

.def sr_temp	=	r0
.def temp		= 	r16
.def t_i		= 	r17
.def t_j		= 	r18
.def bitcnt		=	r19			;bit counter
.def Txbyte		=	r20			;Data to be transmitted
.def A		=   r21
.def B		=   r22
.def C		=   r23
.def D		=   r24
.def E		=   r25

; 
.equ	F_CPU 	= 1000000
.equ	BOD 	= 2400	; somehow ir resolves to 2690bps
.equ	bps		= (F_CPU/BOD-23)/6  


.cseg

		.ORG 0
		rjmp RESET
		.ORG 4
		reti
		rjmp ti0_int

		.ORG 10

ti0_int:
		in sr_temp, sreg	; enters every ~.15s

		dec t_i
		brne ti0_skipp
		ldi t_i, cnt	; enters here every 15s

		rjmp ti0_skippp
ti0_skipp:
		rjmp ti0_skip
ti0_skippp:

;########################## gather info

		in D, PINB
		or D,r8
		ldi temp, 0
		mov r8, temp
		ldi temp, 0x1E
		and D, temp
		lsr D
		mov E, D
		swap E
		or D,E
		ldi temp, 0b10100000
		eor D, temp

		sbrc D, 0
		rjmp ti0_emergency
;########################## send packet

		dec t_j
		brne ti0_skip

ti0_emergency:
		ldi t_j, cntj	; enters here every 5min

		ldi C, 7		

ti0_send:
		ldi E, 0xF0

		ldi Txbyte, 0xFF		; syncronization
		rcall putchar
		ldi Txbyte, 0xFF
		rcall putchar
		ldi Txbyte, 0xAA
		rcall putchar
		ldi Txbyte, 0xAA
		rcall putchar
		ldi Txbyte, 0xAA
		rcall putchar
		ldi Txbyte, SIGNATURE_0	; signature
		rcall putchar
		ldi Txbyte, SIGNATURE_1
		rcall putchar
		ldi Txbyte, SIGNATURE_2
		rcall putchar
		ldi Txbyte, SIGNATURE_3
		rcall putchar
		ldi Txbyte, SIGNATURE_4
		rcall putchar
		ldi Txbyte, CARNUM_0	; car number
		rcall putchar
		ldi Txbyte, CARNUM_1
		rcall putchar
		ldi Txbyte, CARNUM_2
		rcall putchar
		ldi Txbyte, CARNUM_3
		rcall putchar
		ldi Txbyte, CARNUM_4
		rcall putchar
		ldi Txbyte, CARNUM_5
		rcall putchar

		mov Txbyte, D
		eor Txbyte, E
		rcall putchar
		inc E
		mov Txbyte, D
		eor Txbyte, E
		rcall putchar
		inc E
		mov Txbyte, D
		eor Txbyte, E
		rcall putchar
		inc E
		mov Txbyte, D
		eor Txbyte, E
		rcall putchar
		inc E
		mov Txbyte, D
		eor Txbyte, E
		rcall putchar
		
		cbi PORTB, TX433_PORT

		rcall delay
		dec C
		brne ti0_send

;##########################

ti0_skip:
		WDR
		ldi temp, 0
		out TCNT0, temp
		out sreg, sr_temp
		reti


RESET:
  in temp, ACSR
  ori temp, 0x80
  out ACSR, temp

  ldi temp, 0x05	; CLK/1024
  out TCCR0, temp
  ldi temp, 0x02
  out TIMSK, temp
  ldi temp, 0
  out TCNT0, temp
  ldi t_i, cnt
  ldi t_j, cntj

;  ldi temp, 0b00001011
;  out WDTCR, temp

  ldi temp, 0
  mov r8, temp
  out PORTB, temp
  sbi DDRB, TX433_PORT

  sei
  
forever:
  in r7, PINB
  or r8, r7
  rjmp forever


;***************************************************************************
delay:
	ldi A, 128
f1:	ldi B, 50
f2:	dec B
	brne f2
	dec A
	brne f1
	ret


;***************************************************************************
;*
;* "putchar"
;*
;* This subroutine transmits the byte stored in the "Txbyte" register
;* The number of stop bits used is set with the sb constant
;*
;* Number of words	:14 including return
;* Number of cycles	:Depens on bit rate
;* Low registers used	:None
;* High registers used	:2 (bitcnt,Txbyte)
;* Pointers used	:None
;*
;***************************************************************************
.equ		sb	=1		;Number of stop bits (1, 2, ...)

putchar:	ldi	bitcnt,9+sb	;1+8+sb (sb is # of stop bits)
		com	Txbyte		;Inverte everything
		sec			;Start bit

putchar0:	brcc	putchar1	;If carry set
		cbi	PORTB, TxD	;    send a '0'
		rjmp	putchar2	;else	

putchar1:	sbi	PORTB,TxD	;    send a '1'
		nop

putchar2:	rcall UART_delay	;One bit delay
		rcall UART_delay

		lsr	Txbyte		;Get next bit
		dec	bitcnt		;If not all bit sent
		brne	putchar0	;   send next
					;else
		WDR
		ret			;   return

;***************************************************************************
;*
;* "UART_delay"
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "b":
;*
;*	3·b + 7 cycles (including rcall and ret)
;*
;* Number of words	:4 including return
;* Low registers used	:None
;* High registers used	:1 (temp)
;* Pointers used	:None
;*
;***************************************************************************


UART_delay:	ldi	temp,bps
UART_delay1:	dec	temp
		brne	UART_delay1
		ret




