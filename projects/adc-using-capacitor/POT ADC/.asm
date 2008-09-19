;CodeVisionAVR C Compiler V1.24.4 Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type           : ATmega8L
;Program type        : Application
;Clock frequency     : 8.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : float, width, precision
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
	LDS  R@2,@0+@1
	.ENDM

	.MACRO __GETWRMN
	LDS  R@2,@0+@1
	LDS  R@3,@0+@1+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
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

	.INCLUDE "POTADC.vec"
	.INCLUDE "POTADC.inc"

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
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.4 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 e-mail:office@hpinfotech.com
;       8 
;       9 Project : 
;      10 Version : 
;      11 Date    : 9/16/2008
;      12 Author  : Osama                           
;      13 Company : OSAMA                           
;      14 Comments: 
;      15 
;      16 
;      17 Chip type           : ATmega8L
;      18 Program type        : Application
;      19 Clock frequency     : 8.000000 MHz
;      20 Memory model        : Small
;      21 External SRAM size  : 0
;      22 Data Stack size     : 256
;      23 *****************************************************/
;      24 
;      25 #include <mega8.h>
;      26 #include <delay.h>
;      27 #include <stdio.h>
;      28 
;      29 int r;
;      30 
;      31 // Timer 1 input capture interrupt service routine
;      32 interrupt [TIM1_CAPT] void timer1_capt_isr(void)
;      33 {                

	.CSEG
_timer1_capt_isr:
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
;      34           TCCR1B &= 0xF8; //stop Timer/Counter1 
	RCALL SUBOPT_0x0
;      35           TIMSK &= 0xDF;  //disable Input Capture Interrupt
;      36           DDRB |= 0x01;   //set PORTB.0 as output
	SBI  0x17,0
;      37           PORTB &= 0xFE;  //clear PORTB.0
	CBI  0x18,0
;      38           delay_ms(500);  //discharge the capacitor
	RCALL SUBOPT_0x1
;      39           DDRB &= 0xFE;   //set PORTB.0 as input
	CBI  0x17,0
;      40           PORTB &= 0xFE;  //Tri-state
	CBI  0x18,0
;      41           TIMSK |= 0x20;  //enable Input Capture Interrupt       
	RCALL SUBOPT_0x2
;      42           r = TCNT1/1000;
	IN   R30,0x2C
	IN   R31,0x2C+1
	MOVW R26,R30
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RCALL __DIVW21U
	__PUTW1R 4,5
;      43           printf("\b\b\b\b\b\b\b\b\bLevel: %d",r);
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 4,5
	RCALL __CWD1
	RCALL __PUTPARD1
	LDI  R24,4
	RCALL _printf
	ADIW R28,6
;      44           TCNT1H = 0x00;  //clear Timer1 MSBs       
	RCALL SUBOPT_0x3
;      45           TCNT1L = 0x00;  //clear Timer1 LSBs                 
;      46           TCCR1B |= 0x03; //set prescaler to clk/64 (start counter)
	RCALL SUBOPT_0x4
;      47 }
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
;      48 
;      49 // Declare your global variables here
;      50 
;      51 void main(void)
;      52 {
_main:
;      53 // Declare your local variables here
;      54 
;      55 // Input/Output Ports initialization
;      56 // Port B initialization
;      57 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      58 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      59 PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;      60 DDRB=0x00;
	OUT  0x17,R30
;      61 
;      62 // Port C initialization
;      63 // Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      64 // State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      65 PORTC=0x00;
	OUT  0x15,R30
;      66 DDRC=0x00;
	OUT  0x14,R30
;      67 
;      68 // Port D initialization
;      69 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      70 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      71 PORTD=0x00;
	OUT  0x12,R30
;      72 DDRD=0x00;
	OUT  0x11,R30
;      73 
;      74 // Timer/Counter 0 initialization
;      75 // Clock source: System Clock
;      76 // Clock value: Timer 0 Stopped
;      77 TCCR0=0x00;
	OUT  0x33,R30
;      78 TCNT0=0x00;
	OUT  0x32,R30
;      79 
;      80 // Timer/Counter 1 initialization
;      81 // Clock source: System Clock
;      82 // Clock value: 125.000 kHz
;      83 // Mode: CTC top=OCR1A
;      84 // OC1A output: Discon.
;      85 // OC1B output: Discon.
;      86 // Noise Canceler: Off
;      87 // Input Capture on Rising Edge
;      88 TCCR1A=0x00;
	OUT  0x2F,R30
;      89 TCCR1B=0x4B;
	LDI  R30,LOW(75)
	OUT  0x2E,R30
;      90 TCNT1H=0x00;
	RCALL SUBOPT_0x3
;      91 TCNT1L=0x00;
;      92 ICR1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x27,R30
;      93 ICR1L=0x00;
	OUT  0x26,R30
;      94 OCR1AH=0xFF;
	LDI  R30,LOW(255)
	OUT  0x2B,R30
;      95 OCR1AL=0xFF;
	OUT  0x2A,R30
;      96 OCR1BH=0x00;
	LDI  R30,LOW(0)
	OUT  0x29,R30
;      97 OCR1BL=0x00;
	OUT  0x28,R30
;      98 
;      99 // Timer/Counter 2 initialization
;     100 // Clock source: System Clock
;     101 // Clock value: Timer 2 Stopped
;     102 // Mode: Normal top=FFh
;     103 // OC2 output: Disconnected
;     104 ASSR=0x00;
	OUT  0x22,R30
;     105 TCCR2=0x00;
	OUT  0x25,R30
;     106 TCNT2=0x00;
	OUT  0x24,R30
;     107 OCR2=0x00;
	OUT  0x23,R30
;     108 
;     109 // External Interrupt(s) initialization
;     110 // INT0: Off
;     111 // INT1: Off
;     112 MCUCR=0x00;
	OUT  0x35,R30
;     113 
;     114 // Timer(s)/Counter(s) Interrupt(s) initialization
;     115 TIMSK=0x20;
	LDI  R30,LOW(32)
	OUT  0x39,R30
;     116     
;     117 // USART initialization
;     118 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     119 // USART Receiver: Off
;     120 // USART Transmitter: On
;     121 // USART Mode: Asynchronous
;     122 // USART Baud rate: 9600
;     123 UCSRA=0x00;
	LDI  R30,LOW(0)
	OUT  0xB,R30
;     124 UCSRB=0x08;
	LDI  R30,LOW(8)
	OUT  0xA,R30
;     125 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     126 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     127 UBRRL=0x33;
	LDI  R30,LOW(51)
	OUT  0x9,R30
;     128 
;     129 // Analog Comparator initialization
;     130 // Analog Comparator: Off
;     131 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     132 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     133 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     134 
;     135 // Global enable interrupts
;     136 #asm("sei")          
	sei
;     137 
;     138           TCCR1B &= 0xF8; //stop Timer/Counter1 
	RCALL SUBOPT_0x0
;     139           TIMSK &= 0xDF;  //disable Input Capture Interrupt
;     140           DDRB |= 0x01;   //set PORTB.0 as output
	SBI  0x17,0
;     141           PORTB &= 0xFE;  //clear PORTB.0
	CBI  0x18,0
;     142           delay_ms(500); //discharge the capacitor
	RCALL SUBOPT_0x1
;     143           DDRB &= 0xFE;   //set PORTB.0 as input 
	CBI  0x17,0
;     144           PORTB &= 0xFE;  //Tri-state          
	CBI  0x18,0
;     145           TIMSK |= 0x20;  //enable Input Capture Interrupt 
	RCALL SUBOPT_0x2
;     146           TCCR1B |= 0x03; //set prescaler to clk/64 (start counter)  
	RCALL SUBOPT_0x4
;     147 }
_0x3:
	RJMP _0x3

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
	RCALL SUBOPT_0x5
	SBIW R30,0
	BREQ _0x4
	RCALL SUBOPT_0x5
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x5
_0x4:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x5:
	ADIW R28,3
	RET
