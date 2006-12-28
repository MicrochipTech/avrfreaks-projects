;=====================================================================
;                                - Relaes Form -
;=====================================================================
; New [ ]  Update [X]    AVR application [ ]   Created   
;--------------------------------------------------------------------- 
; Title :  4x4keyscan
; Author:  BBH.                                       Date: 27-12-2006                
; E-mail: (BRIAN@maxicoon.dk)
;---------------------------------------------------------------------------------------------------------------- 
; Target AVR MCU       : mega 16 
; Frq xtal             : 3,686 Mhz
; Include file         : m16def.inc. 
; Language             : ASM/Atmel compiler/assembler v2 
; Dev. Tool            : AVR Studio	4.12.460 service paket 2 
; Code available to use: yes 
; Filename             : 4x4keyscan          
; Fil zise  .hex       : 
; Version release      :1.0 4x4keyscan 
;-----------------------------------------------------------------------------------------------------------------
; Description   :scan port c colunm for key push aktiv low stat   
; Input         :KEYVALUE =R22,KEYVALUE2=R23   
; Output        : R0 to lcd for now temp 
;               :  
;
; Note          ;
;                          
;                          
;==================================================================================================================
;Change Log:
;28-03-2006 Created  4x4 keyboard sub
;   
;27-12-2006 make clean 4x4 key scan moduel ready to be include in other programs
;           the LCD moduel is include in the zip.file to make a test if you want to
;           the delay moduel ar inplentet in the LCDmoduel.asm                         
;Version release record 
;-------------------------------------------
;VERSION	DATE		DESCRIPTIO           
;  1      27-12-2006      Created   4x4 keyboardtest .        
;-----------------------------------------------------------------------------  
;KEYBOARD ON PORTC







;=============================================================================
;MAIN KEYBOARD SCAN 
;=============================================================================
;config:
.include"F:\A V R programmering\AVR Development project RELEASE\4x4keyscan\LCDmoduel.asm"
;.include"F:\A V R PROGRAMMERING\AVR DEVELOPMENT PROJECT RELEASE\4X4KEYSCAN\delay_1Ms.asm" 
;the delay ar include in lcdmoduel
;.INCLUDE"M16DEF.INC" ar also include in lcdmoduel
;.DEF TEMP=R16              ;temporary variable
;.DEF TEMP1=R17		        ;temporary variable
.def	KEYVALUE =R22       ;key value register
.def	KEYVALUE2=R23       ;key value register
.equ	col1	 =0b11101111	
.equ	col2     =0b11011111	
.equ	col3     =0b10111111	
.equ	col4     =0b01111111		           
;=============================================================================
KEY_SCAN:              
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       : Pc0,1,2,3 inputs, Pc4,5,6,7 outputs enable internal pull-ups on Pc0-Pc4                 
;Version     :1.0 

		ldi	    temp,0b11110000 	                       
		out	    DDRC,temp
		ldi	    temp,0b00001111	                        	
		CALL    OUT_PORTC

Loopagain:
    	
		LDI     KEYVALUE,$30        
		LDI     KEYVALUE2,$30		
		
		CALL	lcd_clr
        CALL    LCD_INIT_8BIT
		
		LDI	    ZH,high(2*message1)	                
		LDI	    ZL,low(2*message1)              	 
		CALL	loadbyte       
;=============================================================================
check_keys:                    ; Enable column 1                  
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                
;Version     :1.0 
		LDI 	temp,col1 		
		CALL    OUT_PORTC

		sbic 	PINC,PC0		
		rjmp 	key2			
		ldi 	KEYVALUE,$37
        CALL    DELAY_5Ms	
		rjmp	KEY_TO_LCD	

key2:	sbic 	PINC,PC1		
		rjmp 	key3			
		ldi	    KEYVALUE,$34
		CALL    DELAY_5Ms		
		rjmp	KEY_TO_LCD		

key3:	sbic 	PINC,PC2		
		rjmp 	key4			
		ldi 	KEYVALUE,$31	
		CALL    DELAY_5Ms	
		rjmp	KEY_TO_LCD	

key4:	sbic 	PINC,PC3		
		rjmp 	key5			
		ldi     KEYVALUE,$30
		ldi	    KEYVALUE2,$31	
		CALL    DELAY_5Ms	
		rjmp	KEY_TO_LCD	
;=============================================================================
;check_keys:                     Enable column 2                    
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                    
;Version     :1.0 

key5:   ldi 	temp,col2 		 
		CALL    OUT_PORTC

		sbic 	PINC,PC0		
		rjmp 	key6			 
		ldi 	KEYVALUE,$38
		CALL    DELAY_5Ms	
		rjmp	KEY_TO_LCD

key6:   sbic 	PINC,PC1		
		rjmp 	key7			 
		ldi     KEYVALUE,$35
		CALL    DELAY_5Ms		 
		rjmp	KEY_TO_LCD

key7:	sbic 	PINC,PC2		
		rjmp 	key8			 
		ldi     KEYVALUE,$32	
		CALL    DELAY_5Ms		 
		rjmp	KEY_TO_LCD

key8:   sbic 	PINC,PC3		
		rjmp 	key9			 
		ldi     KEYVALUE,$30
		CALL    DELAY_5Ms		 
		rjmp	KEY_TO_LCD
;=============================================================================
;check_keys:                     Enable column 3                   
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                  
;Version     :1.0 

key9:   LDI     temp,col3	
		CALL    OUT_PORTC

		sbic 	PINC,PC0		
		rjmp 	key10			 
		ldi 	KEYVALUE,$39
		CALL    DELAY_5Ms 
		rjmp	KEY_TO_LCD

key10:  sbic 	PINC,PC1		
		rjmp 	key11			 
		ldi     KEYVALUE,$36		 
		ldi     KEYVALUE2,$30
        CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD

key11:  sbic 	PINC,PC2		
		rjmp 	key12			 
		ldi     KEYVALUE,$33		 
		ldi     KEYVALUE2,$30
        CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD

key12:  sbic 	PINC,PC3		
		rjmp 	key13			 
		ldi     KEYVALUE,$31		 
		ldi     KEYVALUE2,$31
        CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD
;=============================================================================
;;check_keys:                     Enable column 4                    
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                   
;Version     :1.0 

key13:	ldi     temp,col4	
		CALL    OUT_PORTC

		sbic 	PINC,PC0	
		rjmp 	key14			 
		ldi     KEYVALUE,$32		 
		ldi     KEYVALUE2,$31
        CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD

key14:  sbic 	PINC,PC1		
		rjmp 	key15			 
		ldi     KEYVALUE,$33		 
		ldi     KEYVALUE2,$31
        CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD

key15:  sbic 	PINC,PC2		
		rjmp 	key16			 
		ldi     KEYVALUE,$34		 
		ldi     KEYVALUE2,$31
		CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD

key16:  sbic 	PINC,PC3		
		rjmp 	check_keys		 
		ldi     KEYVALUE,$35		 
        ldi     KEYVALUE2,$31
		CALL    DELAY_5Ms
		rjmp	KEY_TO_LCD
;=============================================================================
;small sub that is made for save codespace in key_scan moduel                   
;=============================================================================
; Description:in stet off write the same code many times i make a sud call for it
; Input      :
; Output     :  
; Note       :                 
;Version     :1.0 
       
OUT_PORTC:
          OUT     PORTC,temp
DELAY_5Ms:LDI     LCD_DELAYTIME,5
          CALL    LCD_DELAY
		  RET
;=============================================================================
KEY_TO_LCD:                   
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                  
;Version     :1.0 

		call 	lcd_clr	   
        CALL    LCD_INIT_8BIT
		
		ldi     ZH,high(2*message2)	
		ldi     ZL,low(2*message2)	
		call	loadbyte		
;=============================================================================
;                    
;=============================================================================
; Description:copy the register to lcd register and then call lcd_out.
; Input      :
; Output     :  
; Note       :                  
;Version     :1.0 
		
		MOV     R0,KEYVALUE2
 		CALL	lcd_OUT
		
		MOV     R0,KEYVALUE
		CALL	lcd_OUT
		
		LDI     LCD_DELAYTIME,255
        CALL    LCD_DELAY       
		LDI     LCD_DELAYTIME,255
        CALL    LCD_DELAY       	
				
		CALL	Loopagain
;=============================================================================
;LCD MESSAGE                    
;=============================================================================
; Description:
; Input      :
; Output     :  
; Note       :                   
;Version     : 

message1:
.db	"Press any key",0

message2:
.db	"You pressed",0 

;===================================END OF KEYBOARD SCAN============================
