;===============================================================================
;                                - Relaes Form -
;===============================================================================
; New [X]                Update [X]           [ ]  Created   
;------------------------------------------------------------------------------- 
; Title :   timer 
; Author:  Ing. Frantisek Safarik                      Date:10-10-2006
; E-mail: (safarik@mag-net.sk)
;---------------------------------------------------------------------------------------------------------------- 
; Target AVR MCU       : ATTINY13
; Frq xtal             : 9.6MHz /4 prescaler = 2.4MHz 
; Include file         : 
; Language             : ASM/Atmel compiler/assembler v2 
; Dev. Tool            : AVR Studio	4.12.460 
; Code available to use: yes
; Filename             : timer.asm
; Fil zise  .hex       : 
;-----------------------------------------------------------------------------------------------------------------
; Description   : Program Casovac - zobrazuje minuty a sekundy, Countdown+ up + memory
; Input         : PIN PB3 (MIN/SEC/MEMoryStart)   

; Output        : PIN PB0 (Data - SI)
;		      PB1 (Strobe - Aktive H)
;		      PB2 (CLK)
;		      PB4 (sekundova dioda - des. bodka)  88 : 88
;		      PB5 (START)
;               : 
;
/* Note   
  Program timer zobrazuje minuty:sekundy 88:88 - ovladanie tlacitka MIN a SEC (nastavenie minut asekund) 
  tlacitko MEMory zapamatanie si casu *(moznost dvoch pamati A/B Min/Sec) tlacitko start
  spusta odcitavanie casu a zastavuje ho popripade pokracuje v odcitavani (counter down) taktiez pracuje ako timer 
  start 00:00 a skonci 99:59 popripade po zastaveni znovu pocita nahor
  sekundova dioda siganlizuje beh hodin 
  start dioda signalizuje start (beh) hodin na ovladacom tlacitku start
  - zmena pri vypocte 1sekundy (korekcia urobena meranim a naslednym vypoctom) 
  
*/ 
;==================================================================================================================
;CHange log.
;
;
;------------------------------------------------------------------------------------------------------------------
;Version       Date         Description
;1	     23.10.2007   - Zaciatok programu  

;
;
;
;
;
.nolist
.include 	"inc\tn13def.inc"
.list


;===================================  Register  ================================
;GLOBAL

.def		OUT_DISPLEJ		=r0					;hodnota na displej 
.def		OUT_KEYB		=r1					;hodnota pre klavesnicu a segme  obKKKKSSSS
.def		MH			=r2					;Minuty High
.def		ML			=r3					;Minuty LOW
.def		SH			=r4             			;Sekundy High
.def		SL			=r5					;Sekundy Low
.def		K_START			=r6					;keyboard START/STOP			
.def		K_MEMO			=r7					;keyboard Memory			
.def		K_MIN			=r8 					;keyboard Minuty
.def		K_SEC			=r9					;keyboard Sekundy
.def		TIMERL			=r10
.def		TIMERH			=r11
.def		MEMORY_S		=r12
.def		MEMORY_M		=r13					;pamat na cas
.def		XX3			=r14
.def		XX4			=r15

.def		TMP_A			=r16					;
.def		TMP_B			=r17					;
.def		TMP_1     		=r18                    		;
.def		TMP_2			=r19                    		;
.def		TMP_3       		=r20                    		;
.def		TMP_4			=r21                    		;

.def		xxx			=r22             			;citanie klavesnice
.def		xxxx			=r23             			;

.def		STATUS_KEYB		=r24             			;stlacenie klavesy
.def		STATUS_REG		=r25					;

.def		X0			=r26            			;X
.def		X1			=r27

.def		Y0			=r28					;Y
.def		Y1			=r29					; 
               
.def		Z0			=r30					;Z
.def		Z1			=r31					;    


;-------------------------------------------------------------------------------
;================================= Konstant ====================================

;+++++++++   Config PORT ++++++++++
;   	DD	PORT
;	0	0	Input   Z
;	0	1	Input	Pull up  (3)
;	1	0	Output	L
;	1	1	Output	H

 
.equ            IO_dirB                 =0b00110111				;
.equ            IO_portB                =0b00101111             		;   

;----------------------------------

.equ		DATA_PIN		=PINB0					;data
.equ		STR			=PINB1					;strobe
.equ		CLK			=PINB2					;clk hodiny
.equ		BUTTON			=PINB3					;klavesnica (min,sec, start, memo)
.equ		SEC_PIN			=PINB4					;sekundova dioda
.equ		START_LED		=PINB5


.equ		max_number		=10					;max cislo
.equ		max_sec_H		=6					;max. cislo pre sekundovku
.equ		Keyboard_Delay		=5					; 5x 0,032 
.equ		Keyboard_Repeat		=6					; 6x 0,032 
.equ		Keyboard_Start_on	=7					; 7x 0,032 =0,224s
.equ		Keyboard_Start_off	=156					;test 5sekund
.equ		Keyboard_Memo_on	=15
.equ		Keyboard_Memo_off	=200
.equ		cas_displaj		=3					;refresh displeja 3x 0,4 ms
.equ		cas_keyboard		=20					;refresh klavesnice 4x 0,008 = 0,032s vsetkych tlacitok 
.equ		cas_sekundy		=2425					;refresh sekundy 0,4mS*2500 - korekcia frekvencie 2425
.equ		timer_set_clock		=14					; 0,4ms

