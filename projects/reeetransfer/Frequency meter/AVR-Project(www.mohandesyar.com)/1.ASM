;CodeVisionAVR C Compiler V1.24.1c Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.ro
;e-mail:office@hpinfotech.ro

;Chip type           : ATmega16
;Program type        : Application
;Clock frequency     : 1.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;(s)scanf features   : long, width
;External SRAM size  : 0
;Data Stack size     : 256 byte(s)
;Heap size           : 0 byte(s)
;Promote char to int : No
;char is unsigned    : Yes
;8 bit enums         : Yes
;Enhanced core instructions    : On
;Automatic register allocation : On

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

	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0

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

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+@2)
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+@3)
	LDI  R@1,HIGH(@2+@3)
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
	CALL __PUTDP1
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
	CALL __PUTDP1
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
	CALL __PUTDP1
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
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOVW R26,R@0
	ADIW R26,@1
	CALL __PUTDP1
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

	.INCLUDE "1.vec"
	.INCLUDE "1.inc"

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
	OUT  WDTCR,R30

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
	LPM  R24,Z+
	LPM  R25,Z+
	SBIW R24,0
	BREQ __GLOBAL_INI_END
	LPM  R26,Z+
	LPM  R27,Z+
	LPM  R0,Z+
	LPM  R1,Z+
	MOVW R22,R30
	MOVW R30,R0
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

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x160
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.25.3 Professional
;       4 Automatic Program Generator
;       5 © Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 
;       8 Project : 
;       9 Version : 
;      10 Date    : 2007/04/23
;      11 Author  : F4CG                            
;      12 Company : F4CG                            
;      13 Comments: 
;      14 
;      15 
;      16 Chip type           : ATmega16
;      17 Program type        : Application
;      18 Clock frequency     : 1.000000 MHz
;      19 Memory model        : Small
;      20 External SRAM size  : 0
;      21 Data Stack size     : 256
;      22 *****************************************************/
;      23 
;      24 #include <mega16.h>
;      25 
;      26 // Alphanumeric LCD Module functions
;      27 #asm
;      28    .equ __lcd_port=0x12 ;PORTD
   .equ __lcd_port=0x12 ;PORTD
;      29 #endasm

;      30 #include <lcd.h>
;      31 #include <delay.h>
;      32 #include <stdlib.h>
;      33 
;      34 int n=0;
;      35 char *s;
_s:
	.BYTE 0x2
;      36 bit p=0;
;      37 int x=0;
;      38 int y=0;
;      39 int count1=0;
;      40 int count2=0;
;      41 
;      42 // External Interrupt 1 service routine
;      43 interrupt [EXT_INT1] void ext_int1_isr(void)
;      44 {

	.CSEG
_ext_int1_isr:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R15
	ST   -Y,R22
	ST   -Y,R23
	ST   -Y,R24
	ST   -Y,R25
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      45 // Place your code here
;      46 int i;
;      47 TCCR0=0x14;
	ST   -Y,R17
	ST   -Y,R16
;	i -> R16,R17
	LDI  R30,LOW(20)
	CALL SUBOPT_0x0
;      48 TCNT0=0x00;
;      49 OCR0=0x00;
;      50 if (n>4 & p!=1) {
	__GETW2R 4,5
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CALL __GTW12
	PUSH R30
	CLR  R30
	SBRC R2,0
	LDI  R30,1
	LDI  R26,LOW(1)
	CALL __NEB12
	POP  R26
	AND  R30,R26
	BREQ _0x3
;      51 n=0;
	CLR  R4
	CLR  R5
;      52 x++;
	CALL SUBOPT_0x1
;      53 };
_0x3:
;      54 n++;
	__GETW1R 4,5
	ADIW R30,1
	__PUTW1R 4,5
;      55 for (i=0;i<=4;i++) {
	__GETWRN 16,17,0
_0x5:
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R16
	CPC  R31,R17
	BRLT _0x6
;      56 delay_ms(30);
	LDI  R30,LOW(30)
	LDI  R31,HIGH(30)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;      57     };
	__ADDWRN 16,17,1
	RJMP _0x5
_0x6:
;      58 }
	LD   R16,Y+
	LD   R17,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R25,Y+
	LD   R24,Y+
	LD   R23,Y+
	LD   R22,Y+
	LD   R15,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;      59 
;      60 // Timer 0 overflow interrupt service routine
;      61 interrupt [TIM0_OVF] void timer0_ovf_isr(void)
;      62 {
_timer0_ovf_isr:
	CALL SUBOPT_0x2
;      63 }
	RETI
;      64 
;      65 // Timer 0 output compare interrupt service routine
;      66 interrupt [TIM0_COMP] void timer0_comp_isr(void)
;      67 {
_timer0_comp_isr:
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      68 // Place your code here
;      69 count1++;
	__GETW1R 10,11
	ADIW R30,1
	__PUTW1R 10,11
;      70 if(count1>=5)
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R10,R30
	CPC  R11,R31
	BRLT _0x7
;      71 count2++;
	__GETW1R 12,13
	ADIW R30,1
	__PUTW1R 12,13
;      72 if(count2>=10)
_0x7:
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CP   R12,R30
	CPC  R13,R31
	BRLT _0x8
;      73 {
;      74 x++;
	CALL SUBOPT_0x1
;      75 n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;      76 count2=0;
	CLR  R12
	CLR  R13
;      77 count1=0;  
	CLR  R10
	CLR  R11
;      78 TCCR0=0x00;
	CLR  R30
	CALL SUBOPT_0x0
;      79 TCNT0=0x00;
;      80 OCR0=0x00;
;      81 };
_0x8:
;      82 }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	RETI
;      83 
;      84 // Timer 2 overflow interrupt service routine
;      85 interrupt [TIM2_OVF] void timer2_ovf_isr(void)
;      86 {
_timer2_ovf_isr:
	CALL SUBOPT_0x2
;      87 // Place your code here
;      88 
;      89 }
	RETI
;      90 
;      91 // Declare your global variables here
;      92 
;      93 
;      94 void main(void)
;      95 {
_main:
;      96 // Declare your local variables here
;      97 
;      98 int BK=0;
;      99 int z=0;
;     100 // Input/Output Ports initialization
;     101 // Port A initialization
;     102 // Func7=In Func6=In Func5=In Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     103 // State7=T State6=T State5=T State4=0 State3=0 State2=0 State1=0 State0=0 
;     104 PORTA=0x00;
;	BK -> R16,R17
;	z -> R18,R19
	LDI  R16,0
	LDI  R17,0
	LDI  R18,0
	LDI  R19,0
	CLR  R30
	OUT  0x1B,R30
;     105 DDRA=0x1F;
	LDI  R30,LOW(31)
	OUT  0x1A,R30
;     106 
;     107 // Port B initialization
;     108 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     109 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     110 PORTB=0x00;
	CLR  R30
	OUT  0x18,R30
;     111 DDRB=0x08;
	LDI  R30,LOW(8)
	OUT  0x17,R30
;     112 
;     113 // Port C initialization
;     114 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     115 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     116 PORTC=0x00;
	CLR  R30
	OUT  0x15,R30
;     117 DDRC=0x00;
	OUT  0x14,R30
;     118 
;     119 // Port D initialization
;     120 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     121 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     122 PORTD=0x00;
	OUT  0x12,R30
;     123 DDRD=0x00;
	OUT  0x11,R30
;     124 
;     125 // Timer/Counter 0 initialization
;     126 // Clock source: System Clock
;     127 // Clock value: 3.906 kHz
;     128 // Mode: Normal top=FFh
;     129 // OC0 output: Toggle on compare match
;     130 TCCR0=0x00;
	CALL SUBOPT_0x0
;     131 TCNT0=0x00;
;     132 OCR0=0x00;
;     133 
;     134 // Timer/Counter 1 initialization
;     135 // Clock source: System Clock
;     136 // Clock value: Timer 1 Stopped
;     137 // Mode: Normal top=FFFFh
;     138 // OC1A output: Discon.
;     139 // OC1B output: Discon.
;     140 // Noise Canceler: Off
;     141 // Input Capture on Falling Edge
;     142 // Timer 1 Overflow Interrupt: Off
;     143 // Input Capture Interrupt: Off
;     144 // Compare A Match Interrupt: Off
;     145 // Compare B Match Interrupt: Off
;     146 TCCR1A=0x00;
	CLR  R30
	OUT  0x2F,R30
;     147 TCCR1B=0x00;
	OUT  0x2E,R30
;     148 TCNT1H=0x00;
	OUT  0x2D,R30
;     149 TCNT1L=0x00;
	OUT  0x2C,R30
;     150 ICR1H=0x00;
	OUT  0x27,R30
;     151 ICR1L=0x00;
	OUT  0x26,R30
;     152 OCR1AH=0x00;
	OUT  0x2B,R30
;     153 OCR1AL=0x00;
	OUT  0x2A,R30
;     154 OCR1BH=0x00;
	OUT  0x29,R30
;     155 OCR1BL=0x00;
	OUT  0x28,R30
;     156 
;     157 // Timer/Counter 2 initialization
;     158 // Clock source: System Clock
;     159 // Clock value: 0.977 kHz
;     160 // Mode: Normal top=FFh
;     161 // OC2 output: Disconnected
;     162 ASSR=0x00;
	OUT  0x22,R30
;     163 TCCR2=0x07;
	LDI  R30,LOW(7)
	OUT  0x25,R30
;     164 TCNT2=0x00;
	CLR  R30
	OUT  0x24,R30
;     165 OCR2=0x00;
	OUT  0x23,R30
;     166 
;     167 // External Interrupt(s) initialization
;     168 // INT0: Off
;     169 // INT1: On
;     170 // INT1 Mode: Rising Edge
;     171 // INT2: Off
;     172 GICR|=0x80;
	IN   R30,0x3B
	ORI  R30,0x80
	OUT  0x3B,R30
;     173 MCUCR=0x0C;
	LDI  R30,LOW(12)
	OUT  0x35,R30
;     174 MCUCSR=0x00;
	CLR  R30
	OUT  0x34,R30
;     175 GIFR=0x80;
	LDI  R30,LOW(128)
	OUT  0x3A,R30
;     176 
;     177 // Timer(s)/Counter(s) Interrupt(s) initialization
;     178 TIMSK=0x42;
	LDI  R30,LOW(66)
	OUT  0x39,R30
;     179 
;     180 // Analog Comparator initialization
;     181 // Analog Comparator: Off
;     182 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     183 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     184 SFIOR=0x00;
	CLR  R30
	OUT  0x30,R30
;     185 
;     186 // LCD module initialization
;     187 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     188 
;     189 // Global enable interrupts
;     190 #asm("sei")
	sei
;     191 
;     192 while (1)
_0x9:
;     193       {
;     194       if(x>=15)
	LDI  R30,LOW(15)
	LDI  R31,HIGH(15)
	CP   R6,R30
	CPC  R7,R31
	BRLT _0xC
;     195       {
;     196       x=0;
	CLR  R6
	CLR  R7
;     197       y=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 8,9
;     198       };
_0xC:
;     199 
;     200       PORTA.0=1;
	SBI  0x1B,0
;     201       if(PINA.7==1)
	SBIS 0x19,7
	RJMP _0xD
;     202       {
;     203       p=0;
	CLT
	BLD  R2,0
;     204       if(BK!=1){
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0xE
;     205       if(z==1)
	CP   R30,R18
	CPC  R31,R19
	BRNE _0xF
;     206    //   x++; 
;     207       z=1;
	__GETWRN 18,19,1
;     208       BK=1;
_0xF:
	__GETWRN 16,17,1
;     209       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     210       };
_0xE:
;     211         itoa(1,s);
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CALL SUBOPT_0x3
;     212         lcd_puts(s);
;     213         PORTA.4=0;
	CBI  0x1B,4
;     214         PORTA.4=1;
	SBI  0x1B,4
;     215     };// Ta hala shomare 1 halle
_0xD:
;     216     
;     217     if (PINA.6==1) {
	SBIS 0x19,6
	RJMP _0x10
;     218     p=0;
	CLT
	BLD  R2,0
;     219     if(BK!=2){  
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x11
;     220   //  if(z==1)
;     221 //    x++;
;     222     z=1;
	__GETWRN 18,19,1
;     223       BK=2;
	__GETWRN 16,17,2
;     224       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     225       };
_0x11:
;     226 
;     227          if(n==1){
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x12
;     228     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     229     lcd_putsf("a");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     230     };
_0x12:
;     231     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x13
;     232     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     233     lcd_putsf("b");
	__POINTW1FN _0,2
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     234     };
_0x13:
;     235     if(n==3){
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x14
;     236 lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     237     lcd_putsf("c");
	__POINTW1FN _0,4
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     238     };
_0x14:
;     239     if(n==4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x15
;     240     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     241     itoa(2,s);
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CALL SUBOPT_0x3
;     242     lcd_puts(s); 
;     243     };
_0x15:
;     244         
;     245     PORTA.4=0;
	CBI  0x1B,4
;     246     PORTA.4=1;
	SBI  0x1B,4
;     247 };//ta hala shomatre 2 halle
_0x10:
;     248 
;     249 if (PINA.5==1) {
	SBIS 0x19,5
	RJMP _0x16
;     250       p=0;
	CLT
	BLD  R2,0
;     251       if(BK!=3){ 
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x17
;     252   //    if(z==1)
;     253      // x++; 
;     254       z=1;
	__GETWRN 18,19,1
;     255       BK=3;
	__GETWRN 16,17,3
;     256       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     257       };
_0x17:
;     258 // lcd_clear();
;     259       if(n==1){  
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x18
;     260       lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     261     lcd_putsf("d");
	__POINTW1FN _0,6
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     262     };
_0x18:
;     263     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x19
;     264         lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     265     lcd_putsf("e");
	__POINTW1FN _0,8
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     266     };
_0x19:
;     267     if(n==3){   
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x1A
;     268         lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     269     lcd_putsf("f");
	__POINTW1FN _0,10
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     270     };
_0x1A:
;     271     if(n==4){ 
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x1B
;     272         lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     273     itoa(3,s);
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CALL SUBOPT_0x3
;     274     lcd_puts(s);
;     275     };       
_0x1B:
;     276     
;     277  PORTA.4=0;
	CBI  0x1B,4
;     278  PORTA.4=1;
	SBI  0x1B,4
;     279 };// t hala shomare 3 halle.
_0x16:
;     280 
;     281       PORTA.0=0;
	CBI  0x1B,0
