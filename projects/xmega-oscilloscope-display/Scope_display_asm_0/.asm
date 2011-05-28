;-----------------------------------------------------------------------------
;
; Title		: 128x128 display on Oscilloscope
; Author	: Ozhan KD
; Created	: 2011-05-28
; Version	: 1.0
; Target MCU	: Atmel AVR XMEGA series
;	
; DACB channel0&1 generate signals for input channels 
; of an analog oscilloscope in XY mode
;-----------------------------------------------------------------------------
		.INCLUDE "atxmega64a3def.inc"	
	
		.DEF    CHAR_COUNTER=		R21
		.DEF    BIT_COUNTER=		R22
		.EQU    CHAR_BASE=		0X2000
		.EQU	OSC_RC32MRDY_bp=	1		
		.EQU 	DAC_CH0DRE_bp= 		0
		.EQU 	DAC_CH1DRE_bp= 		1
	
	.CSEG
	.ORG	0X00

		
		LDI	R16,OSC_RC32MEN_bm | OSC_RC2MEN_bm
		STS	OSC_CTRL,R16		


	WAIT_FOR_RC32M_READY:

		LDS	R16,OSC_STATUS
		SBRS	R16,OSC_RC32MRDY_bp
		RJMP	WAIT_FOR_RC32M_READY
		LDI	R16,CCP_IOREG_gc		
		LDI	R17,CLK_SCLKSEL_RC32M_gc
		OUT	CPU_CCP,R16
		STS	CLK_CTRL,R17; // clkper = 32 MHz
		
		LDI	R16,DAC_CHSEL_DUAL_gc
		STS	DACB_CTRLB,R16		
		LDI 	R16,DAC_REFSEL_INT1V_gc | DAC_LEFTADJ_bm
		STS	DACB_CTRLC,R16
		LDI	R16,DAC_CONINTVAL_64CLK_gc | DAC_REFRESH_256CLK_gc
		STS	DACB_TIMCTRL,R16
		LDI	R16,DAC_CH1EN_bm | DAC_CH0EN_bm | DAC_ENABLE_bm
		STS	DACB_CTRLA,R16
		
		CLR     CHAR_COUNTER

		LDI     ZL,LOW(SCREEN_TABLE*2)
                LDI     ZH,HIGH(SCREEN_TABLE*2)                
                
                LDI     YL,LOW(CHAR_BASE)                 
                LDI     YH,HIGH(CHAR_BASE)
                MOVW    XL,ZL
                
                CLR     R16
                CLR     R17

        FILL_LOOP:
                
                LPM     
                ST      Y+,R0
                ADIW    ZL,16
                INC     R17
                INC     R16
                CPI     R17,16
                BRNE    FILL_LOOP
                ADIW    XL,1
                TST     R16
                BRNE    CONTINUE_LOOP
                RJMP 	MAIN_LOOP

        CONTINUE_LOOP:

                MOVW    ZL,XL                
                CLR     R17                
                RJMP    FILL_LOOP                

;----------------------------------------------------------------------------

       	MAIN_LOOP:

                LDI     YL,LOW(CHAR_BASE)
                LDI     YH,HIGH(CHAR_BASE)

        NEXT_CHAR:                

                CLR     BIT_COUNTER
                LD      R16,Y+
                LDI     ZL,LOW(CODE_TABLE*2)
                LDI     ZH,HIGH(CODE_TABLE*2)                
                ADD     ZL,R16
                CLR     R16
                ADC     ZH,R16
                LPM                             
                
                LDI     R16,8
                MUL     R16,R0

                LDI     ZL,LOW(FONT_TABLE*2)
                LDI     ZH,HIGH(FONT_TABLE*2)
                
                ADD     ZL,R0                 
                ADC     ZH,R1                

        LOOP_1:

                LPM     R18,Z+
                MOV     R19,CHAR_COUNTER
                CBR     R19,0X0F
                LSR     R19
                
        LOOP_2:

                MOV     R17,BIT_COUNTER
                LSL     R17
                CBR     R17,0B10001111
                SWAP    R17
                MOV     R20,R19
                OR      R20,R17                
		LSL	R20

                MOV     R16,CHAR_COUNTER
                CBR     R16,0XF0
                SWAP    R16
                LSR     R16
                MOV     R17,BIT_COUNTER                
                CBR     R17,0B11111000
                OR      R16,R17		
		LSL	R16

                LSL     R18
                BRCC    ZERO_COORDINATE       

	WAIT_FOR_DACB_CH0:	                
		
		LDS	R17,DACB_STATUS
		SBRS	R17,DAC_CH0DRE_bp
		RJMP	WAIT_FOR_DACB_CH0
		STS	DACB_CH0DATA+1,R16

	WAIT_FOR_DACB_CH1:

		LDS	R17,DACB_STATUS
		SBRS	R17,DAC_CH1DRE_bp
		RJMP	WAIT_FOR_DACB_CH1
		STS	DACB_CH1DATA+1,R20
		

        ZERO_COORDINATE:

                INC     BIT_COUNTER
                MOV     R16,BIT_COUNTER
                CBR     R16,0XF8
                BRNE    LOOP_2
                CPI     BIT_COUNTER,64
                BRNE    LOOP_1
                INC     CHAR_COUNTER

                BRNE    NEXT_CHAR
                RJMP    MAIN_LOOP

;----------------------------------------------------------------------------

        CODE_TABLE:                                  

                .DB      0,  0,  0,  0,  0,  0,  0,  0
                .DB      0,  0,  0,  0,  0,  0,  0,  0
                .DB      0,  0,  0,  0,  0,  0,  0,  0
                .DB      0,  0,  0,  0,  0,  0,  0,  0
                .DB      0,  1,  2,  3,  4,  5,  6,  7
                .DB      8,  9, 10, 11, 12, 13, 14, 15
                .DB      16, 17, 18, 19, 20, 21, 22, 23
                .DB      24, 25, 26, 27, 28, 29, 30, 31
                .DB      32, 33, 34, 35, 36, 37, 38, 39
                .DB      40, 41, 42, 43, 44, 45, 46, 47
                .DB      48, 49, 50, 51, 52, 53, 54, 55
                .DB      56, 57, 58, 59, 60, 61, 62, 63
                .DB      64, 65, 66, 67, 68, 69, 70, 71
                .DB      72, 73, 74, 75, 76, 77, 78, 79
                .DB      80, 81, 82, 83, 84, 85, 86, 87
                .DB      88, 89, 90, 91, 92, 93, 94,  0

;----------------------------------------------------------------------------

        FONT_TABLE:
                         
                .DB      0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00   ; SPACE
                .DB      0X00,0X00,0X00,0XBE,0X00,0X00,0X00,0X00   ; !
                .DB      0X00,0X00,0X0E,0X00,0X0E,0X00,0X00,0X00   ; "
                .DB      0X00,0X24,0X7E,0X24,0X7E,0X24,0X00,0X00   ; #
                .DB      0X00,0X48,0XD4,0X56,0X24,0X00,0X00,0X00   ; $
                .DB      0X00,0X40,0X24,0X10,0X48,0X04,0X00,0X00   ; %
                .DB      0X00,0X6C,0X92,0XAE,0X40,0X80,0X00,0X00   ; &
                .DB      0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00   ; '
                .DB      0X00,0X00,0X00,0X38,0X44,0X82,0X00,0X00   ; (
                .DB      0X00,0X00,0X82,0X44,0X38,0X00,0X00,0X00   ; )
                .DB      0X00,0X24,0X18,0X7E,0X18,0X24,0X00,0X00   ; *
                .DB      0X00,0X10,0X10,0X7C,0X10,0X10,0X00,0X00   ; +
                .DB      0X00,0X00,0X80,0X70,0X30,0X00,0X00,0X00   ; ,
                .DB      0X00,0X10,0X10,0X10,0X10,0X10,0X00,0X00   ; -
                .DB      0X00,0X00,0XC0,0XC0,0X00,0X00,0X00,0X00   ; .
                .DB      0X00,0X80,0X40,0X20,0X10,0X08,0X04,0X00   ; /
 
                .DB      0X00,0X7C,0X82,0X82,0X82,0X7C,0X00,0X00   ; 0
                .DB      0X00,0X80,0X82,0XFE,0X80,0X80,0X00,0X00   ; 1
                .DB      0X00,0XC4,0XA2,0X92,0X8A,0X84,0X00,0X00   ; 2
                .DB      0X00,0X44,0X82,0X92,0X92,0X6C,0X00,0X00   ; 3
                .DB      0X00,0X30,0X28,0XA4,0XFE,0XA0,0X00,0X00   ; 4
                .DB      0X00,0X00,0X9E,0X92,0X92,0X62,0X00,0X00   ; 5
                .DB      0X00,0X78,0X94,0X92,0X92,0X62,0X00,0X00   ; 6
                .DB      0X00,0X06,0X02,0XC2,0X32,0X0E,0X00,0X00   ; 7
                .DB      0X00,0X6C,0X92,0X92,0X92,0X6C,0X00,0X00   ; 8
                .DB      0X00,0X8C,0X92,0X92,0X52,0X3C,0X00,0X00   ; 9
 
                .DB      0X00,0X00,0XCC,0XCC,0X00,0X00,0X00,0X00   ; :
                .DB      0X00,0X00,0X80,0X76,0X36,0X00,0X00,0X00   ; ;
                .DB      0X00,0X10,0X10,0X28,0X28,0X44,0X44,0X00   ; <
                .DB      0X00,0X28,0X28,0X28,0X28,0X28,0X00,0X00   ; =
                .DB      0X00,0X44,0X44,0X28,0X28,0X10,0X10,0X00   ; >
                .DB      0X00,0X00,0X04,0X02,0XB2,0X0C,0X00,0X00   ; ?
                .DB      0X00,0X7C,0X82,0XB2,0XAA,0XBC,0X00,0X00   ; @
                                         
                .DB      0X80,0XC0,0XBA,0X26,0XB8,0XC0,0X80,0X00   ; A
                .DB      0X82,0XFE,0X92,0X92,0X92,0X6C,0X00,0X00   ; B
                .DB      0X00,0X7C,0X82,0X82,0X82,0X46,0X00,0X00   ; C
                .DB      0X82,0XFE,0X82,0X82,0X66,0X38,0X00,0X00   ; D
                .DB      0X00,0X82,0XFE,0X92,0XBA,0XC6,0X00,0X00   ; E
                .DB      0X00,0X82,0XFE,0X92,0X3A,0X06,0X00,0X00   ; F
                .DB      0X00,0X7C,0X82,0X82,0XA2,0X66,0X20,0X00   ; G
                .DB      0X82,0XFE,0X92,0X10,0X92,0XFE,0X82,0X00   ; H
                .DB      0X00,0X82,0X82,0XFE,0X82,0X82,0X00,0X00   ; I
                .DB      0X00,0X60,0X82,0X82,0X7E,0X02,0X00,0X00   ; J
                .DB      0X82,0XFE,0X92,0X10,0X2A,0XC6,0X82,0X00   ; K
                .DB      0X00,0X82,0XFE,0X82,0X80,0XE0,0X00,0X00   ; L
                .DB      0X82,0XFE,0X8E,0X10,0X8E,0XFE,0X82,0X00   ; M
                .DB      0X82,0XFE,0X8E,0X30,0XC2,0XFE,0X02,0X00   ; N
                .DB      0X00,0X7C,0X82,0X82,0X82,0X7C,0X00,0X00   ; O
                .DB      0X00,0X82,0XFE,0XA2,0X22,0X1C,0X00,0X00   ; P
                .DB      0X00,0X3C,0X62,0XC2,0XC2,0XBC,0X00,0X00   ; Q
                .DB      0X00,0X82,0XFE,0XA2,0X22,0X5C,0X80,0X00   ; R
                .DB      0X00,0XCC,0X92,0X92,0X94,0X66,0X00,0X00   ; S
                .DB      0X00,0X06,0X82,0XFE,0X82,0X06,0X00,0X00   ; T
                .DB      0X02,0X7E,0X82,0X80,0X82,0X7E,0X02,0X00   ; U
                .DB      0X02,0X1E,0X72,0X80,0X72,0X1E,0X02,0X00   ; V
                .DB      0X02,0X7e,0X82,0X78,0X82,0X7e,0X02,0X00   ; W
                .DB      0X82,0XC6,0X28,0X10,0X28,0XC6,0X82,0X00   ; X
                .DB      0X02,0X06,0X8A,0XF0,0X8A,0X06,0X02,0X00   ; Y
                .DB      0X00,0XC6,0XA2,0X92,0X8A,0XC6,0X00,0X00   ; Z

                .DB      0X00,0X00,0X00,0XFE,0X82,0X00,0X00,0X00   ; [
                .DB      0X00,0X04,0X08,0X10,0X20,0X40,0X80,0X00   ; \
                .DB      0X00,0X00,0X82,0XFE,0X00,0X00,0X00,0X00   ; ]
                .DB      0X00,0X08,0X04,0X02,0X04,0X08,0X00,0X00   ; ^
                .DB      0X00,0X80,0X80,0X80,0X80,0X80,0X80,0X00   ; _
                .DB      0X00,0X00,0X02,0X04,0X00,0X00,0X00,0X00   ; `

                .DB      0X00,0XD0,0XA8,0XA8,0XA8,0XF0,0X80,0X00   ; a
                .DB      0X82,0XFE,0X90,0X88,0X88,0X70,0X00,0X00   ; b
                .DB      0X00,0X70,0X88,0X88,0X88,0X98,0X00,0X00   ; c
                .DB      0X00,0X70,0X88,0X88,0X92,0XFE,0X80,0X00   ; d    
                .DB      0X00,0X70,0XA8,0XA8,0XA8,0XB0,0X00,0X00   ; e
                .DB      0X00,0X00,0X88,0XFC,0X8A,0X8A,0X00,0X00   ; f
                .DB      0X00,0X18,0XA4,0XA4,0XA8,0X7C,0X04,0X00   ; g
                .DB      0X82,0XFE,0X90,0X08,0X88,0XF0,0X80,0X00   ; h
                .DB      0X00,0X88,0X88,0XFA,0X80,0X80,0X00,0X00   ; i
                .DB      0X00,0X88,0X88,0X8A,0X78,0X00,0X00,0X00   ; j
                .DB      0X82,0XFE,0X20,0XA8,0XD8,0X88,0X00,0X00   ; k
                .DB      0X00,0X80,0X82,0XFE,0X80,0X80,0X00,0X00   ; l    
                .DB      0X88,0XF8,0X88,0XF0,0X88,0XF0,0X80,0X00   ; m
                .DB      0X88,0XF8,0X90,0X08,0X88,0XF0,0X80,0X00   ; n
                .DB      0X00,0X70,0X88,0X88,0X88,0X70,0X00,0X00   ; o
                .DB      0X84,0XFC,0XA8,0X24,0X24,0X18,0X00,0X00   ; p
                .DB      0X00,0X1C,0X22,0X22,0XA4,0XFE,0X82,0X00   ; q
                .DB      0X00,0X88,0XF8,0X90,0X88,0X08,0X00,0X00   ; r
                .DB      0X00,0X90,0XA8,0XA8,0XA8,0X48,0X00,0X00   ; s
                .DB      0X08,0X7C,0X88,0X88,0X88,0X40,0X00,0X00   ; t
                .DB      0X08,0X78,0X80,0X80,0X48,0XF8,0X80,0X00   ; u
                .DB      0X08,0X38,0XC8,0X80,0X68,0X18,0X08,0X00   ; v
                .DB      0X08,0X78,0X88,0X60,0X88,0X78,0X08,0X00   ; w
                .DB      0X00,0X88,0XD8,0X20,0XD8,0X88,0X00,0X00   ; x
                .DB      0X04,0X8C,0XB4,0XC0,0X34,0X0C,0X04,0X00   ; y
                .DB      0X00,0X98,0XC8,0XA8,0X98,0XC8,0X00,0X00   ; z
        
                .DB      0X00,0X00,0X10,0X7C,0X82,0X00,0X00,0X00   ; {
                .DB      0X00,0X00,0X00 ,0XEE,0X00,0X00,0X00,0X00   ; |
                .DB      0X00,0X00,0X82,0X7C,0X10,0X00,0X00,0X00   ; }
                .DB      0X00,0X04,0X02,0X06,0X04,0X02,0X00,0X00   ; ~                
                

;----------------------------------------------------------------------------

        SCREEN_TABLE:

		;	This table appears from bottom to top on oscilloscope

                .DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'		
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'		
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*',' ',' ',' ','O','z','h','a','n',' ','K','D',' ',' ',' ','*'
		.DB     '*',' ',' ','W','r','i','t','t','e','n',' ','B','y',' ',' ','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*',' ',' ',' ',' ','D','i','s','p','l','a','y',' ',' ',' ','*'
		.DB     '*',' ','O','s','c','i','l','l','o','s','c','o','p','e',' ','*'                
		.DB     '*',' ',' ',' ',' ','X','M','E','G','A',' ',' ',' ',' ',' ','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
		.DB     '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'
