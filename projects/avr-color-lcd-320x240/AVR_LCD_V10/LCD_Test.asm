;CodeVisionAVR C Compiler V1.24.6 Standard
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega162
;Program type           : Application
;Clock frequency        : 16,000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 200 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Automatic register allocation : On

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
	CALL __GETW1PF
	ICALL
	.ENDM


	.MACRO __CALL2EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	CALL __EEPROMRDW
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
	CALL __PUTDP1
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

	.INCLUDE "lcd_test.vec"
	.INCLUDE "lcd_test.inc"

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
	LDI  R24,LOW(0x400)
	LDI  R25,HIGH(0x400)
	LDI  R26,LOW(0x100)
	LDI  R27,HIGH(0x100)
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
	LDI  R30,LOW(0x4FF)
	OUT  SPL,R30
	LDI  R30,HIGH(0x4FF)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x1C8)
	LDI  R29,HIGH(0x1C8)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x1C8
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.6 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 e-mail:office@hpinfotech.com
;       8 
;       9 Project : LCD Test
;      10 Version : 
;      11 Date    : 22-2-2007
;      12 Author  : Patrick Loef                    
;      13 Company : -                               
;      14 Comments: 
;      15 ------------------------------------------------------
;      16 Version: 
;      17         0,1: First release 
;      18                 - Clear and Fill LCD
;      19                 - 16 colors
;      20         
;      21         0,2: Second release
;      22                 - Font 8x8
;      23                 - BMP picture working
;      24                 
;      25         0,3: Release
;      26                 - 256 colors text possible
;      27                 - separate LUT source in file
;      28                 - possible to define 16 or 256 colors   
;      29         0,4: Release
;      30                 - 256 color bmp picture possible
;      31                 - BMP LUT table compatible, no need to manual switch the RED and BLUE value
;      32                 - Debug information in ENGLISH
;      33                                                       
;      34         0,5: Release
;      35                 - Compact flash driver added
;      36                 - CF info present on LCD(When choosed in Terminal menu)
;      37                 - Application tool avaible to send BMP to CF
;      38                 - Draw picture from CF data to S1D13705
;      39                 - Terminal menu present for settings
;      40                 - Link menu present(When choosded in Terminal menu)
;      41                 - Baudrate increased to 57600, this the max of 16Mhz      
;      42                 - Make a define option in S1D13705.h for more info of the S1D13705
;      43         0,6: Release           
;      44                 - Mod: Increased the receiving speed a lot! This done with receive 512 bytes and then send the ready sign
;      45                 - Mod: Changed Blok to Page  
;      46                 - Mod: Changed calculation of the sector of the CF info (152 * Page) + 1, default of Page always 0
;      47                 - Mod: Changed Show_BMP '1 to 32' to '0 to 32' and adjusted function for that
;      48                 - Bug: Only Page 1 was written, error at the check function in RX_BMP_Data:
;      49                        the check was uncorrect, The /else Page = 1/ removed and set as default value
;      50                        also the i-1 function in CF_Page[i-1] changed to CF_Page[i]  
;      51         1.0: Release
;      52                 - Mod:  Function CF_Info disabled for memory programm space
;      53                 - Mod:  Added touchscreen driver
;      54                 - Mod:  Added touchscreen calibration
;      55                 - Mod:  Added vertical and horizontal line drawing S1D13705
;      56                 - Mod:  Added LTC1298(ADC) driver for touchscreen control
;      57                 - Mod:  When pressed on the touchscreen a cross is drawed
;      58                 - Mod:  Added storing calibration coeffecients and touchscreen flag register in eeprom
;      59                 
;      60 ------------------------------------------------------
;      61 Chip type           : ATmega162
;      62 Program type        : Application
;      63 Clock frequency     : 16,000000 MHz
;      64 Memory model        : Small
;      65 External SRAM size  : 0
;      66 Data Stack size     : 256
;      67 *****************************************************/
;      68 
;      69 #include <mega162.h>
;      70 #include <stdio.h> 
;      71 #include <delay.h> 
;      72 #include <string.h>
;      73 #include <spi.h>
;      74 
;      75 //Include niet standaard bibliotheek file's
;      76 #include "LCD_Test_Defs.h"
;      77 #include "S1D13705.h"
_aS1DRegs_G1:
_0aS1DRegs_G1:
	.BYTE 0x1
_1aS1DRegs_G1:
	.BYTE 0x1
_2aS1DRegs_G1:
	.BYTE 0x1
_3aS1DRegs_G1:
	.BYTE 0x1
_4aS1DRegs_G1:
	.BYTE 0x1
_5aS1DRegs_G1:
	.BYTE 0x1
_6aS1DRegs_G1:
	.BYTE 0x1
_7aS1DRegs_G1:
	.BYTE 0x1
_8aS1DRegs_G1:
	.BYTE 0x1
_9aS1DRegs_G1:
	.BYTE 0x1
_AaS1DRegs_G1:
	.BYTE 0x1
_BaS1DRegs_G1:
	.BYTE 0x1
_CaS1DRegs_G1:
	.BYTE 0x1
_DaS1DRegs_G1:
	.BYTE 0x1
_EaS1DRegs_G1:
	.BYTE 0x1
_FaS1DRegs_G1:
	.BYTE 0x1
_10aS1DRegs_G1:
	.BYTE 0x1
_11aS1DRegs_G1:
	.BYTE 0x1
_12aS1DRegs_G1:
	.BYTE 0x1
_13aS1DRegs_G1:
	.BYTE 0x1
_14aS1DRegs_G1:
	.BYTE 0x1
_15aS1DRegs_G1:
	.BYTE 0x1
_16aS1DRegs_G1:
	.BYTE 0x1
_17aS1DRegs_G1:
	.BYTE 0x1
_18aS1DRegs_G1:
	.BYTE 0x1
_19aS1DRegs_G1:
	.BYTE 0x1
_1AaS1DRegs_G1:
	.BYTE 0x1
_1BaS1DRegs_G1:
	.BYTE 0x1
_1CaS1DRegs_G1:
	.BYTE 0x1
_1DaS1DRegs_G1:
	.BYTE 0x1
_1EaS1DRegs_G1:
	.BYTE 0x1
_1FaS1DRegs_G1:
	.BYTE 0x1
_20aS1DRegs_G1:
	.BYTE 0x1
_21aS1DRegs_G1:
	.BYTE 0x1
_22aS1DRegs_G1:
	.BYTE 0x1
_23aS1DRegs_G1:
	.BYTE 0x1
_24aS1DRegs_G1:
	.BYTE 0x1
_25aS1DRegs_G1:
	.BYTE 0x1
_26aS1DRegs_G1:
	.BYTE 0x1
_27aS1DRegs_G1:
	.BYTE 0x1
_28aS1DRegs_G1:
	.BYTE 0x1
_29aS1DRegs_G1:
	.BYTE 0x1
_2AaS1DRegs_G1:
	.BYTE 0x1
_2BaS1DRegs_G1:
	.BYTE 0x1
_2CaS1DRegs_G1:
	.BYTE 0x1
_2DaS1DRegs_G1:
	.BYTE 0x1
_2EaS1DRegs_G1:
	.BYTE 0x1
_2FaS1DRegs_G1:
	.BYTE 0x1
_30aS1DRegs_G1:
	.BYTE 0x1
_31aS1DRegs_G1:
	.BYTE 0x1
;      78 #include "S1D13705_LUT.h"       
;      79 #include "Font8x8.h" 

	.CSEG
;      80 #include "cf_driver.h"  
;      81 #include "cf_functions.h"                     
;      82 #include "RX_BMP_Data.h"
;      83 #include "Touchscreen.h"
;      84 #include "ADC_LTC1298.h"
;      85 
;      86 //Global variabelen      
;      87 unsigned char CF_Sector_Data[512]; 

	.DSEG
_CF_Sector_Data:
	.BYTE 0x200
;      88 unsigned char string[20];   
_string:
	.BYTE 0x14
;      89 unsigned char CF_Page[32];        
_CF_Page:
	.BYTE 0x20
;      90 
;      91 //Global variabelen structere
;      92 typedef struct
;      93 {
;      94         unsigned char Flag_Register;
;      95         unsigned int X_pos;
;      96         unsigned int Y_pos;
;      97 }TouchData;
;      98 TouchData Touchscreen;
_Touchscreen:
	.BYTE 0x5
;      99 
;     100 //Eeprom variabelen
;     101 eeprom unsigned char ee_Touch_Flag = 0;                 //Eeprom touchscreen flag register

	.ESEG
_ee_Touch_Flag:
	.DB  0x0
;     102 eeprom float ee_float1 = 0.0;           
_ee_float1:
	.DW  0x0
	.DW  0x0
;     103 eeprom float ee_float2 = 0.0;
_ee_float2:
	.DW  0x0
	.DW  0x0
;     104 eeprom float ee_float3 = 0.0;
_ee_float3:
	.DW  0x0
	.DW  0x0
;     105 eeprom float ee_float4 = 0.0;
_ee_float4:
	.DW  0x0
	.DW  0x0
;     106 eeprom float ee_float5 = 0.0;
_ee_float5:
	.DW  0x0
	.DW  0x0
;     107 eeprom float ee_float6 = 0.0;
_ee_float6:
	.DW  0x0
	.DW  0x0
;     108 
;     109 
;     110 
;     111 /**************************************
;     112 Functie: Laad geslecteerde plaatje zien
;     113 **************************************/
;     114 void Show_BMP(void)
;     115 {

	.CSEG
_Show_BMP:
;     116         int Page_Number;
;     117         
;     118         while(1)
	ST   -Y,R17
	ST   -Y,R16
;	Page_Number -> R16,R17
_0x28:
;     119         {
;     120                 printf("\n\rGive Page number(0 to 31)");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     121                 scanf("%d",&Page_Number);
	__POINTW1FN _0,28
	ST   -Y,R31
	ST   -Y,R30
	IN   R30,SPL
	IN   R31,SPH
	CALL __PUTPARD1LDEC
	PUSH R17
	PUSH R16
	LDI  R24,4
	CALL _scanf
	ADIW R28,6
	POP  R16
	POP  R17
;     122         
;     123                 if(Page_Number<0 | Page_Number>31)
	__GETW2R 16,17
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	CALL __LTW12
	PUSH R30
	LDI  R30,LOW(31)
	LDI  R31,HIGH(31)
	CALL __GTW12
	POP  R26
	OR   R30,R26
	BREQ _0x2B
;     124                         printf("\n\rNot correct between 0 and 31");
	__POINTW1FN _0,31
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     125                 else
	RJMP _0x2C
_0x2B:
;     126                         break;
	RJMP _0x2A
;     127         }
_0x2C:
	RJMP _0x28
_0x2A:
;     128         
;     129         //Check of blok data aanwezig is
;     130         if(CF_Page[Page_Number] != Page_Number)
	LDI  R26,LOW(_CF_Page)
	LDI  R27,HIGH(_CF_Page)
	ADD  R26,R16
	ADC  R27,R17
	LD   R26,X
	__GETW1R 16,17
	LDI  R27,0
	CP   R30,R26
	CPC  R31,R27
	BREQ _0x2D
;     131         {
;     132                 printf("\n\rPage %d is not present, return",Page_Number);
	__POINTW1FN _0,62
	CALL SUBOPT_0x0
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;     133                 return;
	RJMP _0x249
;     134         }
;     135         
;     136         //Laat plaatje zien
;     137         printf("\n\rShow BMP picture: %d",Page_Number);
_0x2D:
	__POINTW1FN _0,95
	CALL SUBOPT_0x0
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;     138         
;     139         CF_Draw_Pic(0,0,Page_Number);
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	ST   -Y,R31
	ST   -Y,R30
	CALL SUBOPT_0x0
	RCALL _CF_Draw_Pic
;     140         
;     141         return;
_0x249:
	LD   R16,Y+
	LD   R17,Y+
	RET
;     142 }
;     143 //*************************************
;     144 
;     145 /************************************** 
;     146 External Interrupt 0 service routine  
;     147 **************************************/
;     148 interrupt [EXT_INT0] void ext_int0_isr(void)
;     149 {
_ext_int0_isr:
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
;     150         unsigned long int i;
;     151         unsigned int c;
;     152         
;     153         //Interupt uit
;     154         GICR &= ~(1<<INT0);                             
	CALL SUBOPT_0x1
;	i -> Y+2
;	c -> R16,R17
	IN   R30,0x3B
	ANDI R30,0xBF
	OUT  0x3B,R30
;     155         
;     156         if((Touchscreen.Flag_Register & (1<<TouchCalProcess)) == (1<<TouchCalProcess))                          //Als calibratie bezig is dan terug Flag zetten en terug gaan
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x4)
	CPI  R30,LOW(0x4)
	BRNE _0x2E
;     157         {
;     158                 Touchscreen.Flag_Register |= (1<<TouchPressed);                 //Zet TouchPress flag
	CALL SUBOPT_0x2
;     159                 printf("\n\rTouchPressed");
	__POINTW1FN _0,118
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     160         }
;     161         else
	RJMP _0x2F
_0x2E:
;     162         {
;     163                 Touchscreen.Flag_Register |= (1<<TouchPressed);                 //Zet TouchPress flag
	CALL SUBOPT_0x2
;     164                 
;     165                 Touchscreen_XY_positie();                                       //Meet XY postie
	CALL _Touchscreen_XY_positie
;     166                 printf("\n\rTouchPressed");
	__POINTW1FN _0,118
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     167 
;     168                 GIFR |= (1<<INT0);                                              //Erase bit so that no interupt is generated 
	CALL SUBOPT_0x3
;     169                 GICR |= (1<<INT0);                                              //Enable the INT0 request
;     170         }
_0x2F:
;     171 
;     172         if((Touchscreen.Flag_Register & (1<<XY_Value)) == (1<<XY_Value))
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x8)
	CPI  R30,LOW(0x8)
	BRNE _0x30
;     173         {
;     174                // printf("\n\rX: %d, Y: %d",Touchscreen.X_pos,Touchscreen.Y_pos);
;     175                 Touchscreen.Flag_Register &= ~(XY_Value);
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0xFC)
	STS  _Touchscreen,R30
;     176                 TouchscreenTest(Touchscreen.X_pos,Touchscreen.Y_pos);
	__GETW1MN _Touchscreen,1
	ST   -Y,R31
	ST   -Y,R30
	__GETW1MN _Touchscreen,3
	ST   -Y,R31
	ST   -Y,R30
	RCALL _TouchscreenTest
;     177         }
;     178                 
;     179 }
_0x30:
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,6
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
;     180 //*************************************
;     181   
;     182 void TouchscreenTest(unsigned int X, unsigned int Y)
;     183 {
_TouchscreenTest:
;     184         static unsigned int Xbak;

	.DSEG

	.CSEG
;     185         static unsigned int Ybak;  

	.DSEG

	.CSEG
;     186         
;     187         S1D13705_Draw_Horizontal_Line(X-3,X+3,Y,0);                             //Instellen als X(begin), X1(eind), Y, kleur
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	CALL SUBOPT_0x4
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	CALL SUBOPT_0x5
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	CLR  R22
	CLR  R23
	CALL SUBOPT_0x6
;     188         S1D13705_Draw_Vertical_Line(X,Y-3,Y+3,0);                               //Instellen als X, Y(begin), Y1(eind), kleur     
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDD  R30,Y+4
	LDD  R31,Y+4+1
	CALL SUBOPT_0x4
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	CALL SUBOPT_0x5
	CALL SUBOPT_0x7
;     189         
;     190         S1D13705_Draw_Horizontal_Line(Xbak-3,Xbak+3,Ybak,15);                   //Instellen als X(begin), X1(eind), Y, kleur
	__GETW1R 4,5
	CALL SUBOPT_0x4
	__GETW1R 4,5
	CALL SUBOPT_0x5
	__GETW1R 6,7
	CLR  R22
	CLR  R23
	CALL SUBOPT_0x8
	RCALL _S1D13705_Draw_Horizontal_Line
;     191         S1D13705_Draw_Vertical_Line(Xbak,Ybak-3,Ybak+3,15);                     //Instellen als X, Y(begin), Y1(eind), kleur      
	__GETW1R 4,5
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETW1R 6,7
	CALL SUBOPT_0x4
	__GETW1R 6,7
	CALL SUBOPT_0x5
	LDI  R30,LOW(15)
	ST   -Y,R30
	RCALL _S1D13705_Draw_Vertical_Line
;     192         
;     193         Xbak = X;
	__GETWRS 4,5,2
;     194         Ybak = Y;
	__GETWRS 6,7,0
;     195         return;
	ADIW R28,4
	RET
;     196 }
;     197 
;     198 /**************************************
;     199 Functie: MAIN
;     200 **************************************/
;     201 void main(void)
;     202 {
_main:
;     203 // Declare your local variables here
;     204 
;     205 #ifdef Info_S1D13705
;     206 unsigned char r;
;     207 unsigned char g;
;     208 unsigned char b;
;     209 #endif
;     210 
;     211 unsigned char Blok_Data;
;     212 unsigned char i;  
;     213 unsigned int T_waarde;  
;     214 
;     215 
;     216 // Crystal Oscillator division factor: 1
;     217 #pragma optsize-
;     218 CLKPR=0x80;
;	Blok_Data -> R16
;	i -> R17
;	T_waarde -> R18,R19
	LDI  R30,LOW(128)
	STS  0x61,R30
;     219 CLKPR=0x00;
	LDI  R30,LOW(0)
	STS  0x61,R30
;     220 #ifdef _OPTIMIZE_SIZE_
;     221 #pragma optsize+
;     222 #endif
;     223 
;     224 // Input/Output Ports initialization
;     225 // Port A initialization
;     226 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     227 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     228 PORTA=0x00;
	OUT  0x1B,R30
;     229 DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     230 
;     231 // Port B initialization
;     232 // Func7=In Func6=In Func5=In Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     233 // State7=T State6=T State5=T State4=0 State3=0 State2=1 State1=1 State0=1 
;     234 PORTB=0x07;
	LDI  R30,LOW(7)
	OUT  0x18,R30
;     235 DDRB=0xBF;
	LDI  R30,LOW(191)
	OUT  0x17,R30
;     236 
;     237 // Port C initialization
;     238 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     239 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     240 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;     241 DDRC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x14,R30
;     242 
;     243 // Port D initialization
;     244 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=In Func1=In Func0=In 
;     245 // State7=1 State6=1 State5=1 State4=1 State3=1 State2=T State1=T State0=T 
;     246 PORTD=0xF0;
	LDI  R30,LOW(240)
	OUT  0x12,R30
;     247 DDRD=0xF8;
	LDI  R30,LOW(248)
	OUT  0x11,R30
;     248 
;     249 // Port E initialization
;     250 // Func2=Out Func1=Out Func0=In 
;     251 // State2=0 State1=0 State0=T 
;     252 PORTE=0x00;
	LDI  R30,LOW(0)
	OUT  0x7,R30
;     253 DDRE=0x06;
	LDI  R30,LOW(6)
	OUT  0x6,R30
;     254 
;     255 // Timer/Counter 0 initialization
;     256 // Clock source: System Clock
;     257 // Clock value: Timer 0 Stopped
;     258 // Mode: Normal top=FFh
;     259 // OC0 output: Disconnected
;     260 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
;     261 TCNT0=0x00;
	OUT  0x32,R30
;     262 OCR0=0x00;
	OUT  0x31,R30
;     263 
;     264 // Timer/Counter 1 initialization
;     265 // Clock source: System Clock
;     266 // Clock value: Timer 1 Stopped
;     267 // Mode: Normal top=FFFFh
;     268 // OC1A output: Discon.
;     269 // OC1B output: Discon.
;     270 // Noise Canceler: Off
;     271 // Input Capture on Falling Edge
;     272 // Timer 1 Overflow Interrupt: Off
;     273 // Input Capture Interrupt: Off
;     274 // Compare A Match Interrupt: Off
;     275 // Compare B Match Interrupt: Off
;     276 TCCR1A=0x00;
	OUT  0x2F,R30
;     277 TCCR1B=0x00;
	OUT  0x2E,R30
;     278 TCNT1H=0x00;
	OUT  0x2D,R30
;     279 TCNT1L=0x00;
	OUT  0x2C,R30
;     280 ICR1H=0x00;
	OUT  0x25,R30
;     281 ICR1L=0x00;
	OUT  0x24,R30
;     282 OCR1AH=0x00;
	OUT  0x2B,R30
;     283 OCR1AL=0x00;
	OUT  0x2A,R30
;     284 OCR1BH=0x00;
	OUT  0x29,R30
;     285 OCR1BL=0x00;
	OUT  0x28,R30
;     286 
;     287 // Timer/Counter 2 initialization
;     288 // Clock source: System Clock
;     289 // Clock value: Timer 2 Stopped
;     290 // Mode: Normal top=FFh
;     291 // OC2 output: Disconnected
;     292 ASSR=0x00;
	OUT  0x26,R30
;     293 TCCR2=0x00;
	OUT  0x27,R30
;     294 TCNT2=0x00;
	OUT  0x23,R30
;     295 OCR2=0x00;
	OUT  0x22,R30
;     296 
;     297 // Timer/Counter 3 initialization
;     298 // Clock value: Timer 3 Stopped
;     299 // Mode: Normal top=FFFFh
;     300 // Noise Canceler: Off
;     301 // Input Capture on Falling Edge
;     302 // OC3A output: Discon.
;     303 // OC3B output: Discon.
;     304 // Timer 3 Overflow Interrupt: Off
;     305 // Input Capture Interrupt: Off
;     306 // Compare A Match Interrupt: Off
;     307 // Compare B Match Interrupt: Off
;     308 TCCR3A=0x00;
	STS  0x8B,R30
;     309 TCCR3B=0x00;
	STS  0x8A,R30
;     310 TCNT3H=0x00;
	STS  0x89,R30
;     311 TCNT3L=0x00;
	STS  0x88,R30
;     312 ICR3H=0x00;
	STS  0x81,R30
;     313 ICR3L=0x00;
	STS  0x80,R30
;     314 OCR3AH=0x00;
	STS  0x87,R30
;     315 OCR3AL=0x00;
	STS  0x86,R30
;     316 OCR3BH=0x00;
	STS  0x85,R30
;     317 OCR3BL=0x00;
	STS  0x84,R30
;     318 
;     319 // External Interrupt(s) initialization
;     320 // INT0: Off
;     321 // INT0 Mode: Falling Edge
;     322 // INT1: Off
;     323 // INT2: Off
;     324 // Interrupt on any change on pins PCINT0-7: Off
;     325 // Interrupt on any change on pins PCINT8-15: Off
;     326 GICR=0x00;
	OUT  0x3B,R30
;     327 MCUCR=0x02;
	LDI  R30,LOW(2)
	OUT  0x35,R30
;     328 EMCUCR=0x00;
	LDI  R30,LOW(0)
	OUT  0x36,R30
;     329 
;     330 // Timer(s)/Counter(s) Interrupt(s) initialization
;     331 TIMSK=0x00;
	OUT  0x39,R30
;     332 ETIMSK=0x00;
	STS  0x7D,R30
;     333 
;     334 // USART0 initialization
;     335 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     336 // USART0 Receiver: On
;     337 // USART0 Transmitter: On
;     338 // USART0 Mode: Asynchronous
;     339 // USART0 Baud rate: 57600 (Double Speed Mode)
;     340 UCSR0A=0x02;
	LDI  R30,LOW(2)
	OUT  0xB,R30
;     341 UCSR0B=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
;     342 UCSR0C=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     343 UBRR0H=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     344 UBRR0L=0x22;
	LDI  R30,LOW(34)
	OUT  0x9,R30
;     345 
;     346 // SPI initialization
;     347 // SPI Type: Master
;     348 // SPI Clock Rate: 125 kHz
;     349 // SPI Clock Phase: Cycle Start
;     350 // SPI Clock Polarity: High
;     351 // SPI Data Order: MSB First
;     352 SPCR=0x13;
	LDI  R30,LOW(19)
	OUT  0xD,R30
;     353 SPSR=0x00;
	LDI  R30,LOW(0)
	OUT  0xE,R30
;     354 
;     355 // Analog Comparator initialization
;     356 // Analog Comparator: Off
;     357 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     358 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     359 //*************************************
;     360 
;     361 /**************************************
;     362              Main program
;     363 **************************************/
;     364 
;     365         #asm("sei");                                            //Enable the global interupt.
	sei
;     366         
;     367         //Init de compact flash
;     368         CF_Init();
	RCALL _CF_Init
;     369         
;     370         //Init Touchscreen
;     371         Touchscreen_Init();
	RCALL _Touchscreen_Init
;     372         
;     373         
;     374         printf("\n\rHello World!!\n\r");
	__POINTW1FN _0,133
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     375         printf("\n\rS1D13705 test programm V:0.6");
	__POINTW1FN _0,151
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     376         printf("\n\rS1D13705 Reset and Init");
	__POINTW1FN _0,182
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     377         
;     378         //Reset en init de S1D13705
;     379         S1D13705_Reset();
	RCALL _S1D13705_Reset
;     380         S1D13705_Init();
	RCALL _S1D13705_Init
;     381         
;     382         printf("\n\rS1D13705 ID: %x", i = S1D13705_Read_Register(0));
	__POINTW1FN _0,208
	CALL SUBOPT_0x9
	RCALL _S1D13705_Read_Register
	MOV  R17,R30
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;     383         
;     384         #ifdef Info_S1D13705  
;     385         for(i=1;i<29;i++)
;     386                 printf("\n\rRegister: %x, value: %x",i, data = S1D13705_Read_Register(i));
;     387         
;     388         //Lees LUT tabel
;     389         S1D13705_Write_Register(0x15,0x00);
;     390         for(i=0;i<16;i++)
;     391         {
;     392                 S1D13705_Write_Register(0x15,i);                //Selecteer LUT adress register
;     393 
;     394                 r = S1D13705_Read_Register(0x17);               //Lees de rode kleur
;     395                 g = S1D13705_Read_Register(0x17);               //Lees de groene kleur
;     396                 b = S1D13705_Read_Register(0x17);               //Lees de blauwe kleur
;     397                 printf("\n\rRed: %x, Green: %x, Blue: %x",r,g,b);                   
;     398         }
;     399         #endif
;     400         
;     401         printf("\n\rS1D13705 init done\n\r");
	__POINTW1FN _0,226
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     402 
;     403 
;     404         //Clear het scherm
;     405         S1D13705_Clear_LCD();
	RCALL _S1D13705_Clear_LCD
;     406         
;     407         //Zet LCD voeding aan
;     408         LCD_PWR_ON;
	SBI  0x12,3
;     409         
;     410         //Check CompactFlash en lees blok informatie uit
;     411         sprintf(string,"Hello World!");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,249
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0xA
;     412         S1D13705_Text_LCD(1,0,string,0);                       //Center tekst = (ScanlineBytes/2) - ((Lengestring*8)/4)   
	__GETD1N 0x0
	CALL SUBOPT_0xB
	CALL SUBOPT_0xC
;     413         
;     414         sprintf(string,"Make use of the terminal for settings");
	__POINTW1FN _0,262
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0xA
;     415         S1D13705_Text_LCD(1,2,string,0);    
	__GETD1N 0x2
	CALL SUBOPT_0xB
	RCALL _S1D13705_Text_LCD
;     416         
;     417         //Check of touchscreen is gekalibreerdt
;     418         if((Touchscreen.Flag_Register & (1<<TouchCalibratie)) == 0)   
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x2)
	BRNE _0x31
;     419         {
;     420                 printf("Touchscreen niet geijkt, ga nu ijken");
	__POINTW1FN _0,300
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0xD
;     421                 Touchscreen_Calibratie();
;     422         }
;     423         
;     424         //Hoofdmenu op de terminal
;     425         while(1)                
_0x31:
_0x32:
;     426         {
;     427                 T_waarde = 0;
	__GETWRN 18,19,0
;     428                 //Check eerst of er blok data aanwezig is
;     429                 Blok_Data = CF_Check_Blok_Data();
	RCALL _CF_Check_Blok_Data
	MOV  R16,R30
;     430                 
;     431                 printf("\n\rMain Menu");
	__POINTW1FN _0,337
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     432                 printf("\n\r1. Link menu");
	__POINTW1FN _0,349
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     433                 printf("\n\r2. Read info CF");
	__POINTW1FN _0,364
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     434                 printf("\n\r3. Delete data on CF");
	__POINTW1FN _0,382
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     435                 printf("\n\r4. Show Picture of selected Blok");
	__POINTW1FN _0,405
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     436                 printf("\n\r5. Touchscreen Calibration\n\r");
	__POINTW1FN _0,440
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     437                 printf("\n\rPage data present: %d",Blok_Data);
	__POINTW1FN _0,471
	CALL SUBOPT_0xE
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;     438                 
;     439                 i = getchar();
	CALL _getchar
	MOV  R17,R30
;     440                 
;     441                 switch(i)
	MOV  R30,R17
;     442                 {
;     443                         case '1':
	CPI  R30,LOW(0x31)
	BRNE _0x38
;     444                                 printf("\n\rGo to link menu");
	__POINTW1FN _0,495
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     445                                 Receive_BMP_Data();
	RCALL _Receive_BMP_Data
;     446                                 break;
	RJMP _0x37
;     447                         case '2':
_0x38:
	CPI  R30,LOW(0x32)
	BREQ _0x37
;     448                               //  printf("\n\rCheck CF card, info on LCD");
;     449                               //  CF_Read_Info();
;     450                                 break;
;     451                         case '3':
	CPI  R30,LOW(0x33)
	BRNE _0x3A
;     452                                 printf("\n\rDelete data on CF wait..");
	__POINTW1FN _0,513
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     453                                 CF_Delete_Data();
	RCALL _CF_Delete_Data
;     454                                 printf("\n\rData on CF deleted");
	__POINTW1FN _0,540
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     455                                 break;
	RJMP _0x37
;     456                         case '4':
_0x3A:
	CPI  R30,LOW(0x34)
	BRNE _0x3B
;     457                                 if(Blok_Data == 0)
	CPI  R16,0
	BRNE _0x3C
;     458                                         printf("\n\rNo data present on CF");
	__POINTW1FN _0,561
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;     459                                 else
	RJMP _0x3D
_0x3C:
;     460                                         Show_BMP();
	CALL _Show_BMP
;     461                                 break;
_0x3D:
	RJMP _0x37
;     462                         case '5':
_0x3B:
	CPI  R30,LOW(0x35)
	BRNE _0x37
;     463                                 printf("\n\rTouchscreen calibration");
	__POINTW1FN _0,585
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0xD
;     464                                 Touchscreen_Calibratie();
;     465                                 break;                        
;     466                 }
_0x37:
;     467         }
	RJMP _0x32
;     468         
;     469                      
;     470         
;     471         
;     472         while (1);
_0x3F:
	RJMP _0x3F
;     473 }
_0x42:
	NOP
	RJMP _0x42
;     474 /**************************************
;     475 S1D13705 Driver
;     476 
;     477 Functie's:
;     478 - Lezen en schrijven via Latch naar S1D13705
;     479 - Lezen register
;     480 - Schrijven register
;     481 - Lezen geheugen
;     482 - Schrijven geheugen                    
;     483 
;     484 Versie:
;     485         0,1: First release 
;     486                 - Clear and Fill LCD
;     487                 - 16 colors
;     488         
;     489         0,2: Second release
;     490                 - Font 8x8
;     491                 - BMP picture working
;     492                 
;     493         0,3: Release
;     494                 - 256 colors text possible
;     495                 - separate LUT source in file
;     496                 - possible to define 16 or 256 colors   
;     497         0,4: Release
;     498                 - 256 color bmp picture possible
;     499                 - BMP LUT table compatible, no need to manual switch the RED and BLUE value
;     500                 - Debug information in ENGLISH
;     501 **************************************/
;     502 
;     503 /*************************************
;     504         Include header file's                   
;     505 *************************************/
;     506 #include "LCD_Test_Defs.h"
;     507 #include "S1D13705.h" 

	.DSEG
_aS1DRegs_G2:
_0aS1DRegs_G2:
	.BYTE 0x1
_1aS1DRegs_G2:
	.BYTE 0x1
_2aS1DRegs_G2:
	.BYTE 0x1
_3aS1DRegs_G2:
	.BYTE 0x1
_4aS1DRegs_G2:
	.BYTE 0x1
_5aS1DRegs_G2:
	.BYTE 0x1
_6aS1DRegs_G2:
	.BYTE 0x1
_7aS1DRegs_G2:
	.BYTE 0x1
_8aS1DRegs_G2:
	.BYTE 0x1
_9aS1DRegs_G2:
	.BYTE 0x1
_AaS1DRegs_G2:
	.BYTE 0x1
_BaS1DRegs_G2:
	.BYTE 0x1
_CaS1DRegs_G2:
	.BYTE 0x1
_DaS1DRegs_G2:
	.BYTE 0x1
_EaS1DRegs_G2:
	.BYTE 0x1
_FaS1DRegs_G2:
	.BYTE 0x1
_10aS1DRegs_G2:
	.BYTE 0x1
_11aS1DRegs_G2:
	.BYTE 0x1
_12aS1DRegs_G2:
	.BYTE 0x1
_13aS1DRegs_G2:
	.BYTE 0x1
_14aS1DRegs_G2:
	.BYTE 0x1
_15aS1DRegs_G2:
	.BYTE 0x1
_16aS1DRegs_G2:
	.BYTE 0x1
_17aS1DRegs_G2:
	.BYTE 0x1
_18aS1DRegs_G2:
	.BYTE 0x1
_19aS1DRegs_G2:
	.BYTE 0x1
_1AaS1DRegs_G2:
	.BYTE 0x1
_1BaS1DRegs_G2:
	.BYTE 0x1
_1CaS1DRegs_G2:
	.BYTE 0x1
_1DaS1DRegs_G2:
	.BYTE 0x1
_1EaS1DRegs_G2:
	.BYTE 0x1
_1FaS1DRegs_G2:
	.BYTE 0x1
_20aS1DRegs_G2:
	.BYTE 0x1
_21aS1DRegs_G2:
	.BYTE 0x1
_22aS1DRegs_G2:
	.BYTE 0x1
_23aS1DRegs_G2:
	.BYTE 0x1
_24aS1DRegs_G2:
	.BYTE 0x1
_25aS1DRegs_G2:
	.BYTE 0x1
_26aS1DRegs_G2:
	.BYTE 0x1
_27aS1DRegs_G2:
	.BYTE 0x1
_28aS1DRegs_G2:
	.BYTE 0x1
_29aS1DRegs_G2:
	.BYTE 0x1
_2AaS1DRegs_G2:
	.BYTE 0x1
_2BaS1DRegs_G2:
	.BYTE 0x1
_2CaS1DRegs_G2:
	.BYTE 0x1
_2DaS1DRegs_G2:
	.BYTE 0x1
_2EaS1DRegs_G2:
	.BYTE 0x1
_2FaS1DRegs_G2:
	.BYTE 0x1
_30aS1DRegs_G2:
	.BYTE 0x1
_31aS1DRegs_G2:
	.BYTE 0x1
;     508 #include "S1D13705_LUT.h"       
;     509 //*************************************
;     510 
;     511 /**************************************
;     512 Functie: Lees data van S1D13705
;     513 **************************************/
;     514 unsigned char S1D13705_Read_Data(unsigned long int adress)
;     515 { 

	.CSEG
_S1D13705_Read_Data:
;     516         unsigned char S1Ddata;
;     517         unsigned int Addr_High;
;     518         unsigned int Addr_Low;
;     519 
;     520         //Zet Data / Adres port als output
;     521         PORT_AD_ctrl = PORT_OUPUT;                                      //Maak port ad output
	CALL __SAVELOCR5
;	adress -> Y+5
;	S1Ddata -> R16
;	Addr_High -> R17,R18
;	Addr_Low -> R19,R20
	CALL SUBOPT_0xF
;     522         
;     523         //Bepaal of A16 1 of 0 is
;     524         if((adress & 0x10000) == 0x10000)                               //Als waar A16 is 1 anders 0
	BRNE _0x68
;     525                 PORT_Addr_A16 = 1;
	SBI  0x7,2
;     526         else
	RJMP _0x69
_0x68:
;     527                 PORT_Addr_A16 = 0;
	CBI  0x7,2
;     528                 
;     529         //Maak hoge en lage adress waardes
;     530         Addr_High = (adress >> 8) & 0xFF;
_0x69:
	CALL SUBOPT_0x10
	__PUTW1R 17,18
;     531         Addr_Low = adress & 0xFF;         
	__GETD1S 5
	__ANDD1N 0xFF
	__PUTW1R 19,20
;     532         
;     533         //Latch de lage waarde naar uitgang van de Latch
;     534         PORT_Addr_Low = Addr_Low;
	__GETW1R 19,20
	OUT  0x1B,R30
;     535         
;     536         ALE_ON;                                                         //Latch de data, data word nu naar uitgang geschoven
	SBI  0x7,1
;     537         ALE_OFF;                                                        //Ale uit
	CBI  0x7,1
;     538         
;     539         //Zet hoge waarde op Port_addr_high
;     540         PORT_Addr_High = Addr_High;
	__GETW1R 17,18
	OUT  0x15,R30
;     541         
;     542         //Zet AD port als input
;     543         PORT_AD_ctrl = PORT_INPUT;
	LDI  R30,LOW(0)
	OUT  0x1A,R30
;     544         
;     545         //Enable S1D13705 en RD on
;     546         S1D13705_CS = CS_ON;
	CBI  0x12,4
;     547         S1D13705_RD = RD_ON;
	CBI  0x12,7
;     548         
;     549         //Lees de data
;     550         while(S1D13705_WAIT);                                           //Wacht totdat WAIT 0 wordt
_0x6A:
	SBIC 0x5,0
	RJMP _0x6A
;     551         while(S1D13705_WAIT == 0);                                      //Wacht totdat WAIT 1 wordt lees dan data in
_0x6D:
	SBIS 0x5,0
	RJMP _0x6D
;     552         S1Ddata = PORT_Data_IN;                                         //Lees de data van de S1D13705
	IN   R16,25
;     553         
;     554         //Disable S1D13705 en RD off
;     555         S1D13705_RD = RD_OFF;
	SBI  0x12,7
;     556         S1D13705_CS = CS_OFF;
	SBI  0x12,4
;     557         
;     558         return(S1Ddata);
	MOV  R30,R16
	CALL __LOADLOCR5
	RJMP _0x247
;     559 }   
;     560 //*************************************
;     561 
;     562 /**************************************
;     563 Functie: Schrijf data naar S1D13705
;     564 **************************************/
;     565 void S1D13705_Write_Data(unsigned long int adress, unsigned char data)
;     566 {
_S1D13705_Write_Data:
;     567         unsigned int Addr_High;
;     568         unsigned int Addr_Low;
;     569         
;     570         //Zet Data / Adres port als output
;     571         PORT_AD_ctrl = PORT_OUPUT;                                      //Maak port ad output
	CALL __SAVELOCR4
;	adress -> Y+5
;	data -> Y+4
;	Addr_High -> R16,R17
;	Addr_Low -> R18,R19
	CALL SUBOPT_0xF
;     572 
;     573         //Bepaal of A16 1 of 0 is
;     574         if((adress & 0x10000) == 0x10000)                               //Als waar A16 is 1 anders 0
	BRNE _0x70
;     575                 PORT_Addr_A16 = 1;
	SBI  0x7,2
;     576         else
	RJMP _0x71
_0x70:
;     577                 PORT_Addr_A16 = 0;
	CBI  0x7,2
;     578                 
;     579         //Maak hoge en lage adress waardes
;     580         Addr_High = (adress >> 8) & 0xFF;
_0x71:
	CALL SUBOPT_0x10
	__PUTW1R 16,17
;     581         Addr_Low = adress & 0xFF;         
	__GETD1S 5
	__ANDD1N 0xFF
	__PUTW1R 18,19
;     582         
;     583         //Latch de lage waarde naar uitgang van de Latch
;     584         PORT_Addr_Low = Addr_Low;
	__GETW1R 18,19
	OUT  0x1B,R30
;     585         
;     586         ALE_ON;                                                         //Latch de data, data word nu naar uitgang geschoven
	SBI  0x7,1
;     587         ALE_OFF;                                                        //Ale uit
	CBI  0x7,1
;     588         
;     589         //Zet hoge waarde op Port_addr_high
;     590         PORT_Addr_High = Addr_High;
	__GETW1R 16,17
	OUT  0x15,R30
;     591          
;     592         //Zet data op port
;     593         PORT_Data_OUT = data;                                           //Schrijf de data naar de S1D13705     
	LDD  R30,Y+4
	OUT  0x1B,R30
;     594 
;     595         //Enable S1D13705               
;     596         S1D13705_CS = CS_ON;                                            //Enable de S1D13705
	CBI  0x12,4
;     597                 
;     598         //WR enable S1D13705 en wacht op wait
;     599         S1D13705_WR = WR_ON;                                            //Maak WR laag en zet schrijven actief 
	CBI  0x12,6
;     600         while(S1D13705_WAIT);                                           //Wacht totdat WAIT 0 wordt
_0x72:
	SBIC 0x5,0
	RJMP _0x72
;     601         while(S1D13705_WAIT == 0);                                      //Wacht totdat WAIT 1 wordt
_0x75:
	SBIS 0x5,0
	RJMP _0x75
;     602         
;     603         //Disable S1D13705 en WR
;     604         S1D13705_WR = WR_OFF;
	SBI  0x12,6
;     605         S1D13705_CS = CS_OFF;
	SBI  0x12,4
;     606         
;     607         return;
	CALL __LOADLOCR4
	RJMP _0x247
;     608 }
;     609 //*************************************
;     610 
;     611 /**************************************
;     612 Functie: Lees register S1D13705
;     613 **************************************/
;     614 unsigned char S1D13705_Read_Register(unsigned char reg)
;     615 {
_S1D13705_Read_Register:
;     616         unsigned char data;
;     617         
;     618         data = S1D13705_Read_Data(S1D13705_Reg_Base_Addr + reg);
	ST   -Y,R16
;	reg -> Y+1
;	data -> R16
	CALL SUBOPT_0x11
	CALL _S1D13705_Read_Data
	MOV  R16,R30
;     619         
;     620         return(data);
	MOV  R30,R16
	LDD  R16,Y+0
	RJMP _0x248
;     621 }
;     622 //*************************************
;     623 
;     624 /**************************************
;     625 Functie: Schrijf register S1D13705
;     626 **************************************/
;     627 void S1D13705_Write_Register(unsigned char reg, unsigned char data)
;     628 {
_S1D13705_Write_Register:
;     629         S1D13705_Write_Data(S1D13705_Reg_Base_Addr + reg, data);
	CALL SUBOPT_0x11
	CALL SUBOPT_0x12
;     630         
;     631         return;
_0x248:
	ADIW R28,2
	RET
;     632 }
;     633 //*************************************
;     634 
;     635 /**************************************
;     636 Functie: Init S1D13705
;     637 **************************************/
;     638 void S1D13705_Init(void)
;     639 {
_S1D13705_Init:
;     640         unsigned int i;
;     641         
;     642         //Stel registers van S1D13705 in
;     643         for(i=1;i<28;i++)  
	ST   -Y,R17
	ST   -Y,R16
;	i -> R16,R17
	__GETWRN 16,17,1
_0x79:
	__CPWRN 16,17,28
	BRSH _0x7A
;     644                 S1D13705_Write_Register(aS1DRegs[i].Index,aS1DRegs[i].Value);
	__GETW1R 16,17
	LSL  R30
	ROL  R31
	SUBI R30,LOW(-_aS1DRegs_G2)
	SBCI R31,HIGH(-_aS1DRegs_G2)
	LD   R30,Z
	ST   -Y,R30
	__GETW1R 16,17
	LSL  R30
	ROL  R31
	__POINTW2MN _aS1DRegs_G2,1
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _S1D13705_Write_Register
;     645         
;     646         //Stel LUT tabel in
;     647         #ifdef S1D13705_16_colors
;     648         S1D13705_LUT_16();       
;     649         #endif
;     650         
;     651         #ifdef S1D13705_256_colors
;     652         S1D13705_LUT_256();
	__ADDWRN 16,17,1
	RJMP _0x79
_0x7A:
	RCALL _S1D13705_LUT_256
;     653         #endif
;     654         
;     655         return;
	LD   R16,Y+
	LD   R17,Y+
	RET
;     656 }
;     657 //*************************************
;     658 
;     659 /**************************************
;     660 Functie: Zet tekst op LCD op x,y voor 4BPP en 8BPP!!! 
;     661          
;     662          4BPP: 2 pixel per byte geheugen 37,5KB
;     663          8BPP: 1 pixel per byte geheugen 75KB
;     664 **************************************/
;     665 void S1D13705_Text_LCD(unsigned long int X, unsigned long int Y, unsigned char *Tekst, unsigned char color)
;     666 {
_S1D13705_Text_LCD:
;     667         unsigned long int S1D_mem;
;     668         unsigned int Ytekst;
;     669         unsigned char Font_Waarde;
;     670         unsigned char data;
;     671         unsigned char ch;
;     672         unsigned char BPP;
;     673         unsigned char bits;
;     674         
;     675         //Check kleurinstelling
;     676         BPP = S1D13705_Read_Register(0x02);                             //Vanuit gaan dat kleuren scherm is alleen reg 2 lezen
	SBIW R28,5
	CALL __SAVELOCR6
;	X -> Y+18
;	Y -> Y+14
;	*Tekst -> Y+12
;	color -> Y+11
;	S1D_mem -> Y+7
;	Ytekst -> R16,R17
;	Font_Waarde -> R18
;	data -> R19
;	ch -> R20
;	BPP -> R21
;	bits -> Y+6
	LDI  R30,LOW(2)
	ST   -Y,R30
	CALL _S1D13705_Read_Register
	MOV  R21,R30
;     677         BPP &= 0xC0;                                                    //Alleen 2 hoogste bit gelden 
	ANDI R21,LOW(192)
;     678         
;     679         //X is pixels, Y in regels 240/8 = 30 regels
;     680         Y = Y * 8;                                                      //Doe Y maal acht vanwege 8 pixel font hoogte
	__GETD2S 14
	__GETD1N 0x8
	CALL __MULD12U
	__PUTD1S 14
;     681         color &= 0x0F;                                                  //Van kleur alleen de low nibbble
	LDD  R30,Y+11
	ANDI R30,LOW(0xF)
	STD  Y+11,R30
;     682         
;     683         switch(BPP)
	MOV  R30,R21
;     684         {       
;     685                 case 0x80:                                              //4BPP
	CPI  R30,LOW(0x80)
	BREQ PC+3
	JMP _0x7E
;     686         
;     687                         while(*Tekst != 0)                                              //Alle tekens gehad, verlaat loop
_0x7F:
	CALL SUBOPT_0x13
	BRNE PC+3
	JMP _0x81
;     688                         {                                                               
;     689                                 ch = *Tekst++;                                          //Haal teken uit de string
	CALL SUBOPT_0x14
;     690                 
;     691                                 for(Ytekst=0;Ytekst<8;Ytekst++)
	__GETWRN 16,17,0
_0x83:
	__CPWRN 16,17,8
	BRLO PC+3
	JMP _0x84
;     692                                 {
;     693                                         Font_Waarde = FontTable[ch - ' '][Ytekst];      //Haal Font waarde uit fonttable
	CALL SUBOPT_0x15
;     694 
;     695                         
;     696                                         //Bereken geheugen plaats van te schrijven data voor pixel
;     697                                         S1D_mem = X + (Ytekst * 160) + (Y * 160);       
	LDI  R30,LOW(160)
	LDI  R31,HIGH(160)
	CALL SUBOPT_0x16
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETD2S 14
	__GETD1N 0xA0
	CALL __MULD12U
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __ADDD12
	__PUTD1S 7
;     698                         
;     699                                         //Check 2 pixels van Tekst(4bpp mode)
;     700                                         //en zet waarde in S1D mem 
;     701                                         data = 0xFF;                                    //Zet data op wit kleur   
	LDI  R19,LOW(255)
;     702                                         if(Font_Waarde & 0x80)
	SBRS R18,7
	RJMP _0x85
;     703                                                 data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
	CALL SUBOPT_0x17
;     704                                         if(Font_Waarde & 0x40)
_0x85:
	SBRS R18,6
	RJMP _0x86
;     705                                                 data &= (0xF0 | color);                 //Zet low nibble op kleur 
	LDD  R30,Y+11
	ORI  R30,LOW(0xF0)
	AND  R19,R30
;     706                                         S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
_0x86:
	CALL SUBOPT_0x18
;     707                 
;     708                                         data = 0xFF;                                    //Zet data op wit kleur   
	LDI  R19,LOW(255)
;     709                                         if(Font_Waarde & 0x20)
	SBRS R18,5
	RJMP _0x87
;     710                                                 data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
	CALL SUBOPT_0x17
;     711                                         if(Font_Waarde & 0x10)
_0x87:
	SBRS R18,4
	RJMP _0x88
;     712                                                 data &= (0xF0 | color);                 //Zet low nibble op kleur 
	LDD  R30,Y+11
	ORI  R30,LOW(0xF0)
	AND  R19,R30
;     713                                         S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
_0x88:
	CALL SUBOPT_0x18
;     714                 
;     715                                         data = 0xFF;                                    //Zet data op wit kleur   
	LDI  R19,LOW(255)
;     716                                         if(Font_Waarde & 0x08)
	SBRS R18,3
	RJMP _0x89
;     717                                                 data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
	CALL SUBOPT_0x17
;     718                                         if(Font_Waarde & 0x04)
_0x89:
	SBRS R18,2
	RJMP _0x8A
;     719                                                 data &= (0xF0 | color);                 //Zet low nibble op kleur 
	LDD  R30,Y+11
	ORI  R30,LOW(0xF0)
	AND  R19,R30
;     720                                         S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
_0x8A:
	CALL SUBOPT_0x18
;     721                 
;     722                                         data = 0xFF;                                    //Zet data op wit kleur   
	LDI  R19,LOW(255)
;     723                                         if(Font_Waarde & 0x02)
	SBRS R18,1
	RJMP _0x8B
;     724                                                 data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
	CALL SUBOPT_0x17
;     725                                         if(Font_Waarde & 0x01)
_0x8B:
	SBRS R18,0
	RJMP _0x8C
;     726                                                 data &= (0xF0 | color);                 //Zet low nibble op kleur 
	LDD  R30,Y+11
	ORI  R30,LOW(0xF0)
	AND  R19,R30
;     727                                         S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
_0x8C:
	CALL SUBOPT_0x18
;     728                                 }
	__ADDWRN 16,17,1
	RJMP _0x83
_0x84:
;     729                                 
;     730                                 X += 4;                                                 //Ga 1 karakter naar rechts
	__GETD1S 18
	__ADDD1N 4
	__PUTD1S 18
;     731                         }
	RJMP _0x7F
_0x81:
;     732                         break;
	RJMP _0x7D
;     733                          
;     734                 case 0xC0:                                                              //8BPP
_0x7E:
	CPI  R30,LOW(0xC0)
	BREQ PC+3
	JMP _0x7D
;     735                         while(*Tekst != 0)                                              //Alle tekens gehad, verlaat loop
_0x8E:
	CALL SUBOPT_0x13
	BRNE PC+3
	JMP _0x90
;     736                         {                                                               
;     737                                 ch = *Tekst++;                                          //Haal teken uit de string
	CALL SUBOPT_0x14
;     738                 
;     739                                 for(Ytekst=0;Ytekst<8;Ytekst++)
	__GETWRN 16,17,0
_0x92:
	__CPWRN 16,17,8
	BRSH _0x93
;     740                                 {
;     741                                         Font_Waarde = FontTable[ch - ' '][Ytekst];      //Haal Font waarde uit fonttable
	CALL SUBOPT_0x15
;     742 
;     743                                         //Bereken geheugen plaats van te schrijven data voor pixel
;     744                                         S1D_mem = X + (Ytekst * 320) + (Y * 320);       
	LDI  R30,LOW(320)
	LDI  R31,HIGH(320)
	CALL SUBOPT_0x16
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETD2S 14
	__GETD1N 0x140
	CALL __MULD12U
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __ADDD12
	__PUTD1S 7
;     745                         
;     746                                         //Check alle 8 bits en schrijf bij 1 de kleur en bij 0 wit
;     747                                         
;     748                                         for(bits=0;bits<8;bits++)
	LDI  R30,LOW(0)
	STD  Y+6,R30
_0x95:
	LDD  R26,Y+6
	CPI  R26,LOW(0x8)
	BRSH _0x96
;     749                                         {
;     750                                                 //Check of bit 1, zo ja zet pixel kleur
;     751                                                 if(Font_Waarde & 0x80)
	SBRS R18,7
	RJMP _0x97
;     752                                                         data = color;                   //Zet kleur
	LDD  R19,Y+11
;     753                                                 else
	RJMP _0x98
_0x97:
;     754                                                         data = 0xFF;                    //Zet kleur wit
	LDI  R19,LOW(255)
;     755                                                 S1D13705_Write_Data(S1D_mem++,data);    //Schrijf data naar memory
_0x98:
	CALL SUBOPT_0x18
;     756                                                 
;     757                                                 Font_Waarde <<= 1;                      //Font Waarde 1 naar links opschuiven
	LSL  R18
;     758                                         }
	LDD  R30,Y+6
	SUBI R30,-LOW(1)
	STD  Y+6,R30
	RJMP _0x95
_0x96:
;     759                                 }
	__ADDWRN 16,17,1
	RJMP _0x92
_0x93:
;     760                                 
;     761                                 X +=8;                                                  //Ga 1 karakter naar rechts
	__GETD1S 18
	__ADDD1N 8
	__PUTD1S 18
;     762                         }
	RJMP _0x8E
_0x90:
;     763                         break;
;     764                 }                                        
_0x7D:
;     765                          
;     766         return;
	CALL __LOADLOCR6
	ADIW R28,22
	RET
;     767 }
;     768 //*************************************             
;     769 
;     770 /**************************************
;     771 Functie: Teken plaatje op scherm
;     772          Plaatje die als header is toegevoed
;     773 **************************************/
;     774 void S1D13705_Draw_Pic(unsigned int X, unsigned int Y, unsigned char flash* Pic)
;     775 {
;     776         unsigned long int S1D_mem;                                      //S1D13705 memory adress
;     777         unsigned int W;                                                 //Breedte plaatje
;     778         unsigned int H;                                                 //Hoogte plaatje
;     779         unsigned int Wx;                                                //Counter breedte
;     780         unsigned int Byte;                                              //Byte teller van de array
;     781         unsigned int Yx;                                                //Waarde voor scanline bytes, by 4bpp 160, by 8bpp 320
;     782         unsigned char BPP;                                              //Kleur waarde
;     783         
;     784         //Check kleurinstelling
;     785         BPP = S1D13705_Read_Register(0x02);                             //Vanuit gaan dat kleuren scherm is alleen reg 2 lezen
;	X -> Y+19
;	Y -> Y+17
;	*Pic -> Y+15
;	S1D_mem -> Y+11
;	W -> R16,R17
;	H -> R18,R19
;	Wx -> R20,R21
;	Byte -> Y+9
;	Yx -> Y+7
;	BPP -> Y+6
;     786         BPP &= 0xC0;                                                    //Alleen 2 hoogste bit gelden 
;     787        
;     788         //Bepaal welke kleur instelling en laad juiste LUT tabel
;     789         switch(BPP)
;     790         {
;     791                 case 0x80:                                              //16 kleuren
;     792                         S1D13705_LUT_16_BMP();                          //Schrijf LUT tabel 16 kleuren
;     793                         Yx = 160;                                       //160 is aantal scanline bytes
;     794                         break;
;     795                 case 0xC0:                                              //256 kleuren
;     796                         S1D13705_LUT_256_BMP();                         //Schrijf LUT voor 256 kleuren
;     797                         Yx = 320;                                       //320 is aantal scaneline bytes
;     798                         break;
;     799        }
;     800         
;     801         
;     802         //Lees de hoogte en de breedte van de Pic
;     803         Byte = 0;
;     804         W = Pic[Byte++];
;     805         H = Pic[Byte++];
;     806         
;     807         //Zet S1D_mem op begin adress/pixel
;     808         S1D_mem = X + (Y * Yx);
;     809         
;     810         while(H--)
;     811         {
;     812                 Wx = W;
;     813                 while(Wx--)
;     814                 {
;     815                         S1D13705_Write_Data(S1D_mem++,Pic[Byte++]);     //Schrijf data naar S1D13705
;     816                 }
;     817                 
;     818                 //Herbereken S1D_mem voor de volgende lijn
;     819                 S1D_mem = (S1D_mem - W) + Yx;
;     820         }                                     
;     821        
;     822         return;
;     823 }
;     824 //*************************************
;     825 
;     826 /**************************************
;     827 Functie: Teken een pixel
;     828 **************************************/
;     829 void S1D13705_Draw_Pixel(unsigned long int X, unsigned long int Y, unsigned char color)
;     830 {
;     831         unsigned long int S1D_mem_addr;
;     832         
;     833         //Bereken welk adress S1D13705 moet worden geschreven Pixel 0,0 = adres 0, Pixel 319,0 = 319 etc
;     834         S1D_mem_addr = X + (Y * 320);                                   //Voor Y doe Y aantal maal de breedte van het display en tel X erbij op
;	X -> Y+9
;	Y -> Y+5
;	color -> Y+4
;	S1D_mem_addr -> Y+0
;     835         
;     836         //Schrijf Pixel
;     837         S1D13705_Write_Data(S1D_mem_addr,color);
;     838         
;     839         return;
;     840 }
;     841 //*************************************
;     842 
;     843 /**************************************
;     844 Functie: Teken een horizontale lijn
;     845 **************************************/
;     846 void S1D13705_Draw_Horizontal_Line(unsigned long int X, unsigned long int X1, unsigned long int Y, unsigned char color)
;     847 {
_S1D13705_Draw_Horizontal_Line:
;     848         unsigned long int S1D_mem_addr;
;     849         unsigned int Xa;
;     850         
;     851         //Bereken begin adress van de beginde pixel 
;     852         S1D_mem_addr = X + (Y * 320);
	CALL SUBOPT_0x1
;	X -> Y+15
;	X1 -> Y+11
;	Y -> Y+7
;	color -> Y+6
;	S1D_mem_addr -> Y+2
;	Xa -> R16,R17
	__GETD2S 7
	__GETD1N 0x140
	CALL __MULD12U
	__GETD2S 15
	CALL __ADDD12
	__PUTD1S 2
;     853         
;     854         //Teken lijn, eerst checken of X1 kleiner is als X, dan word lijn naar links getekend
;     855         if(X1 < X)                                                      //X1 is kleiner dan X, dan moet lijn links toe
	__GETD1S 15
	__GETD2S 11
	CALL __CPD21
	BRSH _0xA4
;     856         {
;     857                 Xa= X- X1;
	CALL __SUBD12
	__PUTW1R 16,17
;     858                 do
_0xA6:
;     859                 {
;     860                         S1D13705_Write_Data(S1D_mem_addr--,color);
	__GETD1S 2
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	__PUTD1S 2
	__SUBD1N -1
	CALL SUBOPT_0x19
;     861                 }
;     862                 while(Xa--);
	__GETW1R 16,17
	__SUBWRN 16,17,1
	SBIW R30,0
	BRNE _0xA6
;     863                
;     864         }
;     865         else          
	RJMP _0xA8
_0xA4:
;     866         {
;     867                 Xa = X1 - X;
	__GETD2S 15
	__GETD1S 11
	CALL __SUBD12
	__PUTW1R 16,17
;     868                 do
_0xAA:
;     869                 {
;     870                         S1D13705_Write_Data(S1D_mem_addr++,color);   
	CALL SUBOPT_0x1A
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	CALL SUBOPT_0x19
;     871                 }
;     872                 while(Xa--);                
	__GETW1R 16,17
	__SUBWRN 16,17,1
	SBIW R30,0
	BRNE _0xAA
;     873         }
_0xA8:
;     874         
;     875         return;
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,19
	RET
;     876 }
;     877 //*************************************
;     878 
;     879 /**************************************
;     880 Functie: Teken verticale lijn
;     881 **************************************/
;     882 void S1D13705_Draw_Vertical_Line(unsigned long int X, unsigned long int Y, unsigned long int Y1, unsigned char color)
;     883 {
_S1D13705_Draw_Vertical_Line:
;     884         unsigned long int S1D_mem_addr;
;     885         unsigned long int Ya;
;     886         
;     887         //Bereken begin mem adress van de beginde pixel
;     888  //       S1D_mem_addr = X + (Y * 320);
;     889         
;     890         //Teken lijn, eerst checken of Y1 kleiner is dan Y, dan wordt lijn naar boven getekend     
;     891         if(Y1 < Y)                                                      //Y1 is kleiner dus lijn naar boven tekenen
	SBIW R28,8
;	X -> Y+17
;	Y -> Y+13
;	Y1 -> Y+9
;	color -> Y+8
;	S1D_mem_addr -> Y+4
;	Ya -> Y+0
	__GETD1S 13
	__GETD2S 9
	CALL __CPD21
	BRSH _0xAC
;     892         {
;     893                 Ya = Y - Y1;
	CALL __SUBD12
	__PUTD1S 0
;     894                 do
_0xAE:
;     895                 {
;     896                         S1D_mem_addr = X + ((Y + Ya) * 320);                  //Herberken mem addres
	CALL SUBOPT_0x1B
;     897                         S1D13705_Write_Data(S1D_mem_addr,color);        //Schrijf data
;     898                 }
;     899                 while(Ya--);
	CALL SUBOPT_0x1C
	BRNE _0xAE
;     900         }
;     901         else
	RJMP _0xB0
_0xAC:
;     902         {
;     903                 Ya = Y1 - Y;
	__GETD2S 13
	__GETD1S 9
	CALL __SUBD12
	__PUTD1S 0
;     904                 do
_0xB2:
;     905                 {
;     906                         S1D_mem_addr = X + ((Y + Ya) * 320);                  //Herberken mem addres
	CALL SUBOPT_0x1B
;     907                         S1D13705_Write_Data(S1D_mem_addr,color);        //Schrijf data
;     908                 } 
;     909                 while(Ya--);
	CALL SUBOPT_0x1C
	BRNE _0xB2
;     910         }
_0xB0:
;     911         return;
	ADIW R28,21
	RET
;     912 }
;     913 //*************************************
;     914                  
;     915 /**************************************
;     916 Functie: Vul LCD scherm met een kleur
;     917 **************************************/
;     918 void S1D13705_Fill_LCD(unsigned long int adress, unsigned long int len, unsigned char color)
;     919 {
_S1D13705_Fill_LCD:
;     920         color |=(color<<4);
	LD   R30,Y
	SWAP R30
	ANDI R30,0xF0
	LD   R26,Y
	OR   R30,R26
	ST   Y,R30
;     921         while(len--)
_0xB4:
	__GETD1S 1
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	__PUTD1S 1
	__SUBD1N -1
	BREQ _0xB6
;     922                 S1D13705_Write_Data(adress++,color); 
	CALL SUBOPT_0x1D
	CALL SUBOPT_0x12
;     923               
;     924         return;
	RJMP _0xB4
_0xB6:
_0x247:
	ADIW R28,9
	RET
;     925 }
;     926 //*************************************
;     927 
;     928 /**************************************
;     929 Functie: LCD CLS
;     930 **************************************/
;     931 void S1D13705_Clear_LCD(void)
;     932 {
_S1D13705_Clear_LCD:
;     933         S1D13705_Fill_LCD(0,0x14000,15);
	__GETD1N 0x0
	CALL __PUTPARD1
	__GETD1N 0x14000
	CALL SUBOPT_0x8
	CALL _S1D13705_Fill_LCD
;     934         
;     935         return;
	RET
;     936 }                   
;     937 //*************************************
;     938 
;     939 /**************************************
;     940 Functie: Reset S1D13705
;     941 **************************************/
;     942 void S1D13705_Reset(void)
;     943 {
_S1D13705_Reset:
;     944         S1D13705_RST = RST_ON;
	CBI  0x12,5
;     945         #asm("nop");
	nop
;     946         S1D13705_RST = RST_OFF;
	SBI  0x12,5
;     947 
;     948         return;
	RET
;     949 }
;     950 //*************************************       
;     951  
;     952 /**************************************
;     953         LUT table values:
;     954 - 16 Colors
;     955 - 256 Colors
;     956 - own bmp Colors
;     957 **************************************/
;     958 
;     959 /*************************************
;     960         LUT values 16 colors
;     961 *************************************/
;     962 flash unsigned char LUT_16[16*3] =
;     963 { 
;     964 	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,
;     965 	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,
;     966 	0x00, 0x00, 0x00,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,
;     967 	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
;     968 };
;     969 //*************************************
;     970 
;     971 /**************************************
;     972         LUT values 256 colors
;     973 **************************************/
;     974 flash unsigned char LUT_256[256*3] =
;     975 {
;     976 /* Primary and secondary colors */
;     977 	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,  
;     978 	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,  
;     979 	0x50, 0x50, 0x50,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,  
;     980 	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0,  
;     981 
;     982 /* Gray shades */
;     983 	0x00, 0x00, 0x00,  0x10, 0x10, 0x10,  0x20, 0x20, 0x20,  0x30, 0x30, 0x30,  
;     984 	0x40, 0x40, 0x40,  0x50, 0x50, 0x50,  0x60, 0x60, 0x60,  0x70, 0x70, 0x70,  
;     985 	0x80, 0x80, 0x80,  0x90, 0x90, 0x90,  0xA0, 0xA0, 0xA0,  0xB0, 0xB0, 0xB0,  
;     986 	0xC0, 0xC0, 0xC0,  0xD0, 0xD0, 0xD0,  0xE0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  
;     987 
;     988 /* Black to red */
;     989 	0x00, 0x00, 0x00,  0x10, 0x00, 0x00,  0x20, 0x00, 0x00,  0x30, 0x00, 0x00,  
;     990 	0x40, 0x00, 0x00,  0x50, 0x00, 0x00,  0x60, 0x00, 0x00,  0x70, 0x00, 0x00,  
;     991 	0x80, 0x00, 0x00,  0x90, 0x00, 0x00,  0xA0, 0x00, 0x00,  0xB0, 0x00, 0x00,  
;     992 	0xC0, 0x00, 0x00,  0xD0, 0x00, 0x00,  0xE0, 0x00, 0x00,  0xF0, 0x00, 0x00,  
;     993 
;     994 /* Black to green */
;     995 	0x00, 0x00, 0x00,  0x00, 0x10, 0x00,  0x00, 0x20, 0x00,  0x00, 0x30, 0x00,  
;     996 	0x00, 0x40, 0x00,  0x00, 0x50, 0x00,  0x00, 0x60, 0x00,  0x00, 0x70, 0x00,  
;     997 	0x00, 0x80, 0x00,  0x00, 0x90, 0x00,  0x00, 0xA0, 0x00,  0x00, 0xB0, 0x00,  
;     998 	0x00, 0xC0, 0x00,  0x00, 0xD0, 0x00,  0x00, 0xE0, 0x00,  0x00, 0xF0, 0x00,  
;     999 
;    1000 /* Black to blue */
;    1001 	0x00, 0x00, 0x00,  0x00, 0x00, 0x10,  0x00, 0x00, 0x20,  0x00, 0x00, 0x30,  
;    1002 	0x00, 0x00, 0x40,  0x00, 0x00, 0x50,  0x00, 0x00, 0x60,  0x00, 0x00, 0x70,  
;    1003 	0x00, 0x00, 0x80,  0x00, 0x00, 0x90,  0x00, 0x00, 0xA0,  0x00, 0x00, 0xB0,  
;    1004 	0x00, 0x00, 0xC0,  0x00, 0x00, 0xD0,  0x00, 0x00, 0xE0,  0x00, 0x00, 0xF0,
;    1005 
;    1006 /* Blue to cyan (blue and green) */
;    1007 	0x00, 0x00, 0xF0,  0x00, 0x10, 0xF0,  0x00, 0x20, 0xF0,  0x00, 0x30, 0xF0,  
;    1008 	0x00, 0x40, 0xF0,  0x00, 0x50, 0xF0,  0x00, 0x60, 0xF0,  0x00, 0x70, 0xF0,  
;    1009 	0x00, 0x80, 0xF0,  0x00, 0x90, 0xF0,  0x00, 0xA0, 0xF0,  0x00, 0xB0, 0xF0,  
;    1010 	0x00, 0xC0, 0xF0,  0x00, 0xD0, 0xF0,  0x00, 0xE0, 0xF0,  0x00, 0xF0, 0xF0,
;    1011 
;    1012 /* Cyan (blue and green) to green */
;    1013 	0x00, 0xF0, 0xF0,  0x00, 0xF0, 0xE0,  0x00, 0xF0, 0xD0,  0x00, 0xF0, 0xC0,  
;    1014 	0x00, 0xF0, 0xB0,  0x00, 0xF0, 0xA0,  0x00, 0xF0, 0x90,  0x00, 0xF0, 0x80,  
;    1015 	0x00, 0xF0, 0x70,  0x00, 0xF0, 0x60,  0x00, 0xF0, 0x50,  0x00, 0xF0, 0x40,  
;    1016 	0x00, 0xF0, 0x30,  0x00, 0xF0, 0x20,  0x00, 0xF0, 0x10,  0x00, 0xF0, 0x00,  
;    1017 
;    1018 /* Green to yellow (red and green) */
;    1019 	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x00,  0x20, 0xF0, 0x00,  0x30, 0xF0, 0x00,  
;    1020 	0x40, 0xF0, 0x00,  0x50, 0xF0, 0x00,  0x60, 0xF0, 0x00,  0x70, 0xF0, 0x00,  
;    1021 	0x80, 0xF0, 0x00,  0x90, 0xF0, 0x00,  0xA0, 0xF0, 0x00,  0xB0, 0xF0, 0x00,  
;    1022 	0xC0, 0xF0, 0x00,  0xD0, 0xF0, 0x00,  0xE0, 0xF0, 0x00,  0xF0, 0xF0, 0x00,  
;    1023 
;    1024 /* Yellow (red and green) to red */
;    1025 	0xF0, 0xF0, 0x00,  0xF0, 0xE0, 0x00,  0xF0, 0xD0, 0x00,  0xF0, 0xC0, 0x00,  
;    1026 	0xF0, 0xB0, 0x00,  0xF0, 0xA0, 0x00,  0xF0, 0x90, 0x00,  0xF0, 0x80, 0x00,  
;    1027 	0xF0, 0x70, 0x00,  0xF0, 0x60, 0x00,  0xF0, 0x50, 0x00,  0xF0, 0x40, 0x00,  
;    1028 	0xF0, 0x30, 0x00,  0xF0, 0x20, 0x00,  0xF0, 0x10, 0x00,  0xF0, 0x00, 0x00,  
;    1029 
;    1030 /* Red to magenta (blue and red) */
;    1031 	0xF0, 0x00, 0x00,  0xF0, 0x00, 0x10,  0xF0, 0x00, 0x20,  0xF0, 0x00, 0x30,  
;    1032 	0xF0, 0x00, 0x40,  0xF0, 0x00, 0x50,  0xF0, 0x00, 0x60,  0xF0, 0x00, 0x70,  
;    1033 	0xF0, 0x00, 0x80,  0xF0, 0x00, 0x90,  0xF0, 0x00, 0xA0,  0xF0, 0x00, 0xB0,  
;    1034 	0xF0, 0x00, 0xC0,  0xF0, 0x00, 0xD0,  0xF0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  
;    1035 
;    1036 /* Magenta (blue and red) to blue */
;    1037 	0xF0, 0x00, 0xF0,  0xE0, 0x00, 0xF0,  0xD0, 0x00, 0xF0,  0xC0, 0x00, 0xF0,  
;    1038 	0xB0, 0x00, 0xF0,  0xA0, 0x00, 0xF0,  0x90, 0x00, 0xF0,  0x80, 0x00, 0xF0,  
;    1039 	0x70, 0x00, 0xF0,  0x60, 0x00, 0xF0,  0x50, 0x00, 0xF0,  0x40, 0x00, 0xF0,  
;    1040 	0x30, 0x00, 0xF0,  0x20, 0x00, 0xF0,  0x10, 0x00, 0xF0,  0x00, 0x00, 0xF0,
;    1041 
;    1042 /* Black to magenta (blue and red) */
;    1043 	0x00, 0x00, 0x00,  0x10, 0x00, 0x10,  0x20, 0x00, 0x20,  0x30, 0x00, 0x30,  
;    1044 	0x40, 0x00, 0x40,  0x50, 0x00, 0x50,  0x60, 0x00, 0x60,  0x70, 0x00, 0x70,  
;    1045 	0x80, 0x00, 0x80,  0x90, 0x00, 0x90,  0xA0, 0x00, 0xA0,  0xB0, 0x00, 0xB0,  
;    1046 	0xC0, 0x00, 0xC0,  0xD0, 0x00, 0xD0,  0xE0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  
;    1047 
;    1048 /* Black to cyan (blue and green) */
;    1049 	0x00, 0x00, 0x00,  0x00, 0x10, 0x10,  0x00, 0x20, 0x20,  0x00, 0x30, 0x30,  
;    1050 	0x00, 0x40, 0x40,  0x00, 0x50, 0x50,  0x00, 0x60, 0x60,  0x00, 0x70, 0x70,  
;    1051 	0x00, 0x80, 0x80,  0x00, 0x90, 0x90,  0x00, 0xA0, 0xA0,  0x00, 0xB0, 0xB0,  
;    1052 	0x00, 0xC0, 0xC0,  0x00, 0xD0, 0xD0,  0x00, 0xE0, 0xE0,  0x00, 0xF0, 0xF0,  
;    1053 
;    1054 /* Red to white */
;    1055 	0xF0, 0x00, 0x00,  0xF0, 0x10, 0x10,  0xF0, 0x20, 0x20,  0xF0, 0x30, 0x30,  
;    1056 	0xF0, 0x40, 0x40,  0xF0, 0x50, 0x50,  0xF0, 0x60, 0x60,  0xF0, 0x70, 0x70,  
;    1057 	0xF0, 0x80, 0x80,  0xF0, 0x90, 0x90,  0xF0, 0xA0, 0xA0,  0xF0, 0xB0, 0xB0,  
;    1058 	0xF0, 0xC0, 0xC0,  0xF0, 0xD0, 0xD0,  0xF0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  
;    1059 
;    1060 /* Green to white */
;    1061 	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x10,  0x20, 0xF0, 0x20,  0x30, 0xF0, 0x30,  
;    1062 	0x40, 0xF0, 0x40,  0x50, 0xF0, 0x50,  0x60, 0xF0, 0x60,  0x70, 0xF0, 0x70,  
;    1063 	0x80, 0xF0, 0x80,  0x90, 0xF0, 0x90,  0xA0, 0xF0, 0xA0,  0xB0, 0xF0, 0xB0,  
;    1064 	0xC0, 0xF0, 0xC0,  0xD0, 0xF0, 0xD0,  0xE0, 0xF0, 0xE0,  0xF0, 0xF0, 0xF0,  
;    1065 
;    1066 /* Blue to white */
;    1067 	0x00, 0x00, 0xF0,  0x10, 0x10, 0xF0,  0x20, 0x20, 0xF0,  0x30, 0x30, 0xF0,  
;    1068 	0x40, 0x40, 0xF0,  0x50, 0x50, 0xF0,  0x60, 0x60, 0xF0,  0x70, 0x70, 0xF0,  
;    1069 	0x80, 0x80, 0xF0,  0x90, 0x90, 0xF0,  0xA0, 0xA0, 0xF0,  0xB0, 0xB0, 0xF0,  
;    1070 	0xC0, 0xC0, 0xF0,  0xD0, 0xD0, 0xF0,  0xE0, 0xE0, 0xF0,  0xF0, 0xF0, 0xF0
;    1071 };
;    1072 //*************************************
;    1073 
;    1074 /**************************************
;    1075         LUT TABLE PIC 256 Colors
;    1076 BMP hs not RGB but BGR value, in function
;    1077 S1D13705_LUT_256_BMP these values are
;    1078 automaticly being switch
;    1079 ***************************************/
;    1080 flash unsigned char LUT_256_BMP[256*3] =
;    1081 {
;    1082 	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 
;    1083 	0x80, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 
;    1084 	0xC0, 0xDC, 0xC0, 0xF0, 0xCA, 0xA6, 0x00, 0x20, 0x40, 0x00, 0x20, 0x60, 
;    1085 	0x00, 0x20, 0x80, 0x00, 0x20, 0xA0, 0x00, 0x20, 0xC0, 0x00, 0x20, 0xE0, 
;    1086 	
;    1087 	0x00, 0x40, 0x00, 0x00, 0x40, 0x20, 0x00, 0x40, 0x40, 0x00, 0x40, 0x60, 
;    1088 	0x00, 0x40, 0x80, 0x00, 0x40, 0xA0, 0x00, 0x40, 0xC0, 0x00, 0x40, 0xE0, 
;    1089 	0x00, 0x60, 0x00, 0x00, 0x60, 0x20, 0x00, 0x60, 0x40, 0x00, 0x60, 0x60, 
;    1090 	0x00, 0x60, 0x80, 0x00, 0x60, 0xA0, 0x00, 0x60, 0xC0, 0x00, 0x60, 0xE0, 
;    1091 
;    1092 	0x00, 0x80, 0x00, 0x00, 0x80, 0x20, 0x00, 0x80, 0x40, 0x00, 0x80, 0x60, 
;    1093 	0x00, 0x80, 0x80, 0x00, 0x80, 0xA0, 0x00, 0x80, 0xC0, 0x00, 0x80, 0xE0, 
;    1094 	0x00, 0xA0, 0x00, 0x00, 0xA0, 0x20, 0x00, 0xA0, 0x40, 0x00, 0xA0, 0x60, 
;    1095 	0x00, 0xA0, 0x80, 0x00, 0xA0, 0xA0, 0x00, 0xA0, 0xC0, 0x00, 0xA0, 0xE0, 
;    1096 
;    1097 	0x00, 0xC0, 0x00, 0x00, 0xC0, 0x20, 0x00, 0xC0, 0x40, 0x00, 0xC0, 0x60, 
;    1098 	0x00, 0xC0, 0x80, 0x00, 0xC0, 0xA0, 0x00, 0xC0, 0xC0, 0x00, 0xC0, 0xE0, 
;    1099 	0x00, 0xE0, 0x00, 0x00, 0xE0, 0x20, 0x00, 0xE0, 0x40, 0x00, 0xE0, 0x60, 
;    1100 	0x00, 0xE0, 0x80, 0x00, 0xE0, 0xA0, 0x00, 0xE0, 0xC0, 0x00, 0xE0, 0xE0, 
;    1101 
;    1102 	0x40, 0x00, 0x00, 0x40, 0x00, 0x20, 0x40, 0x00, 0x40, 0x40, 0x00, 0x60, 
;    1103 	0x40, 0x00, 0x80, 0x40, 0x00, 0xA0, 0x40, 0x00, 0xC0, 0x40, 0x00, 0xE0, 
;    1104 	0x40, 0x20, 0x00, 0x40, 0x20, 0x20, 0x40, 0x20, 0x40, 0x40, 0x20, 0x60, 
;    1105 	0x40, 0x20, 0x80, 0x40, 0x20, 0xA0, 0x40, 0x20, 0xC0, 0x40, 0x20, 0xE0, 
;    1106 
;    1107 	0x40, 0x40, 0x00, 0x40, 0x40, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 
;    1108 	0x40, 0x40, 0x80, 0x40, 0x40, 0xA0, 0x40, 0x40, 0xC0, 0x40, 0x40, 0xE0, 
;    1109 	0x40, 0x60, 0x00, 0x40, 0x60, 0x20, 0x40, 0x60, 0x40, 0x40, 0x60, 0x60, 
;    1110 	0x40, 0x60, 0x80, 0x40, 0x60, 0xA0, 0x40, 0x60, 0xC0, 0x40, 0x60, 0xE0, 
;    1111 	
;    1112 	0x40, 0x80, 0x00, 0x40, 0x80, 0x20, 0x40, 0x80, 0x40, 0x40, 0x80, 0x60, 
;    1113 	0x40, 0x80, 0x80, 0x40, 0x80, 0xA0, 0x40, 0x80, 0xC0, 0x40, 0x80, 0xE0, 
;    1114 	0x40, 0xA0, 0x00, 0x40, 0xA0, 0x20, 0x40, 0xA0, 0x40, 0x40, 0xA0, 0x60, 
;    1115 	0x40, 0xA0, 0x80, 0x40, 0xA0, 0xA0, 0x40, 0xA0, 0xC0, 0x40, 0xA0, 0xE0, 
;    1116 	
;    1117 	0x40, 0xC0, 0x00, 0x40, 0xC0, 0x20, 0x40, 0xC0, 0x40, 0x40, 0xC0, 0x60, 
;    1118 	0x40, 0xC0, 0x80, 0x40, 0xC0, 0xA0, 0x40, 0xC0, 0xC0, 0x40, 0xC0, 0xE0, 
;    1119 	0x40, 0xE0, 0x00, 0x40, 0xE0, 0x20, 0x40, 0xE0, 0x40, 0x40, 0xE0, 0x60, 
;    1120 	0x40, 0xE0, 0x80, 0x40, 0xE0, 0xA0, 0x40, 0xE0, 0xC0, 0x40, 0xE0, 0xE0, 
;    1121 	
;    1122 	0x80, 0x00, 0x00, 0x80, 0x00, 0x20, 0x80, 0x00, 0x40, 0x80, 0x00, 0x60, 
;    1123 	0x80, 0x00, 0x80, 0x80, 0x00, 0xA0, 0x80, 0x00, 0xC0, 0x80, 0x00, 0xE0, 
;    1124 	0x80, 0x20, 0x00, 0x80, 0x20, 0x20, 0x80, 0x20, 0x40, 0x80, 0x20, 0x60, 
;    1125 	0x80, 0x20, 0x80, 0x80, 0x20, 0xA0, 0x80, 0x20, 0xC0, 0x80, 0x20, 0xE0, 
;    1126 	
;    1127 	0x80, 0x40, 0x00, 0x80, 0x40, 0x20, 0x80, 0x40, 0x40, 0x80, 0x40, 0x60, 
;    1128 	0x80, 0x40, 0x80, 0x80, 0x40, 0xA0, 0x80, 0x40, 0xC0, 0x80, 0x40, 0xE0, 
;    1129 	0x80, 0x60, 0x00, 0x80, 0x60, 0x20, 0x80, 0x60, 0x40, 0x80, 0x60, 0x60, 
;    1130 	0x80, 0x60, 0x80, 0x80, 0x60, 0xA0, 0x80, 0x60, 0xC0, 0x80, 0x60, 0xE0, 
;    1131 	
;    1132 	0x80, 0x80, 0x00, 0x80, 0x80, 0x20, 0x80, 0x80, 0x40, 0x80, 0x80, 0x60, 
;    1133 	0x80, 0x80, 0x80, 0x80, 0x80, 0xA0, 0x80, 0x80, 0xC0, 0x80, 0x80, 0xE0, 
;    1134 	0x80, 0xA0, 0x00, 0x80, 0xA0, 0x20, 0x80, 0xA0, 0x40, 0x80, 0xA0, 0x60, 
;    1135 	0x80, 0xA0, 0x80, 0x80, 0xA0, 0xA0, 0x80, 0xA0, 0xC0, 0x80, 0xA0, 0xE0, 
;    1136 
;    1137 	0x80, 0xC0, 0x00, 0x80, 0xC0, 0x20, 0x80, 0xC0, 0x40, 0x80, 0xC0, 0x60, 
;    1138 	0x80, 0xC0, 0x80, 0x80, 0xC0, 0xA0, 0x80, 0xC0, 0xC0, 0x80, 0xC0, 0xE0, 
;    1139 	0x80, 0xE0, 0x00, 0x80, 0xE0, 0x20, 0x80, 0xE0, 0x40, 0x80, 0xE0, 0x60, 
;    1140 	0x80, 0xE0, 0x80, 0x80, 0xE0, 0xA0, 0x80, 0xE0, 0xC0, 0x80, 0xE0, 0xE0, 
;    1141 
;    1142 	0xC0, 0x00, 0x00, 0xC0, 0x00, 0x20, 0xC0, 0x00, 0x40, 0xC0, 0x00, 0x60, 
;    1143 	0xC0, 0x00, 0x80, 0xC0, 0x00, 0xA0, 0xC0, 0x00, 0xC0, 0xC0, 0x00, 0xE0, 
;    1144 	0xC0, 0x20, 0x00, 0xC0, 0x20, 0x20, 0xC0, 0x20, 0x40, 0xC0, 0x20, 0x60, 
;    1145 	0xC0, 0x20, 0x80, 0xC0, 0x20, 0xA0, 0xC0, 0x20, 0xC0, 0xC0, 0x20, 0xE0, 
;    1146 
;    1147 	0xC0, 0x40, 0x00, 0xC0, 0x40, 0x20, 0xC0, 0x40, 0x40, 0xC0, 0x40, 0x60, 
;    1148 	0xC0, 0x40, 0x80, 0xC0, 0x40, 0xA0, 0xC0, 0x40, 0xC0, 0xC0, 0x40, 0xE0, 
;    1149 	0xC0, 0x60, 0x00, 0xC0, 0x60, 0x20, 0xC0, 0x60, 0x40, 0xC0, 0x60, 0x60, 
;    1150 	0xC0, 0x60, 0x80, 0xC0, 0x60, 0xA0, 0xC0, 0x60, 0xC0, 0xC0, 0x60, 0xE0, 
;    1151 
;    1152 	0xC0, 0x80, 0x00, 0xC0, 0x80, 0x20, 0xC0, 0x80, 0x40, 0xC0, 0x80, 0x60, 
;    1153 	0xC0, 0x80, 0x80, 0xC0, 0x80, 0xA0, 0xC0, 0x80, 0xC0, 0xC0, 0x80, 0xE0, 
;    1154 	0xC0, 0xA0, 0x00, 0xC0, 0xA0, 0x20, 0xC0, 0xA0, 0x40, 0xC0, 0xA0, 0x60, 
;    1155 	0xC0, 0xA0, 0x80, 0xC0, 0xA0, 0xA0, 0xC0, 0xA0, 0xC0, 0xC0, 0xA0, 0xE0, 
;    1156 
;    1157 	0xC0, 0xC0, 0x00, 0xC0, 0xC0, 0x20, 0xC0, 0xC0, 0x40, 0xC0, 0xC0, 0x60, 
;    1158 	0xC0, 0xC0, 0x80, 0xC0, 0xC0, 0xA0, 0xF0, 0xFB, 0xFF, 0xA4, 0xA0, 0xA0, 
;    1159 	0x80, 0x80, 0x80, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 
;    1160 	0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF 
;    1161 };
;    1162 //*************************************
;    1163 
;    1164 /**************************************
;    1165         LUT TABEL PIC 16 Colors  
;    1166 BMP has not RGB value but BGR in function:
;    1167 S1D13705_LUT_16_BMP, the red and blue
;    1168 are switch automaticly!
;    1169 **************************************/
;    1170 flash unsigned char LUT_16_BMP[16*3] =
;    1171 {    
;    1172 	0x00, 0x00, 0x00,  0x00, 0x00, 0x80,   0x00, 0x80, 0x00,  0x00, 0x80, 0x80, 
;    1173 	0x80, 0x00, 0x00,  0x80, 0x00, 0x80,   0x80, 0x80, 0x00,  0x80, 0x80, 0x80, 
;    1174 	0xC0, 0xC0, 0xC0,  0x00, 0x00, 0xF0,   0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0, 
;    1175 	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,   0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
;    1176 };
;    1177 //*************************************
;    1178                                         
;    1179 /**************************************
;    1180 Functie: Activeer LUT 16 colors
;    1181 **************************************/
;    1182 void S1D13705_LUT_16(void)
;    1183 {
;    1184         flash unsigned char *pLUT;
;    1185         unsigned char i;
;    1186         
;    1187         //Stel LUT tabel informatie in
;    1188         S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
;	*pLUT -> R16,R17
;	i -> R18
;    1189         
;    1190         pLUT = LUT_16;                                                  //pLUT wijst naar de LUT array 
;    1191         for(i=0;i<16;i++)
;    1192         {
;    1193                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
;    1194                 pLUT++;        
;    1195                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
;    1196                 pLUT++;        
;    1197                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
;    1198                 pLUT++;                                                   
;    1199         }
;    1200         
;    1201         return;
;    1202 }
;    1203 //*************************************
;    1204 
;    1205 /**************************************
;    1206 Functie: Activeer LUT 256 colors
;    1207 **************************************/
;    1208 void S1D13705_LUT_256(void)
;    1209 {
_S1D13705_LUT_256:
;    1210         flash unsigned char *pLUT;
;    1211         unsigned int i;
;    1212         
;    1213         //Stel LUT tabel informatie in
;    1214         S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
	CALL __SAVELOCR4
;	*pLUT -> R16,R17
;	i -> R18,R19
	CALL SUBOPT_0x1E
;    1215         
;    1216         pLUT = LUT_256;                                                 //pLUT wijst naar de LUT array 
	LDI  R30,LOW(_LUT_256*2)
	LDI  R31,HIGH(_LUT_256*2)
	__PUTW1R 16,17
;    1217         for(i=0;i<256;i++)
	__GETWRN 18,19,0
_0xBB:
	__CPWRN 18,19,256
	BRSH _0xBC
;    1218         {
;    1219                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
	CALL SUBOPT_0x1F
;    1220                 pLUT++;        
	__ADDWRN 16,17,1
;    1221                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
	CALL SUBOPT_0x1F
;    1222                 pLUT++;        
	__ADDWRN 16,17,1
;    1223                 S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
	CALL SUBOPT_0x1F
;    1224                 pLUT++;                                                   
	__ADDWRN 16,17,1
;    1225         }
	__ADDWRN 18,19,1
	RJMP _0xBB
_0xBC:
;    1226         
;    1227         return;
	CALL __LOADLOCR4
	ADIW R28,4
	RET
;    1228 }
;    1229 //*************************************
;    1230 
;    1231 /**************************************
;    1232 Functie: Write LUT table 16 colors for
;    1233          BMP. LUT table BMP begins with
;    1234          the blue color and not red!
;    1235          LUT BMP is BGR, S1D13705 LUT is RGB!
;    1236 **************************************/
;    1237 void S1D13705_LUT_16_BMP(void)
;    1238 {
;    1239         flash unsigned char *pLUT;
;    1240         unsigned char red;
;    1241         unsigned char green;
;    1242         unsigned char blue;
;    1243         unsigned char i;
;    1244         
;    1245         //Set first the LUT adress at 0
;    1246         S1D13705_Write_Register(0x15,0);                                
;	*pLUT -> R16,R17
;	red -> R18
;	green -> R19
;	blue -> R20
;	i -> R21
;    1247         
;    1248         pLUT = LUT_16_BMP;                                              //pLUT points to LUT_16_BMP
;    1249         
;    1250         //Write LUT values to LUT register        
;    1251         for(i=0;i<16;i++)
;    1252         {
;    1253                 //Read color values from LUT table
;    1254                 blue = *pLUT++;                                         //Read the blue value from LUT table
;    1255                 green = *pLUT++;                                        //Read the green value from LUT table
;    1256                 red = *pLUT++;                                          //Read the red value from LUT table
;    1257                 
;    1258                 //Write color values to LUT register S1D13705
;    1259                 S1D13705_Write_Register(0x17,red);                      //Write red value
;    1260                 S1D13705_Write_Register(0x17,green);                    //Write green value
;    1261                 S1D13705_Write_Register(0x17,blue);                     //Write blue value
;    1262         }
;    1263         
;    1264         return;
;    1265 }
;    1266 //*************************************
;    1267 
;    1268 /**************************************
;    1269 Functie: Write LUT tabel 256 colors for
;    1270          BMP, LUT table BMP begins with
;    1271          the blue color and not red!
;    1272          LUT BMP is BGR, S1D13705 LUT is RGB!
;    1273 **************************************/
;    1274 void S1D13705_LUT_256_BMP(void)
;    1275 {
;    1276         flash unsigned char *pLUT;
;    1277         unsigned char red;
;    1278         unsigned char green;
;    1279         unsigned char blue;
;    1280         unsigned int i;
;    1281         
;    1282         //Set first the LUT adress at 0
;    1283         S1D13705_Write_Register(0x15,0);                                
;	*pLUT -> R16,R17
;	red -> R18
;	green -> R19
;	blue -> R20
;	i -> Y+5
;    1284         
;    1285         pLUT = LUT_256_BMP;                                             //pLUT points to LUT_16_BMP
;    1286         
;    1287         //Schrijf LUT waardes, let erop
;    1288         for(i=0;i<256;i++)
;    1289         {
;    1290                 //Read color values from LUT table
;    1291                 blue = *pLUT++;                                         //Read the blue value from LUT table                                    
;    1292                 green = *pLUT++;                                        //Read the green value from LUT table
;    1293                 red = *pLUT++;                                          //Read the red value from LUT table
;    1294                 
;    1295                 //Write color values to LUT register S1D13705
;    1296                 S1D13705_Write_Register(0x17,red);                      //Write red value  
;    1297                 S1D13705_Write_Register(0x17,green);                    //Write green value
;    1298                 S1D13705_Write_Register(0x17,blue);                     //Write blue value  
;    1299         }
;    1300         
;    1301         return;
;    1302 }
;    1303 //************************************* 
;    1304         
;    1305 /*********************************************************
;    1306                 COMPACT FLASH DRIVER
;    1307 
;    1308 Versie informatie:
;    1309 ----------------------------------------------------------
;    1310 Versie 0.1:
;    1311 - Beta versie.
;    1312 
;    1313 ----------------------------------------------------------
;    1314 
;    1315 *********************************************************/
;    1316 
;    1317 //Include DEFS voor CF
;    1318 
;    1319 
;    1320 //**************************************
;    1321       
;    1322 
;    1323 //CF DEFS
;    1324 //Eerst de poorten, dit hoef dan alleen maar verander te worden
;    1325 #define CF_WriteData    PORTA
;    1326 #define CF_ReadData     PINA
;    1327 
;    1328 #define CF_DataPort_Out DDRA = 0xFF
;    1329 #define CF_DataPort_In  DDRA = 0x00  
;    1330 
;    1331 #define CF_CE1          PORTB.0                         
;    1332 #define CF_A2           PORTC.0
;    1333 #define CF_A1           PORTC.1
;    1334 #define CF_A0           PORTC.2
;    1335 
;    1336 #define CF_WR           PORTD.6                 
;    1337 #define CF_RD           PORTD.7                                
;    1338 
;    1339 //Besturing CF
;    1340 #define CF_ON           CF_CE1 = 0  
;    1341 #define CF_OFF          CF_CE1 = 1
;    1342 #define CF_Read_ON      CF_RD = 0
;    1343 #define CF_Read_OFF     CF_RD = 1
;    1344 #define CF_Write_ON     CF_WR = 0
;    1345 #define CF_Write_OFF    CF_WR = 1
;    1346 
;    1347 //Offset 
;    1348 #define CF_OffSet_ReadData              (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 0)
;    1349 #define CF_OffSet_WriteData             (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 0)
;    1350 #define CF_OffSet_Error                 (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 1)
;    1351 #define CF_OffSet_Features              (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 1)
;    1352 #define CF_OffSet_Sector_CNT            (CF_A2 = 0) | (CF_A1 = 1) | (CF_A0 = 0)
;    1353 #define CF_OffSet_Secotor_NO            (CF_A2 = 0) | (CF_A1 = 1) | (CF_A0 = 1)
;    1354 #define CF_OffSet_Cylinder_Low          (CF_A2 = 1) | (CF_A1 = 0) | (CF_A0 = 0)     
;    1355 #define CF_OffSet_Cylinder_High         (CF_A2 = 1) | (CF_A1 = 0) | (CF_A0 = 1)
;    1356 #define CF_OffSet_Head                  (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 0)       
;    1357 #define CF_OffSet_Status                (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 1)
;    1358 #define CF_OffSet_Command               (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 1)
;    1359          
;    1360 //CF-ATA commando's         
;    1361 #define CF_CMD_Erase_Sector     0xC0
;    1362 #define CF_CMD_ID               0xEC
;    1363 #define CF_CMD_Read_Sector      0x20
;    1364 #define CF_CMD_Write_Sector     0x30
;    1365 //**************************************
;    1366 
;    1367 /**************************************
;    1368 CF INIT
;    1369 I/O poorten instellen van de CF
;    1370 **************************************/
;    1371 void CF_Init(void)
;    1372 {
_CF_Init:
;    1373         //Zet CF, lezen en schrijven uit
;    1374         CF_OFF;                                         
	SBI  0x18,0
;    1375         CF_Read_OFF;
	SBI  0x12,7
;    1376         CF_Write_OFF;                   
	SBI  0x12,6
;    1377         
;    1378         //Zet CF offset op 0
;    1379         CF_OffSet_ReadData;
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	CBI  0x15,2
	OR   R30,R26
;    1380         
;    1381         return;
	RET
;    1382 }
;    1383 //*************************************
;    1384 
;    1385 /**************************************
;    1386 CF Wait Ready
;    1387 lees of CF is ready voor actie
;    1388 **************************************/
;    1389 void CF_Wait_RDY(void)
;    1390 {
_CF_Wait_RDY:
;    1391         unsigned char CF_Register;
;    1392         
;    1393         //Lees de register van de CF en check of
;    1394         //de ready bit is set
;    1395         while(1)
	ST   -Y,R16
;	CF_Register -> R16
_0xC3:
;    1396         {
;    1397                // Check_Data_Ready();
;    1398                 
;    1399                 CF_Register = CF_Read_Register(7);      //Offset 7
	CALL SUBOPT_0x20
;    1400                 CF_Register = CF_Register & 0x40;       //Check alleen RDY Bit
	ANDI R16,LOW(64)
;    1401                 
;    1402                 if(CF_Register == 0x40)                 //Als RDY bit is 1 dan verlaat loop
	CPI  R16,64
	BREQ _0xC5
;    1403                         break;
;    1404         }  
	RJMP _0xC3
_0xC5:
;    1405 
;    1406         return;
	RJMP _0x246
;    1407 }
;    1408 //************************************* 
;    1409 
;    1410 /**************************************
;    1411 CF Wait Data Request
;    1412 wacht totdat de DRQ bit is set
;    1413 **************************************/
;    1414 void CF_Wait_DRQ(void)
;    1415 {
_CF_Wait_DRQ:
;    1416         unsigned char CF_Register;
;    1417         
;    1418         //Lees de register van de CF en
;    1419         //check of de DRQ bit is set
;    1420         while(1)
	ST   -Y,R16
;	CF_Register -> R16
_0xC7:
;    1421         {
;    1422          //       Check_Data_Ready();
;    1423                
;    1424                 CF_Register = CF_Read_Register(7);      //Offset 7
	CALL SUBOPT_0x20
;    1425                 CF_Register = CF_Register & 0x08;       //Check alleen DRQ Bit
	ANDI R16,LOW(8)
;    1426                 
;    1427                 if(CF_Register == 0x08)                 //Als DRQ bit is 1, verlaat loop
	CPI  R16,8
	BREQ _0xC9
;    1428                         break;
;    1429         }
	RJMP _0xC7
_0xC9:
;    1430         
;    1431         return;
_0x246:
	LD   R16,Y+
	RET
;    1432 }
;    1433 //*************************************
;    1434 
;    1435 /**************************************
;    1436 CF Error register
;    1437 lees de error register van de CF
;    1438 **************************************/
;    1439 unsigned char CF_Error_Reg(void)
;    1440 {
;    1441         unsigned char CF_Register;
;    1442         
;    1443         //Lees de error register van de CF
;    1444         CF_Register = CF_Read_Register(1);              //Offset 1
;	CF_Register -> R16
;    1445         
;    1446         return (CF_Register);
;    1447 }
;    1448 //*************************************
;    1449 
;    1450 /**************************************
;    1451 CF Write registers
;    1452 schrijf naar de register van de CF
;    1453 **************************************/
;    1454 void CF_Write_Register(unsigned char Reg_Adres, unsigned char Reg_Value)
;    1455 {
_CF_Write_Register:
;    1456         CF_DataPort_Out;                                //PORTx instellen als output
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;    1457         CF_Write_OFF;                                   //Zet CF schrijven uit
	SBI  0x12,6
;    1458         
;    1459         
;    1460         //Bepaal de offset instelling
;    1461         switch(Reg_Adres)
	LDD  R30,Y+1
;    1462         {
;    1463                 case 0:
	CPI  R30,0
	BRNE _0xCE
;    1464                         CF_OffSet_WriteData;            //Offset voor schrijven van data
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	CBI  0x15,2
	RJMP _0x24A
;    1465                         break;
;    1466                 case 1:
_0xCE:
	CPI  R30,LOW(0x1)
	BRNE _0xCF
;    1467                         CF_OffSet_Features;             //Offset voor schrijven van features
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	SBI  0x15,2
	RJMP _0x24B
;    1468                         break;
;    1469                 case 2:
_0xCF:
	CPI  R30,LOW(0x2)
	BRNE _0xD0
;    1470                         CF_OffSet_Sector_CNT;           //Offset voor schrijven van de sector teller 
	CBI  0x15,0
	SBI  0x15,1
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24A
;    1471                         break;
;    1472                 case 3:
_0xD0:
	CPI  R30,LOW(0x3)
	BRNE _0xD1
;    1473                         CF_OffSet_Secotor_NO;           //Offset voor schrijven van de sector nummer
	CBI  0x15,0
	SBI  0x15,1
	LDI  R30,LOW(1)
	MOV  R26,R30
	SBI  0x15,2
	RJMP _0x24B
;    1474                         break;
;    1475                 case 4:
_0xD1:
	CPI  R30,LOW(0x4)
	BRNE _0xD2
;    1476                         CF_OffSet_Cylinder_Low;         //Offset voor schrijven van de cylinder laag
	SBI  0x15,0
	CBI  0x15,1
	CALL SUBOPT_0x21
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24A
;    1477                         break;
;    1478                 case 5:
_0xD2:
	CPI  R30,LOW(0x5)
	BRNE _0xD3
;    1479                         CF_OffSet_Cylinder_High;        //Offset voor schrijven van de cylinder hoog
	SBI  0x15,0
	CBI  0x15,1
	CALL SUBOPT_0x21
	SBI  0x15,2
	RJMP _0x24B
;    1480                         break;
;    1481                 case 6:
_0xD3:
	CPI  R30,LOW(0x6)
	BRNE _0xD4
;    1482                         CF_OffSet_Head;                 //Offset voor schrijven van de head of select card
	SBI  0x15,0
	SBI  0x15,1
	CALL SUBOPT_0x22
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24A
;    1483                         break;
;    1484                 case 7:
_0xD4:
	CPI  R30,LOW(0x7)
	BRNE _0xCD
;    1485                         CF_OffSet_Command;              //Offset voor schrijven van de commando
	SBI  0x15,0
	SBI  0x15,1
	CALL SUBOPT_0x22
	SBI  0x15,2
_0x24B:
	LDI  R30,LOW(1)
_0x24A:
	OR   R30,R26
;    1486                         break;                                                                 
;    1487         }
_0xCD:
;    1488                 
;    1489         CF_WriteData = Reg_Value;                       //Zet register waarde op de data port 
	LD   R30,Y
	OUT  0x1B,R30
;    1490         CF_Write_ON;                                    //Zet CF schrijven aan
	CBI  0x12,6
;    1491         
;    1492         #asm("nop");                                    //Wacht 4 klokcycles
	nop
;    1493         #asm("nop");
	nop
;    1494         #asm("nop");
	nop
;    1495         #asm("nop");                                                        
	nop
;    1496         
;    1497         CF_Write_OFF;                                   //Zet CF schrijven uit
	SBI  0x12,6
;    1498         
;    1499 
;    1500         return;
	ADIW R28,2
	RET
;    1501 }
;    1502 //*************************************
;    1503 
;    1504 /**************************************
;    1505 CF Read registers
;    1506 lees de waarde van een register van de CF
;    1507 **************************************/
;    1508 unsigned char CF_Read_Register(unsigned char Reg_Adres)
;    1509 {
_CF_Read_Register:
;    1510         unsigned char CF_Data;
;    1511         
;    1512         CF_DataPort_In;                                 //PORTx instellen als input
	ST   -Y,R16
;	Reg_Adres -> Y+1
;	CF_Data -> R16
	LDI  R30,LOW(0)
	OUT  0x1A,R30
;    1513         CF_Write_OFF;                                   //CF schrijven uit
	SBI  0x12,6
;    1514         CF_Read_OFF;                                    //CF lezen uit
	SBI  0x12,7
;    1515         
;    1516         //Bepaal de offset instelling
;    1517         switch(Reg_Adres)
	LDD  R30,Y+1
;    1518         {
;    1519                 case 0:
	CPI  R30,0
	BRNE _0xD9
;    1520                         CF_OffSet_ReadData;             //Offset voor lezen van data
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	CBI  0x15,2
	RJMP _0x24C
;    1521                         break;
;    1522                 case 1:
_0xD9:
	CPI  R30,LOW(0x1)
	BRNE _0xDA
;    1523                         CF_OffSet_Error;                //Offset voor lezen van errors
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	SBI  0x15,2
	RJMP _0x24D
;    1524                         break;
;    1525                 case 2:
_0xDA:
	CPI  R30,LOW(0x2)
	BRNE _0xDB
;    1526                         CF_OffSet_Sector_CNT;           //Offset voor lezen van de sector teller 
	CBI  0x15,0
	SBI  0x15,1
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24C
;    1527                         break;
;    1528                 case 3:
_0xDB:
	CPI  R30,LOW(0x3)
	BRNE _0xDC
;    1529                         CF_OffSet_Secotor_NO;           //Offset voor lezen van de sector nummer
	CBI  0x15,0
	SBI  0x15,1
	LDI  R30,LOW(1)
	MOV  R26,R30
	SBI  0x15,2
	RJMP _0x24D
;    1530                         break;
;    1531                 case 4:
_0xDC:
	CPI  R30,LOW(0x4)
	BRNE _0xDD
;    1532                         CF_OffSet_Cylinder_Low;         //Offset voor lezen van de cylinder laag
	SBI  0x15,0
	CBI  0x15,1
	CALL SUBOPT_0x21
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24C
;    1533                         break;
;    1534                 case 5:
_0xDD:
	CPI  R30,LOW(0x5)
	BRNE _0xDE
;    1535                         CF_OffSet_Cylinder_High;        //Offset voor lezen van de cylinder hoog
	SBI  0x15,0
	CBI  0x15,1
	CALL SUBOPT_0x21
	SBI  0x15,2
	RJMP _0x24D
;    1536                         break;
;    1537                 case 6:
_0xDE:
	CPI  R30,LOW(0x6)
	BRNE _0xDF
;    1538                         CF_OffSet_Head;                 //Offset voor lezen van de head of select card
	SBI  0x15,0
	SBI  0x15,1
	CALL SUBOPT_0x22
	CBI  0x15,2
	LDI  R30,LOW(0)
	RJMP _0x24C
;    1539                         break;
;    1540                 case 7:
_0xDF:
	CPI  R30,LOW(0x7)
	BRNE _0xD8
;    1541                         CF_OffSet_Status;               //Offset voor lezen van de status
	SBI  0x15,0
	SBI  0x15,1
	CALL SUBOPT_0x22
	SBI  0x15,2
_0x24D:
	LDI  R30,LOW(1)
_0x24C:
	OR   R30,R26
;    1542                         break;                                                                 
;    1543         }
_0xD8:
;    1544 
;    1545         #asm("nop");                                    //Wacht 1 klokcycle
	nop
;    1546         
;    1547         CF_Read_ON;                                     //CF lezen aan
	CBI  0x12,7
;    1548         
;    1549         //delay_us(2);                                    //Wacht even voordat data wordt gelezen
;    1550         #asm("nop");                                    //Wacht 1 klokcycle
	nop
;    1551         #asm("nop");                                    //Wacht 1 klokcycle
	nop
;    1552         #asm("nop");                                    //Wacht 1 klokcycle
	nop
;    1553         
;    1554         CF_Data =  PINA; //CF_ReadData;                         //Lees de data van de CF
	IN   R16,25
;    1555         CF_Read_OFF;                                    //CF lezen uit
	SBI  0x12,7
;    1556 
;    1557         return(CF_Data);
	MOV  R30,R16
	LDD  R16,Y+0
	ADIW R28,2
	RET
;    1558 }
;    1559 //*************************************  
;    1560 
;    1561 
;    1562 /**************************************
;    1563 CF Read Sector
;    1564 lees een sector van de CF
;    1565 **************************************/
;    1566 //void CF_Read_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head)
;    1567 void CF_Read_Sector(unsigned long int Sector)
;    1568 {
_CF_Read_Sector:
;    1569         unsigned int Bytes;
;    1570         unsigned char Sector_Addr[3];
;    1571         
;    1572         CF_ON;                                          //Zet CF aan
	CALL SUBOPT_0x23
;	Sector -> Y+5
;	Bytes -> R16,R17
;	Sector_Addr -> Y+2
	CBI  0x18,0
;    1573         
;    1574         CF_Wait_RDY();                                  //Wacht op CF tot RDY is set
	CALL SUBOPT_0x24
;    1575         
;    1576         //Bereken de sector die gelezen moet worden
;    1577         Sector_Addr[0] = Sector & 0x000000FF;
;    1578         Sector_Addr[1] = ((Sector & 0x0000FF00) >> 8);
;    1579         Sector_Addr[2] = ((Sector & 0x00FF0000) >> 16); 
;    1580         
;    1581         //Zend Sector Count, offset = 2
;    1582         CF_Write_Register(2,0x01);
;    1583         
;    1584         //Zend Sector No, offset = 3
;    1585         //CF_Write_Register(3,Sector_No);
;    1586         CF_Write_Register(3,Sector_Addr[0]);
;    1587 
;    1588         //Zend Cylinder Low, offset = 4
;    1589         //CF_Write_Register(4,Cylinder_Low);
;    1590         CF_Write_Register(4,Sector_Addr[1]);
;    1591         
;    1592         //Zend Cylinder High, offset = 5 
;    1593         //CF_Write_Register(5,Cylinder_High);
;    1594         CF_Write_Register(5,Sector_Addr[2]);
;    1595         
;    1596         //Zend Select Card / Head, offset = 6. Normaal 0xE0
;    1597         CF_Write_Register(6,0xE0);
;    1598         
;    1599         //Zend Commando Read Sector, offset = 7
;    1600         CF_Write_Register(7,CF_CMD_Read_Sector);
	LDI  R30,LOW(32)
	CALL SUBOPT_0x25
;    1601         
;    1602         CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
;    1603         
;    1604         CF_OffSet_ReadData;                             //Offset op 0, lezen data
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	CBI  0x15,2
	OR   R30,R26
;    1605         CF_DataPort_In;                                 //PORTx als input
	LDI  R30,LOW(0)
	OUT  0x1A,R30
;    1606        
;    1607         //Inlezen van de sector, dat is 512bytes
;    1608         for(Bytes=0; Bytes<512; Bytes++)
	__GETWRN 16,17,0
_0xE2:
	__CPWRN 16,17,512
	BRSH _0xE3
;    1609         {
;    1610                 CF_Read_OFF;                            //CF lezen uit
	SBI  0x12,7
;    1611              
;    1612                 #asm("nop");                            //2x NOP
	nop
;    1613              //   #asm("nop");                            
;    1614                 
;    1615                 CF_Read_ON;                             //CF lezen aan  
	CBI  0x12,7
;    1616                 
;    1617                 #asm("nop");                            //3x NOP
	nop
;    1618             //    #asm("nop");                            
;    1619             //    #asm("nop");                            
;    1620                 
;    1621                 CF_Sector_Data[Bytes] = CF_ReadData;    //Lees data van CF en sla op in buffer
	__GETW2R 16,17
	SUBI R26,LOW(-_CF_Sector_Data)
	SBCI R27,HIGH(-_CF_Sector_Data)
	IN   R30,0x19
	ST   X,R30
;    1622                 CF_Read_OFF;                            //CF lezen uit
	SBI  0x12,7
;    1623         }
	__ADDWRN 16,17,1
	RJMP _0xE2
_0xE3:
;    1624         
;    1625         //Lezen voltooid, zet CF uit
;    1626         CF_OFF;
	SBI  0x18,0
;    1627         
;    1628         return;
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,9
	RET
;    1629 }
;    1630 //*************************************
;    1631 
;    1632 /**************************************
;    1633 CF Write Sector
;    1634 schrijven van een sector naar de CF
;    1635 **************************************/
;    1636 //void CF_Write_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head)
;    1637 void CF_Write_Sector(unsigned long int Sector)
;    1638 {
_CF_Write_Sector:
;    1639         unsigned int Bytes;
;    1640         unsigned char Sector_Addr[3];
;    1641 
;    1642         CF_ON;                                          //Zet CF aan
	CALL SUBOPT_0x23
;	Sector -> Y+5
;	Bytes -> R16,R17
;	Sector_Addr -> Y+2
	CBI  0x18,0
;    1643         
;    1644         CF_Wait_RDY();                                  //Wacht op CF tot RDY is set
	CALL SUBOPT_0x24
;    1645         
;    1646          //Bereken de sector die gelezen moet worden
;    1647         Sector_Addr[0] = Sector & 0x000000FF;
;    1648         Sector_Addr[1] = ((Sector & 0x0000FF00) >> 8);
;    1649         Sector_Addr[2] = ((Sector & 0x00FF0000) >> 16); 
;    1650         
;    1651         //Zend Sector Count, offset = 2
;    1652         CF_Write_Register(2,0x01);
;    1653         
;    1654         //Zend Sector No, offset = 3
;    1655         //CF_Write_Register(3,Sector_No);
;    1656         CF_Write_Register(3,Sector_Addr[0]);
;    1657 
;    1658         //Zend Cylinder Low, offset = 4
;    1659         //CF_Write_Register(4,Cylinder_Low);
;    1660         CF_Write_Register(4,Sector_Addr[1]);
;    1661         
;    1662         //Zend Cylinder High, offset = 5 
;    1663         //CF_Write_Register(5,Cylinder_High);
;    1664         CF_Write_Register(5,Sector_Addr[2]);
;    1665         
;    1666         //Zend Select Card / Head, offset = 6. Normaal 0xE0
;    1667         CF_Write_Register(6,0xE0);
;    1668         
;    1669         //Zend Commando Write Sector, offset = 7
;    1670         CF_Write_Register(7,CF_CMD_Write_Sector);
	LDI  R30,LOW(48)
	CALL SUBOPT_0x25
;    1671         
;    1672         CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
;    1673         
;    1674         CF_OffSet_WriteData;                            //Offset op 0, schrijven van data
	CBI  0x15,0
	CBI  0x15,1
	LDI  R30,LOW(0)
	MOV  R26,R30
	CBI  0x15,2
	OR   R30,R26
;    1675         CF_DataPort_Out;                                //PORTx als output
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;    1676         
;    1677         //Schrijf sector, 512bytes naar de CF 
;    1678         for(Bytes=0; Bytes<512; Bytes++)
	__GETWRN 16,17,0
_0xE5:
	__CPWRN 16,17,512
	BRSH _0xE6
;    1679         {
;    1680                 CF_Write_OFF;                           //CF schrijven uit
	SBI  0x12,6
;    1681                 
;    1682                 #asm("nop");                            //2x nop
	nop
;    1683                 #asm("nop");            
	nop
;    1684                 
;    1685                 CF_WriteData = CF_Sector_Data[Bytes];   //Zet de te schrijven data klaar voor de CF
	LDI  R26,LOW(_CF_Sector_Data)
	LDI  R27,HIGH(_CF_Sector_Data)
	ADD  R26,R16
	ADC  R27,R17
	LD   R30,X
	OUT  0x1B,R30
;    1686                 CF_Write_ON;                            //CF schrijven aan
	CBI  0x12,6
;    1687                 
;    1688                 #asm("nop");                            //3x nop
	nop
;    1689                 #asm("nop"); 
	nop
;    1690                 #asm("nop"); 
	nop
;    1691                 
;    1692                 CF_Write_OFF;                           //CF schrijven uit
	SBI  0x12,6
;    1693                 
;    1694         }
	__ADDWRN 16,17,1
	RJMP _0xE5
_0xE6:
;    1695         
;    1696         //Schrijven voltooid, zet CF uit
;    1697         CF_OFF;                                 
	SBI  0x18,0
;    1698         
;    1699         return;
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,9
	RET
;    1700 }
;    1701 //*************************************
;    1702 
;    1703 /**************************************
;    1704 CF Info
;    1705 lees informatie over de aangesloten CF
;    1706 **************************************/
;    1707 void CF_Info(void)
;    1708 {
;    1709         unsigned int Bytes;
;    1710                    
;    1711         CF_ON;                                          //Zet CF aan
;	Bytes -> R16,R17
;    1712         
;    1713         CF_Wait_RDY();                                  //Wacht op CF tot RDY is set
;    1714 
;    1715         //Zend 0 commande voor Head, offset = 6
;    1716         CF_Write_Register(6,0);
;    1717         
;    1718         //Zend Commando ID device, offset = 7
;    1719         CF_Write_Register(7,CF_CMD_ID);
;    1720          
;    1721         CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
;    1722         
;    1723         CF_OffSet_ReadData;                             //Offset op 0, lezen data
;    1724         CF_DataPort_In;                                 //PORTx als input
;    1725        
;    1726         //Inlezen van de info, dat is 512bytes
;    1727         for(Bytes=0; Bytes<512; Bytes++)
;    1728         {
;    1729                 CF_Read_OFF;                            //CF lezen uit
;    1730                 
;    1731                 #asm("nop");                            //2x NOP
;    1732                 #asm("nop");                            
;    1733                 
;    1734                 CF_Read_ON;                             //CF lezen aan  
;    1735                 
;    1736                 #asm("nop");                            //3x NOP
;    1737                 #asm("nop");                            
;    1738                 #asm("nop");                            
;    1739                 
;    1740                 CF_Sector_Data[Bytes] = CF_ReadData;    //Lees data van CF en sla op in buffer
;    1741                 CF_Read_OFF;                            //CF lezen uit
;    1742         }
;    1743         
;    1744         //Lezen info voltooid, zet CF uit
;    1745         CF_OFF;
;    1746         
;    1747         return;
;    1748 }
;    1749 //*************************************
;    1750         
;    1751         
;    1752                                                         
;    1753         
;    1754 /**************************************
;    1755              CF functions
;    1756 **************************************/
;    1757 
;    1758 /*************************************
;    1759         Include header file's                   
;    1760 *************************************/
;    1761 #include "LCD_Test_Defs.h"
;    1762 #include "S1D13705.h"          

	.DSEG
_aS1DRegs_G5:
_0aS1DRegs_G5:
	.BYTE 0x1
_1aS1DRegs_G5:
	.BYTE 0x1
_2aS1DRegs_G5:
	.BYTE 0x1
_3aS1DRegs_G5:
	.BYTE 0x1
_4aS1DRegs_G5:
	.BYTE 0x1
_5aS1DRegs_G5:
	.BYTE 0x1
_6aS1DRegs_G5:
	.BYTE 0x1
_7aS1DRegs_G5:
	.BYTE 0x1
_8aS1DRegs_G5:
	.BYTE 0x1
_9aS1DRegs_G5:
	.BYTE 0x1
_AaS1DRegs_G5:
	.BYTE 0x1
_BaS1DRegs_G5:
	.BYTE 0x1
_CaS1DRegs_G5:
	.BYTE 0x1
_DaS1DRegs_G5:
	.BYTE 0x1
_EaS1DRegs_G5:
	.BYTE 0x1
_FaS1DRegs_G5:
	.BYTE 0x1
_10aS1DRegs_G5:
	.BYTE 0x1
_11aS1DRegs_G5:
	.BYTE 0x1
_12aS1DRegs_G5:
	.BYTE 0x1
_13aS1DRegs_G5:
	.BYTE 0x1
_14aS1DRegs_G5:
	.BYTE 0x1
_15aS1DRegs_G5:
	.BYTE 0x1
_16aS1DRegs_G5:
	.BYTE 0x1
_17aS1DRegs_G5:
	.BYTE 0x1
_18aS1DRegs_G5:
	.BYTE 0x1
_19aS1DRegs_G5:
	.BYTE 0x1
_1AaS1DRegs_G5:
	.BYTE 0x1
_1BaS1DRegs_G5:
	.BYTE 0x1
_1CaS1DRegs_G5:
	.BYTE 0x1
_1DaS1DRegs_G5:
	.BYTE 0x1
_1EaS1DRegs_G5:
	.BYTE 0x1
_1FaS1DRegs_G5:
	.BYTE 0x1
_20aS1DRegs_G5:
	.BYTE 0x1
_21aS1DRegs_G5:
	.BYTE 0x1
_22aS1DRegs_G5:
	.BYTE 0x1
_23aS1DRegs_G5:
	.BYTE 0x1
_24aS1DRegs_G5:
	.BYTE 0x1
_25aS1DRegs_G5:
	.BYTE 0x1
_26aS1DRegs_G5:
	.BYTE 0x1
_27aS1DRegs_G5:
	.BYTE 0x1
_28aS1DRegs_G5:
	.BYTE 0x1
_29aS1DRegs_G5:
	.BYTE 0x1
_2AaS1DRegs_G5:
	.BYTE 0x1
_2BaS1DRegs_G5:
	.BYTE 0x1
_2CaS1DRegs_G5:
	.BYTE 0x1
_2DaS1DRegs_G5:
	.BYTE 0x1
_2EaS1DRegs_G5:
	.BYTE 0x1
_2FaS1DRegs_G5:
	.BYTE 0x1
_30aS1DRegs_G5:
	.BYTE 0x1
_31aS1DRegs_G5:
	.BYTE 0x1
;    1763 //*************************************     
;    1764 
;    1765 /**************************************
;    1766 Function: Read info CF and put on LCD
;    1767 **************************************/
;    1768 void CF_Read_Info(void)
;    1769 {

	.CSEG
;    1770         unsigned int i;
;    1771         unsigned int test[5];
;    1772         unsigned char d;   
;    1773 
;    1774         S1D13705_Clear_LCD();                                                   //Clear the LCD
;	i -> R16,R17
;	test -> Y+3
;	d -> R18
;    1775         
;    1776         CF_Info();                                                              //Read info from CF
;    1777         
;    1778         sprintf(string,"Signature CF: %x%x",CF_Sector_Data[1],CF_Sector_Data[0]);
;    1779         S1D13705_Text_LCD(0,2,string,0);                       
;    1780         
;    1781         test[1] = CF_Sector_Data[3];
;    1782         test[2] = (test[1] << 8) + CF_Sector_Data[2];
;    1783         
;    1784         sprintf(string,"Cylinders: %d",test[2]);
;    1785         S1D13705_Text_LCD(0,3,string,0);
;    1786         
;    1787         sprintf(string,"Heads: %d",CF_Sector_Data[6]);
;    1788         S1D13705_Text_LCD(0,4,string,0);
;    1789         
;    1790         test[1] = CF_Sector_Data[13];
;    1791         test[2] = (test[1] << 8) + CF_Sector_Data[12];
;    1792         
;    1793         sprintf(string,"Number Sectors/Track: %d",test[2]);
;    1794         S1D13705_Text_LCD(0,5,string,0);
;    1795         
;    1796         test[0] = CF_Sector_Data[15];
;    1797         test[1] = (test[0] <<8) + CF_Sector_Data[14];
;    1798         test[2] = CF_Sector_Data[17];
;    1799         test[3] = (test[2] <<8) + CF_Sector_Data[16];
;    1800         
;    1801         sprintf(string,"Sectors: %x%x HEX",test[1],test[3]);
;    1802         S1D13705_Text_LCD(0,6,string,0);
;    1803         
;    1804         //Serial number
;    1805         i = 39;
;    1806         d = 40;
;    1807         sprintf(string,"SN: ");
;    1808         S1D13705_Text_LCD(0,7,string,0);
;    1809         
;    1810         while(i >19)
;    1811         {
;    1812                 sprintf(string,"%c",CF_Sector_Data[i]);
;    1813                 S1D13705_Text_LCD(d,7,string,0);
;    1814                 i--;  
;    1815                 d += 8;
;    1816         }
;    1817          
;    1818         delay_ms(1000);
;    1819         
;    1820         return;
;    1821 }
;    1822 //*************************************         
;    1823 
;    1824 /**************************************
;    1825 Function: Check of Blok/data info 
;    1826           aanwezig is in de CF
;    1827 **************************************/
;    1828 unsigned char CF_Check_Blok_Data(void)
;    1829 {
_CF_Check_Blok_Data:
;    1830         unsigned char i;            
;    1831         unsigned char Page_Present;
;    1832         unsigned long int Page;
;    1833         
;    1834         //Wis CF_Blok info
;    1835         for(i=0;i<32;i++)
	CALL SUBOPT_0x1
;	i -> R16
;	Page_Present -> R17
;	Page -> Y+2
	LDI  R16,LOW(0)
_0x113:
	CPI  R16,32
	BRSH _0x114
;    1836                 CF_Page[i] = 0xFF;
	CALL SUBOPT_0x26
	LDI  R30,LOW(255)
	ST   X,R30
;    1837         
;    1838         Page = 0;                                                       //Zet Page
	SUBI R16,-1
	RJMP _0x113
_0x114:
	__CLRD1S 2
;    1839         Page_Present = 0;                                               //Aantal Pages 0
	LDI  R17,LOW(0)
;    1840         
;    1841         for(i=0;i<32;i++)
	LDI  R16,LOW(0)
_0x116:
	CPI  R16,32
	BRSH _0x117
;    1842         {
;    1843                 CF_Read_Sector((152 * Page) + 1);
	__GETD1S 2
	CALL SUBOPT_0x27
	CALL __PUTPARD1
	CALL _CF_Read_Sector
;    1844                 
;    1845                 if(CF_Sector_Data[0] == 'B' & CF_Sector_Data[1] == 'l'& CF_Sector_Data[2] == 'o' & CF_Sector_Data[3] == 'k')
	LDS  R26,_CF_Sector_Data
	LDI  R30,LOW(66)
	CALL __EQB12
	PUSH R30
	__GETB1MN _CF_Sector_Data,1
	LDI  R26,LOW(108)
	CALL __EQB12
	POP  R26
	AND  R30,R26
	PUSH R30
	__GETB1MN _CF_Sector_Data,2
	LDI  R26,LOW(111)
	CALL __EQB12
	POP  R26
	AND  R30,R26
	PUSH R30
	__GETB1MN _CF_Sector_Data,3
	LDI  R26,LOW(107)
	CALL __EQB12
	POP  R26
	AND  R30,R26
	BREQ _0x118
;    1846                 {
;    1847                         CF_Page[i] = i;                                 //Page aanwezig zet Page aanwezig in Memory
	CALL SUBOPT_0x26
	ST   X,R16
;    1848                         Page_Present++;                                 //Als Page aanwezig is met 1 verhogen 
	SUBI R17,-1
;    1849                 }       
;    1850         
;    1851                 Page++;                                                 //Verhoog Page met 1 voor volgende sector
_0x118:
	CALL SUBOPT_0x1A
;    1852         }  
	SUBI R16,-1
	RJMP _0x116
_0x117:
;    1853         
;    1854         
;    1855         return(Page_Present);
	MOV  R30,R17
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,6
	RET
;    1856 }               
;    1857 //*************************************
;    1858 
;    1859 /**************************************
;    1860 Function: Draw data on LCD from CF
;    1861 **************************************/
;    1862 void CF_Draw_Pic(unsigned int X, unsigned int Y, unsigned long int Page)
;    1863 {
_CF_Draw_Pic:
;    1864         unsigned char b;                                                //blauwe kleur
;    1865         unsigned char g;                                                //groene kleur
;    1866         unsigned char r;                                                //rode kleur
;    1867 
;    1868         unsigned int i;
;    1869         unsigned int W;                                                 //Breedte plaatje
;    1870         unsigned int H;                                                 //Hoogte plaatje
;    1871         unsigned int Wx;                                                //Counter breedte
;    1872         unsigned int Byte;                                              //Byte teller van de array
;    1873         unsigned int Yx;                                                //Waarde voor scanline bytes, by 4bpp 160, by 8bpp 320
;    1874 
;    1875         unsigned long int Sector;                                       //Sector van CF
;    1876         unsigned long int S1D_mem;                                      //S1D13705 memory adress
;    1877         
;    1878         //S1D13705_Clear_LCD();                                   //Clear the LCD
;    1879 
;    1880         //Bereken de begin sector van de CF
;    1881         Sector = (152 * Page) + 1;
	SBIW R28,18
	CALL __SAVELOCR5
;	X -> Y+29
;	Y -> Y+27
;	Page -> Y+23
;	b -> R16
;	g -> R17
;	r -> R18
;	i -> R19,R20
;	W -> Y+21
;	H -> Y+19
;	Wx -> Y+17
;	Byte -> Y+15
;	Yx -> Y+13
;	Sector -> Y+9
;	S1D_mem -> Y+5
	__GETD1S 23
	CALL SUBOPT_0x27
	__PUTD1S 9
;    1882         
;    1883         //Lees eerste info sector in
;    1884         CF_Read_Sector(Sector++);
	CALL SUBOPT_0x28
	CALL SUBOPT_0x29
;    1885         
;    1886          //Lees de hoogte en de breedte van de Pic
;    1887         W = CF_Sector_Data[6];                                          //Lees lage Width
	__GETB1MN _CF_Sector_Data,6
	LDI  R31,0
	STD  Y+21,R30
	STD  Y+21+1,R31
;    1888         i = CF_Sector_Data[7];                                          //Lees hoge Width
	__POINTW1MN _CF_Sector_Data,7
	LD   R19,Z
	CLR  R20
;    1889         W = W + i;                                                      //Tel ze bij elkaar op
	__GETW1R 19,20
	LDD  R26,Y+21
	LDD  R27,Y+21+1
	ADD  R30,R26
	ADC  R31,R27
	STD  Y+21,R30
	STD  Y+21+1,R31
;    1890         H = CF_Sector_Data[8];                                          //Hoogte van BMP in pixels
	__GETB1MN _CF_Sector_Data,8
	LDI  R31,0
	STD  Y+19,R30
	STD  Y+19+1,R31
;    1891         
;    1892         
;    1893         //Check kleur en stel S1D13705 in
;    1894         if(CF_Sector_Data[5] == 4)                                      //16 kleuren
	__GETB1MN _CF_Sector_Data,5
	CPI  R30,LOW(0x4)
	BRNE _0x119
;    1895         {
;    1896                 printf("\n\rLUT 16 config");
	__POINTW1FN _0,705
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2A
;    1897                 S1D13705_Write_Register(0x02,0xB0);                     //Stel kleur register 2 in op 16 kleuren
	LDI  R30,LOW(176)
	CALL SUBOPT_0x2B
;    1898                 S1D13705_Write_Register(0x1C,0xA0);                     //Stel line byte counter in op 160 lines
	LDI  R30,LOW(160)
	CALL SUBOPT_0x2C
;    1899                 
;    1900                 //Zend LUT tabel van BMP plaatje naar S1D13705
;    1901                 S1D13705_Write_Register(0x15,0);                        //Set first the LUT adress at 0
;    1902                 
;    1903                 Byte = 255;                                             //Kleur begin Sector
	LDI  R30,LOW(255)
	LDI  R31,HIGH(255)
	STD  Y+15,R30
	STD  Y+15+1,R31
;    1904                 for(i=0;i<16;i++)
	__GETWRN 19,20,0
_0x11B:
	__CPWRN 19,20,16
	BRSH _0x11C
;    1905                 { 
;    1906                         //Read color values from LUT table
;    1907                         b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
	CALL SUBOPT_0x2D
	LD   R16,Z
;    1908                         g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
	CALL SUBOPT_0x2D
	LD   R17,Z
;    1909                         r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
	CALL SUBOPT_0x2D
	CALL SUBOPT_0x2E
;    1910                 
;    1911                         //Write color values to LUT register S1D13705
;    1912                         S1D13705_Write_Register(0x17,r);                //Write red value
;    1913                         S1D13705_Write_Register(0x17,g);                //Write green value
;    1914                         S1D13705_Write_Register(0x17,b);                //Write blue value  
;    1915                 }
	__ADDWRN 19,20,1
	RJMP _0x11B
_0x11C:
;    1916                 
;    1917                 Sector++; 
	CALL SUBOPT_0x28
;    1918                 Yx = 160;                                               //Stel Yx in op 160 scanline bytes
	LDI  R30,LOW(160)
	LDI  R31,HIGH(160)
	STD  Y+13,R30
	STD  Y+13+1,R31
;    1919                
;    1920                 printf("\n\rLUT 16 config done");
	__POINTW1FN _0,721
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;    1921         }
;    1922         
;    1923         if(CF_Sector_Data[5] == 8)                                      //256 kleuren
_0x119:
	__GETB1MN _CF_Sector_Data,5
	CPI  R30,LOW(0x8)
	BREQ PC+3
	JMP _0x11D
;    1924         {
;    1925                 printf("\n\rLUT 256 config");
	__POINTW1FN _0,742
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2A
;    1926                 S1D13705_Write_Register(0x02,0xF0);                     //Stel kleur register 2 in op 256 kleuren
	LDI  R30,LOW(240)
	CALL SUBOPT_0x2B
;    1927                 S1D13705_Write_Register(0x1C,0xFF);                     //Stel line byte counter in op 320 lines
	LDI  R30,LOW(255)
	CALL SUBOPT_0x2C
;    1928                 
;    1929                 //Zend LUT tabel van BMP plaatje naar S1D13705
;    1930                 S1D13705_Write_Register(0x15,0);                        //Set first the LUT adress at 0
;    1931                 
;    1932                 Byte = 254;                                             //Kleur begin Sector
	LDI  R30,LOW(254)
	LDI  R31,HIGH(254)
	STD  Y+15,R30
	STD  Y+15+1,R31
;    1933                 
;    1934                 //Schrijf eerste 258 kleuren daarna CF lezen
;    1935                 for(i=0;i<86;i++)
	__GETWRN 19,20,0
_0x11F:
	__CPWRN 19,20,86
	BRSH _0x120
;    1936                 {
;    1937                         //Read color values from LUT table
;    1938                         b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
	CALL SUBOPT_0x2D
	LD   R16,Z
;    1939                         g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
	CALL SUBOPT_0x2D
	LD   R17,Z
;    1940                         r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
	CALL SUBOPT_0x2D
	CALL SUBOPT_0x2E
;    1941                 
;    1942                         //Write color values to LUT register S1D13705
;    1943                         S1D13705_Write_Register(0x17,r);                //Write red value
;    1944                         S1D13705_Write_Register(0x17,g);                //Write green value
;    1945                         S1D13705_Write_Register(0x17,b);                //Write blue value       
;    1946                 }
	__ADDWRN 19,20,1
	RJMP _0x11F
_0x120:
;    1947                 
;    1948                 //Lees tweede info sector in
;    1949                 CF_Read_Sector(Sector++);   
	CALL SUBOPT_0x28
	CALL SUBOPT_0x29
;    1950                 
;    1951                 Byte = 0;
	LDI  R30,0
	STD  Y+15,R30
	STD  Y+15+1,R30
;    1952                 //Schrijf de laatste 510 kleuren naar S1D13705
;    1953                 for(i=0;i<170;i++)
	__GETWRN 19,20,0
_0x122:
	__CPWRN 19,20,170
	BRSH _0x123
;    1954                 {
;    1955                         //Read color values from LUT table
;    1956                         b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
	CALL SUBOPT_0x2D
	LD   R16,Z
;    1957                         g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
	CALL SUBOPT_0x2D
	LD   R17,Z
;    1958                         r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
	CALL SUBOPT_0x2D
	CALL SUBOPT_0x2E
;    1959                 
;    1960                         //Write color values to LUT register S1D13705
;    1961                         S1D13705_Write_Register(0x17,r);                //Write red value
;    1962                         S1D13705_Write_Register(0x17,g);                //Write green value
;    1963                         S1D13705_Write_Register(0x17,b);                //Write blue value       
;    1964                 }
	__ADDWRN 19,20,1
	RJMP _0x122
_0x123:
;    1965                  
;    1966                 Yx = 320;                                               //Stel scanline bytes in op 320
	LDI  R30,LOW(320)
	LDI  R31,HIGH(320)
	STD  Y+13,R30
	STD  Y+13+1,R31
;    1967                 
;    1968                 printf("\n\rLUT 256 config done");
	__POINTW1FN _0,759
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;    1969         }
;    1970                 
;    1971                 
;    1972         printf("\n\rDraw BMP data on LCD");
_0x11D:
	__POINTW1FN _0,781
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;    1973         
;    1974         //Zet S1D_mem op begin adress/pixel
;    1975         S1D_mem = X + (Y * Yx);
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	LDD  R26,Y+27
	LDD  R27,Y+27+1
	CALL __MULW12U
	LDD  R26,Y+29
	LDD  R27,Y+29+1
	ADD  R30,R26
	ADC  R31,R27
	CLR  R22
	CLR  R23
	__PUTD1S 5
;    1976         Byte = 0;
	LDI  R30,0
	STD  Y+15,R30
	STD  Y+15+1,R30
;    1977         
;    1978         //Lees eerste bitmap data in
;    1979         CF_Read_Sector(Sector++);   
	CALL SUBOPT_0x28
	CALL SUBOPT_0x29
;    1980         
;    1981         while(H--)
_0x124:
	LDD  R30,Y+19
	LDD  R31,Y+19+1
	SBIW R30,1
	STD  Y+19,R30
	STD  Y+19+1,R31
	ADIW R30,1
	BREQ _0x126
;    1982         {
;    1983                 Wx = W;
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	STD  Y+17,R30
	STD  Y+17+1,R31
;    1984                 while(Wx--)
_0x127:
	LDD  R30,Y+17
	LDD  R31,Y+17+1
	SBIW R30,1
	STD  Y+17,R30
	STD  Y+17+1,R31
	ADIW R30,1
	BREQ _0x129
;    1985                 {
;    1986                         S1D13705_Write_Data(S1D_mem++,CF_Sector_Data[Byte++]);     //Schrijf data naar S1D13705
	CALL SUBOPT_0x1D
	LDD  R30,Y+19
	LDD  R31,Y+19+1
	ADIW R30,1
	STD  Y+19,R30
	STD  Y+19+1,R31
	SBIW R30,1
	SUBI R30,LOW(-_CF_Sector_Data)
	SBCI R31,HIGH(-_CF_Sector_Data)
	LD   R30,Z
	ST   -Y,R30
	CALL _S1D13705_Write_Data
;    1987                         if(Byte == 512)                                            //Nieuwe data inlezen van CF
	LDD  R26,Y+15
	LDD  R27,Y+15+1
	CPI  R26,LOW(0x200)
	LDI  R30,HIGH(0x200)
	CPC  R27,R30
	BRNE _0x12A
;    1988                         {
;    1989                                 Byte = 0;
	LDI  R30,0
	STD  Y+15,R30
	STD  Y+15+1,R30
;    1990                                 CF_Read_Sector(Sector++);   
	CALL SUBOPT_0x28
	CALL SUBOPT_0x29
;    1991                         }
;    1992                 }
_0x12A:
	RJMP _0x127
_0x129:
;    1993                 
;    1994                 //Herbereken S1D_mem voor de volgende lijn
;    1995                 S1D_mem = (S1D_mem - W) + Yx;
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	__GETD2S 5
	CLR  R22
	CLR  R23
	CALL __SUBD21
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	CLR  R22
	CLR  R23
	CALL __ADDD12
	__PUTD1S 5
;    1996         }                                     
	RJMP _0x124
_0x126:
;    1997        
;    1998         return;
	CALL __LOADLOCR5
	ADIW R28,31
	RET
;    1999 }                                                    
;    2000 //*************************************              
;    2001 
;    2002 /**************************************
;    2003 Function: Delete data on CF
;    2004 **************************************/
;    2005 void CF_Delete_Data(void)
;    2006 {
_CF_Delete_Data:
;    2007         unsigned int i;
;    2008         unsigned long int Sector;
;    2009         unsigned long int End_Sector = 152 * 32;   
;    2010         
;    2011         //Wis CF_Sector_data
;    2012         for(i=0;i<512;i++)
	SBIW R28,8
	LDI  R24,4
	LDI  R26,LOW(0)
	LDI  R27,HIGH(0)
	LDI  R30,LOW(_0x12B*2)
	LDI  R31,HIGH(_0x12B*2)
	CALL __INITLOCB
	ST   -Y,R17
	ST   -Y,R16
;	i -> R16,R17
;	Sector -> Y+6
;	End_Sector -> Y+2
	__GETWRN 16,17,0
_0x12D:
	__CPWRN 16,17,512
	BRSH _0x12E
;    2013                 CF_Sector_Data[i] = 0x00;
	LDI  R26,LOW(_CF_Sector_Data)
	LDI  R27,HIGH(_CF_Sector_Data)
	ADD  R26,R16
	ADC  R27,R17
	LDI  R30,LOW(0)
	ST   X,R30
;    2014                 
;    2015         //Wis de blad blok data
;    2016         Sector = 1;
	__ADDWRN 16,17,1
	RJMP _0x12D
_0x12E:
	__GETD1N 0x1
	__PUTD1S 6
;    2017         while(Sector < End_Sector)
_0x12F:
	__GETD1S 2
	__GETD2S 6
	CALL __CPD21
	BRSH _0x131
;    2018         {
;    2019                 CF_Write_Sector(Sector++);
	__GETD1S 6
	__SUBD1N -1
	__PUTD1S 6
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	CALL __PUTPARD1
	CALL _CF_Write_Sector
;    2020         }
	RJMP _0x12F
_0x131:
;    2021         
;    2022         return;
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,10
	RET
;    2023 }
;    2024 //*************************************
;    2025         
;    2026                 
;    2027 /**************************************
;    2028                 LINK
;    2029 This will recieve the data and put it 
;    2030 in the CF
;    2031 **************************************/
;    2032 
;    2033 /**************************************
;    2034         Include header file's                   
;    2035 **************************************/
;    2036 #include "LCD_Test_Defs.h" 
;    2037 #include "cf_driver.h"
;    2038 #include "S1D13705.h"          

	.DSEG
_aS1DRegs_G6:
_0aS1DRegs_G6:
	.BYTE 0x1
_1aS1DRegs_G6:
	.BYTE 0x1
_2aS1DRegs_G6:
	.BYTE 0x1
_3aS1DRegs_G6:
	.BYTE 0x1
_4aS1DRegs_G6:
	.BYTE 0x1
_5aS1DRegs_G6:
	.BYTE 0x1
_6aS1DRegs_G6:
	.BYTE 0x1
_7aS1DRegs_G6:
	.BYTE 0x1
_8aS1DRegs_G6:
	.BYTE 0x1
_9aS1DRegs_G6:
	.BYTE 0x1
_AaS1DRegs_G6:
	.BYTE 0x1
_BaS1DRegs_G6:
	.BYTE 0x1
_CaS1DRegs_G6:
	.BYTE 0x1
_DaS1DRegs_G6:
	.BYTE 0x1
_EaS1DRegs_G6:
	.BYTE 0x1
_FaS1DRegs_G6:
	.BYTE 0x1
_10aS1DRegs_G6:
	.BYTE 0x1
_11aS1DRegs_G6:
	.BYTE 0x1
_12aS1DRegs_G6:
	.BYTE 0x1
_13aS1DRegs_G6:
	.BYTE 0x1
_14aS1DRegs_G6:
	.BYTE 0x1
_15aS1DRegs_G6:
	.BYTE 0x1
_16aS1DRegs_G6:
	.BYTE 0x1
_17aS1DRegs_G6:
	.BYTE 0x1
_18aS1DRegs_G6:
	.BYTE 0x1
_19aS1DRegs_G6:
	.BYTE 0x1
_1AaS1DRegs_G6:
	.BYTE 0x1
_1BaS1DRegs_G6:
	.BYTE 0x1
_1CaS1DRegs_G6:
	.BYTE 0x1
_1DaS1DRegs_G6:
	.BYTE 0x1
_1EaS1DRegs_G6:
	.BYTE 0x1
_1FaS1DRegs_G6:
	.BYTE 0x1
_20aS1DRegs_G6:
	.BYTE 0x1
_21aS1DRegs_G6:
	.BYTE 0x1
_22aS1DRegs_G6:
	.BYTE 0x1
_23aS1DRegs_G6:
	.BYTE 0x1
_24aS1DRegs_G6:
	.BYTE 0x1
_25aS1DRegs_G6:
	.BYTE 0x1
_26aS1DRegs_G6:
	.BYTE 0x1
_27aS1DRegs_G6:
	.BYTE 0x1
_28aS1DRegs_G6:
	.BYTE 0x1
_29aS1DRegs_G6:
	.BYTE 0x1
_2AaS1DRegs_G6:
	.BYTE 0x1
_2BaS1DRegs_G6:
	.BYTE 0x1
_2CaS1DRegs_G6:
	.BYTE 0x1
_2DaS1DRegs_G6:
	.BYTE 0x1
_2EaS1DRegs_G6:
	.BYTE 0x1
_2FaS1DRegs_G6:
	.BYTE 0x1
_30aS1DRegs_G6:
	.BYTE 0x1
_31aS1DRegs_G6:
	.BYTE 0x1
;    2039 //************************************* 
;    2040 
;    2041 /**************************************
;    2042 Function: Send ready sign to PC
;    2043 **************************************/
;    2044 void TX_Ready_Sign(void)
;    2045 {

	.CSEG
_TX_Ready_Sign:
;    2046         #asm("nop");
	nop
;    2047         putchar('!');
	LDI  R30,LOW(33)
	ST   -Y,R30
	CALL _putchar
;    2048         
;    2049         return;
	RET
;    2050 }
;    2051 //*************************************
;    2052 
;    2053 /**************************************
;    2054 Function: Receive bmp data and store
;    2055 **************************************/
;    2056 void Receive_BMP_Data(void)
;    2057 {
_Receive_BMP_Data:
;    2058         unsigned char data;
;    2059 
;    2060         unsigned int i;
;    2061         unsigned int Aantal_BMP_Sectors;                        
;    2062 
;    2063         unsigned long int Page;           
;    2064         unsigned long int Sector;   
;    2065         
;    2066         //Check welke blok vrij is begin daar
;    2067         Page = 0;                                               //Stel Page altijd standaard in op 0
	SBIW R28,8
	CALL __SAVELOCR5
;	data -> R16
;	i -> R17,R18
;	Aantal_BMP_Sectors -> R19,R20
;	Page -> Y+9
;	Sector -> Y+5
	__CLRD1S 9
;    2068         while(Page<32)
_0x157:
	__GETD2S 9
	__CPD2N 0x20
	BRSH _0x159
;    2069         {
;    2070                 if(CF_Page[Page] == Page)                        //Als Page gelijk is dan Page met 1 verhogen anders loop verlaten
	__GETD1S 9
	SUBI R30,LOW(-_CF_Page)
	SBCI R31,HIGH(-_CF_Page)
	LD   R30,Z
	MOV  R26,R30
	__GETD1S 9
	CLR  R27
	CLR  R24
	CLR  R25
	CALL __CPD12
	BRNE _0x15A
;    2071                         Page++;
	CALL SUBOPT_0x28
;    2072                 else
	RJMP _0x15B
_0x15A:
;    2073                         break;                                  //Break the loop                
	RJMP _0x159
;    2074         }
_0x15B:
	RJMP _0x157
_0x159:
;    2075           
;    2076         //Bepaal start Sector
;    2077         Sector = (152 * Page)+ 1;                               //Example 152 * 2 = 304 + 1 = 305
	__GETD1S 9
	CALL SUBOPT_0x27
	__PUTD1S 5
;    2078         
;    2079         //Zet ID in CF_Sector_data
;    2080         CF_Sector_Data[0] = 'B';
	LDI  R30,LOW(66)
	STS  _CF_Sector_Data,R30
;    2081         CF_Sector_Data[1] = 'l';
	LDI  R30,LOW(108)
	__PUTB1MN _CF_Sector_Data,1
;    2082         CF_Sector_Data[2] = 'o';
	LDI  R30,LOW(111)
	__PUTB1MN _CF_Sector_Data,2
;    2083         CF_Sector_Data[3] = 'k';
	LDI  R30,LOW(107)
	__PUTB1MN _CF_Sector_Data,3
;    2084         CF_Sector_Data[4] = Page;
	LDD  R30,Y+9
	__PUTB1MN _CF_Sector_Data,4
;    2085         
;    2086         S1D13705_Clear_LCD();                                   //Clear the LCD
	CALL _S1D13705_Clear_LCD
;    2087         
;    2088         sprintf(string,"Waiting for Link");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,804
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2089         S1D13705_Text_LCD(0,0,string,0);
	__GETD1N 0x0
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2090 
;    2091         data = getchar();
	RCALL _getchar
	MOV  R16,R30
;    2092         if(data == 'D')  
	CPI  R16,68
	BRNE _0x15C
;    2093         {
;    2094                 sprintf(string,"Link is maked");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,821
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2095                 S1D13705_Text_LCD(0,1,string,0);
	__GETD1N 0x1
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2096                 TX_Ready_Sign();
	CALL _TX_Ready_Sign
;    2097         }
;    2098                
;    2099         //Ontvang hoeveel sectors nodig voor bitmap data
;    2100         Aantal_BMP_Sectors = getchar();
_0x15C:
	RCALL _getchar
	LDI  R31,0
	__PUTW1R 19,20
;    2101         
;    2102         //Ontvang info over bmp, kleurdiepte/width/hoogte
;    2103         CF_Sector_Data[5] = getchar();                          //Kleurdiepte
	RCALL _getchar
	__PUTB1MN _CF_Sector_Data,5
;    2104         CF_Sector_Data[6] = getchar();                          //Width_l
	RCALL _getchar
	__PUTB1MN _CF_Sector_Data,6
;    2105         CF_Sector_Data[7] = getchar();                          //Width_h
	RCALL _getchar
	__PUTB1MN _CF_Sector_Data,7
;    2106         CF_Sector_Data[8] = getchar();                          //Hoogte
	RCALL _getchar
	__PUTB1MN _CF_Sector_Data,8
;    2107         
;    2108         
;    2109         //Ontvang LUT tabel
;    2110         if(CF_Sector_Data[5] == 4)                              //16 kleuren, 48 bytes ontvangen
	__GETB1MN _CF_Sector_Data,5
	CPI  R30,LOW(0x4)
	BRNE _0x15D
;    2111         {
;    2112                sprintf(string,"Info recieved!Receive LUT 16");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,835
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2113                 S1D13705_Text_LCD(0,2,string,0);
	__GETD1N 0x2
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2114                 TX_Ready_Sign();                                //Zend ready Signal
	CALL _TX_Ready_Sign
;    2115                 for(i=255;i<=302;i++)                           //Sla data op in CF_Sector_data
	__GETWRN 17,18,255
_0x15F:
	LDI  R30,LOW(302)
	LDI  R31,HIGH(302)
	CP   R30,R17
	CPC  R31,R18
	BRLO _0x160
;    2116                 {
;    2117                         TX_Ready_Sign();                        //Zend ready signal;
	CALL SUBOPT_0x30
;    2118                         CF_Sector_Data[i] = getchar();
	PUSH R31
	PUSH R30
	RCALL _getchar
	POP  R26
	POP  R27
	ST   X,R30
;    2119                 }
	__ADDWRN 17,18,1
	RJMP _0x15F
_0x160:
;    2120                 
;    2121                 //Sla CF_Sector_Data op
;    2122                 CF_Write_Sector(Sector);                        //Sla Informatie Sector op in begin Sector
	CALL SUBOPT_0x31
;    2123                 Sector += 2;                                    //Verhoog Sector met 2, omdat dan de BMP data begint
	__ADDD1N 2
	__PUTD1S 5
;    2124         }
;    2125         if(CF_Sector_Data[5] == 8)                              //256 kleuren 768 bytes ontvangen verdelen over 2 sectoren
_0x15D:
	__GETB1MN _CF_Sector_Data,5
	CPI  R30,LOW(0x8)
	BREQ PC+3
	JMP _0x161
;    2126         {
;    2127 
;    2128                 sprintf(string,"Info recieved!Receive LUT 256");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,864
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2129                 S1D13705_Text_LCD(0,2,string,0);
	__GETD1N 0x2
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2130                 
;    2131                 //Ontvanger eerst 256 bytes in de eerste sector
;    2132                 TX_Ready_Sign();                                //Zend ready signal;
	CALL _TX_Ready_Sign
;    2133                 for(i=254;i<=511;i++)
	__GETWRN 17,18,254
_0x163:
	LDI  R30,LOW(511)
	LDI  R31,HIGH(511)
	CP   R30,R17
	CPC  R31,R18
	BRLO _0x164
;    2134                 {
;    2135                         TX_Ready_Sign();                        //Zend ready signal;
	CALL SUBOPT_0x30
;    2136                         CF_Sector_Data[i] = getchar();
	PUSH R31
	PUSH R30
	RCALL _getchar
	POP  R26
	POP  R27
	ST   X,R30
;    2137                 }        
	__ADDWRN 17,18,1
	RJMP _0x163
_0x164:
;    2138                 
;    2139                 //Sla eerste info sector op CF
;    2140                 CF_Write_Sector(Sector);                        //Sla eerste info sector op in start sector
	CALL SUBOPT_0x31
;    2141                 Sector++;                                       //Verhoog met 1                           
	__SUBD1N -1
	__PUTD1S 5
;    2142                 
;    2143                                
;    2144                 //Ontvang tweede sector LUT
;    2145                 for(i=0;i<=509;i++)
	__GETWRN 17,18,0
_0x166:
	LDI  R30,LOW(509)
	LDI  R31,HIGH(509)
	CP   R30,R17
	CPC  R31,R18
	BRLO _0x167
;    2146                 {
;    2147                         TX_Ready_Sign();                        //Zend ready signal;
	CALL SUBOPT_0x30
;    2148                         CF_Sector_Data[i] = getchar();
	PUSH R31
	PUSH R30
	RCALL _getchar
	POP  R26
	POP  R27
	ST   X,R30
;    2149                 }                        
	__ADDWRN 17,18,1
	RJMP _0x166
_0x167:
;    2150                 
;    2151                 //Sla tweede info sector op CF
;    2152                 CF_Write_Sector(Sector);
	CALL SUBOPT_0x31
;    2153                 Sector++;
	__SUBD1N -1
	__PUTD1S 5
;    2154         }
;    2155         
;    2156         sprintf(string,"LUT recieved! Get BITMAP data");
_0x161:
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,894
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2157         S1D13705_Text_LCD(0,3,string,0);
	__GETD1N 0x3
	CALL SUBOPT_0xB
	CALL SUBOPT_0xC
;    2158         sprintf(string,"Sectors to recieve: %d at Page %d",Aantal_BMP_Sectors,Page);
	__POINTW1FN _0,924
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 19,20
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETD1S 17
	CALL __PUTPARD1
	LDI  R24,8
	CALL _sprintf
	ADIW R28,12
;    2159         S1D13705_Text_LCD(0,4,string,0);
	__GETD1N 0x0
	CALL __PUTPARD1
	__GETD1N 0x4
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2160         
;    2161         //Ontvangen van de bitmap Data
;    2162         TX_Ready_Sign();                                        //Zend ready signal;
	CALL _TX_Ready_Sign
;    2163         
;    2164         for(data=1;data<=Aantal_BMP_Sectors;data++)
	LDI  R16,LOW(1)
_0x169:
	__GETW1R 19,20
	MOV  R26,R16
	LDI  R27,0
	CP   R30,R26
	CPC  R31,R27
	BRLO _0x16A
;    2165         {
;    2166                 //Lees eerste 512 bytes in van de PC
;    2167                 for(i=0;i<=511;i++) 
	__GETWRN 17,18,0
_0x16C:
	LDI  R30,LOW(511)
	LDI  R31,HIGH(511)
	CP   R30,R17
	CPC  R31,R18
	BRLO _0x16D
;    2168                 {      
;    2169                         //TX_Ready_Sign();                        //Zend ready signal;
;    2170                         CF_Sector_Data[i] = getchar();  
	__GETW1R 17,18
	SUBI R30,LOW(-_CF_Sector_Data)
	SBCI R31,HIGH(-_CF_Sector_Data)
	PUSH R31
	PUSH R30
	RCALL _getchar
	POP  R26
	POP  R27
	ST   X,R30
;    2171                 }
	__ADDWRN 17,18,1
	RJMP _0x16C
_0x16D:
;    2172                         
;    2173                 sprintf(string,"Sector %d stored",data);
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,958
	CALL SUBOPT_0xE
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
;    2174                 S1D13705_Text_LCD(0,5,string,0);
	__GETD1N 0x0
	CALL __PUTPARD1
	__GETD1N 0x5
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2175                 
;    2176                 //Sla bitmap data op
;    2177                 CF_Write_Sector(Sector++);
	CALL SUBOPT_0x1D
	CALL _CF_Write_Sector
;    2178                 TX_Ready_Sign();                        //Zend ready signal
	CALL _TX_Ready_Sign
;    2179         }
	SUBI R16,-1
	RJMP _0x169
_0x16A:
;    2180         
;    2181         sprintf(string,"Received done");
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,975
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL SUBOPT_0x2F
;    2182         S1D13705_Text_LCD(0,6,string,0);
	__GETD1N 0x6
	CALL SUBOPT_0xB
	CALL _S1D13705_Text_LCD
;    2183                                       
;    2184         delay_ms(1000); 
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    2185 
;    2186         return;
	CALL __LOADLOCR5
	ADIW R28,13
	RET
;    2187 }
;    2188 //*************************************       
;    2189 
;    2190 /**************************************
;    2191       4 Wire Touchscreen driver      
;    2192       
;    2193 Version:  
;    2194 **************************************/        
;    2195 #include "lcd_test_defs.h"
;    2196 //#include "s1d13705.h"
;    2197 #include "Touchscreen.h"               
;    2198 
;    2199 static float A,B,C,D,E,F;

	.DSEG
_A_G7:
	.BYTE 0x4
_B_G7:
	.BYTE 0x4
_C_G7:
	.BYTE 0x4
_D_G7:
	.BYTE 0x4
_E_G7:
	.BYTE 0x4
_F_G7:
	.BYTE 0x4
;    2200 
;    2201 /**************************************
;    2202 Functie: Touchscreen init
;    2203 **************************************/
;    2204 void Touchscreen_Init(void)
;    2205 {

	.CSEG
_Touchscreen_Init:
;    2206         //Lees calibratie coeffecients in van eeprom
;    2207         A = ee_float1;
	LDI  R26,LOW(_ee_float1)
	LDI  R27,HIGH(_ee_float1)
	CALL __EEPROMRDD
	STS  _A_G7,R30
	STS  _A_G7+1,R31
	STS  _A_G7+2,R22
	STS  _A_G7+3,R23
;    2208         B = ee_float2;
	LDI  R26,LOW(_ee_float2)
	LDI  R27,HIGH(_ee_float2)
	CALL __EEPROMRDD
	STS  _B_G7,R30
	STS  _B_G7+1,R31
	STS  _B_G7+2,R22
	STS  _B_G7+3,R23
;    2209         C = ee_float3;
	LDI  R26,LOW(_ee_float3)
	LDI  R27,HIGH(_ee_float3)
	CALL __EEPROMRDD
	STS  _C_G7,R30
	STS  _C_G7+1,R31
	STS  _C_G7+2,R22
	STS  _C_G7+3,R23
;    2210         D = ee_float4;
	LDI  R26,LOW(_ee_float4)
	LDI  R27,HIGH(_ee_float4)
	CALL __EEPROMRDD
	STS  _D_G7,R30
	STS  _D_G7+1,R31
	STS  _D_G7+2,R22
	STS  _D_G7+3,R23
;    2211         E = ee_float5;
	LDI  R26,LOW(_ee_float5)
	LDI  R27,HIGH(_ee_float5)
	CALL __EEPROMRDD
	STS  _E_G7,R30
	STS  _E_G7+1,R31
	STS  _E_G7+2,R22
	STS  _E_G7+3,R23
;    2212         F = ee_float6;
	LDI  R26,LOW(_ee_float6)
	LDI  R27,HIGH(_ee_float6)
	CALL __EEPROMRDD
	STS  _F_G7,R30
	STS  _F_G7+1,R31
	STS  _F_G7+2,R22
	STS  _F_G7+3,R23
;    2213         
;    2214         //Lees Flag register in van eeprom
;    2215         Touchscreen.Flag_Register = ee_Touch_Flag; 
	LDI  R26,LOW(_ee_Touch_Flag)
	LDI  R27,HIGH(_ee_Touch_Flag)
	CALL __EEPROMRDB
	STS  _Touchscreen,R30
;    2216         printf("\n\rTouch_Flag: %x",Touchscreen.Flag_Register);
	__POINTW1FN _0,989
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_Touchscreen
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;    2217         //Zet Touchscreen standby
;    2218         Touch_Standby;
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x18,3
	CALL SUBOPT_0x32
	CBI  0x18,4
	CALL SUBOPT_0x33
;    2219         
;    2220         //Zet TouchPressed flag op 0
;    2221         Touchscreen.Flag_Register &= ~(1<<TouchPressed);
;    2222         
;    2223         //Zet int0 actief
;    2224         GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
	CALL SUBOPT_0x3
;    2225         GICR |= (1<<INT0);                                      //Enable the INT0 request
;    2226 
;    2227         return;
	RET
;    2228 }
;    2229 //*************************************
;    2230 
;    2231 /**************************************
;    2232 Functie: Check of touchscreen nog is 
;    2233          ingedrukt,debounce
;    2234 **************************************/
;    2235 unsigned char Touchscreen_Pressed(void)
;    2236 {
_Touchscreen_Pressed:
;    2237         //Check of de touchscreen nog is ingedrukt
;    2238         delay_ms(10);
	CALL SUBOPT_0x34
;    2239         if(TouchPress != 0)             
	SBIS 0x10,2
	RJMP _0x16E
;    2240                 return(0);                                      //Touchscreen is niet ingedrukt, ga terug.
	LDI  R30,LOW(0)
	RET
;    2241         
;    2242         return(0x01);                                           //Touchscreen is nog ingedrukt return een 1
_0x16E:
	LDI  R30,LOW(1)
	RET
;    2243 }
;    2244 //*************************************
;    2245                                              
;    2246 /**************************************
;    2247 Functie: Lees x of y raw value van touchscreen
;    2248 **************************************/
;    2249 unsigned int Touchscreen_Value(unsigned char T_as)
;    2250 {
_Touchscreen_Value:
;    2251         unsigned int Touch_Data[6];                             //Gemeten touch data 
;    2252         unsigned char Channel;                                  //Channel voor de ADC om te meten   
;    2253         unsigned char i;
;    2254         
;    2255         //Checke of aan X of Y moeten worden gemeten
;    2256         if(T_as == 0)                                           //X as       
	SBIW R28,12
	ST   -Y,R17
	ST   -Y,R16
;	T_as -> Y+14
;	Touch_Data -> Y+2
;	Channel -> R16
;	i -> R17
	LDD  R30,Y+14
	CPI  R30,0
	BRNE _0x16F
;    2257         {
;    2258                 Touch_Xas;                                      //Stel I/O in zodat aan de X as wordt gemeten.
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	SBI  0x18,3
	OR   R30,R26
	MOV  R26,R30
	CBI  0x18,4
	LDI  R30,LOW(0)
	OR   R30,R26
;    2259                 Channel = 0x01;                                 //Stel channel in om te meten aan X as
	LDI  R16,LOW(1)
;    2260         }
;    2261         else                                                           
	RJMP _0x170
_0x16F:
;    2262         {
;    2263                 Touch_Yas;                                      //Stel I/O in zodat aan de Y as wordt gemeten.
	SBI  0x18,1
	CBI  0x18,2
	CALL SUBOPT_0x21
	CBI  0x18,3
	CALL SUBOPT_0x32
	SBI  0x18,4
	LDI  R30,LOW(1)
	OR   R30,R26
;    2264                 Channel = 0x00;                                 //Stel channel in om te meten aan Y as
	LDI  R16,LOW(0)
;    2265         }
_0x170:
;    2266         
;    2267         //Meet 5x touchscreen data
;    2268         for(i=0;i<5;i++) 
	LDI  R17,LOW(0)
_0x172:
	CPI  R17,5
	BRSH _0x173
;    2269                 Touch_Data[i] = ADC_LTC1298_Data(Channel);
	MOV  R30,R17
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,2
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	ST   -Y,R16
	RCALL _ADC_LTC1298_Data
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;    2270         
;    2271         //Neem gemiddelde van meetwaarde
;    2272         Touch_Data[5] = (Touch_Data[0] / 5) + (Touch_Data[1] / 5) + (Touch_Data[2] / 5) + (Touch_Data[3] / 5) + (Touch_Data[4] / 5);
	SUBI R17,-1
	RJMP _0x172
_0x173:
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL __DIVW21U
	PUSH R31
	PUSH R30
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL __DIVW21U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL __DIVW21U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL __DIVW21U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CALL __DIVW21U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	STD  Y+12,R30
	STD  Y+12+1,R31
;    2273         
;    2274         return(Touch_Data[5]);
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,15
	RET
;    2275 }
;    2276 //*************************************
;    2277 
;    2278 /**************************************
;    2279 Functie: Touchscreen X and Y positie
;    2280 **************************************/
;    2281 void Touchscreen_XY_positie(void)
;    2282 {
_Touchscreen_XY_positie:
;    2283         float Xpos, Ypos;
;    2284         unsigned int Xraw, Yraw;
;    2285         unsigned char X = 0;
;    2286         unsigned char Y = 1;    
;    2287         unsigned char i;
;    2288         
;    2289         //Check of de touchscreen nog is ingedrukt dan pas meten
;    2290         if((i=Touchscreen_Pressed()) != 1)                              //Touchscreen niet ingedrukt, return
	SBIW R28,9
	CALL __SAVELOCR6
;	Xpos -> Y+11
;	Ypos -> Y+7
;	Xraw -> R16,R17
;	Yraw -> R18,R19
;	X -> R20
;	Y -> R21
;	i -> Y+6
	LDI  R20,0
	LDI  R21,1
	CALL SUBOPT_0x35
	BREQ _0x174
;    2291         {
;    2292                 //Zet TouchPressed flag op 0
;    2293                 Touchscreen.Flag_Register &= ~(1<<TouchPressed); 
	CALL SUBOPT_0x36
;    2294                 printf("\n\rReleased");    
	__POINTW1FN _0,1006
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
;    2295                 return;
	RJMP _0x245
;    2296         }
;    2297                 
;    2298         //Meet Xraw en Yraw value
;    2299         Xraw = Touchscreen_Value(X);                                    //Meet Xraw data
_0x174:
	ST   -Y,R20
	CALL _Touchscreen_Value
	__PUTW1R 16,17
;    2300         Yraw = Touchscreen_Value(Y);                                    //Meet Yraw data
	ST   -Y,R21
	CALL _Touchscreen_Value
	__PUTW1R 18,19
;    2301         
;    2302         //Bereken Xpos en Ypos
;    2303         Xpos = (A * (float)Xraw) + (B * (float)Yraw) + C;
	__GETW1R 16,17
	CALL SUBOPT_0x37
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETW1R 18,19
	CALL SUBOPT_0x38
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __ADDF12
	LDS  R26,_C_G7
	LDS  R27,_C_G7+1
	LDS  R24,_C_G7+2
	LDS  R25,_C_G7+3
	CALL __ADDF12
	__PUTD1S 11
;    2304         Ypos= (D * (float)Xraw) + (E * (float)Yraw) + F;
	__GETW1R 16,17
	CALL SUBOPT_0x39
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETW1R 18,19
	CALL SUBOPT_0x3A
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __ADDF12
	LDS  R26,_F_G7
	LDS  R27,_F_G7+1
	LDS  R24,_F_G7+2
	LDS  R25,_F_G7+3
	CALL __ADDF12
	__PUTD1S 7
;    2305         
;    2306         //Sla Xpos en Ypos op in sruct als unsigned int
;    2307         Touchscreen.X_pos = (unsigned int)Xpos;
	__GETD1S 11
	CALL __CFD1
	__PUTW1MN _Touchscreen,1
;    2308         Touchscreen.Y_pos = (unsigned int)Ypos;        
	__GETD1S 7
	CALL __CFD1
	__PUTW1MN _Touchscreen,3
;    2309         
;    2310         //Zet TouchPressed flag op 0
;    2311         Touchscreen.Flag_Register &= ~(1<<TouchPressed);  
	CALL SUBOPT_0x36
;    2312         
;    2313         //Zet XY_Value flag op 1 als teken dat er een geldige XY waarde aanwezig is
;    2314         Touchscreen.Flag_Register |= (1<<XY_Value);
	LDS  R30,_Touchscreen
	ORI  R30,8
	STS  _Touchscreen,R30
;    2315         
;    2316         //Zet touchscreen standby voor nieuwe drukmeting
;    2317         Touch_Standby;
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x18,3
	CALL SUBOPT_0x32
	CBI  0x18,4
	LDI  R30,LOW(0)
	OR   R30,R26
;    2318         
;    2319         return;
_0x245:
	CALL __LOADLOCR6
	ADIW R28,15
	RET
;    2320 }
;    2321 //*************************************
;    2322                        
;    2323 /**************************************
;    2324 Functie: Touchscreen calibratie
;    2325 **************************************/
;    2326 void Touchscreen_Calibratie(void)
;    2327 {
_Touchscreen_Calibratie:
;    2328         unsigned int Xt1,Xt2,Xt3;                                       //3 sample points X as van de touchscreen
;    2329         unsigned int Yt1,Yt2,Yt3;                                       //3 sample points Y as van de touchscreen
;    2330         unsigned int Xd1,Xd2,Xd3;                                       //3 referentie punten van de display X 
;    2331         unsigned int Yd1,Yd2,Yd3;                                       //3 referentie punten van de display Y
;    2332         unsigned char X = 0;
;    2333         unsigned char Y = 1;   
;    2334         unsigned char i;
;    2335         
;    2336         //Zet in flag register TouchCalProcess aan
;    2337         Touchscreen.Flag_Register |= (1<<TouchCalProcess);      
	SBIW R28,21
	LDI  R24,2
	LDI  R26,LOW(1)
	LDI  R27,HIGH(1)
	LDI  R30,LOW(_0x175*2)
	LDI  R31,HIGH(_0x175*2)
	CALL __INITLOCB
	CALL __SAVELOCR6
;	Xt1 -> R16,R17
;	Xt2 -> R18,R19
;	Xt3 -> R20,R21
;	Yt1 -> Y+25
;	Yt2 -> Y+23
;	Yt3 -> Y+21
;	Xd1 -> Y+19
;	Xd2 -> Y+17
;	Xd3 -> Y+15
;	Yd1 -> Y+13
;	Yd2 -> Y+11
;	Yd3 -> Y+9
;	X -> Y+8
;	Y -> Y+7
;	i -> Y+6
	LDS  R30,_Touchscreen
	ORI  R30,4
	STS  _Touchscreen,R30
;    2338         
;    2339         //Stel referentie punten vast van de display
;    2340         //Meet punt 1
;    2341         Xd1 = 31;
	LDI  R30,LOW(31)
	LDI  R31,HIGH(31)
	STD  Y+19,R30
	STD  Y+19+1,R31
;    2342         Yd1 = 215;
	LDI  R30,LOW(215)
	LDI  R31,HIGH(215)
	STD  Y+13,R30
	STD  Y+13+1,R31
;    2343         //Xd1 = 287;
;    2344         //Yd1 = 119;
;    2345         
;    2346         //Meet punt 2
;    2347         Xd2 = 159;
	LDI  R30,LOW(159)
	LDI  R31,HIGH(159)
	STD  Y+17,R30
	STD  Y+17+1,R31
;    2348         Yd2 = 23;
	LDI  R30,LOW(23)
	LDI  R31,HIGH(23)
	STD  Y+11,R30
	STD  Y+11+1,R31
;    2349         
;    2350         //Meet punt 3
;    2351         Xd3 = 287;
	LDI  R30,LOW(287)
	LDI  R31,HIGH(287)
	STD  Y+15,R30
	STD  Y+15+1,R31
;    2352         Yd3 = 119;
	LDI  R30,LOW(119)
	LDI  R31,HIGH(119)
	STD  Y+9,R30
	STD  Y+9+1,R31
;    2353         //Xd3 = 31;
;    2354         //Yd3 = 23;
;    2355            
;    2356         //Stel display in
;    2357         S1D13705_Clear_LCD();                                           //Wis LCD scherm
	CALL _S1D13705_Clear_LCD
;    2358         
;    2359         //Teken kruis meet Punt1
;    2360         //S1D13705_Draw_Horizontal_Line(283,291,119,0);                 //Instellen als X(begin), X1(eind), Y, kleur
;    2361         //S1D13705_Draw_Vertical_Line(287,115,123,0);                   //Instellen als X, Y(begin), Y1(eind), kleur
;    2362         S1D13705_Draw_Horizontal_Line(27,35,215,0);                     //Instellen als X(begin), X1(eind), Y, kleur
	CALL SUBOPT_0x3B
	CALL SUBOPT_0x6
;    2363         S1D13705_Draw_Vertical_Line(31,211,219,0);                      //Instellen als X, Y(begin), Y1(eind), kleur     
	CALL SUBOPT_0x3C
	CALL __PUTPARD1
	CALL SUBOPT_0x7
;    2364                  
;    2365         do
_0x177:
;    2366         {
;    2367                 //Wacht totdat op het touchscreen is gedrukt
;    2368                 while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);     //Als uitkomst 0 is dan is er nog niet gedrukt 
_0x179:
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x1)
	BREQ _0x179
;    2369                 
;    2370                 //Check of de touchscreen nog is ingedrukt dan pas meten
;    2371                 if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
	CALL SUBOPT_0x35
	BRNE _0x17C
;    2372                 {        
;    2373                         Xt1 = Touchscreen_Value(X);                     //Meet de X waarde
	CALL SUBOPT_0x3D
	__PUTW1R 16,17
;    2374                         Yt1 = Touchscreen_Value(Y);                     //Meet de Y waarde
	CALL SUBOPT_0x3E
	STD  Y+25,R30
	STD  Y+25+1,R31
;    2375                 }
;    2376                 else
	RJMP _0x17D
_0x17C:
;    2377                 {
;    2378                         Xt1 = 0;
	__GETWRN 16,17,0
;    2379                         Yt1 = 0;
	LDI  R30,0
	STD  Y+25,R30
	STD  Y+25+1,R30
;    2380                 }
_0x17D:
;    2381                 
;    2382                 
;    2383                 //Zet touchscreen standby voor nieuwe drukmeting
;    2384                 Touch_Standby;
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x18,3
	CALL SUBOPT_0x32
	CBI  0x18,4
	CALL SUBOPT_0x33
;    2385         
;    2386                 //Zet TouchPressed flag op 0
;    2387                 Touchscreen.Flag_Register &= ~(1<<TouchPressed);
;    2388                 
;    2389                 //Wacht totdat touchscreen wordt los gelaten
;    2390                 delay_ms(10);
	CALL SUBOPT_0x34
;    2391                 while(TouchPress == 0);
_0x17E:
	SBIS 0x10,2
	RJMP _0x17E
;    2392 
;    2393                 GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
	CALL SUBOPT_0x3
;    2394                 GICR |= (1<<INT0);                                      //Enable the INT0 request
;    2395 
;    2396         }
;    2397         while(Xt1 == 0 || Yt1 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
	CLR  R0
	CP   R0,R16
	CPC  R0,R17
	BREQ _0x181
	LDD  R26,Y+25
	LDD  R27,Y+25+1
	CALL __CPW02
	BRNE _0x178
_0x181:
	RJMP _0x177
_0x178:
;    2398                 
;    2399         //Meten punt 1 klaar
;    2400         //Wis kruis meetpunt 1
;    2401         //S1D13705_Draw_Horizontal_Line(283,291,119,15);                //Stuur kleur wit om de pixels te wissen       
;    2402         //S1D13705_Draw_Vertical_Line(287,115,123,15);                                                    
;    2403         S1D13705_Draw_Horizontal_Line(27,35,215,15);                    //Stuur kleur wit om de pixels te wissen         
	CALL SUBOPT_0x3B
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Horizontal_Line
;    2404         S1D13705_Draw_Vertical_Line(31,211,219,15);                                         
	CALL SUBOPT_0x3C
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Vertical_Line
;    2405         
;    2406         //Teken kruis meet punt 2 
;    2407         S1D13705_Draw_Horizontal_Line(155,163,23,0);                    //Instellen als X(begin), X1(eind), Y, kleur
	CALL SUBOPT_0x3F
	CALL SUBOPT_0x6
;    2408         S1D13705_Draw_Vertical_Line(159,19,27,0);                       //Instellen als X, Y(begin), Y1(eind), kleur  
	CALL SUBOPT_0x40
	CALL __PUTPARD1
	CALL SUBOPT_0x7
;    2409         
;    2410         do
_0x184:
;    2411         {
;    2412                 //Wacht totdat op het touchscreen is gedrukt
;    2413                 while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);         //Als uitkomst 0 is dan is er nog niet gedrukt 
_0x186:
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x1)
	BREQ _0x186
;    2414 
;    2415                 //Check of de touchscreen nog is ingedrukt dan pas meten
;    2416                 if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
	CALL SUBOPT_0x35
	BRNE _0x189
;    2417                 {        
;    2418                         Xt2 = Touchscreen_Value(X);                     //Meet de X waarde
	CALL SUBOPT_0x3D
	__PUTW1R 18,19
;    2419                         Yt2 = Touchscreen_Value(Y);                     //Meet de Y waarde
	CALL SUBOPT_0x3E
	STD  Y+23,R30
	STD  Y+23+1,R31
;    2420                 }
;    2421                 else
	RJMP _0x18A
_0x189:
;    2422                 {
;    2423                         Xt2 = 0;
	__GETWRN 18,19,0
;    2424                         Yt2 = 0;
	LDI  R30,0
	STD  Y+23,R30
	STD  Y+23+1,R30
;    2425                 }
_0x18A:
;    2426                 
;    2427                 //Zet touchscreen standby voor nieuwe drukmeting
;    2428                 Touch_Standby;
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x18,3
	CALL SUBOPT_0x32
	CBI  0x18,4
	CALL SUBOPT_0x33
;    2429 
;    2430 
;    2431                 //Zet TouchPressed flag op 0
;    2432                 Touchscreen.Flag_Register &= ~(1<<TouchPressed);
;    2433                 
;    2434                 //Wacht totdat touchscreen wordt los gelaten
;    2435                 delay_ms(10);
	CALL SUBOPT_0x34
;    2436                 while(TouchPress == 0);
_0x18B:
	SBIS 0x10,2
	RJMP _0x18B
;    2437 
;    2438                 GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
	CALL SUBOPT_0x3
;    2439                 GICR |= (1<<INT0);                                      //Enable the INT0 request
;    2440 
;    2441         }
;    2442         while(Xt2 == 0 || Yt2 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
	CLR  R0
	CP   R0,R18
	CPC  R0,R19
	BREQ _0x18E
	LDD  R26,Y+23
	LDD  R27,Y+23+1
	CALL __CPW02
	BRNE _0x185
_0x18E:
	RJMP _0x184
_0x185:
;    2443         
;    2444         //Meten punt 2 klaar
;    2445         //Wis kruis meetpunt 2   
;    2446         S1D13705_Draw_Horizontal_Line(155,163,23,15);                   //Stuur kleur wit om de pixels te wissen
	CALL SUBOPT_0x3F
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Horizontal_Line
;    2447         S1D13705_Draw_Vertical_Line(159,19,27,15);   
	CALL SUBOPT_0x40
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Vertical_Line
;    2448         
;    2449         //Teken kruis meet punt 3
;    2450         //S1D13705_Draw_Horizontal_Line(27,35,215,0);                   //Instellen als X(begin), X1(eind), Y, kleur
;    2451         //S1D13705_Draw_Vertical_Line(31,211,219,0);                    //Instellen als X, Y(begin), Y1(eind), kleur     
;    2452         S1D13705_Draw_Horizontal_Line(283,291,119,0);                   //Instellen als X(begin), X1(eind), Y, kleur
	CALL SUBOPT_0x41
	CALL SUBOPT_0x6
;    2453         S1D13705_Draw_Vertical_Line(287,115,123,0);                     //Instellen als X, Y(begin), Y1(eind), kleur
	CALL SUBOPT_0x42
	CALL __PUTPARD1
	CALL SUBOPT_0x7
;    2454                
;    2455         do
_0x191:
;    2456         {
;    2457                 //Wacht totdat op het touchscreen is gedrukt
;    2458                 while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);         //Als uitkomst 0 is dan is er nog niet gedrukt 
_0x193:
	LDS  R30,_Touchscreen
	ANDI R30,LOW(0x1)
	BREQ _0x193
;    2459 
;    2460                 //Check of de touchscreen nog is ingedrukt dan pas meten
;    2461                 if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
	CALL SUBOPT_0x35
	BRNE _0x196
;    2462                 {        
;    2463                         Xt3 = Touchscreen_Value(X);                     //Meet de X waarde
	CALL SUBOPT_0x3D
	__PUTW1R 20,21
;    2464                         Yt3 = Touchscreen_Value(Y);                     //Meet de Y waarde
	CALL SUBOPT_0x3E
	STD  Y+21,R30
	STD  Y+21+1,R31
;    2465                 }
;    2466                 else
	RJMP _0x197
_0x196:
;    2467                 {
;    2468                         Xt3 = 0;
	__GETWRN 20,21,0
;    2469                         Yt3 = 0;
	LDI  R30,0
	STD  Y+21,R30
	STD  Y+21+1,R30
;    2470                 } 
_0x197:
;    2471                 
;    2472                 //Zet touchscreen standby voor nieuwe drukmeting
;    2473                 Touch_Standby;
	CBI  0x18,1
	SBI  0x18,2
	LDI  R30,LOW(1)
	MOV  R26,R30
	CBI  0x18,3
	CALL SUBOPT_0x32
	CBI  0x18,4
	CALL SUBOPT_0x33
;    2474 
;    2475                 //Zet TouchPressed flag op 0
;    2476                 Touchscreen.Flag_Register &= ~(1<<TouchPressed);
;    2477                 
;    2478                 //Wacht totdat touchscreen wordt los gelaten
;    2479                 delay_ms(10);
	CALL SUBOPT_0x34
;    2480                 while(TouchPress == 0);
_0x198:
	SBIS 0x10,2
	RJMP _0x198
;    2481         
;    2482                 GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
	CALL SUBOPT_0x3
;    2483                 GICR |= (1<<INT0);                                      //Enable the INT0 request
;    2484                 
;    2485         }
;    2486         while(Xt3 == 0 || Yt3 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
	CLR  R0
	CP   R0,R20
	CPC  R0,R21
	BREQ _0x19B
	LDD  R26,Y+21
	LDD  R27,Y+21+1
	CALL __CPW02
	BRNE _0x192
_0x19B:
	RJMP _0x191
_0x192:
;    2487          
;    2488         
;    2489         
;    2490         //Bereken de calibratie coeffenties
;    2491         A = ((((float)Xd1 - (float)Xd3) * ((float)Yt2 - (float)Yt3)) - (((float)Xd2 - (float)Xd3) * ((float)Yt1 - (float)Yt3))) / ((((float)Xt1 -(float) Xt3) * ((float)Yt2 - (float)Yt3)) - (((float)Xt2 - (float)Xt3) * ((float)Yt1 - (float)Yt3)));
	LDD  R30,Y+19
	LDD  R31,Y+19+1
	CALL SUBOPT_0x43
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	LDD  R30,Y+17
	LDD  R31,Y+17+1
	CALL SUBOPT_0x43
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x45
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x46
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x47
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x45
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __DIVF21
	STS  _A_G7,R30
	STS  _A_G7+1,R31
	STS  _A_G7+2,R22
	STS  _A_G7+3,R23
;    2492         B = ( (A * ((float)Xt3 - (float)Xt2)) + (float)Xd2 - (float)Xd3) / ((float)Yt2 - (float)Yt3);
	CALL SUBOPT_0x48
	LDS  R26,_A_G7
	LDS  R27,_A_G7+1
	LDS  R24,_A_G7+2
	LDS  R25,_A_G7+3
	CALL __MULF12
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+17
	LDD  R31,Y+17+1
	CALL SUBOPT_0x49
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	CALL SUBOPT_0x4A
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __DIVF21
	STS  _B_G7,R30
	STS  _B_G7+1,R31
	STS  _B_G7+2,R22
	STS  _B_G7+3,R23
;    2493         C = (float)Xd3 - (A * (float)Xt3) - (B * (float)Yt3);  
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETW1R 20,21
	CALL SUBOPT_0x37
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	CALL SUBOPT_0x38
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	STS  _C_G7,R30
	STS  _C_G7+1,R31
	STS  _C_G7+2,R22
	STS  _C_G7+3,R23
;    2494         D = ((((float)Yd1 - (float)Yd3) * ((float)Yt2 - (float)Yt3)) - (((float)Yd2 - (float)Yd3) * ((float)Yt1 - (float)Yt3))) / ((((float)Xt1 - (float)Xt3) * ((float)Yt2 - (float)Yt3)) - (((float)Xt2 - (float)Xt3) * ((float)Yt1 - (float)Yt3)));
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	CALL SUBOPT_0x4B
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	LDD  R30,Y+11
	LDD  R31,Y+11+1
	CALL SUBOPT_0x4B
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x45
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x46
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x47
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x45
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __DIVF21
	STS  _D_G7,R30
	STS  _D_G7+1,R31
	STS  _D_G7+2,R22
	STS  _D_G7+3,R23
;    2495         E = ((D * ((float)Xt3 - (float)Xt2)) + (float)Yd2 - (float)Yd3) / ((float)Yt2 - (float)Yt3);
	CALL SUBOPT_0x48
	LDS  R26,_D_G7
	LDS  R27,_D_G7+1
	LDS  R24,_D_G7+2
	LDS  R25,_D_G7+3
	CALL __MULF12
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+11
	LDD  R31,Y+11+1
	CALL SUBOPT_0x49
	LDD  R30,Y+9
	LDD  R31,Y+9+1
	CALL SUBOPT_0x4A
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	CALL SUBOPT_0x44
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __DIVF21
	STS  _E_G7,R30
	STS  _E_G7+1,R31
	STS  _E_G7+2,R22
	STS  _E_G7+3,R23
;    2496         F = (float)Yd3 - (D * (float)Xt3) - (E * (float)Yt3);
	LDD  R30,Y+9
	LDD  R31,Y+9+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETW1R 20,21
	CALL SUBOPT_0x39
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	CALL SUBOPT_0x3A
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	CALL __SWAPD12
	CALL __SUBF12
	STS  _F_G7,R30
	STS  _F_G7+1,R31
	STS  _F_G7+2,R22
	STS  _F_G7+3,R23
;    2497        
;    2498         
;    2499         //Sla waardes op in de EEPROM
;    2500         ee_float1 = A;
	LDS  R30,_A_G7
	LDS  R31,_A_G7+1
	LDS  R22,_A_G7+2
	LDS  R23,_A_G7+3
	LDI  R26,LOW(_ee_float1)
	LDI  R27,HIGH(_ee_float1)
	CALL __EEPROMWRD
;    2501         ee_float2 = B;
	LDS  R30,_B_G7
	LDS  R31,_B_G7+1
	LDS  R22,_B_G7+2
	LDS  R23,_B_G7+3
	LDI  R26,LOW(_ee_float2)
	LDI  R27,HIGH(_ee_float2)
	CALL __EEPROMWRD
;    2502         ee_float3 = C;
	LDS  R30,_C_G7
	LDS  R31,_C_G7+1
	LDS  R22,_C_G7+2
	LDS  R23,_C_G7+3
	LDI  R26,LOW(_ee_float3)
	LDI  R27,HIGH(_ee_float3)
	CALL __EEPROMWRD
;    2503         ee_float4 = D;
	LDS  R30,_D_G7
	LDS  R31,_D_G7+1
	LDS  R22,_D_G7+2
	LDS  R23,_D_G7+3
	LDI  R26,LOW(_ee_float4)
	LDI  R27,HIGH(_ee_float4)
	CALL __EEPROMWRD
;    2504         ee_float5 = E;
	LDS  R30,_E_G7
	LDS  R31,_E_G7+1
	LDS  R22,_E_G7+2
	LDS  R23,_E_G7+3
	LDI  R26,LOW(_ee_float5)
	LDI  R27,HIGH(_ee_float5)
	CALL __EEPROMWRD
;    2505         ee_float6 = F;
	LDS  R30,_F_G7
	LDS  R31,_F_G7+1
	LDS  R22,_F_G7+2
	LDS  R23,_F_G7+3
	LDI  R26,LOW(_ee_float6)
	LDI  R27,HIGH(_ee_float6)
	CALL __EEPROMWRD
;    2506 
;    2507         //Meten punt 3 klaar
;    2508         //Wis kruis meetpunt 3
;    2509         //S1D13705_Draw_Horizontal_Line(27,35,215,15);                  //Stuur kleur wit om de pixels te wissen         
;    2510         //S1D13705_Draw_Vertical_Line(31,211,219,15);                                                       
;    2511         S1D13705_Draw_Horizontal_Line(283,291,119,15);                  //Stuur kleur wit om de pixels te wissen       
	CALL SUBOPT_0x41
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Horizontal_Line
;    2512         S1D13705_Draw_Vertical_Line(287,115,123,15);                                                    
	CALL SUBOPT_0x42
	CALL SUBOPT_0x8
	CALL _S1D13705_Draw_Vertical_Line
;    2513 
;    2514         //Zet Flag in register dat calibratie is gedaan en wis calprocces
;    2515         Touchscreen.Flag_Register &= ~(1<<TouchCalProcess);             //Wis de TouchCalProcess bit      
	LDS  R30,_Touchscreen
	ANDI R30,0xFB
	STS  _Touchscreen,R30
;    2516         Touchscreen.Flag_Register &= ~(1<<TouchPressed);                //Wis de TouchPressed bit
	CALL SUBOPT_0x36
;    2517         Touchscreen.Flag_Register |= (1<<TouchCalibratie);              
	LDS  R30,_Touchscreen
	ORI  R30,2
	STS  _Touchscreen,R30
;    2518         ee_Touch_Flag = Touchscreen.Flag_Register;                      //Sla Flag register op in eeprom
	LDI  R26,LOW(_ee_Touch_Flag)
	LDI  R27,HIGH(_ee_Touch_Flag)
	CALL __EEPROMWRB
;    2519 
;    2520         return;
	CALL __LOADLOCR6
	ADIW R28,27
	RET
;    2521 }                                         
;    2522 //*************************************
;    2523 /**************************************
;    2524           ADC LTC 1298 driver
;    2525 **************************************/
;    2526 #include "lcd_test_defs.h"             
;    2527 #include "adc_ltc1298.h"
;    2528 
;    2529 /**************************************
;    2530 Function: ADC On 
;    2531 **************************************/
;    2532 void ADC_LTC1298_On(void)
;    2533 {                                                
_ADC_LTC1298_On:
;    2534         //SPI clock 125Khz
;    2535         //SPI_CLK_125K;  
;    2536         
;    2537         //Zet ADC actief
;    2538         ADC_LTC1298_Power_On;
	CBI  0x7,2
;    2539         
;    2540         return;
	RET
;    2541 }
;    2542 //*************************************
;    2543 
;    2544 /**************************************
;    2545 Function: ADC off
;    2546 **************************************/
;    2547 void ADC_LTC1298_Off(void)
;    2548 {
_ADC_LTC1298_Off:
;    2549         //Set ADC off
;    2550         ADC_LTC1298_Power_Off;
	SBI  0x7,2
;    2551         
;    2552         return;
	RET
;    2553 }
;    2554 //*************************************
;    2555 
;    2556 /**************************************
;    2557 Function: ADC measure data
;    2558 **************************************/
;    2559 unsigned int ADC_LTC1298_Data(unsigned char Channel)
;    2560 {
_ADC_LTC1298_Data:
;    2561         unsigned char LowByte;
;    2562         unsigned int HighByte;
;    2563         unsigned int test;   
;    2564         unsigned int test2;
;    2565         
;    2566         ADC_LTC1298_On();                                       //Set ADC active
	SBIW R28,2
	CALL __SAVELOCR5
;	Channel -> Y+7
;	LowByte -> R16
;	HighByte -> R17,R18
;	test -> R19,R20
;	test2 -> Y+5
	CALL _ADC_LTC1298_On
;    2567         SPI_ON;                                                 //Set SPI on
	SBI  0xD,6
;    2568         
;    2569         //Check wich channel must be measure
;    2570         if(Channel == 0)                                        //Channel 0
	LDD  R30,Y+7
	CPI  R30,0
	BRNE _0x19D
;    2571                 spi(ADC_LTC1298_Channel_0);                     //Zend starbit en config voor channel 0
	LDI  R30,LOW(13)
	RJMP _0x24E
;    2572         else                                                    //Channel 1
_0x19D:
;    2573                 spi(ADC_LTC1298_Channel_1);                     //Zend startbit en config voor channel 1
	LDI  R30,LOW(15)
_0x24E:
	ST   -Y,R30
	CALL _spi
;    2574                
;    2575         //Ontvang 12bit data, MSB eerst
;    2576         HighByte = spi(0xFF);        
	CALL SUBOPT_0x4C
	LDI  R31,0
	__PUTW1R 17,18
;    2577         LowByte = spi(0xFF);                                    //Send dummy data on the spi to create CLK    
	CALL SUBOPT_0x4C
	MOV  R16,R30
;    2578         
;    2579         ADC_LTC1298_Off();                                      //Set ADC inactive
	CALL _ADC_LTC1298_Off
;    2580         SPI_OFF;                                                //Set SPI off
	CBI  0xD,6
;    2581         
;    2582         //Schuif HighByte naar hoge byte daarna lowbyte dan 
;    2583         //correctie voor 12bit waarde, 3 naar rechts schuiven
;    2584         return(((HighByte << 8) | LowByte) >> 3);               //Return with measure data
	__GETW1R 17,18
	MOV  R31,R30
	LDI  R30,0
	MOVW R26,R30
	MOV  R30,R16
	LDI  R31,0
	OR   R30,R26
	OR   R31,R27
	CALL __LSRW3
	CALL __LOADLOCR5
	ADIW R28,8
	RET
;    2585 }
;    2586 //*************************************
;    2587 

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
__put_G9:
	put:
	LD   R26,Y
	LDD  R27,Y+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x19F
	CALL SUBOPT_0x4D
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x1A0
_0x19F:
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _putchar
_0x1A0:
	ADIW R28,3
	RET
__print_G9:
	SBIW R28,6
	CALL __SAVELOCR6
	LDI  R16,0
_0x1A1:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+3
	JMP _0x1A3
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x1A7
	CPI  R19,37
	BRNE _0x1A8
	LDI  R16,LOW(1)
	RJMP _0x1A9
_0x1A8:
	CALL SUBOPT_0x4E
_0x1A9:
	RJMP _0x1A6
_0x1A7:
	CPI  R30,LOW(0x1)
	BRNE _0x1AA
	CPI  R19,37
	BRNE _0x1AB
	CALL SUBOPT_0x4E
	LDI  R16,LOW(0)
	RJMP _0x1A6
_0x1AB:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x1AC
	LDI  R17,LOW(1)
	RJMP _0x1A6
_0x1AC:
	CPI  R19,43
	BRNE _0x1AD
	LDI  R21,LOW(43)
	RJMP _0x1A6
_0x1AD:
	CPI  R19,32
	BRNE _0x1AE
	LDI  R21,LOW(32)
	RJMP _0x1A6
_0x1AE:
	RJMP _0x1AF
_0x1AA:
	CPI  R30,LOW(0x2)
	BRNE _0x1B0
_0x1AF:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x1B1
	ORI  R17,LOW(128)
	RJMP _0x1A6
_0x1B1:
	RJMP _0x1B2
_0x1B0:
	CPI  R30,LOW(0x3)
	BREQ PC+3
	JMP _0x1A6
_0x1B2:
	CPI  R19,48
	BRLO _0x1B5
	CPI  R19,58
	BRLO _0x1B6
_0x1B5:
	RJMP _0x1B4
_0x1B6:
	CALL SUBOPT_0x4F
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x1A6
_0x1B4:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x1BA
	CALL SUBOPT_0x50
	LD   R30,X
	CALL SUBOPT_0x51
	RJMP _0x1BB
_0x1BA:
	CPI  R30,LOW(0x73)
	BRNE _0x1BD
	CALL SUBOPT_0x50
	CALL SUBOPT_0x52
	CALL _strlen
	MOV  R16,R30
	RJMP _0x1BE
_0x1BD:
	CPI  R30,LOW(0x70)
	BRNE _0x1C0
	CALL SUBOPT_0x50
	CALL SUBOPT_0x52
	CALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x1BE:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x1C1
_0x1C0:
	CPI  R30,LOW(0x64)
	BREQ _0x1C4
	CPI  R30,LOW(0x69)
	BRNE _0x1C5
_0x1C4:
	ORI  R17,LOW(4)
	RJMP _0x1C6
_0x1C5:
	CPI  R30,LOW(0x75)
	BRNE _0x1C7
_0x1C6:
	LDI  R30,LOW(_tbl10_G9*2)
	LDI  R31,HIGH(_tbl10_G9*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x1C8
_0x1C7:
	CPI  R30,LOW(0x58)
	BRNE _0x1CA
	ORI  R17,LOW(8)
	RJMP _0x1CB
_0x1CA:
	CPI  R30,LOW(0x78)
	BREQ PC+3
	JMP _0x1F9
_0x1CB:
	LDI  R30,LOW(_tbl16_G9*2)
	LDI  R31,HIGH(_tbl16_G9*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x1C8:
	SBRS R17,2
	RJMP _0x1CD
	CALL SUBOPT_0x50
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x1CE
	CALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x1CE:
	CPI  R21,0
	BREQ _0x1CF
	SUBI R16,-LOW(1)
	RJMP _0x1D0
_0x1CF:
	ANDI R17,LOW(251)
_0x1D0:
	RJMP _0x1D1
_0x1CD:
	CALL SUBOPT_0x50
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0x1D1:
_0x1C1:
	SBRC R17,0
	RJMP _0x1D2
_0x1D3:
	CP   R16,R20
	BRSH _0x1D5
	SBRS R17,7
	RJMP _0x1D6
	SBRS R17,2
	RJMP _0x1D7
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x1D8
_0x1D7:
	LDI  R19,LOW(48)
_0x1D8:
	RJMP _0x1D9
_0x1D6:
	LDI  R19,LOW(32)
_0x1D9:
	CALL SUBOPT_0x4E
	SUBI R20,LOW(1)
	RJMP _0x1D3
_0x1D5:
_0x1D2:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x1DA
_0x1DB:
	CPI  R18,0
	BREQ _0x1DD
	SBRS R17,3
	RJMP _0x1DE
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x24F
_0x1DE:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x24F:
	ST   -Y,R30
	CALL SUBOPT_0x53
	CPI  R20,0
	BREQ _0x1E0
	SUBI R20,LOW(1)
_0x1E0:
	SUBI R18,LOW(1)
	RJMP _0x1DB
_0x1DD:
	RJMP _0x1E1
_0x1DA:
_0x1E3:
	LDI  R19,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,2
	CALL __GETW1PF
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
	BRSH _0x1E5
	SBRS R17,3
	RJMP _0x1E6
	SUBI R19,-LOW(7)
	RJMP _0x1E7
_0x1E6:
	SUBI R19,-LOW(39)
_0x1E7:
_0x1E5:
	SBRC R17,4
	RJMP _0x1E9
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x1EB
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x1EA
_0x1EB:
	ORI  R17,LOW(16)
	RJMP _0x1ED
_0x1EA:
	CP   R20,R18
	BRLO _0x1EF
	SBRS R17,0
	RJMP _0x1F0
_0x1EF:
	RJMP _0x1EE
_0x1F0:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x1F1
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x1ED:
	SBRS R17,2
	RJMP _0x1F2
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0x53
	CPI  R20,0
	BREQ _0x1F3
	SUBI R20,LOW(1)
_0x1F3:
_0x1F2:
_0x1F1:
_0x1E9:
	RCALL SUBOPT_0x4E
	CPI  R20,0
	BREQ _0x1F4
	SUBI R20,LOW(1)
_0x1F4:
_0x1EE:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x1E4
	RJMP _0x1E3
_0x1E4:
_0x1E1:
	SBRS R17,0
	RJMP _0x1F5
_0x1F6:
	CPI  R20,0
	BREQ _0x1F8
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x51
	RJMP _0x1F6
_0x1F8:
_0x1F5:
_0x1F9:
_0x1BB:
	LDI  R16,LOW(0)
_0x1A6:
	RJMP _0x1A1
_0x1A3:
	CALL __LOADLOCR6
	ADIW R28,18
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x54
	MOVW R26,R28
	ADIW R26,6
	CALL __ADDW2R15
	CALL __GETW1P
	STD  Y+2,R30
	STD  Y+2+1,R31
	RCALL SUBOPT_0x55
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	RJMP _0x243
_printf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x54
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
	RCALL SUBOPT_0x55
	RJMP _0x243
__get_G9:
	ST   -Y,R16
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LD   R30,X
	MOV  R16,R30
	CPI  R30,0
	BREQ _0x1FA
	LDI  R30,LOW(0)
	ST   X,R30
	RJMP _0x1FB
_0x1FA:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x1FC
	CALL __GETW1P
	LD   R30,Z
	MOV  R16,R30
	CPI  R30,0
	BREQ _0x1FD
	RCALL SUBOPT_0x4D
_0x1FD:
	RJMP _0x1FE
_0x1FC:
	CALL _getchar
	MOV  R16,R30
_0x1FE:
_0x1FB:
	MOV  R30,R16
	LDD  R16,Y+0
	ADIW R28,5
	RET
__scanf_G9:
	SBIW R28,4
	CALL __SAVELOCR6
	LDI  R30,LOW(0)
	STD  Y+9,R30
	MOV  R21,R30
_0x1FF:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	ADIW R30,1
	STD  Y+14,R30
	STD  Y+14+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R18,R30
	CPI  R30,0
	BRNE PC+3
	JMP _0x201
	RCALL SUBOPT_0x56
	BREQ _0x202
_0x203:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	MOV  R18,R30
	CPI  R30,0
	BREQ _0x206
	RCALL SUBOPT_0x56
	BRNE _0x207
_0x206:
	RJMP _0x205
_0x207:
	RJMP _0x203
_0x205:
	MOV  R21,R18
	RJMP _0x208
_0x202:
	CPI  R18,37
	BREQ PC+3
	JMP _0x209
	LDI  R20,LOW(0)
_0x20A:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	LPM  R18,Z+
	STD  Y+14,R30
	STD  Y+14+1,R31
	CPI  R18,48
	BRLO _0x20E
	LDI  R30,LOW(57)
	CP   R30,R18
	BRSH _0x20D
_0x20E:
	RJMP _0x20C
_0x20D:
	RCALL SUBOPT_0x4F
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x20A
_0x20C:
	CPI  R18,0
	BRNE _0x210
	RJMP _0x201
_0x210:
_0x211:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	MOV  R19,R30
	ST   -Y,R30
	CALL _isspace
	CPI  R30,0
	BRNE _0x211
	CPI  R19,0
	BRNE _0x214
	RJMP _0x215
_0x214:
	MOV  R21,R19
	CPI  R20,0
	BRNE _0x216
	LDI  R20,LOW(255)
_0x216:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0x21A
	RCALL SUBOPT_0x58
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	__GETW2R 16,17
	ST   X,R30
	RJMP _0x219
_0x21A:
	CPI  R30,LOW(0x73)
	BRNE _0x222
	RCALL SUBOPT_0x58
_0x21C:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0x21E
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	MOV  R18,R30
	CPI  R30,0
	BREQ _0x220
	RCALL SUBOPT_0x56
	BREQ _0x21F
_0x220:
	RJMP _0x21E
_0x21F:
	PUSH R17
	PUSH R16
	__ADDWRN 16,17,1
	MOV  R30,R18
	POP  R26
	POP  R27
	ST   X,R30
	RJMP _0x21C
_0x21E:
	__GETW2R 16,17
	LDI  R30,LOW(0)
	ST   X,R30
	RJMP _0x219
_0x222:
	LDI  R30,LOW(1)
	STD  Y+8,R30
	MOV  R30,R18
	CPI  R30,LOW(0x64)
	BREQ _0x227
	CPI  R30,LOW(0x69)
	BRNE _0x228
_0x227:
	LDI  R30,LOW(0)
	STD  Y+8,R30
	RJMP _0x229
_0x228:
	CPI  R30,LOW(0x75)
	BRNE _0x22A
_0x229:
	LDI  R19,LOW(10)
	RJMP _0x225
_0x22A:
	CPI  R30,LOW(0x78)
	BRNE _0x22B
	LDI  R19,LOW(16)
	RJMP _0x225
_0x22B:
	CPI  R30,LOW(0x25)
	BRNE _0x22E
	RJMP _0x22D
_0x22E:
	LDD  R30,Y+9
	RJMP _0x244
_0x225:
	LDI  R30,0
	STD  Y+6,R30
	STD  Y+6+1,R30
_0x22F:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0x231
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	MOV  R18,R30
	MOV  R26,R30
	LDI  R30,LOW(32)
	CP   R30,R26
	BRSH _0x233
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0x234
	CPI  R18,45
	BRNE _0x235
	LDI  R30,LOW(255)
	STD  Y+8,R30
	RJMP _0x22F
_0x235:
	LDI  R30,LOW(1)
	STD  Y+8,R30
_0x234:
	CPI  R18,48
	BRLO _0x233
	CPI  R18,97
	BRLO _0x238
	SUBI R18,LOW(87)
	RJMP _0x239
_0x238:
	CPI  R18,65
	BRLO _0x23A
	SUBI R18,LOW(55)
	RJMP _0x23B
_0x23A:
	SUBI R18,LOW(48)
_0x23B:
_0x239:
	CP   R18,R19
	BRLO _0x23C
_0x233:
	MOV  R21,R18
	RJMP _0x231
_0x23C:
	MOV  R30,R19
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R31,0
	CALL __MULW12U
	MOVW R26,R30
	MOV  R30,R18
	LDI  R31,0
	ADD  R30,R26
	ADC  R31,R27
	STD  Y+6,R30
	STD  Y+6+1,R31
	RJMP _0x22F
_0x231:
	RCALL SUBOPT_0x58
	LDD  R30,Y+8
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R31,0
	SBRC R30,7
	SER  R31
	CALL __MULW12U
	__GETW2R 16,17
	ST   X+,R30
	ST   X,R31
_0x219:
	LDD  R30,Y+9
	SUBI R30,-LOW(1)
	STD  Y+9,R30
	RJMP _0x23D
_0x209:
_0x22D:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x57
	POP  R21
	CP   R30,R18
	BREQ _0x23E
_0x215:
	LDD  R30,Y+9
	CPI  R30,0
	BRNE _0x23F
	LDI  R30,LOW(255)
	RJMP _0x244
_0x23F:
	RJMP _0x201
_0x23E:
_0x23D:
_0x208:
	RJMP _0x1FF
_0x201:
	LDD  R30,Y+9
_0x244:
	CALL __LOADLOCR6
	ADIW R28,16
	RET
_scanf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x54
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
	MOVW R26,R28
	ADIW R26,4
	CALL __ADDW2R15
	CALL __GETW1P
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	RCALL __scanf_G9
_0x243:
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	POP  R15
	RET
_spi:
	LD   R30,Y
	OUT  0xF,R30
_0x240:
	SBIS 0xE,7
	RJMP _0x240
	IN   R30,0xF
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x0:
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 16,17
	CALL __CWD1
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1:
	SBIW R28,4
	ST   -Y,R17
	ST   -Y,R16
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	LDS  R30,_Touchscreen
	ORI  R30,1
	STS  _Touchscreen,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x3:
	IN   R30,0x3A
	ORI  R30,0x40
	OUT  0x3A,R30
	IN   R30,0x3B
	ORI  R30,0x40
	OUT  0x3B,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x4:
	SBIW R30,3
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x5:
	ADIW R30,3
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x6:
	CALL __PUTPARD1
	LDI  R30,LOW(0)
	ST   -Y,R30
	JMP  _S1D13705_Draw_Horizontal_Line

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x7:
	LDI  R30,LOW(0)
	ST   -Y,R30
	JMP  _S1D13705_Draw_Vertical_Line

;OPTIMIZER ADDED SUBROUTINE, CALLED 8 TIMES
SUBOPT_0x8:
	CALL __PUTPARD1
	LDI  R30,LOW(15)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 11 TIMES
SUBOPT_0x9:
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	RCALL _sprintf
	ADIW R28,4
	__GETD1N 0x1
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 10 TIMES
SUBOPT_0xB:
	CALL __PUTPARD1
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	RJMP SUBOPT_0x9

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	CALL _S1D13705_Text_LCD
	LDI  R30,LOW(_string)
	LDI  R31,HIGH(_string)
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	RCALL _printf
	ADIW R28,2
	JMP  _Touchscreen_Calibratie

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R16
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	LDI  R30,LOW(255)
	OUT  0x1A,R30
	__GETD1S 5
	__ANDD1N 0x10000
	__CPD1N 0x10000
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	LDD  R30,Y+5
	LDD  R31,Y+5+1
	MOV  R30,R31
	__ANDD1N 0xFF
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	LDD  R30,Y+1
	__GETD2N 0x1FFE0
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __ADDD12
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	LDD  R30,Y+4
	ST   -Y,R30
	JMP  _S1D13705_Write_Data

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	LDD  R26,Y+12
	LDD  R27,Y+12+1
	LD   R30,X
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	LDD  R26,Y+12
	LDD  R27,Y+12+1
	LD   R20,X+
	STD  Y+12,R26
	STD  Y+12+1,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	LDI  R26,LOW(_FontTable*2)
	LDI  R27,HIGH(_FontTable*2)
	MOV  R30,R20
	SUBI R30,LOW(32)
	LDI  R31,0
	CALL __LSLW3
	ADD  R30,R26
	ADC  R31,R27
	ADD  R30,R16
	ADC  R31,R17
	LPM  R18,Z
	__GETW2R 16,17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x16:
	CALL __MULW12U
	__GETD2S 18
	CLR  R22
	CLR  R23
	CALL __ADDD12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x17:
	LDD  R30,Y+11
	SWAP R30
	ANDI R30,0xF0
	ORI  R30,LOW(0xF)
	MOV  R19,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x18:
	__GETD1S 7
	__SUBD1N -1
	__PUTD1S 7
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	CALL __PUTPARD1
	ST   -Y,R19
	JMP  _S1D13705_Write_Data

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x19:
	CALL __PUTPARD1
	LDD  R30,Y+10
	ST   -Y,R30
	JMP  _S1D13705_Write_Data

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1A:
	__GETD1S 2
	__SUBD1N -1
	__PUTD1S 2
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1B:
	__GETD1S 0
	__GETD2S 13
	CALL __ADDD21
	__GETD1N 0x140
	CALL __MULD12U
	__GETD2S 17
	CALL __ADDD12
	__PUTD1S 4
	CALL __PUTPARD1
	LDD  R30,Y+12
	ST   -Y,R30
	JMP  _S1D13705_Write_Data

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1C:
	__GETD1S 0
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	__PUTD1S 0
	__SUBD1N -1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1D:
	__GETD1S 5
	__SUBD1N -1
	__PUTD1S 5
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1E:
	LDI  R30,LOW(21)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	JMP  _S1D13705_Write_Register

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1F:
	LDI  R30,LOW(23)
	ST   -Y,R30
	__GETW1R 16,17
	LPM  R30,Z
	ST   -Y,R30
	JMP  _S1D13705_Write_Register

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x20:
	LDI  R30,LOW(7)
	ST   -Y,R30
	CALL _CF_Read_Register
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x21:
	LDI  R30,LOW(0)
	ORI  R30,1
	MOV  R26,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x22:
	LDI  R30,LOW(1)
	ORI  R30,1
	MOV  R26,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x23:
	SBIW R28,3
	ST   -Y,R17
	ST   -Y,R16
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x24:
	CALL _CF_Wait_RDY
	__GETD1S 5
	__ANDD1N 0xFF
	STD  Y+2,R30
	__GETD1S 5
	__ANDD1N 0xFF00
	MOVW R26,R30
	MOVW R24,R22
	LDI  R30,LOW(8)
	CALL __LSRD12
	STD  Y+3,R30
	__GETD1S 5
	__ANDD1N 0xFF0000
	CALL __LSRD16
	STD  Y+4,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _CF_Write_Register
	LDI  R30,LOW(3)
	ST   -Y,R30
	LDD  R30,Y+3
	ST   -Y,R30
	CALL _CF_Write_Register
	LDI  R30,LOW(4)
	ST   -Y,R30
	LDD  R30,Y+4
	ST   -Y,R30
	CALL _CF_Write_Register
	LDI  R30,LOW(5)
	ST   -Y,R30
	LDD  R30,Y+5
	ST   -Y,R30
	CALL _CF_Write_Register
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(224)
	ST   -Y,R30
	CALL _CF_Write_Register
	LDI  R30,LOW(7)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x25:
	ST   -Y,R30
	CALL _CF_Write_Register
	JMP  _CF_Wait_DRQ

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x26:
	MOV  R26,R16
	LDI  R27,0
	SUBI R26,LOW(-_CF_Page)
	SBCI R27,HIGH(-_CF_Page)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x27:
	__GETD2N 0x98
	CALL __MULD12U
	__ADDD1N 1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x28:
	__GETD1S 9
	__SUBD1N -1
	__PUTD1S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x29:
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	CALL __PUTPARD1
	JMP  _CF_Read_Sector

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2A:
	RCALL _printf
	ADIW R28,2
	LDI  R30,LOW(2)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2B:
	ST   -Y,R30
	CALL _S1D13705_Write_Register
	LDI  R30,LOW(28)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2C:
	ST   -Y,R30
	CALL _S1D13705_Write_Register
	RJMP SUBOPT_0x1E

;OPTIMIZER ADDED SUBROUTINE, CALLED 9 TIMES
SUBOPT_0x2D:
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ADIW R30,1
	STD  Y+15,R30
	STD  Y+15+1,R31
	SBIW R30,1
	SUBI R30,LOW(-_CF_Sector_Data)
	SBCI R31,HIGH(-_CF_Sector_Data)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x2E:
	LD   R18,Z
	LDI  R30,LOW(23)
	ST   -Y,R30
	ST   -Y,R18
	CALL _S1D13705_Write_Register
	LDI  R30,LOW(23)
	ST   -Y,R30
	ST   -Y,R17
	CALL _S1D13705_Write_Register
	LDI  R30,LOW(23)
	ST   -Y,R30
	ST   -Y,R16
	JMP  _S1D13705_Write_Register

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x2F:
	RCALL _sprintf
	ADIW R28,4
	__GETD1N 0x0
	CALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x30:
	CALL _TX_Ready_Sign
	__GETW1R 17,18
	SUBI R30,LOW(-_CF_Sector_Data)
	SBCI R31,HIGH(-_CF_Sector_Data)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x31:
	__GETD1S 5
	CALL __PUTPARD1
	CALL _CF_Write_Sector
	__GETD1S 5
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x32:
	LDI  R30,LOW(0)
	OR   R30,R26
	MOV  R26,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x33:
	LDI  R30,LOW(0)
	OR   R30,R26
	LDS  R30,_Touchscreen
	ANDI R30,0xFE
	STS  _Touchscreen,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x34:
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x35:
	CALL _Touchscreen_Pressed
	STD  Y+6,R30
	CPI  R30,LOW(0x1)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x36:
	LDS  R30,_Touchscreen
	ANDI R30,0xFE
	STS  _Touchscreen,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x37:
	CLR  R22
	CLR  R23
	CALL __CDF1
	LDS  R26,_A_G7
	LDS  R27,_A_G7+1
	LDS  R24,_A_G7+2
	LDS  R25,_A_G7+3
	CALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x38:
	CLR  R22
	CLR  R23
	CALL __CDF1
	LDS  R26,_B_G7
	LDS  R27,_B_G7+1
	LDS  R24,_B_G7+2
	LDS  R25,_B_G7+3
	CALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x39:
	CLR  R22
	CLR  R23
	CALL __CDF1
	LDS  R26,_D_G7
	LDS  R27,_D_G7+1
	LDS  R24,_D_G7+2
	LDS  R25,_D_G7+3
	CALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3A:
	CLR  R22
	CLR  R23
	CALL __CDF1
	LDS  R26,_E_G7
	LDS  R27,_E_G7+1
	LDS  R24,_E_G7+2
	LDS  R25,_E_G7+3
	CALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3B:
	__GETD1N 0x1B
	CALL __PUTPARD1
	__GETD1N 0x23
	CALL __PUTPARD1
	__GETD1N 0xD7
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3C:
	__GETD1N 0x1F
	CALL __PUTPARD1
	__GETD1N 0xD3
	CALL __PUTPARD1
	__GETD1N 0xDB
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3D:
	LDD  R30,Y+8
	ST   -Y,R30
	JMP  _Touchscreen_Value

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3E:
	LDD  R30,Y+7
	ST   -Y,R30
	JMP  _Touchscreen_Value

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3F:
	__GETD1N 0x9B
	CALL __PUTPARD1
	__GETD1N 0xA3
	CALL __PUTPARD1
	__GETD1N 0x17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x40:
	__GETD1N 0x9F
	CALL __PUTPARD1
	__GETD1N 0x13
	CALL __PUTPARD1
	__GETD1N 0x1B
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x41:
	__GETD1N 0x11B
	CALL __PUTPARD1
	__GETD1N 0x123
	CALL __PUTPARD1
	__GETD1N 0x77
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x42:
	__GETD1N 0x11F
	CALL __PUTPARD1
	__GETD1N 0x73
	CALL __PUTPARD1
	__GETD1N 0x7B
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x43:
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x44:
	LDD  R30,Y+23
	LDD  R31,Y+23+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x45:
	LDD  R30,Y+25
	LDD  R31,Y+25+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+21
	LDD  R31,Y+21+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x46:
	__GETW1R 16,17
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETW1R 20,21
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x47:
	__GETW1R 18,19
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETW1R 20,21
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x48:
	__GETW1R 20,21
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETW1R 18,19
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x49:
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __ADDF12
	MOVW R26,R30
	MOVW R24,R22
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x4A:
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4B:
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	LDD  R30,Y+9
	LDD  R31,Y+9+1
	RJMP SUBOPT_0x4A

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4C:
	LDI  R30,LOW(255)
	ST   -Y,R30
	RJMP _spi

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4D:
	CALL __GETW1P
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x4E:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G9

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4F:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x50:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x51:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G9

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x52:
	CALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x53:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G9

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x54:
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	__PUTW2R 16,17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x55:
	MOVW R26,R28
	ADIW R26,4
	CALL __ADDW2R15
	CALL __GETW1P
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	JMP  __print_G9

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x56:
	ST   -Y,R18
	CALL _isspace
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x57:
	LDD  R30,Y+12
	LDD  R31,Y+12+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __get_G9

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x58:
	LDD  R26,Y+12
	LDD  R27,Y+12+1
	SBIW R26,4
	STD  Y+12,R26
	STD  Y+12+1,R27
	ADIW R26,4
	LD   R16,X+
	LD   R17,X
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

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0xFA0
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

__ADDD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	ADC  R23,R25
	RET

__ADDD21:
	ADD  R26,R30
	ADC  R27,R31
	ADC  R24,R22
	ADC  R25,R23
	RET

__SUBD12:
	SUB  R30,R26
	SBC  R31,R27
	SBC  R22,R24
	SBC  R23,R25
	RET

__SUBD21:
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SBC  R25,R23
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

__LSRD12:
	TST  R30
	MOV  R0,R30
	MOVW R30,R26
	MOVW R22,R24
	BREQ __LSRD12R
__LSRD12L:
	LSR  R23
	ROR  R22
	ROR  R31
	ROR  R30
	DEC  R0
	BRNE __LSRD12L
__LSRD12R:
	RET

__LSLW3:
	LSL  R30
	ROL  R31
__LSLW2:
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
	RET

__LSRW3:
	LSR  R31
	ROR  R30
__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
	RET

__LSRD16:
	MOV  R30,R22
	MOV  R31,R23
	LDI  R22,0
	LDI  R23,0
	RET

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
	RET

__EQB12:
	CP   R30,R26
	LDI  R30,1
	BREQ __EQB12T
	CLR  R30
__EQB12T:
	RET

__LTW12:
	CP   R26,R30
	CPC  R27,R31
	LDI  R30,1
	BRLT __LTW12T
	CLR  R30
__LTW12T:
	RET

__GTW12:
	CP   R30,R26
	CPC  R31,R27
	LDI  R30,1
	BRLT __GTW12T
	CLR  R30
__GTW12T:
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

__MULD12U:
	MUL  R23,R26
	MOV  R23,R0
	MUL  R22,R27
	ADD  R23,R0
	MUL  R31,R24
	ADD  R23,R0
	MUL  R30,R25
	ADD  R23,R0
	MUL  R22,R26
	MOV  R22,R0
	ADD  R23,R1
	MUL  R31,R27
	ADD  R22,R0
	ADC  R23,R1
	MUL  R30,R24
	ADD  R22,R0
	ADC  R23,R1
	CLR  R24
	MUL  R31,R26
	MOV  R31,R0
	ADD  R22,R1
	ADC  R23,R24
	MUL  R30,R27
	ADD  R31,R0
	ADC  R22,R1
	ADC  R23,R24
	MUL  R30,R26
	MOV  R30,R0
	ADD  R31,R1
	ADC  R22,R24
	ADC  R23,R24
	RET

__DIVW21U:
	CLR  R0
	CLR  R1
	LDI  R25,16
__DIVW21U1:
	LSL  R26
	ROL  R27
	ROL  R0
	ROL  R1
	SUB  R0,R30
	SBC  R1,R31
	BRCC __DIVW21U2
	ADD  R0,R30
	ADC  R1,R31
	RJMP __DIVW21U3
__DIVW21U2:
	SBR  R26,1
__DIVW21U3:
	DEC  R25
	BRNE __DIVW21U1
	MOVW R30,R26
	MOVW R26,R0
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

__PUTPARD1LDEC:
	SBIW R30,1
__PUTPARD1L:
	LDI  R22,0
	LDI  R23,0
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

__EEPROMRDD:
	ADIW R26,2
	RCALL __EEPROMRDW
	MOV  R23,R31
	MOV  R22,R30
	SBIW R26,2

__EEPROMRDW:
	ADIW R26,1
	RCALL __EEPROMRDB
	MOV  R31,R30
	SBIW R26,1

__EEPROMRDB:
	SBIC EECR,EEWE
	RJMP __EEPROMRDB
	PUSH R31
	IN   R31,SREG
	CLI
	OUT  EEARL,R26
	OUT  EEARH,R27
	SBI  EECR,EERE
	IN   R30,EEDR
	OUT  SREG,R31
	POP  R31
	RET

__EEPROMWRD:
	RCALL __EEPROMWRW
	ADIW R26,2
	MOV  R0,R30
	MOV  R1,R31
	MOV  R30,R22
	MOV  R31,R23
	RCALL __EEPROMWRW
	MOV  R30,R0
	MOV  R31,R1
	SBIW R26,2
	RET

__EEPROMWRW:
	RCALL __EEPROMWRB
	ADIW R26,1
	PUSH R30
	MOV  R30,R31
	RCALL __EEPROMWRB
	POP  R30
	SBIW R26,1
	RET

__EEPROMWRB:
	SBIC EECR,EEWE
	RJMP __EEPROMWRB
	IN   R25,SREG
	CLI
	OUT  EEARL,R26
	OUT  EEARH,R27
	SBI  EECR,EERE
	IN   R24,EEDR
	CP   R30,R24
	BREQ __EEPROMWRB0
	OUT  EEDR,R30
	SBI  EECR,EEMWE
	SBI  EECR,EEWE
__EEPROMWRB0:
	OUT  SREG,R25
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

__CPW02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
	RET

__CPD12:
	CP   R30,R26
	CPC  R31,R27
	CPC  R22,R24
	CPC  R23,R25
	RET

__CPD21:
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	CPC  R25,R23
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

;END OF CODE MARKER
__END_OF_CODE:
