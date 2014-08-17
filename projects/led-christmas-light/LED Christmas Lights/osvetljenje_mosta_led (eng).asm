.device ATtiny13

.equ	SREG	=$3F	
.equ	SPL		=$3D	
.equ	GIMSK	=$3B	
.equ	GICR	=$3B   												 
.equ	GIFR	=$3A	
.equ	TIMSK0	=$39	
.equ	TIFR0	=$38	
.equ	SPMCSR  =$37   												  
.equ	OCR0A	=$36   												
.equ	MCUCR	=$35	;MCU control reg.
.equ	MCUSR	=$34	;MCU status reg.
.equ	TCCR0B	=$33	;biti 2,1 i 0  -  closk select bit				
.equ	TCNT0	=$32	
.equ	OSCCAL	=$31   												   
.equ	TCCR0A	=$2F  												   
.equ	DWDR	=$2E   												   
.equ	OCR0B	=$29   												   
.equ	GTCCR	=$28   												   
.equ	CLKPR	=$26   												   
.equ	WDTCR	=$21   												   
.equ   	EEAR	=$1E   												   
.equ	EEARL	=$1E   												   
.equ	EEDR	=$1D   												   
.equ	EECR	=$1C   												   
.equ	PORTB	=$18	
.equ	DDRB	=$17	
.equ	PINB	=$16	
.equ	PCMSK	=$15	;Pin Change Mask reg.
.equ	DIDR0	=$14	  					
.equ	ACSR	=$08	
.equ	ADMUX	=$07   						
.equ	ADCSRA	=$06    					
.equ	ADCH	=$05                                                   
.equ	ADCL	=$04   												   
.equ	ADCSRB	=$03												   

.def temp	=r16				
.def semafor=r17		
.def tt		=r18		
.def long	=r19			
.def www	=r20
.def qqq	=r21
.def ttemp	=r22


;----------------INTERRUPT VECTOR TABLE-----------------------------------------+
.org	0x000				;Reset, Brown-on, Watchdog, Power on				|
	rjmp	reset															   ;|
.org	0x001				;External Interrupt Request 0		(INT0)			|
	reti																	   ;|
.org	0x002				;Pin Change Interrupt Request 0		(PCINT0)		|
	reti																	   ;|
.org	0x003				;Timer/Counter Overflow				(TIM0_OVF)		|
	rjmp overtake_taimer													   ;|
.org	0x004				;EEPROM Ready										|
	reti																	   ;|
.org	0x005				;Analog Comparator									|
	reti																	   ;|
.org	0x006				;Timer/Counter Compare Match A						|
	reti																	   ;|
.org	0x007				;Timer/Counter Compare Match B						|
	reti																	   ;|
.org	0x008				;Watchdog Time-out									|
	reti																	   ;|
.org	0x009				;ADC Conversion Complete							|
	reti																	   ;|
;-------------------------------------------------------------------------------+

	reset:						
		cli						
		nop		
				
		clr semafor
		clr tt		
		clr www
		clr qqq
							
		ldi temp, 0x9F		
		out SPL, temp		
							
		ldi temp, 0b00000001
		out DDRB, temp		
		ldi temp, 0x80		
		out ACSR, temp		
							
		ldi temp, 0b00000011
		out TCCR0A, temp		
		ldi temp, 0b00000010	
		out TCCR0B, temp		
		ldi temp, 0b00000010	
		out TIMSK0, temp		
	
;	For PWM mod.using clk/8 (clkdev its set), internal clock 4,8MHz.
;	Timer clock time:  8*256*(4/4.8)*1us=1.706ms (585Hz)  CSxx = 0 1 0
;	Modulation "Fast PWM", count to TOP, update in TOP, WGMxx =  0 1 1

	sei																
	nop
master_program:						

	;input status pins PORTB for select LED programs
	in temp, PINB			
	ldi ttemp, 0b00011110	
	and ttemp, temp			
	ldi temp, 0b00011110	
	eor	ttemp, temp			
	lsr ttemp				

	;select programs
select_program_1:
	ldi temp, 0x00
	cp temp, ttemp
	brne select_program_2
	rcall program_01		;jump to program 01
	rjmp master_program
select_program_2:
	ldi temp, 0x01
	cp temp, ttemp
	brne select_program_3
	rcall program_02		;jump to program 02
	rjmp master_program
select_program_3:
	ldi temp, 0x02
	cp temp, ttemp
	brne select_program_4
	rcall program_03		;jump to program 03
	rjmp master_program
select_program_4:
	ldi temp, 0x03
	cp temp, ttemp
	brne select_program_5
	rcall program_04		;jump to program 04
	rjmp master_program
select_program_5:
	ldi temp, 0x04
	cp temp, ttemp
	brne select_program_6
	rcall program_05		;jump to program 05
	rjmp master_program
select_program_6:
	ldi temp, 0x05
	cp temp, ttemp
	brne select_program_7
	rcall program_06		;jump to program 06
	rjmp master_program
select_program_7:
	ldi temp, 0x06
	cp temp, ttemp
	brne select_program_8
	rcall program_07		;jump to program 07
	rjmp master_program
select_program_8:
	ldi temp, 0x07
	cp temp, ttemp
	brne select_program_9
	rcall program_08		;jump to program 08
	rjmp master_program
select_program_9:
	ldi temp, 0x08
	cp temp, ttemp
	brne select_program_10
	rcall program_09		;jump to program 09
	rjmp master_program
select_program_10:
	ldi temp, 0x09
	cp temp, ttemp
	brne select_program_11
	rcall program_10		;jump to program 10
	rjmp master_program
select_program_11:
	ldi temp, 0x0A
	cp temp, ttemp
	brne select_program_12
	rcall program_11		;jump to program 11
	rjmp master_program
select_program_12:
	ldi temp, 0x0B
	cp temp, ttemp
	brne select_program_13
	rcall program_12		;jump to program 12
	rjmp master_program
select_program_13:
	ldi temp, 0x0C
	cp temp, ttemp
	brne select_program_14
	rcall program_13		;jump to program 13
	rjmp master_program
select_program_14:
	ldi temp, 0x0D
	cp temp, ttemp
	brne select_program_15
	rcall program_14		;jump to program 14
	rjmp master_program
select_program_15:
	ldi temp, 0x0E
	cp temp, ttemp
	brne select_program_16
	rcall program_15		;jump to program 15
	rjmp master_program
select_program_16:
	rcall program_16		;jump to program 16
	rjmp master_program


program_01:
	rcall seq_1	;	/-||||-\
	rcall seq_1	;	/-||||-\
	ldi long, 0x04
	rcall pause_led	
	rcall seq_4	;	/\/\/\/\
	rcall seq_2	;	////-
	rcall seq_3	;	||||   | | | |     |   |   |   |
	rcall seq_4	;	/\/\/\/\
	rcall seq_5	;	\\\\
	ldi long, 0x2F
	rcall pause_led
	ret

program_02:
	rcall seq_3_1	;|||| //// |||| \\\\
	rcall seq_6	
	rcall seq_3_1	
	rcall seq_5	
	ret

program_03:
	rcall seq_4	;/\/\/\/\   |  |  |  | 
	rcall seq_3_4
	ret

program_04:
	rcall seq_6	;////
	rcall seq_6	;////
	rcall seq_4	;/\/\/\/\
	rcall seq_5	;\\\\
	rcall seq_5	;\\\\
	ret

program_05:
	rcall seq_1	;	/-||||-\
	ret

program_06:
	rcall seq_4_1	; //  /  /    /    /    /  /  
	rcall seq_4_2
	rcall seq_4_3
	rcall seq_4_2
	ret

program_07:				;/\/\/\ / \ / \  /   \   /   \   / \ / \ 
	rcall seq_4_1
	rcall seq_5_1
	rcall seq_4_2
	rcall seq_5_2
	rcall seq_4_3
	rcall seq_5_3
	rcall seq_4_2
	rcall seq_5_2
	ret

program_08:
	rcall seq_5_1	; \\  \  \    \    \    \  \  
	rcall seq_5_2
	rcall seq_5_3
	rcall seq_5_2
	ret

program_09:
	rcall seq_3_1	;||||
	ret

program_10:
	rcall seq_3_3	;|  |  |  |
	ret

program_11:
	rcall seq_3_2	;||||     |  |  |  |  |
	rcall seq_3_4
	ret

program_12:
	rcall seq_3_1	;|||   |  |  |      |     |     |      |  |  |   |||
	rcall seq_3_2
	rcall seq_3_3
	rcall seq_3_4
	rcall seq_3_3
	rcall seq_3_2	
	ret

program_13:
	rcall seq_6	;////
	ret

program_14:
	rcall seq_5	;\\\\
	ret

program_15:
	rcall seq_6	;////
	rcall seq_5	;\\\\
	ret

program_16:
	rcall seq_6	;////
	rcall seq_6	;////
	rcall seq_5	;\\\\
	rcall seq_5	;\\\\
	ret



seq_1:						;		/-||||-\
	ldi long, 0x04 
	rcall pwm_led_rise
		ldi long, 0x3F
		rcall led_power_on

			ldi ttemp, 0x0F
			ldi long, 0x02
			rcall pulse_led

		ldi long, 0x3F
		rcall led_power_on
	ldi long, 0x04 
	rcall pwm_led_down
	ret

seq_2:						;		////-
	ldi long, 0x04 
	rcall pwm_led_rise
	rcall pwm_led_rise
	rcall pwm_led_rise
	rcall pwm_led_rise
	ldi long, 0x2F 
	rcall led_power_on
	ldi long, 0x01
	rcall led_power_off
	ret

seq_3:						;		||||   | | | |     |   |   |   |
	rcall seq_3_1
	rcall seq_3_2
	rcall seq_3_3
	rcall seq_3_4
	ldi long, 0x2F 
	rcall led_power_on
	ldi long, 0x01
	rcall led_power_off
	ret

seq_3_1:					;		||||
	ldi ttemp, 0x1F
	ldi long, 0x02
	rcall pulse_led
	ret

seq_3_2:					;		| | | |
	ldi ttemp, 0x0F
	ldi long, 0x04
	rcall pulse_led
	ret

seq_3_3:					;		|   |   |   |
	ldi ttemp, 0x07
	ldi long, 0x08
	rcall pulse_led	
	ret

seq_3_4:					;		|     |     |     |
	ldi ttemp, 0x05
	ldi long, 0x0D
	rcall pulse_led
	ret

seq_4:						;		/\/\/\/\
	rcall seq_4_3
	rcall seq_5_3
	rcall seq_4_3
	rcall seq_5_3
	rcall seq_4_3
	rcall seq_5_3
	rcall seq_4_3
	rcall seq_5_3
	ret

seq_4_1:					;		/////
	ldi long, 0x01 
	rcall pwm_led_rise
	ret

seq_4_2:					;		/ / / /
	ldi long, 0x02 
	rcall pwm_led_rise
	ret

seq_4_3:
	ldi long, 0x03 
	rcall pwm_led_rise
	ret

seq_4_4:
	ldi long, 0x04 
	rcall pwm_led_rise
	ret

seq_5:						;		\\\\
	rcall seq_5_4
	rcall seq_5_4
	rcall seq_5_4
	rcall seq_5_4
	ret

seq_5_1:
	ldi long, 0x01 
	rcall pwm_led_down
	ret

seq_5_2:
	ldi long, 0x02 
	rcall pwm_led_down
	ret

seq_5_3:
	ldi long, 0x03 
	rcall pwm_led_down
	ret

seq_5_4:
	ldi long, 0x04 
	rcall pwm_led_down
	ret

seq_6:
	ldi long, 0x04 
	rcall pwm_led_rise
	rcall pwm_led_rise
	rcall pwm_led_rise
	rcall pwm_led_rise
	ret

led_power_on:				
	clr www
	clr semafor
	ldi temp, 0b00000011	;power off pwm
	out TCCR0A, temp
	sbi PORTB,0				;set out pun
cekaj_tajmer_1:
	ldi temp, 0x01
	cp temp, semafor
	brne cekaj_tajmer_1
	clr semafor
	inc www
	mov temp, long
	cp temp, www
	breq idi_dalje_1
	rjmp cekaj_tajmer_1
idi_dalje_1:
	ret

led_power_off:
	clr www					
	clr semafor
	ldi temp, 0b00000011	;power on pwm
	out TCCR0A, temp
	cbi PORTB,0				;reset out pin
wait_timer_2:
	ldi temp, 0x01
	cp temp, semafor
	brne wait_timer_2
	clr semafor
	inc www
	mov temp, long
	cp temp, www
	breq next_2
	rjmp wait_timer_2
next_2:
	ret

pulse_led:
	clr qqq
pulse_led_next:
	rcall led_power_on
	rcall led_power_off
	inc qqq
	cp ttemp, qqq
	brne pulse_led_next
	ret

pause_led:
	push temp
	clr www
	clr semafor
wait_timer_3:
	ldi temp, 0x01
	cp temp, semafor
	brne wait_timer_3
	clr semafor
	inc www
	cp long, www
	breq next_3
	rjmp wait_timer_3
next_3:
	pop temp
	ret

pwm_led_rise:
	ldi temp, 0b10000011     ;power on pwm
	out TCCR0A, temp	
;	ldi temp, 0x00			 ;(  0%)
;	rcall pwm_led_mod
	ldi temp, 0x19			 ;( 10%)
	rcall pwm_led_mod
	ldi temp, 0x32			 ;( 20%)
	rcall pwm_led_mod		
	ldi temp, 0x4B			 ;( 30%)
	rcall pwm_led_mod
	ldi temp, 0x64			 ;( 40%)
	rcall pwm_led_mod	
	ldi temp, 0x7D			 ;( 50%)
	rcall pwm_led_mod	
	ldi temp, 0x96			 ;( 60%)
	rcall pwm_led_mod	
	ldi temp, 0xAF			 ;( 70%)
	rcall pwm_led_mod	
	ldi temp, 0xC8			 ;( 80%)
	rcall pwm_led_mod	
	ldi temp, 0xE1			 ;( 90%)
	rcall pwm_led_mod	
	ldi temp, 0xFF			 ;(100%)
	rcall pwm_led_mod
	ret

pwm_led_down:
	ldi temp, 0b10000011	;power on pwm
	out TCCR0A, temp	
	ldi temp, 0xFF			 ;(100%)
	rcall pwm_led_mod
	ldi temp, 0xE1			 ;( 90%)
	rcall pwm_led_mod		
	ldi temp, 0xC8			 ;( 80%)
	rcall pwm_led_mod
	ldi temp, 0xAF			 ;( 70%)
	rcall pwm_led_mod	
	ldi temp, 0x96			 ;( 60%)
	rcall pwm_led_mod	
	ldi temp, 0x7D			 ;( 50%)
	rcall pwm_led_mod	
	ldi temp, 0x64			 ;( 40%)
	rcall pwm_led_mod	
	ldi temp, 0x4B			 ;( 30%)
	rcall pwm_led_mod	
	ldi temp, 0x32			 ;( 20%)
	rcall pwm_led_mod	
	ldi temp, 0x19			 ;( 10%)
	rcall pwm_led_mod	
	ldi temp, 0x00			 ;(  0%)
	rcall pwm_led_mod	
	ret

pwm_led_mod:
	out OCR0A, temp
	rcall pause_led
	ret

overtake_taimer:		
	push temp
	inc tt
	ldi temp, 0x07
	cp temp, tt
	brne overtake_next
	clr tt					; 12ms
	ldi semafor, 0x01
overtake_next:
	pop temp
	reti				





