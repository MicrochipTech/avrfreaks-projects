;********* Daytona Sensors Graphics LCD Test Program Rev 1.0 *******************

;Graphics_LCD_Test.asm
;9-20-2010 CFS

;fuse settings for production version
;on-chip debug disabled (OCDEN=1)
;JTAG interface disabled (JTAGEN=1)
;serial program downloading enabled (SPIEN=0)
;preserve EEPROM memory disabled (EESAVE=1)
;boot FLASH section size=1024 words, start address $7C00 (BOOTSZ=10)
;boot reset vector disabled (BOOTRST=1)
;brown-out detection disabled (BODLEVEL=111)
;full swing oscillator/high frequency/64 msec startup (CKSEL=0111 SUT=11)
;divide clock by 8 disabled (CKDIV8=0)
;clock output on PORTB1 disabled (CKOUT=0)
;no memory lock features


.device ATmega644P			;Prohibits use of non-implemented instructions

.include "m644Pdef.inc"


;********* Conditional Assembly Options*****************************************



;********* Program Constants ***************************************************


.equ	char_dly		=1	;RS-232 character delay (.255 msec tics)
					;should be longer than the time
					;required to transmit one character at selected
					;baud rate
					;char_dly > 40000/baud_rate


.equ	input_debounce_time	=2	;input debounce time (4.096 msec units)	


					;parameter limits
.equ	param0_min		=0	
.equ	param0_max		=200
.equ	param1_min		=0	
.equ	param1_max		=200
.equ	param2_min		=-100
.equ	param2_max		=200
.equ	param3_min		=-100
.equ	param3_max		=200
.equ	param4_min		=0
.equ	param4_max		=6000
.equ	param5_min		=0
.equ	param5_max		=7000
.equ	param6_min		=0
.equ	param6_max		=8000
.equ	param7_min		=0
.equ	param7_max		=9000


					;parameter increment/decrement values
					;on encoder scroll
.equ	param0_inc		=1
.equ	param1_inc		=1
.equ	param2_inc		=10
.equ	param3_inc		=10
.equ	param4_inc		=100
.equ	param5_inc		=100
.equ	param6_inc		=100
.equ	param7_inc		=100


					;parameter decimal point
.equ	param0_dp			=2
.equ	param1_dp			=2					 
.equ	param2_dp			=2
.equ	param3_dp			=2
.equ	param4_dp			=0
.equ	param5_dp			=0
.equ	param6_dp			=0
.equ	param7_dp			=0


					;initial data values
.equ init_data0		=100
.equ init_data1		=200
.equ init_data2		=300
.equ init_data3		=400
.equ init_data4		=5000
.equ init_data5		=6000
.equ init_data6		=7000
.equ init_data7		=8000


					;data decimal point
.equ	data0_dp			=1
.equ	data1_dp			=1					 
.equ	data2_dp			=2
.equ	data3_dp			=2
.equ	data4_dp			=0
.equ	data5_dp			=0
.equ	data6_dp			=2
.equ	data7_dp			=2





;********* Global Register Variables *******************************************


					;r0-r2 reserved for hardware multiply and
					;table lookup

.def	temp_sr		=r3	;status register context save
					;pushed on stack in longer
					;interrupt service routines


					;following registers are not pushed onto
					;stack and must be available at all times

.def	timer0H		=r4	;timer0 high byte (16 msec tics)
					;rolls over to zero after 1.048 sec
					;used to increment various long duration timers

.def	timer1B3		=r5	;timer1 3rd byte (.262 sec tics)
					;updated by timer1 overflow ISR

.def	timer2H		=r6	;timer2 high byte (4.096 msec tics)
					;updated by timer2 overflow ISR


					;subroutine calls may change temp registers
					;temp1-4 registers also used by some math routines
.def	temp1			=r20
.def	temp2			=r21
.def	temp3			=r22
.def	temp4			=r23

.def	temp			=r24	;register used for subroutine parameter passing


.def	flag			=r25	;shared flag register (not pushed onto stack)

									
.equ	encoder_debounce	=0	;b0 set to sample and debounce encoder inputs
						;when input_debounce_time elapsed

.equ	display_delay		=1	;b1 set when display_delay_time elapsed

;********* Initial Values ******************************************************


;********* SRAM Locations ******************************************************


					;mega644 SRAM starts at $100 and ends at $10FF
					

					;parameters for RS-232 commands
					;received from serial port

.equ	cmd_addL		=$100	;address command (used for EEPROM/SRAM/FRAM/SPM)
.equ	cmd_addH		=$101


.equ	debounce_timer		=$133	;input debounce timer (4.096 msec tics)
							;updated by timer2 overflow ISR

.equ	display_timer		=$134	;display/user interface long range timer (1.048 sec tics)
							;updated by timer2 overflow ISR

.equ	display_delay_time	=$135	;display delay time (1.048 sec tics)
							;used to set various display/user interfaces delays
							;display_delay flag set when elapsed


.equ	intp_par_start	=$160	;reserved for interpolation parameters (11 locations)
.equ	intp_par_end	=$16A

.equ	scratch_pad	=$160	;scratch pad area (11 locations same as above)
					 

					;start of SRAM parameters copied from EEPROM
					;signed binary values
.equ	sparam_start	=$170
.equ	sparam0L		=$170
.equ	sparam0H		=$171	
.equ	sparam1L		=$172
.equ	sparam1H		=$173
.equ	sparam2L		=$174
.equ	sparam2H		=$175
.equ	sparam3L		=$176
.equ	sparam3H		=$177
.equ	sparam4L		=$178
.equ	sparam4H		=$179
.equ	sparam5L		=$17A
.equ	sparam5H		=$17B
.equ	sparam6L		=$17C
.equ	sparam6H		=$17D
.equ	sparam7L		=$17E
.equ	sparam7H		=$17F
.equ	sparam_end	=$17F	;end of SRAM parameters copied from EEPROM



					;start of display data values
					;signed binary values
.equ	data0L		=$180
.equ	data0H		=$181	
.equ	data1L		=$182
.equ	data1H		=$183
.equ	data2L		=$184
.equ	data2H		=$185
.equ	data3L		=$186
.equ	data3H		=$187
.equ	data4L		=$188
.equ	data4H		=$189
.equ	data5L		=$18A
.equ	data5H		=$18B
.equ	data6L		=$18C
.equ	data6H		=$18D
.equ	data7L		=$18E
.equ	data7H		=$18F



							;quadrature encoder related
.equ	encoder_position	=$1F0 	;encoder position 
.equ	encoder_state		=$1F1 	;encoder state
.equ	encoder_switch		=$1F2	;encoder switch input (b6)
.equ	prev_encoder_switch	=$1F3	;previous encoder switch input (b6)
.equ	encoder_switch_deb	=$1F4	;debounced encoder switch input (b6)	

.equ	encoder_flag		=$1F5	;debounced encoder flag bits
						
.equ	encoder_inc		=0		;b0 set on increment
.equ	encoder_dec		=1		;b1 set on decrement
.equ	encoder_push		=2		;b2 set on switch pushed (edge triggered)
							;note that encoder_push flag must be cleared in user
							;interface routine after servicing 

.equ	ui_state_num		=$1F6	;user interface state number

.equ	param_num			=$1F7	;parameter number (0 to 7)


.equ	data_start		=$1F8	;display data starting data number (0 to 7)
							;data_num for first line of scrolling data display

.equ	data_num			=$1F9	;display data number (0 to 7)
.equ	data_row			=$1FA	;display data row (0-3)


						;$200-21F reserved for binary-to-ASCII conversion
						;routine temp_buffers


.equ lcd_buffer	=$C00	;1K SRAM grahics buffer
						;ends at $FFF


;********* EEPROM Locations ****************************************************


.equ	firstEEPROM		=$10	;first address for parameter data
					;lower address can be corrupted

.equ	lastEEPROM		=$60	;last address for parameter data

.equ	checksum		=$61	;checksum byte (not presently used)

				
						
					;EEPROM locations $10 to $60 reserved for parameters
					;and user data



;********* Port A Pin Definitions **********************************************


					;PA0-7 used for parallel LCD data
.equ	LCD1_DATA_DDR		=DDRA
.equ	LCD1_DATA_PORT		=PORTA
.equ	LCD1_DATA_PIN		=PINA
							

;********* Port B Pin Definitions **********************************************

					;LCD_signame refers to serial (SPI) bus interface LCD
					;signals
					 
.equ	LCD_PORT		=PORTB
.equ	LED_PORT		=PORTB

.equ	LCD_NRST		=0	;LCD_RST\ (LCD reset) output
.equ	LCD_NCS		=1	;LCD_CS (LCD chip select) output
.equ	LCD_A0		=2	;LCD_A0 (LCD address line A0) output
.equ	LED_CTRL		=3	;LED_CTRL output

					;PB5-7 used for LCD SPI interface and ISP

;********* Port C Pin Definitions **********************************************

					

.equ	S1_NB	=0		;S1B\ encoder channel B
.equ	S1_NA	=1		;S1A\ encoder channel A

					;PC2-5 used for JTAG

.equ	S1_NPUSH	=6		;S1_PUSH\ encoder pushbutton	

					


;********* Port D Pin Definitions **********************************************



.equ	RXD			=0	;RS-232 RXD (receive)	
.equ	TXD			=1	;RS-232 TXD (transmit)

					;LCD1_signame refers to parallel interface LCD
					;signals

.equ	LCD1_CTRL_PORT	=PORTD

.equ	LCD1_RS		=2	;LCD1_RS (register select) output
.equ	LCD1_RW		=3	;LCD1_RW (read/write) output
.equ	LCD1_E		=4	;LCD1_E (active low enable) output
.equ	LCD1_CS1		=5	;LCD1_CS1 (chip select 1) output
.equ	LCD1_CS2		=6	;LCD1_CS2 (chip select 2) output
.equ	LCD1_NRST		=7	;LCD1_RST\ (reset) output


;********* Interrupt Vectors ***************************************************

					;start of application FLASH


		jmp	RESET		;reset vector
		jmp	EXT_INT0		;IRQ0 vector
		jmp	EXT_INT1		;IRQ1 vector
		jmp	EXT_INT2		;IRQ2 vector
		jmp	PC_INT0		;PCINT0 vector
		jmp	PC_INT1		;PCINT1 vector
		jmp	PC_INT2		;PCINT2 vector
		jmp	PC_INT3		;PCINT3 vector
		jmp	WDT			;watchdog timeout interrupt vector
		jmp	TIM2_COMPA	;Timer2 compareA interrupt vector
		jmp	TIM2_COMPB	;Timer2 compareB interrupt vector
		jmp	TIM2_OVF		;Timer2 overflow interrupt vector
		jmp	TIM1_CAPT		;Timer1 input capture interrupt vector
		jmp	TIM1_COMPA	;Timer1 compareA interrupt vector
		jmp	TIM1_COMPB	;Timer1 compareB interrupt vector
		jmp	TIM1_OVF		;Timer1 overflow interrupt vector
		jmp	TIM0_COMPA	;Timer0 compareA interrupt vector
		jmp	TIM0_COMPB	;Timer0 compareB interrupt vector
		jmp	TIM0_OVF		;timer0 overflow interrupt vector
		jmp	SPI_STC		;SPI transfer complete interrupt vector
		jmp	UART0_RXC		;UART0 receive complete interrupt vector
		jmp	UART0_DRE		;UART0 data register empty interrupt vector
		jmp	UART0_TXC		;UART0 transmit complete interrupt vector
		jmp	ANA_COMP		;analog comparator interrupt vector
		jmp	ADC_INT		;ADC conversion complete interrupt vector
		jmp	EE_RDY		;EEPROM ready interrupt vector
		jmp	TWSI			;two wire serial interface interrupt vector
		jmp	SPM_RDY		;SPM ready interrupt vector
		jmp	UART1_RXC		;UART1 receive complete interrupt vector
		jmp	UART1_DRE		;UART1 data register empty interrupt vector
		jmp	UART1_TXC		;UART1 transmit complete interrupt vector






;********* Main Program ********************************************************

					;reset
					;entry point for processor reset

					;set stack pointer to top of RAM
RESET:	ldi	temp,high(RAMEND)
		out	SPH,temp
		ldi	temp,low(RAMEND)
		out	SPL,temp


		clr	flag		;clear flag register


					;disable interrupts
		cli

					;initialize ports
		clr	temp
		out	PORTA,temp

					;LCD_RST\ low
		ldi	temp,0b00011010
		out	PORTB,temp

					
		ldi	temp,0b00000000
		out	PORTC,temp
					
					;LCD1_E and LCD1_RST\ high
		ldi	temp,0b10010000	
		out	PORTD,temp

					;set port A as outputs
		ldi	temp,0b11111111	
		out	DDRA,temp

					;set port B as outputs except PB6
		ldi	temp,0b10111111			 
		out	DDRB,temp			
					
					;set port C as outputs except PC0-1,6
		ldi	temp,0b10111100	
		out	DDRC,temp

					;set port D as outputs except PD0
		ldi	temp,0b11111110	
		out	DDRD,temp

	
					;enable .12 sec watchdog timer
					;timed sequence required by ATMega644
		ldi	temp,$18	
		sts	WDTCSR,temp	
		ldi	temp,$0B	
		sts	WDTCSR,temp
		wdr


		ldi	temp,$80	;disable analog comparator
		out	ACSR,temp



		clr	temp		;disable all timer and external interrupts
		sts	TIMSK0,temp
		sts	TIMSK1,temp
		sts	TIMSK2,temp
		out	EIMSK,temp


					;setup timer0 for 16 usec tics
		ldi	temp,$04
		out	TCCR0B,temp


					;disable timer1 compareA and compareB
					;(normal port pin operation)
		ldi	temp,$00
		sts	TCCR1A,temp

					;setup timer1 for 4 usec tics
					;setup ICP for capture on falling edge
					;(not used in this version)
		ldi	temp,$83
		sts	TCCR1B,temp

					;setup timer2 for 16 usec tics
					;gives 4.096 msec overflow interrupt
					;interval
		ldi	temp,$06
		sts	TCCR2B,temp	
						 
		
							

		sei			;global interrupt enable

					;initialize timer0
		clr	temp
		out	TCNT0,temp
		clr	timer0H


					;initialize timer1
		clr	temp
		sts	TCNT1H,temp
		sts	TCNT1L,temp
		clr	timer1B3

					;initialize timer2 
		clr	temp
		sts	TCNT2,temp
		clr	timer2H
		sts	debounce_timer,temp
		sts	display_timer,temp
				

					;set TXD output high to allow RS-232
					;operations
		sbi	PORTD,TXD

		ldi	temp,$90	;enable RS-232 receive only
		sts	UCSR0B,temp	;and RXC0 interrupts
		ldi	temp,17	;56000 baud (actually 55555 baud)
		sts	UBRR0L,temp


				;disable all timer interrupts except
				;timer2 overflow
		clr	temp
		sts	TIMSK0,temp
		sts	TIMSK1,temp
		ldi	temp,$1	
		sts	TIMSK2,temp	
		
					
					;initialize parameters
					;copy EEPROM parameters to SRAM
					;initialize SRAM parameters
		call	PARAM_INIT	

					;initialize user interface state machine
		clr	temp
		sts	ui_state_num,temp
		sts	param_num,temp
		sts	data_start,temp
		sts	data_num,temp
		sts	data_row,temp


					;initialize encoder values
		clr	temp
		sts	encoder_position,temp 
		sts	encoder_state,temp 
		sts	encoder_switch,temp
		sts	prev_encoder_switch,temp
		sts	encoder_flag,temp



					;main loop
LOOP1:	wdr			;reset watchdog timer


					;if encoder_debounce flag set
					;run encoder debounce and user interface state machine routines 
					;else jump ahead
		sbrs	flag,encoder_debounce
		rjmp	LOOP2	

		
					;encoder debounce routine
		call	ENC_DEB
		
					;user interface state machine
		call	UI_SM

					;clear flag
		cbr	flag,1<<encoder_debounce


LOOP2:	rjmp	LOOP1

	



;********* Parameter Initialization Subroutine *********************************

					;initialize SRAM parameters and data

PARAM_INIT:
					;copy EEPROM parameters to SRAM
					;must maintain same sequence
					;8 parameters (2 bytes each)
		ldi	r17,low(eparam_start)
		ldi	r18,high(eparam_start)
		ldi	r30,low(sparam_start)
		ldi	r31,high(sparam_start)

PARAM_INIT1:	call	EERead
		st	Z+,r16
		subi	r17,low(-1)
		sbci	r18,high(-1)
		cpi	r30,low(sparam_start + 16)
		ldi	temp,high(sparam_start + 16)
		cpc	r31,temp
		brne	PARAM_INIT1



				;initialize data values
		ldi	r16,low(init_data0)
		ldi	r17,high(init_data0)
		sts	data0L,r16
		sts	data0H,r17

		ldi	r16,low(init_data1)
		ldi	r17,high(init_data1)
		sts	data1L,r16
		sts	data1H,r17

		ldi	r16,low(init_data2)
		ldi	r17,high(init_data2)
		sts	data2L,r16
		sts	data2H,r17

		ldi	r16,low(init_data3)
		ldi	r17,high(init_data3)
		sts	data3L,r16
		sts	data3H,r17

		ldi	r16,low(init_data4)
		ldi	r17,high(init_data4)
		sts	data4L,r16
		sts	data4H,r17

		ldi	r16,low(init_data5)
		ldi	r17,high(init_data5)
		sts	data5L,r16
		sts	data5H,r17

		ldi	r16,low(init_data6)
		ldi	r17,high(init_data6)
		sts	data6L,r16
		sts	data6H,r17

		ldi	r16,low(init_data7)
		ldi	r17,high(init_data7)
		sts	data7L,r16
		sts	data7H,r17


PARAM_RET:	ret




;********* Encoder Input Debounce Subroutine ***********************************

		
					;debounce encoder switch inputs
ENC_DEB:
					;quadrature encoder state machine
					;get previous encoder position
 					;read last table value
					;keep only previous encoder position bits 
        	lds 	r16,encoder_position
        	lds 	ZH,encoder_state
        	andi ZH,$0C
        	
			
					;read position bits from rotary encoder
					;on PINC b0-1 
					;mask off unused bits 
		in	ZL,PINC
		andi	ZL,$03
	
					;combine new and previous encoder bits
        	or 	ZL,ZH 
        	subi ZL,low(-(2 * Encoder_Table)) 
        	ldi 	ZH,(high(2 * Encoder_Table))+1 
        	clr	temp
		sbc 	ZH,temp 
        	lpm 	r17,Z   	;read next state and counts from table 
 

					;bits 0 and 1 of r17 now contain a value that is one greater than 
					;the number of steps to be added to the encoder current position 
					;bits 2 and 3 contain a copy of the values just read from the encoder 
        	sts 	encoder_state,r17 
        	andi r17,$03  	;keep only delta bits 
        	dec 	r17       ;r17 now ranges from -1 to 1 
        	add 	r16,r17   ;add the delta value and updaate encoder position 
        	sts  encoder_position,r16
		
					;clear encoder_inc and encoder_dec flags
					;note that encoder_flag stays in temp until end
					;if r17=1 set encoder_inc flag and clear encoder_dec flag  
		lds	temp,encoder_flag
		cbr	temp,1<<encoder_inc +1<<encoder_dec

					;if r17=1 set encoder_inc flag 
		cpi	r17,1
		brne	ENC_DEB1
		sbr	temp,1<<encoder_inc

					;if r17=-1 set encoder_dec flag
ENC_DEB1:	cpi	r17,-1
		brne	ENC_DEB2
		sbr	temp,1<<encoder_dec


					;debounce encoder switch					

					;prev_encoder_switch = encoder_switch		
ENC_DEB2:	lds	temp1,encoder_switch
		sts	prev_encoder_switch,temp1

					;invert and mask off encoder switch input
					;on PINC b6
		in	temp1,PINC
		com	temp1
		andi	temp1,$40

	
					;encoder switch b6 now set correctly in temp1
					;save as new encoder_switch
		sts	encoder_switch,temp1

					;XOR samples to find debounced (same) bits
		lds	temp2,prev_encoder_switch
		eor	temp2,temp1
		

		sbrc	temp2,6
		rjmp	ENC_DEB3

					;b6 (S1_PUSH) debounced (same) in temp1 
					;load previous debounced value into temp2
					;and save new debounced value
		lds	temp2,encoder_switch_deb
		sts	encoder_switch_deb,temp1			
		
					;if (S1_PUSH) b6 rising edge, set encoder_push flag
					;note that encoder_push flag must be cleared in user
					;interface routine after servicing 
		sbrc	temp2,6
		rjmp	ENC_DEB3
		
		sbrc	temp1,6
		sbr	temp,1<<encoder_push
	
		
					;save debounced flag value				
ENC_DEB3:	sts	encoder_flag,temp

ENC_DEBRT:	ret


					;encoder table format: 
					;  b0-1 are 2-bit delta increment + 1 
					;  b2-3 are shifted copy of current state to be loaded into previous state
					;  b4   set for error (old & new states not adjacent) 
Encoder_Table: 
.db 0b00001, 0b00101 
.db 0b01010, 0b11101 
.db 0b00001, 0b00101 
.db 0b11001, 0b01101 
.db 0b00000, 0b10101 
.db 0b01001, 0b01101 
.db 0b10001, 0b00101 
.db 0b01001, 0b01101 






;********* User Interface State Machine Subroutine *****************************

					;user interface state machine subroutine  
	
					;proceed to state	
UI_SM:	lds	temp,ui_state_num

		cpi	temp,12	;test for invalid state >=12
		brsh	UI_SMNL

		clr	ZH
		mov	ZL,temp
		subi	ZL,low(-UI_SMC0)	;add offset
		sbci	ZH,high(-UI_SMC0)
		ijmp			;jump to offset

UI_SMC0:	rjmp	UI_SMS0	;state 0: initial splash screen
UI_SMC1:	rjmp	UI_SMS1	;state 1: splash screen delay
UI_SMC2:	rjmp	UI_SMS2	;state 2; data display, wait for encoder push 
UI_SMC3:	rjmp	UI_SMS3	;state 3: parameter screen
UI_SMC4:	rjmp	UI_SMS4	;state 4: parameter screen, wait for encoder scroll/push
UI_SMC5:	rjmp	UI_SMS5	;state 5: highlight next parameter 
UI_SMC6:	rjmp	UI_SMS6	;state 6: highlight next parameter, wait for encoder scroll/push
UI_SMC7:	rjmp	UI_SMS7	;state 7: highlight data display
UI_SMC8:	rjmp	UI_SMS8	;state 8; highlight data display, wait for encoder scroll/push 
UI_SMC9:	rjmp	UI_SMS9	;state 9: list next parameter
UI_SMC10:	rjmp	UI_SMS10	;state 10: parameter edit, display initial value
UI_SMC11:	rjmp	UI_SMS11	;state 11: parameter edit, wait for encoder scroll/push
UI_SMNL:	rjmp	UI_SMNUL	;invalid state 


					;state 0: initial splash screen
					;transition to state 1
UI_SMS0:	
					;initialize LCD display
		call	LCD_Init	
	
					;clear LCD buffer
		call LCD_BufClear
			
					;logo graphic start address to Z
		ldi	r30,low(fdaytona_sensors_graphic)
		ldi	r31,high(fdaytona_sensors_graphic)
					;write graphic - entire screen
;		call Graphic_Write128x64

					;write graphic showing use of alternate graphics routine
		ldi	r20,0	;starting column (0-127)
		ldi	r21,0	;starting row (0-7)
		ldi	r22,128	;number of columns (1-128)
		ldi	r23,8	;number of rows (1-8)
		ldi	r24,0	;graphics flag (b0 set to invert pixels)
		call Graphic_Write

	
		ldi	r30,low(SPLASH_STRING)
		ldi	r31,high(SPLASH_STRING)
		ldi	r21,0
		ldi	r22,7
		clr	r23
		call	StringF_Write6x8 

		call	LCD_BufWrite


					;start 2 second splash screen delay
		clr	temp
		sts	display_timer,temp
		ldi	temp,2
		sts	display_delay_time,temp
		cbr	flag,1<<display_delay

					;initialize param_num and data display
		clr	temp
		sts	param_num,temp
		sts	data_start,temp
		sts	data_num,temp
		sts	data_row,temp


		ldi	temp,1
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 1: splash screen delay
					;transition to state 2
UI_SMS1:	

		sbrs	flag,display_delay
		rjmp	UI_SMRET

		ldi	temp,2
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 2: data display, wait for encoder push/scroll
					;transition to state 3 on push
UI_SMS2:
	
					;test encoder_push flag
					;jump ahead if clear
		lds	temp,encoder_flag
		sbrs	temp,encoder_push
		rjmp	UI_SMS2_1

					;encoder_push flag set
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;initialize param_num
		clr	temp
		sts	param_num,temp


		ldi	temp,3
		sts	ui_state_num,temp

		rjmp	UI_SMRET
	
	

					;test encoder position for scroll
					;jump ahead to data display if no scroll
UI_SMS2_1:	
		lds	temp,encoder_position
		cpi	temp,4
		brlt	UI_SMS2_2	
					
					;scrolled data screen
					;increment data_start (roll over at data_start = 7)
		lds	temp,data_start
		inc	temp
		andi	temp,$07					
		sts	data_start,temp

	
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

	
	
					;clear LCD buffer
UI_SMS2_2:	call LCD_BufClear

					;write display data to LCD
		lds	temp,data_start
		sts	data_num,temp
		ldi	temp,0
		sts	data_row,temp
		call	DT_DSP


		lds	temp,data_start
		subi	temp,-1
		andi	temp,$07
		sts	data_num,temp
		ldi	temp,1
		sts	data_row,temp
		call	DT_DSP

		lds	temp,data_start
		subi	temp,-2
		andi	temp,$07
		sts	data_num,temp
		ldi	temp,2
		sts	data_row,temp
		call	DT_DSP

		lds	temp,data_start
		subi	temp,-3
		andi	temp,$07
		sts	data_num,temp
		ldi	temp,3
		sts	data_row,temp
		call	DT_DSP

		call	LCD_BufWrite

	
		rjmp	UI_SMRET


					;state 3: parameter screen
					;highlight initial parameter
					;transition to state 4
UI_SMS3:

					;clear LCD buffer
		call LCD_BufClear

		ldi	r30,low(PARAM_SCREEN_STRING)
		ldi	r31,high(PARAM_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,0
		clr	r23
		call	StringF_Write8x16 


		call	PR_ADD
		ldi	r21,0
		ldi	r22,1
		ldi	r23,$80
		call	StringF_Write8x16 

		ldi	r30,low(NEXT_PARAM_STRING)
		ldi	r31,high(NEXT_PARAM_STRING)
		ldi	r21,0
		ldi	r22,2
		clr	r23
		call	StringF_Write8x16 

		ldi	r30,low(DATA_SCREEN_STRING)
		ldi	r31,high(DATA_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,3
		clr	r23
		call	StringF_Write8x16 

		call	LCD_BufWrite

					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp


		ldi	temp,4
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 4: parameter screen
					;wait for encoder push/scroll
					;transition to state 5 or 10 
UI_SMS4:

					;test encoder_push flag
					;jump ahead if clear
		lds	temp,encoder_flag
		sbrs	temp,encoder_push
		rjmp	UI_SMS4_1

					;encoder_push flag set
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 10
		ldi	temp,10
		sts	ui_state_num,temp

		rjmp	UI_SMRET

					;test encoder_position for scroll
					;return if no scroll
UI_SMS4_1:	

		lds	temp,encoder_position
		cpi	temp,4
		brge	UI_SMS4_2
		rjmp	UI_SMRET

					;scrolled to next parameter
UI_SMS4_2:		
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 5
		ldi	temp,5
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 5: highlight next parameter
					;transition to state 6 
UI_SMS5:
					;clear LCD buffer
		call LCD_BufClear

		ldi	r30,low(PARAM_SCREEN_STRING)
		ldi	r31,high(PARAM_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,0
		clr	r23
		call	StringF_Write8x16 


		call	PR_ADD
		ldi	r21,0
		ldi	r22,1
		clr	r23
		call	StringF_Write8x16 

		ldi	r30,low(NEXT_PARAM_STRING)
		ldi	r31,high(NEXT_PARAM_STRING)
		ldi	r21,0
		ldi	r22,2
		ldi	r23,$80
		call	StringF_Write8x16 

		ldi	r30,low(DATA_SCREEN_STRING)
		ldi	r31,high(DATA_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,3
		clr	r23
		call	StringF_Write8x16 

		call	LCD_BufWrite

					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

		ldi	temp,6
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 6: highlight next parameter
					;wait for encoder push/scroll
					;transition to state 7 or 9 
UI_SMS6:

					;test encoder_push flag
					;jump ahead if clear
		lds	temp,encoder_flag
		sbrs	temp,encoder_push
		rjmp	UI_SMS6_1

					;encoder_push flag set
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 9
		ldi	temp,9
		sts	ui_state_num,temp

		rjmp	UI_SMRET

					;test encoder_position for scroll
					;return if no scroll
UI_SMS6_1:	

		lds	temp,encoder_position
		cpi	temp,4
		brge	UI_SMS6_2
		rjmp	UI_SMRET

					;scrolled to data display
UI_SMS6_2:		
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 7
		ldi	temp,7
		sts	ui_state_num,temp

		rjmp	UI_SMRET



					;state 7: highlight data display
					;transition to state 8 
UI_SMS7:
					;clear LCD buffer
		call LCD_BufClear

		ldi	r30,low(PARAM_SCREEN_STRING)
		ldi	r31,high(PARAM_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,0
		clr	r23
		call	StringF_Write8x16 


		call	PR_ADD
		ldi	r21,0
		ldi	r22,1
		clr	r23
		call	StringF_Write8x16 

		ldi	r30,low(NEXT_PARAM_STRING)
		ldi	r31,high(NEXT_PARAM_STRING)
		ldi	r21,0
		ldi	r22,2
		clr	r23
		call	StringF_Write8x16 

		ldi	r30,low(DATA_SCREEN_STRING)
		ldi	r31,high(DATA_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,3
		ldi	r23,$80
		call	StringF_Write8x16 

		call	LCD_BufWrite

					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

		ldi	temp,8
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 8: highlight data display
					;wait for encoder push/scroll
					;transition to state 2 or 3 
UI_SMS8:

					;test encoder_push flag
					;jump ahead if clear
		lds	temp,encoder_flag


		sbrs	temp,encoder_push
		rjmp	UI_SMS8_1

					;encoder_push flag set
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 2
		ldi	temp,2
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;test encoder_position for scroll
					;return if no scroll
UI_SMS8_1:	

		lds	temp,encoder_position
		cpi	temp,4
		brge	UI_SMS8_2
		rjmp	UI_SMRET


					;scrolled to parameter screen
UI_SMS8_2:		
					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 3
		ldi	temp,3
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 9: list next parameter
					;transition to state 4 
UI_SMS9:
					;clear LCD buffer
		call LCD_BufClear

		ldi	r30,low(PARAM_SCREEN_STRING)
		ldi	r31,high(PARAM_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,0
		clr	r23
		call	StringF_Write8x16 


					;increment parameter 
		lds	temp,param_num
		inc	temp
		cpi	temp,8
		brlo	UI_SMS9_1
		clr	temp

UI_SMS9_1:
		sts	param_num,temp

		call	PR_ADD
		ldi	r21,0
		ldi	r22,1
		ldi	r23,$80
		call	StringF_Write8x16 

		ldi	r30,low(NEXT_PARAM_STRING)
		ldi	r31,high(NEXT_PARAM_STRING)
		ldi	r21,0
		ldi	r22,2
		clr	r23
		call	StringF_Write8x16 

		ldi	r30,low(DATA_SCREEN_STRING)
		ldi	r31,high(DATA_SCREEN_STRING)
		ldi	r21,0
		ldi	r22,3
		clr	r23
		call	StringF_Write8x16 

		call	LCD_BufWrite

					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp


		ldi	temp,4
		sts	ui_state_num,temp

		rjmp	UI_SMRET


					;state 10: parameter edit, display initial value
					;transition to state 11 
UI_SMS10:

					;clear encoder flag bits (to prevent parameter change)
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp


		call	PR_EDIT


					;right justify 7 characters on row
		ldi	r21,(16-7)
		ldi	r22,1
		ldi	r23,$80
		ldi	r24,7	
		call	StringS_Write8x16 

		call	LCD_BufWrite

					;transition to state 11
		ldi	temp,11
		sts	ui_state_num,temp


		rjmp	UI_SMRET


					;state 11: parameter edit, display initial value
					;wait for encoder push/scroll
					;transition to state 3 when done
UI_SMS11:

					;return if no encoder_flag bit set
		lds	temp,encoder_flag
		tst	temp
		brne	UI_SMS11_1
		rjmp	UI_SMRET


					;test encoder_push flag
					;jump ahead if clear

UI_SMS11_1:
		lds	temp,encoder_flag
		sbrs	temp,encoder_push
		rjmp	UI_SMS11_2

					;encoder_push flag set

					;save edited parameter value to EEPROM
					;calculate parameter address
		ldi	r17,low(eparam_start)
		ldi	r18,high(eparam_start)
		ldi	r30,low(sparam_start)
		ldi	r31,high(sparam_start)
		lds	r19,param_num
		clr	r20
		lsl	r19
		add	r17,r19
		adc	r18,r20
		add	r30,r19
		adc	r31,r20	
		ld	r16,Z+
		call	EEWrite1
		subi	r17,low(-1)
		sbci	r18,high(-1)
		ld	r16,Z
		call	EEWrite1


					;initialize encoder
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp

					;transition to state 3
		ldi	temp,3
		sts	ui_state_num,temp

		rjmp	UI_SMRET

					;encoder scroll (encoder_inc or encoder_dec flag set)
					;update parameter 
UI_SMS11_2:	

		call	PR_EDIT


					;right justify 7 characters on row
		ldi	r21,(16-7)
		ldi	r22,1
		ldi	r23,$80
		ldi	r24,7	
		call	StringS_Write8x16 

		call	LCD_BufWrite

					;clear encoder flag bits
		clr	temp
		sts	encoder_flag,temp
		sts	encoder_position,temp


		rjmp	UI_SMRET


					;invalid state
					;transition to state 0 (initial splash screen)
UI_SMNUL:	clr	temp
		sts	ui_state_num,temp


UI_SMRET:	ret





;********* Parameter Address Subroutine ****************************************

					;parameter address subroutine
					;enter with param_num (valid range 0 to 7)
					
					;returns address of corresponding 
					;parameter description string in Z	

PR_ADD:	lds	temp,param_num

		cpi	temp,8	;test for invalid param_num >=8
		brsh	PR_ADDS0

		clr	ZH
		mov	ZL,temp
		subi	ZL,low(-PR_ADDC0)	;add offset
		sbci	ZH,high(-PR_ADDC0)
		ijmp			;jump to offset

PR_ADDC0:	rjmp	PR_ADDS0	
PR_ADDC1:	rjmp	PR_ADDS1	
PR_ADDC2:	rjmp	PR_ADDS2	
PR_ADDC3:	rjmp	PR_ADDS3	
PR_ADDC4:	rjmp	PR_ADDS4	
PR_ADDC5:	rjmp	PR_ADDS5
PR_ADDC6:	rjmp	PR_ADDS6
PR_ADDC7:	rjmp	PR_ADDS7


PR_ADDS0:	ldi	r30,low(PARAM_STRING0)
		ldi	r31,high(PARAM_STRING0)
		rjmp	PR_ADDRT
			
PR_ADDS1:	ldi	r30,low(PARAM_STRING1)
		ldi	r31,high(PARAM_STRING1)
		rjmp	PR_ADDRT


PR_ADDS2:	ldi	r30,low(PARAM_STRING2)
		ldi	r31,high(PARAM_STRING2)
		rjmp	PR_ADDRT

PR_ADDS3:	ldi	r30,low(PARAM_STRING3)
		ldi	r31,high(PARAM_STRING3)
		rjmp	PR_ADDRT


PR_ADDS4:	ldi	r30,low(PARAM_STRING4)
		ldi	r31,high(PARAM_STRING4)
		rjmp	PR_ADDRT
			
PR_ADDS5:	ldi	r30,low(PARAM_STRING5)
		ldi	r31,high(PARAM_STRING5)
		rjmp	PR_ADDRT


PR_ADDS6:	ldi	r30,low(PARAM_STRING6)
		ldi	r31,high(PARAM_STRING6)
		rjmp	PR_ADDRT

PR_ADDS7:	ldi	r30,low(PARAM_STRING7)
		ldi	r31,high(PARAM_STRING7)

PR_ADDRT:	ret




;********* Parameter Edit Subroutine *******************************************

					;parameter edit subroutine
					;enter with param_num (valid range 0 to 7)
					
					;increments/decrements parameter based on encoder flag
					;limits parameter value 
					;returns with ASCII string at Z 


PR_EDIT:	lds	temp,param_num

		cpi	temp,8	;test for invalid param_num >=8
		brsh	PR_EDITS0

		clr	ZH
		mov	ZL,temp
		subi	ZL,low(-PR_EDITC0)	;add offset
		sbci	ZH,high(-PR_EDITC0)
		ijmp			;jump to offset

PR_EDITC0:	rjmp	PR_EDITS0	
PR_EDITC1:	rjmp	PR_EDITS1	
PR_EDITC2:	rjmp	PR_EDITS2	
PR_EDITC3:	rjmp	PR_EDITS3	
PR_EDITC4:	rjmp	PR_EDITS4	
PR_EDITC5:	rjmp	PR_EDITS5
PR_EDITC6:	rjmp	PR_EDITS6
PR_EDITC7:	rjmp	PR_EDITS7


PR_EDITS0:
					;calculate parameter address
		ldi	r30,low(sparam0L)
		ldi	r31,high(sparam0L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS0_1
		subi	r16,low(param0_inc)
		sbci	r17,high(param0_inc)

		rjmp	PR_EDITS0_2


PR_EDITS0_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS0_2		
		subi	r16,low(-param0_inc)
		sbci	r17,high(-param0_inc)

					;limit parameter value
PR_EDITS0_2:
		
		ldi	r18,low(param0_min)
		ldi	r19,high(param0_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS0_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS0_3:
		
		ldi	r18,low(param0_max)
		ldi	r19,high(param0_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS0_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS0_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param0_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT
			

PR_EDITS1:
					;calculate parameter address
		ldi	r30,low(sparam1L)
		ldi	r31,high(sparam1L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS1_1
		subi	r16,low(param1_inc)
		sbci	r17,high(param1_inc)

		rjmp	PR_EDITS1_2


PR_EDITS1_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS1_2		
		subi	r16,low(-param1_inc)
		sbci	r17,high(-param1_inc)

					;limit parameter value
PR_EDITS1_2:
		
		ldi	r18,low(param1_min)
		ldi	r19,high(param1_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS1_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS1_3:
		
		ldi	r18,low(param1_max)
		ldi	r19,high(param1_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS1_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS1_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param1_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT



PR_EDITS2:
					;calculate parameter address
		ldi	r30,low(sparam2L)
		ldi	r31,high(sparam2L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS2_1
		subi	r16,low(param2_inc)
		sbci	r17,high(param2_inc)

		rjmp	PR_EDITS2_2


PR_EDITS2_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS2_2		
		subi	r16,low(-param2_inc)
		sbci	r17,high(-param2_inc)

					;limit parameter value
PR_EDITS2_2:
		
		ldi	r18,low(param2_min)
		ldi	r19,high(param2_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS2_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS2_3:
		
		ldi	r18,low(param2_max)
		ldi	r19,high(param2_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS2_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS2_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param2_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT


PR_EDITS3:
					;calculate parameter address
		ldi	r30,low(sparam3L)
		ldi	r31,high(sparam3L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS3_1
		subi	r16,low(param3_inc)
		sbci	r17,high(param3_inc)

		rjmp	PR_EDITS3_2


PR_EDITS3_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS3_2		
		subi	r16,low(-param3_inc)
		sbci	r17,high(-param3_inc)

					;limit parameter value
PR_EDITS3_2:
		
		ldi	r18,low(param3_min)
		ldi	r19,high(param3_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS3_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS3_3:
		
		ldi	r18,low(param3_max)
		ldi	r19,high(param3_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS3_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS3_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param3_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT


PR_EDITS4:
					;calculate parameter address
		ldi	r30,low(sparam4L)
		ldi	r31,high(sparam4L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS4_1
		subi	r16,low(param4_inc)
		sbci	r17,high(param4_inc)

		rjmp	PR_EDITS4_2


PR_EDITS4_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS4_2		
		subi	r16,low(-param4_inc)
		sbci	r17,high(-param4_inc)

					;limit parameter value
PR_EDITS4_2:
		
		ldi	r18,low(param4_min)
		ldi	r19,high(param4_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS4_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS4_3:
		
		ldi	r18,low(param4_max)
		ldi	r19,high(param4_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS4_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS4_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param4_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT

			
PR_EDITS5:
					;calculate parameter address
		ldi	r30,low(sparam5L)
		ldi	r31,high(sparam5L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS5_1
		subi	r16,low(param5_inc)
		sbci	r17,high(param5_inc)

		rjmp	PR_EDITS5_2


PR_EDITS5_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS5_2		
		subi	r16,low(-param5_inc)
		sbci	r17,high(-param5_inc)

					;limit parameter value
PR_EDITS5_2:
		
		ldi	r18,low(param5_min)
		ldi	r19,high(param5_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS5_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS5_3:
		
		ldi	r18,low(param5_max)
		ldi	r19,high(param5_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS5_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS5_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param5_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT


PR_EDITS6:
					;calculate parameter address
		ldi	r30,low(sparam6L)
		ldi	r31,high(sparam6L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS6_1
		subi	r16,low(param6_inc)
		sbci	r17,high(param6_inc)

		rjmp	PR_EDITS6_2


PR_EDITS6_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS6_2		
		subi	r16,low(-param6_inc)
		sbci	r17,high(-param6_inc)

					;limit parameter value
PR_EDITS6_2:
		
		ldi	r18,low(param6_min)
		ldi	r19,high(param6_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS6_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS6_3:
		
		ldi	r18,low(param6_max)
		ldi	r19,high(param6_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS6_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS6_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param6_dp
		
		call	Bin2SToAscD

		rjmp	PR_EDITRT


PR_EDITS7:
					;calculate parameter address
		ldi	r30,low(sparam7L)
		ldi	r31,high(sparam7L)
	
					;get 16 bit parameter
		ld	r16,Z
		ldd	r17,Z+1
					;if encoder_dec flag set, decrement parameter
		lds	temp,encoder_flag
		sbrs	temp,encoder_dec
		rjmp	PR_EDITS7_1
		subi	r16,low(param7_inc)
		sbci	r17,high(param7_inc)

		rjmp	PR_EDITS7_2


PR_EDITS7_1:
					;if encoder_inc flag set, increment parameter
		sbrs	temp,encoder_inc
		rjmp	PR_EDITS7_2		
		subi	r16,low(-param7_inc)
		sbci	r17,high(-param7_inc)

					;limit parameter value
PR_EDITS7_2:
		
		ldi	r18,low(param7_min)
		ldi	r19,high(param7_min)
		cp	r16,r18
		cpc	r17,r19
		brge	PR_EDITS7_3

		mov	r16,r18
		mov	r17,r19 

PR_EDITS7_3:
		
		ldi	r18,low(param7_max)
		ldi	r19,high(param7_max)
		cp	r18,r16
		cpc	r19,r17
		brge	PR_EDITS7_4

		mov	r16,r18
		mov	r17,r19 

				;save parameter and convert to ASCII string
PR_EDITS7_4:

		st	Z,r16
		std	Z+1,r17

		ldi	decimal_places,param7_dp
		
		call	Bin2SToAscD

	
PR_EDITRT:	ret



;********* Data Display Subroutine *********************************************


					;data display subroutine
					;enter with data_num (valid range 0 to 7)
					;and data_row_num (valid range 0-3 for large characters)

					;writes corresponding data to display	

DT_DSP:	lds	temp,data_num

		cpi	temp,8	;test for invalid param_num >=8
		brsh	DT_DSPS0

		clr	ZH
		mov	ZL,temp
		subi	ZL,low(-DT_DSPC0)	;add offset
		sbci	ZH,high(-DT_DSPC0)
		ijmp			;jump to offset

DT_DSPC0:	rjmp	DT_DSPS0	
DT_DSPC1:	rjmp	DT_DSPS1	
DT_DSPC2:	rjmp	DT_DSPS2	
DT_DSPC3:	rjmp	DT_DSPS3	
DT_DSPC4:	rjmp	DT_DSPS4	
DT_DSPC5:	rjmp	DT_DSPS5
DT_DSPC6:	rjmp	DT_DSPS6
DT_DSPC7:	rjmp	DT_DSPS7


DT_DSPS0:		
		ldi	r30,low(DATA_STRING0)
		ldi	r31,high(DATA_STRING0)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data0L
		lds	r17,data0H
		ldi	decimal_places,data0_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT
			
DT_DSPS1:	
		ldi	r30,low(DATA_STRING1)
		ldi	r31,high(DATA_STRING1)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 
		
		lds	r16,data1L
		lds	r17,data1H
		ldi	decimal_places,data1_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS2:
		ldi	r30,low(DATA_STRING2)
		ldi	r31,high(DATA_STRING2)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 
		
		lds	r16,data2L
		lds	r17,data2H
		ldi	decimal_places,data2_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS3:	
		ldi	r30,low(DATA_STRING3)
		ldi	r31,high(DATA_STRING3)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data3L
		lds	r17,data3H
		ldi	decimal_places,data3_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS4:	
		ldi	r30,low(DATA_STRING4)
		ldi	r31,high(DATA_STRING4)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data4L
		lds	r17,data4H
		ldi	decimal_places,data4_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS5:	
		ldi	r30,low(DATA_STRING5)
		ldi	r31,high(DATA_STRING5)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data5L
		lds	r17,data5H
		ldi	decimal_places,data5_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS6:	
		ldi	r30,low(DATA_STRING6)
		ldi	r31,high(DATA_STRING6)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data6L
		lds	r17,data6H
		ldi	decimal_places,data6_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

		rjmp	DT_DSPRT

DT_DSPS7:	
		ldi	r30,low(DATA_STRING7)
		ldi	r31,high(DATA_STRING7)
		ldi	r21,0
		lds	r22,data_row
		clr	r23
		call	StringF_Write8x16 

		lds	r16,data7L
		lds	r17,data7H
		ldi	decimal_places,data7_dp

		call	Bin2SToAscD

					;right justify 7 characters on row
		ldi	r21,(16-7)
		lds	r22,data_row
		clr	r23
		ldi	r24,7	
		call	StringS_Write8x16 

DT_DSPRT:	ret





;********* Interrupt Service Routines ******************************************




EXT_INT0:	reti
EXT_INT1:	reti
EXT_INT2:	reti
PC_INT0:	reti
PC_INT1:	reti
PC_INT2:	reti
PC_INT3:	reti	
WDT:		reti
TIM2_COMPA:	reti
TIM2_COMPB:	reti

				
					;service debounce and long range timers at 4.096 msec interval
TIM2_OVF:	in	temp_sr,SREG
		push	temp_sr	;save status
		push	r20
		push	r24

					;increment timer2H
		inc	timer2H
		brne	TIM2_OVF2

					;timers incremented at 1.048 sec interval

					;increment display_timer
					;test if display_delay_time elapsed
TIM2_OVF1:	lds	temp,display_timer
		inc	temp			
		lds	temp1,display_delay_time
		cp	temp,temp1
		brlo	TIM2_OVF1A
						
					;display_delay_time elapsed
					;clear display_timer, set flag and continue
		clr	temp		

		sbr	flag,1<<display_delay

TIM2_OVF1A:	sts	display_timer,temp


					;timers incremented at 4.096 msec interval

					;increment debounce_timer
					;test if input_debounce_time elapsed
TIM2_OVF2:	lds	temp,debounce_timer
		inc	temp			
		ldi	temp1,input_debounce_time
		cp	temp,temp1
		brlo	TIM2_OVF2A
						
					;input_debounce_time elapsed
					;clear debounce_timer, set flag and continue
		clr	temp		

		sbr	flag,1<<encoder_debounce

TIM2_OVF2A:	sts	debounce_timer,temp


		pop	r24
		pop	r20
		pop	temp_sr	;restore status
		out	SREG,temp_sr
		reti



TIM1_CAPT:	reti
TIM1_COMPA:	reti
TIM1_COMPB:	reti



					;timer1 overflow interrupt service routine
					;increments timer1B3 every .262 seconds
TIM1_OVF:	in	temp_sr,SREG
		push	temp_sr		;save status

		inc	timer1B3

		pop	temp_sr		;restore status
		out	SREG,temp_sr
		reti


TIM0_COMPA:	reti
TIM0_COMPB:	reti
TIM0_OVF:		reti
SPI_STC:		reti



					;RS-232 receive interrupt service routine
UART0_RXC:	in	temp_sr,SREG
		push	temp_sr	;save status
		push	r0
		push	r16
		push	r17
		push	r18
		push	r19
		push	r20
		push	r21
		push	r24
		push	r28
		push	r29
		push	r30
		push	r31


		lds	temp,UDR0	;get the received command
					;valid command range $10-$33

					;disable further RXC0 interrupts
		lds	r16,UCSR0B
		cbr	r16,1<<RXCIE0
		sts	UCSR0B,r16

		sei			;re-enable global interrupts

		cpi	temp,$10
		brlo	URXC_CMNUL	;test for invalid command <$10

		cpi	temp,$34	;test for invalid command >$33
		brsh	URXC_CMNUL


		subi	temp,$10	;subtract offset $10 for first command
					;jump to appropriate command routine

		clr	ZH
		mov	ZL,temp
		subi	ZL,low(-URXC_CMJ10)	;add offset
		sbci	ZH,high(-URXC_CMJ10)
		ijmp			;jump to offset
URXC_CMJ10:	rjmp	URXC_CMD10	;command $10: read firmware ID code
URXC_CMJ11:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ12:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ13:	rjmp	URXC_CMD13	;command $13: write to digital outputs
URXC_CMJ14:	rjmp	URXC_CMD14	;command $14: set address (EEPROM/FLASH)
URXC_CMJ15:	rjmp	URXC_CMD15	;command $15: read from EEPROM
URXC_CMJ16:	rjmp	URXC_CMD16	;command $16: write to EEPROM
URXC_CMJ17:	nop			;commands $17-$1D are reserved 
URXC_CMJ18:	rjmp	URXC_RET	;return - no response to invalid commands $17-$18
URXC_CMJ19:	rjmp	URXC_CMD19	;command $19: read FLASH
URXC_CMJ1A:	nop
URXC_CMJ1B:	nop
URXC_CMJ1C:	nop
URXC_CMJ1D:	nop
URXC_CMJ1E:	nop
URXC_CMJ1F:	rjmp	URXC_RET	;return - no response to invalid commands $1A-$1F
URXC_CMJ20:	rjmp	URXC_CMD20	;command $20: set LCD contrast
URXC_CMJ21:	rjmp	URXC_CMD21	;command $21: read debounced encoder switch
URXC_CMJ22:	rjmp	URXC_CMD22	;command $22: set LCD data
URXC_CMJ23:	rjmp	URXC_CMD23	;command $23: read parameter
URXC_CMJ24:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ25:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ26:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ27:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ28:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ29:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2A:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2B:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2C:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2D:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2E:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ2F:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ30:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ31:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ32:	rjmp	URXC_RET	;return - no response to invalid command
URXC_CMJ33:	rjmp	URXC_CMD33	;command $33: sync RS-232
URXC_CMNUL:	rjmp	URXC_RET	;return - no response to invalid command




					;command $10: read variable length
					;firmware ID code
					
					;load firmware ID starting address
URXC_CMD10:	ldi	r30,low(FIRMWARE_ID)
		ldi	r31,high(FIRMWARE_ID)

		lsl	r30
		rol	r31

					;selectively enable TXD, disable RXD
		ldi	temp1,$08
		sts	UCSR0B,temp1



URXC_CM101:	wdr				;reset watchdog and wait for UDRE0
		lds	temp,UCSR0A
		sbrs	temp,UDRE0	;(indicates UDR0 ready for more data)
		rjmp	URXC_CM101

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

		lpm			;firmware ID byte in r0
		sts	UDR0,r0	;send data

		adiw	r30,1		;increment pointer

		ldi	temp,$0D	;loop until terminator reached
		cp	r0,temp
		brne	URXC_CM101

		rjmp	URXC_RET	;done


					;command $13: write to PORTB outputs

URXC_CMD13:	wdr			;reset watchdog and wait for RXC0 
		lds	temp,UCSR0A
		sbrs	temp,RXC0
		rjmp	URXC_CMD13

		lds	temp,UDR0	;get the received data
	

		out	PORTB,temp	;write to the port


					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC



		ldi	r16,$06	;send acknowledge character
		sts	UDR0,r16


		rjmp	URXC_RET	;done



					;command $14: set address for subsequent
					;EEPROM operations 
					;following two bytes are cmd_addH:cmd_addL
					;low byte send first

URXC_CMD14:	wdr			;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD14

		lds	r16,UDR0	



URXC_CM141:	wdr			;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0
		rjmp	URXC_CM141

		lds	r17,UDR0

					;store received data
		sts	cmd_addL,r16
		sts	cmd_addH,r17

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC


		ldi	r16,$06	;send acknowledge character
		sts	UDR0,r16


		rjmp	URXC_RET	;done


					;command $15: read from EEPROM
					;post-increment address

URXC_CMD15:	ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;read EEPROM
		lds	r18,cmd_addH
		lds	r17,cmd_addL
		call	EERead

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		sts	UDR0,r16	;send EEPROM data


		lds	r31,cmd_addH	;post-increment address
		lds	r30,cmd_addL
		adiw	r30,1
		sts	cmd_addH,r31
		sts	cmd_addL,r30


		rjmp	URXC_RET	;done



					;command $16: write to EEPROM
					;post-increment address

URXC_CMD16:	wdr			;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0
		rjmp	URXC_CMD16

		lds	r16,UDR0	;get data

					;write to EEPROM
		lds	r18,cmd_addH
		lds	r17,cmd_addL

		call	EEWrite

		lds	r31,cmd_addH	;post-increment address
		lds	r30,cmd_addL
		adiw	r30,1
		sts	cmd_addH,r31
		sts	cmd_addL,r30

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC


		ldi	r16,$06	;send acknowledge character
		sts	UDR0,r16


		rjmp	URXC_RET	;done


					;command $19: read byte from FLASH
					;post-increment address
			
URXC_CMD19:	lds	r31,cmd_addH
		lds	r30,cmd_addL


		lpm			;read byte


		adiw	r30,1		;increment address

		sts	cmd_addH,r31	;store incremented address
		sts	cmd_addL,r30
		
					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp
		
		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

		sts	UDR0,r0	;send data


		rjmp	URXC_RET	;done



					;command $20: set LCD contrast
URXC_CMD20:	wdr		;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD20

		lds	r16,UDR0	;get the received data
				
		call	LCD_Contrast_Set	

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC


		ldi	r16,$06	;send acknowledge character
		sts	UDR0,r16


		rjmp	URXC_RET	;done



					;read debounced encoder switch
					
URXC_CMD21:	ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		lds	r16, encoder_flag			
	
		sts	UDR0,r16	;send data


		rjmp	URXC_RET	;done



					;command $22: set LCD data
					;get data_num, dataL, and dataH
URXC_CMD22:	wdr		;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD22

		lds	r16,UDR0	;get data_num (valid range 0-9)
		cpi	r16,9
		brlo	URXC_CMD221
		ldi	r16,9
	
URXC_CMD221:
					;calculate data address
		lsl	r16
		clr	r17
		ldi	r30,low(data0L)
		ldi	r31,high(data0L)
		add	r30,r16
		adc	r31,r17


URXC_CMD222:	wdr		;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD222

		lds	r16,UDR0	;get dataL

URXC_CMD223:	wdr		;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD223

		lds	r17,UDR0	;get dataH

					;store 16 bit data
		st	Z+,r16
		st	Z,r17
		
					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC


		ldi	r16,$06	;send acknowledge character
		sts	UDR0,r16


		rjmp	URXC_RET	;done



					;command $23: read parameter
					;get param_num, then send paramL and paramH
URXC_CMD23:	wdr		;reset watchdog and wait for RXC0
		lds	temp,UCSR0A
		sbrs	temp,RXC0		
		rjmp	URXC_CMD23

		lds	r16,UDR0	;get param_num (valid range 0-9)
		cpi	r16,9
		brlo	URXC_CMD231
		ldi	r16,9
	
URXC_CMD231:
					;calculate parameter address
		lsl	r16
		clr	r17
		ldi	r30,low(sparam0L)
		ldi	r31,high(sparam0L)
		add	r30,r16
		adc	r31,r17

					;get 16 bit parameter
		ld	r16,Z+
		ld	r17,Z
		
					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;send paramL
		sts	UDR0,r16



URXC_CM232:	wdr		;reset watchdog and wait for UDRE0
		lds	temp,UCSR0A
		sbrs	temp,UDRE0	;(indicates UDR0 ready for more data)
		rjmp	URXC_CM232

		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;send paramH
		sts	UDR0,r17

		rjmp	URXC_RET	;done



					;resync RS-232
					;does nothing except reply with ACK character
					
URXC_CMD33:	ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;selectively enable TXD, disable RXD
		ldi	temp,$08
		sts	UCSR0B,temp

		ldi	r16,$06

		sts	UDR0,r16	;send ACK character


		rjmp	URXC_RET	;done



URXC_RET:	wdr			;reset watchdog and wait for UDRE0 
		lds	temp,UCSR0A
		sbrs	temp,UDRE0	;(indicates UDR0 ready for more data)
		rjmp	URXC_RET


		ldi	temp,char_dly	;1 character delay
		call	DLY_MSEC

					;selectively enable RXD, disable TXD

		ldi	temp,$10
		sts	UCSR0B,temp


		lds	temp,UDR0	;dummy read to get rid of any data echo

					;re-enable RXC0 interrupts
		lds	r16,UCSR0B
		sbr	r16,1<<RXCIE0
		sts	UCSR0B,r16


		pop	r31		;return
		pop	r30
		pop	r29
		pop	r28
		pop	r24
		pop	r21
		pop	r20
		pop	r19
		pop	r18
		pop	r17
		pop	r16
		pop	r0
		pop	temp_sr	;restore status
		out	SREG,temp_sr
		reti




UART0_DRE:	reti
UART0_TXC:	reti
ANA_COMP:		reti
ADC_INT:		reti
EE_RDY:		reti
TWSI:		reti
SPM_RDY:		reti
UART1_RXC:	reti
UART1_DRE:	reti
UART1_TXC:	reti





;********* Text Strings and Graphics Data **************************************



SPLASH_STRING:			
.DB "DAYTONA SENSORS LLC "
.DB $0D,$00


DATA_STRING0:
.DB "DATA 0          "
.DB $0D,$00

DATA_STRING1:
.DB "DATA 1          "
.DB $0D,$00

DATA_STRING2:
.DB "DATA 2          "
.DB $0D,$00

DATA_STRING3:
.DB "DATA 3          "
.DB $0D,$00

DATA_STRING4:
.DB "DATA 4          "
.DB $0D,$00

DATA_STRING5:
.DB "DATA 5          "
.DB $0D,$00

DATA_STRING6:
.DB "DATA 6          "
.DB $0D,$00

DATA_STRING7:
.DB "DATA 7          "
.DB $0D,$00


PARAM_SCREEN_STRING:
.DB "PARAMETER ENTRY "
.DB $0D,$00

NEXT_PARAM_STRING:
.DB "NEXT PARAMETER  "
.DB $0D,$00

DATA_SCREEN_STRING:
.DB "DATA DISPLAY    "
.DB $0D,$00


PARAM_STRING0:			
.DB "PARAM 0 "
.DB $0D,$00

PARAM_STRING1:			
.DB "PARAM 1 "
.DB $0D,$00

PARAM_STRING2:			
.DB "PARAM 2 "
.DB $0D,$00

PARAM_STRING3:			
.DB "PARAM 3 "
.DB $0D,$00

PARAM_STRING4:			
.DB "PARAM 4 "
.DB $0D,$00

PARAM_STRING5:			
.DB "PARAM 5 "
.DB $0D,$00

PARAM_STRING6:			
.DB "PARAM 6 "
.DB $0D,$00

PARAM_STRING7:			
.DB "PARAM 7 "
.DB $0D,$00


;Daytona Sensors logo 128x64

fdaytona_sensors_graphic:
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$7F,$7F,$7F,$7F,$7F,$7F,$7F,$7F,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$7F,$7F,$3F,$3F,$1F,$1F,$1F,$0F,$8F,$87,$87,$C7,$C3,$C3,$E3,$E3,$E1,$E1,$F1,$F0,$F0,$F0,$F8,$F8,$F8,$F8,$F8,$F8,$FC,$FC,$F0,$C0,$03,$0F,$1F,$7F,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$7F,$7F,$3F,$1F,$0F,$0F,$87,$C7,$C3,$E1,$E1,$F0,$F8,$F8,$F8,$FC,$FC,$FE,$FE,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FE,$F8,$F0,$E0,$C1,$87,$07,$0F,$1F,$3F,$7F,$7F,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$7F,$3F,$1F,$0F,$87,$C3,$C3,$E1,$F0,$F8,$FC,$FC,$FE,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FE,$FE,$FC,$F8,$F8,$F0,$F0,$E1,$E1,$E3,$C3,$C3,$C7,$C7,$87,$87,$87,$87,$87,$87,$87,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$F8,$FC,$FE,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.DB $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00


;test graphic (not used)

frectangle64x40_graphic:
.DB 	$FF,$FF,$FF,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$FF,$FF,$FF
.DB 	$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF
.DB 	$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF
.DB 	$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF
.DB 	$FF,$FF,$FF,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$E0,$FF,$FF,$FF



;********* Utility Subroutines and Misc Include Files *************************


.include "Mega_math.inc"			;megaAVR math subroutines
							;for AVR with hardware multiplier

.include "Mega644P_EEPROM.inc"	;EEPROM subroutines


.include "Mega_LCD_Newhaven.inc"	;LCD subroutines


.include "Convert.inc"			;conversion routines


;********* Firmware ID Code  **************************************************


					;64 words reserved for firmware ID code
					;just before end of RWW section
					;location must not change, since it
					;is coded into optional boot loader section
					;(not used with this program version)
.org $7BC0
FIRMWARE_ID:			;firmware ID code
.DB "Daytona Sensors Graphics LCD Test Program Rev 1.0 CFS 2010"
.DB $0D,00			;terminator (required)



;********* Delay Subroutines ***************************************************


					;all delay routines are based on 16 MHz
					;clock oscillator 
.org $7FC0
					;5 and 10 usec delay routines
DLY_5US:	ldi	temp,5	;5 usec fixed delay
		rjmp	DLY_USEC

DLY_10US:	ldi	temp,10	;10 usec fixed delay
		rjmp	DLY_USEC

					;microsecond delay routine
					;call with microsecond delay value in temp
DLY_USEC:	nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		dec	temp
		brne	DLY_USEC
		ret


					;millisecond delay routine
					;call with delay value in temp
					;.255 msec steps
DLY_MSEC:	ldi	temp1,$FF
DLY_MSEC1:	nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		dec	temp1
		brne	DLY_MSEC1
		dec	temp
		brne	DLY_MSEC
		ret


;********* EEPROM Tables, Parameters, and Data *********************************

.ESEG
.ORG $0

					;dummy data - not used due to possible
					;corruption of location $00 when power is cycled
.DB           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0


.ORG $10
					;EEPROM parameters (copied to SRAM)
					;signed binary values
eparam_start:
eparam0:	.DW	100
eparam1:	.DW	200
eparam2:	.DW	300
eparam3:	.DW	400
eparam4:	.DW	5000
eparam5:	.DW	6000	
eparam6:	.DW	7000
eparam7:	.DW	8000



.CSEG				;allows boot loader to follow

		
