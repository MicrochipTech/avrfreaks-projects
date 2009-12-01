;***************************************************************************
;*
;* Title		: Keypad Control PWM
;* Version		: 1.0
;* Last updated	: 2009 29 08
;* Target		: ATmega16
;* Hardware requirements: LCD-interface.
;*
;* DESCRIPTION
;* This program demonstrates how to connect an LCD display with 16
;* characters and two lines to the portB of the  board.
;* The display should be an HD44780 or compatible type.
;* The program is written for the ATMega16 with a 4MHZ X-tal.
;* It also demonstrates the use of the keys on the LCDinterface.
;* 
;***************************************************************
.device	ATmega16
.include "m16def.inc"	

;Variable
.def		char	  =r0			;
.def        status    =r1
.def		digits	  =r13
.def		buffer	  =r16			;
.def		temp	  =r17			;
.def		lo_byte	  =r18
.def		hi_byte	  =r19
.def        tempw     =r20




.CSEG

.ORG		0x00        rjmp  init



		

Init:   
        cli	        
        ldi temp,high(ramend)
        out sph, temp                   ;set highbyte for sp
		clr temp
        ldi	temp,low(ramend)
		out	spl,temp		            ;set lowbyte for sp			
		ser	temp	                    ; set all bit in R18                              (Set all Bit in Register)
		out	DDRB,temp		            ; set PORTB as output	
        ldi temp,0b01001111
		out portd,temp


        rcall lcd_init
        rcall line1    	        	
       	ldi	ZL,LOW(Table17*2)		        
		ldi	ZH,HIGH(Table17*2)	        
		rcall tloop
		rcall line2
		ldi	ZL,LOW(Table18*2)		        
		ldi	ZH,HIGH(Table18*2)	        
		rcall tloop
		jmp simul



Table17:    .DB "Adjustable PWM  #",0
Table18:    .DB "Duty cycle=    %#",0

;Table19:    .DB "Adjustable PWM  #",0
;Table12:    .DB "Duty cycle=    %#",0



;**************************************************************   
; Subroutine delay
;      
;**************************************************************  

delay:	ldi	r28,20;50			
lus:	dec r27                                        ;r21
		brne lus
		dec	r28                                       ;r22
		brne lus
		ret	




;**************************************************************   
; Subroutines LCD
;      
;**************************************************************  

;Definitie van de stuurlijnen
;.equ		BF	= PB4			;Busy flag
.equ		RW	= PB6;			;Read/Write aan Pin PD5
.equ		RS	= PB7			;Register Select instructie/data aan Pin PD6
.equ		E	= PB5			;Enable aan Pin PD7

;LCD Instructions
.equ		clr_LCD		= 0b00000001	;clear display
.equ		home_LCD	= 0b00000010	;return home
.equ		entry_mode	= 0b00000110	;set Cursor
.equ		on_LCD		= 0b00001110	;LCD on
.equ        set_LCD4    = 0b00101000    ;4 bits,2 line,5x7dots
.equ        set_line1   = 0x80
.equ        set_line2   = 0xc0

		
LCD_inst:
		    cbi	    PORTB,RS    ; Stop RS to LCD,clear bit PB6(RS) in PORTB              (Clear bit in I/O register)
		    rjmp	LCD_write   ; relative jump to label: LCD_write
			
LCD_data:
		    sbi	 PORTB,RS		; Send RS to LCD, set(PB6) in PORTB                      (Set Bit in I/O register)
LCD_write:	in	 temp,PORTB     ; load PORTB content to register R18                     (Load an I/O Location to Register)
		    push buffer         ; push R16 on stack
		    swap buffer         ; swap nibble on R16
				
		    andi    buffer,0x0f ; clear upper nibble of R16                              (Logical AND with Immediate,)
		    or	    buffer,temp ; do bitwise OR between R18 and R16                      (Logical OR)
		    out	    PORTB,buffer ; write R16 content on PORTB                            (Store register to I/O location)		
		    sbi	    PORTB,E      ; Enable the LCD, set PB7 on portb                      (Set bit in I/O register)
		    rcall	delay	     ; call delay 10ms, relative call to label Watch         (Relative Call to Subroutine)
		    cbi	    PORTB,E      ; stop EN to LCD, clear PB7 on PORTB                    (Clear bit in I/O register)
		    pop	    buffer       ; load back R18 content from stack                      (Pop Register from Stack)
					
		
		    andi	buffer,0x0f  ; clear upper nibble of R16                             (Logical AND with Immediate,)
		    or	    buffer,temp  ; do bitwise OR between R18 and R16                     (Logical OR)
		    out	    PORTB,buffer ; write R16 content on PORTB                            (Store register to I/O location)
		    sbi	    PORTB,E      ; Enable the LCD, set PB7 on portb                      (Set bit in I/O register)
		    rcall	delay	     ; call delay
		    cbi	    PORTB,E      ; stop EN to LCD, clear PB7 on PORTB                    (Clear bit in I/O register)
		    clr	    temp         ; clear R18 content
		    out	    PORTB,temp   ; write  to PORTB		
		    ret                  
		
LCD_init:	
		    ldi	    buffer,home_lcd     ;load home_LCD to R16
	    	rcall	LCD_inst            ;call subroutine
		    ldi	    buffer,set_LCD4     ;LCD function set:4 bit mode,2 lines,5x7 font
	      	rcall	LCD_inst            ;call subroutine
	    	ldi	    buffer,on_LCD       ;load on_LCD to R16
	    	rcall	LCD_inst            ;call subroutine
	    	ldi	    buffer,entry_mode   ;load entry_mode to r16
	    	rcall	LCD_inst            ;call subroutine
            ldi	    buffer,clr_lcd      ;load clr_LCD to R16
	    	rcall	LCD_inst            ;call subroutine
	     	ret
		
	
;**************************************************************   
; Subroutine loading char
;**************************************************************  


		
		
tloop:	lpm				                ;                                                 (Load Program Memory)
		mov	temp,char                   ; copy R0 to R18
		cpi	temp,'#'		            ; Check whether table end reached,compare data # with R18 (Compare with Immediate)                 
		breq back                       ; If equal to # branch to Tloop1,                 (Branch if Equal)
		mov	buffer,char	            	; Jump if send buffer empty
		rcall	LCD_data
		inc	ZL 			                ;increment  next character
		brne tloop 		            	; next character                                  (Branch if not Equal)
 back:  ret

;*****************************************************************

line1: ldi buffer,0x80 ; 0x80=80h(set cursor position inst) +  00h(address of DDRAM)=80h  
       rcall LCD_inst
	   ret

line2: ldi buffer,0xc0; 0x80=80h(set cursor position inst) +  40h(address of DDRAM)=0c0h 
       rcall LCD_inst
	   ret


;*********************************************************************


.equ pulse_width = 0



simul:CLR TEMPW
      ldi tempw, pulse_width   ;Load pulse width
	  mov digits,tempw
	  rcall display_digit
    
load: out OCR2, tempw        ;OCR1A = Pulse width
      ldi temp, $69           ;8-bit PWM Mode
      out TCCR2, temp        ;Non Inverted
      in temp, DDRD           ;Make PortD.5 as o/p
      ori temp, (1<<7)
      out DDRD, temp


                            



decr: sbic pind,0x02
	  jmp incr
      cpi tempw,0
	  breq incr
	  dec tempw
	  ldi buffer,0xcb
	  rcall lcd_inst		 
      mov digits,tempw                     
	  rcall display_digit
	  call delay
	  jmp load

incr: sbic pind,0x03
      jmp decr
	  cpi tempw,255
	  breq decr
      inc tempw
      ldi buffer,0xcb
	  rcall lcd_inst
      mov digits,tempw
	  rcall display_digit
	  call delay
	  jmp load








display_digit:	
		mov	lo_byte,digits

bin2bcd:	
		clr	hi_byte
		clr temp

bBCD81:subi	lo_byte,10
		brcs	bBCD82
		inc	    hi_byte
		cpi hi_byte,10
		brne bBCD81
		inc temp
		clr hi_byte
		rjmp	bBCD81

bBCD82: subi	lo_byte,-10
        mov buffer,temp
		subi buffer,-48
		rcall LCD_data
		mov	    buffer,hi_byte
		subi	buffer,-48
		rcall	LCD_data
		mov	    buffer,lo_byte
		subi	buffer,-48
		rcall	LCD_data
		ret























