;CodeVisionAVR C Compiler V1.23.8c Professional
;(C) Copyright 1998-2003 HP InfoTech s.r.l.
;http://www.hpinfotech.ro
;e-mail:office@hpinfotech.ro

;Chip type           : ATmega8
;Program type        : Application
;Clock frequency     : 4.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;External SRAM size  : 0
;Data Stack size     : 256
;Promote char to int : No
;char is unsigned    : Yes
;8 bit enums         : Yes
;Enhanced core instructions    : On
;Automatic register allocation : On
;Use AVR Studio Terminal I/O   : No

	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCR=0x35
	.EQU GICR=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

	.DEF R0X0=R0
	.DEF R0X1=R1
	.DEF R0X2=R2
	.DEF R0X3=R3
	.DEF R0X4=R4
	.DEF R0X5=R5
	.DEF R0X6=R6
	.DEF R0X7=R7
	.DEF R0X8=R8
	.DEF R0X9=R9
	.DEF R0XA=R10
	.DEF R0XB=R11
	.DEF R0XC=R12
	.DEF R0XD=R13
	.DEF R0XE=R14
	.DEF R0XF=R15
	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31

	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SBIW R24,1
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __CLRD1S
	CLR  R30
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __PUTD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R31
	STD  Y+@0+2,R22
	STD  Y+@0+3,R23
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+@1)
	LDI  R31,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+@1
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	LDS  R22,@0+@1+2
	LDS  R23,@0+@1+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@2,@0+@1
	.ENDM

	.MACRO __GETWRMN
	LDS  R@2,@0+@1
	LDS  R@3,@0+@1+1
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+@1
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	LDS  R24,@0+@1+2
	LDS  R25,@0+@1+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+@1,R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	STS  @0+@1+2,R22
	STS  @0+@1+3,R23
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETWRS
	LDD  R@0,Y+@2
	LDD  R@1,Y+@2+1
	.ENDM

	.MACRO __PUTWSR
	STD  Y+@2,R@0
	STD  Y+@2+1,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	ICALL
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __GETB1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOVW R30,R0
	.ENDM

	.MACRO __GETB2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOVW R26,R0
	.ENDM

	.MACRO __GETBRSX
	MOVW R30,R28
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __LSLW8SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __CLRD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __PUTB2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTBSRX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R@1
	.ENDM

	.MACRO __PUTWSRX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __MULBRR
	MULS R@0,R@1
	MOV  R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOV  R30,R0
	.ENDM

	.CSEG
	.ORG 0

	.INCLUDE "Stepper01.vec"
	.INCLUDE "Stepper01.inc"

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  GICR,R31
	OUT  GICR,R30
	OUT  MCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	LDI  R31,0x10
	OUT  WDTCR,R31

;CLEAR R2-R14
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x400)
	LDI  R25,HIGH(0x400)
	LDI  R26,0x60
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM  R0,Z+
	LPM  R1,Z+
	MOVW R22,R30
	MOVW R30,R0
	SBIW R30,0
	BREQ __GLOBAL_INI_END
	LPM  R26,Z+
	LPM  R27,Z+
	LPM  R24,Z+
	LPM  R25,Z+
__GLOBAL_INI_LOOP:
	LPM  R0,Z+
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOVW R30,R22
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x45F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x45F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x160)
	LDI  R29,HIGH(0x160)

	RJMP _main

	.ESEG
	.ORG 0
	.DB  0 ; FIRST EEPROM LOCATION NOT USED, SEE ATMEL ERRATA SHEETS

	.DSEG
	.ORG 0x160
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.4 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 e-mail:office@hpinfotech.com
;       8 
;       9 Project : 
;      10 Version : 
;      11 Date    : 10/12/2006
;      12 Author  : Salman Shariati                 
;      13 Company : Home                            
;      14 Comments: 
;      15 
;      16 
;      17 Chip type           : ATmega8
;      18 Program type        : Application
;      19 Clock frequency     : 4.000000 MHz
;      20 Memory model        : Small
;      21 External SRAM size  : 0
;      22 Data Stack size     : 256
;      23 *****************************************************/
;      24 
;      25 #include <mega8.h>
;      26 #include <delay.h> 
;      27 #include <stdio.h>
;      28 #include <stdlib.h>
;      29 
;      30 // Alphanumeric LCD Module functions
;      31 #asm
;      32    .equ __lcd_port=0x18 ;PORTB
   .equ __lcd_port=0x18 ;PORTB
;      33 #endasm

;      34 #include <lcd.h>
;      35 
;      36 // Declare your global variables here
;      37 
;      38 void main(void)
;      39 {

	.CSEG
_main:
;      40 // Declare your local variables here
;      41 
;      42 float deg = 2.7;
;      43 char str1[10],str2[28];
;      44 
;      45 // Input/Output Ports initialization
;      46 // Port B initialization
;      47 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;      48 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;      49 PORTB=0x00;
	SBIW R28,42
	LDI  R24,4
	LDI  R26,LOW(38)
	LDI  R27,HIGH(38)
	LDI  R30,LOW(_0x20*2)
	LDI  R31,HIGH(_0x20*2)
	RCALL __INITLOCB
;	deg -> Y+38
;	str1 -> Y+28
;	str2 -> Y+0
	CLR  R30
	OUT  0x18,R30
;      50 DDRB=0xFF;
	LDI  R30,LOW(255)
	OUT  0x17,R30
;      51 
;      52 // Port C initialization
;      53 // Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      54 // State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      55 PORTC=0x00;
	CLR  R30
	OUT  0x15,R30
;      56 DDRC=0x00;
	OUT  0x14,R30
;      57 
;      58 // Port D initialization
;      59 // Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=Out Func0=Out 
;      60 // State7=T State6=T State5=T State4=T State3=0 State2=0 State1=0 State0=0 
;      61 PORTD=0x00;
	OUT  0x12,R30
;      62 DDRD=0x0F;
	LDI  R30,LOW(15)
	OUT  0x11,R30
;      63 
;      64 // Timer/Counter 0 initialization
;      65 // Clock source: System Clock
;      66 // Clock value: Timer 0 Stopped
;      67 TCCR0=0x00;
	CLR  R30
	OUT  0x33,R30
;      68 TCNT0=0x00;
	OUT  0x32,R30
;      69 
;      70 // Timer/Counter 1 initialization
;      71 // Clock source: System Clock
;      72 // Clock value: Timer 1 Stopped
;      73 // Mode: Normal top=FFFFh
;      74 // OC1A output: Discon.
;      75 // OC1B output: Discon.
;      76 // Noise Canceler: Off
;      77 // Input Capture on Falling Edge
;      78 TCCR1A=0x00;
	OUT  0x2F,R30
;      79 TCCR1B=0x00;
	OUT  0x2E,R30
;      80 TCNT1H=0x00;
	OUT  0x2D,R30
;      81 TCNT1L=0x00;
	OUT  0x2C,R30
;      82 ICR1H=0x00;
	OUT  0x27,R30
;      83 ICR1L=0x00;
	OUT  0x26,R30
;      84 OCR1AH=0x00;
	OUT  0x2B,R30
;      85 OCR1AL=0x00;
	OUT  0x2A,R30
;      86 OCR1BH=0x00;
	OUT  0x29,R30
;      87 OCR1BL=0x00;
	OUT  0x28,R30
;      88 
;      89 // Timer/Counter 2 initialization
;      90 // Clock source: System Clock
;      91 // Clock value: Timer 2 Stopped
;      92 // Mode: Normal top=FFh
;      93 // OC2 output: Disconnected
;      94 ASSR=0x00;
	OUT  0x22,R30
;      95 TCCR2=0x00;
	OUT  0x25,R30
;      96 TCNT2=0x00;
	OUT  0x24,R30
;      97 OCR2=0x00;
	OUT  0x23,R30
;      98 
;      99 // External Interrupt(s) initialization
;     100 // INT0: Off
;     101 // INT1: Off
;     102 MCUCR=0x00;
	OUT  0x35,R30
;     103 
;     104 // Timer(s)/Counter(s) Interrupt(s) initialization
;     105 TIMSK=0x00;
	OUT  0x39,R30
;     106 
;     107 // Analog Comparator initialization
;     108 // Analog Comparator: Off
;     109 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     110 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     111 SFIOR=0x00;
	CLR  R30
	OUT  0x30,R30
;     112 
;     113 // LCD module initialization
;     114 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
;     115 
;     116 while (1)
_0x21:
;     117       {
;     118         lcd_gotoxy(0,0);
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x0
	RCALL _lcd_gotoxy
;     119         lcd_putsf("Position : ");
	__POINTW1FN _31,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     120                 
;     121         if (deg <= 360)
	RCALL SUBOPT_0x1
	BREQ PC+3
	BRCS PC+2
	RJMP _0x24
;     122           {
;     123             PORTD = 0b00000110;
	LDI  R30,LOW(6)
	RCALL SUBOPT_0x2
;     124             delay_ms(10);
;     125             deg = deg + 1.8;
;     126             ftoa(deg,1,str1);
;     127             sprintf(str2,"%5s Degree  ",str1); 
	__POINTW1FN _31,12
	RCALL SUBOPT_0x3
	LDI  R24,4
	RCALL SUBOPT_0x4
;     128             lcd_gotoxy(0,1);
	RCALL SUBOPT_0x5
;     129             lcd_puts(str2); 
;     130             PORTD = 0b00000101;
	LDI  R30,LOW(5)
	RCALL SUBOPT_0x2
;     131             delay_ms(10);
;     132             deg = deg + 1.8;
;     133             ftoa(deg,1,str1);
;     134             sprintf(str2,"%5s Degree  ",str1); 
	__POINTW1FN _31,25
	RCALL SUBOPT_0x3
	LDI  R24,4
	RCALL SUBOPT_0x4
;     135             lcd_gotoxy(0,1); 
	RCALL SUBOPT_0x5
;     136             lcd_puts(str2);
;     137             PORTD = 0b00001001;
	LDI  R30,LOW(9)
	RCALL SUBOPT_0x6
;     138             delay_ms(10);  
;     139             deg = deg + 1.8;
;     140             if (deg <= 360)
	RCALL SUBOPT_0x1
	BREQ PC+3
	BRCS PC+2
	RJMP _0x25
;     141               {
;     142               ftoa(deg,1,str1);
	RCALL SUBOPT_0x7
;     143               sprintf(str2,"%5s Degree  ",str1); 
	__POINTW1FN _31,38
	RCALL SUBOPT_0x3
	LDI  R24,4
	RJMP _0xFB
;     144               lcd_gotoxy(0,1);
;     145               lcd_puts(str2);
;     146               PORTD = 0b00001010;
;     147               delay_ms(10);
;     148               deg = deg + 1.8;
;     149               }
;     150             else 
_0x25:
;     151               {
;     152               deg = 0.9;
	__GETD1N 0x3F666666
	__PUTD1S 38
;     153               ftoa(deg,1,str1);
	RCALL SUBOPT_0x7
;     154               sprintf(str2,"%5s Degree  ",str1); 
	__POINTW1FN _31,51
	RCALL SUBOPT_0x3
	LDI  R24,4
_0xFB:
	RCALL _sprintf
	ADIW R28,8
;     155               lcd_gotoxy(0,1);
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x5
;     156               lcd_puts(str2);
;     157               PORTD = 0b00001010;
	LDI  R30,LOW(10)
	RCALL SUBOPT_0x6
;     158               delay_ms(10);
;     159               deg = deg + 1.8;
;     160               }  
;     161           ftoa(deg,1,str1);
	RCALL SUBOPT_0x7
;     162           sprintf(str2,"%5s Degree  ",str1); 
	__POINTW1FN _31,64
	RCALL SUBOPT_0x3
	LDI  R24,4
	RCALL SUBOPT_0x4
;     163           lcd_gotoxy(0,1);
	RCALL SUBOPT_0x5
;     164           lcd_puts(str2);
;     165           }
;     166       }
_0x24:
	RJMP _0x21
;     167 }
	ADIW R28,42
_0x27:
	RJMP _0x27
_getchar:
     sbis usr,rxc
     rjmp _getchar
     in   r30,udr
	RET
_putchar:
     sbis usr,udre
     rjmp _putchar
     ld   r30,y
     out  udr,r30
	ADIW R28,1
	RET
__put_G2:
	put:
	RCALL SUBOPT_0x8
	SBIW R30,0
	BREQ _0x5E
	RCALL SUBOPT_0x8
	ADIW R30,1
	RCALL __PUTWP1
	SBIW R30,1
	MOVW R26,R30
	LDD  R30,Y+2
	ST   X,R30
	RJMP _0x5F
_0x5E:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x5F:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0x61:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x63
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x67
	CPI  R19,37
	BRNE _0x68
	LDI  R16,LOW(1)
	RJMP _0x69
_0x68:
	RCALL SUBOPT_0x9
_0x69:
	RJMP _0x66
_0x67:
	CPI  R30,LOW(0x1)
	BRNE _0x6A
	CPI  R19,37
	BRNE _0x6B
	RCALL SUBOPT_0x9
	LDI  R16,LOW(0)
	RJMP _0x66
_0x6B:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x6C
	LDI  R17,LOW(1)
	RJMP _0x66
_0x6C:
	CPI  R19,43
	BRNE _0x6D
	LDI  R21,LOW(43)
	RJMP _0x66
_0x6D:
	CPI  R19,32
	BRNE _0x6E
	LDI  R21,LOW(32)
	RJMP _0x66
_0x6E:
	RJMP _0x6F
_0x6A:
	CPI  R30,LOW(0x2)
	BRNE _0x70
_0x6F:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x71
	ORI  R17,LOW(128)
	RJMP _0x66
_0x71:
	RJMP _0x72
_0x70:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x66
_0x72:
	CPI  R19,48
	BRLO _0x75
	CPI  R19,58
	BRLO _0x76
_0x75:
	RJMP _0x74
_0x76:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x66
_0x74:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x7A
	RCALL SUBOPT_0xA
	LD   R30,X
	RCALL SUBOPT_0xB
	RJMP _0x7B
_0x7A:
	CPI  R30,LOW(0x73)
	BRNE _0x7D
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xC
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x7E
_0x7D:
	CPI  R30,LOW(0x70)
	BRNE _0x80
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xC
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x7E:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x81
_0x80:
	CPI  R30,LOW(0x64)
	BREQ _0x84
	CPI  R30,LOW(0x69)
	BRNE _0x85
_0x84:
	ORI  R17,LOW(4)
	RJMP _0x86
_0x85:
	CPI  R30,LOW(0x75)
	BRNE _0x87
_0x86:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x88
_0x87:
	CPI  R30,LOW(0x58)
	BRNE _0x8A
	ORI  R17,LOW(8)
	RJMP _0x8B
_0x8A:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0xB9
_0x8B:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x88:
	SBRS R17,2
	RJMP _0x8D
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xD
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x8E
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x8E:
	CPI  R21,0
	BREQ _0x8F
	SUBI R16,-LOW(1)
	RJMP _0x90