;for STATUS REGISTER r24

.equ		CASOVAC			=1					;if H - Casovac if L - Stopky
.equ		DISPA			=2					;if H - refresh high Minuty
.equ		DISPB			=3					;if H - refresh low Minuty
.equ		DISPC			=4					;if H - refresh high Sekundy
.equ		DISPD			=5					;if H - refresh low Sekundy
.equ		S_Stop			=6					;Start - H, Stop-L
.equ		B			=7
.equ		C			=8					;

;for STATUS KEYBOARD r25

.equ		KEY_START		=0					;if H - stlacene tlacitko start
.equ		KEY_MEMO		=1					;if H - stlacene tlacitko memo
.equ		KEY_MIN			=2					;if H - stlacene tlacitko Minuty
.equ		KEY_SEC			=3					;if H - stlacene tlacitko Sekundy
.equ		KEY_A			=4					;snimanie start
.equ		KEY_B			=5					;snimanie memo
.equ		KEY_C			=6					;snimanie min
.equ		KEY_D			=7					;snimanie sec


;


/*
;+++++++++  Config Program ++++++++
; X blik, O light; . Nolight    4 3 2 1
;				 1   0	svieti
;				 0   0  nesvieti
;				 X   1  blika    
; Led 1 set R4-2     Led2 set R3-1		
.equ		game_demo		= 0b10011010		;
.equ		game_start		= 0b10101100		;
.equ		game_test		= 0b10110100		;
.equ		game_meranie		= 0b11001110		;
.equ		game_koniec		= 0b10011000		;
.equ		game_error		= 0b10000111		;
.equ		game_test1 		= 0b10001101		;
.equ		game_test2 		= 0b10000101		;

;RS232  MENU  Setting
.equ		game_rs232		= 0b10000101		;
.equ		game_write		= 0b10011100		;
.equ		game_read		= 0b10100100		;
.equ		game_wave   		= 0b10111110		;

;  sample
; adresa  0-9  cisla 0,1,2,3,4,5,6,7,8,9
; adresa  10-13     gong, cakaj, priprav sa, fukaj
; adresa  14-16     cela, mas, promile alkoholu

*/

;FLASH address                                                                   
                                                                                 
;SRAM Address                                                                    
.equ		Y_INDEX_ADDR		=0x0060			
; Timer cache 
.equ 		TIMER_AL		=0x0070					
.equ 		TIMER_AH		=0x0071                 
.equ 		TIMER_BL		=0x0072			
.equ 		TIMER_BH		=0x0073                 
.equ 		TIMER_CL		=0x0074			
.equ 		TIMER_CH		=0x0075                 

.equ 		TIMER_DL		=0x0076					;not use
.equ 		TIMER_DH		=0x0077                 
.equ 		TIMER_EL		=0x0078
.equ 		TIMER_EH		=0x0079
.equ 		TIMER_FL		=0x007A
.equ 		TIMER_FH		=0x007B
.equ 		TIMER_GL		=0x007C
.equ 		TIMER_GH		=0x007D
.equ 		TIMER_L			=0x007E
.equ 		TIMER_H			=0x007F
.equ		TIMER_SET		=0x0080



; bity 7 6 5 4 3 2 1 0
;      S A r r X X X X
;      | | | | -------- Free for use X3,X2,X1,X0
;      | | | ---------- reserve  1
;      | | ------------ reserve 2 
;      | -------------- akcia   H-aktivne  L-neaktivne  (pri pretecenii casovaca) -nastavuje casovac
;      ---------------- nastavenie pretecenia H-aktivne  L-neaktivne  - ci sa ma uloha spustit ak A =H 


.equ		STATUS_REG_A		=0x0061					;REFRESH	 
.equ		STATUS_REG_B		=0x0062					;KEYBOARD	       
.equ		STATUS_REG_C		=0x0063 				;pocitadlo sekund
/*
.equ		STATUS_REG_D		=0x0064					;
.equ		STATUS_REG_E		=0x0065				        
.equ		STATUS_REG_F		=0x0066	
.equ		STATUS_REG_G		=0x0067	
*/



;EEPROM Address                                                                                                    
                                                                  

                                                                                                                   
;********************************* Bitove Konstanty *****************************************                      

;************************************ Makra ************************************************                       
                                                                                                                  
;################################ ZACIATOK PROGRAMU ########################################                       
.cseg                                   					;definicia CODE segmentu                                                   

                                                                                                      
.org            0x0000                  					;Reset Vector Addrres                                                      
                rjmp    RESET                                                                                      

.org            INT0addr                					;External Interrupt0 Vector Address                                        
.org            PCI0addr              						;External Interrupt Request 0
.org		OVF0addr							;Timer/Counter0 Overflow
.org		ERDYaddr							;EEPROM Ready
.org            ACIaddr                						;Analog Comparator
.org            OC0Aaddr                					;Timer/Counter Compare Match A

.org            OC0Baddr           						;Timer/Counter Compare Match B
		rjmp	TIMER_SERVICE
.org            WDTaddr             						;ADC Conversion Complete
.org            ADCCaddr                					;ADC Conversion Complete
		
                                                                                                                   
;##############################################################################                                    
;********************** I N I T I A L I Z A T I O N ***************************                                    
;##############################################################################                                    
RESET:                                                                                                             
;===============================================================================
;STACK_POINTER:
;===============================================================================
; Description    : sets stack pointer up
; uses   :           
; Input  : 
; Output :
; Note   :
;Version : 

		ldi     TMP_1, RAMEND
		out     SPL, TMP_1         ;Nastav zasobnik na koniec SRAM
		
;===============================================================================
;INI port A,B:
;===============================================================================
; Description    : sets ini port
; uses   :           
; Input  : 
; Output :
; Note   :
;Version : 

		;ini IO ports

		ldi     TMP_A, IO_portB
		out     PORTB, TMP_A
		
		;ini IO directions

		ldi     TMP_A, IO_dirB
		out     DDRB, TMP_A

;===============================================================================
;CONTROL REGISTERs:
;===============================================================================
; Description    : set any control register
; uses   :           
; Input  : 
; Output :
; Note   :
;Version : 		
		;ini Control Register
		ldi	TMP_A, 0b00000010
		out	MCUCR, TMP_A

		;ini Interrupts
		ldi     TMP_A, 0						; disable INT
		out     GIMSK, TMP_A

		;set timer (xtall)
		ldi	TMP_A, 0b10000000					;enable timer prescaler
		out	CLKPR, TMP_A
		ldi	TMP_A, 0b00000010					;f OSC /4 =2.4MHz	
		out	CLKPR, TMP_A

		
		ldi     TMP_A, (1<<OCIE0B)    					; enable OCIE0B 
		out     TIMSK0, TMP_A
		ldi	TMP_A,timer_set_clock
		out	OCR0B,TMP_A						;uloz casovas (0,0001 s)



		;ini ADC			
		ldi	TMP_A, 0						;disable
		out	ADCSRA, TMP_A
		

		;ini ANALOG
		ldi	TMP_A,0
		out	ADCSRB,TMP_A						;disable multiplexer (set AIN1)
		ldi	TMP_A, 1<<ACD						;disable Analog comparator + enable ref.voltage 
		out	ACSR,TMP_A


		;ini Timer 0                    				
		ldi	TMP_A, 0b00000000					;none
		out	TCCR0A, TMP_A
		ldi	TMP_A, 0b00000011					; fosc/64
		out	TCCR0B, TMP_A
		
;===============================================================================
;CLEAR MEMORY:
;===============================================================================
; Description    : clear memory, set konstant,set control bit
; uses   :           
; Input  : 
; Output :
; Note   :
;Version : 

		ldi	XH, high( 0x0060)					; clear SRAM memory
		ldi	XL, low( 0x0060) 
		ldi	YH, high( RAMEND)
		ldi	YL, low( RAMEND) 

		clr	STATUS_REG						; clear status register
CLR_SRAM:									; continue clear SRAM memory
		st	X+, STATUS_REG
		cp	XL, YL  
		cpc	XH, YH  
		brne	CLR_SRAM

		; ini variables
		ldi	YL, low( Y_INDEX_ADDR)					; Y = Y_INDEX_ADDR
		ldi	YH, high( Y_INDEX_ADDR)

		sei								; Enable Global Interrupt Flag      
		sbr	STATUS_REG,1<<DISPA
		sbr	STATUS_KEYB,1<<KEY_A
		cbr	STATUS_REG,1<<S_STOP
;		sbr	STATUS_KEYB,KEY_START
		ldi	TMP_A,0xF
		mov	TIMERL,TMP_A
		mov	TIMERH,TMP_A
		ldi	TMP_A, 0b11000000
		std 	Y+(STATUS_REG_A - Y_INDEX_ADDR),TMP_A
		std 	Y+(STATUS_REG_B - Y_INDEX_ADDR),TMP_A
		ldi	TMP_A, 0b00000000
		std 	Y+(STATUS_REG_C - Y_INDEX_ADDR),TMP_A

		ldi	TMP_A,0
		mov	SL,TMP_A
		ldi	TMP_A,0
		mov	SH,TMP_A
		ldi	TMP_A,0
		mov	ML,TMP_A
		ldi	TMP_A,0
		mov	MH,TMP_A


;############################################################################## 
;****************************** M A I N *************************************** 
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
MAIN:                                                                           
		
						;start play
		
		

;--- TIMER A - Display  -------------------
TIMER_A_TEST:
		ldd	TMP_A,Y+(STATUS_REG_A - Y_INDEX_ADDR)
		rol	TMP_A
		brcc	TIMER_B_TEST						;je nastaveny cas ??
		rol	TMP_A
		brcc	TIMER_B_TEST						;je aktivna funkcia ??
		rcall	DISPLAY
		nop
		
;--- TIMER B - KEYBOARD  ----------------------
TIMER_B_TEST:
		ldd	TMP_A,Y+(STATUS_REG_B - Y_INDEX_ADDR)
		rol	TMP_A
		brcc	TIMER_C_TEST						;je nastaveny cas ??
		rol	TMP_A
		brcc	TIMER_C_TEST						;je aktivna funkcia ??
		rcall	KEYBOARD
		nop
