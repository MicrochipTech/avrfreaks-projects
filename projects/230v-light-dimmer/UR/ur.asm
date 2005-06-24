;CodeVisionAVR C Compiler V1.24.6 Standard
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega16
;Program type           : Application
;Clock frequency        : 8,000000 MHz
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

	.INCLUDE "ur.vec"
	.INCLUDE "ur.inc"

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
;       1 
;       2 
;       3 
;       4 
;       5 
;       6 
;       7 
;       8 
;       9 
;      10 /*****************************************************
;      11 This program was produced by the
;      12 CodeWizardAVR V1.24.6 Standard
;      13 Automatic Program Generator
;      14 © Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;      15 http://www.hpinfotech.com
;      16 e-mail:office@hpinfotech.com
;      17 
;      18 Project : UR Zero
;      19 Version : 
;      20 Date    : 11-06-2005
;      21 Author  : Lars Chr Fergo                  
;      22 Company : Denmark                         
;      23 Comments: 
;      24 
;      25 
;      26 Chip type           : ATmega16
;      27 Program type        : Application
;      28 Clock frequency     : 8,000000 MHz
;      29 Memory model        : Small
;      30 External SRAM size  : 0
;      31 Data Stack size     : 256
;      32 *****************************************************/
;      33 
;      34 #include <mega16.h>
;      35 
;      36 // Alphanumeric LCD Module functions
;      37 #asm
;      38    .equ __lcd_port=0x15 ;PORTC
   .equ __lcd_port=0x15 ;PORTC
;      39 #endasm
;      40 #include <lcd.h>      
;      41 
;      42 #include <delay.h>     
;      43 #include <math.h>   
;      44 
;      45 
;      46 #define		B1				PINA.0  
;      47 #define		B2				PINA.1
;      48 #define		B3				PINA.2
;      49 #define		B4				PINA.3       
;      50 #define		LED				DDRB.0             
;      51 #define		BUZZ1				PORTB.4		//BUZZ OUTPUT
;      52 #define		BUZZ2				PORTB.5		//BUZZ OUTPUT  
;      53 #define		LYS				PORTB.6		//BUZZ OUTPUT
;      54 #define		ON				1
;      55 #define     	OFF				0    
;      56 
;      57 
;      58 
;      59 int t,u,h,ms,sen,sti,men,mti,ten,tti,menu,pen,pti; 
_sti:
	.BYTE 0x2
_men:
	.BYTE 0x2
_mti:
	.BYTE 0x2
_ten:
	.BYTE 0x2
_tti:
	.BYTE 0x2
_menu:
	.BYTE 0x2
_pen:
	.BYTE 0x2
_pti:
	.BYTE 0x2
;      60 int Omen,Omti,Oten,Otti,Nmen,Nmti,Nten,Ntti;           // LYS tænd, LYS sluk
_Omen:
	.BYTE 0x2
_Omti:
	.BYTE 0x2
_Oten:
	.BYTE 0x2
_Otti:
	.BYTE 0x2
_Nmen:
	.BYTE 0x2
_Nmti:
	.BYTE 0x2
_Nten:
	.BYTE 0x2
_Ntti:
	.BYTE 0x2
;      61 unsigned long  Dx,D,Den,Dti,D_ON,D_UP;                                  // LYS dæmpnings periode        
_Dx:
	.BYTE 0x4
_D:
	.BYTE 0x4
_Den:
	.BYTE 0x4
_Dti:
	.BYTE 0x4
_D_ON:
	.BYTE 0x4
_D_UP:
	.BYTE 0x4
;      62 unsigned long v,v2;
_v:
	.BYTE 0x4
_v2:
	.BYTE 0x4
;      63 
;      64 
;      65 void LCD_TIME(void)
;      66 {     

	.CSEG
_LCD_TIME:
;      67   lcd_gotoxy(8,0);
	LDI  R30,LOW(8)
	CALL SUBOPT_0x0
;      68   lcd_putsf("     ");  
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;      69   lcd_gotoxy(7,0);
	LDI  R30,LOW(7)
	CALL SUBOPT_0x0
;      70   lcd_putchar(48+sen);  
	__GETW1R 12,13
	CALL SUBOPT_0x1
;      71   lcd_gotoxy(6,0);
	LDI  R30,LOW(6)
	CALL SUBOPT_0x0
;      72   lcd_putchar(48+sti);
	LDS  R30,_sti
	LDS  R31,_sti+1
	CALL SUBOPT_0x1
;      73   lcd_gotoxy(5,0);
	LDI  R30,LOW(5)
	CALL SUBOPT_0x0
;      74   lcd_putsf(":");  
	__POINTW1FN _0,6
	CALL SUBOPT_0x2
;      75   
;      76   lcd_gotoxy(4,0);
;      77   lcd_putchar(48+men);  
	LDS  R30,_men
	LDS  R31,_men+1
	CALL SUBOPT_0x1
;      78   lcd_gotoxy(3,0);
	LDI  R30,LOW(3)
	CALL SUBOPT_0x0
;      79   lcd_putchar(48+mti);
	LDS  R30,_mti
	LDS  R31,_mti+1
	CALL SUBOPT_0x1
;      80   lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x0
;      81   lcd_putsf(":");     
	__POINTW1FN _0,6
	CALL SUBOPT_0x3
;      82   
;      83   lcd_gotoxy(1,0);
;      84   lcd_putchar(48+ten);  
	LDS  R30,_ten
	LDS  R31,_ten+1
	CALL SUBOPT_0x1
;      85   lcd_gotoxy(0,0);
	LDI  R30,LOW(0)
	CALL SUBOPT_0x0
;      86   lcd_putchar(48+tti);     
	LDS  R30,_tti
	LDS  R31,_tti+1
	CALL SUBOPT_0x1
;      87 
;      88 }          
	RET