_0x8F:
	ANDI R17,LOW(251)
_0x90:
	RJMP _0x91
_0x8D:
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xD
_0x91:
_0x81:
	SBRC R17,0
	RJMP _0x92
_0x93:
	CP   R16,R20
	BRSH _0x95
	SBRS R17,7
	RJMP _0x96
	SBRS R17,2
	RJMP _0x97
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x98
_0x97:
	LDI  R19,LOW(48)
_0x98:
	RJMP _0x99
_0x96:
	LDI  R19,LOW(32)
_0x99:
	RCALL SUBOPT_0x9
	SUBI R20,LOW(1)
	RJMP _0x93
_0x95:
_0x92:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x9A
_0x9B:
	CPI  R18,0
	BREQ _0x9D
	SBRS R17,3
	RJMP _0x9E
	RCALL SUBOPT_0xE
	LPM  R30,Z
	RJMP _0xFC
_0x9E:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xFC:
	ST   -Y,R30
	RCALL SUBOPT_0xF
	CPI  R20,0
	BREQ _0xA0
	SUBI R20,LOW(1)
_0xA0:
	SUBI R18,LOW(1)
	RJMP _0x9B
_0x9D:
	RJMP _0xA1
_0x9A:
_0xA3:
	LDI  R19,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,2
	RCALL __GETW1PF
	STD  Y+8,R30
	STD  Y+8+1,R31
                                      ldd  r26,y+10  ;R26,R27=n
                                      ldd  r27,y+11
                                  calc_digit:
                                      cp   r26,r30
                                      cpc  r27,r31
                                      brlo calc_digit_done
	SUBI R19,-LOW(1)
	                                  sub  r26,r30
	                                  sbc  r27,r31
	                                  brne calc_digit
                                  calc_digit_done:
                                      std  Y+10,r26 ;n=R26,R27
                                      std  y+11,r27
	LDI  R30,LOW(57)
	CP   R30,R19
	BRSH _0xA5
	SBRS R17,3
	RJMP _0xA6
	SUBI R19,-LOW(7)
	RJMP _0xA7
_0xA6:
	SUBI R19,-LOW(39)
_0xA7:
_0xA5:
	SBRC R17,4
	RJMP _0xA9
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0xAB
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0xAA
_0xAB:
	ORI  R17,LOW(16)
	RJMP _0xAD
_0xAA:
	CP   R20,R18
	BRLO _0xAF
	SBRS R17,0
	RJMP _0xB0
_0xAF:
	RJMP _0xAE
_0xB0:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0xB1
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0xAD:
	SBRS R17,2
	RJMP _0xB2
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0xF
	CPI  R20,0
	BREQ _0xB3
	SUBI R20,LOW(1)
_0xB3:
_0xB2:
_0xB1:
_0xA9:
	RCALL SUBOPT_0x9
	CPI  R20,0
	BREQ _0xB4
	SUBI R20,LOW(1)
_0xB4:
_0xAE:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0xA4
	RJMP _0xA3
_0xA4:
_0xA1:
	SBRS R17,0
	RJMP _0xB5
_0xB6:
	CPI  R20,0
	BREQ _0xB8
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0xB
	RJMP _0xB6
_0xB8:
_0xB5:
_0xB9:
_0x7B:
	LDI  R16,LOW(0)
_0x66:
	RJMP _0x61
_0x63:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_sprintf:
	MOV  R15,R24
	SBIW R28,4
	MOVW R26,R28
	RCALL __ADDW2R15
	STD  Y+2,R26
	STD  Y+2+1,R27
	MOVW R26,R28
	ADIW R26,6
	RCALL SUBOPT_0x10
	ST   Y,R30
	STD  Y+1,R31
	MOVW R26,R28
	ADIW R26,4
	RCALL SUBOPT_0x10
	ST   -Y,R31
	ST   -Y,R30
	LDD  R30,Y+4
	LDD  R31,Y+4+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,4
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G2
	LD   R26,Y
	LDD  R27,Y+1
	RCALL SUBOPT_0x11
	ADIW R28,4
	RET
_ftoa:
	SBIW R28,6
	__GETD2S 9
	RCALL __CPD20
	BRGE _0xE3
	__GETD1S 9
	RCALL __ANEGF1
	__PUTD1S 9
	RCALL SUBOPT_0x12
	LDI  R30,LOW(45)
	ST   X,R30
_0xE3:
	LDD  R30,Y+8
	ST   -Y,R30
	LDI  R30,LOW(5)
	ST   -Y,R30
	RCALL _cmin
	STD  Y+8,R30
	LDI  R26,LOW(__fround_G3*2)
	LDI  R27,HIGH(__fround_G3*2)
	CLR  R31
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	RCALL __GETD1PF
	__GETD2S 9
	RCALL __ADDF12
	__PUTD1S 9
	CLR  R30
	STD  Y+1,R30
	__GETD1N 0x3F800000
	__PUTD1S 2
