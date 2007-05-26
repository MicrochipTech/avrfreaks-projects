;.include "C:\Program Files\Atmel\AVR Studio\Appnotes\m8535def.inc"
;.MACRO enable		;?
;	sbi portc,$02
;	rcall lcd_delay50us
;	cbi portc,$02
;.ENDMACRO

.MACRO sending_delay2us	;?
	nop
	nop
.ENDMACRO


;Definition for the LCD-port
 .equ f=1000000	;frequency 1MHz
 .equ d1s=200;(1000000/5000=200)Booting delay time
 .equ d5ms=f/800;(1000000/800=1250) 5 ms Wait after each control word
 .equ d50us=f/20000;;(1000000/20000=50) 50 us Wait after each char
 .equ ddrc_op_en=0b11111111
 .equ dummy=0b00101000;0b00111000
 .equ enON=3
 .equ rsON=0b00000100
 .equ sys_select=0b00101000
 .equ cur_disp_shift=0b00010100
 .equ disp_off=0b00001000
 .equ disp_on=0b00001111
 .equ entry_mode=0b00000110
 .equ cursor_home=0b00000010
 .equ clr_disp=0b00000001
 




lcd_init:
		rcall lcd_delay1s	;?
		
		ldi temp,ddrc_op_en
		out ddrc,temp
		
		ldi temp,$03
dummy_loop:	
		rcall dummy_commands
		dec temp
		brne dummy_loop		
		
		ldi temp,sys_select	;0b00101000
		rcall lcd4command
		
		ldi temp,cur_disp_shift	;0b00010100
		rcall lcd4command
		
		ldi temp,disp_off	;0b00001000
		rcall lcd4command
		
		ldi temp,disp_on	;0b00001100
		rcall lcd4command
		
		ldi temp,entry_mode	;0b00000110
		rcall lcd4command
		ret	
		
lcd_clr:		
		ldi temp,clr_disp	;0b00000001
		rcall lcd4command
		ret
		
lcd_home:		
		ldi temp,cursor_home	;0b00000010
		rcall lcd4command
		ret
		
lcd4command:
		push temp	
		andi temp,$f0
		rcall portc_data
		pop temp
		swap temp
		andi temp,$f0
		rcall portc_data
		rjmp lcd_delay5ms
		ret
		
lcd4char:
		push temp
		andi temp,$f0
		sbr temp,rsON
		rcall portc_data
		pop temp
		swap temp
		andi temp,$f0
		sbr temp,rsON
		rcall portc_data
		rjmp lcd_delay50us ;ready ?
		ret
portc_data:
		out portc,temp
		nop			;?
		sbi portc,enON
		sending_delay2us
		cbi portc,enON
		nop			;?
		ret

dummy_commands:
		push temp
		ldi temp,dummy
		rcall portc_data
		rcall lcd_delay5ms	;?
		pop temp
		ret
	
lcd_delay1s:
		ldi temp,d1s
lcd_delay1sa:
		rcall lcd_delay5ms	
		dec temp
		brne lcd_delay1sa	
		ret
		
lcd_delay5ms:
		push zh
		push zl
		ldi zh,high(d5ms)
		ldi zl,low(d5ms)
lcd_delay5msa:
		sbiw zl,1
		brne lcd_delay5msa
		pop zl
		pop zh
		ret

lcd_delay50us:
		ldi temp,d50us
lcd_delay50usa:
		nop
		dec temp
		brne lcd_delay50usa
		ret
;lcd_line1:
;	ldi temp,$80
;	rcall lcd4command
;	ret

;lcd_line2:
;	ldi temp,$c0
;	rcall lcd4command
;	ret

lpm_loader:
lp3:	
	lpm 
	tst r0
	breq skip_lpm
	mov temp,r0
	rcall lcd4char
	adiw zl,$01	
	rjmp lp3
skip_lpm:
	ret		



lcdsqrinits:	ldi temp,$40
		rcall lcd4command 
		ldi temp,$01c
		rcall lcd4char
		ldi temp,$004
		rcall lcd4char
		ldi temp,$01f
		rcall lcd4char
		ldi temp,$000
		rcall lcd4char
		ldi temp,$000
		rcall lcd4char
		ldi temp,$000
		rcall lcd4char
		ldi temp,$000
		rcall lcd4char
		ldi temp,$000
		rcall lcd4char
		ret

lcd_line1:
	ldi temp,$80
	rcall lcd4command
	ret

lcd_line2:
	ldi temp,$c0
	rcall lcd4command
	ret