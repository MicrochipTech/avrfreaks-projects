; blinky veggklokkething by rolf r bakke  jan 2007

.include "1200def.inc"


.def	sregsave=r0

.def 	rndl=r2			;for the RNG
.def	rndh=r3

.def	q1=r4			;for the PWM gen
.def	q2=r5
.def	q3=r6
.def	q4=r7
.def	q5=r8
.def	q6=r9
.def	qq7=r10
.def	q8=r11
.def	q9=r12
.def	q10=r13
.def	q11=r14
.def	q12=r15

.def	t=r16			;temporary variable used by main

.def	tt=r17			;temporary variable used by isr

.def	lysl=r18		;bit0=klokken1  bit11=klokken12
.def	lysh=r19		

.def	c=r20

.def	i=r21

.def	speed=r22

.def	cntl=r23
.def	cnth=r24

.def	pwmphase=r25		;phase for the PWM gen.

.def	a=r26

.def	led=r27			;led #



.org	0




	rjmp reset
	reti
	rjmp timer0_overflow
	rjmp analog_comparator

reset:

;	ldi t,ramend		;init stack pointer
;	out spl,t



;--- initalise variables

	
	
;---- configure input/output pins

;	ldi t,0b00000000	;discharge capasitor
;	out portb,t
	ldi t,0b11111101
	out ddrb,t
	
;	ldi t,0b00000000
;	out portd,t
	ldi t,0b01111111
	out ddrd,t


;---- configure analog comparator

	ldi t,0b00000011	
	out acsr,t		

;--- setup ISR

	ldi t,0b00000010	;enable timer0 overflow IRQ
	out timsk,t
	out tifr,t		;reset IRQ flag

	ldi t,0x00		;clear counter
	out tcnt0,t
	
	ldi t,0b00000010	;set prescaler
	out tccr0,t
	
	sei






;----- Main

main:	ldi lysl,1		;lamp test
	ldi lysh,0

	ldi i,12

ma1:	rcall output
	lsl lysl
	rol lysh
	
	ldi zl,250
	rcall wms

	dec i
	brne ma1




ma2:	mov t,rndh			;selector
	
	cpi t,0x10
	brsh ma3
	rjmp ranonly

ma3:	cpi t,0x20
	brsh ma4	
	rjmp rndmov

ma4:	cpi t,0x30
	brsh ma6
	rjmp rotate
	
ma6:	cpi t,0x40
	brsh ma7
	rjmp onernd

ma7:	cpi t,0x50
	brsh ma8
	rjmp allrnd

ma8:	cpi t,0x51
	brsh ma9
	rjmp allbit

ma9:	cpi t,0x61
	brsh ma10
	rjmp omg

ma10:	cpi t,0x71
	brsh ma11
	rjmp fadsta

ma11:	cpi t,0xb1
	brsh ma12
	rjmp swirl


ma12:	rjmp ma2

;-----


;-------- OMGLOLPLX

omg:	mov cntl,rndl
	andi cntl,0x0f
	subi cntl,-1
	
	ldi a,5

om2:	ldi i,250

om3:	rcall pwmone
	subi a,-1
	dec i
	brne om3

	ldi i,250
om4:	rcall pwmone
	subi a,1
	dec i
	brne om4

	dec cntl
	brne om2

 	rjmp ma2
	




;----- Swirler
swirl:	mov a,rndl
	
	ldi cntl,0
	mov cnth,rndl
	andi cnth,0x1f
	subi cnth,-1
	ldi i,1

sw1:	dec i
	brne sw3
	ldi i,10

				;check if mode 0 or 1 (random direction or one direction)
	mov zl,a
	andi zl,0x03
	
	cpi zl,0b00000010	;one direction
	breq sw4
	
	cpi zl,0b00000011	;other direction
	breq sw2
	
	mov zl,rndl		;random direction
	lsr zl
	brcs sw2
	
sw4:	dec led			;dec led, wrap at 0
	brpl sw3
	ldi led,11
	rjmp sw3
	
sw2:	inc led			;inc led, wrap at 11
	cpi led,12
	brlo sw3
	clr led
	
sw3:	ldi c,8
	mov zl,led
	rcall setq		;add c to q[zl]
	rcall pwm		;do the pwm
	ldi speed,1		;do the fade
	rcall fade

	subi cntl,1		;more of the outer loop?
	sbci cnth,0
	brcc sw1


fadeout:ldi c,128
fad1:	rcall pwm		;do the pwm
	ldi speed,2		;do the fade
	rcall fade
	dec c
	brne fad1

	rjmp ma2


;-----  Fading stars

fadsta:	

	ldi cntl,0
	ldi cnth,1

fd1:	mov led,rndl	;star generate: get random led		
	andi led,0x0f
	cpi led,12
	brlo fd3
	subi led,4

fd3:	mov i,rndh	;get random delay to next star generate
	andi i,0x3f
	subi i,-5

	ldi c,0
		
fd2:	cpi c,20
	brsh fd8
	mov zl,led
	rcall setq	;add c to q[zl]
