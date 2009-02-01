; September 13, 2008
; last assembled and burned on October 6
;
; hardware specs:
;
; * mega32 - as-is, this will take 1.4% of memory
; there are undoubtedly optimizations which can be done
; * 7-seg LED a-g segments on portb0-6
; * 7-seg LED anodes on porta0-5
; * a 1hz pulse is on pc0, suitable for an LED or just debugging
; * ds32khz TCXO on TOSC
; * minutes switch on int0 / pd2 via an RC debounce
; * hours switch on int1 / pd3 via an RC debounce
; both ints are rising edge, trigger when you release
;
; * switch on portc1 to change from 24 or 12 hour mode
; I used a spdt switch, because I had one in my junk
; box. you can certainly use a spst switch with a pullup

.include "m32def.inc"

.def	remainder	=	r15		;remainder from divide
.def	result		=	r16		;result from divide
.def	dividend	=	r16		;dividend input to divide
.def	divisor		=	r17		;divisor input from divide
.def	dcnt8u		=	r18		;divide loop count

.def	seconds		=	r20		; seconds counter
.def	minutes		=	r21		; minutes counter
.def	hours		=	r22		; hours counter
.def	hours1224	=	r23		; used to manage 12/24 hour mode

.def	working		=	r25		; reg to hold lower/upper segment unconverted value
.def	digit		=	r26		; digit counter, addresses the LED to activate

; interrupt table
; only using reset, int0, int1 and timer2 overflow
; all the rest are reti in case of spurious interrupts

.org 0x0000
	jmp reset		; reset 		; hardware reset
	jmp int0isr		; int0			; minutes set
	jmp int1isr		; int1			; hours set
	jmp noint		; int2
	jmp noint		; timer2 compare 
	jmp t2ovr		; timer2 overflow 
	jmp noint		; timer1 Capture 
	jmp noint		; timer1 CompareA 
	jmp noint		; timer1 CompareB 
	jmp noint		; timer1 Overflow 
	jmp noint		; timer0 Compare 
	jmp noint		; timer0 Overflow 
	jmp noint		; spi xfer complete
	jmp noint		; USART RX complete
	jmp noint		; UDR empty 
	jmp noint		; USART TX complete 
	jmp noint		; ADC conversion Complete 
	jmp noint		; EEPROM ready 
	jmp noint		; analog comparator 
	jmp noint		; TWI 
	jmp noint		; Store Program Memory Ready Handler

reset:
; set up the stack pointer
	ldi r16,high(ramend)
	out sph,r16
	ldi r16,low(ramend)
	out spl,r16

; async clock on timer2 via a ds32khz chip
	ldi r16,(1<<as2)
	out assr,r16

; enable timer2 overflow interrupt
	ldi r16,(1<<toie2)
	out timsk,r16

; set timer2 prescaler to 128, which gives an overflow every second
; 32768 divided by 128 equals 256, a single byte
; which is exactly how wide timer2 is
	ldi r16,(1<<cs20 | 1<<cs22 | 0<<cs21)
	out tccr2,r16

; load a static variable for division routine
; this is the number that the timer counters
; are divided by
	ldi divisor,10

; init time registers, probably superfluous, but what the hey
	ldi seconds,0
	ldi minutes,0
	ldi hours,0

; init output ports 
; cathode segments, and set them low so none are active
	ldi r16,255
	out ddrb,r16
	ldi r16,0
	out portb,r16

; anodes on porta
	ldi r16,255
	out ddra,r16
	ldi r16,0
	out porta,r16

; portc0 as output, to allow 1hz pulse
	sbi	ddrc,0

; int0 and int1 are on portd, enable as input
; and enable pullups. I'm acquiring all of portd
; with this method, but at the moment, it's unused
; anyways
	ldi r16,0
	out ddrd,r16
	ldi r16,255
	out portd,r16
	out pind,r16

; enable int0 and int1
	ldi r16,(1<<int0|1<<int1)
	out gicr,r16

; and set them for trigger on rising-edge
	ldi r16,(1<<isc01 | 1<<isc00 | 1<<isc11 | 1<<isc10)
	out mcucr,r16

