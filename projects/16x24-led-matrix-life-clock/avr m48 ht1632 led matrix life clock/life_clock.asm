;***************************************************************************
;* Title: AVR led matrix clock with life simulator   
;* Commercial use forbidden without license from author.  
;* (c) copyright 2008 Henry Carl Ott N2RVQ, All Rights Reserved.
;* Questions, comments or offers of cash? hcarlott@gmail.com  http://users.rcn.com/carlott/
;***************************************************************************

.nolist 
.include "m48def.inc"	
.list
.listmac

;***************************************************************************
;*
;* MACROS
;*
;***************************************************************************


.macro	_latch_data	  			; latch serial data into latch
		sbi	strobe_port,strobe
		cbi	strobe_port,strobe
.endm

;--- send single char to console
.macro	_send
	ldi	temp,@0 
	rcall	putc
.endm	

;--- send flash stored string to console
.macro	_send_flash
		ldi	zh,high(@0 * 2)
       	ldi	zl,low(@0 * 2) 	;Init Z-pointer
		rcall	flash_str2con
.endm

;--- load z pointer with arg
.macro _init_z
	ldi	zh,high(@0)
   	ldi	zl,low(@0) 	
.endm	

;--- load y pointer with arg
.macro _init_y
	ldi	yh,high(@0)
	ldi	yl,low(@0) 	
.endm	

.macro _init_x
	ldi	xh,high(@0)
    ldi	xl,low(@0) 	
.endm	

;--- load eeaddr hi/lo with arg, read value
.macro _get_ee
	ldi	xl,low(@0)
	ldi	xh,high(@0) 	
	rcall	eerd	
.endm	

;--- load eeaddr hi/lo with arg
.macro _init_ee
	ldi	xl,low(@0)
	ldi	xh,high(@0) 	
.endm	


;--- load z pointer with arg*2 for flash stored data (convert byte to word pointer) 
.macro	_init_zflash
		ldi	zh,high(@0 * 2)
   		ldi	zl,low(@0 * 2) 	;Init Z-pointer
.endm	

;--- load x pointer with arg*2 for flash stored data (convert byte to word pointer) 
.macro	_init_xflash
		ldi	xh,high(@0 * 2)
    	ldi	xl,low(@0 * 2) 	;Init x-pointer
.endm	

;--- skip next instruction
.macro	skip
	rjmp	(pc + 2)	; skip, next instruction	
.endm	


;------delay two cycles in one instruction
.macro	_nop_2
		rjmp	(pc + 1)
.endm	


;**************************************************************************
;     ATmega48 pin out
;                                                          
;
;**************************************************************************
;--- port and pin defines

;PORTB
.equ	NC_PB0		= 0x00 	; 
.equ 	NC_PB1		= 0x01	; 
.equ	NC_PB2		= 0x02	; 
.equ	NC_PB3		= 0x03	; 
.equ	NC_PB4		= 0x04	; 
.equ	NC_PB5		= 0x05	; 
.equ	NC_PB6		= 0x06	; xtal 1	
.equ	NC_PB7		= 0x07	; xtal 2

.equ	PORTB_DDR 	= 0
.equ	PORTB_PUP	= 0xff & (~(PORTB_DDR)) 		; pull ups

;PORTC
.equ	v_det		= 0x00 	
.equ	NC_PC1		= 0x01	
.equ	DATA		= 0x02	
.equ	WR_			= 0x03	
.equ	RD_			= 0x04	
.equ	CS_			= 0x05
.equ	NC_PC6		= 0x06	; np on dip	
.equ	NC_PC7		= 0x07	; np on dip

.equ	V_DET_PORT 	 = PINC

.equ	DATA_PORT	 = PORTC
.equ	RD_PORT		 = PORTC
.equ	WR_PORT		 = PORTC
.equ	CS_PORT		 = PORTC

.equ	PORTC_DDR	= 1<<CS_ | 1<<WR_ | 1<<RD_ | 1<<DATA 
.equ	PORTC_PUP	= 0xff & (~(PORTC_DDR | 1<<V_DET))

;PORTD
.equ 	RXD			= 0x00
.equ	TXD			= 0x01
.equ	spare		= 0x02	
.equ	ZC_IN		= 0x03	
.equ	LED			= 0x04		
.equ	ENC_A		= 0x05	
.equ	ENC_B		= 0x06	
.equ	ENC_PB		= 0x07	

.equ	encoder_port  = PIND	
.equ	ZC_PORT		  = PIND	
.equ	led_port 	  = PORTD

.equ	PORTD_DDR	= 1<<LED | 1<<TXD | 1<<spare 
.equ	PORTD_PUP	= 0xff & (~(PORTD_DDR| 1<<ZC_IN))					; pull ups

;---- some program equates and constants
.equ	clock 		  = 8000000			; cpu clock
.equ	console_baud  = 38400			; for debug

.equ	_60hz_divider = 120				; 60 hz zc mains divider	
.equ	_50hz_divider = 100				; 50 hz zc mains divider

.equ	buff_size  	 = 48				; primary buffer size


.equ	pb_inc		= 0x01
.equ	pb_dec		= 0x02
.equ	pb_sel		= 0x04
.equ	pb_long		= 0x08	
.equ	pb_pwr		= 0x80

.equ	sel_long_val = 3		; in seconds for a long press

.equ	rev_major	= 0x01	
.equ	rev_minor	= 0x00

.equ	num_x		= 24
.equ	num_y		= 16
.equ	num_cells	= num_x * num_y

;--- ascii char codes
.equ	ctrl_c 		= 0x03
.equ	etx			= 0x03
.equ	eot			= 0x04
.equ	bel 		= 0x07 		; bell
.equ	bs  		= 0x08		; back space
.equ	cr  		= 0x0d		; carriage return 
.equ	lf  		= 0x0a		; line feed
;-------------------------------------
.equ	T0_DIV1		=$01		; timer 0 divider values
.equ	T0_DIV8		=$02		;
.equ	T0_DIV64	=$03		;
.equ	T0_DIV256	=$04		;
.equ	T0_DIV1024	=$05		;

.equ	T1_DIV1		=$01		;timer 1 divider values
.equ	T1_DIV8		=$02		;
.equ	T1_DIV64	=$03		;
.equ	T1_DIV256	=$04		;
.equ	T1_DIV1024	=$05		;	

.equ	T2_OFF		=$00		; timer 2 divider values
.equ	T2_DIV1		=$01		
.equ	T2_DIV8		=$02	
.equ	T2_DIV32	=$03
.equ	T2_DIV64	=$04	
.equ	T2_DIV128	=$05
.equ	T2_DIV256	=$06	
.equ	T2_DIV1024	=$07	

; op codes for holtek driver chip
.equ	hol_cmd			= 0b10000000
.equ	hol_ram_wr		= 0b10100000

.equ	hol_sys_dis		= 0b00000000	
.equ	hol_sys_en		= 0b00000001
.equ	hol_led_off		= 0b00000010
.equ	hol_led_on		= 0b00000011	
.equ	hol_blk_off		= 0b00001000
.equ	hol_blk_on		= 0b00001001 	
.equ	hol_slave		= 0b00010000
.equ	hol_master	    = 0b00010100
.equ	hol_clk_rc		= 0b00011000
.equ	hol_clk_ext		= 0b00011100
.equ	hol_com_opt		= 0b00101100
.equ	hol_pwm			= 0b10100000
	

;*****************************
;* Global Register Variables *
;*****************************
.def	math_l		= r0
.def	math_h		= r1	
.def	t0_tic		= r3	; @ 500hz
.def	edit_var	= r4	; var edit temp storage  	
.def	cfg			= r5	; var mode being edited 
.def	edit_flags	= r6	; bit flags associated with var being edited	

.def	time_l		= r7	; fractional second counter
.def	time_m		= r8
.def	time_h		= r9

.def	mains		= r10	; mains zc divider
.def	buff_crc	= r11	; place to calc crc on buffer

.def	isr_save	= r14	; isr storage for status word
.def	zero		= r15	; always zero,  handy for 16 bit math, 	

.def	temp		= r16	; gp working reg 	
.def	temp2		= r17	; another gp reg
.def	ii			= r18

.def	sys_flags	= r21	; system bit flags, explained below 

.def	cell_x		= r22	; cols
.def	cell_y		= r23	; rows


;**************************
;-  system bit flags defined
;**************************
.equ	scrl_dir		= 0x00		; scroll direction
.equ	space_char	    = 0x01		; char is a space, display 5 blanks cols 
.equ	dc_op			= 0x03		; running on dc or battery
.equ	zerocross		= 0x04		
.equ	debug			= 0x07		; set debug mode
;-------------------------

.equ	clk_state		= GPIOR0
;**************************
;-  clock bit flags defined
;**************************
.equ	clk_halt	= 0x00		; set to halt rtc
.equ	am			= 0x01		; set if 00-11:59
.equ	sec_f		= 0x04		; bits set by rtc routine
.equ	min_f		= 0x05
.equ	hour_f		= 0x06
.equ	day_f		= 0x07	

;**************************
;-  edit argument bit fields
;**************************
.equ	IN_EE		= 0x00		; set if var is in EE, else ram
.equ	CHAR		= 0x01		; single ascii char
.equ	TIME		= 0x02		; time related arg	
.equ	HACK		= 0x03		; if var is edited, stop clock
.equ	ALPHA		= 0x04		; if argument to display is alpha string		
.equ	EXIT		= 0x05		; set last argument	in table (we can do this differently)

.DSEG

_seconds:			.byte	1		; current time/date
_minutes:			.byte	1
_hours:				.byte	1
_day:				.byte	1
_date:				.byte	1
_month:				.byte	1	
_year:				.byte	1

_1hz_divider:		.byte	1		; prescale reload val from line freq to 1hz

_crc_buffer:		.byte	16		; stores last 16 crc values	

_count_a:			.byte	1		; vertical counter storage for next sample
_count_b:			.byte	1		; used for debounce routine
_debounced:			.byte	1		; debounced status of inputs	
_pb:				.byte	1		; push button events, bits set by button press
_pb_tmr:			.byte	1	
_enc_old:			.byte	1
	
_disp_buff:			.byte	buff_size		; display buffer
_disp_zbuff:		.byte	buff_size		; for scroll effects
_cell_buff_o:		.byte	buff_size		; storage for current generation
_cell_buff_n:		.byte	buff_size		; new 	

_bcd_buff:			.byte	6		; bcd buffer
_asc_buff:			.byte	6


.equ		edit_var_size	= 11
_edit_var_id:		.byte	4		; same	
_edit_var_min:		.byte	1			
_edit_var_max:		.byte	1
_edit_var_location:	.byte	2		; address
_edit_var_atable:	.byte	2		; location of ascii table for alpha variables
_edit_var_flags:	.byte	1		; variable flag bit fields 


_random:			.byte	3		; storage for random number bits
_comm_timeout:		.byte	1
_second_tmr:		.byte	1		; counts down at 1 hz
_edit_buff:			.byte	1
;-------------------------

;*********************
;* Interrupt Vectors *
;*********************

.CSEG
.org		0x00
			rjmp	reset		; Reset Handle	
.org		INT0addr			; external int 0 ; 1hz int from rtc chip
			rjmp	reset
.org		INT1addr			; external int 1
			rjmp	zc_isr
.org		OVF2addr			; Overflow2 Interrupt Vector Address
			rjmp	t2_isr
.org		OC1Aaddr
			rjmp	reset
.org		OVF1addr  
			rjmp	reset
.org		OVF0addr
			rjmp	t0_isr
.org		URXCaddr			; USART Receive Complete Interrupt Vector Address
			rjmp	reset	
.org		UDREaddr			; USART Data Register Empty Interrupt Vector Address
			rjmp	reset
.org		ADCCaddr
			rjmp	reset

;--- external zc, set up to hit @120hz
zc_isr:		in		isr_save, sreg					; save status reg
			;push	temp	
		
			sbr		sys_flags,1<<zerocross			; set zc flag bit
				
			sbrc	sys_flags,dc_op					; if running dc, just exit
 			rjmp	zc_end	

			dec		mains							; counter
			brne	zc_end							; did not zero
			lds		mains,_1hz_divider				; update with divider 			
			rcall	rtc_update						; call clock

zc_end:		;pop		temp
			out		sreg,isr_save
 			reti


;--- hit at ???hz ------------------------------------------- 
t0_isr:		in		isr_save, sreg					; save status reg
			push	temp
			push	temp2
			push	zl
			push	zh								; and some work registers
					
			inc		t0_tic
;--			
db:			rcall	debounce

t0_end:		pop		zh								; restore work space  
			pop		zl
			pop		temp2
			pop		temp

			out		sreg,isr_save
			reti

;--- 1hz async from 32khc xtal
t2_isr:		in		isr_save,SREG				; save status					
			
			sbrc	sys_flags,dc_op				; if dc ops (or power down)
			rcall	rtc_update
			
			out		SREG,isr_save				; restore status
			reti


;***************************************************************************
;--- put char (temp) to console via int driven circ buffer
putc:		wdr						    ; uncomment these lines to not use the output char buffer

			lds		temp2,UCSR0A
			sbrs	temp2,UDRE0			; Is UART transmitter ready?
			rjmp	putc				; If not, wait
			sts		UDR0,temp	 		; Put character to UART
			ret
			
;------------------------------------------------------------------------
;--- start here on power up or external reset
;--- init ports, ints, and  zero all registers 		
reset:		ldi		temp,0
			_init_z	0x00						; bottom of registers	

clr_reg:	st		z+,temp 
			cpi		zl,0x1e						; don't zero z reg
			brne	clr_reg
	
			_init_z	SRAM_START					; beginning of ram past time storage

clr_ram:	st		z+,r16						; 	
			cpi		zl,LOW(RAMEND) 
			brne	clr_ram		
			cpi		zh,HIGH(RAMEND)
			brne	clr_ram	 	

;--- init stack pointer (somewhat important)
			ldi		temp,LOW(RAMEND) 	;setup stack pointer
			out		SPL,temp
			ldi		temp,HIGH(RAMEND)
			out		SPH,temp

;*******************************************************************
;--- set up io ports / pins 

			rcall	init_ports

;*******************************************************************
;--- set up timers    	

			rcall	init_tmr0				; start counter	
			rcall	init_tmr2				; async rtc 32khz

;*******************************************************************
;--- set up adc
 			;rcall		init_adc
				
;----------------------------------------------------------------------------
			rcall	init_uart
;-----------------------------------------------------------------------------
;--- init vars 
			sei	

			rcall	init_ext_int						; enable zero cross on ac mains
			rcall	get_line_freq		
	
			;rcall	init_tmr1
			
			rcall	init_pb								; get initial encoder state	

			rcall	init_random							; init random number generator
	
			ldi		temp,50								; default time to start up
			sts		_seconds,temp
			ldi		temp,58
			sts		_minutes,temp
			ldi		temp,11
			sts		_hours,temp

			ldi		temp,6
			sts		_month,temp
			sts		_day,temp
			sts		_year,temp
			sts		_date,temp

			_init_zflash	_hello						; hello world
			rcall	scrl_str
			_init_zflash	_spaces
			rcall	scrl_str

;---
main_updt:	 
			rcall	init_display					
			rcall	disp_pwr_mode					

main_s:		sbi		clk_state,sec_f					; for fast display sync		
			
main:		sbis	V_DET_PORT,V_DET				; test for power loss
			rjmp	pwr_down

			rcall	time_updt						; display the time		

			rcall	get_button
 			brcc	main							; nothing pressed, loop	

			cpi		temp,pb_long					; long button press, enter config mode
			breq	cfg_mode	

			lds		temp2,_minutes					; else quick change time, minuters and hours, no date here

m_inc:		cpi		temp,pb_inc						; if encoder changed cw, quickly inc the time
			brne	m_dec			

			inc		temp2
			cpi		temp2,60
			brlo	m_save
			clr		temp2
			
			lds		temp,_hours						; handle hours
			inc		temp
			cpi	 	temp,24	
			brlo	pc + 2	
 
			ldi		temp,0
			sts		_hours,temp						
			rjmp	m_save							; save the updated minutes

m_dec:		cpi		temp,pb_dec						; if encoder changed ccw, quickly set time back
			brne	main							; nope 	

			dec		temp2
			brpl	m_save				
			ldi		temp2,59						; 

			lds		temp,_hours						; fix hours
			dec		temp
			brpl	pc + 2
			ldi		temp,23
			sts		_hours,temp

m_save:		sts		_minutes,temp2					; update minutes
			rjmp	main_s	


;--- clock configuration mode, set time,date & vars that determine clock behaviour  
;--- will edit vars in ram and eeprom 
cfg_mode:	ldi		cell_x,3						
			ldi		cell_y,4
			rcall	clr_disp_buff					; 

			_init_zflash	_cfg					; string address 
			rcall	fstr2xy							
			rcall	buff2disp						; display cfg string
				
			rcall	discard_button					; toss press
			clr		cfg								; start at zero

;-- top of the cfg loop
cfg_mode_top:
			mov		temp,cfg						; get current var to possibly edit
			rcall	get_var							; get variable info from table into correct registers to edit 
	
			rcall	clr_disp_buff					; 			
			ldi		cell_x,0
			ldi		cell_y,0
			rcall	disp_var_id						; display var label
			rcall	buff2disp			

;--- wait for button press to inc/dec which var to edit, to enter the actual var edit mode, or to exit
cfg_mode_lp:
			rcall	get_button	
			brcc	cfg_mode_lp							; wait forever or power loss

cfg_v_edit:
			cpi		temp,pb_sel							; short press									
			brne	cfg_v_ee
			
			sbrc	edit_flags,exit						; if var is exit var, leave
			rjmp	cfg_exit			

			rcall	var_editor							; if not,  go and edit this var
			rjmp	cfg_mode_top						; next

;-- inc to next var, or dec to previous. handle roll over
cfg_v_ee:
			clr		math_l								; set low limit
			ldi		temp2,num_args-1					; set high limit
			mov		math_h,temp2
			mov		edit_var,cfg						; get copy 

;--
cfg_v_i:	cpi		temp,pb_inc							; inc var number						
			brne	cfg_v_ie
			rcall	inc_mod								; ++ with rollover
			rjmp	cfg_ve	
cfg_v_ie:

;--
cfg_v_d:	cpi		temp,pb_dec							; dec var number
			brne	cfg_d_e	
			rcall	dec_mod								; -- with roll under
cfg_ve:		mov		cfg,edit_var						; get back change val
			rjmp	cfg_mode_top						; to top
cfg_d_e:
	
;---
cfg_exit:
			out		clk_state,zero						; start clock
			rjmp	main_updt							; to the top

;--- actually edit the variable, save it properly when done
var_editor:
var_edit_top:
			rcall	disp_edit_var						; put on screen
var_edit_lp:
			sbrs	edit_flags,time						; is the varable part of the current time /date?
			rjmp	var_edit_gb
			
			sbis	clk_state,sec_f						; update display once a second		
			rjmp	var_edit_gb							; get button press	
			cbi		clk_state,sec_f						; clear seconds flag
			
			lds		yh,_edit_var_location					
			lds		yl,_edit_var_location+1
			ld		edit_var,y							; get time var

			rcall	disp_edit_var						; display it

			sbic	clk_state,clk_halt					; is clock stopped? (hacking mode)
			rjmp	var_edit_gb				

			ldi		cell_x,23							; if clock running blink cell to indicate
			ldi		cell_y,15
			rcall	clr_xy	
			
			lds		temp,_seconds		
			sbrc	temp,0
			rcall	set_xy
			rcall	buff2disp

var_edit_gb:
			rcall	get_button							; check for button
			brcc	var_edit_lp							; nope, loop and display 
			lds		math_l,_edit_var_min				; get var high and low limits	
			lds		math_h,_edit_var_max

edit_var_inc:
			cpi		temp,pb_inc							; and inc
			brne	edit_var_inc_e	

			sbrc	edit_flags,hack						; if var hack bit set, stop clock now
			sbi		clk_state,clk_halt

			rcall	inc_mod								; bump it with rollover support
			sbrc	edit_flags,time						
			rcall	save_edit_var						; if a time var, save it now
			rjmp	var_edit_top						; else loop

edit_var_inc_e:		

edit_var_dec:
			cpi		temp,pb_dec							; dec?
			brne	edit_var_dec_e	

			sbrc	edit_flags,hack						; support hacking bit field	
			sbi		clk_state,clk_halt
			
			rcall	dec_mod								; dec var, and if time, save now
			sbrc	edit_flags,time	
			rcall	save_edit_var
			rjmp	var_edit_top

edit_var_dec_e:

;--- 
edit_var_sel:
			cpi		temp,pb_sel							; select button, save the var
			brne	edit_var_sel_e		
			rcall	save_edit_var
edit_var_sel_e:

edit_var_end:
			ret

;--- save the var in the proper mem space (ram or eeprom) depending on bit field
save_edit_var:
			lds		xh,_edit_var_location
			lds		xl,_edit_var_location+1
			sbrc	edit_flags,in_ee
			rjmp	save_edit_v_ee
			st		x,edit_var
			ret

save_edit_v_ee:
			mov		temp,edit_var
			rcall	eewr
			ret


;--- display edited variable on screen. will display differently if var is string, char or int
disp_edit_var:
			rcall	clr_disp_buff		
			ldi		cell_x,0
			ldi		cell_y,0
			rcall	disp_var_id					; display label on top of screen

			ldi		cell_x,7
			ldi		cell_y,9			

			sbrc	edit_flags,alpha			; is an alpha string?
			rjmp	disp_edit_var_a

			sbrc	edit_flags,char				; is an ascii char?
			rjmp	disp_edit_var_c			

			mov		temp,edit_var				; display as int (limit 00-99)
			rcall	bin2bcd							

			push	temp
			mov		temp,temp2
			rcall	bcd2xy

			pop		temp
			andi	temp,0x0f	
			rcall	bcd2xy			
			
			rjmp	disp_edit_v_end

;--- display var as a single ascii char
disp_edit_var_c:
			ldi		cell_x,9					; location

			mov		temp,edit_var				; get it	
			rcall	char2xy						; dsiplay it
			rjmp	disp_edit_v_end

;--- display var as an ascii string from flash 
disp_edit_var_a:
			ldi		cell_x,3							; screen location
	
			lds		zh,_edit_var_atable					; get alpha table address	
			lds		zl,_edit_var_atable+1
	
			mov		temp,edit_var						; get value
			;rcall	send_byte							; debug
			lsl		temp								; x2
			lsl		temp								; x4
			
			add		zl,temp								; add to pointer
			adc		zh,zero

disp_edit_v_lp:
			lpm		temp,z+
			tst		temp
			breq	disp_edit_v_end	

			rcall	char2xy 	
			rjmp	disp_edit_v_lp
			
;--- display and out
disp_edit_v_end:			
			rcall	buff2disp	
			ret

;----------------------------------                     
;--- inc var mod math h/l                               
inc_mod:	inc		edit_var                
			cp		edit_var,math_h 
			breq	inc_mod_d               
			brlo	inc_mod_d               
			mov		edit_var,math_l 
inc_mod_d:	ret                                     
			                                
;--- dec var mod math h/l                               
dec_mod:	dec		edit_var	        
			cp		math_l,edit_var 
			breq	dec_mod_d	        
			brlt	dec_mod_d	        
			mov		edit_var,math_h 
dec_mod_d:	ret    


;--- display var label on screen
disp_var_id:
			push	ii
			_init_z	_edit_var_id
			ldi		ii,4				

disp_var_id_lp:			
			ld		temp,z+
			rcall	char2xy

			dec		ii
			brne	disp_var_id_lp	
			pop		ii
			ret
		
;--- get temp table entry edit var
get_var:	push	ii
			
			ldi		temp2,edit_var_size
			mul		temp,temp2						
			
			_init_x	_ee_edit_var					; point to first table entry past string							
			add		xl,math_l
			adc		xh,math_h		

			ldi		ii,edit_var_size		

			_init_y	_edit_var_id					; point to ram storage		

get_var_lp:	rcall	eerd_pp
			st		y+,temp							
			dec		ii
			brne	get_var_lp	

			lds		temp,_edit_var_flags
			mov		edit_flags,temp

			lds		xh,_edit_var_location			; get high byte
			lds		xl,_edit_var_location+1			; get low byte of addr

			ld		temp,x							; get from ram
			lds		edit_flags,_edit_var_flags
			sbrc	edit_flags,IN_EE				; if in eeprom read data
			rcall	eerd							; overwite temp	
			
			mov		edit_var,temp

			pop		ii								; out
			ret

;--- toss button press
discard_button:
			rcall	get_button
			brcc	discard_button	
			ret

;--- display current time base mode
;--- ac/dc 50hz/60hz use timers and zerocross
disp_pwr_mode:
			_init_zflash	_run
			rcall	scrl_str

			sbrs	sys_flags,dc_op
			rjmp	ac_op

			_init_zflash	_dc
			rcall	scrl_str
			rjmp	disp_p_end

ac_op:		ldi		temp,'6'						; assume AC 60hz operation
			
			lds		temp2,_1hz_divider
			cpi		temp2,_60hz_divider
			breq	pc + 2
			ldi		temp,'5'						; 50 hz			

			rcall	scrl_char
			_init_zflash	_hz
			rcall	scrl_str

disp_p_end:	_init_zflash	_spaces
			rcall	scrl_str
			ret

;---------------------------------------------------------------------------------	
;--- run the life simulation
run_game:	push	ii

			rcall	delay_long						; small  initial delay
			_get_ee	_ee_lrun						; how many seconds to run for
			sts		_second_tmr,temp				; will dec at 1 hz via isr			

			ldi		temp,16							; clr crc buffer 	
			_init_y	_crc_buffer						; we keep a snapshot of the crc of the last
			rcall	clr_buff_t						; 16 generations to check for short repeating cycles or no change
		
			_init_y		_cell_buff_o				; fill old buffer with random data
			rcall		random_fill
		
			_get_ee	_ee_random						; get random mode
			
			_init_x	_disp_buff						; src 
			_init_y	_cell_buff_o					; dest
			ldi		temp2,buff_size									


; different game modes start with different initial seed data
grand_0:	tst			temp					
			brne		grand_1					; zero, use current time display as seed
			rcall		copy_xy					; straight copy, overwrite random data with curent display data
			rjmp		run_g_lp				; and start game

grand_1:	cpi		temp,1						; random seed
			brne	grand_2						; leave random data nd go
			rjmp	run_g_lp

grand_2:	rcall	xor_xy						; not 0/1 xor time with random data


run_g_lp:	rcall	get_button					; look for quick out if button change
			brcs	run_game_exit

			rcall	do_generation				; one generation of cellular automata
		
			_init_x	_cell_buff_o				; copy old buff to display buff
			_init_y	_disp_buff
			ldi		temp2,buff_size									
			rcall	copy_xy
			
			rcall	buff2disp					; clock out to display
		
			_get_ee	_ee_lspeed					; delay between generations (speed)
			mov		ii,temp
			
run_g_del:	rcall	delay_ee					; lspeed times basic eedelay value
			dec		ii
			brne	run_g_del	

stag_tst:	ldi		ii,7						; check for blinkers or zero change
			_init_y	_crc_buffer+8

stag_lp:	ld		temp,y+						; see if last 7 CRCs are the same
			cp		temp,buff_crc			
			brne	blink_tst					; no, see if we are running blinkers

			dec		ii
			brne	stag_lp						; no change in 7 generations
			rjmp	run_game_exit

;--- test for blinker (cycling back and forth)
blink_tst:
			ldi		ii,7						
			_init_y	_crc_buffer
			ld		math_h,y+						; get two crcs
			ld		math_l,y+

blink_lp:	ld		temp,y+							; get two more
			ld		temp2,y+
			cp		math_h,temp			
			cpc		math_l,temp2
			brne	sec_tmr_tst						; different, get out			

			dec		ii
			brne	blink_lp
			rjmp	run_game_exit					; we are blinking (too boring)
 
sec_tmr_tst:										; see if we exceeded our maximum run time
			lds		temp,_second_tmr
			tst		temp
			brne	run_g_lp	


stagnant:	rcall	delay_long
			rcall	delay_long
			rcall	delay_long		

run_game_exit:				
			cbi		clk_state,min_f					; clear possible minute pending flag
			pop		ii
			ret


;do a single generation, old buffer to new
do_generation:
			push	ii
			_init_y	_cell_buff_n		; destination
			rcall	clr_buff			; clear dest buffer
			clr		cell_x				; start at 0,0
			clr		cell_y

do_gen_lp:	_init_y	_cell_buff_o		; where to read
			rcall	get_neighbors	
			
lonely:		cpi		temp,2				; kill cell, poor lonely cell
			brsh	stable	
			rjmp	gen_next

stable:		cpi		temp,3				; if 2 or 3 and alive, set
			brsh	birth
	
			rcall	get_xy
			breq	gen_next
			rjmp	gen_set_next

birth:		cpi		temp,4				; if 4, set
			brsh	gen_next

gen_set_next:
			_init_y	_cell_buff_n			; retrieve copy of pointer
			rcall	set_xy

gen_next:	inc		cell_x
			cpi		cell_x,num_x
			brlo	do_gen_lp

			clr		cell_x					; reset

			inc		cell_y
			cpi		cell_y,num_y			; range test
			brlo	do_gen_lp				; keep looping
 		

			_init_x	_cell_buff_n			; copy new back to old cell buffer
			_init_y	_cell_buff_o			;
			ldi		temp2,buff_size		
			rcall	copy_xy

			rcall	calc_crc				; run crc on old buffer
			
			ldi		ii,16					; 16 bytes in buffer
			_init_y	_crc_buffer				; point to storage
			
crc_store_lp:
			ldd		temp,y+1				; store new crc
			st		y+,temp					; and rotate older crcs down the buffer
			dec		ii
			brne	crc_store_lp	
		
			st		-y,buff_crc

			pop		ii
			ret							


;--- fill cell buffer with random data
random_fill:
			clr		cell_x
			clr		cell_y
	
random_f_lp:	
			rcall	random
			
			lds		temp,_random
			sbrc	temp,7
			rcall	set_xy

			inc		cell_x

			cpi		cell_x,num_x
			brlo	random_f_lp
			
			clr		cell_x

			inc		cell_y
			cpi		cell_y,num_y
			brlo	random_f_lp
 			ret

;--- simple 8 bit crc on buffer (the old buffer)
;--- think this routine is the same as the dallas one wire bus   
calc_crc:	
		push	ii				; need a counter
		clr		buff_crc		; zero dest
		_init_y	_cell_buff_o	; point to source
		
		ldi		ii,buff_size	; num bytes to check
		ldi		temp,0x8C		; get poly to register for speed    
						
crc_byte:
		ldi		temp2,8			; 8 bits in byte 
		ld		math_h,y+		; get byte
					
crc_bit:	
		mov		math_l,math_h	; save a copy
		eor		math_l,buff_crc	; xor copy with existing crc

		lsr		buff_crc		; rotate to next bit
		lsr		math_h			; rotate incoming byte to next bit position	
		
		sbrc	math_l,0		; if result of above xor on bit 0 was 1
		eor		buff_crc,temp	; xor in poly

		dec		temp2			; bit counter
		brne	crc_bit			; 
		dec		ii				; byte counter
		brne	crc_byte		; 
	
		pop		ii				; restore counter
		ret

;disp_neighbors:
;			clr		cell_x
;			clr		cell_y
;			_init_y	_cell_buff_o

;disp_n_lp1:	rcall	get_neighbors
;			rcall	send_nibble
;			_send	' '		
;
;			inc		cell_x
;			ldi		temp,num_x
;			cp		cell_x,temp
;			brlo	disp_n_lp1
;			
;			clr		cell_x
;
;			_send	cr		
;
;			inc		cell_y
;			ldi		temp,num_y
;			cp		cell_y,temp
;			brlo	disp_n_lp1
;
;			ret

;--- return neighbors of xy buffer @ z
get_neighbors:
			push	ii
			clr		ii			; cell counter

;x-1,y-1
test_0:		dec		cell_x		; x-1
			dec		cell_y		; y-1
			brmi	test_1		; outside of range							
			
			tst		cell_x		; test for x out of range
			brmi	test_1		; out of range

			rcall	get_xy		; test cell
			breq	pc + 2
			inc		ii

;x,y-1
test_1:		inc		cell_x			; restore x, no range test
			tst		cell_y			; test y again	
			brmi	test_2			; out of range		

			rcall	get_xy			; test cell
			breq	pc + 2
			inc		ii

;x+1,y-1
test_2:		inc		cell_x			; x+1
			cpi		cell_x,num_x	; compare limit
			brsh	test_3
			
			tst		cell_y
			brmi	test_3

			rcall	get_xy			; test cell
			breq	pc + 2
			inc		ii

;x-1,y
test_3:		inc		cell_y			; restore y
			subi	cell_x,2		; x-1
			brmi	test_4

			rcall	get_xy		; test cell
			breq	pc + 2
			inc		ii

;x+1,y
test_4:		subi	cell_x,-2		; x + 1
			
			cpi		cell_x,num_x	; compare limit
			brsh	test_5

			rcall	get_xy		; test cell
			breq	pc + 2
			inc		ii

;x-1,y+1
test_5:
			subi	cell_x,2		; x -1

			inc		cell_y			; y+1
			cpi		cell_y,num_y
			brsh	test_6			; y too high	

			tst		cell_x
			brmi	test_6			; x was negative	
			
			rcall	get_xy			; test cell
			breq	pc + 2
			inc		ii

;x,y+1
test_6:		inc		cell_x			; restore x
		
			cpi		cell_y,num_y
			brsh	test_7			; y still too high

			rcall	get_xy			; test cell
			breq	pc + 2
			inc		ii

;x+1,y+1
test_7:		inc		cell_x			; x +1
			cpi		cell_x,num_x
			brsh	test_end		

			cpi		cell_y,num_y
			brsh	test_end		; y still too high

			rcall	get_xy			; test cell
			breq	pc + 2
			inc		ii

test_end:	dec		cell_x			;restore x y
			dec		cell_y
			
			mov		temp,ii			; return value	
			pop		ii
			ret


;--- light led at xy position, buffer @ y		
clr_xy:		push		yl
			push		yh

			rcall		set_xy_addr
			com			temp			; invert mask			

			ld			temp2,y
			and			temp2,temp
			rjmp		set_xy_a


;--- light led at xy position, buffer @ y		
set_xy:		push		yl
			push		yh

			rcall		set_xy_addr
			
			ld			temp2,y
			or			temp2,temp
set_xy_a:	st			y,temp2	
			
			pop			yh
			pop			yl
			ret


;--- get status of xy, nz if led lit buffer @ z
get_xy:		push		yl
			push		yh

			rcall		set_xy_addr					; set z, get mask in temp

			ld			temp2,y						; get byte
			and			temp,temp2
			
			pop			yh
			pop			yl
			ret	

;--- set y to proper address, set mask
set_xy_addr:
			mov			temp,cell_x					; get x
			lsl			temp						; x2
			add			yl,temp						
			adc			yh,zero						; handle any carry				

			sbrc		cell_y,3					; odd even y byte
			adiw		yl,1

			mov			temp,cell_y			
			rcall		bit2mask
			ret

;--- copy buffer to display, 48 bytes
buff2disp:	push	yh
			push	yl
			push	ii

			_init_y	_disp_buff
	
			ldi		ii,buff_size		; how many bytes
			cbi		cs_port,cs_
			
			ldi		temp,hol_ram_wr	
			ldi		temp2,3
			rcall	hol_wr

			clr		temp				; start @ disp ram addr zero 
			ldi		temp2,7
			rcall	hol_wr

buff2disp_lp:
			ld		temp,y+				; get byte		
			ldi		temp2,8				; 8 bits			

buff2disp_lp1:
			lsr		temp			; move msb to carry
			
			cbi		data_port,DATA	; copy carry to pin
			brcc	pc + 2
			sbi		data_port,DATA
		
			cbi		wr_port,WR_		; strobe wr line
			sbi		wr_port,WR_
		
			dec		temp2
			brne	buff2disp_lp1

			dec		ii
			brne	buff2disp_lp	

			sbi		cs_port,cs_

			pop		ii
			pop		yl
			pop		yh
		
			ret
;---
hol_set_pwm:
			ori		temp,hol_pwm						

;--- write temp command to holtec chip
hol_wr_cmd:
			push	temp

			cbi		cs_port,CS_

			ldi		temp,hol_cmd				; write cmd op code	
			ldi		temp2,3
			rcall	hol_wr
			
			pop		temp						; send command
			ldi		temp2,8
			rcall	hol_wr	

			clr		temp						; dummy
			ldi		temp2,1
			rcall	hol_wr

			sbi		cs_port,CS_
			ret

;--- write temp2 bits to holtec, msb first 
hol_wr:		lsl		temp			; move msb to carry
			
			;brcs	pc + 2			; double test, no jitter
			cbi		data_port,DATA	; copy carry to pin
			brcc	pc + 2
			sbi		data_port,DATA
		
			cbi		wr_port,WR_		; strobe wr line
			sbi		wr_port,WR_
		
			dec		temp2
			brne	hol_wr		
			ret

;--- copy flash string to buffer @ y
fstr2xy:	lpm		temp,z+			
			tst		temp	
			brne	pc + 2
			ret
			rcall	char2xy					
			rjmp	fstr2xy	


bcd2dbuff:	_init_y	_disp_buff		; destination buffer
;---
disp_bcd:	rcall	clr_buff		; clear destination	

;--- copy bcd 
bcd2buff:	lds		temp,_bcd_buff + 0	; get Hh
			cpi		temp,0x0f
			breq	bcd_h	
			rcall	bcd2xy

bcd_h:		lds		temp,_bcd_buff + 1	; get hH
			rcall	bcd2xy
		
			_get_ee	_ee_col_char
			rcall	char2xy
			
			lds		temp,_bcd_buff + 2	; get Mm
			rcall	bcd2xy
			
			lds		temp,_bcd_buff + 3	; get mM
		
			rcall	bcd2xy
			ret

;--- display char @ xy in buffer pointed to by y
bcd2xy:		subi	temp,-'0'

char2xy:	push	ii	
			push	yh
			push	yl
			push	zh
			push	zl

			rcall	to_upper

			_init_zflash 	_font			; point to table
			
			subi	temp,32					; range convert
			
			ldi		temp2,5					; 5 byes per char
			mul		temp,temp2
			
			add		zl,math_l
			adc		zh,math_h				; z now points to correct font table entry		
		
			mov		temp,cell_x				; get x arg	
			lsl		temp					; *2, two bytes per column

			add		yl,temp
			adc		yh,zero					; y = dest 

			ldi		ii,5					; max bytes to copy

char2xy_lp:	lpm		math_l,z+				; get font data
			tst		math_l					; test for early end of font data
			breq	char2xy_end			
			inc		cell_x
			
			clr		math_h

			push	cell_y					; save y
char2_lpy:	tst		cell_y
			breq	char2xy_a					

			lsl		math_l
			rol		math_h

			dec		cell_y
			rjmp	char2_lpy		
			
char2xy_a:	pop		cell_y
			
			ld		temp,y					; get existing buffer data
			ldd		temp2,y+1
			
			or		temp,math_l				; or in font data
			or		temp2,math_h	
			
			st		y+,temp
			st		y+,temp2

			dec		ii
			brne	char2xy_lp

char2xy_end:
			inc		cell_x
			pop		zl
			pop		zh
			pop		yl						; restore
			pop		yh	
			pop		ii
			ret

;---------------------------------------------------------
;--- clr bcd buff to blank (0x0f)
clr_bcd_buff:
			_init_y	_bcd_buff
			ldi		temp,0x0f
			ldi		temp2,6

clr_bcd_lp:	st		y+,temp
			dec		temp2
			brne	clr_bcd_lp	
			sbiw	yl,6								; restore pointer
			ret	


clr_disp_buff:
			_init_y	_disp_buff	
;--- clear buff @ y
clr_buff:	ldi		temp,buff_size 						; 
clr_buff_t:	push	temp								; save size
			
clr_b_lp:	ldi		temp2,0x00
			st		y+,temp2
			dec		temp
			brne	clr_b_lp	
			
			pop		temp
			sub		yl,temp
			sbc		yh,zero								; restore pointer
			ret	

;---
xor_xy:		push	ii	
			
xor_lp:		ld		temp,x+
			ld		ii,y
			eor		temp,ii
			st		y+,temp

			dec		temp2
			brne	xor_lp

			pop		ii
			ret	

;--- copy temp2 bytes from y to z
copy_xy:	ld		temp,x+
			st		y+,temp

			dec		temp2
			brne	copy_xy
			ret	

;--- main time display ---------------------------
time_updt:	sbic	clk_state,hour_f
			rcall	hour_events
				
			sbic	clk_state,min_f						;
			rcall	min_events							;

			sbic	clk_state,sec_f						; wait for isr to set second bit	
			rcall	sec_events							; not yet
			ret

;---
hour_events:cbi		clk_state,hour_f		
			ret
;---			
min_events:	cbi		clk_state,min_f
			rcall	disp_time
			rcall	disp_ampm
			rcall	buff2disp

			_get_ee	_ee_date_mode
			rcall	test_min_event
			brcc	pc + 4
			rcall	sync_sec			
			rcall	disp_fdate
			rcall	sync_sec		

			_get_ee	_ee_lmode
			rcall	test_min_event
			brcc	pc + 4
			rcall	sync_sec
			rcall	run_game
			rcall	sync_sec

sec_events:	
			cbi		clk_state,sec_f					; clear for next second
			rcall	disp_time						; put time on display
			rcall	disp_seconds
			rcall	disp_ampm
			rcall	buff2disp						; 
			rcall	random			

sec_e_end:	ret	


sync_sec:	cbi		clk_state,sec_f					; clear for next second
			sbis	clk_state,sec_f
			rjmp	sync_sec + 1		
			ret

;--- divide current min by temp
test_min_event:
			tst		temp
			breq	test_m_f	

			lds		temp2,_minutes
			tst		temp2
			brne	pc + 2
			ldi		temp2,60
					
test_m_lp:	sub		temp2,temp
			brcc	test_m_lp
			add		temp2,temp
	
			tst		temp2	
			
			brne	test_m_f		
			sec
			ret

test_m_f:	clc
			ret

;--- disp fancy date display 
disp_fdate:
	
			ldi		temp,4
			cbr		sys_flags,1<<scrl_dir
			rcall	shift_ud
		
			rcall	delay_long			

			rcall	disp_day
			rcall	disp_delim
			rcall	disp_month
			rcall	disp_delim
			rcall	disp_date			

			rcall	disp_spaces

			rcall	delay_long

			ldi		temp,4
			sbr		sys_flags,1<<scrl_dir
			rcall	shift_ud
			rcall	disp_ampm
			rcall	buff2disp			
			ret

;--- display time ----------------
disp_time:	rcall	ftime2bcd
	
			ldi		cell_x,0						; position
			ldi		cell_y,4
						
			rcall	bcd2dbuff						; load buffer with formatted time

			dec		cell_x 							; remove last cellx inc	
			
			ldi		temp,num_x						; figure out width of time
			sub		temp,cell_x
			lsr		temp
		
			mov		cell_x,temp						; adhust to center
			rcall	bcd2dbuff						; re load buffer with offset to center
		
disp_time_end:
			ret		

;--- disp ampm pip at correct corner
disp_ampm:
			_get_ee	_ee_ampm
			tst		temp
			breq	disp_ampm_end	
			
			ldi		cell_x,0
			ldi		cell_y,0

			dec		temp			

			sbrc	temp,0
			subi	cell_x,-23	
			sbrc	temp,1
			subi	cell_y,-15	

			ldi		temp2,0x04
			sbic	clk_state,AM	
			eor		temp,temp2			

			sbrc	temp,2
			rcall	set_xy
disp_ampm_end:
			ret


;--- disp seconds on bar depending on mode
disp_seconds:
			_get_ee	_ee_sec_mode
			tst		temp
			brne	pc + 2
			ret

			push	ii	
			ldi		cell_x,9
			ldi		cell_y,14

			lds		math_l,_seconds	
	
			lsl		math_l
			lsl		math_l

			ldi		ii,6		; 6 bits to display
disp_sec_lp:
			lsl		math_l
			
			brcs	pc + 3	
			rcall	disp_0
			skip
			rcall	disp_1
			
			dec		ii
			brne	disp_sec_lp	

			pop		ii
			ret

disp_0:		inc		cell_y
			rcall	set_xy
			inc		cell_x
			dec		cell_y
			ret

disp_1:		rcall	set_xy
			inc		cell_x
			ret


;--- copy formatted time to bcd buff
ftime2bcd:	rcall	clr_bcd_buff
			_init_y	_bcd_buff
			cbi		clk_state,AM					; assume am 

			_get_ee	_ee_24hr						; detemine display mode
			tst		temp
			brne	disp_24hr						; if 1, display zulu time	
	
			lds		temp,_hours						; get hour
				
			subi	temp,12							; 24 to 12 hr format			
			brpl	pc + 3
			sbi		clk_state,AM
			subi	temp,-12						; undo sub
								
			tst		temp							; convert 00 into 12 / 12 hour mode
			brne	pc + 2
			ldi		temp,12

			cpi		temp,10							; leading zero suppression
			brsh	disp_hh

			adiw	yl,1							; bump
			st		y+,temp
			rjmp	disp_mmss					
		
disp_24hr:	lds		temp,_hours

disp_hh:	rcall	bin2buff
		
disp_mmss:	lds		temp,_minutes
			rcall	bin2buff
			lds		temp,_seconds
			rcall	bin2buff
			ret
;---
bin2buff:	rcall	bin2bcd
			st		y+,temp2
			andi	temp,0x0f	
			st		y+,temp
			ret

;--- scroll string to display
scrl_str:	lpm		temp,z+					; get char from codespace
			tst		temp					; test for end	
			brne	pc + 2
scrl_str_end:
			ret
			rcall	scrl_char				; send it
			brcs	scrl_str_end			; carry set, abort the string	
			rjmp	scrl_str				

;---
scrl_char:	push	zh
			push	zl
			push	ii	
					
			cbr		sys_flags,1<<space_char
			cpi		temp,' '
			brne	pc + 2
			sbr		sys_flags,1<<space_char

			_init_zflash 	_font			; point to table
			rcall	to_upper
		
			cpi		temp,32					; range test
			brsh	pc +2
			rjmp	scrl_end		

			subi	temp,32					; range convert
			
			ldi		temp2,5					; 5 byes per char
			mul		temp,temp2
			
			add		zl,math_l
			adc		zh,math_h				; z now points to correct font table entry		
		
			ldi		ii,5					; max bytes to copy

scrl_lp:	rcall	get_button
			brcs	scrl_end + 1
			
			lpm		temp,z+					; get font data
			tst		temp
			sbrs	sys_flags,space_char	; special handling for space char
			breq	scrl_p	
			push	temp
			
scrl_c_a:	rcall	delay_ee 
			rcall	shift_buff_ll			; make room for new line
			pop		temp					
			lsl		temp
			sts		_disp_buff + 47,temp
			rcall	buff2disp
			
			dec		ii
			brne	scrl_lp
			
scrl_p:		rcall	delay_ee 
			rcall	shift_buff_ll
			sts		_disp_buff + 47,zero
			rcall	buff2disp
		
			rjmp	scrl_end

scrl_end:	clc								; indicate all good
			pop		ii
			pop		zl
			pop		zh
			ret


;--- shift lower half of buffer left
shift_buff_ll:
			push	zh
			push	zl
	 		push	ii
	
			_init_z	_disp_buff + 1
			ldi		ii,num_x - 1			; how many cols			

shft_bu_ll_lp:
			ldd		temp,z+2
			st		z,temp
			adiw	zl,2					; next

			dec		ii
			brne	shft_bu_ll_lp	
		
			pop		ii
			pop		zl
			pop		zh
			ret

;--- shift disp up.down
shift_ud:	push	ii
			mov		ii,temp				; how many
			
shift_lp:	rcall	shift_buff
			rcall	buff2disp
			dec		ii
			breq	shift_end
			rcall	delay_ee			; delay ee tic rate	
			rjmp	shift_lp	
shift_end:
			pop		ii
			ret

;--- shift buff
shift_buff:	_init_z	_disp_buff
			push	ii
			ldi		ii,num_x				; how many cols

shft_bu_lp:	ld		math_l,z
			ldd		math_h,z+1	
			
			sbrc	sys_flags,scrl_dir
			rjmp	shift_dwn		

shift_up:	lsr		math_h
			ror		math_l
			rjmp	shift_st

shift_dwn:	lsl		math_l
			rol		math_h

shift_st:	st		z+,math_l
			st		z+,math_h			

			dec		ii
			brne	shft_bu_lp	
			
			pop		ii
			ret		

;--- clr buffers, and output drivers
init_display:
			cli
			sbi		data_port,DATA						
			sbi		rd_port,RD_
			sbi		wr_port,WR_
			sbi		cs_port,CS_		

disp_o_lp:		
			
			ldi		temp,hol_sys_dis
			rcall	hol_wr_cmd
			ldi		temp,hol_com_opt
			rcall	hol_wr_cmd
			ldi		temp,hol_master
			rcall	hol_wr_cmd
			ldi		temp,hol_sys_en
			rcall	hol_wr_cmd
			ldi		temp,hol_clk_rc
			rcall	hol_wr_cmd
			ldi		temp,hol_led_on
			rcall	hol_wr_cmd				
			ldi		temp,hol_blk_off					; paranoid
			rcall	hol_wr_cmd	

			ldi		temp,0x0f
			rcall	hol_set_pwm			

			sei

			_init_y	_disp_buff							; clear both buffers	

			ldi		temp,buff_size*2
			rcall	clr_buff_t			
			ret

;--- power down, reduce power to minimum, 1hz isr will wake us up,   
pwr_down:
			rcall	init_display

			out		TCCR0B,zero							; kill counter 0
			out		TCCR0A,zero
			sts		TCCR1B,zero							; kill counter 1				
			sts		TCCR1A,zero
			sts		TIMSK0,zero	

			;sts	UCSR0B,zero							; kill uart
			;rcall	clr_rs_buff							; kill comm buffer
	  	
			out		PORTB,zero							; zero out the ports
			out		PORTC,zero		
			out		PORTD,zero

			out		EIMSK,zero							; kill ext int (mains zc)			
			
			;sts		ADMUX,zero							; kill the adc
			;sts		ADCSRA,zero
						
			ldi		temp,1<<PRTWI	| 1<<PRSPI			; turn off twi (spi should already be off)
			sts		PRR,temp

			ldi		temp,1<<ACD							; kill analog comparator and bandgap												
			out		ACSR,temp	

			sbr		sys_flags,1<<dc_op					; going on batteries 
		
			ldi		temp,1<<CLKPCE						; drop clk speed
			sts		CLKPR,temp
			ldi		temp,0<<CLKPS3 | 1<<CLKPS2 | 1<<CLKPS1 | 0<<CLKPS0
			sts		CLKPR,temp

			ldi		temp,0<<SM2 | 1<<SM1 | 1<<SM0 | 1<<SE	; set power save mode
			out		SMCR,temp
		
pwr_down_a:	sleep										; 
			nop											; first  instructions after wakeup

			sts		OCR2A,zero							; a dummy write, we'll need to know when async updates	

pwr_tmr_lp:	lds		temp,ASSR							; wait till all busy bits clear
			andi	temp,0x1f
			brne	pwr_tmr_lp							; need tmr2 to update in async mode  or we'll get multiple ints

			sbi		led_port,led
			nop
			cbi		led_port,led

			sbis	V_DET_PORT,V_DET					; see if power is back
			rjmp	pwr_down_a							; nope go back to sleep  

wake_up:	out		SMCR,zero							; disable sleep mode	
			
			ldi		temp,1<<CLKPCE
			sts		CLKPR,temp
			sts		CLKPR,zero							; restore full speed clock	
		
			rcall	init_uart					

			sts		PRR,zero							; pwr up twi, (and others)

			rcall	init_ports							; turn on ports
									
			rcall	init_tmr0							; bring up timer (needed for delays)
			

			rcall	init_ext_int						; enable zerocross on ac mains
  			rcall	get_line_freq						; determine line freq
			; rcall	init_tmr1							; pwm brightness
			rcall	init_pb								; get switch status into _debounced	
			rjmp	main_updt							; to top	 


;--- init io ports
init_ports:
;--- PORTB setup						 
			ldi		temp,PORTB_DDR 						
			out		DDRB,temp			
			ldi		temp,PORTB_PUP						; pull ups on imputs  
			out		PORTB,temp				

;--- PORTC setup
			ldi		temp,PORTC_DDR
			out		DDRC,temp
			ldi		temp,PORTC_PUP						; pull ups on other pins  
			out		PORTC,temp

;--- PORTD setup
			ldi		temp,PORTD_DDR							 		
			out		DDRD,temp
			ldi		temp,PORTD_PUP						 ; pull ups on other pins  
			out		PORTD,temp
			ret

;----------------------------------------------------------------
;--- init uart
init_uart:

			ldi		temp,low((clock - 8 * console_baud) / (16 * console_baud))
			sts		UBRR0L,temp				
			ldi		temp,high((clock - 8 * console_baud) / (16 * console_baud))
			sts		UBRR0h,temp		
			
			ldi 	temp,(1<<UCSZ00) | (1<<UCSZ01)
			sts 	UCSR0C,temp
			ldi		temp,(1<<TXEN0) 
			sts		UCSR0B,temp
			ret

;*******************************************************************
;--- set up external int
init_ext_int:
			ldi		temp,0<<ISC01 | 1<<ISC00 | 0<<ISC11 | 1<<ISC10
			sts		EICRA,temp

			ldi		temp,1<<INT1
			out		EIMSK,temp
		
			ret

;******************************************************************
;--- init timer 2  (rtc)
init_tmr2:									
			ldi		temp, 1<<as2			; async mode	
 			sts		ASSR,temp
			
			sts		TCNT2,zero
			ldi		temp,T2_DIV128			; timer2  prescaler (128)
			sts		TCCR2B,temp

t2_lp:		lds		temp,ASSR				; wait till all busy bits clear
			andi	temp,0x0f
			brne	t2_lp

			ldi		temp,1<<TOIE2			; enable ints on overflow
			sts		TIMSK2,temp		
			ret			


;--- init timer 1
;init_tmr1:
			;ldi		temp, t1_div1 | 0<<WGM13 | 1<<WGM12
			;sts		TCCR1B,temp	
		
			;ldi		temp,0<<COM1A1 | 0<<COM1A0 | 1<<COM1B1 | 0<<COM1B0 | 0<<WGM11 | 1<<WGM10 
			;sts		TCCR1A,temp	
			
			;ldi		temp,0xff
			;sts		OCR1BL,temp

			;ldi		temp,1<<TOIE1
			;sts		TIMSK1,temp
		
			;ret
		

;--- init timer 0,
init_tmr0:
			ldi		temp,T0_DIV64												; 
			out		TCCR0B,temp													; Set the prescaler
			
			;ldi		temp,0<<COM0B1 | 0<<COM0B0 | 0<<COM0A1 | 0<<COM0A0 | 1<<WGM01 | 1<<WGM00		; set mode pwm 
			;out		TCCR0A,temp

			ldi		temp,1<<TOIE0
			sts		TIMSK0,temp
			ret

;---
disp_rev:	_init_zflash	_hello
			rcall	scrl_str
			ret

;----------------------------------------
;--- main real time clock update 
;--- call once a second
rtc_update:	push	temp									; save work regs
			push	temp2
			push	zl
			push	zh
		
			_send	'+'

			sbi		clk_state,sec_f							; set sec flag, clear in main

;- handle 1hz timers
sec_timers:	
			lds		temp,_second_tmr						; gp seconds timer
			tst		temp
			breq	pc + 2
			dec		temp
			sts		_second_tmr,temp	

			lds		temp,_pb_tmr							; get push button timer
			tst		temp									; already zero do nothing
			breq	sec_timers_d

			dec		temp									; dec &
			sts		_pb_tmr,temp							; save it
			brne	sec_timers_d							; did it zero?

			lds		temp,_pb								; get events
			sbr		temp,pb_long							; set long press
			sts		_pb,temp

sec_timers_d:
			
			sbic	clk_state,clk_halt						; if clock us halted, don't update time
			rjmp	rtc_exit

;---														; fine adjustments
			ldi		zl,0									; preload adjustment valure with 2^21	
			ldi		zh,0
			ldi		temp2,0x20								
			
			_get_ee	_ee_cal_plus							; get cal plus value
							
			add		zl,temp									; add it in
			adc		zh,zero
			adc		temp2,zero

			_get_ee	_ee_cal_minus							; get calibrate minus value
;---
			sub		zl,temp
			sbc		zh,zero
			sbc		temp2,zero								; and subtract it
;---
			add		time_l,zl								; now add adjust value to running count
			adc		time_m,zh
			adc		time_h,temp2

			mov		temp,time_h								; save  high byte	
			
			ldi		temp2,0b00011111						; clear top bits of running counter
			and		time_h,temp2							; save	
			
			swap	temp									; move top nibble to low
			lsr		temp									; 
			andi	temp,0b00000011							; 0=lose a second, 1=inc 1 sec (normal), 2=inc 2 seconds (add a sec)

			tst		temp									; 0?				
			breq	rtc_exit								; lose a sec					

			push	temp									; save
			rcall	rtc_inc									; inc time	
			pop		temp
	
			dec		temp									
			breq	rtc_exit								; add a sec to time?
			rcall	rtc_inc									; yp

rtc_exit:	
			pop		zh										; restore work space
			pop		zl
			pop		temp2
			pop		temp
			ret


rtc_inc:	_init_z	_seconds								; point to seconds
rtc_seconds:ld		temp,z									; get seconds
			inc		temp									; sec ++
			cpi		temp,60									; test for roll over
			breq	rtc_umin								; yes
			st		z,temp									; save updated seconds
			ret												; no roll, exit

rtc_umin:	sbi		clk_state,min_f							; set flag bit every minute
			st		z,zero									; zero & store seconds
			ldd		temp,z+1								; get current minutes
			inc		temp									; minutes ++
			cpi		temp,60									; hour roll over ?	
			breq	rtc_uhour	 							; yes		
			std		z+1,temp								; just save minutes	
			ret												; and out

rtc_uhour:	sbi		clk_state,hour_f						; hour flag bit every hour
			std		z+1,zero								; zero & store minutes	
			ldd		temp,z+2								; get hours
			inc		temp									; hour ++
			cpi		temp,24									; has a day passed?
			breq	rtc_uday	
			std		z+2,temp								; save hours			
			ret

rtc_uday:	sbi		clk_state,day_f							; set flag bit at midnight
			std		z+2,zero								; zero & store hours	
			
			ldd		temp,z+3								; get day of week 	
			inc		temp		
			
			cpi		temp,7
			brlo	pc + 2
			clr		temp									; 
			std		z+3,temp								; save

rtc_udate:	rcall	days_in_month							; get max days in the current month, adjusted for leap years
			ldd		temp2,z+4								; get date
			inc		temp2									; bump date
			cp		temp,temp2								; days in month > date ?
			brmi	rtc_umonth								; no

			std		z+4,temp2								; save date
			rjmp	rtc_inc_end

rtc_umonth:	ldi		temp,1
			std		z+4,temp								; reset date

			ldd		temp,z+5								; get month
			inc		temp
			cpi		temp,13
			breq	rtc_uyear			
			std		z+5,temp
			rjmp	rtc_inc_end

rtc_uyear:	ldi		temp,1									; roll over month to jan	
			std		z+5,temp								; and save it
		
			ldd		temp,z+6								; get year
			inc		temp									; time marches on	
			std		z+6,temp								; save

rtc_inc_end:
			ret
	

;--- return leap year corrected days in month
; return in temp, trash temp2 
days_in_month:
			ldd	temp,z+5		; get month	
			dec	temp			; 1-12  to 0-11

			push	zl			; save our time pointer
			push	zh

			_init_zflash		_month_table	

			add		zl,temp		; adjust pointer
			adc		zh,zero
			lpm		temp,z		; get month data

			pop		zh			; restore pointer
			pop		zl

			cpi		temp,28		; feb?
			breq	test_leap	
			ret					; no, just return	

test_leap:	ldd		temp2,z+6	; get year
			andi	temp2,3		; see if low two bits are zero (divide by 4)	
			brne	pc + 2		; no, not a leap year
			inc		temp		; 29 days in feb in a leap year
days_in_m_end:
			ret

_month_table:	.db	31,28,31,30,31,30,31,31,30,31,30,31


disp_spaces:	
			_init_zflash	_spaces
			rcall	scrl_str
			ret

disp_delim:	_get_ee	_ee_delim_char
			rcall	scrl_char
			ret	
;---
disp_day:	lds		temp,_day
			lsl		temp
			lsl		temp							;*4
			
			_init_zflash	_sunday
		
			add		zl,temp
			adc		zh,zero
			rcall 	scrl_str
			ret		
;---
disp_month:	lds		temp,_month
			dec		temp								
			lsl		temp
			lsl		temp								; *4
			
			_init_zflash	_jan
			
			add		zl,temp
			adc		zh,zero
			rcall 	scrl_str
			ret		

;----
disp_date:	lds		temp,_date
			rcall	bin2bcd
			andi	temp,0x0f							; toss msd	
			push	temp

			mov		temp,temp2
				
			tst		temp2
			breq	disp_date_a	
			
			mov		temp,temp2
			subi	temp,-'0'	
			rcall	scrl_char						
			
disp_date_a:
			pop		temp
			subi	temp,-'0'	
			rcall	scrl_char	

			lds		temp,_date		
			rcall	bin2bcd		
			andi	temp,0x0f	
			
			lds		temp2,_date
			cpi		temp2,11
			brlo	disp_suf	

			cpi		temp2,21
			brlo	disp_th

disp_suf:	dec		temp					
			cpi		temp,4
			brlo	pc + 2

disp_th:	ldi		temp,3
			lsl		temp							;x 2

			_init_zflash	_suf_table				; point to table

			add		zl,temp
			add		zh,zero
			lpm		temp,z+
			rcall	scrl_char	
			lpm		temp,z+
			rcall	scrl_char
					
disp_d_end:	ret

_suf_table:	.db		"STNDRDTH"						; allowed suffixes		

;---
delay_long:
			push	ii
			_get_ee	_ee_long_delay
			mov		ii,temp
			
del_l_lp:	rcall	delay_ee
			dec		ii
			brne	del_l_lp	

			pop		ii
			ret

;---
delay_ee:	_get_ee		_ee_scroll_speed

;--- delay temp number tics
;--- may go short  < tic 
delay_tic:	add		temp,t0_tic

del_tic_lp:	sbis	V_DET_PORT,V_DET					; check power	
			ret											; quick out
			cp		temp,t0_tic
			brne	del_tic_lp
			ret	

;--- delay temp number of system timer tics
;--- may go short 1 tic
;delay_t0_tic:
;			add		temp,t0_tic

;del_t0_lp:	sbis	V_DET_PORT,V_DET					; check power	
;			ret											; quick out
;			cp		temp,t0_tic
;			brne	del_t0_lp
;			ret	

;--- convert temp to mask
bit2mask:	andi	temp,0x07

			mov     temp2,temp			; get copy
        	ldi     temp,1
        	sbrc    temp2,1
        	ldi     temp,4
        	sbrc    temp2,0
        	lsl     temp
        	sbrc    temp2,2
        	swap    temp 
			ret	

;-----------------------------
;--- send nibbles as ascii		
send_hi_nibble:	
		swap	temp
send_nibble:
		rcall	hex2asc
		rcall	putc
		ret

;--------------------------------------
;--- send byte to console as two asc chars
send_byte:
		push	temp			; save temp
		rcall	send_hi_nibble	; send hi nibble
		pop		temp			; reget data	
		push	temp			; and save it again
		rcall	send_nibble
		pop		temp			; just restores temp
		ret

;--- routine converts 0-f hex in temp to 0 to F ascii
hex2asc:	
		andi   	temp,0b00001111     	;lower nybble only
   		cpi   	temp,10  		;0-9 or A-F?
   		brlo	hex_asc_1           	;
   		subi	temp,-7    		;adjust A-F
hex_asc_1:	
		subi   	temp,-48  		;add offset to convert to ascii
		ret


;************** base conversion routines *************************
;--- convert temp to upper case
to_upper:
		cpi 	temp,'f'	; mangle to handle limited font table					
		brlo	toupper_end
		cpi 	temp,'z'+1
		brsh	toupper_end
		subi	temp,' '
toupper_end:	ret


bin2bcd:	clr		temp2							; clear msd
b2bcd_1:	subi	temp,10							; sub 10
			brcs	b2bcd_2							; abort on carry 
			subi	temp2,-0x10						; add 0x10 to msd
			rjmp	b2bcd_1							; again

b2bcd_2:	subi	temp,-10						; un do last sub
			add		temp,temp2						; add msd to lsd	
			swap	temp2							; swap msd to low nibble
			ret

;--- copy codespace stored strings to console atmega specific
flash_str2con:	lpm		temp,z+						; get char from codespace
				tst		temp						; test for end	
				brne	pc + 2
				ret
				rcall	putc						; send it
				rjmp	flash_str2con

;------------------------------------------------------------------------------
;--- basic eeprom support, x contains eeaddr
eewr_pp:	rcall	eewr		; ee write w/inc of ee address
			rjmp	inc_eeaddr	
eerd_pp:	rcall	eerd		; ee read w/inc of ee address also tests temp before return
inc_eeaddr:	adiw	xl,1	
			tst		temp	
			ret	

;--- ee write temp
eewr:		sbic	EECR,EEWE	; if EEWE not clear
			rjmp	eewr		; keep waiting

			out		EEARl,xl
			out		EEARH,xh
			out		EEDR,temp	; output the data

			in		temp,sreg	; save current global int status
			cli					; turn off ints for write
			sbi 	EECR,EEMWE	; set master write enable	
			sbi		EECR,EEWE	; set EEPROM Write strobe
			sbrc	temp,7		; if ints were off, leave them off
			sei
			rjmp	ee_end

;--- ee read , returns in temp
eerd:		sbic	EECR,EEWE	; if EEWE not clear
			rjmp	eerd		; keep waiting

			out		EEARl,xl
			out		EEARh,xh
			sbi		EECR,EERE	; set EEPROM Read strobe

ee_end:		in		temp,EEDR	; get data	
			out		EEARh,zero	; clear ee address to prevent data corruption
			out		EEARl,zero		
			ret 

;----
get_button:	ldi		temp,pb_pwr		
			sbis	V_DET_PORT,V_DET	; test for power loss, quick out
			rjmp	get_hit				
	
			lds		temp,_pb			; get from ram	 	

			tst		temp
			clc								; assume fail
			breq 	get_button_end	
			sts		_pb,zero			; zero for next press	

get_hit:	sec							; mark hit
get_button_end:
			ret		
;----
Init_Random:                                                                         
			ldi		temp,0xaa	;Init the random number generator                    
			sts		_random + 0,temp	;since a 00,00,00 state will not                     
			sts		_random + 1,temp	;progress.                                           
			sts		_random + 2,temp	;                                                    
			ret                                                                          
                                                                                     
Random:                                                                              
	;Maximal legnth 19 bit shift register sequence,                              
		                                   
                                                                                     
	;  RAND3    RAND2    RAND1                                                   
	;22222111 11111110 00000000                                                  
	;43210987 65432109 87654321                                                  
                                                                                     
			lds		temp,_random + 0		;Make copy                                           
			lds		temp2,_random + 2	    ;Make copy                                           
                                                                                     		    
                                                                                     
			;Now we have to Xor the bits to see what                                     
			;goes in to bit 1                                                            
                                                                                     
			rol		temp2			;Move bit 19->20                                     
			andi	temp2,$08		;Important that D1,0 be zero                         
;                                                                                    
			andi	temp,$13		;Mask out irrelevants, protecting bit 19             
			or		temp2,temp		;Get bits 5->21 2->18 1->17                          
                                                                                    
			andi	temp2,$18		;Nuke bits 18,17                                     
			lsr		temp2			;19->19 5->20                                        
			lsr		temp2			;19->18 5->19                                        
			lsr		temp2			;19->17 5->18                                        
                                                                                    
			eor		temp2,temp		;First xor, result in R17 (5x2 in 02 and 19x1 in 01) 
			mov		temp,temp2		;Make a copy                                         
			ror		temp			;Move the 5x2 result to 01                           
			andi	temp2,$01		;Mask off everything else                            
			andi	temp,$01		;in both                                             
			eor		temp2,temp		;                                                    
			clc						; set carry based on eor	
			breq	pc + 2			;If one, do that, else                               
	        sec		
                                                       
    	
shift_in:   lds		temp,_random + 0                                                             
			rol		temp			
			sts		_random + 0,temp
			
			lds		temp,_random + 1                                                             
			rol		temp			
			sts		_random + 1,temp
			
			lds		temp,_random +2                                                             
			rol		temp			
			sts		_random + 2,temp
                   
			ret        


;div10:		ldi		temp2,10
;div_temp2:	ldi		temp,255	
;div_lp:		inc		temp
;			sub		math_l,temp2
;			sbc		math_h,zero			
;			brcc	div_lp			
;			add		math_l,temp2				; undo last sub
;			ret


crlf:		_send	cr
			_send	lf
			ret	

;--- determine line freq
get_line_freq:
			cbr		sys_flags,1<<dc_op		; start at known

			ldi		temp,t1_div1024	
			sts		tccr1b,temp				; start timer 1		
				
			sts		tcnt1h,zero				; zero it
			sts		tcnt1l,zero
	
			cbr		sys_flags,1<<zerocross
			rcall	wait_zc					; wait for zero cross	

			lds		temp,tcnt1l
			lds		temp,tcnt1h				; time out?

			tst		temp
			breq	test_5060				; no

			sts		_1hz_divider,zero
			sbr		sys_flags,1<<dc_op		; yes, set for dc operation
			ret

test_5060:	sts		tcnt1h,zero				; re-zero counter
			sts		tcnt1l,zero	

			rcall	wait_zc					; wait for two zc
			rcall	wait_zc
		
			lds		temp,tcnt1l				; get the time it took for 2 zc	
			lds		temp2,tcnt1h
			
			tst		temp2						
			brne	get_line_freq			; timed out, start over		

			ldi		temp2,_60hz_divider		; get proper prescaler
			cpi		temp,0x90				; tst limit (1/60hz) / (1/(clock/1024))  
			brlo	pc+2					; if 60 hz, leave flag alone
			ldi		temp2,_50hz_divider		; change divider for isr
			sts		_1hz_divider,temp2		; save in ram
			mov		mains,temp2				; also load mains divider for quick isr sync 	
			ret
;---
wait_zc:	lds		temp,tcnt1l
			lds		temp,tcnt1h
			tst		temp
			breq	pc + 2
			ret

			sbis	V_DET_PORT,V_DET		; check power	
			ret		

			sbrs	sys_flags,zerocross
			rjmp	wait_zc	
			cbr		sys_flags,1<<zerocross
			ret

;-- copy current switch status to reg, clear counters
init_pb:	clr		temp									; copy pins in order

			sbic	encoder_port,enc_a
			sbr		temp,1<<0		
			
			sbic	encoder_port,enc_b
			sbr		temp,1<<1		
			
			sbic	encoder_port,enc_pb
			sbr		temp,1<<2		
		 
			sts		_debounced,temp
			sts		_count_a,zero
			sts		_count_b,zero
			sts		_pb,zero
			sts		_pb_tmr,zero
			ret

;--- debounce push buttons vertical counter needs 4 identical samples to change state
;--- trash temp,temp2, zh,zl, button closures in ram @ _pb
debounce:	clr		temp2				; copy pins in order

			sbic	ENCODER_PORT,ENC_A
			sbr		temp2,1<<0		
			
			sbic	ENCODER_PORT,ENC_B
			sbr		temp2,1<<1	
			
			sbic	ENCODER_PORT,ENC_PB
			sbr		temp2,1<<2	
			
			lds		zh,_count_a			; get vertical counter from last sample
			lds		zl,_count_b
  
			eor		zh,zl				; increment the vertical counter
			com		zl
  		
			eor		temp,temp2			; temp still contains last state of debounced bins

			and		zl,temp				; reset the counter if no change has occurred
			and		zh,temp

			sts		_count_a,zh			; save vertical counter for next sample
			sts		_count_b,zl 

			or		zl,zh				; determine the counter's state
		
			lds		temp,_debounced		; get previous debounced	
			mov		zh,temp				; and save a copy

			and		temp,zl				; clear all bits that are filtered-or more accurately, save
			com		zl					; the state of those that are being filtered
			and		zl,temp2			; re-write the bits that haven't changed.
			or		temp,zl
										
			sts		_debounced,temp		; save debounced state
			eor		zh,temp				; check for changes from last time through
			breq	db_done				; nope, fast out 
			
sel_event:	sbrs	zh,2				; test for sel pb change	
			rjmp	enc_event			; nope	

			sbrc	temp,2
			rjmp	sel_release			; determine press / release

			ldi		temp2,sel_long_val	; start timer
			sts		_pb_tmr,temp2
			rjmp	enc_event

sel_release:
			sts		_pb_tmr,zero		; clear timer	
			lds		temp2,_pb			; get current events
			sbr		temp2,pb_sel	
			sts		_pb,temp2
;---
enc_event:	andi	zh,0x03
			breq	db_done				; any enc changes?			

			lds		temp2,_enc_old		; get last value
			lsl		temp2
			lsl		temp2				; move into position
			andi	temp2,0b00001100	; mask off unwanted	
					
			andi	temp,0x03			; temp, still has debounce state, mask off unwanted 
			
			or		temp,temp2			; or old with new
			sts		_enc_old,temp		; save for next time				

			lds		temp2,_pb			; get current events (ha) 		

			cpi		temp,0x07			; test for two possible valid codes
			breq	enc_inc
			cpi		temp,0x08
			breq	enc_inc
			
			cpi		temp,0x04			; test for two possible valid codes
			breq	enc_dec
			cpi		temp,0x0b			
			brne	db_done				; invalid or tween code

enc_dec:	sbr		temp2,pb_dec		; 1/2
			skip
enc_inc:	sbr		temp2,pb_inc	
			sts		_pb,temp2 
db_done:	ret
		

;*********************************************************************
; codespace string storage / cmd table
;*********************************************************************
_font:
; font num of chars = 64;
			.db	0x00, 0x00, 0x00, 0x00, 0x00,  0x5f, 0x00, 0x00, 0x00, 0x00	;  /!
			.db	0x03, 0x00, 0x03, 0x00, 0x00,  0x14, 0x7f, 0x14, 0x7f, 0x14	; "/#
			.db	0x24, 0x2a, 0x7f, 0x2a, 0x12,  0x23, 0x13, 0x08, 0x64, 0x62	; $/%
			.db	0x36, 0x49, 0x56, 0x20, 0x50,  0x0b, 0x07, 0x00, 0x00, 0x00	; &/'
			.db	0x3e, 0x41, 0x00, 0x00, 0x00,  0x41, 0x3e, 0x00, 0x00, 0x00	; (/)
			.db	0x08, 0x2a, 0x1c, 0x2a, 0x08,  0x08, 0x08, 0x3e, 0x08, 0x08	; */+
			.db	0x58, 0x38, 0x00, 0x00, 0x00,  0x08, 0x08, 0x08, 0x08, 0x08	; ,/-
			.db	0x60, 0x60, 0x00, 0x00, 0x00,  0x20, 0x10, 0x08, 0x04, 0x02	; .//
			.db	0x3e, 0x41, 0x41, 0x3e, 0x00,  0x42, 0x7f, 0x40, 0x00, 0x00	; 0/1
			.db	0x62, 0x51, 0x49, 0x46, 0x00,  0x22, 0x49, 0x49, 0x36, 0x00	; 2/3
			.db	0x18, 0x14, 0x12, 0x7f, 0x00,  0x27, 0x45, 0x45, 0x39, 0x00	; 4/5
			.db	0x3e, 0x49, 0x49, 0x32, 0x00,  0x01, 0x79, 0x05, 0x03, 0x00	; 6/7
			.db	0x36, 0x49, 0x49, 0x36, 0x00,  0x26, 0x49, 0x49, 0x3e, 0x00	; 8/9
			.db	0x36, 0x36, 0x00, 0x00, 0x00,  0x5b, 0x3b, 0x00, 0x00, 0x00	; :/;
			.db	0x08, 0x14, 0x22, 0x41, 0x00,  0x28, 0x28, 0x28, 0x28, 0x28	; </=
			.db	0x41, 0x22, 0x14, 0x08, 0x00,  0x02, 0x01, 0x51, 0x09, 0x06	; >/?
			.db	0x3e, 0x41, 0x5d, 0x55, 0x1e,  0x7e, 0x09, 0x09, 0x09, 0x7e	; @/A
			.db	0x7f, 0x49, 0x49, 0x49, 0x36,  0x3e, 0x41, 0x41, 0x41, 0x22	; B/C
			.db	0x7f, 0x41, 0x41, 0x22, 0x1c,  0x7f, 0x49, 0x49, 0x49, 0x41	; D/E
			.db	0x7f, 0x09, 0x09, 0x09, 0x01,  0x3e, 0x41, 0x49, 0x49, 0x3a	; F/G
			.db	0x7f, 0x08, 0x08, 0x08, 0x7f,  0x41, 0x7f, 0x41, 0x00, 0x00	; H/I
			.db	0x30, 0x40, 0x40, 0x40, 0x3f,  0x7f, 0x08, 0x14, 0x22, 0x41	; J/K
			.db	0x7f, 0x40, 0x40, 0x40, 0x00,  0x7f, 0x02, 0x04, 0x02, 0x7f	; L/M
			.db	0x7f, 0x04, 0x08, 0x10, 0x7f,  0x3e, 0x41, 0x41, 0x41, 0x3e	; N/O
			.db	0x7f, 0x09, 0x09, 0x09, 0x06,  0x3e, 0x41, 0x51, 0x21, 0x5e	; P/Q
			.db	0x7f, 0x09, 0x19, 0x29, 0x46,  0x46, 0x49, 0x49, 0x49, 0x31	; R/S
			.db	0x01, 0x01, 0x7f, 0x01, 0x01,  0x3f, 0x40, 0x40, 0x40, 0x3f	; T/U
			.db	0x1f, 0x20, 0x40, 0x20, 0x1f,  0x7f, 0x20, 0x10, 0x20, 0x7f	; V/W
			.db	0x63, 0x14, 0x08, 0x14, 0x63,  0x03, 0x04, 0x78, 0x04, 0x03	; X/Y
			.db	0x61, 0x51, 0x49, 0x45, 0x43,  0x7f, 0x41, 0x41, 0x00, 0x00	; Z/[
			.db	0x02, 0x04, 0x08, 0x10, 0x20,  0x41, 0x41, 0x7f, 0x00, 0x00	; \/]
			.db	0x04, 0x02, 0x7f, 0x02, 0x04,  0x40, 0x40, 0x40, 0x40, 0x40	; ^/_

_col_chars:	.db	0x40, 0x00, 0x00, 0x00, 0x00,  0x60, 0x60, 0x00, 0x00, 0x00	; 
			.db	0x22, 0x00, 0x00, 0x00, 0x00,  0x36, 0x36, 0x00, 0x00, 0x00	; 
			.db	0x08, 0x08, 0x08, 0x00, 0x00,  0x08, 0x08, 0x00, 0x00, 0x00	;  
;*********************************************************************
; codespace string storage 
;*********************************************************************
;--- message strings----------------
_hello:		.db	cr,lf,"LIFE CLOCK 1.0 (COPYRIGHT H. CARL OTT JUN-2008) ",cr,lf,0x00,0x00
_spaces:	.db	"     ",0
_cfg:		.db "CFG",0
_run:		.db	"RUN ",0,0
_dc:		.db	"DC",0,0
_hz:		.db	"0HZ",0


_sunday:	.db	"SUN",0
_mon:		.db	"MON",0
_tues:		.db	"TUE",0
_wed:		.db	"WED",0
_thur:		.db	"THU",0
_fri:		.db	"FRI",0
_sat:		.db	"SAT",0

_dummy_month:					; months are 1-12 
			.db	"DUM",0			; dummy for zero

_jan:		.db	"JAN",0
			.db	"FEB",0
			.db	"MAR",0
			.db	"APR",0
			.db	"MAY",0
			.db	"JUN",0
			.db	"JUL",0
			.db	"AUG",0
			.db	"SEP",0
			.db	"OCT",0
			.db	"NOV",0
			.db	"DEC",0

_null:		.db	0,0	

_off_on:	.db	"OFF",0
			.db	"ON",0,0
;*********************************************************************
; EEPROM 
;*********************************************************************
.eseg
.org				0x00

;_empty:				.db	0x00							; we leave empty, may possibly get corrupted
_ee_vars:
_ee_24hr:			.db	0	;0								
_ee_cal_plus:		.db	0	;9
_ee_cal_minus:		.db 0	;10
_ee_delim_char:		.db	'/'
_ee_scroll_speed:	.db	18
_ee_long_delay:		.db	12
_ee_sec_mode:		.db	1
_ee_date_mode:		.db 2	
_ee_lspeed:			.db 6
_ee_lmode:			.db	2
_ee_lrun:			.db	99
_ee_random:			.db	2
_ee_col_char:		.db	'a'
_ee_ampm:			.db	1	
.equ	num_args = 22
						; edit var table 
						; label to display on screen , var low limit, var high limit, address, bit flags to determine what kind of var
_ee_edit_var:
					.db	"HOUR",0,23,high(_hours),low(_hours),high(_null),low(_null),(1<<TIME)		
					.db	"MIN ",0,59,high(_minutes),low(_minutes),high(_null),low(_null),(1<<TIME)
					.db	"SEC ",0,59,high(_seconds),low(_seconds),high(_null),low(_null),(1<<TIME | 1<<HACK)
					.db	"DAY ",0,6,high(_day),low(_day),high(_sunday*2),low(_sunday*2),(1<<TIME|1<<ALPHA)
					.db	"DATE",1,31,high(_date),low(_date),high(_null),low(_null),(1<<TIME)
					.db	"MNTH",1,12,high(_month),low(_month),high(_dummy_month*2),low(_dummy_month*2),(1<<TIME|1<<ALPHA)
					.db	"YEAR",8,99,high(_year),low(_year),high(_null),low(_null),(1<<TIME)
					.db	"24HR",0,1,high(_ee_24hr),low(_ee_24hr),high(_off_on*2),low(_off_on*2),(1<<IN_EE|1<<alpha)
					.db	"AMPM",0,8,high(_ee_ampm),low(_ee_ampm),high(_null),low(_null),(1<<IN_EE)
					.db	"BSEC",0,1,high(_ee_sec_mode),low(_ee_sec_mode),high(_off_on*2),low(_off_on*2),(1<<IN_EE | 1<<ALPHA)
					.db	"CAL+",0,99,high(_ee_cal_plus),low(_ee_cal_plus),high(_null),low(_null),(1<<IN_EE)
					.db	"CAL-",0,99,high(_ee_cal_minus),low(_ee_cal_minus),high(_null),low(_null),(1<<IN_EE)
					.db	"DLIM",' ','e',high(_ee_delim_char),low(_ee_delim_char),high(_null),low(_null),(1<<IN_EE | 1<<CHAR)
					.db	"COLC",96,101,high(_ee_col_char),low(_ee_col_char),high(_null),low(_null),(1<<IN_EE | 1<<CHAR)
					.db	"SSPD",1,99,high(_ee_scroll_speed),low(_ee_scroll_speed),high(_null),low(_null),(1<<IN_EE)
					.db	"DLAY",1,99,high(_ee_long_delay),low(_ee_long_delay),high(_null),low(_null),(1<<IN_EE)
					.db	"DDSP",0,60,high(_ee_date_mode),low(_ee_date_mode),high(_null),low(_null),(1<<IN_EE)
					.db	"LSPD",1,99,high(_ee_lspeed),low(_ee_lspeed),high(_null),low(_null),(1<<IN_EE)
					.db	"LIFE",0,60,high(_ee_lmode),low(_ee_lmode),high(_null),low(_null),(1<<IN_EE)
					.db	"LRUN",2,99,high(_ee_lrun),low(_ee_lrun),high(_null),low(_null),(1<<IN_EE)
					.db	"RNDM",0,2,high(_ee_random),low(_ee_random),high(_null),low(_null),(1<<IN_EE)
					.db	"EXIT",0,0,high(_null),low(_null),high(_null),low(_null),(1<<EXIT)