fd8:	rcall pwm	;do the pwm
	ldi speed,1	;do the fade
	rcall fade
	
	inc c
	dec i
	brne fd2

	subi cntl,1
	sbci cnth,0
	brcc fd1

	rjmp fadeout



;----  subs for the pwm based generators


setq:	inc zl			;add c to q[zl], limit at 255

	dec zl
	brne se1
	add q1,c
	brcc se1
	sub q1,c

se1:	dec zl
	brne se2
	add q2,c
	brcc se2
	sub q2,c

se2:	dec zl
	brne se3
	add q3,c
	brcc se3
	sub q3,c

se3:	dec zl
	brne se4
	add q4,c
	brcc se4
	sub q4,c

se4:	dec zl
	brne se5
	add q5,c
	brcc se5
	sub q5,c

se5:	dec zl
	brne se6
	add q6,c
	brcc se6
	sub q6,c

se6:	dec zl
	brne se7
	add qq7,c
	brcc se7
	sub qq7,c

se7:	dec zl
	brne se8
	add q8,c
	brcc se8
	sub q8,c

se8:	dec zl
	brne se9
	add q9,c
	brcc se9
	sub q9,c

se9:	dec zl
	brne se10
	add q10,c
	brcc se10
	sub q10,c

se10:	dec zl
	brne se11
	add q11,c
	brcc se11
	sub q11,c

se11:	dec zl
	brne se12
	add q12,c
	brcc se12
	sub q12,c

se12:	ret


	
					;--- PWM fade

fade:	sub q1,speed	
	brcc fa1
	clr q1
fa1:	sub q2,speed	
	brcc fa2
	clr q2
fa2:	sub q3,speed	
	brcc fa3
	clr q3
fa3:	sub q4,speed	
	brcc fa4
	clr q4
fa4:	sub q5,speed	
	brcc fa5
	clr q5
fa5:	sub q6,speed	
	brcc fa6
	clr q6
fa6:	sub qq7,speed	
	brcc fa7
	clr qq7
fa7:	sub q8,speed	
	brcc fa8
	clr q8
fa8:	sub q9,speed	
	brcc fa9
	clr q9
fa9:	sub q10,speed	
	brcc fa10
	clr q10
fa10:	sub q11,speed	
	brcc fa11
	clr q11
fa11:	sub q12,speed	
	brcc fa12
	clr q12
fa12:	ret



;---- one byte for all leds PWM gen

pwmone:	ldi pwmphase,0		

po1:	clr t

	cp pwmphase,a
	brcc po2
	ser t

po2:	mov lysl,t
	mov lysh,t
	
	ldi t,7
po3:	dec t
	brne po3

	rcall output

	inc pwmphase
	brne po1

	ret
	
	
					;---- PWM generator

pwm:	ldi pwmphase,0

pw1:	cp pwmphase,q12
	rol lysl
	cp pwmphase,q11
	rol lysl
	cp pwmphase,q10
	rol lysl
	cp pwmphase,q9
	rol lysl
	cp pwmphase,q8
	rol lysl
	cp pwmphase,qq7
	rol lysl
	cp pwmphase,q6
	rol lysl
	cp pwmphase,q5
	rol lysl
	cp pwmphase,q4
	rol lysl
	rol lysh
	cp pwmphase,q3
	rol lysl
	rol lysh
	cp pwmphase,q2
	rol lysl
	rol lysh
	cp pwmphase,q1
	rol lysl
	rol lysh

	rcall output

	inc pwmphase
	brne pw1

	ret


	

;--- all leds from one bit

allbit:	clr cntl
	clr cnth

	mov speed,rndl
	andi speed,3
	subi speed,-2

ab1:	clr lysl
	clr lysh

	cp cnth,rndl
	brlo ab2
	ser lysl
	ser lysh

ab2:	rcall output
	
	ldi zl,1
	rcall wms
	
	ldi zh,0
	add cntl,speed
	adc cnth,zh
	
	ldi zl,0
	ldi zh,128
	cp cntl,zl
	cpc cnth,zh

	brlo ab1



ab3:	clr lysl
	clr lysh

	cp cnth,rndl
	brlo ab4
	ser lysl
	ser lysh

ab4:	rcall output
	
	ldi zl,1
	rcall wms
	
	sub cntl,speed
	sbci cnth,0
	
	brcc ab3

	rjmp ma2



;---   all random shifting pattern


allrnd:	mov i,rndl
	andi i,0x1f
	subi i,-15

al2:	ldi c,12
	
al1:	mov t,rndl
	cpi t,50
	rol lysl
	rol lysh

	ldi zl,20
	rcall wms
	
	dec c
	brne al1
	
	rcall output
	
	dec i
	brne al2	

	rjmp ma2	
	


;----- reverse one random bit 

onernd: mov i,rndl
	andi i,0x7f
	subi i,-127

on4:	ldi zl,1
	ldi zh,0	
	
on1:	mov t,rndl
	andi t,0x0f
	cpi t,12
	brsh on1
		
	tst t
	breq on3

on2: 	lsl zl
	rol zh

	dec t
	brne on2

on3:	eor lysl,zl
	eor lysh,zh
	
	rcall output	

	ldi zl,50
	rcall wms

	dec i
	brne on4

	rjmp ma2

;--------rotate a bitpattern

rotate:	mov t,rndl		;chose a bitpattern	
	andi t,0x03
	
	brne ro1

	ldi lysl,0b00110011
	ldi lysh,0b00000011
	rjmp ro3

ro1:	dec t
	brne ro2

	ldi lysl,0b00111111
	ldi lysh,0b00000000
	rjmp ro3

ro2:	dec t
	brne ro3

	ldi lysl,0b11000011
	ldi lysh,0b00000000

ro3:	ldi zl,50
	rcall wms

	mov c,rndl
	andi c,0x7f
	subi c,-127	

	mov speed,rndh
	andi speed,0x3f
	subi speed,-20	


	mov t,rndl
	lsr t
	brcc ro4

ro5:	bst lysl,0			;rotate antclockwise
	bld lysh,4
	lsr lysh
	ror lysl

	rcall output

	mov zl,speed
	rcall wms

	dec c
	brne ro5
	
	rjmp ro6


ro4:	lsl lysl			;rotate clockwise
	rol lysh
	bst lysh,4			
	bld lysl,0
	
	rcall output

	mov zl,speed
	rcall wms

	dec c
	brne ro4

ro6:	rjmp ma2



;---- move group in random direction

rndmov:	ldi zl,50
	rcall wms
	
	mov c,rndl
	andi c,0x7f
	subi c,-127	

	mov speed,rndh
	andi speed,0x7f
	subi speed,-20	

	ldi lysl,0b10000111
	ldi lysh,0b00000000

rn1:	mov t,rndl
	lsr t

	brcc rn2


	bst lysl,0			;rotate antclockwise
	bld lysh,4
	lsr lysh
	ror lysl

	rjmp rn3

rn2:	lsl lysl			;rotate clockwise
	rol lysh
	bst lysh,4			
	bld lysl,0
	
rn3:	rcall output

	mov zl,speed
	rcall wms

	dec c
	brne rn1

	rjmp ma2



;----   random 'only one bit on', random delay

ranonly:ldi zl,50
	rcall wms
	
	mov c,rndl
	andi c,0x7f
	subi c,-127	

raa4:	ldi lysl,1
	ldi lysh,0	
	
raa1:	mov t,rndl
	andi t,0x0f
	cpi t,12
	brsh raa1
		
	tst t
	breq raa3

raa2: 	lsl lysl
	rol lysh
	dec t
	brne raa2

raa3:	rcall output	

	mov zl,rndh
	andi zl,0x7f
	subi zl,-10	
	rcall wms

	dec c
	brne raa4
	
	rjmp ma2




;----  subs


output:	mov zl,lysl
	mov zh,lysh

	lsl zl
	rol zh
	lsl zl
	rol zh
	
	andi zh,0x3f

	out portb,zl
	out portd,zh

	ret


;----


wms:	ldi t,249		;wait zl mS
wm1:	nop
	dec t
	brne wm1
	dec zl
	brne wms
	ret

timer0_overflow:		;when discharge is complete
	
	in sregsave,sreg 
	

	ldi tt,0b11111100	;turn on capacitor charge
	out ddrb,tt

	ldi tt,0b00000000	;turn off timer0_overflow IRQ
	out timsk,tt


	ldi tt,0b00011011	;enable analog comparator IRQ and reset IRQ flag
	out acsr,tt		

	ldi tt,0x00		;clear counter
	out tcnt0,tt
	
	ldi tt,0b00000001	;set prescaler
	out tccr0,tt

	
	out sreg,sregsave
	reti
	



analog_comparator:		;when charge is completed

	in sregsave,sreg 

	
	in tt,tcnt0		;update random nr with bit 0 from counter
	ror tt
	rol rndl
	rol rndh

;	in tt,portd		;debug
;	bst rndl,0
;	bld tt,6
;	out portd,tt


	ldi tt,0b11111101	;start discharge
	out ddrb,tt

	ldi tt,0b00000011	;turn off analog comparator IRQ
	out acsr,tt		


	mov tt,rndl		;load counter with 7 lowest bits of rndl
	andi tt,0x7f
	out tcnt0,tt
	
	ldi tt,0b00000001	;set prescaler
	out tccr0,tt

	ldi tt,0b00000010	;enable timer0 overflow IRQ
	out timsk,tt
	out tifr,tt		;reset IRQ flag

	out sreg,sregsave
	reti





;kl1=b2
;kl2=b3
;kl3=b4
;kl4=b5
;kl5=b6
;kl6=b7
;kl7=d0
;kl8=d1
;kl9=d2
;kl10=d3
;kl11=d4
;kl12=d5


;   VCC         VCC
;
;    |           |
;    |           |
;    |           |
;   100KO       1KO
;    |           |
;    |           |
;    +--22KO-----|--------B0
;    |           |
;    |           |
;   0.1uF        +--------B1
;    |           |
;    |           |
;    |         3.9KO
;    |           |
;    |           |
;   GND         GND
 