_0xE4:
	RCALL SUBOPT_0x13
	RCALL __CMPF12
	BRLO _0xE6
	RCALL SUBOPT_0x14
	RCALL __MULF12
	__PUTD1S 2
	LDD  R30,Y+1
	SUBI R30,-LOW(1)
	STD  Y+1,R30
	RJMP _0xE4
_0xE6:
	LDD  R30,Y+1
	CPI  R30,0
	BRNE _0xE7
	RCALL SUBOPT_0x12
	LDI  R30,LOW(48)
	ST   X,R30
	RJMP _0xE8
_0xE7:
_0xE9:
	LDD  R30,Y+1
	SUBI R30,LOW(1)
	STD  Y+1,R30
	SUBI R30,-LOW(1)
	BREQ _0xEB
	RCALL SUBOPT_0x14
	RCALL __DIVF21
	__GETD2N 0x3F000000
	RCALL __ADDF12
	RCALL __PUTPARD1
	RCALL _floor
	__PUTD1S 2
	RCALL SUBOPT_0x13
	RCALL __DIVF21
	RCALL SUBOPT_0x15
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	POP  R27
	RCALL SUBOPT_0x17
	__GETD2S 2
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __CDF1
	RCALL __MULF12
	__GETD2S 9
	RCALL SUBOPT_0x18
	RJMP _0xE9
_0xEB:
_0xE8:
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0xEC
	RCALL SUBOPT_0x19
	RJMP _0xFA
_0xEC:
	RCALL SUBOPT_0x12
	LDI  R30,LOW(46)
	ST   X,R30
_0xED:
	LDD  R30,Y+8
	SUBI R30,LOW(1)
	STD  Y+8,R30
	SUBI R30,-LOW(1)
	BREQ _0xEF
	__GETD2S 9
	__GETD1N 0x41200000
	RCALL __MULF12
	__PUTD1S 9
	RCALL SUBOPT_0x15
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	POP  R27
	RCALL SUBOPT_0x17
	__GETD2S 9
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __CDF1
	RCALL SUBOPT_0x18
	RJMP _0xED
_0xEF:
	RCALL SUBOPT_0x19
_0xFA:
	ADIW R28,13
	RET

	.DSEG
__base_y_G4:
	.BYTE 0x4
    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.CSEG
__lcd_ready:
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
    cbi   __lcd_port,__lcd_rs     ;RS=0
__lcd_busy:
    rcall __lcd_delay
    sbi   __lcd_port,__lcd_enable ;EN=1
    rcall __lcd_delay
    sbic  __lcd_pin,__lcd_busy_flag
    rjmp  __lcd_busy
    cbi   __lcd_port,__lcd_enable ;EN=0
	RET
__lcd_write_nibble:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
    rcall __lcd_delay
    cbi   __lcd_port,__lcd_enable ;EN=0
__lcd_delay:
    ldi   r31,15
__lcd_delay0:
    dec   r31
    brne  __lcd_delay0
    ret
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
    rcall __lcd_write_nibble      ;RD=0, write MSN
    ld    r26,y
    swap  r26
    rcall __lcd_write_nibble      ;write LSN
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_gotoxy:
	RCALL __lcd_ready
	LD   R30,Y
	CLR  R31
	SUBI R30,LOW(-__base_y_G4)
	SBCI R31,HIGH(-__base_y_G4)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	RCALL SUBOPT_0x1A
	LDD  R4,Y+1
	LDD  R5,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	RCALL __lcd_ready
	LDI  R30,LOW(12)
	RCALL SUBOPT_0x1A
	RCALL __lcd_ready
	LDI  R30,LOW(1)
	RCALL SUBOPT_0x1A
	CLR  R30
	MOV  R5,R30
	MOV  R4,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R4
	CP   R6,R4
	BRSH _0xF5
	__lcd_putchar1:
	INC  R5
	RCALL SUBOPT_0x0
	ST   -Y,R5
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0xF5:
    rcall __lcd_ready
    sbi  __lcd_port,__lcd_rs ;RS=1
    ld   r26,y
    st   -y,r26
    rcall __lcd_write_data
__lcd_putchar0:
    pop  r31
    pop  r30
	ADIW R28,1
	RET
_lcd_puts:
    ldd  r31,y+1
    ld   r30,y
__lcd_puts0:
    ld   r26,z+
    tst  r26
    breq __lcd_puts1
    st   -y,r26    
    rcall _lcd_putchar
    rjmp __lcd_puts0
__lcd_puts1:
	ADIW R28,2
	RET
_lcd_putsf:
    ld   r30,y
    ldd  r31,y+1
__lcd_putsf0:
	lpm
    tst  r0
    breq __lcd_putsf1
    adiw r30,1
    st   -y,r0
    rcall _lcd_putchar
    rjmp __lcd_putsf0
__lcd_putsf1:
	ADIW R28,2
	RET
__long_delay:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LDD  R6,Y+0
	LD   R30,Y
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G4,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G4,3
	RCALL SUBOPT_0x1B
	RCALL SUBOPT_0x1B
	RCALL SUBOPT_0x1B
	RCALL __long_delay
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x1A
	RCALL __lcd_ready
	LDI  R30,LOW(40)
	RCALL SUBOPT_0x1A
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	RCALL SUBOPT_0x1A
	RCALL _lcd_clear
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x0:
	CLR  R30
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	__GETD2S 38
	__GETD1N 0x43B40000
	RCALL __CMPF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	OUT  0x12,R30
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _delay_ms
	__GETD1S 38
	__GETD2N 0x3FE66666
	RCALL __ADDF12
	__PUTD1S 38
	RCALL __PUTPARD1
	LDI  R30,LOW(1)
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,33
	ST   -Y,R31
	ST   -Y,R30
	RCALL _ftoa
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x3:
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,32
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x4:
	RCALL _sprintf
	ADIW R28,8
	RJMP SUBOPT_0x0

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x5:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	RJMP _lcd_puts

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	OUT  0x12,R30
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _delay_ms
	__GETD1S 38
	__GETD2N 0x3FE66666
	RCALL __ADDF12
	__PUTD1S 38
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x7:
	__GETD1S 38
	RCALL __PUTPARD1
	LDI  R30,LOW(1)
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,33
	ST   -Y,R31
	ST   -Y,R30
	RCALL _ftoa
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x9:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0xA:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	RCALL __GETD1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xE:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	RCALL __ADDW2R15
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x11:
	CLR  R30
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x12:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	__GETD1S 2
	__GETD2S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	__GETD2S 2
	__GETD1N 0x41200000
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	RCALL __CFD1
	ST   Y,R30
	RJMP SUBOPT_0xE

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x16:
	LD   R30,Y
	SUBI R30,-LOW(48)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x17:
	ST   X,R30
	LD   R30,Y
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x18:
	RCALL __SWAPD12
	RCALL __SUBF12
	__PUTD1S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x19:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	RJMP SUBOPT_0x11

;OPTIMIZER ADDED SUBROUTINE, CALLED 9 TIMES
SUBOPT_0x1A:
	ST   -Y,R30
	RJMP __lcd_write_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1B:
	RCALL __long_delay
	LDI  R30,LOW(48)
	RJMP SUBOPT_0x1A

_cmin:
	ld   r26,y+
	ld   r30,y+
	cp   r30,r26
	brlt __cmin0
	mov  r30,r26
__cmin0:
	ret

_strlen:
	ld   r26,y+
	ld   r27,y+
	clr  r30
	clr  r31
__strlen0:
	ld   r22,x+
	tst  r22
	breq __strlen1
	adiw r30,1
	rjmp __strlen0
__strlen1:
	ret

_strlenf:
	clr  r26
	clr  r27
	ld   r30,y+
	ld   r31,y+
__strlenf0:
	lpm  r0,z+
	tst  r0
	breq __strlenf1
	adiw r26,1
	rjmp __strlenf0
__strlenf1:
	movw r30,r26
	ret

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x3E8
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__ftrunc:
	ldd  r23,y+3
	ldd  r22,y+2
	ldd  r31,y+1
	ld   r30,y
	bst  r23,7
	lsl  r23
	sbrc r22,7
	sbr  r23,1
	mov  r25,r23
	subi r25,0x7e
	breq __ftrunc0
	brcs __ftrunc0
	cpi  r25,24
	brsh __ftrunc1
	clr  r26
	clr  r27
	clr  r24
__ftrunc2:
	sec
	ror  r24
	ror  r27
	ror  r26
	dec  r25
	brne __ftrunc2
	and  r30,r26
	and  r31,r27
	and  r22,r24
	rjmp __ftrunc1
__ftrunc0:
	clt
	clr  r23
	clr  r30
	clr  r31
	clr  r22
__ftrunc1:
	cbr  r22,0x80
	lsr  r23
	brcc __ftrunc3
	sbr  r22,0x80
__ftrunc3:
	bld  r23,7
	ld   r26,y+
	ld   r27,y+
	ld   r24,y+
	ld   r25,y+
	cp   r30,r26
	cpc  r31,r27
	cpc  r22,r24
	cpc  r23,r25
	bst  r25,7
	ret

_floor:
	rcall __ftrunc
	brne __floor1
__floor0:
	ret
__floor1:
	brtc __floor0
	ldi  r25,0xbf

__addfc:
	clr  r26
	clr  r27
	ldi  r24,0x80
	rjmp __addf12

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

__ANEGW1:
	COM  R30
	COM  R31
	ADIW R30,1
	RET

__ANEGD1:
	COM  R30
	COM  R31
	COM  R22
	COM  R23
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	SBCI R23,-1
	RET

__CBD1:
	CLR  R31
	CLR  R22
	CLR  R23
	SBRS R30,7
	RET
	SER  R31
	SER  R22
	SER  R23
	RET

__GETW1P:
	LD   R30,X+
	LD   R31,X
	SBIW R26,1
	RET

__GETD1P:
	LD   R30,X+
	LD   R31,X+
	LD   R22,X+
	LD   R23,X
	SBIW R26,3
	RET

__PUTWP1:
	ST   X+,R30
	ST   X,R31
	RET

__GETW1PF:
	LPM  R0,Z+
	LPM  R31,Z
	MOV  R30,R0
	RET

__GETD1PF:
	LPM  R0,Z+
	LPM  R1,Z+
	LPM  R22,Z+
	LPM  R23,Z
	MOVW R30,R0
	RET

__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
	RET

__SWAPD12:
	MOV  R1,R24
	MOV  R24,R22
	MOV  R22,R1
	MOV  R1,R25
	MOV  R25,R23
	MOV  R23,R1

__SWAPW12:
	MOV  R1,R27
	MOV  R27,R31
	MOV  R31,R1

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
	RET

__ANEGF1:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	BREQ __ANEGF10
	SUBI R23,0x80
__ANEGF10:
	RET

__REPACK:
	LDI  R21,0x80
	EOR  R21,R23
	BRNE __REPACK0
	PUSH R21
	RJMP __ZERORES
__REPACK0:
	CPI  R21,0xFF
	BREQ __REPACK1
	LSL  R22
	LSL  R0
	ROR  R21
	ROR  R22
	MOV  R23,R21
	RET
__REPACK1:
	PUSH R21
	TST  R0
	BRMI __REPACK2
	RJMP __MAXRES
__REPACK2:
	RJMP __MINRES

__UNPACK:
	LDI  R21,0x80
	MOV  R1,R25
	AND  R1,R21
	LSL  R24
	ROL  R25
	EOR  R25,R21
	LSL  R21
	ROR  R24

__UNPACK1:
	LDI  R21,0x80
	MOV  R0,R23
	AND  R0,R21
	LSL  R22
	ROL  R23
	EOR  R23,R21
	LSL  R21
	ROR  R22
	RET

__CFD1:
	PUSH R21
	RCALL __UNPACK1
	CPI  R23,0x80
	BRLO __CFD10
	CPI  R23,0xFF
	BRCC __CFD10
	RJMP __ZERORES
__CFD10:
	LDI  R21,22
	SUB  R21,R23
	BRPL __CFD11
	NEG  R21
	CPI  R21,8
	BRLO __CFD17
	SER  R30
	SER  R31
	SER  R22
	LDI  R23,0x7F
	RJMP __CFD15
__CFD17:
	CLR  R23
	TST  R21
	BREQ __CFD15
__CFD18:
	LSL  R30
	ROL  R31
	ROL  R22
	ROL  R23
	DEC  R21
	BRNE __CFD18
	RJMP __CFD15
__CFD11:
	CLR  R23
__CFD12:
	CPI  R21,8
	BRLO __CFD13
	MOV  R30,R31
	MOV  R31,R22
	MOV  R22,R23
	SUBI R21,8
	RJMP __CFD12
__CFD13:
	TST  R21
	BREQ __CFD15
__CFD14:
	LSR  R23
	ROR  R22
	ROR  R31
	ROR  R30
	DEC  R21
	BRNE __CFD14
__CFD15:
	TST  R0
	BRPL __CFD16
	RCALL __ANEGD1
__CFD16:
	POP  R21
	RET

__CDF1U:
	SET
	RJMP __CDF1U0
__CDF1:
	CLT
__CDF1U0:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	BREQ __CDF10
	CLR  R0
	BRTS __CDF11
	TST  R23
	BRPL __CDF11
	COM  R0
	RCALL __ANEGD1
__CDF11:
	MOV  R1,R23
	LDI  R23,30
	TST  R1
__CDF12:
	BRMI __CDF13
	DEC  R23
	LSL  R30
	ROL  R31
	ROL  R22
	ROL  R1
	RJMP __CDF12
__CDF13:
	MOV  R30,R31
	MOV  R31,R22
	MOV  R22,R1
	PUSH R21
	RCALL __REPACK
	POP  R21
__CDF10:
	RET

__SWAPACC:
	MOV  R21,R30
	MOV  R30,R26
	MOV  R26,R21
	MOV  R21,R31
	MOV  R31,R27
	MOV  R27,R21
	MOV  R21,R22
	MOV  R22,R24
	MOV  R24,R21
	MOV  R21,R23
	MOV  R23,R25
	MOV  R25,R21
	MOV  R21,R0
	MOV  R0,R1
	MOV  R1,R21
	RET

__UADD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	RET

__NEGMAN1:
	COM  R30
	COM  R31
	COM  R22
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	RET

__SUBF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129
	LDI  R21,0x80
	EOR  R1,R21

	RJMP __ADDF120

__ADDF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129

__ADDF120:
	CPI  R23,0x80
	BREQ __ADDF128
__ADDF121:
	MOV  R21,R23
	SUB  R21,R25
	BRVS __ADDF129
	BRPL __ADDF122
	RCALL __SWAPACC
	RJMP __ADDF121
__ADDF122:
	CPI  R21,24
	BRLO __ADDF123
	CLR  R26
	CLR  R27
	CLR  R24
__ADDF123:
	CPI  R21,8
	BRLO __ADDF124
	MOV  R26,R27
	MOV  R27,R24
	CLR  R24
	SUBI R21,8
	RJMP __ADDF123
__ADDF124:
	TST  R21
	BREQ __ADDF126
__ADDF125:
	LSR  R24
	ROR  R27
	ROR  R26
	DEC  R21
	BRNE __ADDF125
__ADDF126:
	MOV  R21,R0
	EOR  R21,R1
	BRMI __ADDF127
	RCALL __UADD12
	BRCC __ADDF129
	ROR  R22
	ROR  R31
	ROR  R30
	INC  R23
	BRVC __ADDF129
	RJMP __MAXRES
__ADDF128:
	RCALL __SWAPACC
__ADDF129:
	RCALL __REPACK
	POP  R21
	RET
__ADDF127:
	SUB  R30,R26
	SBC  R31,R27
	SBC  R22,R24
	BREQ __ZERORES
	BRCC __ADDF1210
	COM  R0
	RCALL __NEGMAN1
__ADDF1210:
	TST  R22
	BRMI __ADDF129
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVC __ADDF1210

__MINRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	SER  R23
	POP  R21
	RET

__ZERORES:
	CLR  R30
	CLR  R31
	CLR  R22
	CLR  R23
	POP  R21
	RET

__MAXRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	LDI  R23,0x7F
	POP  R21
	RET

__MULF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BREQ __ZERORES
	CPI  R25,0x80
	BREQ __ZERORES
	EOR  R0,R1
	SEC
	ADC  R23,R25
	BRVC __MULF124
	BRLT __ZERORES
__MULF125:
	TST  R0
	BRMI __MINRES
	RJMP __MAXRES
__MULF124:
	PUSH R19
	PUSH R20
	CLR  R1
	CLR  R19
	CLR  R20
	CLR  R21
	LDI  R25,24
__MULF120:
	LSL  R19
	ROL  R20
	ROL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	BRCC __MULF121
	ADD  R19,R26
	ADC  R20,R27
	ADC  R21,R24
	ADC  R30,R1
	ADC  R31,R1
	ADC  R22,R1
__MULF121:
	DEC  R25
	BRNE __MULF120
	POP  R20
	POP  R19
	TST  R22
	BRMI __MULF122
	LSL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	RJMP __MULF123
__MULF122:
	INC  R23
	BRVS __MULF125
__MULF123:
	RCALL __REPACK
	POP  R21
	RET

__DIVF21:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BRNE __DIVF210
	TST  R1
__DIVF211:
	BRPL __MAXRES
	RJMP __MINRES
__DIVF210:
	CPI  R25,0x80
	BRNE __DIVF218
__DIVF217:
	RJMP __ZERORES
__DIVF218:
	EOR  R0,R1
	SEC
	SBC  R25,R23
	BRVC __DIVF216
	BRLT __DIVF217
	TST  R0
	RJMP __DIVF211
__DIVF216:
	MOV  R23,R25
	LSR  R22
	ROR  R31
	ROR  R30
	LSR  R24
	ROR  R27
	ROR  R26
	PUSH R20
	CLR  R1
	CLR  R20
	CLR  R21
	LDI  R25,24
__DIVF212:
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __DIVF213
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SEC
	RJMP __DIVF214
__DIVF213:
	CLC
__DIVF214:
	ROL  R1
	ROL  R20
	ROL  R21
	ROL  R26
	ROL  R27
	ROL  R24
	DEC  R25
	BRNE __DIVF212
	MOV  R30,R1
	MOV  R31,R20
	MOV  R22,R21
	LSR  R26
	ADC  R30,R25
	ADC  R31,R25
	ADC  R22,R25
	POP  R20
	TST  R22
	BRMI __DIVF215
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVS __DIVF217
__DIVF215:
	RCALL __REPACK
	POP  R21
	RET

__CMPF12:
	TST  R25
	BRMI __CMPF120
	TST  R23
	BRMI __CMPF121
	CP   R25,R23
	BRLO __CMPF122
	BRNE __CMPF121
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __CMPF122
	BREQ __CMPF123
__CMPF121:
	CLZ
	CLC
	RET
__CMPF122:
	CLZ
	SEC
	RET
__CMPF123:
	SEZ
	CLC
	RET
__CMPF120:
	TST  R23
	BRPL __CMPF122
	CP   R25,R23
	BRLO __CMPF121
	BRNE __CMPF122
	CP   R30,R26
	CPC  R31,R27
	CPC  R22,R24
	BRLO __CMPF122
	BREQ __CMPF123
	RJMP __CMPF121

__CPD20:
	SBIW R26,0
	SBCI R24,0
	SBCI R25,0
	RET

__SAVELOCR6:
	ST   -Y,R21
__SAVELOCR5:
	ST   -Y,R20
__SAVELOCR4:
	ST   -Y,R19
__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR6:
	LDD  R21,Y+5
__LOADLOCR5:
	LDD  R20,Y+4
__LOADLOCR4:
	LDD  R19,Y+3
__LOADLOCR3:
	LDD  R18,Y+2
__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

__INITLOCB:
__INITLOCW:
	ADD R26,R28
	ADC R27,R29
__INITLOC0:
	LPM  R0,Z+
	ST   X+,R0
	DEC  R24
	BRNE __INITLOC0
	RET