;      89 
;      90 
;      91 void LCD_OTIME(void)
;      92 {   
_LCD_OTIME:
;      93   lcd_gotoxy(5,0);
	LDI  R30,LOW(5)
	CALL SUBOPT_0x0
;      94   lcd_putsf("       ");  
	__POINTW1FN _0,8
	CALL SUBOPT_0x2
;      95   
;      96   lcd_gotoxy(4,0);
;      97   lcd_putchar(48+Omen);  
	LDS  R30,_Omen
	LDS  R31,_Omen+1
	CALL SUBOPT_0x1
;      98   lcd_gotoxy(3,0);
	LDI  R30,LOW(3)
	CALL SUBOPT_0x0
;      99   lcd_putchar(48+Omti);
	LDS  R30,_Omti
	LDS  R31,_Omti+1
	CALL SUBOPT_0x1
;     100   lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x0
;     101   lcd_putsf(":");     
	__POINTW1FN _0,6
	CALL SUBOPT_0x3
;     102   
;     103   lcd_gotoxy(1,0);
;     104   lcd_putchar(48+Oten);  
	LDS  R30,_Oten
	LDS  R31,_Oten+1
	CALL SUBOPT_0x1
;     105   lcd_gotoxy(0,0);
	LDI  R30,LOW(0)
	CALL SUBOPT_0x0
;     106   lcd_putchar(48+Otti);     
	LDS  R30,_Otti
	LDS  R31,_Otti+1
	CALL SUBOPT_0x1
;     107 
;     108 }      
	RET
;     109 
;     110 void LCD_NTIME(void)
;     111 {
_LCD_NTIME:
;     112   
;     113   lcd_gotoxy(5,0);
	LDI  R30,LOW(5)
	CALL SUBOPT_0x0
;     114   lcd_putsf("       ");  
	__POINTW1FN _0,8
	CALL SUBOPT_0x2
;     115   
;     116   lcd_gotoxy(4,0);
;     117   lcd_putchar(48+Nmen);  
	LDS  R30,_Nmen
	LDS  R31,_Nmen+1
	CALL SUBOPT_0x1
;     118   lcd_gotoxy(3,0);
	LDI  R30,LOW(3)
	CALL SUBOPT_0x0
;     119   lcd_putchar(48+Nmti);
	LDS  R30,_Nmti
	LDS  R31,_Nmti+1
	CALL SUBOPT_0x1
;     120   lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x0
;     121   lcd_putsf(":");     
	__POINTW1FN _0,6
	CALL SUBOPT_0x3
;     122   
;     123   lcd_gotoxy(1,0);
;     124   lcd_putchar(48+Nten);  
	LDS  R30,_Nten
	LDS  R31,_Nten+1
	CALL SUBOPT_0x1
;     125   lcd_gotoxy(0,0);
	LDI  R30,LOW(0)
	CALL SUBOPT_0x0
;     126   lcd_putchar(48+Ntti);     
	LDS  R30,_Ntti
	LDS  R31,_Ntti+1
	CALL SUBOPT_0x1
;     127 
;     128 }
	RET