;     282       PORTA.1=1;
	SBI  0x1B,1
;     283       PORTA.2=0;
	CBI  0x1B,2
;     284       PORTA.3=0;
	CBI  0x1B,3
;     285       if(PINA.7==1)
	SBIS 0x19,7
	RJMP _0x1C
;     286       {
;     287       p=0;
	CLT
	BLD  R2,0
;     288       if(BK!=4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x1D
;     289      // if(z==1)
;     290     //  x++; 
;     291       z=1;
	__GETWRN 18,19,1
;     292       BK=4;
	__GETWRN 16,17,4
;     293       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     294       };
_0x1D:
;     295 //lcd_clear();
;     296     if(n==1){
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x1E
;     297     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     298     lcd_putsf("g");
	__POINTW1FN _0,12
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     299     };
_0x1E:
;     300     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x1F
;     301     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     302     lcd_putsf("h");
	__POINTW1FN _0,14
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     303     };
_0x1F:
;     304     if(n==3){  
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x20
;     305     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     306     lcd_putsf("i");
	__POINTW1FN _0,16
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     307     };
_0x20:
;     308     if(n==4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x21
;     309     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     310     itoa(4,s);
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CALL SUBOPT_0x3
;     311     lcd_puts(s);
;     312     };  
_0x21:
;     313 
;     314        PORTA.4=0;
	CBI  0x1B,4
;     315        PORTA.4=1;
	SBI  0x1B,4
;     316     };// ta hala shomare 4 halle.
_0x1C:
;     317     
;     318     if (PINA.6==1) {
	SBIS 0x19,6
	RJMP _0x22
;     319     p=0; 
	CLT
	BLD  R2,0
;     320     if(BK!=5){
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x23
;     321       BK=5;
	__GETWRN 16,17,5
;     322   //    if(z==1)
;     323   //    x++; 
;     324       z=1;
	__GETWRN 18,19,1
;     325       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     326       };
_0x23:
;     327 //     lcd_clear();
;     328      if(n==1){ 
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x24
;     329     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     330     lcd_putsf("j");
	__POINTW1FN _0,18
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     331     };
_0x24:
;     332     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x25
;     333     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     334     lcd_putsf("k");
	__POINTW1FN _0,20
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     335     };
_0x25:
;     336     if(n==3){   
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x26
;     337     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     338     lcd_putsf("l");
	__POINTW1FN _0,22
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     339     };
_0x26:
;     340     if(n==4){  
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x27
;     341     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     342     itoa(5,s);
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL SUBOPT_0x3
;     343     lcd_puts(s);
;     344     };
_0x27:
;     345 
;     346     PORTA.4=0;
	CBI  0x1B,4
