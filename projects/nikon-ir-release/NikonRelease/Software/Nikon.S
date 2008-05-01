#include <avr/io.h>
#include <avr/interrupt.h>

	.global Nikon_Shutter_Release
	.global Wait_Next_Nikon_command

	.global PCINT0_vect
PCINT0_vect:
	reti

	.macro IR_on
	sbi	PORTB-0x20,4
	nop
	nop
	nop
	nop
	nop
	nop
	.endm

	.macro IR_off
	cbi	PORTB-0x20,4
	nop
	nop
	nop
	.endm

; The pulse/pause ratio of the 36kHz carrier frequency is 1/3 or 1/4 which reduces power consumption.
Togel_ir:
	IR_on	
	IR_off
	IR_off	
	ret

Wait_ir:
	IR_off	
	nop
	nop
	nop
	IR_off
	IR_off
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This is the Shutter releas of a Nikon
; 2.0ms on  * 36 Khz =   72 Pulse
; 28ms off  * 36 Khz = 1008 Pulse
; 0.5ms on  * 36 Khz =   18 Pulse
; 1.5ms off * 36 Khz =   54 Pulse
; 0.5ms on  * 36 Khz =   18 Pulse
; 3.5ms off * 36 Khz =  126 Pulse
; 0.5 ms on * 36 Khz =   18 Pulse
; repeated after a 63.0 ms pause = 2268 Pulse


Send500us:
	ldi 	r23,18		; 0.5 ms
Send5:
	rcall Togel_ir
	subi	r23,1
	brne Send5
	ret

Send_n_500us:
	rcall Send500us
	subi r24,1
	brne Send_n_500us
	ret

Wait500us:
	ldi 	r23,18		; 0.5 ms
Wait1:
	rcall Wait_ir
	subi	r23,1
	brne Wait1
	ret

Wait_Next_Nikon_command:
	ldi r24,63*2			; Wait 63 ms before sending next command
Wait_n_500us:
	rcall Wait500us
	subi r24,1
	brne Wait_n_500us
	ret

; This is the Shutter releas of a Nikon
; 2.0ms on  * 36 Khz =   72 Pulse
; 28ms off  * 36 Khz = 1008 Pulse
; 0.5ms on  * 36 Khz =   18 Pulse
; 1.5ms off * 36 Khz =   54 Pulse
; 0.5ms on  * 36 Khz =   18 Pulse
; 3.5ms off * 36 Khz =  126 Pulse
; 0.5 ms on * 36 Khz =   18 Pulse
; repeated after a 63.0 ms pause = 2268 Pulse
Nikon_Shutter_Release:
	ldi	r24,2*2
	rcall Send_n_500us
	ldi	r24,28*2
	rcall Wait_n_500us
	rcall Send500us
	ldi	r24,3
	rcall Wait_n_500us
	rcall Send500us
	ldi	r24,7
	rcall Wait_n_500us
	rcall Send500us
	ret

