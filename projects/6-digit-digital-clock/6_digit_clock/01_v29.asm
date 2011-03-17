; clock v2.9
; button PinB0	enter setup/cancel without changes
; button PinB1  increase selected display/select
; button PinB2	select next display/move in menu
; button PinB4	set values to counter of seconds
; internal alarm now uses speaker to generate sound
; external/internal alarm clock may be disabled (until next cycle)
; TIMER0 - display multiplexer TIMER2 - seconds counter
; TIMER 2 starts disabled and gets disabled after countdown, too.
; added function OFF to disable alarm/coutdown. OFF function appears in menu only if countdown/alarm is set.
;  When selected, alarm/coutdown is disabled immeditaly

.include "m8def.inc" ;atmega8 

.DEF	data = R0	;special register for reading of constants from FLASH (characters data here)


.DEF   time_s = R1	;values of actual time
.DEF   time_m = R2
.DEF   time_h = R3


.DEF mode = R17	;register for recognising time settings 0 = no setup, anything greater than 0 = time setup, used also for lihting dot at selected display
.DEF clock_operation = R6 ; 0b00000000 = simple clock,
						  ; 0b00000001 = alarm clock - int. alarm,
						  ; 0b00000011 = alarm clock - ext. alarm,
						  ; 0b00000111 = countdown - int. alarm,
						  ; 0b00001111 = countdown - ext.alarm

.DEF   menu = R4		; defines where values will be saved in  bit 0 set = clock, bit 1 set = alarm clock

.DEF delay_1 = R7
.DEF delay_2 = R14
.DEF delay_3 = R15


.DEF alarm_s = R18
.DEF alarm_m = R22
.DEF alarm_h = R23
.DEF TIM2_OVF_temp = R24
;.DEF alarm_mode = R25
.DEF alarm_mode = R5

;R31 alarm_5		; used for pointer but reusable
;.DEF   TIM2_OVF_temp = R4
;.DEF   alarm_mode = R5
;.DEF alarm_mode = 


;used for various purposes but reusable
;ldi R30,0
;ldi R29,0
;ldi R28,0
;ldi R27,0
;ldi R26,0




.DEF	display_0 = R8		;values to display
.DEF	display_1 = R9
.DEF	display_2 = R10
.DEF	display_3 = R11
.DEF	display_4 = R12
.DEF	display_5 = R13


.DEF temp       = R16         ; Temporary Variable 


.DEF	segment_code = R19	;which segments_to_display
.DEF	display_code = R20	;which display to light
.DEF	number_to_display = R21



.org 0 
   rjmp RESET            ; Reset Handler 
.org OVF2addr            ; OVF2addr = $004 
   rjmp TIM2_OVF          ; Timer2 Overflow Handler 
.org OVF0addr            ; OVF0addr = $009
   rjmp TIM0_OVF          ; Timer0 Overflow Handler 


RESET: 
   ldi temp,high(RAMEND)      ; Main program start 
   out SPH,temp             ; Set Stack Pointer to top of RAM 
   ldi temp,low(RAMEND) 
   out SPL,temp 


   ldi temp, (1<<SE)         ; Sleep Enable 
   out MCUCR, temp       

; ----------- Real time clock Timer2 .-----------

   ;ldi   temp,0x04      ;prescale clk/64 
   ;ldi   temp,0x01      ;no prescale
   ldi   temp,0x05      ; prescale for 1 sec
   out   tccr2,temp      ;for counter2 
   ldi   temp,0x08      ;enable external 
   out   assr,temp      ;crystal 32.768 kHz 
;   ldi temp,(1<<TOIE2)
;   out   timsk,temp      ;for counter2



; ----------- Timer 0,2 ------------               

   ;ldi temp, (1<<CS02)|(0<<CS01)(0<<CS00)		;Timer0 prescaler 256
   ldi temp, (0<<CS02)|(1<<CS01)|(0<<CS00)		;Timer0 prescaler 8
   out TCCR0, temp
   ldi temp, (1<<TOIE0)|(0<<TOIE2)      ; Timer 0 enabled, timer 2 (seconds) disabled for now

   out TIMSK,temp ; in the Timer Interupt Mask Register

;ldi counter_to_sec,0
ldi temp,0

;########### [ configure input ports and output ports for alarm ( 3 and 5 ) ]

ldi	temp,(0<<PORTB0)|(0<<PORTB1)|(0<<PORTB2)|(0<<PORTB4)|(1<<PORTB3)|(1<<PORTB5) ;
out	DDRB,temp ; to data direction register - iput and alarm pins

;ldi	temp,(1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(0<<PORTB4) ;
;out	PORTB,temp ; defined input port D pins (these are the pull-ups now!)

		; enablin pull-up resistors for input pins
sbi PortB,0	; setup mode
sbi PortB,1	; increment of selected display
sbi PortB,2	; select display to increment
sbi PortB,4	; save changes

cbi PortB,5 ; set output of alarm pins to 0
cbi PortB,3

;########### [configure output ports ]




;ldi	temp,(1<<PORTB5)	; output pin for alarm ;-)
;out	DDRB,temp
;cbi PortB,5				;disabled by default


;ldi	temp,(1<<PORTB3)	; output pin for alarm external ;-)
;out	DDRB,temp
;cbi PortB,3				;disabled by default

ldi temp,0xff
out DDRD, temp    ; setting port for switching display to output

;ldi temp,0x00		; ----------- GND!!!
;out PortD, temp

;sbi PortD,0			;setting ports to sink when value 1 - kathodes of segments connected to pins
;sbi PortD,1
;sbi PortD,2
;sbi PortD,3
;sbi PortD,4
;sbi PortD,5
;sbi PortD,6
;sbi PortD,7

ldi temp,0xff
out DDRC, temp    ; setting port for switching display to output

;ldi temp,0x00
;out PortC, temp

;cbi PortC,0			;setting ports to current when value 1 - anodes of segments connected to pins
;cbi PortC,1
;cbi PortC,2
;sbi PortC,3
;sbi PortC,4
;sbi PortC,5




ldi display_code,0b00000001		; which display to start with




   sei                  ; #### Global interrupt enable 

ldi temp,0
mov display_0,temp
mov display_1,temp
mov display_2,temp
mov display_3,temp
mov display_4,temp
mov display_5,temp
mov time_s,temp
mov time_m,temp
mov time_h,temp


clr mode
clr alarm_mode
clr clock_operation

;temp 
;ldi TIM2_OVF_temp,0b00000011	
;mov alarm_mode,TIM2_OVF_temp	; internal alarm


; -------------- MAIN LOOP ------------
main_loop: 

  ; sleep





sbrc alarm_mode,0 ;skips internal alarm clock if not set
rcall alarm_routine


sbis	PINB,0	;if pinB0 grounded, calls time setup
rcall setup


rjmp main_loop

;---------------- TIMER0 OVERFLOW - lighting displays --------------------- 
TIM0_OVF:

	push temp

	cpi mode,1					; if in setup, counting on "background", but
	brsh TIM0_OVF_setup_mode	;don't move data in displays

	mov temp,time_s
	cbr temp,0b11110000
	mov display_0,temp
	mov temp,time_s
	lsr temp
	lsr temp
	lsr temp
	lsr temp
	mov display_1,temp

	mov temp,time_m
	cbr temp,0b11110000
	mov display_2,temp
	mov temp,time_m
	lsr temp
	lsr temp
	lsr temp
	lsr temp
	mov display_3,temp

	mov temp,time_h
	cbr temp,0b11110000
	mov display_4,temp
	mov temp,time_h
	lsr temp
	lsr temp
	lsr temp
	lsr temp
	mov display_5,temp

 		TIM0_OVF_setup_mode:

  rcall light_displays

	pop temp

reti




;---------------- TIMER2 OVERFLOW - seconds counter - increasing/decreasing secs,mins,hrs --------------------- 
TIM2_OVF:

;push temp

mov TIM2_OVF_temp,clock_operation
cbr TIM2_OVF_temp,0b11110011		;leaves only values of count-down parameters
cpi TIM2_OVF_temp,0					; compare if count-down set (non-zero result)
breq seconds_counter
rjmp seconds_counter_down				; otherwise decrease of time



seconds_counter:;label to name it  - not used!
 
	;---	seconds




	inc time_s	;increases by second - display0
	mov TIM2_OVF_temp,time_s
	cbr TIM2_OVF_temp,0b11110000 ;separates lower seconds
	cpi TIM2_OVF_temp,10
	brne seconds_counter_over
	mov TIM2_OVF_temp,time_s
	cbr TIM2_OVF_temp,0b00001111	;if is 10, changing to 0
	mov time_s,TIM2_OVF_temp	
	
	ldi TIM2_OVF_temp,0b00010000	;increases by second - display1
	add time_s,TIM2_OVF_temp
	
	mov TIM2_OVF_temp,time_s 
	cpi TIM2_OVF_temp,0b01100000	; checks for 60 secs
	brne seconds_counter_over
	ldi TIM2_OVF_temp,0
	mov time_s,TIM2_OVF_temp		;setting seconds to 00

	;---	minutes

	inc time_m
	mov TIM2_OVF_temp,time_m
	cbr TIM2_OVF_temp,0b11110000 ;separates lower seconds
	cpi TIM2_OVF_temp,10
	brne seconds_counter_over
	mov TIM2_OVF_temp,time_m
	cbr TIM2_OVF_temp,0b00001111	;if is 10, changing to 0
	mov time_m,TIM2_OVF_temp	
	
	ldi TIM2_OVF_temp,0b00010000	;increases by minute - display3
	add time_m,TIM2_OVF_temp

	mov TIM2_OVF_temp,time_m
	cpi TIM2_OVF_temp,0b01100000	; checks for 60 mins
	brne seconds_counter_over
	ldi TIM2_OVF_temp,0
	mov time_m,TIM2_OVF_temp		;setting minutes to 00

	;---	hours

	inc time_h
	mov TIM2_OVF_temp,time_h
	cpi TIM2_OVF_temp,0b00100100	; compares to 24 hours
		brne seconds_counter_24_check
		ldi TIM2_OVF_temp,0
		mov time_h,TIM2_OVF_temp
		rjmp seconds_counter_over
   			seconds_counter_24_check:

	cbr TIM2_OVF_temp,0b11110000 ;separates lower seconds
	cpi TIM2_OVF_temp,10
	brne seconds_counter_over
	mov TIM2_OVF_temp,time_h
	cbr TIM2_OVF_temp,0b00001111	;if is 10, changing to 0
	mov time_h,TIM2_OVF_temp	
	
	ldi TIM2_OVF_temp,0b00010000	;increases by hour - display5
	add time_h,TIM2_OVF_temp

	mov TIM2_OVF_temp,time_h
	cpi TIM2_OVF_temp,0x24	; checks for 24 hrs
	brne seconds_counter_over
	ldi TIM2_OVF_temp,0
	mov time_h,TIM2_OVF_temp		;setting minutes to 00





seconds_counter_over:


;----- ALARM CLOCK - part of seconds_counter - decided after second is counted
ldi TIM2_OVF_temp,0
cp alarm_mode,TIM2_OVF_temp
brne seconds_counter_alarm_clock_over ; chacks if alarm not enabled already

mov TIM2_OVF_temp,clock_operation	; check if clock_operation set in 0 - no alarm
cpi TIM2_OVF_temp,0
breq seconds_counter_alarm_clock_over ; no alarm set

cbr TIM2_OVF_temp,0b00000011	;check if only last 2 bites are possibly 1
cpi TIM2_OVF_temp,0
brne seconds_counter_alarm_clock_over


cp time_s,alarm_s	;checking if current time is same as alarm time
brne seconds_counter_alarm_clock_over
cp time_m,alarm_m
brne seconds_counter_alarm_clock_over
cp time_h,alarm_h
brne seconds_counter_alarm_clock_over

mov TIM2_OVF_temp,clock_operation
cbr TIM2_OVF_temp,0b11111100
cpi TIM2_OVF_temp,0b00000001
breq seconds_counter_over_alarm_clock_internal
ldi TIM2_OVF_temp,0b00000011	; if not interal, external
mov alarm_mode,TIM2_OVF_temp	; external alarm
rjmp seconds_counter_alarm_clock_over

seconds_counter_over_alarm_clock_internal:
ldi TIM2_OVF_temp,0b00000001	
mov alarm_mode,TIM2_OVF_temp	; internal alarm

seconds_counter_alarm_clock_over:
;pop TIM2_OVF_temp	;restoring original value - for case it is used by other code before calling this IRQ
;pop temp
reti


seconds_counter_down: ; ---- counting down by seconds -----


;check if alrarm not active already - which means also no need of further countdown
ldi TIM2_OVF_temp,0
cp alarm_mode,TIM2_OVF_temp
brne seconds_counter_down_check_over


;	------------ countdown seconds ------------					

	dec time_s
	mov TIM2_OVF_temp,time_s
	cbr TIM2_OVF_temp,0b11110000
	cpi TIM2_OVF_temp,0b00001111	;if it was0 before decrease, will be now 0xF
	brne seconds_counter_down_over

	mov TIM2_OVF_temp,time_s
	cbr TIM2_OVF_temp,0b00001111
	mov time_s,TIM2_OVF_temp
	ldi TIM2_OVF_temp,9
	add time_s,TIM2_OVF_temp
	ldi TIM2_OVF_temp,0b11111001
	cp time_s,TIM2_OVF_temp		;if higher s if 0xf, we whow it must be 59 in end
	brne seconds_counter_down_over
	ldi TIM2_OVF_temp,0b01011001
	mov time_s,TIM2_OVF_temp

;	------------ countdown minutes ------------
	seconds_counter_down_minutes:
	dec time_m
	mov TIM2_OVF_temp,time_m
	cbr TIM2_OVF_temp,0b11110000
	cpi TIM2_OVF_temp,0b00001111	;if it was0 before decrease, will be now 0xF
	brne seconds_counter_down_over

	mov TIM2_OVF_temp,time_m
	cbr TIM2_OVF_temp,0b00001111
	mov time_m,TIM2_OVF_temp
	ldi TIM2_OVF_temp,9
	add time_m,TIM2_OVF_temp
	ldi TIM2_OVF_temp,0b11111001
	cp time_m,TIM2_OVF_temp		;if higher s if 0xf, we whow it must be 59 in end
	brne seconds_counter_down_over
	ldi TIM2_OVF_temp,0b01011001
	mov time_m,TIM2_OVF_temp

;	------------ countdown hours ------------
		
	dec time_h
	mov TIM2_OVF_temp,time_h
	cbr TIM2_OVF_temp,0b11110000
	cpi TIM2_OVF_temp,0b00001111	;if it was0 before decrease, will be now 0xF
	brne seconds_counter_down_over

	mov TIM2_OVF_temp,time_h
	cbr TIM2_OVF_temp,0b00001111
	mov time_h,TIM2_OVF_temp
	ldi TIM2_OVF_temp,9
	add time_h,TIM2_OVF_temp
	ldi TIM2_OVF_temp,0b11111001
	cp time_h,TIM2_OVF_temp		;if higher s if 0xf, we whow it must be 23 in end
	brne seconds_counter_down_over
	ldi TIM2_OVF_temp,0b00100011
	mov time_h,TIM2_OVF_temp


seconds_counter_down_over:




; check for 00:00:00

ldi TIM2_OVF_temp,0
cp time_s,TIM2_OVF_temp
brne seconds_counter_down_check_over
ldi TIM2_OVF_temp,0
cp time_m,TIM2_OVF_temp
brne seconds_counter_down_check_over
ldi TIM2_OVF_temp,0
cp time_h,TIM2_OVF_temp
brne seconds_counter_down_check_over

	rcall counter_disable	


; if time is 00:00:00, terrible things happens! - ACTIVATION
ldi TIM2_OVF_temp,0b00000111
cp clock_operation,TIM2_OVF_temp
brne seconds_counter_down_check_external

ldi TIM2_OVF_temp,0b00000111		;internal alarm
mov alarm_mode,TIM2_OVF_temp
rjmp seconds_counter_down_check_over

seconds_counter_down_check_external:
ldi TIM2_OVF_temp,0b00001111
mov alarm_mode,TIM2_OVF_temp


seconds_counter_down_check_over:
reti

light_displays:

ldi temp,0

cpi display_code,0b00000001		; --- deciding which display to light...
breq light_displays_light_0 

cpi display_code,0b00000010
breq light_displays_light_1

cpi display_code,0b00000100
breq light_displays_light_2

cpi display_code,0b00001000
breq light_displays_light_3

cpi display_code,0b00010000
breq light_displays_light_4

cpi display_code,0b00100000
breq light_displays_light_5



light_displays_light_0:			; ...and moves appropriate number from registry dedicated to individual display	
mov number_to_display,display_0
rjmp light_displays_led_display		;then jumps over rest to light display

light_displays_light_1:
mov number_to_display,display_1
rjmp light_displays_led_display

light_displays_light_2:
mov number_to_display,display_2
rjmp light_displays_led_display

light_displays_light_3:
mov number_to_display,display_3
rjmp light_displays_led_display

light_displays_light_4:
mov number_to_display,display_4
rjmp light_displays_led_display

light_displays_light_5:
mov number_to_display,display_5

light_displays_led_display:		; ---

rcall led_display				;lights appropriate segments on given display


rol display_code				;rotates for next display in another call
cpi display_code,0b01000000		;if rotated to 7th display, returned back to first display (ground)
brne light_displays_no_over
ldi display_code,0b00000001
light_displays_no_over:

ret


led_display:

push zl
push zh

ldi ZL, LOW(characters*2) ;set Low memory pointer for characters
ldi ZH, HIGH(characters*2) ;set High memory pointer for characters

;mov number_to_display_decrement,number_to_display

cpi number_to_display,0   ;compares with 0
breq led_display_zero               ; when 0 , no need to move pointer of data

led_display_db_move1: ;moving to number position in db
inc zl
brne led_display_no_zh
inc zh
led_display_no_zh:
dec number_to_display    ;otherwise loop repeats
brne led_display_db_move1
led_display_zero:

lpm 						;reads data of segments to light from db to registry R0 ( data )
mov segment_code, data		;which segments to light

		;================ [ if display selected in setup mode, adds dot to number ]
		cpse mode,display_code	
		rjmp led_display_not_dot ;checking if lighting display which is selected
		ldi temp,0b10000000		;add dot to data
		add segment_code,temp
		led_display_not_dot:
		;===================================

;out DDRD, segment_code		;lights appropriate segments by grounding cathodes
out PortD, segment_code

ldi temp, 0xff			; inverting value of display code, pin with 0 will sink over PNP transistor
sub temp, display_code
out PortC, temp		; which display to light

pop zl
pop zh
ret




setup:	; [CLOCK CONFIGURATION]
;rcall push_displays ;saving values on display for returning before changing it's values
ldi mode,0b00000001		; sets the configuration mode which will used by lighting displays to not show values of time registry - also used for adding dot to display number
rcall delay200ms	;waiting for button release while entering setup
rcall delay200ms


; -- main menu --



setup_main_menu:

ldi temp,0
cp clock_operation,temp
breq clock_operation_only_set ; if 0, no alarm/coutdown is set - skipping choice of disabling those

rcall write_OFF
ldi temp,0
mov menu,temp			;changes menu mode - necessary for writing in correct registers

	rcall 	delay200ms		; delay for releasing button

; - (disable alarm or countdown by setting clock_operation to 0) -
	setup_main_menu_clock_only:

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp	clock_operation_zero
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_clock_only	;loop

	clock_operation_only_set:

	rcall 	delay200ms		; delay for releasing button after end of loop

rcall write_CLO			;writes CLO on display
ldi temp,0
mov menu,temp			;changes menu mode - necessary for writing in correct registers

; - setup clock -
	setup_main_menu_normal_time:
	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp    setup_clock
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_normal_time	;loop

	rcall 	delay200ms		; delay for releasing button after end of loop

rcall write_ALA
ldi temp,1
mov menu,temp

; - setup alarm -
	setup_main_menu_alarm:
	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp    setup_alarm_i
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_alarm	;loop

	rcall 	delay200ms		; delay for releasing button after end of loop


rcall write_ALA_E
ldi temp,1
mov menu,temp

; - setup alarm e -
	setup_main_menu_alarm_e:
	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp    setup_alarm_e
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_alarm_e	;loop

	rcall 	delay200ms		; delay for releasing button after end of loop

rcall write_COU
ldi temp,0
mov menu,temp

; - setup countdown i -
	setup_main_menu_cou_i:
	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp    setup_countdown_i
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_cou_i	;loop

	rcall 	delay200ms		; delay for releasing button after end of loop

rcall write_COU_E
ldi temp,0
mov menu,temp

; - setup countdown e -
	setup_main_menu_cou_e:
	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~
	sbis	PINB,1
	rjmp    setup_countdown_e
	sbic	PINB,2			;for moving to setup of next menu
	rjmp 	setup_main_menu_cou_e	;loop

	rcall 	delay200ms		; delay for releasing button after end of loop


rjmp setup_main_menu

clock_operation_zero:

clr clock_operation

rjmp clock_operation_only_set

; ----- setting of actual time or alarm clock 

setup_normal_time:
rcall delay200ms

sbrs menu,0				;skip if no time settings
rcall refresh_displays
sbrc menu,0				;skip if no alarm settings
rcall refresh_displays_alarm
	; display 5
	setup_time_hr_hi:


ldi mode,0b00100000		; sets the configuration mode which will used by lighting displays to not show values of time registry - also used for adding dot to display number

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_5_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_hr_hi	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop

	; display 4
ldi mode,0b00010000		; used for chanding data of displaying dot

	setup_time_hr_lo:

	sbis	PINB,0
	rjmp 	setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_4_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_hr_lo	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop

	; display 3
ldi mode,0b00001000		; used for chanding data of displaying dot
	setup_time_min_hi:

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_3_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_min_hi	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop

	; display 2
ldi mode,0b00000100		; used for chanding data of displaying dot
	setup_time_min_lo:

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_2_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_min_lo	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop

	; display 1
ldi mode,0b00000010		; used for chanding data of displaying dot
	setup_time_sec_hi:

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_1_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_sec_hi	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop

	; display 0
ldi mode,0b00000001		; used for chanding data of displaying dot
	setup_time_sec_lo:

	sbis	PINB,0
	rjmp setup_cancel	;~ exiting while not changing anything~

	sbis	PINB,1
	rcall display_0_set

	sbis 	PINB,4			;for moving of set values in counter registry
	rjmp	setup_save_settings

	sbic	PINB,2			;for moving to setup of next display
	rjmp 	setup_time_sec_lo	;loop
	rcall 	delay200ms		; delay for releasing button after end of loop



	rjmp setup_time_hr_hi		;LOOP - returns to display_5
; ----------------------------------------------


setup_save_settings:

	;mov clock_operation,menu ;saves bits of enabled functions for deciding of running them in main loop

	mov temp,display_1		;saving seconds
	lsl temp				;moving low 4 bits to be high 4 bits
	lsl temp
	lsl temp
	lsl temp
	add temp,display_0		;adding lower 4 bits
	sbrs menu,0				; if bit 0 set  don't save in time
	mov time_s,temp
	sbrc menu,0				; if bit 0 cleared don't save in alarm
	mov alarm_s,temp

	mov temp,display_3		;saving minutes
	lsl temp
	lsl temp
	lsl temp
	lsl temp
	add temp,display_2
	sbrs menu,0
	mov time_m,temp
	sbrc menu,0
	mov alarm_m,temp

	mov temp,display_5		;saving hours
	lsl temp
	lsl temp
	lsl temp
	lsl temp
	add temp,display_4
	sbrs menu,0
	mov time_h,temp
	sbrc menu,0
	mov alarm_h,temp

	rcall counter_enable 	;enabling counter of seconds

setup_cancel:	;~ for exiting while not changing anything~
ldi mode,0		;configuration mode ends
rcall delay200ms	;waiting for button release while leaving setup
ret

display_5_set:
	;display_set_hr_hi:
	rcall delay200ms
	inc display_5
	ldi temp,3
	cp display_5,temp
	brne display_set_hr_hi_0
	sub display_5,temp
	display_set_hr_hi_0:

					ldi temp,2
					cp display_5,temp			; if highest display 2, checking if second display lower than 4
					brlo display_set_hr_hi_1				
					
									mov temp,display_4
									cpi temp,4
									brlo display_set_hr_hi_1	; if display_4 lower than 4, no need of changing it - skipping
									clr display_4				; otherwise putting 0
					
					display_set_hr_hi_1:

ret

display_4_set:
	;display_set_hr_lo:
	rcall delay200ms
	inc display_4
	ldi temp,10
	cp display_4,temp
	brne display_set_hr_lo_0
	sub display_4,temp
	display_set_hr_lo_0:
											;civilisation's stupidity strikes back!
					ldi temp,2				;we have to check if there is not "2" on highest display.If so, max "3" is acceptable. 
					cp display_5,temp
					brne display_set_hr_lo_1	; if highest display is not "2", we must skip
					
					ldi temp,4
					cp display_4,temp
					brlo display_set_hr_lo_1	; if lower than 4, we have to skip putting "0"
					clr display_4				; otherwise, we put 0 there	

	display_set_hr_lo_1:						;(X_X)
ret

display_3_set:
	;display_set_min_hi:
	rcall delay200ms
	inc display_3
	ldi temp,6
	cp display_3,temp
	brne display_set_min_hi_0
	sub display_3,temp
	display_set_min_hi_0:
ret

display_2_set:
	;display_set_min_lo:
	rcall delay200ms
	inc display_2
	ldi temp,10
	cp display_2,temp
	brne display_set_min_lo_0
	sub display_2,temp
	display_set_min_lo_0:
ret

display_1_set:
	;display_set_sec_hi:
	rcall delay200ms
	inc display_1
	ldi temp,6
	cp display_1,temp
	brne display_set_sec_hi_0
	sub display_1,temp
	display_set_sec_hi_0:
ret

display_0_set:
	;display_set_sec_lo:
	rcall delay200ms
	inc display_0
	ldi temp,10
	cp display_0,temp
	brne display_set_sec_lo_0
	sub display_0,temp
	display_set_sec_lo_0:
ret

write_ALA:

ldi temp,0x0A
mov display_5,temp
mov display_3,temp
ldi temp,0x11
mov display_4,temp
ldi temp,0x13
mov display_2,temp
mov display_1,temp
mov display_0,temp

ret

write_ALA_E:

rcall write_ALA
ldi temp,0x0E
mov display_1,temp

ret

write_CLO:

ldi temp,0x0C
mov display_5,temp
ldi temp,0x11
mov display_4,temp
ldi temp,0x00
mov display_3,temp
ldi temp,0x13
mov display_2,temp
mov display_1,temp
mov display_0,temp

ret

write_COU:

ldi temp,0x0C
mov display_5,temp
ldi temp,0x00
mov display_4,temp
ldi temp,0x12
mov display_3,temp
ldi temp,0x13
mov display_2,temp
mov display_1,temp
mov display_0,temp

ret

write_COU_E:

rcall write_COU
ldi temp,0x0E
mov display_1,temp

ret

write_OFF:

ldi temp,0x00
mov display_5,temp
ldi temp,0x0f
mov display_4,temp
ldi temp,0x0f
mov display_3,temp
ldi temp,0x13
mov display_2,temp
mov display_1,temp
mov display_0,temp

ret


refresh_displays:

ldi mode,0	;disables configuration mode
rcall delay200ms  ; waits for changing dosplays to values of counter
ldi mode,0b00100000		; sets the configuration mode which will used by lighting displays to not show values of time registry - also used for adding dot to display number

ret

refresh_displays_alarm:

clr display_0
clr display_1
clr display_2
clr display_3
clr display_4
clr display_5

ret


setup_clock:

ldi temp,0b00000000 ; normal clock
mov clock_operation,temp

rjmp setup_normal_time

setup_alarm_i:

ldi temp,0b00000001 ; alarm - internal
mov clock_operation,temp

rjmp setup_normal_time

setup_alarm_e:

ldi temp,0b00000011  ; alarm - external
mov clock_operation,temp

rjmp setup_normal_time

setup_countdown_i:
ldi temp,0b00000111 ; countdown - internal
mov clock_operation,temp

rjmp setup_normal_time

setup_countdown_e:
ldi temp,0b00001111 ; countdown - external
mov clock_operation,temp

rjmp setup_normal_time





alarm_routine:

;deciding alarm type to run
mov temp, alarm_mode
cpi temp, 0b00000001
breq alarm_routine_clock_internal
cpi temp, 0b00000011
breq alarm_routine_clock_external
cpi temp, 0b00000111
breq alarm_routine_clock_internal
cpi temp, 0b00001111
breq alarm_routine_clock_external
rjmp alarm_routine_end ; this line should not happen



;alarm activation itself
alarm_routine_clock_internal:
rcall alarm_internal_start	;internal alarm makes sound
rjmp alarm_routine_end

alarm_routine_clock_external:
rcall alarm_external_start
rjmp alarm_routine_end

alarm_routine_end:
alarm_routine_noalarm:
ret



alarm_external_start:
sbi PortB,3 ; external alarm on
sbic	PINB,0	;if pinB0 grounded, ends alarm
rjmp alarm_external_start
cbi PortB,3 ; external alarm off
rcall delay200ms
clr alarm_mode ;desibles alarm
ret


alarm_internal_start:

ldi temp,255
alarm_internal_start_sound:
rcall alarm_square_generator	;runs square for while
dec temp
brne alarm_internal_start_sound


cbi PortB,5				;then nothing for while
rcall delay200ms
sbis	PINB,0	;if pinB0 grounded, ends alarm sound
rjmp alarm_internal_start_end
rjmp alarm_internal_start
alarm_internal_start_end:
clr alarm_mode
rcall delay200ms
rcall delay200ms
ret


alarm_square_generator:	;terrific sound - square generator

push temp
sbi PortB,5		;up
ldi temp, 255	;delay
alarm_square_generator_loop:	;length of square
dec temp
brne alarm_square_generator_loop
cbi PortB,5					;down
ldi temp, 255				;delay
alarm_square_generator_loop2:;length of square
dec temp
brne alarm_square_generator_loop2
pop temp

ret


counter_disable:

ldi temp, (1<<TOIE0)|(0<<TOIE2)      ; disabling interrupt of seconds counter
out TIMSK,temp ; in the Timer Interupt Mask Register

ret

counter_enable:

ldi temp, (1<<TOIE0)|(1<<TOIE2)      ; disabling interrupt seconds counter
out TIMSK,temp ; in the Timer Interupt Mask Register

ret


; delaying 199998 cycles:
delay200ms:
          ldi  temp, $06
		  mov  delay_1, temp
WGLOOP0:  ldi  temp, $37
		  mov  delay_2, temp
WGLOOP1:  ldi  temp, $C9
		  mov  delay_3, temp
WGLOOP2:  dec  delay_3
          brne WGLOOP2
          dec  delay_2
          brne WGLOOP1
          dec  delay_1
          brne WGLOOP0
ret



; the highest bit is DOT! - characters 0-F
characters:

;0-F without dot
.DB 0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111
.DB 0b01110111,0b01111100,0b00111001,0b01011110,0b01111001,0b01110001

;		H 0x10	  L 0x11	U 0x12		space 0x13
.DB 0b01110110,0b00111000,0b00111110,0b00000000

;0-F with dot
;.DB 0b10111111,0b10000110,0b11011011,0b11001111,0b11100110,0b11101101,0b11111101,0b10000111,0b11111111,0b11101111
;.DB 0b11110111,0b11111100,0b10111001,0b11011110,0b11111001,0b11110001

;SPACE and Space with dot - position 33 and 34
;.DB 0b00000000,0b10000000
	