__ftoa_G2:
	SBIW R28,4
	RCALL __SAVELOCR2
	LDD  R26,Y+8
	RCALL SUBOPT_0x6
	BRSH _0x6
	LDI  R30,LOW(5)
	STD  Y+8,R30
_0x6:
	LDI  R26,LOW(__fround_G2*2)
	LDI  R27,HIGH(__fround_G2*2)
	LDD  R30,Y+8
	LDI  R31,0
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
	LDI  R16,LOW(0)
	__GETD1N 0x3F800000
	__PUTD1S 2
_0x7:
	RCALL SUBOPT_0x7
	RCALL __CMPF12
	BRLO _0x9
	RCALL SUBOPT_0x8
	RCALL __MULF12
	__PUTD1S 2
	SUBI R16,-LOW(1)
	RJMP _0x7
_0x9:
	CPI  R16,0
	BRNE _0xA
	RCALL SUBOPT_0x9
	LDI  R30,LOW(48)
	ST   X,R30
	RJMP _0xB
_0xA:
_0xC:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0xE
	RCALL SUBOPT_0x8
	RCALL SUBOPT_0xA
	__PUTD1S 2
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0xC
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0xD
	POP  R26
	POP  R27
	RCALL SUBOPT_0xE
	__GETD2S 2
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __CDF1
	RCALL __MULF12
	__GETD2S 9
	RCALL SUBOPT_0xF
	RJMP _0xC
_0xE:
_0xB:
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0xF
	RCALL SUBOPT_0x10
	RJMP _0xAD
_0xF:
	RCALL SUBOPT_0x9
	RCALL SUBOPT_0x11
_0x10:
	LDD  R30,Y+8
	SUBI R30,LOW(1)
	STD  Y+8,R30
	SUBI R30,-LOW(1)
	BREQ _0x12
	__GETD2S 9
	RCALL SUBOPT_0x12
	__PUTD1S 9
	RCALL __CFD1
	MOV  R17,R30
	RCALL SUBOPT_0xC
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0xD
	POP  R26
	POP  R27
	RCALL SUBOPT_0xE
	__GETD2S 9
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __CDF1
	RCALL SUBOPT_0xF
	RJMP _0x10
_0x12:
	RCALL SUBOPT_0x10
_0xAD:
	RCALL __LOADLOCR2
	ADIW R28,13
	RET
__ftoe_G2:
	SBIW R28,4
	RCALL __SAVELOCR3
	__GETD1N 0x3F800000
	__PUTD1S 3
	LDD  R26,Y+10
	RCALL SUBOPT_0x6
	BRSH _0x13
	LDI  R30,LOW(5)
	STD  Y+10,R30
_0x13:
	LDD  R16,Y+10
_0x14:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0x16
	RCALL SUBOPT_0x13
	__PUTD1S 3
	RJMP _0x14
_0x16:
	__GETD1S 11
	RCALL __CPD10
	BRNE _0x17
	LDI  R18,LOW(0)
	RCALL SUBOPT_0x13
	__PUTD1S 3
	RJMP _0x18
_0x17:
	LDD  R18,Y+10
	RCALL SUBOPT_0x14
	BREQ PC+2
	BRCC PC+2
	RJMP _0x19
	RCALL SUBOPT_0x13
	__PUTD1S 3
_0x1A:
	RCALL SUBOPT_0x14
	BRLO _0x1C
	RCALL SUBOPT_0x15
	SUBI R18,-LOW(1)
	RJMP _0x1A
_0x1C:
	RJMP _0x1D
_0x19:
_0x1E:
	RCALL SUBOPT_0x14
	BRSH _0x20
	__GETD2S 11
	RCALL SUBOPT_0x12
	__PUTD1S 11
	SUBI R18,LOW(1)
	RJMP _0x1E
_0x20:
	RCALL SUBOPT_0x13
	__PUTD1S 3
_0x1D:
	__GETD1S 11
	__GETD2N 0x3F000000
	RCALL __ADDF12
	__PUTD1S 11
	RCALL SUBOPT_0x14
	BRLO _0x21
	RCALL SUBOPT_0x15
	SUBI R18,-LOW(1)
_0x21:
_0x18:
	LDI  R16,LOW(0)
_0x22:
	LDD  R30,Y+10
	CP   R30,R16
	BRLO _0x24
	__GETD2S 3
	__GETD1N 0x41200000
	RCALL SUBOPT_0xA
	__PUTD1S 3
	__GETD2S 11
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x16
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0xD
	POP  R26
	POP  R27
	RCALL SUBOPT_0xE
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __CDF1
	__GETD2S 3
	RCALL __MULF12
	__GETD2S 11
	RCALL __SWAPD12
	RCALL __SUBF12
	__PUTD1S 11
	MOV  R30,R16
	SUBI R16,-1
	CPI  R30,0
	BRNE _0x22
	RCALL SUBOPT_0x17
	RCALL SUBOPT_0x11
	RJMP _0x22
_0x24:
	RCALL SUBOPT_0x16
	LDD  R26,Y+9
	STD  Z+0,R26
	CPI  R18,0
	BRGE _0x26
	RCALL SUBOPT_0x17
	LDI  R30,LOW(45)
	ST   X,R30
	NEG  R18
_0x26:
	LDI  R30,LOW(9)
	CP   R30,R18
	BRGE _0x27
	RCALL SUBOPT_0x16
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x18
	RCALL __DIVB21
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
_0x27:
	RCALL SUBOPT_0x16
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x18
	RCALL __MODB21
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	LDI  R30,LOW(0)
	ST   X,R30
	RCALL __LOADLOCR3
	ADIW R28,15
	RET
__print_sign_G2:
    ;flags&=~F_SIGNED
    andi r17,~4
    ;_put(s,ptr_ps)
    ldd  r30,y+17
    st   -y,r30
    ldd  r30,y+19+16
    ldd  r31,y+20+16
    st   -y,r31
    st   -y,r30
    rcall put
    ;if (width) --width
    cpi  r20,0
    breq width0
    dec  r20
width0:
	RET
__print_G2:
	SBIW R28,28
	RCALL __SAVELOCR6
	LDI  R16,0
_0x28:
	LDD  R30,Y+38
	LDD  R31,Y+38+1
	ADIW R30,1
	STD  Y+38,R30
	STD  Y+38+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x2A
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x2E
	CPI  R19,37
	BRNE _0x2F
	LDI  R16,LOW(1)
	RJMP _0x30
_0x2F:
	RCALL SUBOPT_0x19
_0x30:
	RJMP _0x2D
_0x2E:
	CPI  R30,LOW(0x1)
	BRNE _0x31
	CPI  R19,37
	BRNE _0x32
	RCALL SUBOPT_0x19
	LDI  R16,LOW(0)
	RJMP _0x2D
_0x32:
	LDI  R16,LOW(2)
	LDI  R30,LOW(0)
	STD  Y+17,R30
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x33
	LDI  R17,LOW(1)
	RJMP _0x2D
_0x33:
	CPI  R19,43
	BRNE _0x34
	LDI  R30,LOW(43)
	STD  Y+17,R30
	RJMP _0x2D
_0x34:
	CPI  R19,32
	BRNE _0x35
	LDI  R30,LOW(32)
	STD  Y+17,R30
	RJMP _0x2D
_0x35:
	RJMP _0x36
_0x31:
	CPI  R30,LOW(0x2)
	BRNE _0x37
_0x36:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x38
	ORI  R17,LOW(128)
	RJMP _0x2D
_0x38:
	RJMP _0x39
_0x37:
	CPI  R30,LOW(0x3)
	BRNE _0x3A
_0x39:
	CPI  R19,48
	BRLO _0x3C
	CPI  R19,58
	BRLO _0x3D
_0x3C:
	RJMP _0x3B
_0x3D:
	MOV  R26,R20
	RCALL SUBOPT_0x1A
	MOV  R20,R30
	RCALL SUBOPT_0x1B
	ADD  R20,R30
	RJMP _0x2D
_0x3B:
	LDI  R21,LOW(0)
	CPI  R19,46
	BRNE _0x3E
	LDI  R16,LOW(4)
	RJMP _0x2D
_0x3E:
	RJMP _0x3F
_0x3A:
	CPI  R30,LOW(0x4)
	BRNE _0x41
	CPI  R19,48
	BRLO _0x43
	CPI  R19,58
	BRLO _0x44
_0x43:
	RJMP _0x42
_0x44:
	ORI  R17,LOW(32)
	MOV  R26,R21
	RCALL SUBOPT_0x1A
	MOV  R21,R30
	RCALL SUBOPT_0x1B
	ADD  R21,R30
	RJMP _0x2D
_0x42:
_0x3F:
	CPI  R19,108
	BRNE _0x45
	ORI  R17,LOW(2)
	LDI  R16,LOW(5)
	RJMP _0x2D
_0x45:
	RJMP _0x46
_0x41:
	CPI  R30,LOW(0x5)
	BREQ PC+2
	RJMP _0x2D
_0x46:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x4B
	RCALL SUBOPT_0x1C
	LD   R30,X
	RCALL SUBOPT_0x1D
	RJMP _0x4C
_0x4B:
	CPI  R30,LOW(0x45)
	BREQ _0x4F
	CPI  R30,LOW(0x65)
	BRNE _0x50
_0x4F:
	RJMP _0x51
_0x50:
	CPI  R30,LOW(0x66)
	BRNE _0x52
_0x51:
	RCALL SUBOPT_0x1E
	RCALL SUBOPT_0x1C
	RCALL SUBOPT_0x1F
	MOVW R26,R30
	MOVW R24,R22
	RCALL __CPD20
	BRLT _0x53
	LDD  R26,Y+17
	CPI  R26,LOW(0x2B)
	BREQ _0x55
	RJMP _0x56
_0x53:
	__GETD1S 8
	RCALL __ANEGF1
	RCALL SUBOPT_0x20
_0x55:
	SBRS R17,7
	RJMP _0x57
	LDD  R30,Y+17
	RCALL SUBOPT_0x1D
	RJMP _0x58
_0x57:
	RCALL SUBOPT_0xC
	LDD  R26,Y+17
	STD  Z+0,R26
_0x58:
_0x56:
	SBRS R17,5
	LDI  R21,LOW(5)
	CPI  R19,102
	BRNE _0x5A
	RCALL SUBOPT_0x21
	LDD  R30,Y+11
	LDD  R31,Y+11+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __ftoa_G2
	RJMP _0x5B
_0x5A:
	RCALL SUBOPT_0x21
	ST   -Y,R19
	LDD  R30,Y+12
	LDD  R31,Y+12+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __ftoe_G2
_0x5B:
	RCALL SUBOPT_0x1E
	RCALL SUBOPT_0x22
	RJMP _0x5C
_0x52:
	CPI  R30,LOW(0x73)
	BRNE _0x5E
	RCALL SUBOPT_0x1C
	RCALL SUBOPT_0x23
	RCALL SUBOPT_0x22
	RJMP _0x5F
_0x5E:
	CPI  R30,LOW(0x70)
	BRNE _0x61
	RCALL SUBOPT_0x1C
	RCALL SUBOPT_0x23
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x5F:
	ANDI R17,LOW(127)
	CPI  R21,0
	BREQ _0x63
	CP   R21,R16
	BRLO _0x64
_0x63:
	RJMP _0x62
_0x64:
	MOV  R16,R21
_0x62:
_0x5C:
	LDI  R21,LOW(0)
	LDI  R30,LOW(0)
	STD  Y+16,R30
	LDI  R18,LOW(0)
	RJMP _0x65
_0x61:
	CPI  R30,LOW(0x64)
	BREQ _0x68
	CPI  R30,LOW(0x69)
	BRNE _0x69
_0x68:
	ORI  R17,LOW(4)
	RJMP _0x6A
_0x69:
	CPI  R30,LOW(0x75)
	BRNE _0x6B
_0x6A:
	LDI  R30,LOW(10)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x6C
	__GETD1N 0x3B9ACA00
	__PUTD1S 12
	LDI  R16,LOW(10)
	RJMP _0x6D
_0x6C:
	__GETD1N 0x2710
	__PUTD1S 12
	LDI  R16,LOW(5)
	RJMP _0x6D
_0x6B:
	CPI  R30,LOW(0x58)
	BRNE _0x6F
	ORI  R17,LOW(8)
	RJMP _0x70
_0x6F:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0xAC
_0x70:
	LDI  R30,LOW(16)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x72
	__GETD1N 0x10000000
	__PUTD1S 12
	LDI  R16,LOW(8)
	RJMP _0x6D
_0x72:
	__GETD1N 0x1000
	__PUTD1S 12
	LDI  R16,LOW(4)
_0x6D:
	CPI  R21,0
	BREQ _0x73
	ANDI R17,LOW(127)
	RJMP _0x74
_0x73:
	LDI  R21,LOW(1)
_0x74:
	SBRS R17,1
	RJMP _0x75
	RCALL SUBOPT_0x1C
	RCALL SUBOPT_0x1F
	RJMP _0x76
_0x75:
	SBRS R17,2
	RJMP _0x77
	RCALL SUBOPT_0x1C
	RCALL __GETW1P
	RCALL __CWD1
	RJMP _0xAE
_0x77:
	RCALL SUBOPT_0x1C
	RCALL __GETW1P
	CLR  R22
	CLR  R23
_0xAE:
	__PUTD1S 8
_0x76:
	SBRS R17,2
	RJMP _0x79
	__GETD2S 8
	RCALL __CPD20
	BRGE _0x7A
	__GETD1S 8
	RCALL __ANEGD1
	RCALL SUBOPT_0x20
_0x7A:
	LDD  R30,Y+17
	CPI  R30,0
	BREQ _0x7B
	SUBI R16,-LOW(1)
	SUBI R21,-LOW(1)
	RJMP _0x7C
_0x7B:
	ANDI R17,LOW(251)
_0x7C:
_0x79:
	MOV  R18,R21
_0x65:
	SBRC R17,0
	RJMP _0x7D
_0x7E:
	CP   R16,R20
	BRSH _0x81
	CP   R18,R20
	BRLO _0x82
_0x81:
	RJMP _0x80
_0x82:
	SBRS R17,7
	RJMP _0x83
	SBRS R17,2
	RJMP _0x84
	ANDI R17,LOW(251)
	LDD  R19,Y+17
	SUBI R16,LOW(1)
	RJMP _0x85
_0x84:
	LDI  R19,LOW(48)
_0x85:
	RJMP _0x86
_0x83:
	LDI  R19,LOW(32)
_0x86:
	RCALL SUBOPT_0x19
	SUBI R20,LOW(1)
	RJMP _0x7E
_0x80:
_0x7D:
_0x87:
	CP   R16,R21
	BRSH _0x89
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0x8A
	RCALL __print_sign_G2
	SUBI R16,LOW(1)
	SUBI R21,LOW(1)
_0x8A:
	LDI  R30,LOW(48)
	RCALL SUBOPT_0x1D
	CPI  R20,0
	BREQ _0x8B
	SUBI R20,LOW(1)
_0x8B:
	SUBI R21,LOW(1)
	RJMP _0x87
_0x89:
	MOV  R18,R16
	LDD  R30,Y+16
	CPI  R30,0
	BRNE _0x8C
_0x8D:
	CPI  R18,0
	BREQ _0x8F
	SBRS R17,3
	RJMP _0x90
	RCALL SUBOPT_0xC
	LPM  R30,Z
	RJMP _0xAF
_0x90:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xAF:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G2
	CPI  R20,0
	BREQ _0x92
	SUBI R20,LOW(1)
_0x92:
	SUBI R18,LOW(1)
	RJMP _0x8D
_0x8F:
	RJMP _0x93
_0x8C:
_0x95:
	RCALL SUBOPT_0x24
	RCALL __DIVD21U
	MOV  R19,R30
	LDI  R30,LOW(9)
	CP   R30,R19
	BRSH _0x97
	SBRS R17,3
	RJMP _0x98
	SUBI R19,-LOW(55)
	RJMP _0x99
_0x98:
	SUBI R19,-LOW(87)
_0x99:
	RJMP _0x9A
_0x97:
	SUBI R19,-LOW(48)
_0x9A:
	SBRC R17,4
	RJMP _0x9C
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x9E
	__GETD2S 12
	__CPD2N 0x1
	BRNE _0x9D
_0x9E:
	RJMP _0xA0
_0x9D:
	CP   R21,R18
	BRLO _0xA1
	LDI  R19,LOW(48)
	RJMP _0xA0
_0xA1:
	CP   R20,R18
	BRLO _0xA3
	SBRS R17,0
	RJMP _0xA4
_0xA3:
	RJMP _0xA2
_0xA4:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0xA5
	LDI  R19,LOW(48)
_0xA0:
	ORI  R17,LOW(16)
	SBRC R17,2
	RCALL __print_sign_G2
_0xA5:
_0x9C:
	RCALL SUBOPT_0x19
	CPI  R20,0
	BREQ _0xA7
	SUBI R20,LOW(1)
_0xA7:
_0xA2:
	SUBI R18,LOW(1)
	RCALL SUBOPT_0x24
	RCALL __MODD21U
	__PUTD1S 8
	LDD  R30,Y+16
	__GETD2S 12
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __DIVD21U
	__PUTD1S 12
	RCALL __CPD10
	BREQ _0x96
	RJMP _0x95
_0x96:
_0x93:
	SBRS R17,0
	RJMP _0xA8
_0xA9:
	CPI  R20,0
	BREQ _0xAB
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x1D
	RJMP _0xA9
_0xAB:
_0xA8:
_0xAC:
_0x4C:
	LDI  R16,LOW(0)
_0x2D:
	RJMP _0x28
_0x2A:
	RCALL __LOADLOCR6
	ADIW R28,40
	RET
_printf:
	PUSH R15
	MOV  R15,R24
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
	RCALL __print_G2
	RCALL __LOADLOCR2
	ADIW R28,4
	POP  R15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	IN   R30,0x2E
	ANDI R30,LOW(0xF8)
	OUT  0x2E,R30
	IN   R30,0x39
	ANDI R30,0xDF
	OUT  0x39,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	RJMP _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	IN   R30,0x39
	ORI  R30,0x20
	OUT  0x39,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	LDI  R30,LOW(0)
	OUT  0x2D,R30
	OUT  0x2C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	IN   R30,0x2E
	ORI  R30,LOW(0x3)
	OUT  0x2E,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	LDI  R30,LOW(5)
	CP   R30,R26
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	__GETD1S 2
	__GETD2S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	__GETD2S 2
	__GETD1N 0x41200000
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x9:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	RCALL __DIVF21
	__GETD2N 0x3F000000
	RCALL __ADDF12
	RCALL __PUTPARD1
	RJMP _floor

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	RCALL __DIVF21
	RCALL __CFD1
	MOV  R17,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xC:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xD:
	MOV  R30,R17
	SUBI R30,-LOW(48)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xE:
	ST   X,R30
	MOV  R30,R17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	RCALL __SWAPD12
	RCALL __SUBF12
	__PUTD1S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(0)
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	LDI  R30,LOW(46)
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x12:
	__GETD1N 0x41200000
	RCALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x13:
	__GETD2S 3
	RJMP SUBOPT_0x12

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x14:
	__GETD1S 3
	__GETD2S 11
	RCALL __CMPF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	__GETD2S 11
	__GETD1N 0x41200000
	RCALL __DIVF21
	__PUTD1S 11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x16:
	LDD  R30,Y+7
	LDD  R31,Y+7+1
	ADIW R30,1
	STD  Y+7,R30
	STD  Y+7+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x17:
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	ADIW R26,1
	STD  Y+7,R26
	STD  Y+7+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x18:
	MOV  R26,R18
	LDI  R30,LOW(10)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x19:
	ST   -Y,R19
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1A:
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1B:
	MOV  R30,R19
	SUBI R30,LOW(48)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x1C:
	LDD  R26,Y+36
	LDD  R27,Y+36+1
	SBIW R26,4
	STD  Y+36,R26
	STD  Y+36+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1D:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1E:
	MOVW R30,R28
	ADIW R30,18
	STD  Y+6,R30
	STD  Y+6+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1F:
	RCALL __GETD1P
	__PUTD1S 8
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x20:
	__PUTD1S 8
	LDI  R30,LOW(45)
	STD  Y+17,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x21:
	__GETD1S 8
	RCALL __PUTPARD1
	ST   -Y,R21
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x22:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlen
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x23:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x24:
	__GETD1S 12
	__GETD2S 8
	RET

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
	MOV  R31,R30
	ADD  R31,R31
	SBC  R31,R31
	MOV  R22,R31
	MOV  R23,R31
	RET

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
	RET

__DIVB21U:
	CLR  R0
	LDI  R25,8
__DIVB21U1:
	LSL  R26
	ROL  R0
	SUB  R0,R30
	BRCC __DIVB21U2
	ADD  R0,R30
	RJMP __DIVB21U3
__DIVB21U2:
	SBR  R26,1
__DIVB21U3:
	DEC  R25
	BRNE __DIVB21U1
	MOV  R30,R26
	MOV  R26,R0
	RET

__DIVB21:
	RCALL __CHKSIGNB
	RCALL __DIVB21U
	BRTC __DIVB211
	NEG  R30
__DIVB211:
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

__DIVD21U:
	PUSH R19
	PUSH R20
	PUSH R21
	CLR  R0
	CLR  R1
	CLR  R19
	CLR  R20
	LDI  R21,32
__DIVD21U1:
	LSL  R26
	ROL  R27
	ROL  R24
	ROL  R25
	ROL  R0
	ROL  R1
	ROL  R19
	ROL  R20
	SUB  R0,R30
	SBC  R1,R31
	SBC  R19,R22
	SBC  R20,R23
	BRCC __DIVD21U2
	ADD  R0,R30
	ADC  R1,R31
	ADC  R19,R22
	ADC  R20,R23
	RJMP __DIVD21U3
__DIVD21U2:
	SBR  R26,1
__DIVD21U3:
	DEC  R21
	BRNE __DIVD21U1
	MOVW R30,R26
	MOVW R22,R24
	MOVW R26,R0
	MOV  R24,R19
	MOV  R25,R20
	POP  R21
	POP  R20
	POP  R19
	RET

__MODB21:
	CLT
	SBRS R26,7
	RJMP __MODB211
	NEG  R26
	SET
__MODB211:
	SBRC R30,7
	NEG  R30
	RCALL __DIVB21U
	MOV  R30,R26
	BRTC __MODB212
	NEG  R30
__MODB212:
	RET

__MODD21U:
	RCALL __DIVD21U
	MOVW R30,R26
	MOVW R22,R24
	RET

__CHKSIGNB:
	CLT
	SBRS R30,7
	RJMP __CHKSB1
	NEG  R30
	SET
__CHKSB1:
	SBRS R26,7
	RJMP __CHKSB2
	NEG  R26
	BLD  R0,0
	INC  R0
	BST  R0,0
__CHKSB2:
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

__CPD10:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	RET

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

;END OF CODE MARKER
__END_OF_CODE:
