;CodeVisionAVR C Compiler V1.24.6 Professional
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega8
;Program type           : Application
;Clock frequency        : 8.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 256 byte(s)
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

	.INCLUDE "8a.vec"
	.INCLUDE "8a.inc"

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

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x160
;       1 /*****************************************************
;       2 Chip type           : ATmega8
;       3 Program type        : Application
;       4 Clock frequency     : 8.000000 MHz
;       5 Memory model        : Small
;       6 External SRAM size  : 0
;       7 Data Stack size     : 256
;       8 
;       9 
;      10 
;      11 PLEASE NOTE 
;      12 
;      13 
;      14 lcd on port d
;      15 
;      16 sensors on adc 0,1 and 3
;      17 outputs
;      18 steering portb 0and 1
;      19 wheel motor on portb 6 and 7
;      20 
;      21 FOR CIRCUIT AND FURTHER DETAILS MAIL TO 
;      22                         "anoopjangra@gmail.com"
;      23                         with subject line 
;      24                         "REQUEST FOR LINE FOLLOWER CIRCUIT"
;      25 
;      26 *****************************************************/
;      27 
;      28 #include <mega8.h>    
;      29 #include<delay.h>
;      30 #include<stdio.h>
;      31 #include<string.h>
;      32 
;      33 
;      34 #asm
;      35    .equ __lcd_port=0x12 ;PORTD
   .equ __lcd_port=0x12 ;PORTD
;      36 #endasm
;      37 #include <lcd.h>
;      38 
;      39 #define ADC_VREF_TYPE 0x20
;      40 
;      41 // Read the 8 most significant bits
;      42 unsigned char read_adc(unsigned char adc_input)
;      43 {

	.CSEG
_read_adc:
;      44 ADMUX=adc_input|ADC_VREF_TYPE;
	LD   R30,Y
	ORI  R30,0x20
	OUT  0x7,R30
;      45 // Start the AD conversion
;      46 ADCSRA|=0x40;
	SBI  0x6,6
;      47 // Wait for the AD conversion to complete
;      48 while ((ADCSRA & 0x10)==0);
_0x3:
	SBIS 0x6,4
	RJMP _0x3
;      49 ADCSRA|=0x10;
	SBI  0x6,4
;      50 return ADCH;
	IN   R30,0x5
	ADIW R28,1
	RET
;      51 }
;      52     
;      53 
;      54 
;      55 
;      56 void read_sensor();
;      57 void env_c();      
;      58 void comp_v();
;      59 void move_();       
;      60 
;      61 #define STL     0x01
;      62 #define STR     0x02
;      63 #define STC     0x00
;      64 #define MOVF    0x80 
;      65 #define MOVR    0x40
;      66 #define MOVS    0x00
;      67 
;      68 
;      69 
;      70 
;      71 
;      72 
;      73 
;      74 
;      75 // Declare your global variables here
;      76 unsigned int i,j,k,l,t1,t2,t;               //ijk sensot ip,  l for loop

	.DSEG
_t2:
	.BYTE 0x2
_t:
	.BYTE 0x2
;      77 unsigned int s[3],sw[3],sb[3];                  // standard limit to black and white
_s:
	.BYTE 0x6
_sw:
	.BYTE 0x6
_sb:
	.BYTE 0x6
;      78 int m,n;                                        // move variables
_m:
	.BYTE 0x2
_n:
	.BYTE 0x2
;      79 char x,y,z,a=0,temp[10],sens[4];                    // sens sensor string , disp display string,
_y:
	.BYTE 0x1
_z:
	.BYTE 0x1
_a:
	.BYTE 0x1
_temp:
	.BYTE 0xA
_sens:
	.BYTE 0x4
;      80 
;      81 
;      82 
;      83 
;      84 void main(void)
;      85 {

	.CSEG
_main:
;      86 // Declare your local variables here
;      87 
;      88 
;      89 PORTB=0x00;
	RCALL SUBOPT_0x0
;      90 DDRB=0xC3;        //0,1,6,7 are outputs
	LDI  R30,LOW(195)
	OUT  0x17,R30
;      91 
;      92 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;      93 DDRC=0x00;
	OUT  0x14,R30
;      94 
;      95 PORTD=0x00;
	OUT  0x12,R30
;      96 DDRD=0x00;
	OUT  0x11,R30
;      97 
;      98 TCCR0=0x00;
	OUT  0x33,R30
;      99 TCNT0=0x00;
	OUT  0x32,R30
;     100 
;     101 TCCR1A=0x00;
	OUT  0x2F,R30
;     102 TCCR1B=0x00;
	OUT  0x2E,R30
;     103 TCNT1H=0x00;
	OUT  0x2D,R30
;     104 TCNT1L=0x00;
	OUT  0x2C,R30
;     105 ICR1H=0x00;
	OUT  0x27,R30
;     106 ICR1L=0x00;
	OUT  0x26,R30
;     107 OCR1AH=0x00;
	OUT  0x2B,R30
;     108 OCR1AL=0x00;
	OUT  0x2A,R30
;     109 OCR1BH=0x00;
	OUT  0x29,R30
;     110 OCR1BL=0x00;
	OUT  0x28,R30
;     111 
;     112 ASSR=0x00;
	OUT  0x22,R30
;     113 TCCR2=0x00;
	OUT  0x25,R30
;     114 TCNT2=0x00;
	OUT  0x24,R30
;     115 OCR2=0x00;
	OUT  0x23,R30
;     116 
;     117 MCUCR=0x00;
	OUT  0x35,R30
;     118 
;     119 // Timer(s)/Counter(s) Interrupt(s) initialization
;     120 TIMSK=0x00;
	OUT  0x39,R30
;     121 
;     122 // Analog Comparator initialization
;     123 // Analog Comparator: Off
;     124 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     125 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     126 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     127 
;     128 // ADC initialization
;     129 // ADC Clock frequency: 125.000 kHz
;     130 // ADC Voltage Reference: AREF pin
;     131 // Only the 8 most significant bits of
;     132 // the AD conversion result are used
;     133 ADMUX=ADC_VREF_TYPE;
	LDI  R30,LOW(32)
	OUT  0x7,R30
;     134 ADCSRA=0x86;
	LDI  R30,LOW(134)
	OUT  0x6,R30
;     135 
;     136 
;     137 
;     138 // LCD module initialization
;     139 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
;     140 
;     141 lcd_clear();
	RCALL _lcd_clear
;     142 lcd_putsf("Starting Up");
	__POINTW1FN _0,0
	RCALL SUBOPT_0x1
;     143 for(l=0;l<10;l++)
	CLR  R10
	CLR  R11
_0x7:
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CP   R10,R30
	CPC  R11,R31
	BRSH _0x8
;     144 {
;     145         lcd_putsf(".");
	__POINTW1FN _0,12
	RCALL SUBOPT_0x1
;     146         delay_ms(100);
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	RCALL SUBOPT_0x2
;     147 } 
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 10,11,30,31
	RJMP _0x7
_0x8:
;     148 
;     149 delay_ms(1000);
	RCALL SUBOPT_0x3
;     150 lcd_clear();
	RCALL _lcd_clear
;     151 
;     152 
;     153 //inital calib
;     154 env_c();
	RCALL _env_c
;     155 
;     156 
;     157 
;     158 
;     159 while (1)
_0x9:
;     160       {
;     161       read_sensor();
	RCALL _read_sensor
;     162       comp_v(); 
	RCALL _comp_v
;     163       lcd_clear();
	RCALL _lcd_clear
;     164       move_();
	RCALL _move_
;     165       //delay_ms(100);
;     166        a++;    
	LDS  R30,_a
	SUBI R30,-LOW(1)
	STS  _a,R30
;     167       };
	RJMP _0x9
;     168 }
_0xC:
	RJMP _0xC
;     169 
;     170 
;     171 void read_sensor(void)          // read sensor
;     172 {
_read_sensor:
;     173         i=read_adc(0); 
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x5
	__PUTW1R 4,5
;     174         j=read_adc(1);
	RCALL SUBOPT_0x6
	RCALL SUBOPT_0x5
	__PUTW1R 6,7
;     175         k=read_adc(2);
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0x5
	__PUTW1R 8,9
;     176         
;     177         
;     178       
;     179 }
	RET
;     180 
;     181 
;     182 // environment calib
;     183 
;     184 void env_c(void)
;     185 {       
_env_c:
;     186         //black detection
;     187         
;     188         
;     189         
;     190         for(l=51;l>47;l--)
	RCALL SUBOPT_0x8
_0xE:
	RCALL SUBOPT_0x9
	BRSH _0xF
;     191         {       lcd_clear();
	RCALL _lcd_clear
;     192                 lcd_putsf("Put BOT on Black");
	__POINTW1FN _0,14
	RCALL SUBOPT_0x1
;     193                 lcd_putchar(l);
	RCALL SUBOPT_0xA
;     194                 delay_ms(1000);
;     195         }
	RCALL SUBOPT_0xB
	RJMP _0xE
_0xF:
;     196         
;     197         
;     198         lcd_clear();
	RCALL _lcd_clear
;     199         lcd_putsf("\nDetecting..");
	__POINTW1FN _0,31
	RCALL SUBOPT_0x1
;     200         delay_ms(1000);
	RCALL SUBOPT_0x3
;     201        
;     202                 read_sensor();
	RCALL _read_sensor
;     203                 sb[0]=i;
	__PUTWMRN _sb,0,4,5
;     204                 sb[1]=j;
	__PUTWMRN _sb,2,6,7
;     205                 sb[2]=k; 
	__PUTWMRN _sb,4,8,9
;     206                 t1=(i+j+k)/3;
	RCALL SUBOPT_0xC
	__PUTW1R 12,13
;     207                 sprintf(temp,":%d:%d:%d:",i,j,k);
	RCALL SUBOPT_0xD
	__POINTW1FN _0,44
	RCALL SUBOPT_0xE
	LDI  R24,12
	RCALL SUBOPT_0xF
;     208                 lcd_clear();
;     209                 lcd_putsf("Black=\n");
	__POINTW1FN _0,55
	RCALL SUBOPT_0x1
;     210                 lcd_puts(temp);
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0x10
;     211                 delay_ms(1000);
;     212           
;     213         
;     214         
;     215         //white detection
;     216         
;     217         
;     218         
;     219         for(l=51;l>47;l--)
	RCALL SUBOPT_0x8
_0x11:
	RCALL SUBOPT_0x9
	BRSH _0x12
;     220         {
;     221                 lcd_clear();
	RCALL _lcd_clear
;     222                 lcd_putsf("Put BOT on White");
	__POINTW1FN _0,63
	RCALL SUBOPT_0x1
;     223                 lcd_putchar(l);
	RCALL SUBOPT_0xA
;     224                 delay_ms(1000);
;     225         
;     226         }
	RCALL SUBOPT_0xB
	RJMP _0x11
_0x12:
;     227         
;     228         
;     229         lcd_clear();
	RCALL _lcd_clear
;     230         lcd_putsf("\nDetecting..");
	__POINTW1FN _0,31
	RCALL SUBOPT_0x1
;     231         delay_ms(1000); 
	RCALL SUBOPT_0x3
;     232        
;     233                 read_sensor(); 
	RCALL _read_sensor
;     234                 sb[0]=i;
	__PUTWMRN _sb,0,4,5
;     235                 sb[1]=j;
	__PUTWMRN _sb,2,6,7
;     236                 sb[2]=k; 
	__PUTWMRN _sb,4,8,9
;     237                 t2=(i+j+k)/3;
	RCALL SUBOPT_0xC
	STS  _t2,R30
	STS  _t2+1,R31
;     238                 sprintf(temp,":%d:%d:%d:",i,j,k);
	RCALL SUBOPT_0xD
	__POINTW1FN _0,44
	RCALL SUBOPT_0xE
	LDI  R24,12
	RCALL SUBOPT_0xF
;     239                 lcd_clear();
;     240                 lcd_putsf("White=\n");
	__POINTW1FN _0,80
	RCALL SUBOPT_0x1
;     241                 lcd_puts(temp);
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0x10
;     242                 delay_ms(1000);   
;     243        
;     244         
;     245         // if there is a little or no diff b/w black and white
;     246                               
;     247         if(t2<=t1)
	LDS  R26,_t2
	LDS  R27,_t2+1
	CP   R12,R26
	CPC  R13,R27
	BRLO _0x13
;     248         {
;     249         lcd_clear();
	RCALL _lcd_clear
;     250         lcd_putsf("Error!\nPlease Recalbrt");
	__POINTW1FN _0,88
	RJMP _0xA1
;     251         delay_ms(2000);
;     252         env_c();
;     253         }
;     254         
;     255         else
_0x13:
;     256         {
;     257                 t=t2-t1;
	LDS  R30,_t2
	LDS  R31,_t2+1
	SUB  R30,R12
	SBC  R31,R13
	STS  _t,R30
	STS  _t+1,R31
;     258                 if(t<5)
	LDS  R26,_t
	LDS  R27,_t+1
	CPI  R26,LOW(0x5)
	LDI  R30,HIGH(0x5)
	CPC  R27,R30
	BRSH _0x15
;     259                 {
;     260                         lcd_clear();
	RCALL _lcd_clear
;     261                         lcd_putsf("Error!\nPlease Recalbrt");
	__POINTW1FN _0,88
_0xA1:
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     262                         delay_ms(2000);
	RCALL SUBOPT_0x11
;     263                         env_c();
	RCALL _env_c
;     264                 }
;     265                 
;     266         } 
_0x15:
;     267          
;     268         lcd_clear();
	RCALL _lcd_clear
;     269         lcd_putsf("Detection Done!");
	__POINTW1FN _0,111
	RCALL SUBOPT_0x1
;     270         delay_ms(2000);
	RCALL SUBOPT_0x11
;     271         lcd_clear();
	RCALL _lcd_clear
;     272                 //t=(t1+t2)
;     273                 //std=t;
;     274         for(l=0;l<3;l++) s[l]=(sw[l]+sb[l])/2;        
	CLR  R10
	CLR  R11
_0x17:
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R10,R30
	CPC  R11,R31
	BRSH _0x18
	__GETW1R 10,11
	LDI  R26,LOW(_s)
	LDI  R27,HIGH(_s)
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	__GETW1R 10,11
	LDI  R26,LOW(_sw)
	LDI  R27,HIGH(_sw)
	RCALL SUBOPT_0x12
	PUSH R31
	PUSH R30
	__GETW1R 10,11
	LDI  R26,LOW(_sb)
	LDI  R27,HIGH(_sb)
	RCALL SUBOPT_0x12
	POP  R26
	POP  R27
	ADD  R26,R30
	ADC  R27,R31
	MOVW R30,R26
	LSR  R31
	ROR  R30
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;     275 }             
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 10,11,30,31
	RJMP _0x17
_0x18:
	RET
;     276 
;     277 
;     278 //assign black and white
;     279 
;     280 void comp_v()
;     281 {
_comp_v:
;     282         if(i<=s[0])     x='B';
	LDS  R30,_s
	LDS  R31,_s+1
	CP   R30,R4
	CPC  R31,R5
	BRLO _0x19
	LDI  R30,LOW(66)
	RJMP _0xA2
;     283         else            x='W';
_0x19:
	LDI  R30,LOW(87)
_0xA2:
	MOV  R14,R30
;     284         if(j<=s[1])     y='B';
	__GETW1MN _s,2
	CP   R30,R6
	CPC  R31,R7
	BRLO _0x1B
	LDI  R30,LOW(66)
	RJMP _0xA3
;     285         else            y='W';
_0x1B:
	LDI  R30,LOW(87)
_0xA3:
	STS  _y,R30
;     286         if(k<=s[2])     z='B';
	__GETW1MN _s,4
	CP   R30,R8
	CPC  R31,R9
	BRLO _0x1D
	LDI  R30,LOW(66)
	RJMP _0xA4
;     287         else            z='W';
_0x1D:
	LDI  R30,LOW(87)
_0xA4:
	STS  _z,R30
;     288         
;     289         sprintf(sens,"%c%c%c",x,y,z);
	RCALL SUBOPT_0x13
	__POINTW1FN _0,127
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R14
	RCALL SUBOPT_0x14
	LDS  R30,_y
	RCALL SUBOPT_0x14
	LDS  R30,_z
	RCALL SUBOPT_0x14
	LDI  R24,12
	RCALL _sprintf
	ADIW R28,16
;     290  
;     291 
;     292 }   
	RET
;     293 
;     294 //writing to motors
;     295 
;     296 void move_()
;     297 {
_move_:
;     298       if(((x=='W')&(y=='B')&(z=='W'))||((x=='B')&(y=='W')&(z=='B'))) m=1;
	RCALL SUBOPT_0x15
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x17
	POP  R26
	AND  R30,R26
	BRNE _0x20
	RCALL SUBOPT_0x18
	PUSH R30
	RCALL SUBOPT_0x19
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x1A
	POP  R26
	AND  R30,R26
	BREQ _0x1F
_0x20:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _m,R30
	STS  _m+1,R31
;     299       else if(((x=='W')&(y=='B')&(z=='B'))||((x=='W')&(y=='W')&(z=='B'))) m=2;
	RJMP _0x22
_0x1F:
	RCALL SUBOPT_0x15
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x1A
	POP  R26
	AND  R30,R26
	BRNE _0x24
	RCALL SUBOPT_0x15
	PUSH R30
	RCALL SUBOPT_0x19
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x1A
	POP  R26
	AND  R30,R26
	BREQ _0x23
_0x24:
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	STS  _m,R30
	STS  _m+1,R31
;     300       else if(((x=='B')&(y=='W')&(z=='W'))||((x=='B')&(y=='B')&(z=='W'))) m=3;
	RJMP _0x26
_0x23:
	RCALL SUBOPT_0x18
	PUSH R30
	RCALL SUBOPT_0x19
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x17
	POP  R26
	AND  R30,R26
	BRNE _0x28
	RCALL SUBOPT_0x18
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x17
	POP  R26
	AND  R30,R26
	BREQ _0x27
_0x28:
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	STS  _m,R30
	STS  _m+1,R31
;     301       else if(((x=='W')&(y=='W')&(z=='W'))||((x=='B')&(y=='B')&(z=='B'))) m=0;
	RJMP _0x2A
_0x27:
	RCALL SUBOPT_0x15
	PUSH R30
	RCALL SUBOPT_0x19
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x17
	POP  R26
	AND  R30,R26
	BRNE _0x2C
	RCALL SUBOPT_0x18
	PUSH R30
	RCALL SUBOPT_0x16
	POP  R26
	AND  R30,R26
	PUSH R30
	RCALL SUBOPT_0x1A
	POP  R26
	AND  R30,R26
	BREQ _0x2B
_0x2C:
	LDI  R30,0
	STS  _m,R30
	STS  _m+1,R30
;     302       
;     303       if(m!=0) n=m;
_0x2B:
_0x2A:
_0x26:
_0x22:
	RCALL SUBOPT_0x1B
	BREQ _0x2E
	LDS  R30,_m
	LDS  R31,_m+1
	STS  _n,R30
	STS  _n+1,R31
;     304 
;     305       lcd_putsf("SENSORS  ACTION\n");
_0x2E:
	__POINTW1FN _0,134
	RCALL SUBOPT_0x1
;     306       lcd_puts(sens);      
	RCALL SUBOPT_0x13
	RCALL _lcd_puts
;     307       switch(m?m:n)
	RCALL SUBOPT_0x1B
	BREQ _0x32
	LDS  R30,_m
	LDS  R31,_m+1
	RJMP _0x33
_0x32:
	LDS  R30,_n
	LDS  R31,_n+1
_0x33:
;     308       {
;     309         case 1: PORTB=(STC|((a%4==0)?MOVF:MOVS));lcd_putsf("      forward"); break;
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BRNE _0x35
	RCALL SUBOPT_0x1C
	BRNE _0x36
	LDI  R30,LOW(128)
	RJMP _0x37
_0x36:
	LDI  R30,LOW(0)
_0x37:
	OUT  0x18,R30
	__POINTW1FN _0,151
	RCALL SUBOPT_0x1
	RJMP _0x31
;     310         
;     311         case 2: PORTB=(STR|((a%4==0)?MOVF:MOVS));lcd_putsf("      right"); break;
_0x35:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BRNE _0x39
	RCALL SUBOPT_0x1C
	BRNE _0x3A
	LDI  R30,LOW(128)
	RJMP _0x3B
_0x3A:
	LDI  R30,LOW(0)
_0x3B:
	ORI  R30,2
	OUT  0x18,R30
	__POINTW1FN _0,165
	RCALL SUBOPT_0x1
	RJMP _0x31
;     312         
;     313         case 3: PORTB=(STL|((a%4==0)?MOVF:MOVS));lcd_putsf("      left"); break;
_0x39:
	CPI  R30,LOW(0x3)
	LDI  R26,HIGH(0x3)
	CPC  R31,R26
	BRNE _0x41
	RCALL SUBOPT_0x1C
	BRNE _0x3E
	LDI  R30,LOW(128)
	RJMP _0x3F
_0x3E:
	LDI  R30,LOW(0)
_0x3F:
	ORI  R30,1
	OUT  0x18,R30
	__POINTW1FN _0,177
	RCALL SUBOPT_0x1
	RJMP _0x31
;     314         
;     315         default: PORTB=0x00;
_0x41:
	RCALL SUBOPT_0x0
;     316       }
_0x31:
;     317       
;     318       
;     319 }
	RET

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
	RCALL SUBOPT_0x1D
	SBIW R30,0
	BREQ _0x42
	RCALL SUBOPT_0x1D
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x43
_0x42:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x43:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0x44:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	RCALL SUBOPT_0x1E
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x46
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x4A
	CPI  R19,37
	BRNE _0x4B
	LDI  R16,LOW(1)
	RJMP _0x4C
_0x4B:
	RCALL SUBOPT_0x1F
_0x4C:
	RJMP _0x49
_0x4A:
	CPI  R30,LOW(0x1)
	BRNE _0x4D
	CPI  R19,37
	BRNE _0x4E
	RCALL SUBOPT_0x1F
	LDI  R16,LOW(0)
	RJMP _0x49
_0x4E:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x4F
	LDI  R17,LOW(1)
	RJMP _0x49
_0x4F:
	CPI  R19,43
	BRNE _0x50
	LDI  R21,LOW(43)
	RJMP _0x49
_0x50:
	CPI  R19,32
	BRNE _0x51
	LDI  R21,LOW(32)
	RJMP _0x49
_0x51:
	RJMP _0x52
_0x4D:
	CPI  R30,LOW(0x2)
	BRNE _0x53
_0x52:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x54
	ORI  R17,LOW(128)
	RJMP _0x49
_0x54:
	RJMP _0x55
_0x53:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x49
_0x55:
	CPI  R19,48
	BRLO _0x58
	CPI  R19,58
	BRLO _0x59
_0x58:
	RJMP _0x57
_0x59:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x49
_0x57:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x5D
	RCALL SUBOPT_0x20
	LD   R30,X
	RCALL SUBOPT_0x21
	RJMP _0x5E
_0x5D:
	CPI  R30,LOW(0x73)
	BRNE _0x60
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x22
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x61
_0x60:
	CPI  R30,LOW(0x70)
	BRNE _0x63
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x22
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x61:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x64
_0x63:
	CPI  R30,LOW(0x64)
	BREQ _0x67
	CPI  R30,LOW(0x69)
	BRNE _0x68
_0x67:
	ORI  R17,LOW(4)
	RJMP _0x69
_0x68:
	CPI  R30,LOW(0x75)
	BRNE _0x6A
_0x69:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x6B
_0x6A:
	CPI  R30,LOW(0x58)
	BRNE _0x6D
	ORI  R17,LOW(8)
	RJMP _0x6E
_0x6D:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x9C
_0x6E:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x6B:
	SBRS R17,2
	RJMP _0x70
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x23
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x71
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x71:
	CPI  R21,0
	BREQ _0x72
	SUBI R16,-LOW(1)
	RJMP _0x73
_0x72:
	ANDI R17,LOW(251)
_0x73:
	RJMP _0x74
_0x70:
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x23
_0x74:
_0x64:
	SBRC R17,0
	RJMP _0x75
_0x76:
	CP   R16,R20
	BRSH _0x78
	SBRS R17,7
	RJMP _0x79
	SBRS R17,2
	RJMP _0x7A
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x7B
_0x7A:
	LDI  R19,LOW(48)
_0x7B:
	RJMP _0x7C
_0x79:
	LDI  R19,LOW(32)
_0x7C:
	RCALL SUBOPT_0x1F
	SUBI R20,LOW(1)
	RJMP _0x76
_0x78:
_0x75:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x7D
_0x7E:
	CPI  R18,0
	BREQ _0x80
	SBRS R17,3
	RJMP _0x81
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	RCALL SUBOPT_0x1E
	RJMP _0xA5
_0x81:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xA5:
	ST   -Y,R30
	RCALL SUBOPT_0x24
	CPI  R20,0
	BREQ _0x83
	SUBI R20,LOW(1)
_0x83:
	SUBI R18,LOW(1)
	RJMP _0x7E
_0x80:
	RJMP _0x84
_0x7D:
_0x86:
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
	BRSH _0x88
	SBRS R17,3
	RJMP _0x89
	SUBI R19,-LOW(7)
	RJMP _0x8A
_0x89:
	SUBI R19,-LOW(39)
_0x8A:
_0x88:
	SBRC R17,4
	RJMP _0x8C
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x8E
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x8D
_0x8E:
	ORI  R17,LOW(16)
	RJMP _0x90
_0x8D:
	CP   R20,R18
	BRLO _0x92
	SBRS R17,0
	RJMP _0x93
_0x92:
	RJMP _0x91
_0x93:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x94
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x90:
	SBRS R17,2
	RJMP _0x95
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0x24
	CPI  R20,0
	BREQ _0x96
	SUBI R20,LOW(1)
_0x96:
_0x95:
_0x94:
_0x8C:
	RCALL SUBOPT_0x1F
	CPI  R20,0
	BREQ _0x97
	SUBI R20,LOW(1)
_0x97:
_0x91:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x87
	RJMP _0x86
_0x87:
_0x84:
	SBRS R17,0
	RJMP _0x98
_0x99:
	CPI  R20,0
	BREQ _0x9B
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x21
	RJMP _0x99
_0x9B:
_0x98:
_0x9C:
_0x5E:
	LDI  R16,LOW(0)
_0x49:
	RJMP _0x44
_0x46:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	RCALL __SAVELOCR2
	MOVW R26,R28
	RCALL __ADDW2R15
	__PUTW2R 16,17
	MOVW R26,R28
	ADIW R26,6
	RCALL SUBOPT_0x25
	STD  Y+2,R30
	STD  Y+2+1,R31
	MOVW R26,R28
	ADIW R26,4
	RCALL SUBOPT_0x25
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G2
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	RCALL __LOADLOCR2
	ADIW R28,4
	POP  R15
	RET
    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G4:
	.BYTE 0x4
__lcd_x:
	.BYTE 0x1
__lcd_y:
	.BYTE 0x1
__lcd_maxx:
	.BYTE 0x1

	.CSEG
__lcd_delay_G4:
    ldi   r31,15
__lcd_delay0:
    dec   r31
    brne  __lcd_delay0
	RET
__lcd_ready:
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
    cbi   __lcd_port,__lcd_rs     ;RS=0
__lcd_busy:
	RCALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G4:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G4
    ld    r26,y
    swap  r26
	RCALL __lcd_write_nibble_G4
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G4:
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
    andi  r30,0xf0
	RET
_lcd_read_byte0_G4:
	RCALL __lcd_delay_G4
	RCALL __lcd_read_nibble_G4
    mov   r26,r30
	RCALL __lcd_read_nibble_G4
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	RCALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G4)
	SBCI R31,HIGH(-__base_y_G4)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	RCALL SUBOPT_0x26
	LDD  R30,Y+1
	STS  __lcd_x,R30
	LD   R30,Y
	STS  __lcd_y,R30
	ADIW R28,2
	RET
_lcd_clear:
	RCALL __lcd_ready
	RCALL SUBOPT_0x7
	RCALL __lcd_write_data
	RCALL __lcd_ready
	LDI  R30,LOW(12)
	RCALL SUBOPT_0x26
	RCALL __lcd_ready
	RCALL SUBOPT_0x6
	RCALL __lcd_write_data
	LDI  R30,LOW(0)
	STS  __lcd_y,R30
	STS  __lcd_x,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	LDS  R30,__lcd_x
	SUBI R30,-LOW(1)
	STS  __lcd_x,R30
	LDS  R30,__lcd_maxx
	LDS  R26,__lcd_x
	CP   R30,R26
	BRSH _0x9E
	__lcd_putchar1:
	LDS  R30,__lcd_y
	SUBI R30,-LOW(1)
	STS  __lcd_y,R30
	RCALL SUBOPT_0x4
	LDS  R30,__lcd_y
	ST   -Y,R30
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0x9E:
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
__long_delay_G4:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G4:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G4
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LD   R30,Y
	STS  __lcd_maxx,R30
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G4,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G4,3
	RCALL SUBOPT_0x27
	RCALL SUBOPT_0x27
	RCALL SUBOPT_0x27
	RCALL __long_delay_G4
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL __lcd_init_write_G4
	RCALL __long_delay_G4
	LDI  R30,LOW(40)
	RCALL SUBOPT_0x26
	RCALL __long_delay_G4
	LDI  R30,LOW(4)
	RCALL SUBOPT_0x26
	RCALL __long_delay_G4
	LDI  R30,LOW(133)
	RCALL SUBOPT_0x26
	RCALL __long_delay_G4
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RCALL _lcd_read_byte0_G4
	CPI  R30,LOW(0x5)
	BREQ _0x9F
	LDI  R30,LOW(0)
	RJMP _0xA0
_0x9F:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	RCALL SUBOPT_0x26
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0xA0:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	LDI  R30,LOW(0)
	OUT  0x18,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 13 TIMES
SUBOPT_0x1:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _lcd_putsf

;OPTIMIZER ADDED SUBROUTINE, CALLED 10 TIMES
SUBOPT_0x2:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x3:
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RJMP SUBOPT_0x2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	LDI  R30,LOW(0)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x5:
	RCALL _read_adc
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	LDI  R30,LOW(2)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	LDI  R30,LOW(51)
	LDI  R31,HIGH(51)
	__PUTW1R 10,11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x9:
	LDI  R30,LOW(47)
	LDI  R31,HIGH(47)
	CP   R30,R10
	CPC  R31,R11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	ST   -Y,R10
	RCALL _lcd_putchar
	RJMP SUBOPT_0x3

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	__GETW1R 10,11
	SBIW R30,1
	__PUTW1R 10,11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	__GETW1R 6,7
	ADD  R30,R4
	ADC  R31,R5
	ADD  R30,R8
	ADC  R31,R9
	MOVW R26,R30
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	RCALL __DIVW21U
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xD:
	LDI  R30,LOW(_temp)
	LDI  R31,HIGH(_temp)
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 4,5
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	__GETW1R 6,7
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	__GETW1R 8,9
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	RCALL _sprintf
	ADIW R28,16
	RJMP _lcd_clear

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	RCALL _lcd_puts
	RJMP SUBOPT_0x3

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	LDI  R30,LOW(2000)
	LDI  R31,HIGH(2000)
	RJMP SUBOPT_0x2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	LDI  R30,LOW(_sens)
	LDI  R31,HIGH(_sens)
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x14:
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x15:
	MOV  R26,R14
	LDI  R30,LOW(87)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x16:
	LDS  R26,_y
	LDI  R30,LOW(66)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x17:
	LDS  R26,_z
	LDI  R30,LOW(87)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x18:
	MOV  R26,R14
	LDI  R30,LOW(66)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x19:
	LDS  R26,_y
	LDI  R30,LOW(87)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1A:
	LDS  R26,_z
	LDI  R30,LOW(66)
	RCALL __EQB12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1B:
	LDS  R30,_m
	LDS  R31,_m+1
	SBIW R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1C:
	LDS  R30,_a
	ANDI R30,LOW(0x3)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1D:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1E:
	SBIW R30,1
	LPM  R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1F:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x20:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x21:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x22:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x23:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x24:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x25:
	RCALL __ADDW2R15
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x26:
	ST   -Y,R30
	RJMP __lcd_write_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x27:
	RCALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	RJMP __lcd_init_write_G4

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
	__DELAY_USW 0x7D0
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

__ANEGW1:
	COM  R30
	COM  R31
	ADIW R30,1
	RET

__EQB12:
	CP   R30,R26
	LDI  R30,1
	BREQ __EQB12T
	CLR  R30
__EQB12T:
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
