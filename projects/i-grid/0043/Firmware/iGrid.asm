;****************************************************************************
;*																			*
;*			iGrid v1.0 						        						*
;*			Features:	     												*
;*			1) 	8MHZ internal Oscilator    									*
;*			2)	2x16LCD module w/backlight									*
;*			3) 	Interrupt driven											*
;*			4)  PORTB: PB2=RS,PB3=E, PB4..7=D4..D6 of LCD pins				*
;*			5)  MAX range is 24hours and 59minutes (24:59)					*
;*			6)  Auto power-off system function								*
;*																			*
;*																			*
;*			BUG Reports   													*
;*			>Cancel button bounces causing over menu jumps        (30/09/06)*
;*			  							  						            *
;*	Depeloped and tested by Timothy Kondos     								*
;*	TK Electronics Ltd logo is registered to the author						* 
;*																			*
;*	Contact info: timos_t@yahoo.com											*
;*	Disclaimer:You CAN NOT modify any part of this code for					* 
;*  commercial purpose.														*
;****************************************************************************
.include "tn2313def.inc"				;Includes the tn2313def. file

;****************************
;*	General registers    2  *
;****************************
.DEF STATUS = R10 						;Machine status register
.DEF Temp 	= R16 						;General storage register

;****************************
;*	Keypad registers     1  *
;****************************
.DEF Keys	= R11 						;Keypad status register

;****************************
;*	Timer registers      4  *
;****************************
.DEF CNT0   = R17 						;Delay reload(low)  register
.DEF CNT1   = R18 						;Delay reload(High) register
.DEF Tick   = R19						;20ms register(Delays)
.DEF Tock   = R13						;20ms register(RTC)
.DEF Timer0 = R22 						;Interrupt reload register

;****************************
;*	LCD registers        3  *
;****************************
.DEF LCD_CMD= R20 						;LCD command register
.DEF LCD_DAT= R21						;LCD data register
.DEF LCD_LP = R12 						;LCD loop register

;****************************
;*	Digit registers      7  *
;****************************
.DEF HouMSD  = R4						;6th digit(MSD) 
.DEF HouLSD  = R5						;5th 
.DEF MinMSD  = R2						;4th
.DEF MinLSD  = R3						;3rd  
.DEF SecMSD  = R23						;2nd 
.DEF SecLSD  = R24						;1st digit(LSD)
.DEF Shadow  = R6						;Shadow register

;****************************
;*	RTC registers      	 3  *
;****************************
.DEF RTCmin  = R7						;1st counter (Mins)
.DEF RTChou  = R8						;2nd counter (Hours)
.DEF RTCsec  = R9						;3rd counter (Secs)

;****************************
;*	ROM AREA setup          *
;****************************
.CSEG
.ORG 0x000 								;The code starts from 0x0000
RJMP RESET 								;Relative Jump to the RESET 
RETI
RETI
RETI
RETI
.ORG 0x005 								;Inerrupt service sub
RJMP ISR 								;
RETI
	
;********************
;*	Stack setup		*
;********************
RESET:	CLI								;Global interrupts disabled
		LDI		Temp,Low(RAMEND)		;STACK pointer location 
		OUT		SPL,Temp				;Started from the end of SRAM
;********************
;*	PORTS setup		*
;********************
;************ 
;************
		LDI 	Temp,0b11111101 		;Setup PORTB I/O pin
		OUT 	DDRB,Temp
		LDI 	Temp,0b00000000 		;Setup PORTB I/O pin behaviour
		OUT 	PORTB,Temp
;************
;*	PORTD	*
;************
		LDI 	Temp,0b11110000 		;Setup PORTD I/O pin
		OUT 	DDRD,Temp
		LDI 	Temp,0b00000000 		;Setup PORTD I/O pin behaviour
		OUT 	PORTD,Temp
;****************************
;*	Timers setup			*
;****************************
		LDI 	Temp,0x80 				;Setup Mask register
		OUT 	TIMSK,Temp 				;
		LDI 	Temp,0x00 				;Setup Timer0 properties
		OUT 	TCCR1A,Temp 			;
		LDI 	Temp,0x02 				;Setup Timer0 prescaler
		OUT 	TCCR1B,Temp 			;
		LDI		Timer0,0xB1				;Reload timer1(high byte)
		OUT		TCNT1H,Timer0
		LDI		Timer0,0xE0				;low byte
		OUT		TCNT1L,Timer0
		SEI								;Enable Global interrupts
		SBI		PORTD,5					;Connect system
        RCALL 	D20ms					;Delay for 20msec
		RCALL 	init_lcd				;Initialize LCD
		SBI		PORTD,4					;Light-up the LCD
		LDI 	ZH, high(msg1*2)		;Load 16bit address table start
        LDI 	ZL, low(msg1*2)			;for message display
		RCALL	print_lcd			 	;Print message to LCD
		CBI		PORTD,4					;Light-down the LCD
		RCALL	D1s
		LDI 	ZH, high(msg2*2)		;Load 16bit address table start
        LDI 	ZL, low(msg2*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	D2s
		LDI 	ZH, high(msg3*2)		;Load 16bit address table start
        LDI 	ZL, low(msg3*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		CLR		RTCmin					;Reset minutes
		CLR		RTChou					;Reset hours
		RJMP	CoreM					;Go to Core handler

;********************************************************************
;*																	*
;*																	*
;*				C o r e  handler									*
;*																	*
;*																	*
;********************************************************************
;************************
;*						*
;*						*
;* Minutes Entries Task	*
;*						*
;*						*
;************************
CoreM: 	CLR		Shadow					;Reset Shadow register
		SBI		PORTD,4					;Light-up the LCD
		RCALL	CLR_h_pointer			;Clear hours pointer and refresh
		RCALL	ToneHigh				;Audible key-press
		CBI		PORTD,4					;Light-down the LCD
Co1:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	CoreM1					;If true jump back to CoreM1 task
		RJMP	Co1						;Else loop until keypad is idle
CoreM1:	CLR		Tick					;Reset Tick register
CoreM2:	SBIC	PINB,1					;Check (BYPASS) key
		RJMP	Bypass
		LDI		Temp,0b00000010			;Check if (+) key pressed
		CP		Keys,Temp				
		BREQ	INCM					;Branch if true to increment handler
		LDI		Temp,0b00000001			;Check if (-) key pressed
		CP		Keys,Temp
		BREQ	DECM					;Branch if true to decrement handler
		LDI		Temp,0b00000100			;Check if (ENTER) key pressed
		CP		Keys,Temp
		BREQ	CoreH					;Branch if true to Hours handler
;********************
;* Check idle		*
;* keypad timeout	*
;********************
		CPI		Tick,50					;Check idle second
		BRNE	CoreM2					;Loop back to CoreM2
		RCALL	Check_Idle_Keypad		;Idle keypad to power-off handler
		RJMP	CoreM1					;Wait another second
						
;************************
;* Increment 1(min)step	*
;************************
INCM:	RCALL	Rep_UPmin				;Check for repeated keypress	
		RJMP	CoreM1					;Continiue to data entry handler
;************************
;* Decrement 1(min)step	*
;************************						
DECM:	RCALL	Rep_DOmin				;Check for repeated keypress
		RJMP	CoreM1					;Continiue to data entry handler
					
;************************
;*						*
;*						*
;* Hours Entries Task	*
;*						*
;*						*
;************************
CoreH: 	SBI		PORTD,4					;Light-up the LCD
		RCALL	ToneHigh				;Audible key-press
		RCALL	CLR_m_pointer			;Clear minutes pointer and refresh
		CBI		PORTD,4					;Light-down the LCD
Co2:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	CoreH1					;If true jump back to CoreH1 task
		RJMP	Co2						;Else loop until keypad is idle
CoreH1:	CLR		Tick					;Reset Tick register
CoreH2:	SBIC	PINB,1					;Check (BYPASS) key
		RJMP	Bypass
		LDI		Temp,0b00000010			;Check if (+) key pressed
		CP		Keys,Temp				
		BREQ	INCH					;Branch if true to increment handler
		LDI		Temp,0b00000001			;Check if (-) key pressed
		CP		Keys,Temp
		BREQ	DECH					;Branch if true to decrement handler
		LDI		Temp,0b00000100			;Check if (ENTER) key pressed
		CP		Keys,Temp
		BREQ	Valid					;Branch if true to Validation handler
		LDI		Temp,0b00001000			;Check if (CANCEL) key pressed
		CP		Keys,Temp
		BREQ	CoreM					;Branch if true to Minutes handler
;********************
;* Check idle		*
;* keypad timeout	*
;********************
		CPI		Tick,50					;Check idle second
		BRNE	CoreH2					;Loop back to CoreH2
		RCALL	Check_Idle_Keypad		;Idle keypad to power-off handler
		RJMP	CoreH1					;Wait another second

;************************
;* Increment 1(hour)step*
;************************
INCH:	RCALL	Rep_UPhou				;Check for repeated keypress
		RJMP	CoreH1					;Loop back to Rescan keypad	
;************************
;* Decrement 1(hour)step*
;************************						
DECH:	RCALL	Rep_DOhou				;Check for repeated keypress				
		RJMP	CoreH1					;Loop back to Rescan keypad

;************************
;* Validation task    	*
;************************
Valid:	LDI		Temp,0					;Check if we have 00 Hours
		CP		RTChou,Temp
		BREQ	Va1						;If true check if hours is 00 too
		RJMP	Confi					
Va1:	LDI		Temp,0					;Check if we have 00 minutes
		CP		RTCmin,Temp
		BREQ	Er100					;If true an error generated
		RJMP	Confi					;Else continiue to Confirmation

Er100:	LDI 	ZH, high(msg6*2)		;Load 16bit address table start
        LDI 	ZL, low(msg6*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	D1s						;1seconds delay
Er101:	LDI 	ZH, high(msg3*2)		;Load 16bit address table start
        LDI 	ZL, low(msg3*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	BinToBCD_Hours			;Binary to BCD conversion(Hours)
		RCALL	ToneHigh				;Audible key-press
		RJMP	CoreM					;Far jump to CoreM
;************************
;* Confirmation task    *
;************************
Confi:	RCALL	ToneHigh				;Audible key-press
		LDI 	ZH, high(msg5*2)		;Load 16bit address table start
        LDI 	ZL, low(msg5*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
Co4:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	Co5						;If true proceed to confirmation
		RJMP	Co4						;Else loop until keypad is idle
Co5:	LDI		Temp,0b00000100			;Check if (ENTER) key pressed
		CP		Keys,Temp
		BREQ	CountC					;Branch if true to Countdown handler
		LDI		Temp,0b00001000			;Check if (CANCEL) key pressed
		CP		Keys,Temp
		BREQ	Er101					;Branch if true to Er401
		RJMP	Co5						;Else wait for confirmation or exiting
;************************
;*						*
;*						*
;*    Countdown Task	*
;*						*
;*						*
;************************
CountC:	SBI		PORTD,4					;Light-up the LCD
		SBI		PORTD,6					;Open load(drive)
		LDI 	ZH, high(msg4*2)		;Load 16bit address table start
        LDI 	ZL, low(msg4*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	ToneHigh				;Audible key-press
		CBI		PORTD,4					;Light-down the LCD
		CLR		Shadow					;Clear Shadow register
		CLR		Tick					;Reset all counts
		CLR		Tock					;(mins, hours, seconds)
		LDI		Temp,59					;Load Temp with 59
		MOV		RTCsec,Temp				;Copy Temp to RTCsec
;********************
;* Check RTChou=0?	*
;********************
		LDI		Temp,0					;Load Temp with 0
		CP		RTChou,Temp				;Is RTChou=0 ?
		BREQ	Count00					;(Yes)-Count minutes normally
		RJMP	Count03					;(No) -Adjust hours/mins
;********************
;* Mins/secs counts *
;********************
Count00:RCALL	RTCmodule				;RTCmodule handler
		LDI		Temp,0b00000001		
		CP		Shadow,Temp				;Do we have a user exit?
		BREQ	EmEx					;(Yes)-Exit all countings/Abort
			
Done:	RCALL	DoneFixed				;Message handler(Completed)
		RJMP	CoreM

EmEx:	RCALL	AbortFixed				;Message handler(Completed)
		RJMP	CoreM

Count03:LDI		Temp,0					;Load Temp with 0
		CP		RTCmin,Temp				;Is RTCmin=0 ?
		BREQ	Count05					;(Yes)-Count remaining hours
		RJMP	Count04					;(No) -Count  minutes 1st
;********************
;* Mins trimmer 	*
;********************
Count04:RCALL	RTCmodule				;RTCmodule handler
		LDI		Temp,0b00000001		
		CP		Shadow,Temp				;Do we have a user exit?
		BREQ	EmEx					;(Yes)-Exit all countings/Abort
		RJMP	Count03
;********************
;* Hours counts 	*
;********************
Count05:DEC		RTChou					;Decrement hours
		LDI		Temp,60					;Load Temp with 60
		MOV		RTCmin,Temp				;Copy Temp to RTCmin
		RCALL	RTCmodule				;RTCmodule handler
		LDI		Temp,0b00000001		
		CP		Shadow,Temp				;Do we have a user exit?
		BREQ	EmEx					;(Yes)-Exit all countings/Abort
		LDI		Temp,0
		CP		RTChou,Temp
		BREQ	Done
		RJMP	Count05
			
;********************
;* Bypass checkup 	*
;********************
Bypass:	SBI		PORTD,4					;Light-up the LCD
		SBI		PORTD,6					;Open load(drive)
		RCALL	ToneHigh				;Audible key-press
		LDI 	ZH, high(msg8*2)		;Load 16bit address table start
        LDI 	ZL, low(msg8*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		CBI		PORTD,4					;Light-down the LCD

By01:	RCALL	D20ms					;Bounce time
		RCALL	D20ms
		SBIC	PINB,1					;Key released?
		RJMP	By01					;(No) - Recheck

By02:	SBIS	PINB,1					;Key pressed again?
		RJMP	By02					;(No) - Recheck
		;CBI		PORTD,4					;Light-down the LCD
		CBI		PORTD,6					;Close load(drive)
		LDI 	ZH, high(msg3*2)		;Load 16bit address table start
        LDI 	ZL, low(msg3*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		CLR		RTCmin
		CLR		RTChou

By03:	RCALL	D20ms					;Bounce time
		RCALL	D20ms
		SBIC	PINB,1					;Key released?
		RJMP	By03					;(No) - Recheck
		RJMP	CoreM

;****************************
;*	LCD mod. initialization	*
;*	4bit/Read only/No cur.	*
;****************************
init_lcd:
        LDI 	LCD_CMD,0x03			;Load LCD_CMD<-0x03 
        RCALL 	lcd_low_cmd
        RCALL 	D20ms
        LDI 	LCD_CMD,0x03			;Load LCD_CMD<-0x03
        RCALL 	lcd_low_cmd
        RCALL 	D20ms
        LDI 	LCD_CMD,0x03			;Load LCD_CMD<-0x03
        RCALL 	lcd_low_cmd
        RCALL 	D20ms
        LDI 	LCD_CMD, $28			;Set 4-bit interface
        RCALL 	lcd_all_cmd   
        LDI 	LCD_CMD,0x08			;Set DDRAM address to 0x80
        RCALL 	lcd_all_cmd
        LDI 	LCD_CMD,0x0C			;
        RCALL 	lcd_all_cmd
        LDI 	LCD_CMD,0x06			;
        RCALL 	lcd_all_cmd
        RET

;****************************
;*		I.S.R				*
;*		20ms intervals		*
;****************************
ISR:	IN		STATUS,SREG				;Store machine status
		LDI		Timer0,0xB1				;Reload timer1(high byte)
		OUT		TCNT1H,Timer0
		LDI		Timer0,0xE0				;low byte
		OUT		TCNT1L,Timer0
		INC		Tick					;+1 to xms register
		RCALL	Keyp_R					;Read encoder and keypad				
IS_X:	OUT		SREG,STATUS				;Restore machine status
		RETI

;********************************************************************
;*																	*
;*																	*
;*				Delay handlers										*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	25us delay				*
;****************************
D25us:  LDI		CNT0,$42				;Re-init CNT0 register
DB0:	DEC		CNT0					;(-1) to CNT0 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB0						;(Yes)
		RET

;****************************
;*	50us delay				*
;****************************
D50us:  LDI		CNT0,$85				;Re-init CNT0 register
DB1:	DEC		CNT0					;(-1) to CNT0 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB1						;(Yes)
		RET

;****************************
;*	190us delay				*
;****************************
D190us: LDI		CNT1,$02				;Re-init CNT1 register
DB2:	LDI		CNT0,$FC				;Re-init CNT0 register
DB3:	DEC		CNT0					;(-1) to CNT0 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB3						;(Yes)
		DEC		CNT1					;(-1) to CNT1 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB2						;(Yes)
		RET
;****************************
;*	200us delay				*
;****************************
D200us: LDI		CNT1,$0D				;Re-init CNT1 register
DB4:	LDI		CNT0,$28				;Re-init CNT0 register
DB5:	DEC		CNT0					;(-1) to CNT0 register
		BRNE	DB5						;(Yes)
		DEC		CNT1					;(-1) to CNT1 register
		BRNE	DB4						;(Yes)
		RET

;****************************
;*	400us delay				*
;****************************
D400us: LDI		CNT1,$0D				;Re-init CNT1 register
DB6:	LDI		CNT0,$51				;Re-init CNT0 register
DB7:	DEC		CNT0					;(-1) to CNT0 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB7						;(Yes)
		DEC		CNT1					;(-1) to CNT1 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB6						;(Yes)
		RET

;****************************
;*	1ms delay				*
;****************************
D1ms: 	LDI		CNT1,$1F				;Re-init CNT1 register
DB8:	LDI		CNT0,$55				;Re-init CNT0 register
DB9:	DEC		CNT0					;(-1) to CNT0 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB9						;(Yes)
		DEC		CNT1					;(-1) to CNT1 register
		CPI		CNT0,0					;Delay timeout?
		BRNE	DB8						;(Yes)
		RET

;****************************
;*	20ms delay				*
;****************************
D20ms:  CLR		Tick					;Re-init 20ms register
DA1:	CPI		Tick,1					;Delay timeout?
		BRNE	DA1						;(Yes)
		RET

;****************************
;*	160ms delay				*
;****************************
D160ms: CLR		Tick					;Re-init 20ms register
DA2:	CPI		Tick,8					;Delay timeout?
		BRNE	DA2						;(Yes)
		RET

;****************************
;*	500ms delay				*
;****************************
D500ms:	CLR		Tick					;Re-init 20ms register
DA3:	CPI		Tick,25					;Delay timeout?
		BRNE	DA3						;(Yes)
		RET

;****************************
;*	1s delay				*
;****************************
D1s:	CLR		Tick					;Re-init 20ms register
DA4:	CPI		Tick,50					;Delay timeout?
		BRNE	DA4						;(Yes)
		RET

;****************************
;*	2s delay				*
;****************************
D2s:	CLR		Tick					;Re-init 20ms register
DA5:	CPI		Tick,100				;Delay timeout?
		BRNE	DA5						;(Yes)
		RET
       
;********************************************************************
;*																	*
;*																	*
;*				LCD module handlers									*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	4bit Command handler	*
;*	Upper nibble send		*
;****************************
lcd_low_cmd:
        MOV 	TEMP, LCD_CMD			;Copy LCD_CMD to TEMP
        LSL 	TEMP					;Transfer 4 lower bits 
        LSL 	TEMP					;to upper 4 bits
        LSL 	TEMP
        LSL 	TEMP
        ANDI 	TEMP,0b11110000			;Pass high nibble (0x30)
        OUT 	PORTB,TEMP				;Output to PORTB
        ORI 	TEMP,0b00001000			;Set bit3-->(E=1)
        OUT 	PORTB,TEMP				;Output to PORTB
        ANDI 	TEMP,0b11110111			;Pass all except bit3-->(E=0)
        OUT 	PORTB,TEMP				;Output to PORTB
        RET

;****************************
;*	Sends 1 command byte to *
;*  the LCD module  	    *
;****************************
lcd_all_cmd:
        PUSH 	LCD_CMD					;Store LCD_CMD
        LSR 	LCD_CMD					;Transfer 4 upper bits
        LSR 	LCD_CMD					;to lower 4 bits
        LSR 	LCD_CMD
        LSR 	LCD_CMD
        RCALL 	lcd_low_cmd				;Call #lcd_low_cmd# handler
        POP 	LCD_CMD					;Restore LCD_CMD
        ANDI 	LCD_CMD,0b00001111		;Pass low nibble to be "out"
        RCALL 	lcd_low_cmd				;Call #lcd_low_cmd# handler
 		RCALL	D1ms
        RET

;****************************
;*	4bit Data handler 	    *
;*	Upper nibble send		*
;****************************
lcd_low_dat:
        MOV 	TEMP, lcd_dat			;Copy lcd_dat to TEMP
        LSL 	TEMP					;Transfer 4 lower bits
        LSL 	TEMP
        LSL 	TEMP
        LSL 	TEMP
        ANDI 	TEMP, 0b11110000		;Pass high nibble
        ORI		TEMP, 0b00000100		;Set bit2
        OUT		PORTB, TEMP				;Output to PORTB
        ORI		TEMP, 0b00001000		;Set bit3
        out 	PORTB, TEMP				;Output to PORTB
        ANDI 	TEMP, 0b11110111		;Pass all, except bit3
        OUT 	PORTB, TEMP
        RET

;****************************
;*	Sends 1 data byte to    *
;*  the LCD module  	    *
;****************************
lcd_all_dat:
        PUSH 	lcd_dat					;Store lcd_dat
        LSR 	lcd_dat
        LSR 	lcd_dat
        LSR 	lcd_dat
        LSR 	lcd_dat
        ANDI 	lcd_dat,0b00001111		;Pass low nibble to be "out"
        RCALL 	lcd_low_dat
        POP 	lcd_dat
        ANDI 	LCD_DAT,0b00001111		;Pass low nibble to be "out"
        RCALL 	lcd_low_dat
        RCALL 	D50us
        RET


;****************************
;*	LCD module printer		*
;*  Tables loaded from ROM	*
;****************************
print_lcd:
		LDI		Temp,16
		MOV 	LCD_LP,Temp				;Setup character length
        LDI 	LCD_CMD,0x80			;Set cursor address to 0x80
        RCALL 	lcd_all_cmd	
more1:  LPM								;Setup Flash ROM area
        MOV 	lcd_dat, r0				;Read current table's byte
        RCALL   lcd_all_dat
        ADIW 	ZL, 1
        DEC 	LCD_LP
		LDI		Temp,0
        CP 		LCD_LP,Temp				;Line full of chars?
        BRNE 	more1
		LDI		Temp,16
		MOV 	LCD_LP,Temp				;Setup character length			
        LDI 	LCD_CMD,0xC0			;Set cursor address to 0xC0
        RCALL 	lcd_all_cmd
more2:  LPM								;Setup Flash ROM area
        MOV		lcd_dat, r0				;Read current table's byte
        RCALL 	lcd_all_dat
        ADIW 	ZL, 1
        DEC 	LCD_LP
 		LDI		Temp,0
        CP 		LCD_LP,Temp				;Line full of chars?
        BRNE 	more2
        RET

;********************************************************************
;*																	*
;*																	*
;*				keypad entry handlers								*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	Keypad raw reading and 	*
;*	 filtering				*
;****************************
Keyp_R:	IN		Keys,PIND
		LDI		Temp,0b00001111
		AND		Keys,Temp
		RET

;****************************
;*	Repeated minutes key(-) *
;*  scanner and burster	    *
;****************************
Rep_DOmin:
		SBI		PORTD,4					;Light-up the LCD
		RCALL	ToneHigh				;Audible key-press
		LDI		Temp,0					;Check if we reached 0
		CP		RTCmin,Temp
		BREQ	DECM0					;Adjustment to 0 if true
		RJMP	DECM1			
DECM0:	LDI		Temp,0					;Adjust to 0
		MOV		RTCmin,Temp
		RJMP	DECM2					;Jump to display digits	
DECM1:	DEC		RTCmin					;Decrement RTCmin
DECM2:	RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	D20ms
		CLR		Tick					;Reset Tick register
RepDm1:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	RepDmx					;Exit repeated condition
		CPI		Tick,50					;1second elapsed?
		BRNE	RepDm1					;If not reloop
;********************
;* (-)1minutes rep.	*
;********************
RepDm5:	LDI		Temp,0					;Check if we reached 0
		CP		RTCmin,Temp
		BREQ	RepDm2					;Adjustment to 0 if true
		RJMP	RepDm3
RepDm2:	LDI		Temp,0					;Adjust to 0
		MOV		RTCmin,Temp
		RJMP	RepDm4					
RepDm3:	DEC		RTCmin					;Decrement RTCmin
RepDm4:	LDI		Temp,0b00000000			;Check if keypad is still busy
		CP		Keys,Temp		
		BREQ	RepDmx					;If keypad not busy then exit
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	D160ms					;Repetition delay
		RJMP	RepDm5					;Reloop			
RepDmx:	CBI		PORTD,4					;Light-down the LCD
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RET

;****************************
;*	Repeated minutes key(+) *
;*  scanner and burster	    *
;****************************
Rep_UPmin:
		SBI		PORTD,4					;Light-up the LCD
		RCALL	ToneHigh				;Audible key-press
		INC		RTCmin					;Increment RTCmin
		LDI		Temp,60					;Check if we reached 60
		CP		RTCmin,Temp
		BREQ	INCM0					;Adjustment to 59 if true
		RJMP	INCM1					;Else check for repeated condition
INCM0:	LDI		Temp,59					;Adjust to 59
		MOV		RTCmin,Temp
INCM1:	RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	D20ms
		CLR		Tick					;Reset Tick register
RepIm1:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	RepImx					;Exit repeated condition
		CPI		Tick,50					;1second elapsed?
		BRNE	RepIm1					;If not reloop
;********************
;* (+)1minutes rep.	*
;********************
RepIm4:	INC		RTCmin					;Increment RTCmin
		LDI		Temp,60					;Check if we reached 60
		CP		RTCmin,Temp
		BREQ	RepIm3					;Adjustment to 59 if true
		RJMP	RepIm2					;Else check for repeated condition
RepIm3:	LDI		Temp,59					;Adjust to 59
		MOV		RTCmin,Temp
RepIm2:	LDI		Temp,0b00000000			;Check if keypad is still busy
		CP		Keys,Temp		
		BREQ	RepImx					;If keypad not busy then exit
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	D160ms					;Repetition delay
		RJMP	RepIm4					;Reloop
RepImx:	CBI		PORTD,4					;Light-down the LCD
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RET

;****************************
;*	Repeated hours key(-)   *
;*  scanner and burster	    *
;****************************
Rep_DOhou:
		SBI		PORTD,4					;Light-up the LCD
		RCALL	ToneHigh				;Audible key-press
		LDI		Temp,0					;Check if we reached 0
		CP		RTChou,Temp
		BREQ	DECH0					;Adjustment to 0 if true
		RJMP	DECH1			
DECH0:	LDI		Temp,0					;<<Adjust to 0>>
		MOV		RTChou,Temp
		RJMP	DECH2					;Jump to display digits	
DECH1:	DEC		RTChou					;Decrement RTChou
DECH2:	RCALL	BinToBCD_Hours			;Binary to BCD conversion(Mins)
		RCALL	D20ms
		CLR		Tick					;Reset Tick register
RepDh1:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	RepDhx					;Exit repeated condition
		CPI		Tick,50					;1second elapsed?
		BRNE	RepDh1					;If not reloop
;********************
;* (-)1hours rep.	*
;********************
RepDh5:	LDI		Temp,0					;Check if we reached 0
		CP		RTChou,Temp
		BREQ	RepDh2					;Adjustment to 0 if true
		RJMP	RepDh3
RepDh2:	LDI		Temp,0					;Adjust to 0
		MOV		RTChou,Temp
		RJMP	RepDh4					
RepDh3:	DEC		RTChou					;Decrement RTCmin
RepDh4:	LDI		Temp,0b00000000			;Check if keypad is still busy
		CP		Keys,Temp		
		BREQ	RepDhx					;If keypad not busy then exit
		RCALL	BinToBCD_Hours			;Binary to BCD conversion(Hours)
		RCALL	D500ms					;Repetition delay
		RJMP	RepDh5					;Reloop
RepDhx:	CBI		PORTD,4					;Light-down the LCD
		RCALL	BinToBCD_Hours			;Binary to BCD conversion(Mins)
		RET


;****************************
;*	Repeated hours key(+)   *
;*  scanner and burster	    *
;****************************
Rep_UPhou:
		SBI		PORTD,4					;Light-up the LCD
		RCALL	ToneHigh				;Audible key-press
		INC		RTChou					;Increment RTChou
		LDI		Temp,25					;Check if we reached 25
		CP		RTChou,Temp
		BREQ	INCH0					;Adjustment to 24 if true
		RJMP	INCH1					;Else check next stage
INCH0:	LDI		Temp,24					;<<Adjust to 24>>
		MOV		RTChou,Temp
INCH1:	RCALL	BinToBCD_Hours			;Binary to BCD conversion(Mins)
		RCALL	D20ms
		CLR		Tick
RepIh1:	LDI		Temp,0b00000000			;Check if keypad is idle
		CP		Keys,Temp		
		BREQ	RepIhx					;Exit repeated condition
		CPI		Tick,50					;1second elapsed?
		BRNE	RepIh1					;If not reloop
;********************
;* (+)1minutes rep.	*
;********************
RepIh4:	INC		RTChou					;Increment RTChou
		LDI		Temp,25					;Check if we reached 25
		CP		RTChou,Temp
		BREQ	RepIh3					;Adjustment to 24 if true
		RJMP	RepIh2					;Else check for repeated condition
RepIh3:	LDI		Temp,24					;Adjust to 24
		MOV		RTChou,Temp
RepIh2:	LDI		Temp,0b00000000			;Check if keypad is still busy
		CP		Keys,Temp		
		BREQ	RepIhx					;If keypad not busy then exit
		RCALL	BinToBCD_Hours			;Binary to BCD conversion(Hours)
		RCALL	D500ms					;Repetition delay
		RJMP	RepIh4					;Reloop
RepIhx:	CBI		PORTD,4					;Light-down the LCD
		RCALL	BinToBCD_Hours			;Binary to BCD conversion(Mins)
		RET

;********************************************************************
;*																	*
;*																	*
;*				Binary to BCD/ASCII handlers						*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	BCD all 				*
;*	Hours/Minutes/Second    *
;****************************
BCDall:	RCALL	BinToBCD_Hours			;Binary to BCD conversion(Hours)
		RCALL	BinToBCD_Minutes		;Binary to BCD conversion(Mins)
		RCALL	BinToBCD_Secs			;Binary to BCD conversion(Secs)
		RET
;****************************
;*	Binary to BCD handler 	*
;*	Hours handler			*
;****************************
BinToBCD_Hours:
		PUSH	RTChou
		LDI		Temp,'0'
		MOV		HouLSD,Temp
		MOV		HouMSD,Temp
		LDI		Temp,10
Bin05:	SUB		RTChou,Temp
		BRCS	Bin06
		INC		HouMSD
		RJMP	Bin05
Bin06:	LDI		Temp,10
		ADC		RTChou,Temp		
Bin07:	DEC		RTChou
		LDI		Temp,0
		CP		RTChou,Temp
		BREQ	Bin08
		INC		HouLSD
		RJMP	Bin07
Bin08:	POP		RTChou
		RCALL	Show_2d_hours			;Show the hours digits to LCD	
		RET

;****************************
;*	Binary to BCD handler 	*
;*	Minutes handler			*
;****************************
BinToBCD_Minutes:
		PUSH	RTCmin
		LDI		Temp,'0'
		MOV		MinMSD,Temp
		MOV		MinLSD,Temp
		LDI		Temp,10
Bin01:	SUB		RTCmin,Temp
		BRCS	Bin02
		INC		MinMSD
		RJMP	Bin01
Bin02:	LDI		Temp,10
		ADC		RTCmin,Temp	
Bin03:	DEC		RTCmin
		LDI		Temp,0
		CP		RTCmin,Temp
		BREQ	Bin04
		INC		MinLSD
		RJMP	Bin03
Bin04:	POP		RTCmin
		RCALL	Show_2d_mins			;Show the minutes digits to LCD
		RET

;****************************
;*	Binary to BCD handler 	*
;*	Seconds handler			*
;****************************
BinToBCD_Secs:
		PUSH	RTCsec
		LDI		Temp,'0'
		MOV		SecMSD,Temp
		MOV		SecLSD,Temp
		LDI		Temp,10
Bin09:	SUB		RTCsec,Temp
		BRCS	Bin10
		INC		SecMSD
		RJMP	Bin09
Bin10:	LDI		Temp,10
		ADC		RTCsec,Temp	
Bin11:	DEC		RTCsec
		LDI		Temp,0
		CP		RTCsec,Temp
		BREQ	Bin12
		INC		SecLSD
		RJMP	Bin11
Bin12:	POP		RTCsec
		RCALL	Show_2d_secs			;Show the seconds digits to LCD
		RET

;****************************
;*	2 Digits draw 			*
;*	Hours handler			*
;****************************
Show_2d_hours:
		LDI 	LCD_CMD,0x88			;Set cursor address to 0x88
        RCALL 	lcd_all_cmd				;Hours position
		MOV 	lcd_dat,HouMSD
        RCALL   lcd_all_dat
		MOV 	lcd_dat,HouLSD
        RCALL   lcd_all_dat	
		RET

;****************************
;*	2 Digits draw 			*
;*	Minutes handler			*
;****************************
Show_2d_mins:
		LDI 	LCD_CMD,0x8B			;Set cursor address to 0x8B
        RCALL 	lcd_all_cmd				;Minutes position
		MOV 	lcd_dat,MinMSD
        RCALL   lcd_all_dat
		MOV 	lcd_dat,MinLSD
        RCALL   lcd_all_dat
		RET

;****************************
;*	2 Digits draw 			*
;*	Secs handler			*
;****************************
Show_2d_secs:
		LDI 	LCD_CMD,0x8E			;Set cursor address to 0x8E
        RCALL 	lcd_all_cmd				;Seconds position
		MOV 	lcd_dat,SecMSD
        RCALL   lcd_all_dat
		MOV 	lcd_dat,SecLSD
        RCALL   lcd_all_dat	
		RET

;****************************
;*	Erase handler 			*
;*	Clears minutes pointer	*
;****************************
CLR_m_pointer:
		LDI 	LCD_CMD,0xC9			;Set cursor address to 0xC9
        RCALL 	lcd_all_cmd				;Arrow pointer position
		LDI		lcd_dat,'^'				;Load current font
        RCALL   lcd_all_dat				;and display it
		LDI 	LCD_CMD,0xCC			;Set cursor address to 0xCC
        RCALL 	lcd_all_cmd				;Arrow pointer position
		LDI		lcd_dat,' '				;Load current (empty) font
        RCALL   lcd_all_dat				;and display it
		RET

;****************************
;*	Erase handler 			*
;*	Clears hours pointer	*
;****************************
CLR_h_pointer:
		LDI 	LCD_CMD,0xCC			;Set cursor address to 0xCC
        RCALL 	lcd_all_cmd				;Arrow pointer position
		LDI		lcd_dat,'^'				;Load current font
        RCALL   lcd_all_dat				;and display it
		LDI 	LCD_CMD,0xC9			;Set cursor address to 0xC9
        RCALL 	lcd_all_cmd				;Arrow pointer position
		LDI		lcd_dat,' '				;Load current (empty) font
        RCALL   lcd_all_dat				;and display it
		RET
;****************************
;*	Message handler 		*
;*	~ Counting completed ~	*
;****************************
DoneFixed:
		SBI		PORTD,4					;Light-up the LCD
		CBI		PORTD,6					;Close load(drive)
		LDI 	ZH, high(msg9*2)		;Load 16bit address table start
        LDI 	ZL, low(msg9*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	ToneHigh				;Audible key-press
		RCALL	D1s
		RCALL	ToneHigh				;Audible key-press
		RCALL	D1s
		RCALL	ToneHigh				;Audible key-press
		RCALL	D1s
		CBI		PORTD,4					;Light-down the LCD
		LDI 	ZH, high(msg3*2)		;Load 16bit address table start
        LDI 	ZL, low(msg3*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		CLR		RTCmin
		CLR		RTChou
		RET

;****************************
;*	Message handler 		*
;*	~ Counting aborted ~	*
;****************************
AbortFixed:
		SBI		PORTD,4					;Light-up the LCD
		CBI		PORTD,6					;Close load(drive)
		LDI 	ZH, high(msg7*2)		;Load 16bit address table start
        LDI 	ZL, low(msg7*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	D2s
		CBI		PORTD,4					;Light-down the LCD
		LDI 	ZH, high(msg3*2)		;Load 16bit address table start
        LDI 	ZL, low(msg3*2)			;for message display
		RCALL	print_lcd				;Print message to LCD
		CLR		RTCmin
		CLR		RTChou
		RET

;********************************************************************
;*																	*
;*																	*
;*				Tone handlers										*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	Tone (High Frequency)   *
;****************************
ToneHigh:
		LDI		Temp,200
To1:	SBI		PORTB,0					;Pin0 = "1"
		RCALL	D200us					;Ton time
		CBI		PORTB,0					;Pin0 = "0"
		RCALL	D200us					;Toff time
		DEC		Temp					;Tone duration check
		BRNE	To1						;If not ready, re-check
		RET

;********************************************************************
;*																	*
;*																	*
;*				RTC handlers										*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	RTCmodule    			*
;****************************
RTCmodule:
		CBI		PORTD,4					;Light-down the LCD
		DEC		RTCmin					;Offset RTCmin (-1)
		RCALL	BCDall					;Update all LCD digits
Count01:LDI		Temp,0b00000001
		CP		Shadow,Temp				;Is Shadow=1 - (ESCAPE)key?
		BREQ	CNT01x					;Emergency exit counting

		CPI		Tick,50					;Is Tick=50 ? (1second lap time)
		BRNE	Count01					;(No) - Recheck
		RCALL	CheckC					;(CANCEL) key update handler
		CLR		Tick					;Reset Tick register
		INC		Tock					;Increment Tock register
		LDI		Temp,60					;Load Temp with 60
		CP		Tock,Temp				;Is Tock=60? (1minute lap)
		BREQ	Count02					;(Yes)- Update minutes
		DEC		RTCsec					;Decrement RTCsec register
		RCALL	BCDall					;Update all LCD digits
		RJMP	Count01					;Continiue loop(seconds level)
Count02:SBI		PORTD,4					;Light-up the LCD(every minute)
		LDI		Temp,59					;Load Temp with 59
		MOV		RTCsec,Temp				;Copy Temp to RTCsec
		CLR		Tock					;Clear Tock register
		LDI		Temp,0					;Load Temp with 0
		CP		RTCmin,Temp				;Is RTCmin=0? (xx minutes lap)	
		BREQ	CNT01x					;(Yes)-Finished counting
		RJMP	RTCmodule				;Continiue loop(Mins/Secs level)

CNT01x:	RET


;********************
;* Check and update	*
;********************
CheckC:
		LDI		Temp,0b00001000			;Check if (CANCEL) key pressed
		CP		Keys,Temp
		BREQ	SetCbit					;(Yes)-Set  (C) flag bit/pressed
		RJMP	ClrCbit					;(No) -Clear(C) flag bit/idle

SetCbit:LDI		Temp,0b00000001
		MOV		Shadow,Temp				;Set (C) bit
		RJMP	CheckB2
				
ClrCbit:LDI		Temp,0b00000000	
		MOV		Shadow,Temp				;Clear (C) bit
		RJMP	CheckB2

CheckB2:RET


;********************
;* Auto disconnect  *
;********************
Check_Idle_Keypad:
		INC		Shadow
		LDI		Temp,240				;Load Temp with 240
		CP		Shadow,Temp				;4minutes elapsed?
		BREQ	Auto_off				;If true jump to Auto_off
		RJMP	Auto_xx					;Else keep waiting
Auto_off:
		SBI		PORTD,4					;Light-up the LCD(every minute)	
		LDI 	ZH, high(msg10*2)		;Load 16bit address table start
        LDI 	ZL, low(msg10*2)		;for message display
		RCALL	print_lcd				;Print message to LCD
		RCALL	ToneHigh				;Audible tone
		RCALL	ToneHigh
		RCALL	ToneHigh
		RCALL	D2s
		CBI		PORTD,4					;Light-down the LCD(every minute)
Auto_x:	CBI		PORTD,5					;Disconnect system
		RJMP	Auto_x
Auto_xx:RET	

;********************************************************************
;*																	*
;*				R O M   Message  tables								*
;*																	*
;********************************************************************
;****************************
;*	Message #1				*
;****************************
msg1: .db "TK Technologies, Athens 2006Ltd "
;          12345678123456781234567812345678

;****************************
;*	Message #2				*
;****************************
msg2: .db "Initializing...   please wait   "
;          12345678123456781234567812345678
 
;****************************
;*	Message #3				*
;****************************
msg3: .db "Setup   00:00:xxtime            "
;          12345678123456781234567812345678 

;****************************
;*	Message #4				*
;****************************
msg4: .db "Time    --:--:--left     H  M  S"
;          12345678123456781234567812345678

;****************************
;*	Message #5				*
;****************************
msg5: .db " Are you sure ?     (YES/NO)    "
;          12345678123456781234567812345678  

;****************************
;*	Message #6				*
;****************************
msg6: .db "Enter at least  1min to count!  "
;          12345678123456781234567812345678

;****************************
;*	Message #7				*
;****************************
msg7: .db " Countdown        canceled !    "
;          12345678123456781234567812345678

;****************************
;*	Message #8				*
;****************************
msg8: .db " Timer bypassed   Loads are on  "
;          12345678123456781234567812345678

;****************************
;*	Message #9				*
;****************************
msg9: .db " PROCESS         COMPLETED !    "
;          12345678123456781234567812345678

;****************************
;*	Message #10				*
;****************************
msg10: .db" Auto power off                 "
;          12345678123456781234567812345678
