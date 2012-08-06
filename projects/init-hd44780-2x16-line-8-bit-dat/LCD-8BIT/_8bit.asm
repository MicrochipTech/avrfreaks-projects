
;===============================================================================================================
;                                - Relaes Form -
;===============================================================================================================
; New [ ]  Update [X] after req.27-12-2006      [ ]                Created   
;--------------------------------------------------------------------------------------------------------------- 
; Title :  8bit HD44780-2 LINE 16 CHAR GLOBAL LCD-MODUEL   
; Author:  BBH                                       Date:01-12-2005                 
; E-mail: (brian@maxicoon.dk)
;---------------------------------------------------------------------------------------------------------------- 
; Target AVR MCU       : mega 16 
; Frq xtal             : 3,686 Mhz 
; Include file         : m16def.inc 
; Language             : ASM/Atmel compiler/assembler v2 
; Dev. Tool            : AVR Studio	4.12.460 
; Code available to use: yes
; Filename             : lcd_8bit-LPM.ASM         
; Fil zise  .hex       : 
;-----------------------------------------------------------------------------------------------------------------
; Description   : SETS LCD POWERTRIP TO 8 BIT DATA, 2 LINE, 5X8 FRONT CHAR, cursor on,blinking of cursor  character position 
; Input         : BUSYGFLAG DB7 PORT "B"  
; Output        : CTRL OUTPUT = PORT "D". ENABLE=PD0   RW=PD1   RS=PD2 
;               : DATA OUTPUT = PORT "B". DB0-DB7 EQU LCD DATA. 
;
;                              MAIN lcd        STACK_POINTER
; Note                         LCD_BUSYFLAG    LCD_INIT_8BIT           
;              :LCD sub calls: LCD_ENABLE      LCD_WRITE_COMMAND       LCD_FUNKTIONSSET        LOAD BYTE
;                              LCD_CLR         LCD_READ_COMMAND        LCD_SET_line2_COMMAND   LCD_OUT
;                              CUROSER_HOME    LCD_DELAY               LCD_SET_line1_COMMAND   (message)( onley to show that lcd) 
;==================================================================================================================
;Change Log:
;01-12-2005  Created. 
;27-12-2005  Loads the string "message" from program memory (LPM). byte by byte.(REMOVE messages-strign from loadbyte moduel) 
;15-03-2006  don som program cleaning on LPM moduel.
;15-03-2006  lcd_write_command donte clear en,rs bit after aktion thes is correctet.
;20-03-2006  make som change in old/new sub moduels. to make lcd moduel more availabl for Global use.
;24-03-2006  make LCD_8bit-lpm clean lcd moduel.is only init the LCD(HD44780)-2line-16char
;31-03-2006  bug in LCD_BUSYFLAG i use CALL instet off rjmp can give (stack problem).bug is fix now.
;27-12-2006  bug.Make sure to reinit lcd in xx sec. In my case is don by call LCD_FUNKTIONSSET and use delay
;            Every sec.Else lcd losing setup and maybe text string if you wait to long before you writhe to it aging 
;            bug is fix now            
;Version release record 
;-------------------------------------------------------------------------------------------------------------------
;VERSION:	DATE		DESCRIPTIO            
;  1      01-12-2005      Created 
;  2      15-03-2006    se change log 
;  3      24-03-2006    ------"------ release clean lcd-lpm moduel  avr freaks
;  4      27-12-2006    ------"------ update.bug is fix now (lcd losing setup) release lcd-lpm moduel  avr freaks 
;===================================================================================================================
;hardware connect:
;LCD.   MCU
;1 -
;2 + 5,5 dc
;3  contrast (10k pot)
;________________________________________________________________________
;4 rs    Pd2
;5 rw    Pd1              port D
;6 en    Pd0  
;_________________________________________________________________________
;7  Db0   Pb0
;8  Db1   Pb1
;9  Db2   Pb2
;10 Db3   Pb3              port B   
;11 Db4   Pb4
;12 Db5   Pb5
;13 Db6   Pb6
;=============================================================================
;MAIN lcd 
;=============================================================================
.INCLUDE"M16DEF.INC"
.EQU LCD_PORT=PORTD         ;PORT D  CTRL OUTPUT RS,RW,EN TO LCD
.EQU LCD_PIN=PIND           ;PORT B USE AS DATABUS TO LCD , db7=busyflag.
.EQU LCD_EN=PD0             ;EN = ENABEL DATA READ/WRITE LCD                                      						    
.EQU LCD_RW=PD1             ;RW =
.EQU LCD_RS=PD2             ;RS =
.DEF LCD_DELAYTIME=R20      ;R20=TIMES FAKTOR OF 1Ms)R21
.DEF TEMP=R16               ;temporary variable
.DEF TEMP1=R17		        ;temporary variable	           
;=============================================================================
;STACK_POINTER:
;=============================================================================
; Description    : sets stack pointer up
; uses   :           
; Input  : 
; Output :
; Note   :for Global use in atmega xx
;Version :1.0 
.CSEG
.ORG 0
reset: 
     LDI           R25,LOW(ramend)
	 OUT           SPL,R25            
     LDI           R25,HIGH(ramend)
     OUT           SPH,R25               
;=============================================================================
;MAIN PROGRAM CALLS
;=============================================================================
    LDI            TEMP,0B11111111
	OUT            DDRB,TEMP             ;init port B,D to output  
    OUT            DDRD,TEMP

	LDI            LCD_DELAYTIME,255
	CALL           LCD_DELAY 
	LDI            LCD_DELAYTIME,255
	CALL           LCD_DELAY                             
	CALL           LCD_CLR
    CALL           LCD_INIT_8BIT
	CALL           LCD_FUNKTIONSSET  
	call           message              ; test moduel 	          
;=============================================================================
LCD_CLR:
;=============================================================================
; Description:Clears entire display and sets DDRAM address 0 in address counter.
;            : Also returns display from being shifted to original position.           
; Input  :
; Output :   
; Note   :                      
;Version :1.4 
					 				  
     LDI            TEMP,0B00000001     
     OUT            PORTB,temp                        
     CALL           LCD_ENABLE
	 CALL           LCD_BUSYFLAG 
	 LDI            LCD_DELAYTIME,255      
	 CALL           LCD_DELAY              
     LDI            LCD_DELAYTIME,255     
     CALL           LCD_DELAY              
     LDI            LCD_DELAYTIME,255
	 CALL           LCD_DELAY
	 CALL           CURSOR_HOME
	 CALL           LCD_INIT_8BIT
	 CALL           LCD_FUNKTIONSSET 
     RET
;=============================================================================			   
CURSOR_HOME:			      
;=============================================================================				                                                       
; Description
; Input  : 
; Output :   
; Note   :
;Version :1
			  
        LDI            TEMP,0B00000010     
	    OUT            PORTB,temp
        CALL           LCD_ENABLE
        OUT            PORTB,TEMP
        CALL           LCD_BUSYFLAG
	    RET 
;=============================================================================
LCD_BUSYFLAG:
;=============================================================================
; Description:Read busyflag indicating internal operation in lcd
; uses       :      
; Input      :the busy flag is input to pb7 on atmega mcu.
; Output     :sets rw(PD1) for reading bf and address counter contents.
; Note       :                    
;Version     :1.4 
           
	     CLR            temp1	                 
         LDI            TEMP,0B00000000           
         OUT            DDRB,TEMP            
         CALL           LCD_ENABLE      
         SBI            LCD_PORT,LCD_RW
	     IN             TEMP1,7              
         CALL           LCD_ENABLE      
         SBRC           TEMP1,7              
         RJMP           LCD_BUSYFLAG         
         CBI            LCD_PORT,LCD_RW
	     LDI            TEMP,0B11111111     
         OUT            DDRB,TEMP            
         RET
;=============================================================================
LCD_ENABLE:
;=============================================================================
; Description: Starts data read/write. lcd (enable/strobe)
; uses             
; Input  :
; Output : port "D"  pind 0
; Note   : 
;Version :1
               
         SBI           LCD_PORT,LCD_EN
         NOP
	     NOP
         CBI           LCD_PORT,LCD_EN
         RET
;=============================================================================
LCD_WRITE_COMMAND:
;============================================================================= 
; Description:Writes data into DDRAM or CGRAM. Send character data to lcd
; uses   :            
; Input  :  
; Output :sets  en,rs on port d 
; Note   :                          
;Version :1.1 change (put en,rs low after aktion)
          
	LDI            TEMP,0B00000101
    OUT            PORTD,TEMP
	NOP
	NOP
    CLR            TEMP
	OUT            PORTD,TEMP            
	RET
;=============================================================================
LCD_READ_COMMAND:
;=============================================================================
; Description: Read data from lcd CG or DDRAM 
; uses       :      
; Input      :
; Output     :  
; Note       : SETS  en,rw,rs (READ FROM LCD)                  
;Version     :0. never ben testet i dont use it.
            
    LDI           TEMP,0b00000111
    OUT           PORTB,TEMP
    NOP
	NOP
    CLR           TEMP
	OUT           PORTD,TEMP            
	RET
;=============================================================================
LCD_SET_line1_COMMAND:
;=============================================================================
; Description:sets lcd to start on line 1 start at char 0
; uses       :                                                                                  
; Input      :
; Output     :
; Note       :      
;Version     :1  
       
    LDI            TEMP,0b10000000                  
	OUT            PORTB,TEMP                      
    CALL           LCD_ENABLE                       
	CALL           LCD_BUSYFLAG
	RET  
;=============================================================================
LCD_SET_line2_COMMAND:
;=============================================================================
; Description:sets lcd to start on line 2 start at char 0  
; uses       :      
; Input      :
; Output     :
; Note       :        
; Version    :1 
        
    LDI            TEMP,0B11000000                               
	OUT            PORTB,TEMP                      
    CALL           LCD_ENABLE                 
	CALL           LCD_BUSYFLAG
	RET       		  		      		  
;=============================================================================
LCD_DELAY:
;=============================================================================
; Description: delay in Ms TIMES VALUE in register r20 = timedelay
; uses             
; Input  : 
; Output :for Global use in avr atmega xx se version.
; Note   :inputvalue in r21 = lcd_delaytime
;Version :2.0 cal. for 3,686 Mhz

.EQU CODETIME_3_686_1MS_VALUE=244
CODETIME_DELAYMS:
        LDI           R21,CODETIME_3_686_1MS_VALUE 
CODETIME_DMS_INNERLOOP:
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        DEC             R21
        BRNE            CODETIME_DMS_INNERLOOP
        DEC             R20
        BRNE            CODETIME_DELAYMS
        RET
;=============================================================================
LCD_INIT_8BIT:
;=============================================================================
; Description: Initializing lcd. sets 8-bit interface data 
; uses  :           
; Input :  
; Output:sets port d as ctrl output en,rw,rs . Sets port b as output lcd data
; Note  :Wait for more than 15 Ms after VCC rises to 4.5 V and then Initializing lcd.
; Note  :busyflag cannot be use	yet                               
;Version:1 
                                 
        LDI             TEMP,0XFF
        OUT             DDRB,TEMP                  
 
        LDI             TEMP,0B0111               
        OUT             DDRD,TEMP

        LDI             TEMP,0B000110000           
        OUT             PORTB,TEMP
        CALL            LCD_ENABLE                   
        LDI             LCD_DELAYTIME,16              
        CALL            LCD_DELAY                      
                                                     
	    CALL            LCD_ENABLE       
        LDI             LCD_DELAYTIME,5           
        CALL            LCD_DELAY
         
	    CALL            LCD_ENABLE  
        LDI             LCD_DELAYTIME,1
        CALL            LCD_DELAY
        RET
;=============================================================================
LCD_FUNKTIONSSET:                                            
;=============================================================================
; Descrip:Function set.8-bit interface data.Display,cursor,blinking on/off control.Entry mode set.
; uses            
; Input:
; Output: 
; noto  :
; Note:busyflag can be checked after the following instructions.	                 
 ;Version :1.1  
                  
        LDI            TEMP,0B00111000                      
        OUT            PORTB,TEMP
        CALL           LCD_ENABLE
        LDI            LCD_DELAYTIME,1
        CALL           LCD_DELAY
          
        LDI            TEMP,0B00001111          
        OUT            PORTB,TEMP
        CALL           LCD_ENABLE
        CALL           LCD_BUSYFLAG                                
        
        LDI            TEMP,0B00000110            
        OUT            PORTB,TEMP
        RET 	  
;=============================================================================
LOADBYTE:                        
;=============================================================================
; Description:This Application use the Load Program Memory (LPM)
;             instruction.Loads the string "message" from 
;             program memory byte by byte.      
; Input      :
; Output     :  
; Note       :                   
;Version     :1.2 don som program cleaning on the sub. To make sub more availabl 

        LPM			     	                     
        TST	               R0
	    BREQ	           RETURN
        CALL               LCD_OUT
	    ADIW	           ZL,1			             
	    CALL	           LOADBYTE
RETURN: RET
;=============================================================================
LCD_OUT:                    
;=============================================================================
; Description: send char to LCD
; Input      :
; Output     :  
; Note       :remov from loadbyte to stand a lone sub for global lcd use                  
;Version     :1.1  

        OUT 	           PORTB,R0		             
        CALL               LCD_WRITE_COMMAND
	    CALL               LCD_BUSYFLAG
	    LDI                LCD_DELAYTIME,1
        CALL               LCD_DELAY
	    RET	
		
;=============================================================================
message:           ;test moduel onley to show that lcd moduel works                   
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :Here is typed the message string to LCD                  
;Version     : 
        
        CALL    LCD_SET_line1_COMMAND
		LDI     LCD_DELAYTIME,15
        CALL    LCD_DELAY 
		
		LDI	    ZH,high(2*message1)	
		LDI	    ZL,low(2*message1)	
		CALL	loadbyte

        LDI     LCD_DELAYTIME,15
        CALL    LCD_DELAY 

        Call    LCD_SET_line2_COMMAND         
        LDI     LCD_DELAYTIME,15              
        CALL    LCD_DELAY 
        
		LDI	    ZH,high(2*message2)	
		LDI	    ZL,low(2*message2)	
		CALL	loadbyte
;=============================================================================
 quit:                             
;=============================================================================
; Description: reinit lcd Every sec.Else lcd losing setup and maybe text string 
; Input      :
; Output     :  
; Note       :                  
;Version     :1 
     LDI            LCD_DELAYTIME,255      
	 CALL           LCD_DELAY              
     LDI            LCD_DELAYTIME,255     
     CALL           LCD_DELAY              
     LDI            LCD_DELAYTIME,255
	 CALL           LCD_DELAY                                                 	
     LDI            LCD_DELAYTIME,255
	 CALL           LCD_DELAY        
     call           LCD_FUNKTIONSSET
     call           quit


message1:
.db	"CODE WORK",0                           
                                            
message2:                         
.db "are you happy ",0						 
;===================================END OF LCD===============================