;     129 
;     130 // External Interrupt 2 service routine
;     131 interrupt [EXT_INT2] void ext_int2_isr(void)
;     132 {    
_ext_int2_isr:
	ST   -Y,R0
	ST   -Y,R1
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
;     133  if(D_ON==1)
	LDS  R26,_D_ON
	LDS  R27,_D_ON+1
	LDS  R24,_D_ON+2
	LDS  R25,_D_ON+3
	__CPD2N 0x1
	BREQ PC+3
	JMP _0x3
;     134 {   
;     135  LYS =0; 
	CBI  0x18,6
;     136  ++Dx; 
	LDS  R30,_Dx
	LDS  R31,_Dx+1
	LDS  R22,_Dx+2
	LDS  R23,_Dx+3
	__SUBD1N -1
	STS  _Dx,R30
	STS  _Dx+1,R31
	STS  _Dx+2,R22
	STS  _Dx+3,R23
;     137  v2=(1500/D);
	LDS  R30,_D
	LDS  R31,_D+1
	LDS  R22,_D+2
	LDS  R23,_D+3
	__GETD2N 0x5DC
	CALL __DIVD21U
	STS  _v2,R30
	STS  _v2+1,R31
	STS  _v2+2,R22
	STS  _v2+3,R23
;     138  v=v+v2;       
	LDS  R26,_v
	LDS  R27,_v+1
	LDS  R24,_v+2
	LDS  R25,_v+3
	CALL __ADDD12
	STS  _v,R30
	STS  _v+1,R31
	STS  _v+2,R22
	STS  _v+3,R23
;     139  
;     140   for(h=0;h<v;)
	CLR  R8
	CLR  R9
_0x5:
	LDS  R30,_v
	LDS  R31,_v+1
	LDS  R22,_v+2
	LDS  R23,_v+3
	__GETW2R 8,9
	CALL __CWD2
	CALL __CPD21
	BRGE _0x6
;     141  {        
;     142      #asm("nop");      
	nop
;     143     ++h;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 8,9,30,31
;     144  }  
	RJMP _0x5
_0x6:
;     145  h=0;
	CLR  R8
	CLR  R9
;     146  LYS = 1;    
	SBI  0x18,6
;     147  
;     148     
;     149 
;     150  
;     151  if(Dx==D) 
	LDS  R30,_D
	LDS  R31,_D+1
	LDS  R22,_D+2
	LDS  R23,_D+3
	LDS  R26,_Dx
	LDS  R27,_Dx+1
	LDS  R24,_Dx+2
	LDS  R25,_Dx+3
	CALL __CPD12
	BRNE _0x7
;     152  {LYS=0;
	CBI  0x18,6
;     153   D_ON=0;
	LDI  R30,0
	STS  _D_ON,R30
	STS  _D_ON+1,R30
	STS  _D_ON+2,R30
	STS  _D_ON+3,R30
;     154   Dx=0;
	LDI  R30,0
	STS  _Dx,R30
	STS  _Dx+1,R30
	STS  _Dx+2,R30
	STS  _Dx+3,R30
;     155  }
;     156  }   
_0x7:
;     157 
;     158      
;     159 
;     160 ++ms; 
_0x3:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 10,11,30,31
;     161 if(ms==100)
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	CP   R30,R10
	CPC  R31,R11
	BREQ PC+3
	JMP _0x8
;     162 {       
;     163 ms=0;
	CLR  R10
	CLR  R11
;     164 ++sen;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 12,13,30,31
;     165 if(sen==10) 
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CP   R30,R12
	CPC  R31,R13
	BRNE _0x9
;     166 {sen=0;  
	CLR  R12
	CLR  R13
;     167 ++sti;
	LDS  R30,_sti
	LDS  R31,_sti+1
	ADIW R30,1
	STS  _sti,R30
	STS  _sti+1,R31
;     168 }
;     169 if(sti==6) 
_0x9:
	LDS  R26,_sti
	LDS  R27,_sti+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0xA
;     170 {sti=0;  
	LDI  R30,0
	STS  _sti,R30
	STS  _sti+1,R30
;     171 ++men;
	CALL SUBOPT_0x4
;     172 }
;     173 if(men==10) 
_0xA:
	LDS  R26,_men
	LDS  R27,_men+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0xB
;     174 {men=0;  
	CALL SUBOPT_0x5
;     175 ++mti;
;     176 }
;     177 if(mti==6) 
_0xB:
	LDS  R26,_mti
	LDS  R27,_mti+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0xC
;     178 {mti=0;  
	LDI  R30,0
	STS  _mti,R30
	STS  _mti+1,R30
;     179 ++ten;
	CALL SUBOPT_0x6
;     180 }
;     181 if(ten==10) 
_0xC:
	LDS  R26,_ten
	LDS  R27,_ten+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0xD
;     182 {ten=0;  
	CALL SUBOPT_0x7
;     183 ++tti;      
;     184 }
;     185 if(tti==2 && ten==4) 
_0xD:
	LDS  R26,_tti
	LDS  R27,_tti+1
	CPI  R26,LOW(0x2)
	LDI  R30,HIGH(0x2)
	CPC  R27,R30
	BRNE _0xF
	LDS  R26,_ten
	LDS  R27,_ten+1
	CPI  R26,LOW(0x4)
	LDI  R30,HIGH(0x4)
	CPC  R27,R30
	BREQ _0x10
_0xF:
	RJMP _0xE
_0x10:
;     186 {ten=0;  
	LDI  R30,0
	STS  _ten,R30
	STS  _ten+1,R30
;     187 tti=0;
	LDI  R30,0
	STS  _tti,R30
	STS  _tti+1,R30
;     188 }   
;     189 //Dx=Dx + (7700/(D*60));
;     190 }
_0xE:
;     191 
;     192 }       
_0x8:
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
	LD   R1,Y+
	LD   R0,Y+
	RETI
;     193 
;     194 
;     195 
;     196 // Declare your global variables here
;     197 
;     198 void main(void)
;     199 {
_main:
;     200 // Declare your local variables here
;     201 
;     202 // Input/Output Ports initialization
;     203 // Port A initialization
;     204 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     205 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     206 PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     207 DDRA=0x00;
	OUT  0x1A,R30
;     208 
;     209 // Port B initialization
;     210 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     211 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     212 PORTB=0x00;
	OUT  0x18,R30
;     213 DDRB=0x40;
	LDI  R30,LOW(64)
	OUT  0x17,R30
;     214 
;     215 // Port C initialization
;     216 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     217 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     218 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;     219 DDRC=0x00;
	OUT  0x14,R30
;     220 
;     221 // Port D initialization
;     222 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     223 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     224 PORTD=0x00;
	OUT  0x12,R30
;     225 DDRD=0x00;
	OUT  0x11,R30
;     226 
;     227 // Timer/Counter 0 initialization
;     228 // Clock source: System Clock
;     229 // Clock value: Timer 0 Stopped
;     230 // Mode: Normal top=FFh
;     231 // OC0 output: Disconnected
;     232 TCCR0=0x00;
	OUT  0x33,R30
;     233 TCNT0=0x00;
	OUT  0x32,R30
;     234 OCR0=0x00;
	OUT  0x3C,R30
;     235 
;     236 // Timer/Counter 1 initialization
;     237 // Clock source: System Clock
;     238 // Clock value: Timer 1 Stopped
;     239 // Mode: Normal top=FFFFh
;     240 // OC1A output: Discon.
;     241 // OC1B output: Discon.
;     242 // Noise Canceler: Off
;     243 // Input Capture on Falling Edge
;     244 // Timer 1 Overflow Interrupt: Off
;     245 // Input Capture Interrupt: Off
;     246 // Compare A Match Interrupt: Off
;     247 // Compare B Match Interrupt: Off
;     248 TCCR1A=0x00;
	OUT  0x2F,R30
;     249 TCCR1B=0x00;
	OUT  0x2E,R30
;     250 TCNT1H=0x00;
	OUT  0x2D,R30
;     251 TCNT1L=0x00;
	OUT  0x2C,R30
;     252 ICR1H=0x00;
	OUT  0x27,R30
;     253 ICR1L=0x00;
	OUT  0x26,R30
;     254 OCR1AH=0x00;
	OUT  0x2B,R30
;     255 OCR1AL=0x00;
	OUT  0x2A,R30
;     256 OCR1BH=0x00;
	OUT  0x29,R30
;     257 OCR1BL=0x00;
	OUT  0x28,R30
;     258 
;     259 // Timer/Counter 2 initialization
;     260 // Clock source: System Clock
;     261 // Clock value: Timer 2 Stopped
;     262 // Mode: Normal top=FFh
;     263 // OC2 output: Disconnected
;     264 ASSR=0x00;
	OUT  0x22,R30
;     265 TCCR2=0x00;
	OUT  0x25,R30
;     266 TCNT2=0x00;
	OUT  0x24,R30
;     267 OCR2=0x00;
	OUT  0x23,R30
;     268 
;     269 // External Interrupt(s) initialization
;     270 // INT0: Off
;     271 // INT1: Off
;     272 // INT2: On
;     273 // INT2 Mode: Rising Edge
;     274 GICR|=0x20;
	IN   R30,0x3B
	ORI  R30,0x20
	OUT  0x3B,R30
;     275 MCUCR=0x00;
	LDI  R30,LOW(0)
	OUT  0x35,R30
;     276 MCUCSR=0x40;
	LDI  R30,LOW(64)
	OUT  0x34,R30
;     277 GIFR=0x20;
	LDI  R30,LOW(32)
	OUT  0x3A,R30
;     278 
;     279 // Timer(s)/Counter(s) Interrupt(s) initialization
;     280 TIMSK=0x00;
	LDI  R30,LOW(0)
	OUT  0x39,R30
;     281 
;     282 // Analog Comparator initialization
;     283 // Analog Comparator: Off
;     284 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     285 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     286 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     287 
;     288 // LCD module initialization
;     289 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     290 
;     291 // Global enable interrupts
;     292 #asm("sei")    
	sei
;     293 delay_ms(100);
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     294 
;     295 while (1)
_0x11:
;     296       {    
;     297  
;     298  // v=1500;        // LYSSTYRKE 7700 = min, 1 = max
;     299   if(B1==0)
	SBIC 0x19,0
	RJMP _0x14
;     300   { 
;     301   ++menu;
	LDS  R30,_menu
	LDS  R31,_menu+1
	ADIW R30,1
	STS  _menu,R30
	STS  _menu+1,R31
;     302   while(B1==0)
_0x15:
	SBIC 0x19,0
	RJMP _0x17
;     303   delay_ms(10);
	CALL SUBOPT_0x8
;     304   }     
	RJMP _0x15
_0x17:
;     305   
;     306 
;     307   
;     308   if(menu==6) menu=0;         
_0x14:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0x18
	LDI  R30,0
	STS  _menu,R30
	STS  _menu+1,R30
;     309   
;     310               //-----------------------menu 1 start
;     311   
;     312   if(menu==1) 
_0x18:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BREQ PC+3
	JMP _0x19
;     313   {    
;     314   
;     315    if(B2==0)
	SBIC 0x19,1
	RJMP _0x1A
;     316   {      
;     317   ++men;
	CALL SUBOPT_0x4
;     318   sen=0;
	CLR  R12
	CLR  R13
;     319   sti=0;     
	LDI  R30,0
	STS  _sti,R30
	STS  _sti+1,R30
;     320   while(B2==0)
_0x1B:
	SBIC 0x19,1
	RJMP _0x1D
;     321   delay_ms(10);
	CALL SUBOPT_0x8
;     322   }  
	RJMP _0x1B
_0x1D:
;     323   if(men==10) 
_0x1A:
	LDS  R26,_men
	LDS  R27,_men+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x1E
;     324   {men=0;  
	CALL SUBOPT_0x5
;     325   ++mti;
;     326   if(mti==6) mti=0;  
	LDS  R26,_mti
	LDS  R27,_mti+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0x1F
	LDI  R30,0
	STS  _mti,R30
	STS  _mti+1,R30
;     327   }           
_0x1F:
;     328   
;     329    if(B3==0)
_0x1E:
	SBIC 0x19,2
	RJMP _0x20
;     330   {      
;     331   ++ten;   
	CALL SUBOPT_0x6
;     332   sen=0;
	CLR  R12
	CLR  R13
;     333   sti=0;
	LDI  R30,0
	STS  _sti,R30
	STS  _sti+1,R30
;     334   while(B3==0)
_0x21:
	SBIC 0x19,2
	RJMP _0x23
;     335   delay_ms(10);
	CALL SUBOPT_0x8
;     336   }  
	RJMP _0x21
_0x23:
;     337 
;     338   if(ten==10) 
_0x20:
	LDS  R26,_ten
	LDS  R27,_ten+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x24
;     339   {ten=0;  
	CALL SUBOPT_0x7
;     340   ++tti; 
;     341   }
;     342   if(tti==2 && ten==4) 
_0x24:
	LDS  R26,_tti
	LDS  R27,_tti+1
	CPI  R26,LOW(0x2)
	LDI  R30,HIGH(0x2)
	CPC  R27,R30
	BRNE _0x26
	LDS  R26,_ten
	LDS  R27,_ten+1
	CPI  R26,LOW(0x4)
	LDI  R30,HIGH(0x4)
	CPC  R27,R30
	BREQ _0x27
_0x26:
	RJMP _0x25
_0x27:
;     343   {ten=0;  
	LDI  R30,0
	STS  _ten,R30
	STS  _ten+1,R30
;     344    tti=0;  
	LDI  R30,0
	STS  _tti,R30
	STS  _tti+1,R30
;     345    }
;     346   LCD_TIME();
_0x25:
	CALL SUBOPT_0x9
;     347   lcd_gotoxy(0,1);
;     348   lcd_putsf("SET TIME");      
	__POINTW1FN _0,16
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     349   }
;     350                //-----------------------menu 1 slut
;     351               //-----------------------menu 2 start
;     352   
;     353   if(menu==2) 
_0x19:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x2)
	LDI  R30,HIGH(0x2)
	CPC  R27,R30
	BREQ PC+3
	JMP _0x28
;     354   {    
;     355    if(B2==0)
	SBIC 0x19,1
	RJMP _0x29
;     356   {      
;     357   ++Omen;
	LDS  R30,_Omen
	LDS  R31,_Omen+1
	ADIW R30,1
	STS  _Omen,R30
	STS  _Omen+1,R31
;     358   while(B2==0)
_0x2A:
	SBIC 0x19,1
	RJMP _0x2C
;     359   delay_ms(10);
	CALL SUBOPT_0x8
;     360   }  
	RJMP _0x2A
_0x2C:
;     361   if(Omen==10) 
_0x29:
	LDS  R26,_Omen
	LDS  R27,_Omen+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x2D
;     362   {Omen=0;  
	LDI  R30,0
	STS  _Omen,R30
	STS  _Omen+1,R30
;     363   ++Omti;
	LDS  R30,_Omti
	LDS  R31,_Omti+1
	ADIW R30,1
	STS  _Omti,R30
	STS  _Omti+1,R31
;     364   if(Omti==6) Omti=0;  
	LDS  R26,_Omti
	LDS  R27,_Omti+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0x2E
	LDI  R30,0
	STS  _Omti,R30
	STS  _Omti+1,R30
;     365   }           
_0x2E:
;     366   
;     367    if(B3==0)
_0x2D:
	SBIC 0x19,2
	RJMP _0x2F
;     368   {      
;     369   ++Oten;   
	LDS  R30,_Oten
	LDS  R31,_Oten+1
	ADIW R30,1
	STS  _Oten,R30
	STS  _Oten+1,R31
;     370   while(B3==0)
_0x30:
	SBIC 0x19,2
	RJMP _0x32
;     371   delay_ms(10);
	CALL SUBOPT_0x8
;     372   }  
	RJMP _0x30
_0x32:
;     373 
;     374   if(Oten==10) 
_0x2F:
	LDS  R26,_Oten
	LDS  R27,_Oten+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x33
;     375   {Oten=0;  
	LDI  R30,0
	STS  _Oten,R30
	STS  _Oten+1,R30
;     376   ++Otti; 
	LDS  R30,_Otti
	LDS  R31,_Otti+1
	ADIW R30,1
	STS  _Otti,R30
	STS  _Otti+1,R31
;     377   }
;     378   if(Otti==2 && Oten==4) 
_0x33:
	LDS  R26,_Otti
	LDS  R27,_Otti+1
	CPI  R26,LOW(0x2)
	LDI  R30,HIGH(0x2)
	CPC  R27,R30
	BRNE _0x35
	LDS  R26,_Oten
	LDS  R27,_Oten+1
	CPI  R26,LOW(0x4)
	LDI  R30,HIGH(0x4)
	CPC  R27,R30
	BREQ _0x36
_0x35:
	RJMP _0x34
_0x36:
;     379   {Oten=0;  
	LDI  R30,0
	STS  _Oten,R30
	STS  _Oten+1,R30
;     380    Otti=0;  
	LDI  R30,0
	STS  _Otti,R30
	STS  _Otti+1,R30
;     381    }  
;     382 
;     383  
;     384   LCD_OTIME();
_0x34:
	CALL _LCD_OTIME
;     385   lcd_gotoxy(0,1);
	CALL SUBOPT_0xA
;     386   lcd_putsf("LIGHT ON");      
	__POINTW1FN _0,25
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     387   }
;     388                //-----------------------menu 2 slut 
;     389   
;     390               //-----------------------menu 3 start
;     391   
;     392   if(menu==3) 
_0x28:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x3)
	LDI  R30,HIGH(0x3)
	CPC  R27,R30
	BREQ PC+3
	JMP _0x37
;     393   {    
;     394     if(B2==0)
	SBIC 0x19,1
	RJMP _0x38
;     395   {      
;     396   ++Nmen;
	LDS  R30,_Nmen
	LDS  R31,_Nmen+1
	ADIW R30,1
	STS  _Nmen,R30
	STS  _Nmen+1,R31
;     397   while(B2==0)
_0x39:
	SBIC 0x19,1
	RJMP _0x3B
;     398   delay_ms(10);
	CALL SUBOPT_0x8
;     399   }  
	RJMP _0x39
_0x3B:
;     400   if(Nmen==10) 
_0x38:
	LDS  R26,_Nmen
	LDS  R27,_Nmen+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x3C
;     401   {Nmen=0;  
	LDI  R30,0
	STS  _Nmen,R30
	STS  _Nmen+1,R30
;     402   ++Nmti;
	LDS  R30,_Nmti
	LDS  R31,_Nmti+1
	ADIW R30,1
	STS  _Nmti,R30
	STS  _Nmti+1,R31
;     403   if(Omti==6) Omti=0;  
	LDS  R26,_Omti
	LDS  R27,_Omti+1
	CPI  R26,LOW(0x6)
	LDI  R30,HIGH(0x6)
	CPC  R27,R30
	BRNE _0x3D
	LDI  R30,0
	STS  _Omti,R30
	STS  _Omti+1,R30
;     404   }           
_0x3D:
;     405   
;     406    if(B3==0)
_0x3C:
	SBIC 0x19,2
	RJMP _0x3E
;     407   {      
;     408   ++Nten;   
	LDS  R30,_Nten
	LDS  R31,_Nten+1
	ADIW R30,1
	STS  _Nten,R30
	STS  _Nten+1,R31
;     409   while(B3==0)
_0x3F:
	SBIC 0x19,2
	RJMP _0x41
;     410   delay_ms(10);
	CALL SUBOPT_0x8
;     411   }  
	RJMP _0x3F
_0x41:
;     412 
;     413   if(Nten==10) 
_0x3E:
	LDS  R26,_Nten
	LDS  R27,_Nten+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRNE _0x42
;     414   {Nten=0;  
	LDI  R30,0
	STS  _Nten,R30
	STS  _Nten+1,R30
;     415   ++Ntti; 
	LDS  R30,_Ntti
	LDS  R31,_Ntti+1
	ADIW R30,1
	STS  _Ntti,R30
	STS  _Ntti+1,R31
;     416   }
;     417   if(Ntti==2 && Nten==4) 
_0x42:
	LDS  R26,_Ntti
	LDS  R27,_Ntti+1
	CPI  R26,LOW(0x2)
	LDI  R30,HIGH(0x2)
	CPC  R27,R30
	BRNE _0x44
	LDS  R26,_Nten
	LDS  R27,_Nten+1
	CPI  R26,LOW(0x4)
	LDI  R30,HIGH(0x4)
	CPC  R27,R30
	BREQ _0x45
_0x44:
	RJMP _0x43
_0x45:
;     418   {Nten=0;  
	LDI  R30,0
	STS  _Nten,R30
	STS  _Nten+1,R30
;     419    Ntti=0;  
	LDI  R30,0
	STS  _Ntti,R30
	STS  _Ntti+1,R30
;     420    }   
;     421 
;     422   LCD_NTIME();
_0x43:
	CALL _LCD_NTIME
;     423   lcd_gotoxy(0,1);
	CALL SUBOPT_0xA
;     424   lcd_putsf("LIGHT OFF");      
	__POINTW1FN _0,34
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     425   }
;     426                //-----------------------menu 3 slut 
;     427                 //-----------------------menu 4 start
;     428   
;     429   if(menu==4) 
_0x37:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x4)
	LDI  R30,HIGH(0x4)
	CPC  R27,R30
	BREQ PC+3
	JMP _0x46
;     430   {    
;     431    if(B2==0)
	SBIC 0x19,1
	RJMP _0x47
;     432   {      
;     433   ++Den;
	LDS  R30,_Den
	LDS  R31,_Den+1
	LDS  R22,_Den+2
	LDS  R23,_Den+3
	__SUBD1N -1
	STS  _Den,R30
	STS  _Den+1,R31
	STS  _Den+2,R22
	STS  _Den+3,R23
;     434   while(B2==0)
_0x48:
	SBIC 0x19,1
	RJMP _0x4A
;     435   delay_ms(10);
	CALL SUBOPT_0x8
;     436   }  
	RJMP _0x48
_0x4A:
;     437   if(Den==10) 
_0x47:
	LDS  R26,_Den
	LDS  R27,_Den+1
	LDS  R24,_Den+2
	LDS  R25,_Den+3
	__CPD2N 0xA
	BRNE _0x4B
;     438   {Den=0;  
	LDI  R30,0
	STS  _Den,R30
	STS  _Den+1,R30
	STS  _Den+2,R30
	STS  _Den+3,R30
;     439   ++Dti;  
	LDS  R30,_Dti
	LDS  R31,_Dti+1
	LDS  R22,_Dti+2
	LDS  R23,_Dti+3
	__SUBD1N -1
	STS  _Dti,R30
	STS  _Dti+1,R31
	STS  _Dti+2,R22
	STS  _Dti+3,R23
;     440   }
;     441   if(Dti==10) Dti=0;  
_0x4B:
	LDS  R26,_Dti
	LDS  R27,_Dti+1
	LDS  R24,_Dti+2
	LDS  R25,_Dti+3
	__CPD2N 0xA
	BRNE _0x4C
	LDI  R30,0
	STS  _Dti,R30
	STS  _Dti+1,R30
	STS  _Dti+2,R30
	STS  _Dti+3,R30
;     442       
;     443    if(B3==0)
_0x4C:
	SBIC 0x19,2
	RJMP _0x4D
;     444   {  
;     445   Dx=0;    
	LDI  R30,0
	STS  _Dx,R30
	STS  _Dx+1,R30
	STS  _Dx+2,R30
	STS  _Dx+3,R30
;     446   if (D_ON==0) { D_ON=1;
	LDS  R30,_D_ON
	LDS  R31,_D_ON+1
	LDS  R22,_D_ON+2
	LDS  R23,_D_ON+3
	CALL __CPD10
	BRNE _0x4E
	__GETD1N 0x1
	STS  _D_ON,R30
	STS  _D_ON+1,R31
	STS  _D_ON+2,R22
	STS  _D_ON+3,R23
;     447     }
;     448  else  {   D_ON=0; 
	RJMP _0x4F
_0x4E:
	LDI  R30,0
	STS  _D_ON,R30
	STS  _D_ON+1,R30
	STS  _D_ON+2,R30
	STS  _D_ON+3,R30
;     449     };
_0x4F:
;     450 
;     451   while(B3==0)
_0x50:
	SBIC 0x19,2
	RJMP _0x52
;     452   delay_ms(10);
	CALL SUBOPT_0x8
;     453   }   
	RJMP _0x50
_0x52:
;     454   
;     455 
;     456   
;     457   lcd_gotoxy(1,0);
_0x4D:
	LDI  R30,LOW(1)
	CALL SUBOPT_0x0
;     458   lcd_putchar(48+Den);  
	LDS  R30,_Den
	LDS  R31,_Den+1
	LDS  R22,_Den+2
	LDS  R23,_Den+3
	CALL SUBOPT_0xB
;     459   lcd_gotoxy(0,0);
	LDI  R30,LOW(0)
	CALL SUBOPT_0x0
;     460   lcd_putchar(48+Dti);      
	LDS  R30,_Dti
	LDS  R31,_Dti+1
	LDS  R22,_Dti+2
	LDS  R23,_Dti+3
	CALL SUBOPT_0xB
;     461     
;     462   if(D_ON==1)
	LDS  R26,_D_ON
	LDS  R27,_D_ON+1
	LDS  R24,_D_ON+2
	LDS  R25,_D_ON+3
	__CPD2N 0x1
	BRNE _0x53
;     463   {   
;     464   lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x0
;     465   lcd_putsf(" min    ON ");    
	__POINTW1FN _0,44
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     466   }            
;     467   
;     468     if(D_ON==0)
_0x53:
	LDS  R30,_D_ON
	LDS  R31,_D_ON+1
	LDS  R22,_D_ON+2
	LDS  R23,_D_ON+3
	CALL __CPD10
	BRNE _0x54
;     469   {   
;     470   lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x0
;     471   lcd_putsf(" min    OFF");    
	__POINTW1FN _0,56
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     472   }
;     473   D=(Dti*10+Den)*6000;  
_0x54:
	LDS  R26,_Dti
	LDS  R27,_Dti+1
	LDS  R24,_Dti+2
	LDS  R25,_Dti+3
	__GETD1N 0xA
	CALL __MULD12U
	LDS  R26,_Den
	LDS  R27,_Den+1
	LDS  R24,_Den+2
	LDS  R25,_Den+3
	CALL __ADDD21
	__GETD1N 0x1770
	CALL __MULD12U
	STS  _D,R30
	STS  _D+1,R31
	STS  _D+2,R22
	STS  _D+3,R23
;     474 //  D=(7700/(D*6))/1000;
;     475 
;     476   lcd_gotoxy(0,1);
	CALL SUBOPT_0xA
;     477   lcd_putsf("DIMMER TIME");      
	__POINTW1FN _0,68
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     478   }
;     479                //-----------------------menu 4 slut         
;     480               //-----------------------menu 5 start
;     481   
;     482   if(menu==5) 
_0x46:
	LDS  R26,_menu
	LDS  R27,_menu+1
	CPI  R26,LOW(0x5)
	LDI  R30,HIGH(0x5)
	CPC  R27,R30
	BRNE _0x55
;     483   {    
;     484   
;     485    if(B2==0)
	SBIC 0x19,1
	RJMP _0x56
;     486   {      
;     487 ++v;  
	LDS  R30,_v
	LDS  R31,_v+1
	LDS  R22,_v+2
	LDS  R23,_v+3
	__SUBD1N -1
	STS  _v,R30
	STS  _v+1,R31
	STS  _v+2,R22
	STS  _v+3,R23
;     488 
;     489   }           
;     490   
;     491    if(B3==0)
_0x56:
	SBIC 0x19,2
	RJMP _0x57
;     492   {      
;     493  --v;
	LDS  R30,_v
	LDS  R31,_v+1
	LDS  R22,_v+2
	LDS  R23,_v+3
	SBIW R30,1
	SBCI R22,0
	SBCI R23,0
	STS  _v,R30
	STS  _v+1,R31
	STS  _v+2,R22
	STS  _v+3,R23
;     494  
;     495   }  
;     496 
;     497 
;     498   LCD_TIME();
_0x57:
	CALL SUBOPT_0x9
;     499   lcd_gotoxy(0,1);
;     500   lcd_putsf("DIMMER        ");      
	__POINTW1FN _0,80
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     501   }
;     502                //-----------------------menu 5 slut               
;     503                //-----------------------menu 0 start
;     504   
;     505   if(menu==0) 
_0x55:
	LDS  R30,_menu
	LDS  R31,_menu+1
	SBIW R30,0
	BRNE _0x58
;     506   {           
;     507   LCD_TIME();
	CALL SUBOPT_0x9
;     508   lcd_gotoxy(0,1);
;     509   lcd_putsf("RUN         ");      
	__POINTW1FN _0,95
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     510   }
;     511                //-----------------------menu 0 slut 
;     512   
;     513   
;     514 
;     515 
;     516   
;     517       };
_0x58:
	RJMP _0x11
;     518 }
_0x59:
	RJMP _0x59
;     519 
;     520 
;     521 

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
__lcd_delay_G2:
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
	CALL __lcd_delay_G2
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G2:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G2
    ld    r26,y
    swap  r26
	CALL __lcd_write_nibble_G2
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G2:
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
    andi  r30,0xf0
	RET
_lcd_read_byte0_G2:
	CALL __lcd_delay_G2
	CALL __lcd_read_nibble_G2
    mov   r26,r30
	CALL __lcd_read_nibble_G2
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	CALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
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
	CALL SUBOPT_0xC
	LDI  R30,LOW(12)
	CALL SUBOPT_0xC
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	LDI  R30,LOW(0)
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
	BRSH _0x5B
	__lcd_putchar1:
	LDS  R30,__lcd_y
	SUBI R30,-LOW(1)
	STS  __lcd_y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDS  R30,__lcd_y
	ST   -Y,R30
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x5B:
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
	CALL __lcd_write_nibble_G2
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
	CALL SUBOPT_0xD
	CALL SUBOPT_0xD
	CALL SUBOPT_0xD
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	CALL SUBOPT_0xE
	LDI  R30,LOW(4)
	CALL SUBOPT_0xE
	LDI  R30,LOW(133)
	CALL SUBOPT_0xE
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x5C
	LDI  R30,LOW(0)
	RJMP _0x5D
_0x5C:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x5D:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 25 TIMES
SUBOPT_0x0:
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 14 TIMES
SUBOPT_0x1:
	ADIW R30,48
	ST   -Y,R30
	JMP  _lcd_putchar

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x2:
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(4)
	RJMP SUBOPT_0x0

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3:
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(1)
	RJMP SUBOPT_0x0

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	LDS  R30,_men
	LDS  R31,_men+1
	ADIW R30,1
	STS  _men,R30
	STS  _men+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	LDI  R30,0
	STS  _men,R30
	STS  _men+1,R30
	LDS  R30,_mti
	LDS  R31,_mti+1
	ADIW R30,1
	STS  _mti,R30
	STS  _mti+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	LDS  R30,_ten
	LDS  R31,_ten+1
	ADIW R30,1
	STS  _ten,R30
	STS  _ten+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	LDI  R30,0
	STS  _ten,R30
	STS  _ten+1,R30
	LDS  R30,_tti
	LDS  R31,_tti+1
	ADIW R30,1
	STS  _tti,R30
	STS  _tti+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 9 TIMES
SUBOPT_0x8:
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x9:
	CALL _LCD_TIME
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xA:
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	__ADDD1N 48
	ST   -Y,R30
	JMP  _lcd_putchar

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __lcd_ready

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xD:
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xE:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G2

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

__CWD2:
	MOV  R24,R27
	ADD  R24,R24
	SBC  R24,R24
	MOV  R25,R24
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

__CPD10:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
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

;END OF CODE MARKER
__END_OF_CODE:
