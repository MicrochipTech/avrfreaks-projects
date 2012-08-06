;=====================================================================
;                                - Relaes Form -
;=====================================================================
; New [x] RTC DS1307_master_transmitter/reciver_mode  Update [ ]        Created  04-01-2009   
;-------------------------------------------------------------------------------------------- 
; Title :  
; Author:  BBH.                                       Date:                 
; E-mail: (bbh3450@hotmail.com)
;-------------------------------------------------------------------------------------------- 
; Target AVR MCU       : mega 16 
; Frq xtal             : 3,686 Mhz 
; Include file         : m16def.inc 
; Language             : ASM/Atmel compiler/assembler v2 
; Dev. Tool            : AVR Studio	4.19.730  
; Code available to use: yes
; Filename             : RTC_DS1307.asm        
; Fil zise  .hex       : 
;--------------------------------------------------------------------------------------------
; Description   : RTC DS1307     
; Input         : Pc0=SDA  Pc1 =SDA       TWI PROTOCOL
; Output        : SQW OUT =32.768 Khz PIN 7 RTC  
;               ; 0xd0 equ ds1307 is in write mode. (TWI master_transmitte_mode)
;               ; 0xd1 equ ds1307 is in read mode.  (TWI master_receiving_mode)

; Note          : application of power to the device the time and date registers are
;               : typically reset to 01/01/00 01 00:00:00 
;                 Please note that the initial power-on state of all registers is not defined. 
;                 Therefore it is important to enable the oscillator (CH bit = 0) 
;                 during initial configuration                            
;==================================================================================================================
;Change Log: 
;04-01-2009  Created. 
;07-01-2009  test of sqw output,setup frq out 32Khz ok use to check ds1307 is a live
;07-01-2009  test of clok halt osc bit 7 register adress 00h work when set to off or on.       
;Version release  
;-------------------------------------------------------------------------------------------------------------------
;VERSION:	DATE		DESCRIPTIO            
;  1      04-01-2009      Created       TWI MASTER"TRANSMITTER/reciver"  
;           
;------------------------------------------------------------------------------------------------------------------
;hardware connect:
;RTC  ON PORT C (TWI)    
;1=X-TAL
;2=X-TAL
;3=Vbat SET TO GND dont use backup bat    ((importen))
;4=GND
;5=SDA    Pc1 TWI  note 10k pull up to Vcc
;6=SCL    Pc0 TWI  note 10k pul  up to Vcc
;7=SQW OUT
;8=Vcc +5V
;======================================================================================================================
; config :
;======================================================================================================================
.INCLUDE"M16DEF.INC"                						    
.DEF TEMP         =R16      ;temporary variable
.DEF TEMP1        =R17      ;temporary variable
.DEF DATA         =R19      ;DATA VARIABLE FOR rtc ds1307 AND TWI COM.
.DEF LCD_DELAYTIME=R20      ;R20=TIMES FAKTOR OF 1Ms)R21
.DEF RTC_value    =R22      ;value register
.DEF RTCvalue     =R23      ;value register	           
                            ;STACKPOINTER register R25
                            ;SECONDS adress 00 h timekeeper register adr. rtc  
                            ;MINUTES adress 01 h timekeeper register adr. rtc 
                            ;HOURS   adress 02 h timekeeper register adr. rtc 
                            ;DAY     adress 03 h timekeeper register adr. rtc 
                            ;DATE    adress 04 h timekeeper register adr. rtc 
                            ;MONTH   adress 05 h timekeeper register adr. rtc 
                            ;YEAR    adress 06 h timekeeper register adr. rtc
.EQU CONTROL=0B10010011       ;CONTROL REGISTER RTC SETS SQW OUT ON AND SET FRQ sqw out TO 32,678kHZ
.EQU CLOCK_BIT7_ON=0B00000000 ;TURN CLOCK HALT BIT ON (CLOCK OSC=ON)
.EQU CLOCK_BIT7_OFF=0B10000000;TURN CLOCK HALT BIT OFF(CLOCK OSC=OFF)
.EQU LCD_PORT=PORTD         ;PORT D  CTRL OUTPUT RS,RW,EN TO LCD
.EQU LCD_PIN=PIND           ;PORT A USE AS DATABUS TO LCD , db7=busyflag.
.EQU LCD_EN=PD0             ;EN = ENABEL DATA READ/WRITE LCD                                      						    
.EQU LCD_RW=PD1             ;RW =
.EQU LCD_RS=PD2             ;RS =
;=========================================================================================================================
.CSEG
.ORG 0
reset:          
;=========================================================================================================================
;STACK_POINTER:
;=========================================================================================================================
; Description    : sets stack pointer up
; uses   :           
; Input  : 
; Output :
; Note   :
;Version :1.0 
         LDI           R25,LOW(ramend)
         OUT           SPL,R25            
         LDI           R25,HIGH(ramend)
         OUT           SPH,R25
         CLI          ;DISABLE INTERRUPT
;=============================================================================
;CONFIG PORT B,C.  
;=============================================================================
; Description PORT A,D = LCD DATA. PORT B=TWI. STK500 LED ERRO PORT. C =TWI COM.  
; uses   :                 
; Input  : 
; Output : 
; Note   :
;Version :1.0 	
 

         LDI           TEMP,0B11111111
	     OUT           DDRA,TEMP               
         OUT           DDRD,TEMP
         
		 LDI           R16,0XFF              
         OUT           DDRB,R16   
         OUT           PORTB,R16

         LDI           R16,0XFF              
         OUT           DDRC,r16
         OUT           PORTC,r16 
;=========================================================================================================================
; SETUP BIT RATE AND PRESCALER:
;=========================================================================================================================
; Description    : initialise Two Wire Interface.
; uses   :           
; Input  : TWBR,TWSR
; Output :
; Note   :
;Version :1.1 (prescaler)=.1
 
         Ldi          R16,12	    ;TWBR value
	     out          TWBR,R16		;Bit Rate register, sets speed, prescaler in TWSR
	     ldi          R16,0			;prescaler = 1
	     out          TWSR,R16		;write prescaler			 	 	           
;=========================================================================================================================
;               ***************MASTER_TRANSMITTER_MODE:**********************
;STARTUP and config RTC oscillator (CH bit = 0).
;=========================================================================================================================
; Description:Please note that the initial power-on state of all registers is not defined. 
;             Therefore it is important to enable the oscillator (CH bit = 0) 
;             during initial configuration

;             Timekeeper Registers"Bit 7" of Register 0 is the clock halt
;             (CH) bit. When this bit is set to 1, the oscillator is disabled. 
;             When cleared to 0,the oscillator is enabled."On first".

;             application of power to the device the time and date registers are
;             typically reset to 01/01/00 01 00:00:00              

; Output :register address 00h bit 7 CH  / seconds
; Note   :             ADDRESS   BIT 7   BIT 6 BIT 5 BIT 4    BIT 3 BIT 2 BIT 1 BIT 0 RANGE
;        :               00h      CH     -----10 sec------    ---------sec----------  00–59
; Note   :SECONDS ER SET TO 00.
;Version : 1.0

		 CALL           start_ds                                                       
		 LDI            DATA,0XD0            
		 CALL           WRIT_DS              
		 LDI            DATA,0X00            
		 CALL           WRIT_DATA
         LDI            DATA,CLOCK_BIT7_ON                   
   		 CALL           WRIT_DATA            
		 CALL           STOP_DS              
;=========================================================================================================================
;             ***************MASTER_TRANSMITTER_MODE:**********************       
;STARTUP and config RTC.operation of the SQW/OUT pin.
;=========================================================================================================================  
;             Please note that the initial power-on state of all registers is not defined. 
; Description:The DS1307 control register is used to control the operation of the SQW/OUT pin.
;              
;              BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0
;                 out    0     0    SQWE   0     0    RS1   RS0 
;                  1     0     0      1    0     0     1     1       
;
; Input  : address 07h rtc 
; Output : 32.768kHz pin 7 rtc
; Note   : RS1 and RS0 = SQUAREWAVE OUTPUT FREQUENCY look at data sheet.
; note   : i ues it to check ds1307 is a live
;Version : 1.1 (sub DESCRIPTION)
 
    	 CALL           start_ds             
		 LDI            DATA,0XD0
		 CALL           WRIT_DS
		 LDI            DATA,0X07           
   		 CALL           WRIT_DATA
		 LDI            DATA,CONTROL                             
		 CALL           WRIT_DATA           
		 CALL           STOP_DS
;=============================================================================================================
;                          *******MASTER_TRANSMITTER_MODE:*****
; STARTUP and config RTC. 12-hour or 24-hour mode
;=============================================================================================================
;              Please note that the initial power-on state of all registers is not defined. 
; Description: DS1307 can be run in either 12-hour or 24-hour mode.
;              Bit 6 of the hours register is defined as the
;              12- or 24-hour mode select bit.When high, the 12-hour mode is selected.
;              In the 12-hour mode, bit 5 is the AM/PM bit with logic high being PM.
;              In the 24-hour mode, bit 5 is the second 10 hour bit (20-23 hours). 

; uses  : 24-HOUR MODE / SET DS1307 TIME REGISTER "HOURS"        
; Input : adress 02h rtc  
; Output:
; Note  :     ADDRESS BIT 7     BIT 6     BIT 5     BIT 4    BIT 3 BIT 2 BIT 1 BIT 0  RANGE
;               02      0     24/12mode  ----10houre-----    --------houre----------  00|23
                           
;Version:1.0                               

         CALL           start_ds             
		 LDI            DATA,0XD0
		 CALL           WRIT_DS
		 LDI            DATA,0X02           
    	 CALL           WRIT_DATA
		 LDI            DATA,0B00000000                      
		 CALL           WRIT_DATA           
		 CALL           STOP_DS
;=========================================================================================================================
;                            *******MASTER_TRANSMITTER_MODE:*****
;SET DS1307 TIME REGISTER MINUTES  :
;=========================================================================================================================
; Description    : set user-defined minutes 
; uses   :           
; Input  : adress 01h rtc
; Note   :        ADDRESS BIT 7   BIT 6   BIT 5 BIT 4    BIT 3 BIT 2 BIT 1 BIT 0  RANGE  
;                    01      0   ----10 MINUTES------    -------MINUTES--------   00|59  
;Version :1.0          

         CALL           START_DS
         LDI            DATA,0XD0
		 Call           WRIT_DS
		 LDI            DATA,0X01
         CALL           WRIT_DATA
		 LDI            DATA,0B00000001
         CALL           WRIT_DATA
		 CALL           STOP_DS             
 
;====================================================================================
;                            *******MASTER_TRANSMITTER_MODE:*****
;SET DS1307 TIME REGISTER DAY:
;====================================================================================
; Description:set user-defined day 
;             On first application of power to the device the time and date registers
;             are typically reset to 01/01/00 01 00:00:00 
; uses  :           
; Input : adress 03h rtc    
; Output:
; Note  :the day of week are user-defined but must be sequential 
;       :if 1 equals Sunday,then 2 equals Monday and so on.
;       :The day-of-week register increments at midnight
;
;   ADDRESS BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0  RANGE  
;     03h       0     0     0     0     0    DAY   Day       01–07                            
;Version:1.0   

         CALL           START_DS
         LDI            DATA,0XD0
		 Call           WRIT_DS
		 LDI            DATA,0X03
         CALL           WRIT_DATA
		 LDI            DATA,0B00000001
         CALL           WRIT_DATA
		 CALL           STOP_DS
;=========================================================================================================================
;                            *******MASTER_TRANSMITTER_MODE:*****
;SET DS1307 TIME REGISTER DATE:
;=========================================================================================================================
; Description    : set user-defined date
; uses   :           
; Input  : adress 04h rtc        
; Output :       
; Note   : ADDRESS   BIT 7    BIT 6  BIT 5     BIT 4   BIT 3 BIT 2  BIT 1  BIT 0  RANGE
;           04h        0        0    10 Date  10 Date  DATE  DATE    DATE   DATE  01|31                    01–31
;Version :1.0       

         CALL           START_DS
		 LDI            DATA,0XD0
		 CALL           WRIT_DS
		 LDI            DATA,0X04
		 CALL           WRIT_DATA
		 LDI            DATA,0B00000001
		 CALL           WRIT_DATA
		 CALL           STOP_DS            		 
;=========================================================================================================================
;                            *******MASTER_TRANSMITTER_MODE:*****
;SET DS1307 TIME REGISTER MONTH:
;=========================================================================================================================
; Description    : set user-defined month 
; uses   :           
; Input  : adress 05h rtc
; Note   :        ADDRESS BIT 7 BIT 6 BIT 5 BIT   4 BIT  3 BIT 2 BIT 1 BIT 0  RANGE   
;Version :1.0       05h       0     0     0      10Month     Month Month      01–12
         
		 CALL           START_DS
		 LDI            DATA,0XD0
		 CALL           WRIT_DS
		 LDI            DATA,0X05
		 CALL           WRIT_DATA
		 LDI            DATA,0B00000001
		 CALL           WRIT_DATA
		 CALL           STOP_DS     
	
;=========================================================================================================================
;                            *******MASTER_TRANSMITTER_MODE:*****
;SET DS1307 TIME REGISTER YEAR:
;=========================================================================================================================
; Description    : set user-defined year 
; uses   :           
; Input  : adress 06h rtc
; Note   :             ADDRESS  BIT 7 BIT 6 BIT 5 BIT 4 BIT    3 BIT 2 BIT 1 BIT 0  RANGE
;Version :1.0             06h   ------10 Year------------      -------Year-------  00–99

         CALL           START_DS
		 LDI            DATA,0XD0
		 CALL           WRIT_DS
		 LDI            DATA,0X06
		 CALL           WRIT_DATA
		 LDI            DATA,0B00000001
		 CALL           WRIT_DATA
		 CALL           STOP_DS     

         CALL           DS_READ_MODE
;=========================================================================================================================
;START CONDISION TWI:
;=========================================================================================================================
; Description    : START data transfer: A change in the state of the data line,
;                : from HIGH to LOW, while the clock is HIGH,
;                : defines a START condition.
; uses   :           
; Input  :       : R16 status code $08 to check if start condition is don 
; Output :       :
; Note   :
;Version :1.0 

START_DS: 
         
		 LDI           R16 ,(1<<TWINT)|(1<<TWSTA)|(1<<TWEN) 
         OUT           TWCR,R16 
         
WAIT1:
         IN            R16,TWCR         
	     SBRS          R16,TWINT        
  		 RJMP          WAIT1
		 RET                 

		 IN            R16,TWSR         
	     ANDI          R16,0XF8         
    	 CPI           R16,$08          
    	 BRNE          START_ERRO       
         RET 
START_ERRO:
         LDI           R16,0B11111110           
		 CALL          GLOBAL_SUB1		 
		 RET 
;=============================================================================
;                         *******MASTER_TRANSMITTER_MODE:*****
;WRITE SLA_ID_WRIT MODE TO RTC:
;=============================================================================
; Description:DS_Slave_W = 0xD0
; uses             
; Input  : 
; Output :
; Note   :
;Version :1.0

WRIT_DS:  

         OUT           TWDR,DATA 
         LDI           R16,(1<<TWINT)|(1<<TWEN) 
         OUT           TWCR,R16 
          
 WAIT2:
         IN            R16,TWCR         
	     SBRS          R16,TWINT        
  		 RJMP          WAIT2
		 RET                       
         
		 IN            R16,TWSR         
	     ANDI          R16,0XF8         
    	 CPI           R16,$18          
    	 BRNE          SLA_ID_ERRO      
         RET
SLA_ID_ERRO:
         LDI           R16,0B11111101   
         CALL          GLOBAL_SUB1		 
		 RET 
;=============================================================================
;                   *******MASTER_TRANSMITTER_MODE:*****
;WRITE DATA_BYTE TO SLA:
;=============================================================================
; Description:
; uses             
; Input  : 
; Output :
; Note   :
;Version :1.0

WRIT_DATA:

		 OUT           TWDR,DATA        
		 LDI           R16,(1<<TWINT)|(1<<TWEN)
		 OUT           TWCR,R16
		 
WAIT3:
         IN            R16,TWCR         
	     SBRS          R16,TWINT        
  		 RJMP          WAIT3
		 RET            		               
         
		 IN            R16,TWSR         
	     ANDI          R16,0XF8         
    	 CPI           R16,$28          
    	 BRNE          SLA_DATA_ERRO   
         RET
SLA_DATA_ERRO:
         LDI           R16,0B11111011
         CALL          GLOBAL_SUB1
		 RET
;=========================================================================================================================
;twi stop condition:
;=========================================================================================================================
; Description: STOP data transfer: A change in the state of the data line,
;              from LOW to HIGH, while the clock line is HIGH,
;              defines the STOP condition.
; uses   :           
; Input  : 
; Output :
; Note   :
;Version :1.0 

STOP_DS: 
         LDI           R16,(1<<TWINT)|(1<<TWEN)|(1<<TWSTO) 
         OUT           TWCR,R16 
         RET  
;=========================================================================================================================
;erro codes for TWI communication:
;=========================================================================================================================
; Description: 
; uses   :           
; Input  : 
; Output :
; Note   :
;Version : 

GLOBAL_SUB1:
         
		 OUT           PORTB,R16
		 CALL          STOP_DS
         RET

;*****************************master_transmitt_end******************************************
;=========================================================================================================================
;                             *******MASTER_receiving_MODE:***** 
;========================================================================================================================
; Description:
; uses             
; Input  : 
; Output :
; Note   :
;Version :1.0

DS_READ_MODE:
  
LOOP:
    
	     CALL           START_DS
		 LDI            DATA,0XD1
         CALL           READ_ID_DS

		 LDI            DATA,0X00      ; sekunder
		 CALL           WRIT_DATA
		 CALL           STOP_DS
         in             RTC_value,twdr

         RJMP           LOOP


;=========================================================================================================================
;                             *******MASTER_receiving_MODE:***** 
;WRITE SLA_ID_READ MODE TO RTC:
;========================================================================================================================
; Description:DS_Slave_R = 0xD1
; uses             
; Input  : 
; Output :
; Note   :
;Version :1.0

READ_ID_DS:  

         OUT           TWDR,DATA 
         LDI           R16,(1<<TWINT)|(1<<TWEN) 
         OUT           TWCR,R16 
          
 WAIT1R:
         IN            R16,TWCR         
	     SBRS          R16,TWINT        
  		 RJMP          WAIT1R
		 RET                       
         
		 IN            R16,TWSR         
	     ANDI          R16,0XF8         
    	 CPI           R16,$40          
    	 BRNE          WRITE_SLA_ID_READ_MODE    
         RET
WRITE_SLA_ID_READ_MODE:
         LDI           R16,0B11111100   
         CALL          GLOBAL_SUB1		 
		 RET 
