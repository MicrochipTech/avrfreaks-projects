.equ front = 0b00000010
.equ rear = 0b00000001
.equ clock_ws = 0b01000000
.equ anticlock_ws = 0b00100000

.def temp = r16
.def speed = r17
.def direction = r18
.def distance = r19
.def dly = r20
.def wait_x_10ms = r21
.def curent_angle = r22
.def hut_direction = r23
.def angle = r24
.def steering = r25
.def turn_time_limit = r26
.def usart_byte = r27
.def max_angle = r28
.def max_light = r29

jmp INIT				;interrupt vector
.org 0x08
jmp TIMER2_OVERFLOW
.org 0x12
jmp TIMER0_OVERFLOW
.org 0x1c
jmp ADC_COMPLETE
.org 0x26
jmp TIMER0_COMPARE_MATCH

INIT:
ldi temp,high(RAMEND)	;Arxikopoihsh stoivas	
out SPH,temp 			
ldi temp,low(RAMEND)
out SPL,temp

clr temp				;Eisodoi-Exodoi
out ddrd,temp
ldi temp, 0b00010000	;Pull up
out portd, temp
ldi temp,0b01111011		
out ddrb,temp
ldi temp,0b10000000		;Pull up
out portb,temp			
clr temp
out ddra, temp

ldi temp,0b00000111		;Timer2 clock freq = 4MHz/1024
out TCCR2,temp

ldi temp,0b10000101		;Energopoihsh ADC
out ADCSR, temp

call INIT_USART			;Energopoihsh tou USART

sei						;Energopoiisi interrupts

jmp MAIN				;Enarksi programmatos
