;Detects the center key press on a RC5 IR remote control and 
;in turn generates the RC1 format for clicking the canon camera 

;THIS PROJECT WAS DEVELOPED TO AUTOMATE A CAMERA STUDIO.
;A CANON 450DS CAMERA WAS IN USE WHICH USED A IR REMOTE THAT 
;TRANSMITTED RC1 CODE FOR CLICKING. THE IDEA WAS TO USE A REGULAR 
;RC5 TV REMOTE THAT WAS ALREADY USED FOR OTHER ACTVIYIES IN THE 
;STUDIO TO GENERATE THE RC1 CODE FOR THE CENTER BUTTON PRESS
;VIEW THE PICTURES FOR AN ANALYSIS OF THE RC5 FORMAT vs RC1 FORMAT.
;FOLLOW THE COMMENTS FOR AN UNDERSTANDING OF THE FLOW

;DEVELOPED BY P ANAND SAVANTH (tested on 450DS)

.INCLUDE "m8535def.inc"
.LIST
.DEF	mp = R16

.org	$000
	rjmp main


;////////////////////////INTERRUPTS////////////////////////////////
.org	INT0addr
	cli			;DISABLE INTERRUPTS
	call	mdelay
	SBI	PORTC,7		;MARKER
	call	chkpin
	CBI	PORTC,7

	call	ldelay		;SKIP ADDRESS N COMMAND
	
	LDI	R16,8		;READ DTA BYTES
REPT:	CALL	READ	
	DEC	R16
	BRNE	REPT		;ALL BYTES DONE?
	cpi	r21,$C4		;COMPARE AND EXECUTE
	brne	exit
	CBI	PORTA,0		;DETECTED CLICK DISABLE TRANSMISSION
	
	call	ndelay		;transmit RC1 instead of RC5
	call	nldelay	
	call	ndelay	

	call	del		;wait period
	call	del

exit:	SBI	PORTA,0		;enable normal tranmission
	reti			;goback
;///////////////////////////////////MAIN//////////////////////////////
.org	$0100
main:
     	ldi 	r16,low(RAMEND)
     	out 	spl,r16
     	ldi 	r16,high(RAMEND)
     	out 	sph,r16
	ser 	r16
	out 	DDRC,r16 		; Set Port c as output
	out 	DDRA,r16 		; Set Port a as output
	out 	porta,r16 		; Set Port a as output
	ldi	r16,$f0		;enable for falling edge in MCUCR
	out 	DDRD,r16 		; Set Port d as output
	clr 	r16
	OUT	PORTC,R16			
	;out 	DDRD,r16 		; Set Port d as input
	ldi	r16,$02		;enable for falling edge in MCUCR
	out	MCUCR,r16	;configure INT0 for low detect
	ldi	r16,$40		;enable in GICR
	out	GICR,r16
	CLR	R21
	sei
	
here:	rjmp	here		;idle here

;///////////////////////////////////ROUTINES//////////////////////////////

Ldelay: 	LDI   	ZH,8 		; SKip addrs N cmd bytes
   	LDI   	ZL,75
InitSio1:	SBIW   	ZL,1
   	BRNE   	InitSio1
	LDI   	ZL,110
	rjmp 	InitSio2

DELAY: 	LDI   	ZH,0		 ; 1.732ms/2 delay
   	LDI   	ZL,198
InitSio2:	SBIW   	ZL,1
   	BRNE   	InitSio2
	ret

MDELAY: 	LDI   	ZH,1		 ; debounce delay
   	LDI   	ZL,200
InitSio3:	SBIW   	ZL,1
   	BRNE   	InitSio3
	ret

DEL5:  	LDI     ZH,0             		; 0.5MS delay
   	LDI   	ZL,115
InitSio4:	SBIW   	ZL,1
   	BRNE   	InitSio4
	ret

DEL55:   ldi	r19,10		;7.2ms delay (5.5ms Delay for delayed click)
u:	ldi	r18,230
p:	dec	r18
	brne	p
	dec	r19
	brne	u
	ret

READ:	IN	R20,pinD		;input data byte
	SBI	PORTC,7		;MARKER
	ROR	R20		;0
	ROR	R20		;1
	ROR	R20		;2 INTO CARRY
	ROL	R21		;BUILD  BYTE IN R21
	CALL	DELAY		
	CBI	portc,7		;CLEAR MARKER
	call	delay
	RET

Del:	LDI   	ZH,0 		; Wait some time
   	LDI   	ZL,0
itSio:	SBIW   	ZL,1
   	BRNE   	itSio
	ret

chkpin:	IN	R20,pinD		;WAIT FOR DATA
	ROR	R20		;0
	ROR	R20		;1
	ROR	R20		;2 INTO CARRY
	BRCS	down
	rjmp	chkpin
down:	ret	

ndelay:	ldi	r18,14
up:	;ser	r16
	sbi	portd,7
	ldi	r19,4
u1:	dec	r19
	brne	u1
	;clr	r16
	cbi	portd,7
	ldi	r19,4
u2:	dec	r19
	brne	u2
	dec	r18
	brne	up	
	ret

nldelay:	ldi	r19,10
nu:	ldi	r18,225
np:	dec	r18
	brne	np
	dec	r19
	brne	nu
	ret
;///////////////////////////////////////////////////////////////////////////////////////////////