;     347     PORTA.4=1;
	SBI  0x1B,4
;     348 };// ta hala shomare 5 halle.
_0x22:
;     349 
;     350 if (PINA.5==1) {
	SBIS 0x19,5
	RJMP _0x28
;     351       p=0;
	CLT
	BLD  R2,0
;     352       if(BK!=6){
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x29
;     353   //    if(z==1)
;     354   //    x++; 
;     355       z=1;
	__GETWRN 18,19,1
;     356       BK=6;
	__GETWRN 16,17,6
;     357       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     358       };
_0x29:
;     359  //lcd_clear();
;     360  if(n==1){
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x2A
;     361  lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     362     lcd_putsf("m");
	__POINTW1FN _0,24
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     363     };
_0x2A:
;     364     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x2B
;     365     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     366     lcd_putsf("n");
	__POINTW1FN _0,26
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     367     };
_0x2B:
;     368     if(n==3){
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x2C
;     369     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     370     lcd_putsf("o");
	__POINTW1FN _0,28
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     371     };
_0x2C:
;     372     if(n==4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x2D
;     373     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     374     itoa(6,s);
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	CALL SUBOPT_0x3
;     375     lcd_puts(s);
;     376     }; 
_0x2D:
;     377 
;     378  PORTA.4=0;
	CBI  0x1B,4
;     379  PORTA.4=1;
	SBI  0x1B,4
;     380 };// ta hala shomare 6 halle.
_0x28:
;     381 
;     382       PORTA.0=0;
	CBI  0x1B,0
;     383       PORTA.1=0;
	CBI  0x1B,1
;     384       PORTA.2=1;
	SBI  0x1B,2
;     385       PORTA.3=0;
	CBI  0x1B,3
;     386       if(PINA.7==1)
	SBIS 0x19,7
	RJMP _0x2E
;     387       { 
;     388       p=1;
	SET
	BLD  R2,0
;     389       if(BK!=7){
	LDI  R30,LOW(7)
	LDI  R31,HIGH(7)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x2F
;     390   //    if(z==1)
;     391   //    x++; 
;     392       z=1;
	__GETWRN 18,19,1
;     393       BK=7;
	__GETWRN 16,17,7
;     394       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     395       };
_0x2F:
;     396 //lcd_clear();
;     397  if(n==1){  
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x30
;     398  lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     399     lcd_putsf("p");
	__POINTW1FN _0,30
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     400     };
_0x30:
;     401     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x31
;     402     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     403     lcd_putsf("q");
	__POINTW1FN _0,32
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     404     };
_0x31:
;     405     if(n==3){
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x32
;     406     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     407     lcd_putsf("r");
	__POINTW1FN _0,34
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     408     };
_0x32:
;     409         if(n==4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x33
;     410         lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     411     lcd_putsf("s");
	__POINTW1FN _0,36
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     412     };
_0x33:
;     413     if(n==5){
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x34
;     414     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     415     itoa(7,s);
	LDI  R30,LOW(7)
	LDI  R31,HIGH(7)
	CALL SUBOPT_0x3
;     416     lcd_puts(s);
;     417     };
_0x34:
;     418         if(n>=5){
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R4,R30
	CPC  R5,R31
	BRLT _0x35
;     419         p=0;
	CLT
	BLD  R2,0
;     420         };
_0x35:
;     421        PORTA.4=0;
	CBI  0x1B,4
;     422        PORTA.4=1;
	SBI  0x1B,4
;     423     };//ta hala shomare 7 halle.
_0x2E:
;     424     
;     425     if (PINA.6==1) {
	SBIS 0x19,6
	RJMP _0x36
;     426     p=0;
	CLT
	BLD  R2,0
;     427     if(BK!=8){
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x37
;     428   //    if(z==1)
;     429   //    x++; 
;     430       z=1;
	__GETWRN 18,19,1
;     431       BK=8;
	__GETWRN 16,17,8
;     432       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     433       };
_0x37:
;     434 //lcd_clear();
;     435  if(n==1){
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x38
;     436  lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     437     lcd_putsf("t");
	__POINTW1FN _0,38
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     438     };
_0x38:
;     439     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x39
;     440     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     441     lcd_putsf("u");
	__POINTW1FN _0,40
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     442     };
_0x39:
;     443     if(n==3){
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x3A
;     444     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     445     lcd_putsf("v");
	__POINTW1FN _0,42
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     446     };
_0x3A:
;     447     if(n==4){ 
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x3B
;     448     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     449     itoa(8,s);
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	CALL SUBOPT_0x3
;     450     lcd_puts(s);
;     451     };
_0x3B:
;     452     PORTA.4=0;
	CBI  0x1B,4
;     453     PORTA.4=1;
	SBI  0x1B,4
;     454 };// ta hala shomare 8 halle.
_0x36:
;     455 
;     456 if (PINA.5==1) {
	SBIS 0x19,5
	RJMP _0x3C
;     457 p=1; 
	SET
	BLD  R2,0
;     458       if(BK!=9){
	LDI  R30,LOW(9)
	LDI  R31,HIGH(9)
	CP   R30,R16
	CPC  R31,R17
	BREQ _0x3D
;     459    //   if(z==1)
;     460   //    x++; 
;     461       z=1;
	__GETWRN 18,19,1
;     462       BK=9;
	__GETWRN 16,17,9
;     463       n=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 4,5
;     464       };
_0x3D:
;     465 //lcd_clear();
;     466  if(n==1){ 
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x3E
;     467  lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     468     lcd_putsf("w");
	__POINTW1FN _0,44
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     469     };
_0x3E:
;     470     if(n==2){
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x3F
;     471     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     472     lcd_putsf("x");
	__POINTW1FN _0,46
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     473     };
_0x3F:
;     474     if(n==3){
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x40
;     475     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     476     lcd_putsf("y");
	__POINTW1FN _0,48
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     477     };
_0x40:
;     478     if(n==4){
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x41
;     479     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     480     lcd_putsf("z");
	__POINTW1FN _0,50
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     481     };
_0x41:
;     482     if(n==5){
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R30,R4
	CPC  R31,R5
	BRNE _0x42
;     483     lcd_gotoxy(x,y);
	CALL SUBOPT_0x4
;     484     itoa(9,s);
	LDI  R30,LOW(9)
	LDI  R31,HIGH(9)
	CALL SUBOPT_0x3
;     485     lcd_puts(s);
;     486     };
_0x42:
;     487     if(n>=5){
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R4,R30
	CPC  R5,R31
	BRLT _0x43
;     488     p=0;
	CLT
	BLD  R2,0
;     489     };
_0x43:
;     490  PORTA.4=0;
	CBI  0x1B,4
;     491  PORTA.4=1;
	SBI  0x1B,4
;     492 };
_0x3C:
;     493       PORTA.0=0;
	CBI  0x1B,0
;     494       PORTA.1=0;
	CBI  0x1B,1
;     495       PORTA.2=0;
	CBI  0x1B,2
;     496       PORTA.3=1;
	SBI  0x1B,3
;     497       if(PINA.7==1)
	SBIS 0x19,7
	RJMP _0x44
;     498       {
;     499       p=0;
	CLT
	BLD  R2,0
;     500       lcd_clear();
	CALL _lcd_clear
;     501       lcd_putsf("*");
	__POINTW1FN _0,52
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     502     };
_0x44:
;     503     if (PINA.6==1) {
	SBIS 0x19,6
	RJMP _0x45
;     504     p=0;
	CLT
	BLD  R2,0
;     505     //lcd_clear();
;     506     itoa(0,s);
	CLR  R30
	CLR  R31
	CALL SUBOPT_0x3
;     507     lcd_puts(s);
;     508 };
_0x45:
;     509     if (PINA.5==1) {
	SBIS 0x19,5
	RJMP _0x46
;     510       p=0;
	CLT
	BLD  R2,0
;     511       lcd_clear();
	CALL _lcd_clear
;     512       lcd_putsf("#");
	__POINTW1FN _0,54
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     513 };
_0x46:
;     514       };
	RJMP _0x9
;     515 }
_0x47:
	RJMP _0x47
    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G2:
	.BYTE 0x4
__lcd_y:
	.BYTE 0x1
__lcd_maxx:
	.BYTE 0x1

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
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
    rcall __lcd_delay
    sbi   __lcd_port,__lcd_enable ;EN=1
    rcall __lcd_delay
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
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
__lcd_read_nibble:
    sbi   __lcd_port,__lcd_enable ;EN=1
    rcall __lcd_delay
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
    rcall __lcd_delay
    andi  r30,0xf0
    ret
_lcd_read_byte0_G2:
    rcall __lcd_delay
    rcall __lcd_read_nibble       ;read MSN
    mov   r26,r30
    rcall __lcd_read_nibble       ;read LSN
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	CALL __lcd_ready
	LD   R30,Y
	CLR  R31
	SUBI R30,LOW(-__base_y_G2)
	SBCI R31,HIGH(-__base_y_G2)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	ST   -Y,R30
	CALL __lcd_write_data
	LDD  R14,Y+1
	LD   R30,Y
	STS  __lcd_y,R30
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R30,LOW(2)
	CALL SUBOPT_0x5
	LDI  R30,LOW(12)
	CALL SUBOPT_0x5
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	CLR  R30
	STS  __lcd_y,R30
	MOV  R14,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R14
	LDS  R30,__lcd_maxx
	CP   R30,R14
	BRSH _0x49
	__lcd_putchar1:
	LDS  R30,__lcd_y
	SUBI R30,-LOW(1)
	STS  __lcd_y,R30
	CLR  R30
	ST   -Y,R30
	LDS  R30,__lcd_y
	ST   -Y,R30
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x49:
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
__long_delay_G2:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G2:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
    rcall __lcd_write_nibble      ;RD=0, write MSN
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LD   R30,Y
	STS  __lcd_maxx,R30
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G2,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G2,3
	CALL SUBOPT_0x6
	CALL SUBOPT_0x6
	CALL SUBOPT_0x6
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	CALL SUBOPT_0x7
	LDI  R30,LOW(4)
	CALL SUBOPT_0x7
	LDI  R30,LOW(133)
	CALL SUBOPT_0x7
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x4A
	CLR  R30
	RJMP _0x4B
_0x4A:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x4B:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x0:
	OUT  0x33,R30
	CLR  R30
	OUT  0x32,R30
	OUT  0x3C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	__GETW1R 6,7
	ADIW R30,1
	__PUTW1R 6,7
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	ST   -Y,R30
	IN   R30,SREG
	ST   -Y,R30
	LD   R30,Y+
	OUT  SREG,R30
	LD   R30,Y+
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 10 TIMES
SUBOPT_0x3:
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_s
	LDS  R31,_s+1
	ST   -Y,R31
	ST   -Y,R30
	CALL _itoa
	LDS  R30,_s
	LDS  R31,_s+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  _lcd_puts

;OPTIMIZER ADDED SUBROUTINE, CALLED 34 TIMES
SUBOPT_0x4:
	__GETW1R 6,7
	ST   -Y,R30
	__GETW1R 8,9
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __lcd_ready

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x6:
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x7:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G2

_itoa:
	ld   r26,y+
	ld   r27,y+
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	brpl __itoa0
	com  r30
	com  r31
	adiw r30,1
	ldi  r22,'-'
	st   x+,r22
__itoa0:
	clt
	ldi  r24,low(10000)
	ldi  r25,high(10000)
	rcall __itoa1
	ldi  r24,low(1000)
	ldi  r25,high(1000)
	rcall __itoa1
	ldi  r24,100
	clr  r25
	rcall __itoa1
	ldi  r24,10
	rcall __itoa1
	mov  r22,r30
	rcall __itoa5
	clr  r22
	st   x,r22
	ret
__itoa1:
	clr	 r22
__itoa2:
	cp   r30,r24
	cpc  r31,r25
	brlo __itoa3
	inc  r22
	sub  r30,r24
	sbc  r31,r25
	brne __itoa2
__itoa3:
	tst  r22
	brne __itoa4
	brts __itoa5
	ret
__itoa4:
	set
__itoa5:
	subi r22,-0x30
	st   x+,r22
	ret

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0xFA
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__NEB12:
	CP   R30,R26
	LDI  R30,1
	BRNE __NEB12T
	CLR  R30
__NEB12T:
	RET

__GTW12:
	CP   R30,R26
	CPC  R31,R27
	LDI  R30,1
	BRLT __GTW12T
	CLR  R30
__GTW12T:
	RET