;--- TIMER C - TIMER ----------------------
TIMER_C_TEST:
		ldd	TMP_A,Y+(STATUS_REG_C - Y_INDEX_ADDR)
		rol	TMP_A
		brcc	TIMER_D_TEST						;je nastaveny cas ??
		rol	TMP_A
		brcc	TIMER_D_TEST						;je aktivna funkcia ??
		rcall	TIMER
		nop
TIMER_D_TEST:		
		rjmp	MAIN
		
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;===============================================================================
TIMER:
;===============================================================================
; Description    : Pocitanie casu  
; uses   : TMP_A, TMP_B,   
; Input  : STATUS_REG  (DispA,DispB,DispC,DispD)				;MM:SS  - AB:CD displej 
; Output : OUT_DISPLEJ,OUT_KEYB
; Note   : generuje hodnoty na displej pre jednotlive segmenty 
;Version :
		
		
		ldi	TMP_A,LOW (cas_sekundy)					;1s On 
		ldi	TMP_B,HIGH(cas_sekundy)
		add	TMP_A,TIMERL
		adc	TMP_B,TIMERH
		std	Y+( TIMER_CL - Y_INDEX_ADDR),TMP_A
		std	Y+( TIMER_CH - Y_INDEX_ADDR),TMP_B
		ldd	TMP_A,Y+(STATUS_REG_C - Y_INDEX_ADDR)
		andi	TMP_A,0b01111111					;deaktive
		std 	Y+(STATUS_REG_C - Y_INDEX_ADDR),TMP_A
		
		cbi	PORTB,SEC_PIN						;Low
		sbis	PINB,SEC_PIN						;if old = High
		sbi	PORTB,SEC_PIN						;HIGH
		
		sbrc	STATUS_REG,CASOVAC					;nastavene stopky L
		rjmp	timer_casovac						;nastaveny casovac H
;STOPKY - pocita nahor		

		ldi	TMP_A,max_number					;nastav cislo 10
		ldi	TMP_B,max_sec_H						;nastav cislo 6
		inc	SL							;sekundy
		cp	TMP_A,SL						; je vacsi ako 9
		brne	end_timer
		clr	SL
		inc	SH
		cp	TMP_B,SH
		brne	end_timer						;max hodnota 59 sekund
		clr	SL
		clr	SH
		
		inc	ML							;minuty						
		cp	TMP_A,ML						; je vacsi ako 9
		brne	end_timer
		clr	ML
		inc	MH
		cp	TMP_A,MH
		brne	end_timer						;max hodnota 99minut
		dec	TMP_A
		dec	TMP_B
		mov	MH,TMP_A
		mov	ML,TMP_A
		mov	SH,TMP_B
		mov	SL,TMP_A						;99:59
		sbr	STATUS_KEYB,1<<KEY_START				;end timer
		sbr	STATUS_REG,1<<CASOVAC
end_timer:	
		ret

timer_casovac:									;odpocitava cas
		ldi	TMP_A,max_number-1					;nastav cislo 9
		ldi	TMP_B,max_sec_H-1					;nastav cislo 5
		
		dec	SL
		brpl	end_timer						;ak je kladne 9-0 ak je minus 255 (FF) pretiekol 
		mov	SL,TMP_A						;xx:x9
		
		dec	SH
		brpl	end_timer						;max hodnota 59 sekund
		mov	SL,TMP_A
		mov	SH,TMP_B						;hodnota xx:59
		
		dec	ML
		brpl	end_timer						;ak je kladne 9-0 ak je minus 255 (FF) pretiekol 
		mov	SL,TMP_A
		mov	SH,TMP_B
		mov	ML,TMP_A						;x9:59
		
		dec	MH
		brpl	end_timer						;max hodnota 59 sekund
		clr	MH
		clr	ML
		clr	SH
		clr	SL							;hodnota 99:99
		sbr	STATUS_KEYB,1<<KEY_START					;zastav cas
		cbr	STATUS_REG,1<<CASOVAC
		ret
		

		
;===============================================================================
DISPLAY:
;===============================================================================
; Description    : OUTPUT for DISPLAY 
; uses   : TMP_A, TMP_B,   
; Input  : STATUS_REG  (DispA,DispB,DispC,DispD)				;MM:SS  - AB:CD displej 
; Output : OUT_DISPLEJ,OUT_KEYB
; Note   : generuje hodnoty na displej pre jednotlive segmenty 
;	   register  OUT_KEYB                                        OUT_DISPLEJ
;                ob   K    K    K    K    S   S   S   S            ob G F E D C B A
;                   Sekundy            D- SecLow                    7 - seg. display
;                        Minuty            C- SecHigh
;                             Memory           B- MinLow
;                                  Start           A- MinHigh

;Version :

		ldi	TMP_A,LOW (cas_displaj)						;0,00333s On 
		ldi	TMP_B,HIGH(cas_displaj)
		add	TMP_A,TIMERL
		adc	TMP_B,TIMERH
		std	Y+( TIMER_AL - Y_INDEX_ADDR),TMP_A
		std	Y+( TIMER_AH - Y_INDEX_ADDR),TMP_B
		ldd	TMP_A,Y+(STATUS_REG_A - Y_INDEX_ADDR)
		andi	TMP_A,0b01111111					;deaktive
		std 	Y+(STATUS_REG_A - Y_INDEX_ADDR),TMP_A

		ldi	ZL, low(Table*2)                               		;nastav tabulku na 0      
		ldi	ZH, high(Table*2)     
		mov	TMP_A,OUT_KEYB						;nacitaj register klavesnice
		andi	TMP_A,0b11110000
		
		sbrc	STATUS_REG,DISPD
		rjmp	DISPLEJD
		sbrc	STATUS_REG,DISPC
		rjmp	DISPLEJC
		sbrc	STATUS_REG,DISPB
		rjmp	DISPLEJB
	
DISPLEJA:
		add	ZL,MH							;nastav ukazovatel v tabulke na cislo 
		lpm								;nacitaj cislo do OUT_DISPLEJ
		
		ori 	TMP_A,0b00000001					;MH  Minuty High
		mov	OUT_KEYB,TMP_A
		sbr	STATUS_REG,1<<DISPB
		cbr	STATUS_REG,1<<DISPA
		ret
DISPLEJB:
		add	ZL,ML							;nastav ukazovatel v tabulke na cislo 
		lpm								;nacitaj cislo do OUT_DISPLEJ
		ori 	TMP_A,0b00000010					;ML Minuty Low
		mov	OUT_KEYB,TMP_A
		sbr	STATUS_REG,1<<DISPC
		cbr	STATUS_REG,1<<DISPB
		ret

DISPLEJC:
		add	ZL,SH							;nastav ukazovatel v tabulke na cislo 
		lpm								;nacitaj cislo do OUT_DISPLEJ
		ori 	TMP_A,0b00000100					;SH Sekundy High
		mov	OUT_KEYB,TMP_A
		sbr	STATUS_REG,1<<DISPD
		cbr	STATUS_REG,1<<DISPC
		ret

DISPLEJD:
		add	ZL,SL							;nastav ukazovatel v tabulke na cislo 
		lpm								;nacitaj cislo do OUT_DISPLEJ
		ori 	TMP_A,0b00001000					;SL sekundy low 
		mov	OUT_KEYB,TMP_A
		sbr	STATUS_REG,1<<DISPA
		cbr	STATUS_REG,1<<DISPD
		ret

		
;===============================================================================
KEYBOARD:
;===============================================================================
; Description    : KEYBOARD service
; uses   : TMP_A, TMP_B, STATUS_KEYB       
; Input  : PB3 (Sec,Min,Mem,Start/Stop) ,IN_K
;          
; Output : 
; Note   : Kontrola klavesnice
;Version :	
		
		ldi	TMP_A,LOW (cas_keyboard)				;0,003s On 
		ldi	TMP_B,HIGH(cas_keyboard)
		add	TMP_A,TIMERL
		adc	TMP_B,TIMERH
		std	Y+( TIMER_BL - Y_INDEX_ADDR),TMP_A
		std	Y+( TIMER_BH - Y_INDEX_ADDR),TMP_B
		ldd	TMP_A,Y+(STATUS_REG_B - Y_INDEX_ADDR)
		andi	TMP_A,0b01111111					;deaktive
		std 	Y+(STATUS_REG_B - Y_INDEX_ADDR),TMP_A
		
		mov	TMP_A,OUT_KEYB

		sbrc	STATUS_KEYB,KEY_D	
		rjmp	keyboard_D
		sbrc	STATUS_KEYB,KEY_C	
		rjmp	keyboard_C
		sbrc	STATUS_KEYB,KEY_B	
		rjmp	keyboard_B
;---------------		
keyboard_A:		
		
		mov	TMP_B,K_START						;nacitaj K_Start
		sbic	PINB,BUTTON						;nastalo stlacenie klavesy ?
		rjmp	not_pressed_A
		dec	TMP_B
		brne	not_new_A
		sbr	STATUS_KEYB,1<<KEY_START				;nastalo stlacenie start klavesy
		ldi	TMP_B,Keyboard_Start_off				;pokial neuvolni do daneho casu znova sa spusti
		rjmp	not_new_A
not_pressed_A:	
		ldi	TMP_B,Keyboard_Start_on					;uloz novu hodnotu
not_new_A:					
		mov	K_START,TMP_B
		cbr	STATUS_KEYB,1<<KEY_A
		sbr	STATUS_KEYB,1<<KEY_B					;set keyb B
		andi	TMP_A,0b00001111
		ori	TMP_A,0b11010000					;set keyb B
		mov	OUT_KEYB,TMP_A
		ret
;---------------
keyboard_D:	
		mov	TMP_B,K_SEC
		sbic	PINB,BUTTON						;nastalo stlacenie klavesy ?
		rjmp	not_pressed_D
		dec	TMP_B
		brne	not_new_D
		sbr	STATUS_KEYB,1<<KEY_SEC					;nastalo stlacenie SEC klavesy
		mov	TMP_B,K_MIN
		cp	TMP_B,K_SEC
		brne	not_clear_keyb_D
		rcall	CLEAR_TIME	
not_clear_keyb_D:		
		ldi	TMP_B,Keyboard_Repeat					;uloz novu hodnotu
		mov	K_MIN,TMP_B
		rjmp	not_new_D
not_pressed_D:	
		ldi	TMP_B,Keyboard_Delay					;uloz novu hodnotu
not_new_D:					
		mov	K_SEC,TMP_B
		cbr	STATUS_KEYB,1<<KEY_D
		sbr	STATUS_KEYB,1<<KEY_A					;set keyb A
		andi	TMP_A,0b00001111
		ori	TMP_A,0b11100000					;set keyb A
		mov	OUT_KEYB,TMP_A
		ret

;---------------
keyboard_C:
		mov	TMP_B,K_MIN
		sbic	PINB,BUTTON						;nastalo stlacenie klavesy ?
		rjmp	not_pressed_C
		dec	TMP_B
		brne	not_new_C
		sbr	STATUS_KEYB,1<<KEY_MIN					;nastalo stlacenie SEC klavesy
		mov	TMP_B,K_SEC
		cp	TMP_B,K_MIN
		brne	not_clear_keyb_C
		rcall	CLEAR_TIME	
not_clear_keyb_C:
		ldi	TMP_B,Keyboard_Repeat					;uloz novu hodnotu
		mov	K_SEC,TMP_B
		rjmp	not_new_C
not_pressed_C:	
		ldi	TMP_B,Keyboard_Delay					;uloz novu hodnotu
not_new_C:					
		mov	K_MIN,TMP_B
		cbr	STATUS_KEYB,1<<KEY_C
		sbr	STATUS_KEYB,1<<KEY_D					;set keyb D
		andi	TMP_A,0b00001111
		ori	TMP_A,0b01110000					;set keyb D
		mov	OUT_KEYB,TMP_A
		ret

;---------------
keyboard_B:	
		mov	TMP_B,K_MEMO
		sbic	PINB,BUTTON						;nastalo stlacenie klavesy ?
		rjmp	not_pressed_B
		dec	TMP_B
		brne	not_new_B
		sbr	STATUS_KEYB,1<<KEY_MEMO					;nastalo stlacenie SEC klavesy
		ldi	TMP_B,Keyboard_Memo_off					;pokial neuvolni do daneho casu znova sa spusti
		rjmp	not_new_B
not_pressed_B:	
		ldi	TMP_B,Keyboard_Memo_on					;uloz novu hodnotu
not_new_B:
		mov	K_MEMO,TMP_B
		cbr	STATUS_KEYB,1<<KEY_B
		sbr	STATUS_KEYB,1<<KEY_C					;set keyb C
		andi	TMP_A,0b00001111
		ori	TMP_A,0b10110000					;set keyb C
		mov	OUT_KEYB,TMP_A
		ret

;===============================================================================
REFRESH:
;===============================================================================
; Description    : Refresh displeja,klavesnice, sekund. diody 
; uses   : TMP_1, TMP_2,TMP_3   
; Input  : OUT_DISPLAJ,OUT_KEYB							;MM:SS  - AB:CD displej, KKKK SSSS klavesnica segmenty
; Output : OUT_PIN PB0,PB1,PB2,PB4
; Note   : zobrazuje hodnoty na displeji, refresh klavesnice, Sekundovu diodu
;Version :


		mov	TMP_3,OUT_DISPLEJ					;nacitaj hodnotu pre display
		ldi	TMP_1,2							;registre 2 x
refreshR3x:                                                                                                 
		ldi	TMP_2,8							;Data  8x
refreshD8x:		                                                                         
		rol	TMP_3
		sbi	PORTB, DATA_PIN						;DATA  H
		brcs	ref_D8xx
		cbi	PORTB, DATA_PIN	
		nop								;DATA  L
ref_D8xx:		
		cbi	PORTB, CLK			
		dec	TMP_2
		sbi	PORTB, CLK						;Set CLK
		brne	refreshD8x
			
		mov	TMP_3,OUT_KEYB
		dec	TMP_1
		brne	refreshR3x
		
		sbi	PORTB, STR						;Strobe data
		nop
		cbi	PORTB, STR
end_refresh:
	
		ret		                                                                 
;---------------------------------------------------------------
;===============================================================================
CLEAR_TIME:
;===============================================================================
; Description    : Nulovanie casu 
; uses   :  TMP_A        
; Input  : stlacena klavessa min + sec, Status_reg- S_STOP
; Output : ML,MH,SL,SH
; Note   : nastavy cas 00:00
;Version : 
		sbrc	STATUS_REG,S_STOP					;je spusteny start ?
		ret								;ak ano skonci

		cbr	STATUS_REG,1<<CASOVAC					;stopky pocitanie nahor
		cbr	STATUS_KEYB,1<<KEY_SEC
		cbr	STATUS_KEYB,1<<KEY_MIN
		clr	SL
		clr	SH
		clr	ML
		clr	MH							;nuluj cas
		ret
				
/*
;===============================================================================
MEMORY_B:
;===============================================================================
; Description    : stlacene tlacitko start 
; uses   :          
; Input  : 
; Output :
; Note   : 
;Version : 



		ret
		*/
;===============================================================================
MEMORY_A:
;===============================================================================
; Description    : stlacene tlacitko start 
; uses   : TMP_A         
; Input  : ML,MH,SL,SH
; Output : MEMORY_S,MEMORY_M
; Note   : uchova data do pamate a taktiez ich pouzije ak je cas =00:00
;Version : 
		sbrc	STATUS_REG,S_STOP					;je spusteny start ?
		ret
		
		clc
		clr	TMP_A
		add	TMP_A,SL
		adc	TMP_A,SH
		adc	TMP_A,ML
		adc	TMP_A,MH
		breq	citaj_pamat						;ak je cas 00:00 nacitaj pamat 
		mov	MEMORY_S,SH						;inak uloz cas do pamate
		swap	MEMORY_S
		or	MEMORY_S,SL
		mov	MEMORY_M,MH
		swap	MEMORY_M
		or	MEMORY_M,ML
		ret
citaj_pamat:		
		sbr	STATUS_REG,1<<CASOVAC					;casovac  - nadol
		ldi	TMP_A,0b00001111		
		mov	SL,MEMORY_S
		mov	SH,SL
		swap	SH
		and	SL,TMP_A
		and	SH,TMP_A
		mov	ML,MEMORY_M
		mov	MH,ML
		swap	MH
		and	ML,TMP_A
		and	MH,TMP_A
		ret

;===============================================================================
START_STOP:
;===============================================================================
; Description    : stlacene tlacitko start 
; uses   :TMP_1,TMP_2    
; Input  : 
; Output :STATUS_REG_C - casomiera
; Note   : nastavuje start a stop, povoluje spustanie casomiery
;Version : 
		
		sbrc	STATUS_REG,S_STOP					;ak je teraz nastaveny start nemen C
		rjmp	clear_start						;ak je S_Stop = H skoc na nuluj
		sbr	STATUS_REG,1<<S_STOP					; S_STOP zmen na H - Start casomiery
		ldi	TMP_1,LOW (cas_sekundy)					;1s On 
		ldi	TMP_2,HIGH(cas_sekundy)
		add	TMP_1,TIMERL
		adc	TMP_2,TIMERH
		std	Y+( TIMER_CL - Y_INDEX_ADDR),TMP_1
		std	Y+( TIMER_CH - Y_INDEX_ADDR),TMP_2			;uloz novu hodnotu sekundy

		ldd	TMP_1,Y+(STATUS_REG_C - Y_INDEX_ADDR)
		ori	TMP_1,0b01000000					;aktivuj 
		std 	Y+(STATUS_REG_C - Y_INDEX_ADDR),TMP_1
		cbi	PORTB,START_LED						;start zasviet led
		ret
clear_start:
		cbr	STATUS_REG,1<<S_STOP					; S_STOP zmen na L - stop casomiery
		ldd	TMP_1,Y+(STATUS_REG_C - Y_INDEX_ADDR)
		andi	TMP_1,0b00111111					;deaktive
		std 	Y+(STATUS_REG_C - Y_INDEX_ADDR),TMP_1
		sbi	PORTB,START_LED						;stop zhasni led
		ret
;===============================================================================
SET_MIN:
;===============================================================================
; Description    : Nastavenie minut
; uses   :  TMP_A        
; Input  : stlacena klavessa min, Status_reg- S_STOP
; Output : ML,MH
; Note   : Pricitava cas k minutam max. 99, ak je spusteny cas tak disable function
;Version : 


		sbrc	STATUS_REG,S_STOP					;je spusteny start ?
end_min:		
		ret								;ak ano skonci
		sbr	STATUS_REG,1<<CASOVAC					;casovac pocitanie nadol
		mov	TMP_A,ML
		inc	TMP_A
		mov	ML,TMP_A
		cpi	TMP_A,max_number					; je vacsi ako 9
		brne	end_min
		clr	ML
		mov	TMP_A,MH
		inc	TMP_A
		mov	MH,TMP_A
		cpi	TMP_A,max_number
		brne	end_min							;max hodnota 99minut
		dec	TMP_A
		mov	ML,TMP_A
		mov	MH,TMP_A
		ret

;===============================================================================
SET_SEC:
;===============================================================================
; Description    : Nastavenie sekund
; uses   :  TMP_A        
; Input  : stlacena klavessa sec, Status_reg- S_STOP
; Output : SL,SH
; Note   : Pricitava cas k minutam max. 99, ak je spusteny cas tak disable function
;Version : 
		sbrc	STATUS_REG,S_STOP					;je spusteny start ?
end_sec:		
		ret								;ak ano skonci
		sbr	STATUS_REG,1<<CASOVAC
		mov	TMP_A,SL
		inc	TMP_A
		mov	SL,TMP_A
		cpi	TMP_A,max_number					; je vacsi ako 9
		brne	end_sec
		clr	SL
		mov	TMP_A,SH
		inc	TMP_A
		mov	SH,TMP_A
		cpi	TMP_A,max_sec_H
		brne	end_sec							;max hodnota 99minut
		dec	TMP_A
		mov	SH,TMP_A
		ldi	TMP_A,max_number-1
		mov	SL,TMP_A
		ret

;===============================================================================
;INTERUPT TIMER SERVICE:
;===============================================================================
; Description    : Timer service  0.1 mS
; uses   :  TMP_1,TMP_2,TMP_3,TMP_4         
; Input  : 
; Output :
; Note   : pocitacnie casu (hodnota OCRB = 30)
;Version : 

TIMER_SERVICE:									;
		push	TMP_A
		in	TMP_A, SREG						; save Status register
		push	TMP_A
		clr	TMP_A
		out	TCNT0,TMP_A						;uloz casovas (0 s)
		
		clc								;clear cary
		ldi	TMP_A,low(1)
		add	TIMERL,TMP_A
		ldi	TMP_A,high(1)
		adc	TIMERH,TMP_A						;pocitadlo

		mov	TMP_1,TIMERL
		mov	TMP_2,TIMERH

;---TIMER A------------------------------------------------
		ldd	TMP_3,Y+( TIMER_AL - Y_INDEX_ADDR)			;secundary timer A displej
		ldd	TMP_4,Y+( TIMER_AH - Y_INDEX_ADDR)
		cp	TMP_3,TMP_1
		cpc	TMP_4,TMP_2
		brne	timer_A
		
		ldd	TMP_A,Y+( STATUS_REG_A - Y_INDEX_ADDR)  
		ori	TMP_A,0b10000000
		std 	Y+( STATUS_REG_A - Y_INDEX_ADDR),TMP_A
		
timer_A:		
;---TIMER B------------------------------------------------
		ldd	TMP_3,Y+( TIMER_BL - Y_INDEX_ADDR)			;secundary timer B keyboard
		ldd	TMP_4,Y+( TIMER_BH - Y_INDEX_ADDR)
		cp	TMP_3,TMP_1
		cpc	TMP_4,TMP_2
		brne	timer_B
		
		ldd	TMP_A,Y+( STATUS_REG_B - Y_INDEX_ADDR)
		ori	TMP_A,0b10000000
		std 	Y+( STATUS_REG_B - Y_INDEX_ADDR),TMP_A
timer_B:	
;---TIMER C------------------------------------------------
		ldd	TMP_3,Y+( TIMER_CL - Y_INDEX_ADDR)			;secundary timer C Timer/casovac
		ldd	TMP_4,Y+( TIMER_CH - Y_INDEX_ADDR)
		cp	TMP_3,TMP_1
		cpc	TMP_4,TMP_2
		brne	timer_C
		
		ldd	TMP_A,Y+( STATUS_REG_C - Y_INDEX_ADDR)
		ori	TMP_A,0b10000000
		std 	Y+( STATUS_REG_C - Y_INDEX_ADDR),TMP_A
timer_C:	


		mov	TMP_A,STATUS_KEYB
		andi	TMP_A,0b00001111					;
/*   mozne stavy klavesnice
	SEC    MIN     MEMO    START	
         H	L	L	L	- stlacene sekundy
         L	H	L	L	- stlacene minuty
         H	H	L	L	- clear time
         H	L	H	L	- pamat A
         L	H	H	L	- pamat B
         L	L	L	H	- Start/ Stop
*/
tlacitko_start:
		cpi	TMP_A,1<<KEY_START
		brne	tlacitko_memA
		rcall	START_STOP
		cbr	STATUS_KEYB,1<<KEY_START
		rjmp	END_TIME_SERVICE
tlacitko_memA:		
		cpi	TMP_A,1<<KEY_MEMO
		brne	tlacitko_minuty
		rcall	MEMORY_A
		cbr	STATUS_KEYB,1<<KEY_MEMO
		rjmp	END_TIME_SERVICE
/*		
tlacitko_memB:		
		cpi	TMP_A,0b00000110
		brne	tlacitko_clear
		rcall	MEMORY_B
		cbr	STATUS_KEYB,1<<KEY_MEMO
		cbr	STATUS_KEYB,1<<KEY_MIN
		rjmp	END_TIME_SERVICE
*/		

tlacitko_minuty:		
		cpi	TMP_A,1<<KEY_MIN
		brne	tlacitko_sekundy
		rcall	SET_MIN
		cbr	STATUS_KEYB,1<<KEY_MIN
		rjmp	END_TIME_SERVICE 
tlacitko_sekundy:		
		cpi	TMP_A,1<<KEY_SEC
		brne	END_TIME_SERVICE
		rcall	SET_SEC
		cbr	STATUS_KEYB,1<<KEY_SEC
		rjmp	END_TIME_SERVICE
		

END_TIME_SERVICE:	
		
		
		rcall	REFRESH							;refresh displej, keyboard

		pop	TMP_A
		out	SREG,TMP_A
		pop	TMP_A
		reti								; return from interrupt
;------------------------------------------------------------------------------

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;========================== END of Interrupt ROUTINES =========================
;******************************************************************************



;Table for graphic 7-segment Display BACDEGF
;                  0       ,     1    ,     2    ,   3     ,     4    ,      5    ,    6     ,     7    ,    8     ,   9
TABLE:
.db		0b11111101,0b11010000,0b11101110,0b11111010,0b11010011,0b10111011,0b10111111,0b11110000,0b11111111,0b11110011