; enable global interrupts. 
	sei

; enter the main loop here
loop:

; set the physical LED display we want to activate 
	ldi digit,0

; divide by 10 (and modulo 10)
; dividend is the input register, 
; so I move the seconds into here
	mov dividend,seconds
	call div8u

; remainder is the modulo result, which is the
; 1s digits; move into a working register, 
; and then convert it via a table lookup
	mov working,remainder
	call convert

; delay before activating the next digit
; otherwise you might get flickering displays 
	call delay

; clear the display to prevent ghosting
; of the prior digits from appearing
	ldi r19,255
	out porta,r19
;	call delay

; increment the digit address to activate the next LED
	inc digit

; prepare the tens digit using the same
; process as above.
	mov working,result
	call convert
	call delay
	ldi r19,255
	out porta,r19
;	call delay

	inc digit

; minutes processing routine, same as seconds
	mov dividend,minutes
	call div8u
	mov working,remainder
	call convert
	call delay
	ldi r19,255
	out porta,r19

	inc digit
	mov working,result
	call convert
	call delay
	ldi r19,255
	out porta,r19

	inc digit

; hours processing routine

; the 12/24 switch state is a poll, not an interrupt
; if it were an interrupt, I would need to modify
; the hours register (r22) directly, and would need 
; to maintain state across multiple interrupts. 
; this is just easier
;
; in this manner, just subtract 12 from the hours display
; but the internal state (hours register) still runs at
; 24 hours.

; so the first thing to do is to move the hours 
; into another reg to manipulate. 
; timekeeping is on an interrupt, and can change 
; the hours at any time, so this is a precaution

	mov hours1224,hours

; read the switch
; clear = low = 24 hour mode
; set = high = 12 hour mode
	in r16,pinc
	sbrs r16,1 			; skip next if bit reg set
	jmp twentyfour 			; this gets executed if clear

; if I am this far, I am set for 12 hour mode
; first check for 00 (midnight) hours, because I 
; want midnight to display as 12, not 00
	cpi hours1224,0
	breq itstwelve

; if the hours are less than 13, don't need to do anything
	cpi hours1224,13	
	brlt twentyfour		; branch if less than 13

; otherwise subtract 12 to acquire 12 hour mode
	sbci hours1224,12
	jmp twentyfour		; jmp over the midnight tweak

; this is probably an awkward location, and I suspect
; if I were to analyze the code above, I could use
; better logic. but it works, at a less than 2% memory
; consumption, there's no need to be stingy

itstwelve:
	ldi hours1224,12

; the 12/24 tests are done and midnight is adjusted
; so from here on, the process is the same as the
; seconds and minutes processing, but using the
; working register, hours1223, instead of the "real"
; hours register

twentyfour:
	mov dividend,hours1224
	call div8u
	mov working,remainder
	call convert
	call delay
	ldi r19,255
	out porta,r19

	inc digit
	mov working,result
	call convert
	call delay
	ldi r19,255
	out porta,r19

	jmp loop

; this is the divide routines. these were taken from AVR APPNOTE
; AVR200. I do not claim any ownership on this code. I've only
; modified it to use logical names instead of register names
;
; divide (and mod) by 10
; the divisor is DEFined as R17, and 
; and loaded with decimal 10 during the RESET routine

; the input is in the "dividend" register
; the outputs are: 
; result as my 10s digits
; remainder (modulo) as my 1s digits
; 
; this looks something like this:
; if my input is 38 (seconds, minutes, whatever)
; 38/10 = 3 (result), 38 mod 10 = 8 (remainder)

div8u:	
	sub	remainder,remainder			;clear remainder and carry
	ldi	dcnt8u,9				;init loop seconds
d8u_1:	
	rol	dividend				;shift left dividend
	dec	dcnt8u					;decrement seconds
	brne	d8u_2					;if done
	ret						;return
d8u_2:	
	rol	remainder				;shift dividend into remainder
	sub	remainder,divisor			;remainder = remainder - divisor
	brcc	d8u_3					;if result negative
	add	remainder,divisor			;restore remainder
	clc						;clear carry to be shifted into result
	rjmp	d8u_1					;else
d8u_3:	
	sec						;set carry to be shifted into result
	rjmp	d8u_1

convert:
; this is called after dividing and modulo 
; it converts an input into a table lookup value
; the input value is in the "working" register
; the output is in r0
;
; the documentation states that you need to multiply
; by two to get the byte boundaries. This is *magic*
; so I won't pretend to totally understand

; remember, my input is the "working" register,
; which is in the range of 0-9, so I add that
; to the Z register as the offset of the actual
; table address

	ldi zl,low(segtable*2)
	ldi zh,high(segtable*2)
	add zl,working
	lpm r0,z

; then determine the particular LED to activate
; the input is digit, which is managed in the main loop
; the output is in r1
; the same offset is used as above

	ldi zl,low(digtable*2)
	ldi zh,high(digtable*2)
	add zl,digit
	lpm r1,z

; shove the bits to their output ports
; and return to the main loop 
	out portb,r0
	out porta,r1

	ret

; timer/counter2 overflow interrupt vector
; simple time code, count 0-59,0-59,0-23
t2ovr:
; disable interrupts
	cli

; this enables a 1hz pulse on portc0
	push r16
	in r16,portc
	cpi r16,(1<<pc0)
	breq setbit
	sbi portc,0
	jmp continue
setbit:
	cbi portc,0
continue:
	pop r16

; seconds range from 0-59. use a
; "greater than or equal to" as a test,
; in case a situation arises where the
; seconds might be beyond that range	
;
; if it is greater than or equal to,
; then it is time to look at the minutes,
; otherwise inc the seconds and return from 
; the interrupt
	cpi seconds,59
	brge upmins
	inc seconds
	sei
	reti

; seconds have elapsed past 59, so we
; reset the seconds back to zero, 
; and test the minutes (and increment)
; in the same manner
upmins:
	ldi seconds,0
	cpi minutes,59
	brge uphours
	inc minutes
	sei
	reti

; same thing for the hours, but this
; is only 0-23
uphours:
	ldi minutes,0
	cpi hours,23
	brge resetall
	inc hours
	sei
	reti

; this is when seconds are 59, minutes are 59 and
; hours are 23, so that is basically midnight, roll
; back to zero
resetall:
	ldi seconds,0
	ldi minutes,0
	ldi hours,0
	sei
	reti

; this is a delay routine
; from AVR freaks. Look for a project
; named "Delay LOOPs" by author "maaply"
; dated 2008-07-25
delay:
dly_5000cy: 
	push r17
	push r18
	ldi r17,0xE9
dly_5000cy_l2: 
	ldi r18,0x06
dly_5000cy_l1: 
	dec r18
	brne dly_5000cy_l1
	dec r17
	brne dly_5000cy_l2
	ldi r18,0x17
dly_5000cy_l3: 
	dec r18
	nop
	brne dly_5000cy_l3
	pop r18
	pop r17
	ret

; minutes set switch on int0
int0isr:
; disable interrupts so that the time doesn't
; advance while being set. same as in int1
	cli

; increment the minutes and then test for the ceiling
	inc minutes

; I am comparing greater than or equal to 60 otherwise, if 
; I compare to 59, the advance will hit :58, and rollover to :00
	cpi minutes,60
	brge resetmins
	sei
	reti
resetmins:
	ldi minutes,0
	sei
	reti

; hours set switch on int1
int1isr:
	cli
	inc hours
	cpi hours,24
	brge resethours
	sei
	reti
resethours:
	ldi hours,0
	sei
	reti

; catchall ISR for undefined interrupts
noint:
	reti

;table values for 7 seg LEDs
;there's no reason why these are binary, other than
;as a visual aid
segtable: .db 0b11000000,0b11111001,0b10100100,0b10110000,0b10011001,0b10010010,0b10000010,0b11111000,0b10000000,0b10011000

;table for the LED displays
;digtable: .db 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000

digtable: .db 0b11111110, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111
