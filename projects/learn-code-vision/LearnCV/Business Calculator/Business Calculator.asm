;CodeVisionAVR C Compiler V1.24.6 Professional
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega8535
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
;8 bit enums            : No
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

	.INCLUDE "Business Calculator.vec"
	.INCLUDE "Business Calculator.inc"

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
;       1 #include <mega8535.h>
;       2 
;       3 
;       4 #include <stdio.h> 
;       5  
;       6 //flash char fca[]="Scorpion Controls";
;       7 
;       8 //flash char *fptr; 
;       9 eeprom unsigned char *eepptr;
;      10 
;      11 unsigned char a,b,c;
;      12 //pragma w-
;      13 
;      14 //pragma w+
;      15 
;      16 void add(void)
;      17 {           

	.CSEG
_add:
;      18         printf("Enter the 1st digit to ADD");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      19         scanf("%x",&a);    
	__POINTW1FN _0,27
	RCALL SUBOPT_0x1
	LDI  R24,4
	RCALL SUBOPT_0x2
;      20         *++eepptr = a;
	MOVW R26,R30
	MOV  R30,R6
	RCALL __EEPROMWRB
;      21         printf("Enter the 2nd digit to ADD");
	__POINTW1FN _0,30
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      22         scanf("%x",&b);
	__POINTW1FN _0,27
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x3
	LDI  R24,4
	RCALL SUBOPT_0x2
;      23         *++eepptr = b;
	MOVW R26,R30
	MOV  R30,R7
	RCALL __EEPROMWRB
;      24         c=*--eepptr + *++eepptr;                     
	__GETW1R 4,5
	SBIW R30,1
	RCALL SUBOPT_0x4
	PUSH R30
	RCALL SUBOPT_0x5
	RCALL SUBOPT_0x4
	POP  R26
	ADD  R30,R26
	MOV  R8,R30
;      25         *++eepptr = c;
	RCALL SUBOPT_0x5
	__PUTW1R 4,5
	MOVW R26,R30
	MOV  R30,R8
	RCALL __EEPROMWRB
;      26         printf("Added Result is:%d",c);
	__POINTW1FN _0,57
	RCALL SUBOPT_0x6
	LDI  R24,4
	RCALL _printf
	ADIW R28,6
;      27         a=b=c=0;
	LDI  R30,LOW(0)
	MOV  R8,R30
	MOV  R7,R30
	MOV  R6,R30
;      28 }
	RET
;      29 void sub(void)
;      30 {
_sub:
;      31         printf("Enter two digit to SUB"); 
	__POINTW1FN _0,76
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      32         scanf("%x%x",&a,&b);
	__POINTW1FN _0,99
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x3
	LDI  R24,8
	RCALL SUBOPT_0x7
;      33         c=a-b;  
	MOV  R30,R6
	SUB  R30,R7
	MOV  R8,R30
;      34         printf("Subtracted Result is:%d",c);
	__POINTW1FN _0,104
	RCALL SUBOPT_0x6
	LDI  R24,4
	RCALL _printf
	RJMP _0xAE
;      35         
;      36 }
;      37 void mul(void)
;      38 {
_mul:
;      39         printf("Enter two digit to MUL");
	__POINTW1FN _0,128
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      40         scanf("%x%x",&a,&b);
	__POINTW1FN _0,99
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x3
	LDI  R24,8
	RCALL SUBOPT_0x7
;      41         c=a*b;  
	__MULBRRU 7,6
	MOV  R8,R30
;      42         printf("Multiplied Result is:%d",c);
	__POINTW1FN _0,151
	RCALL SUBOPT_0x6
	LDI  R24,4
	RCALL _printf
	RJMP _0xAE
;      43 }
;      44 void div(void)
;      45 {
_div:
;      46         printf("Enter two digit to DIV");
	__POINTW1FN _0,175
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      47         scanf("%x%x",&a,&b);
	__POINTW1FN _0,99
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x3
	LDI  R24,8
	RCALL SUBOPT_0x7
;      48         c=a/b;  
	MOV  R30,R7
	MOV  R26,R6
	RCALL __DIVB21U
	MOV  R8,R30
;      49         printf("Divided Result is:%d",c);
	__POINTW1FN _0,198
	RCALL SUBOPT_0x6
	LDI  R24,4
	RCALL _printf
_0xAE:
	ADIW R28,6
;      50 }
	RET
;      51 void wreep(unsigned char);
;      52  
;      53 
;      54 void (*funp[5])(void)= {add,sub,mul,div};

	.DSEG
_funp:
	.BYTE 0xA
;      55 
;      56 
;      57 void main(void)
;      58 {               

	.CSEG
_main:
;      59         void (*fp) (void);
;      60         unsigned char FunSel;
;      61         
;      62         UBRRL=0x0C;
	SBIW R28,2
;	*fp -> Y+0
;	FunSel -> R16
	LDI  R30,LOW(12)
	OUT  0x9,R30
;      63         UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;      64         UCSRA=0x02;
	LDI  R30,LOW(2)
	OUT  0xB,R30
;      65         UCSRB=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
;      66         UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;      67          
;      68         while(1)
_0x4:
;      69         {       
;      70                 printf("Select any one operation representing it's serial no:");
	__POINTW1FN _0,219
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      71                 printf("(1)ADD(2)SUB(3)MUL(4)DIV");
	__POINTW1FN _0,273
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      72                 scanf("%x",&FunSel);
	__POINTW1FN _0,27
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
;      73                 wreep(FunSel);
	ST   -Y,R16
	RCALL _wreep
;      74         
;      75                 if((FunSel>0x00) && (FunSel<0x05))
	LDI  R30,LOW(0)
	CP   R30,R16
	BRSH _0x8
	CPI  R16,5
	BRLO _0x9
_0x8:
	RJMP _0x7
_0x9:
;      76                 {
;      77                         FunSel=FunSel-0x01;                         
	SUBI R16,LOW(1)
;      78                         fp=funp[FunSel];
	MOV  R30,R16
	LDI  R26,LOW(_funp)
	LDI  R27,HIGH(_funp)
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	ST   Y,R30
	STD  Y+1,R31
;      79                         (*fp)();   
	ICALL
;      80                 }             
;      81                 
;      82                 printf("End");
_0x7:
	__POINTW1FN _0,298
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x0
;      83         }
	RJMP _0x4
;      84         
;      85 }
_0xA:
	RJMP _0xA
;      86 void wreep(unsigned char data)
;      87 {
_wreep:
;      88    *eepptr = data;
	LD   R30,Y
	__GETW2R 4,5
	RCALL __EEPROMWRB
;      89 }
	ADIW R28,1
	RET
;      90    
;      91 

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
	BREQ _0xB
	RCALL SUBOPT_0x8
	RCALL SUBOPT_0x9
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0xC
_0xB:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0xC:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0xD:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	RCALL SUBOPT_0xA
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0xF
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x13
	CPI  R19,37
	BRNE _0x14
	LDI  R16,LOW(1)
	RJMP _0x15
_0x14:
	RCALL SUBOPT_0xB
_0x15:
	RJMP _0x12
_0x13:
	CPI  R30,LOW(0x1)
	BRNE _0x16
	CPI  R19,37
	BRNE _0x17
	RCALL SUBOPT_0xB
	LDI  R16,LOW(0)
	RJMP _0x12
_0x17:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x18
	LDI  R17,LOW(1)
	RJMP _0x12
_0x18:
	CPI  R19,43
	BRNE _0x19
	LDI  R21,LOW(43)
	RJMP _0x12
_0x19:
	CPI  R19,32
	BRNE _0x1A
	LDI  R21,LOW(32)
	RJMP _0x12
_0x1A:
	RJMP _0x1B
_0x16:
	CPI  R30,LOW(0x2)
	BRNE _0x1C
_0x1B:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x1D
	ORI  R17,LOW(128)
	RJMP _0x12
_0x1D:
	RJMP _0x1E
_0x1C:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x12
_0x1E:
	CPI  R19,48
	BRLO _0x21
	CPI  R19,58
	BRLO _0x22
_0x21:
	RJMP _0x20
_0x22:
	RCALL SUBOPT_0xC
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x12
_0x20:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x26
	RCALL SUBOPT_0xD
	LD   R30,X
	RCALL SUBOPT_0xE
	RJMP _0x27
_0x26:
	CPI  R30,LOW(0x73)
	BRNE _0x29
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0xF
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x2A
_0x29:
	CPI  R30,LOW(0x70)
	BRNE _0x2C
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0xF
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x2A:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x2D
_0x2C:
	CPI  R30,LOW(0x64)
	BREQ _0x30
	CPI  R30,LOW(0x69)
	BRNE _0x31
_0x30:
	ORI  R17,LOW(4)
	RJMP _0x32
_0x31:
	CPI  R30,LOW(0x75)
	BRNE _0x33
_0x32:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x34
_0x33:
	CPI  R30,LOW(0x58)
	BRNE _0x36
	ORI  R17,LOW(8)
	RJMP _0x37
_0x36:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x65
_0x37:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x34:
	SBRS R17,2
	RJMP _0x39
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0x10
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x3A
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x3A:
	CPI  R21,0
	BREQ _0x3B
	SUBI R16,-LOW(1)
	RJMP _0x3C
_0x3B:
	ANDI R17,LOW(251)
_0x3C:
	RJMP _0x3D
_0x39:
	RCALL SUBOPT_0xD
	RCALL SUBOPT_0x10
_0x3D:
_0x2D:
	SBRC R17,0
	RJMP _0x3E
_0x3F:
	CP   R16,R20
	BRSH _0x41
	SBRS R17,7
	RJMP _0x42
	SBRS R17,2
	RJMP _0x43
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x44
_0x43:
	LDI  R19,LOW(48)
_0x44:
	RJMP _0x45
_0x42:
	LDI  R19,LOW(32)
_0x45:
	RCALL SUBOPT_0xB
	SUBI R20,LOW(1)
	RJMP _0x3F
_0x41:
_0x3E:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x46
_0x47:
	CPI  R18,0
	BREQ _0x49
	SBRS R17,3
	RJMP _0x4A
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	RCALL SUBOPT_0xA
	RJMP _0xAF
_0x4A:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xAF:
	ST   -Y,R30
	RCALL SUBOPT_0x11
	CPI  R20,0
	BREQ _0x4C
	SUBI R20,LOW(1)
_0x4C:
	SUBI R18,LOW(1)
	RJMP _0x47
_0x49:
	RJMP _0x4D
_0x46:
_0x4F:
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
	BRSH _0x51
	SBRS R17,3
	RJMP _0x52
	SUBI R19,-LOW(7)
	RJMP _0x53
_0x52:
	SUBI R19,-LOW(39)
_0x53:
_0x51:
	SBRC R17,4
	RJMP _0x55
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x57
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x56
_0x57:
	ORI  R17,LOW(16)
	RJMP _0x59
_0x56:
	CP   R20,R18
	BRLO _0x5B
	SBRS R17,0
	RJMP _0x5C
_0x5B:
	RJMP _0x5A
_0x5C:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x5D
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x59:
	SBRS R17,2
	RJMP _0x5E
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0x11
	CPI  R20,0
	BREQ _0x5F
	SUBI R20,LOW(1)
_0x5F:
_0x5E:
_0x5D:
_0x55:
	RCALL SUBOPT_0xB
	CPI  R20,0
	BREQ _0x60
	SUBI R20,LOW(1)
_0x60:
_0x5A:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x50
	RJMP _0x4F
_0x50:
_0x4D:
	SBRS R17,0
	RJMP _0x61
_0x62:
	CPI  R20,0
	BREQ _0x64
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0xE
	RJMP _0x62
_0x64:
_0x61:
_0x65:
_0x27:
	LDI  R16,LOW(0)
_0x12:
	RJMP _0xD
_0xF:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x12
	RCALL __print_G2
	RJMP _0xAC
__get_G2:
	ST   -Y,R16
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LD   R30,X
	RCALL SUBOPT_0x13
	BREQ _0x66
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	RCALL SUBOPT_0x14
	RJMP _0x67
_0x66:
	RCALL SUBOPT_0x15
	SBIW R30,0
	BREQ _0x68
	RCALL SUBOPT_0x15
	LD   R30,Z
	RCALL SUBOPT_0x13
	BREQ _0x69
	RCALL SUBOPT_0x15
	RCALL SUBOPT_0x9
_0x69:
	RJMP _0x6A
_0x68:
	RCALL _getchar
	MOV  R16,R30
_0x6A:
_0x67:
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
_0x6B:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	ADIW R30,1
	STD  Y+14,R30
	STD  Y+14+1,R31
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0x16
	BRNE PC+2
	RJMP _0x6D
	RCALL SUBOPT_0x17
	BREQ _0x6E
_0x6F:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	RCALL SUBOPT_0x16
	BREQ _0x72
	RCALL SUBOPT_0x17
	BRNE _0x73
_0x72:
	RJMP _0x71
_0x73:
	RJMP _0x6F
_0x71:
	MOV  R21,R18
	RJMP _0x74
_0x6E:
	CPI  R18,37
	BREQ PC+2
	RJMP _0x75
	LDI  R20,LOW(0)
_0x76:
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	LPM  R18,Z+
	STD  Y+14,R30
	STD  Y+14+1,R31
	CPI  R18,48
	BRLO _0x7A
	LDI  R30,LOW(57)
	CP   R30,R18
	BRSH _0x79
_0x7A:
	RJMP _0x78
_0x79:
	RCALL SUBOPT_0xC
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x76
_0x78:
	CPI  R18,0
	BRNE _0x7C
	RJMP _0x6D
_0x7C:
_0x7D:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	MOV  R19,R30
	ST   -Y,R30
	RCALL _isspace
	CPI  R30,0
	BRNE _0x7D
	CPI  R19,0
	BRNE _0x80
	RJMP _0x81
_0x80:
	MOV  R21,R19
	CPI  R20,0
	BRNE _0x82
	LDI  R20,LOW(255)
_0x82:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0x86
	RCALL SUBOPT_0x19
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	__GETW2R 16,17
	ST   X,R30
	RJMP _0x85
_0x86:
	CPI  R30,LOW(0x73)
	BRNE _0x8E
	RCALL SUBOPT_0x19
_0x88:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0x8A
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	RCALL SUBOPT_0x16
	BREQ _0x8C
	RCALL SUBOPT_0x17
	BREQ _0x8B
_0x8C:
	RJMP _0x8A
_0x8B:
	PUSH R17
	PUSH R16
	__ADDWRN 16,17,1
	MOV  R30,R18
	POP  R26
	POP  R27
	ST   X,R30
	RJMP _0x88
_0x8A:
	__GETW2R 16,17
	RCALL SUBOPT_0x14
	RJMP _0x85
_0x8E:
	RCALL SUBOPT_0x1A
	MOV  R30,R18
	CPI  R30,LOW(0x64)
	BREQ _0x93
	CPI  R30,LOW(0x69)
	BRNE _0x94
_0x93:
	LDI  R30,LOW(0)
	STD  Y+8,R30
	RJMP _0x95
_0x94:
	CPI  R30,LOW(0x75)
	BRNE _0x96
_0x95:
	LDI  R19,LOW(10)
	RJMP _0x91
_0x96:
	CPI  R30,LOW(0x78)
	BRNE _0x97
	LDI  R19,LOW(16)
	RJMP _0x91
_0x97:
	CPI  R30,LOW(0x25)
	BRNE _0x9A
	RJMP _0x99
_0x9A:
	LDD  R30,Y+9
	RJMP _0xAD
_0x91:
	LDI  R30,0
	STD  Y+6,R30
	STD  Y+6+1,R30
_0x9B:
	MOV  R30,R20
	SUBI R20,1
	CPI  R30,0
	BREQ _0x9D
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	MOV  R18,R30
	MOV  R26,R30
	LDI  R30,LOW(32)
	CP   R30,R26
	BRSH _0x9F
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0xA0
	CPI  R18,45
	BRNE _0xA1
	LDI  R30,LOW(255)
	STD  Y+8,R30
	RJMP _0x9B
_0xA1:
	RCALL SUBOPT_0x1A
_0xA0:
	CPI  R18,48
	BRLO _0x9F
	CPI  R18,97
	BRLO _0xA4
	SUBI R18,LOW(87)
	RJMP _0xA5
_0xA4:
	CPI  R18,65
	BRLO _0xA6
	SUBI R18,LOW(55)
	RJMP _0xA7
_0xA6:
	SUBI R18,LOW(48)
_0xA7:
_0xA5:
	CP   R18,R19
	BRLO _0xA8
_0x9F:
	MOV  R21,R18
	RJMP _0x9D
_0xA8:
	MOV  R30,R19
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R31,0
	RCALL __MULW12U
	MOVW R26,R30
	MOV  R30,R18
	LDI  R31,0
	ADD  R30,R26
	ADC  R31,R27
	STD  Y+6,R30
	STD  Y+6+1,R31
	RJMP _0x9B
_0x9D:
	RCALL SUBOPT_0x19
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
_0x85:
	LDD  R30,Y+9
	SUBI R30,-LOW(1)
	STD  Y+9,R30
	RJMP _0xA9
_0x75:
_0x99:
	IN   R30,SPL
	IN   R31,SPH
	ST   -Y,R31
	ST   -Y,R30
	PUSH R21
	RCALL SUBOPT_0x18
	POP  R21
	CP   R30,R18
	BREQ _0xAA
_0x81:
	LDD  R30,Y+9
	CPI  R30,0
	BRNE _0xAB
	LDI  R30,LOW(255)
	RJMP _0xAD
_0xAB:
	RJMP _0x6D
_0xAA:
_0xA9:
_0x74:
	RJMP _0x6B
_0x6D:
	LDD  R30,Y+9
_0xAD:
	RCALL __LOADLOCR6
	ADIW R28,16
	RET
_scanf:
	PUSH R15
	MOV  R15,R24
	RCALL SUBOPT_0x12
	RCALL __scanf_G2
_0xAC:
	RCALL __LOADLOCR2
	ADIW R28,4
	POP  R15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 8 TIMES
SUBOPT_0x0:
	RCALL _printf
	ADIW R28,2
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1:
	ST   -Y,R31
	ST   -Y,R30
	__GETD1N 0x6
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	RCALL _scanf
	ADIW R28,6
	__GETW1R 4,5
	ADIW R30,1
	__PUTW1R 4,5
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x3:
	__GETD1N 0x7
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	__PUTW1R 4,5
	MOVW R26,R30
	RCALL __EEPROMRDB
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	__GETW1R 4,5
	ADIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x6:
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R8
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x7:
	RCALL _scanf
	ADIW R28,10
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x9:
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xA:
	SBIW R30,1
	LPM  R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xB:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0xD:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
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
SUBOPT_0x13:
	MOV  R16,R30
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	LDI  R30,LOW(0)
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x15:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x16:
	MOV  R18,R30
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x17:
	ST   -Y,R18
	RCALL _isspace
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x18:
	LDD  R30,Y+12
	LDD  R31,Y+12+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __get_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x19:
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
SUBOPT_0x1A:
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
