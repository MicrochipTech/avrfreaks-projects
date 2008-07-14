;CodeVisionAVR C Compiler V1.24.6 Professional
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega8515
;Program type           : Application
;Clock frequency        : 1.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 128 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Automatic register allocation : On

	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCSR=0x34
	.EQU MCUCR=0x35
	.EQU EMCUCR=0x36
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

	.EQU __se_bit=0x20

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
	LDI  R30,0
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
	LDS  R@0,@1+@2
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+@3
	LDS  R@1,@2+@3+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
	.ENDM

	.MACRO __GETD2Z
	LDD  R26,Z+@0
	LDD  R27,Z+@0+1
	LDD  R24,Z+@0+2
	LDD  R25,Z+@0+3
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

	.MACRO __PUTDZ2
	STD  Z+@0,R26
	STD  Z+@0+1,R27
	STD  Z+@0+2,R24
	STD  Z+@0+3,R25
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __PUTBZR
	STD  Z+@1,R@0
	.ENDM

	.MACRO __PUTWZR
	STD  Z+@2,R@0
	STD  Z+@2+1,R@1
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


	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	RCALL __GETW1PF
	ICALL
	.ENDM


	.MACRO __CALL2EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMRDW
	ICALL
	.ENDM


	.MACRO __GETW1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X
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
	RCALL __PUTDP1
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
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
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

	.INCLUDE "TelPad.vec"
	.INCLUDE "TelPad.inc"

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
	OUT  EMCUCR,R30

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
	LDI  R24,LOW(0x200)
	LDI  R25,HIGH(0x200)
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
	LDI  R30,LOW(0x25F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x25F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0xE0)
	LDI  R29,HIGH(0xE0)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0xE0
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.6 Professional
;       4 Automatic Program Generator
;       5 © Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 e-mail:office@hpinfotech.com
;       8 
;       9 Project : TelPad
;      10 Version : 0.1
;      11 Date    : 7/14/2008
;      12 Author  : Srinivasan                     
;      13 Company : Scorpion Controls                         
;      14 Comments: 
;      15 TelPad is a project to use the Look Up Table(LUT) in the realtime environment......
;      16 
;      17 
;      18 Chip type           : ATmega8515
;      19 Program type        : Application
;      20 Clock frequency     : 1.000000 MHz
;      21 Memory model        : Small
;      22 External SRAM size  : 0
;      23 Data Stack size     : 128
;      24 *****************************************************/
;      25 
;      26 #include <mega8515.h>
;      27 
;      28 // Standard Input/Output functions
;      29 #include <stdio.h>                      
;      30 
;      31 #define TRUE 1
;      32 #define FALSE 0
;      33 
;      34 // Declare your global variables here  
;      35 char enter=0x0D; 
;      36 const char TelPad_LUT[4][3]={'1','2','3',

	.CSEG
;      37                              '4','5','6',
;      38                              '7','8','9',
;      39                              '*','0','#'};  
;      40 char getkeycode(char *row,char *col);
;      41 
;      42 void main(void)
;      43 {
_main:
;      44 // Declare your local variables here
;      45 
;      46 char row,col;
;      47 char i; 
;      48 // Input/Output Ports initialization
;      49 // Port A initialization
;      50 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      51 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      52 PORTA=0x00;
;	row -> R16
;	col -> R17
;	i -> R18
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;      53 DDRA=0x00;
	OUT  0x1A,R30
;      54 
;      55 // Port B initialization
;      56 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      57 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      58 PORTB=0x00;
	OUT  0x18,R30
;      59 DDRB=0x00;
	OUT  0x17,R30
;      60 
;      61 // Port C initialization
;      62 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;      63 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;      64 PORTC=0x00;
	OUT  0x15,R30
;      65 DDRC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x14,R30
;      66 
;      67 // Port D initialization
;      68 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      69 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      70 PORTD=0x00;
	LDI  R30,LOW(0)
	OUT  0x12,R30
;      71 DDRD=0x00;
	OUT  0x11,R30
;      72 
;      73 // Port E initialization
;      74 // Func2=In Func1=In Func0=In 
;      75 // State2=T State1=T State0=T 
;      76 PORTE=0x00;
	OUT  0x7,R30
;      77 DDRE=0x00;
	OUT  0x6,R30
;      78 
;      79 // Timer/Counter 0 initialization
;      80 // Clock source: System Clock
;      81 // Clock value: Timer 0 Stopped
;      82 // Mode: Normal top=FFh
;      83 // OC0 output: Disconnected
;      84 TCCR0=0x00;
	OUT  0x33,R30
;      85 TCNT0=0x00;
	OUT  0x32,R30
;      86 OCR0=0x00;
	OUT  0x31,R30
;      87 
;      88 // Timer/Counter 1 initialization
;      89 // Clock source: System Clock
;      90 // Clock value: Timer 1 Stopped
;      91 // Mode: Normal top=FFFFh
;      92 // OC1A output: Discon.
;      93 // OC1B output: Discon.
;      94 // Noise Canceler: Off
;      95 // Input Capture on Falling Edge
;      96 // Timer 1 Overflow Interrupt: Off
;      97 // Input Capture Interrupt: Off
;      98 // Compare A Match Interrupt: Off
;      99 // Compare B Match Interrupt: Off
;     100 TCCR1A=0x00;
	OUT  0x2F,R30
;     101 TCCR1B=0x00;
	OUT  0x2E,R30
;     102 TCNT1H=0x00;
	OUT  0x2D,R30
;     103 TCNT1L=0x00;
	OUT  0x2C,R30
;     104 ICR1H=0x00;
	OUT  0x25,R30
;     105 ICR1L=0x00;
	OUT  0x24,R30
;     106 OCR1AH=0x00;
	OUT  0x2B,R30
;     107 OCR1AL=0x00;
	OUT  0x2A,R30
;     108 OCR1BH=0x00;
	OUT  0x29,R30
;     109 OCR1BL=0x00;
	OUT  0x28,R30
;     110 
;     111 // External Interrupt(s) initialization
;     112 // INT0: Off
;     113 // INT1: Off
;     114 // INT2: Off
;     115 MCUCR=0x00;
	OUT  0x35,R30
;     116 EMCUCR=0x00;
	OUT  0x36,R30
;     117 
;     118 // Timer(s)/Counter(s) Interrupt(s) initialization
;     119 TIMSK=0x00;
	OUT  0x39,R30
;     120 
;     121 // USART initialization
;     122 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     123 // USART Receiver: On
;     124 // USART Transmitter: On
;     125 // USART Mode: Asynchronous
;     126 // USART Baud rate: 9600 (Double Speed Mode)
;     127 UCSRA=0x02;
	LDI  R30,LOW(2)
	OUT  0xB,R30
;     128 UCSRB=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
;     129 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     130 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     131 UBRRL=0x0C;
	LDI  R30,LOW(12)
	OUT  0x9,R30
;     132 
;     133 // Analog Comparator initialization
;     134 // Analog Comparator: Off
;     135 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     136 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     137 
;     138 while (1)
_0x4:
;     139       { 
;     140       // Place your code here 
;     141       row=0;
	LDI  R16,LOW(0)
;     142       col=0;  
	LDI  R17,LOW(0)
;     143       i= getkeycode(&row,&col); 
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R16
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R17
	RCALL _getkeycode
	POP  R17
	POP  R16
	MOV  R18,R30
;     144       printf("%c",enter); 
	__POINTW1FN _0,0
	RCALL SUBOPT_0x0
	LDI  R24,4
	RCALL SUBOPT_0x1
;     145       if (i==TRUE)
	CPI  R18,1
	BRNE _0x7
;     146       {
;     147       printf("%c",enter);
	__POINTW1FN _0,0
	RCALL SUBOPT_0x0
	LDI  R24,4
	RCALL SUBOPT_0x1
;     148       printf("Key Pressed Is:%c",TelPad_LUT[row][col]);
	__POINTW1FN _0,3
	ST   -Y,R31
	ST   -Y,R30
	LDI  R26,LOW(_TelPad_LUT*2)
	LDI  R27,HIGH(_TelPad_LUT*2)
	MOV  R30,R16
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(3)
	LDI  R27,HIGH(3)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R26,R30
	ADC  R27,R31
	MOV  R30,R17
	RCALL SUBOPT_0x2
	LPM  R30,Z
	RCALL SUBOPT_0x3
	LDI  R24,4
	RCALL SUBOPT_0x1
;     149       }
;     150       else
	RJMP _0x8
_0x7:
;     151       {
;     152       printf("%c",enter);
	__POINTW1FN _0,0
	RCALL SUBOPT_0x0
	LDI  R24,4
	RCALL SUBOPT_0x1
;     153       printf("No key is Pressed");
	__POINTW1FN _0,21
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL _printf
	ADIW R28,2
;     154       }
_0x8:
;     155       
;     156 
;     157       };
	RJMP _0x4
;     158 }
_0x9:
	NOP
	RJMP _0x9
;     159 
;     160 char getkeycode(char *row,char *col)
;     161 {       
_getkeycode:
;     162         char i=FALSE; 
;     163         while(*row != 4)  
	ST   -Y,R16
;	*row -> Y+3
;	*col -> Y+1
;	i -> R16
	LDI  R16,0
_0xA:
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LD   R26,X
	CPI  R26,LOW(0x4)
	BREQ _0xC
;     164         {       
;     165                 for (*col=0;*col<3;(*col)++)
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	RCALL SUBOPT_0x4
_0xE:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R26,X
	CPI  R26,LOW(0x3)
	BRSH _0xF
;     166                 {
;     167                         printf("%c",enter);
	__POINTW1FN _0,0
	RCALL SUBOPT_0x0
	LDI  R24,4
	RCALL SUBOPT_0x1
;     168                         printf("Is the key[%d][%d] pressed?(Y/N)",*row,*col);
	__POINTW1FN _0,39
	ST   -Y,R31
	ST   -Y,R30
	LDD  R26,Y+5
	LDD  R27,Y+5+1
	RCALL SUBOPT_0x5
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	RCALL SUBOPT_0x5
	LDI  R24,8
	RCALL _printf
	ADIW R28,10
;     169                         scanf("%c",&i); 
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	IN   R30,SPL
	IN   R31,SPH
	RCALL __PUTPARD1L
	PUSH R16
	LDI  R24,4
	RCALL _scanf
	ADIW R28,6
	POP  R16
;     170                         if (i=='Y' || i=='y')
	CPI  R16,89
	BREQ _0x11
	CPI  R16,121
	BRNE _0x10
_0x11:
;     171                         { 
;     172                                 i=TRUE;
	LDI  R16,LOW(1)
;     173                                 return i;
	MOV  R30,R16
	RJMP _0xB7
;     174                         }
;     175                         else
_0x10:
;     176                         {   
;     177                                 i=FALSE;
	LDI  R16,LOW(0)
;     178                         }
;     179                  }
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	RCALL SUBOPT_0x6
	ST   X,R30
	RJMP _0xE
_0xF:
;     180                  *row=(*row)+1;   
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	RCALL SUBOPT_0x6
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	ST   X,R30
;     181         }
	RJMP _0xA
_0xC:
;     182         return i;
	MOV  R30,R16
_0xB7:
	LDD  R16,Y+0
	ADIW R28,5
	RET
;     183         
;     184 }
;     185 

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
	RCALL SUBOPT_0x7
	SBIW R30,0
	BREQ _0x14
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0x8
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x15
_0x14:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x15:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0x16:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	RCALL SUBOPT_0x9
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x18
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x1C
	CPI  R19,37
	BRNE _0x1D
	LDI  R16,LOW(1)
	RJMP _0x1E
_0x1D:
	RCALL SUBOPT_0xA
_0x1E:
	RJMP _0x1B
_0x1C:
	CPI  R30,LOW(0x1)
	BRNE _0x1F
	CPI  R19,37
	BRNE _0x20
	RCALL SUBOPT_0xA
	LDI  R16,LOW(0)
	RJMP _0x1B
_0x20:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x21
	LDI  R17,LOW(1)
	RJMP _0x1B
_0x21:
	CPI  R19,43
	BRNE _0x22
	LDI  R21,LOW(43)
	RJMP _0x1B
_0x22:
	CPI  R19,32
	BRNE _0x23
	LDI  R21,LOW(32)
	RJMP _0x1B
_0x23:
	RJMP _0x24
_0x1F:
	CPI  R30,LOW(0x2)
	BRNE _0x25
_0x24:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x26
	ORI  R17,LOW(128)
	RJMP _0x1B
_0x26:
	RJMP _0x27
_0x25:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x1B
_0x27:
	CPI  R19,48
	BRLO _0x2A
	CPI  R19,58
	BRLO _0x2B
_0x2A:
	RJMP _0x29
_0x2B:
	RCALL SUBOPT_0xB
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x1B
_0x29:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x2F
	RCALL SUBOPT_0xC
	LD   R30,X
	RCALL SUBOPT_0xD
	RJMP _0x30
_0x2F:
	CPI  R30,LOW(0x73)
	BRNE _0x32
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xE
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x33
_0x32:
	CPI  R30,LOW(0x70)
	BRNE _0x35
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xE
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x33:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x36
_0x35:
	CPI  R30,LOW(0x64)
	BREQ _0x39
	CPI  R30,LOW(0x69)
	BRNE _0x3A
_0x39:
	ORI  R17,LOW(4)
	RJMP _0x3B
_0x3A:
	CPI  R30,LOW(0x75)
	BRNE _0x3C
_0x3B:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x3D
_0x3C:
	CPI  R30,LOW(0x58)
	BRNE _0x3F
	ORI  R17,LOW(8)
	RJMP _0x40
_0x3F:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x6E
_0x40:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x3D:
	SBRS R17,2
	RJMP _0x42
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xF
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x43
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x43:
	CPI  R21,0
	BREQ _0x44
	SUBI R16,-LOW(1)
	RJMP _0x45
_0x44:
	ANDI R17,LOW(251)
_0x45:
	RJMP _0x46
_0x42:
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xF
_0x46:
_0x36:
	SBRC R17,0
	RJMP _0x47
_0x48:
	CP   R16,R20
	BRSH _0x4A
	SBRS R17,7
	RJMP _0x4B
	SBRS R17,2
	RJMP _0x4C
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x4D
_0x4C:
	LDI  R19,LOW(48)
_0x4D:
	RJMP _0x4E
_0x4B:
	LDI  R19,LOW(32)
_0x4E:
	RCALL SUBOPT_0xA
	SUBI R20,LOW(1)
	RJMP _0x48
_0x4A:
_0x47:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x4F
_0x50:
	CPI  R18,0
	BREQ _0x52
	SBRS R17,3
	RJMP _0x53
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	RCALL SUBOPT_0x9
	RJMP _0xB8
_0x53:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xB8:
	ST   -Y,R30
	RCALL SUBOPT_0x10
	CPI  R20,0
	BREQ _0x55
	SUBI R20,LOW(1)
_0x55:
	SUBI R18,LOW(1)
	RJMP _0x50
_0x52:
	RJMP _0x56
_0x4F:
_0x58:
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
	BRSH _0x5A
	SBRS R17,3
	RJMP _0x5B
	SUBI R19,-LOW(7)
	RJMP _0x5C
_0x5B:
	SUBI R19,-LOW(39)
_0x5C:
_0x5A:
	SBRC R17,4
	RJMP _0x5E
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x60
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x5F
_0x60:
	ORI  R17,LOW(16)
	RJMP _0x62
_0x5F:
	CP   R20,R18
	BRLO _0x64
	SBRS R17,0
	RJMP _0x65
_0x64:
	RJMP _0x63
_0x65:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x66
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x62:
	SBRS R17,2
	RJMP _0x67
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0x10
	CPI  R20,0
	BREQ _0x68
	SUBI R20,LOW(1)
_0x68:
_0x67:
_0x66:
_0x5E:
	RCALL SUBOPT_0xA
	CPI  R20,0
	BREQ _0x69
	SUBI R20,LOW(1)
_0x69:
_0x63:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x59
	RJMP _0x58
_0x59:
_0x56:
	SBRS R17,0
	RJMP _0x6A
_0x6B:
	CPI  R20,0
	BREQ _0x6D
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0xD
	RJMP _0x6B
_0x6D:
_0x6A:
_0x6E:
_0x30:
	LDI  R16,LOW(0)
_0x1B:
	RJMP _0x16
_0x18:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x11
	RCALL __print_G2
	RJMP _0xB5
__get_G2:
	ST   -Y,R16
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LD   R30,X
	RCALL SUBOPT_0x12
	BREQ _0x6F
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	RCALL SUBOPT_0x4
	RJMP _0x70
_0x6F:
	RCALL SUBOPT_0x13
	SBIW R30,0
	BREQ _0x71
	RCALL SUBOPT_0x13
	LD   R30,Z
	RCALL SUBOPT_0x12
	BREQ _0x72
	RCALL SUBOPT_0x13
	RCALL SUBOPT_0x8
_0x72:
	RJMP _0x73
_0x71:
	RCALL _getchar
	MOV  R16,R30
_0x73:
_0x70:
	MOV  R30,R16
	LDD  R16,Y+0
	ADIW R28,5
	RET
__scanf_G2:
	SBIW R28,4
	RCALL __SAVELOCR6
	LDI  R30,LOW(0)
	STD  Y+9,R30
	MOV  R21,R30
_0x74:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	ADIW R30,1
	STD  Y+14,R30
	STD  Y+14+1,R31
	RCALL SUBOPT_0x9
	RCALL SUBOPT_0x14
	BRNE PC+2
	RJMP _0x76
	RCALL SUBOPT_0x15
	BREQ _0x77
_0x78:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	RCALL SUBOPT_0x14
	BREQ _0x7B
	RCALL SUBOPT_0x15
	BRNE _0x7C
_0x7B:
	RJMP _0x7A
_0x7C:
	RJMP _0x78
_0x7A:
	MOV  R21,R18
	RJMP _0x7D
_0x77:
	CPI  R18,37
	BREQ PC+2
	RJMP _0x7E
	LDI  R20,LOW(0)
_0x7F:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	LPM  R18,Z+
	STD  Y+14,R30
	STD  Y+14+1,R31
	CPI  R18,48
	BRLO _0x83
	LDI  R30,LOW(57)
	CP   R30,R18
	BRSH _0x82
_0x83:
	RJMP _0x81
_0x82:
	RCALL SUBOPT_0xB
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x7F
_0x81:
	CPI  R18,0
	BRNE _0x85
	RJMP _0x76
_0x85:
_0x86:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	MOV  R19,R30
	ST   -Y,R30
	RCALL _isspace
	CPI  R30,0
	BRNE _0x86
	CPI  R19,0
	BRNE _0x89
	RJMP _0x8A
_0x89:
	MOV  R21,R19
	CPI  R20,0
	BRNE _0x8B
	LDI  R20,LOW(255)
_0x8B:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0x8F
	RCALL SUBOPT_0x17
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	__GETW2R 16,17
	ST   X,R30
	RJMP _0x8E
_0x8F:
	CPI  R30,LOW(0x73)
	BRNE _0x97
	RCALL SUBOPT_0x17
_0x91:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0x93
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	RCALL SUBOPT_0x14
	BREQ _0x95
	RCALL SUBOPT_0x15
	BREQ _0x94
_0x95:
	RJMP _0x93
_0x94:
	PUSH R17
	PUSH R16
	__ADDWRN 16,17,1
	MOV  R30,R18
	POP  R26
	POP  R27
	ST   X,R30
	RJMP _0x91
_0x93:
	__GETW2R 16,17
	RCALL SUBOPT_0x4
	RJMP _0x8E
_0x97:
	RCALL SUBOPT_0x18
	MOV  R30,R18
	CPI  R30,LOW(0x64)
	BREQ _0x9C
	CPI  R30,LOW(0x69)
	BRNE _0x9D
_0x9C:
	LDI  R30,LOW(0)
	STD  Y+8,R30
	RJMP _0x9E
_0x9D:
	CPI  R30,LOW(0x75)
	BRNE _0x9F
_0x9E:
	LDI  R19,LOW(10)
	RJMP _0x9A
_0x9F:
	CPI  R30,LOW(0x78)
	BRNE _0xA0
	LDI  R19,LOW(16)
	RJMP _0x9A
_0xA0:
	CPI  R30,LOW(0x25)
	BRNE _0xA3
	RJMP _0xA2
_0xA3:
	LDD  R30,Y+9
	RJMP _0xB6
_0x9A:
	LDI  R30,0
	STD  Y+6,R30
	STD  Y+6+1,R30
_0xA4:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0xA6
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	MOV  R18,R30
	MOV  R26,R30
	LDI  R30,LOW(32)
	CP   R30,R26
	BRSH _0xA8
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0xA9
	CPI  R18,45
	BRNE _0xAA
	LDI  R30,LOW(255)
	STD  Y+8,R30
	RJMP _0xA4
_0xAA:
	RCALL SUBOPT_0x18
_0xA9:
	CPI  R18,48
	BRLO _0xA8
	CPI  R18,97
	BRLO _0xAD
	SUBI R18,LOW(87)
	RJMP _0xAE
_0xAD:
	CPI  R18,65
	BRLO _0xAF
	SUBI R18,LOW(55)
	RJMP _0xB0
_0xAF:
	SUBI R18,LOW(48)
_0xB0:
_0xAE:
	CP   R18,R19
	BRLO _0xB1
_0xA8:
	MOV  R21,R18
	RJMP _0xA6
_0xB1:
	MOV  R30,R19
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R31,0
	RCALL __MULW12U
	MOVW R26,R30
	MOV  R30,R18
	RCALL SUBOPT_0x2
	STD  Y+6,R30
	STD  Y+6+1,R31
	RJMP _0xA4
_0xA6:
	RCALL SUBOPT_0x17
	LDD  R30,Y+8
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R31,0
	SBRC R30,7
	SER  R31
	RCALL __MULW12U
	__GETW2R 16,17
	ST   X+,R30
	ST   X,R31
_0x8E:
	LDD  R30,Y+9
	SUBI R30,-LOW(1)
	STD  Y+9,R30
	RJMP _0xB2
_0x7E:
_0xA2:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x16
	POP  R21
	CP   R30,R18
	BREQ _0xB3
_0x8A:
	LDD  R30,Y+9
	CPI  R30,0
	BRNE _0xB4
	LDI  R30,LOW(255)
	RJMP _0xB6
_0xB4:
	RJMP _0x76
_0xB3:
_0xB2:
_0x7D:
	RJMP _0x74
_0x76:
	LDD  R30,Y+9
_0xB6:
	RCALL __LOADLOCR6
	ADIW R28,16
	RET
_scanf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x11
	RCALL __scanf_G2
_0xB5:
	RCALL __LOADLOCR2
	ADIW R28,4
	POP  R15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x0:
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R4
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x1:
	RCALL _printf
	ADIW R28,6
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	LDI  R31,0
	ADD  R30,R26
	ADC  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3:
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x4:
	LDI  R30,LOW(0)
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	LD   R30,X
	RJMP SUBOPT_0x3

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	LD   R30,X
	SUBI R30,-LOW(1)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x9:
	SBIW R30,1
	LPM  R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xA:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0xC:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	SBIW R28,2
	RCALL __SAVELOCR2
	MOVW R26,R28
	RCALL __ADDW2R15
	__PUTW2R 16,17
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
	MOVW R26,R28
	ADIW R26,4
	RCALL __ADDW2R15
	RCALL __GETW1P
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	MOV  R16,R30
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x13:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x14:
	MOV  R18,R30
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x15:
	ST   -Y,R18
	RCALL _isspace
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x16:
	LDD  R30,Y+12
	LDD  R31,Y+12+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __get_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x17:
	LDD  R26,Y+12
	LDD  R27,Y+12+1
	SBIW R26,4
	STD  Y+12,R26
	STD  Y+12+1,R27
	ADIW R26,4
	LD   R16,X+
	LD   R17,X
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x18:
	LDI  R30,LOW(1)
	STD  Y+8,R30
	RET

_isspace:
	ldi  r30,1
	ld   r31,y+
	cpi  r31,' '
	breq __isspace1
	cpi  r31,9
	brlo __isspace0
	cpi  r31,14
	brlo __isspace1
__isspace0:
	clr  r30
__isspace1:
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

__MULW12U:
	MUL  R31,R26
	MOV  R31,R0
	MUL  R30,R27
	ADD  R31,R0
	MUL  R30,R26
	MOV  R30,R0
	ADD  R31,R1
	RET

__GETW1P:
	LD   R30,X+
	LD   R31,X
	SBIW R26,1
	RET

__GETW1PF:
	LPM  R0,Z+
	LPM  R31,Z
	MOV  R30,R0
	RET

__PUTPARD1L:
	LDI  R22,0
	LDI  R23,0
__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
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

;END OF CODE MARKER
__END_OF_CODE:
