;CodeVisionAVR C Compiler V1.24.4 Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type           : ATmega32L
;Program type        : Application
;Clock frequency     : 8.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : float, width, precision
;(s)scanf features   : long, width
;External SRAM size  : 0
;Data Stack size     : 512 byte(s)
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

	.INCLUDE "adc.vec"
	.INCLUDE "adc.inc"

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
	LDI  R24,LOW(0x800)
	LDI  R25,HIGH(0x800)
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
	LDI  R30,LOW(0x85F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x85F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x260)
	LDI  R29,HIGH(0x260)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x260
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
;      11 Date    : 9/4/2008
;      12 Author  : Osama                           
;      13 Company : OSAMA                           
;      14 Comments: 
;      15 
;      16 
;      17 Chip type           : ATmega32L
;      18 Program type        : Application
;      19 Clock frequency     : 8.000000 MHz
;      20 Memory model        : Small
;      21 External SRAM size  : 0
;      22 Data Stack size     : 512
;      23 *****************************************************/
;      24 
;      25 #include <mega32.h>
;      26 #include <delay.h>
;      27 #include <math.h>
;      28 #include <stdio.h>
;      29 
;      30 #define ADC_VREF_TYPE 0x20
;      31 // Read the 8 most significant bits
;      32 // of the AD conversion result
;      33 unsigned char read_adc(unsigned char adc_input)
;      34 {

	.CSEG
_read_adc:
;      35 ADMUX=adc_input|ADC_VREF_TYPE;
	LD   R30,Y
	ORI  R30,0x20
	OUT  0x7,R30
;      36 // Start the AD conversion
;      37 ADCSRA|=0x40;
	SBI  0x6,6
;      38 // Wait for the AD conversion to complete
;      39 while ((ADCSRA & 0x10)==0);
_0x3:
	SBIS 0x6,4
	RJMP _0x3
;      40 ADCSRA|=0x10;
	SBI  0x6,4
;      41 return ADCH;
	IN   R30,0x5
	ADIW R28,1
	RET
;      42 }    
;      43 
;      44    
;      45 // Alphanumeric LCD Module functions
;      46 #asm
;      47    .equ __lcd_port=0x15 ;PORTC
   .equ __lcd_port=0x15 ;PORTC
;      48 #endasm
;      49 #include <lcd.h>
;      50 
;      51 // Declare your global variables here
;      52 union ctoi
;      53 {
;      54    unsigned char ch;
;      55    unsigned int intg;
;      56 }result;

	.DSEG
_result:
	.BYTE 0x2
;      57 
;      58 void main(void)
;      59 {

	.CSEG
_main:
;      60 // Declare your local variables here
;      61 char str[3]; 
;      62 float flt;
;      63 unsigned char res;
;      64 // Input/Output Ports initialization
;      65 // Port A initialization
;      66 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      67 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      68 PORTA=0x00;
	SBIW R28,7
;	str -> Y+4
;	flt -> Y+0
;	res -> R16
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;      69 DDRA=0x00;
	OUT  0x1A,R30
;      70 
;      71 // Port B initialization
;      72 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      73 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      74 PORTB=0x00;
	OUT  0x18,R30
;      75 DDRB=0xFF;
	LDI  R30,LOW(255)
	OUT  0x17,R30
;      76 
;      77 // Port C initialization
;      78 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      79 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      80 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;      81 DDRC=0x00;
	OUT  0x14,R30
;      82 
;      83 // Port D initialization
;      84 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      85 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      86 PORTD=0x00;
	OUT  0x12,R30
;      87 DDRD=0x00;
	OUT  0x11,R30
;      88 
;      89 // Timer/Counter 0 initialization
;      90 // Clock source: System Clock
;      91 // Clock value: Timer 0 Stopped
;      92 // Mode: Normal top=FFh
;      93 // OC0 output: Disconnected
;      94 TCCR0=0x00;
	OUT  0x33,R30
;      95 TCNT0=0x00;
	OUT  0x32,R30
;      96 OCR0=0x00;
	OUT  0x3C,R30
;      97 
;      98 // Timer/Counter 1 initialization
;      99 // Clock source: System Clock
;     100 // Clock value: Timer 1 Stopped
;     101 // Mode: Normal top=FFFFh
;     102 // OC1A output: Discon.
;     103 // OC1B output: Discon.
;     104 // Noise Canceler: Off
;     105 // Input Capture on Falling Edge
;     106 TCCR1A=0x00;
	OUT  0x2F,R30
;     107 TCCR1B=0x00;
	OUT  0x2E,R30
;     108 TCNT1H=0x00;
	OUT  0x2D,R30
;     109 TCNT1L=0x00;
	OUT  0x2C,R30
;     110 ICR1H=0x00;
	OUT  0x27,R30
;     111 ICR1L=0x00;
	OUT  0x26,R30
;     112 OCR1AH=0x00;
	OUT  0x2B,R30
;     113 OCR1AL=0x00;
	OUT  0x2A,R30
;     114 OCR1BH=0x00;
	OUT  0x29,R30
;     115 OCR1BL=0x00;
	OUT  0x28,R30
;     116 
;     117 // Timer/Counter 2 initialization
;     118 // Clock source: System Clock
;     119 // Clock value: Timer 2 Stopped
;     120 // Mode: Normal top=FFh
;     121 // OC2 output: Disconnected
;     122 ASSR=0x00;
	OUT  0x22,R30
;     123 TCCR2=0x00;
	OUT  0x25,R30
;     124 TCNT2=0x00;
	OUT  0x24,R30
;     125 OCR2=0x00;
	OUT  0x23,R30
;     126 
;     127 // External Interrupt(s) initialization
;     128 // INT0: Off
;     129 // INT1: Off
;     130 // INT2: Off
;     131 MCUCR=0x00;
	OUT  0x35,R30
;     132 MCUCSR=0x00;
	OUT  0x34,R30
;     133 
;     134 // Timer(s)/Counter(s) Interrupt(s) initialization
;     135 TIMSK=0x00;
	OUT  0x39,R30
;     136 
;     137 // USART initialization
;     138 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     139 // USART Receiver: Off
;     140 // USART Transmitter: On
;     141 // USART Mode: Asynchronous
;     142 // USART Baud rate: 9600
;     143 UCSRA=0x00;
	OUT  0xB,R30
;     144 UCSRB=0x08;
	LDI  R30,LOW(8)
	OUT  0xA,R30
;     145 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     146 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     147 UBRRL=0x33;
	LDI  R30,LOW(51)
	OUT  0x9,R30
;     148 
;     149 // Analog Comparator initialization
;     150 // Analog Comparator: Off
;     151 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     152 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     153 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     154 
;     155 // ADC initialization
;     156 // ADC Clock frequency: 125.000 kHz
;     157 // ADC Voltage Reference: AREF pin
;     158 // Only the 8 most significant bits of
;     159 // the AD conversion result are used
;     160 ADMUX=ADC_VREF_TYPE;
	LDI  R30,LOW(32)
	OUT  0x7,R30
;     161 ADCSRA=0x86;
	LDI  R30,LOW(134)
	OUT  0x6,R30
;     162 
;     163 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     164 ADCSRA = 0xFF;
	LDI  R30,LOW(255)
	OUT  0x6,R30
;     165 while (1)
_0x6:
;     166       {         
;     167          res = read_adc(0);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _read_adc
	MOV  R16,R30
;     168          flt = (float)res/51;                
	MOV  R30,R16
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x424C0000
	CALL __DIVF21
	__PUTD1S 0
;     169          sprintf(str,"Ch0: %2.3f V",flt);
	MOVW R30,R28
	ADIW R30,4
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	__GETD1S 4
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
;     170          printf("\n\r%s",str);
	__POINTW1FN _0,13
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,6
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,4
	CALL _printf
	ADIW R28,6
;     171          delay_ms(500); 
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     172          lcd_clear();                 
	CALL _lcd_clear
;     173          lcd_puts(str);  
	MOVW R30,R28
	ADIW R30,4
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_puts
;     174       };
	RJMP _0x6
;     175 }
	ADIW R28,7
_0x9:
	RJMP _0x9

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
__put_G3:
	put:
	LD   R26,Y
	LDD  R27,Y+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0xA
	CALL __GETW1P
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0xB
_0xA:
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _putchar
_0xB:
	ADIW R28,3
	RET
__ftoa_G3:
	SBIW R28,4
	ST   -Y,R17
	ST   -Y,R16
	LDD  R26,Y+8
	LDI  R30,LOW(5)
	CP   R30,R26
	BRSH _0xC
	STD  Y+8,R30
_0xC:
	LDI  R26,LOW(__fround_G3*2)
	LDI  R27,HIGH(__fround_G3*2)
	LDD  R30,Y+8
	LDI  R31,0
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	CALL __GETD1PF
	__GETD2S 9
	CALL __ADDF12
	__PUTD1S 9
	LDI  R16,LOW(0)
	__GETD1N 0x3F800000
	__PUTD1S 2
_0xD:
	__GETD1S 2
	__GETD2S 9
	CALL __CMPF12
	BRLO _0xF
	__GETD2S 2
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 2
	SUBI R16,-LOW(1)
	RJMP _0xD
_0xF:
	CPI  R16,0
	BRNE _0x10
	CALL SUBOPT_0x0
	LDI  R30,LOW(48)
	ST   X,R30
	RJMP _0x11
_0x10:
_0x12:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0x14
	__GETD2S 2
	CALL SUBOPT_0x1
	__PUTD1S 2
	__GETD2S 9
	CALL SUBOPT_0x2
	CALL SUBOPT_0x3
	PUSH R31
	PUSH R30
	MOV  R30,R17
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
	MOV  R30,R17
	__GETD2S 2
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __MULF12
	__GETD2S 9
	CALL SUBOPT_0x4
	RJMP _0x12
_0x14:
_0x11:
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0x15
	CALL SUBOPT_0x5
	RJMP _0xB8
_0x15:
	CALL SUBOPT_0x0
	LDI  R30,LOW(46)
	ST   X,R30
_0x16:
	LDD  R30,Y+8
	SUBI R30,LOW(1)
	STD  Y+8,R30
	SUBI R30,-LOW(1)
	BREQ _0x18
	__GETD2S 9
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 9
	CALL __CFD1
	MOV  R17,R30
	CALL SUBOPT_0x3
	PUSH R31
	PUSH R30
	MOV  R30,R17
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
	MOV  R30,R17
	__GETD2S 9
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL SUBOPT_0x4
	RJMP _0x16
_0x18:
	CALL SUBOPT_0x5
_0xB8:
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,13
	RET
__ftoe_G3:
	SBIW R28,4
	CALL __SAVELOCR3
	__GETD1N 0x3F800000
	__PUTD1S 3
	LDD  R26,Y+10
	LDI  R30,LOW(5)
	CP   R30,R26
	BRSH _0x19
	STD  Y+10,R30
_0x19:
	LDD  R16,Y+10
_0x1A:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0x1C
	CALL SUBOPT_0x6
	RJMP _0x1A
_0x1C:
	__GETD1S 11
	CALL __CPD10
	BRNE _0x1D
	LDI  R18,LOW(0)
	CALL SUBOPT_0x6
	RJMP _0x1E
_0x1D:
	LDD  R18,Y+10
	CALL SUBOPT_0x7
	BREQ PC+2
	BRCC PC+3
	JMP  _0x1F
	CALL SUBOPT_0x6
_0x20:
	CALL SUBOPT_0x7
	BRLO _0x22
	CALL SUBOPT_0x8
	SUBI R18,-LOW(1)
	RJMP _0x20
_0x22:
	RJMP _0x23
_0x1F:
_0x24:
	CALL SUBOPT_0x7
	BRSH _0x26
	__GETD2S 11
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 11
	SUBI R18,LOW(1)
	RJMP _0x24
_0x26:
	CALL SUBOPT_0x6
_0x23:
	__GETD1S 11
	__GETD2N 0x3F000000
	CALL __ADDF12
	__PUTD1S 11
	CALL SUBOPT_0x7
	BRLO _0x27
	CALL SUBOPT_0x8
	SUBI R18,-LOW(1)
_0x27:
_0x1E:
	LDI  R16,LOW(0)
_0x28:
	LDD  R30,Y+10
	CP   R30,R16
	BRLO _0x2A
	__GETD2S 3
	CALL SUBOPT_0x1
	__PUTD1S 3
	__GETD2S 11
	CALL SUBOPT_0x2
	CALL SUBOPT_0x9
	PUSH R31
	PUSH R30
	MOV  R30,R17
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
	MOV  R30,R17
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __CDF1
	__GETD2S 3
	CALL __MULF12
	__GETD2S 11
	CALL __SWAPD12
	CALL __SUBF12
	__PUTD1S 11
	MOV  R30,R16
	SUBI R16,-1
	CPI  R30,0
	BRNE _0x28
	CALL SUBOPT_0xA
	LDI  R30,LOW(46)
	ST   X,R30
	RJMP _0x28
_0x2A:
	CALL SUBOPT_0x9
	LDD  R26,Y+9
	STD  Z+0,R26
	CPI  R18,0
	BRGE _0x2C
	CALL SUBOPT_0xA
	LDI  R30,LOW(45)
	ST   X,R30
	NEG  R18
_0x2C:
	LDI  R30,LOW(9)
	CP   R30,R18
	BRGE _0x2D
	CALL SUBOPT_0x9
	PUSH R31
	PUSH R30
	MOV  R26,R18
	LDI  R30,LOW(10)
	CALL __DIVB21
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
_0x2D:
	CALL SUBOPT_0x9
	PUSH R31
	PUSH R30
	MOV  R26,R18
	LDI  R30,LOW(10)
	CALL __MODB21
	SUBI R30,-LOW(48)
	POP  R26
	POP  R27
	ST   X,R30
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	LDI  R30,LOW(0)
	ST   X,R30
	CALL __LOADLOCR3
	ADIW R28,15
	RET
__print_sign_G3:
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
__print_G3:
	SBIW R28,28
	CALL __SAVELOCR6
	LDI  R16,0
_0x2E:
	LDD  R30,Y+38
	LDD  R31,Y+38+1
	ADIW R30,1
	STD  Y+38,R30
	STD  Y+38+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+3
	JMP _0x30
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x34
	CPI  R19,37
	BRNE _0x35
	LDI  R16,LOW(1)
	RJMP _0x36
_0x35:
	CALL SUBOPT_0xB
_0x36:
	RJMP _0x33
_0x34:
	CPI  R30,LOW(0x1)
	BRNE _0x37
	CPI  R19,37
	BRNE _0x38
	CALL SUBOPT_0xB
	LDI  R16,LOW(0)
	RJMP _0x33
_0x38:
	LDI  R16,LOW(2)
	LDI  R30,LOW(0)
	STD  Y+17,R30
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x39
	LDI  R17,LOW(1)
	RJMP _0x33
_0x39:
	CPI  R19,43
	BRNE _0x3A
	LDI  R30,LOW(43)
	STD  Y+17,R30
	RJMP _0x33
_0x3A:
	CPI  R19,32
	BRNE _0x3B
	LDI  R30,LOW(32)
	STD  Y+17,R30
	RJMP _0x33
_0x3B:
	RJMP _0x3C
_0x37:
	CPI  R30,LOW(0x2)
	BRNE _0x3D
_0x3C:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x3E
	ORI  R17,LOW(128)
	RJMP _0x33
_0x3E:
	RJMP _0x3F
_0x3D:
	CPI  R30,LOW(0x3)
	BRNE _0x40
_0x3F:
	CPI  R19,48
	BRLO _0x42
	CPI  R19,58
	BRLO _0x43
_0x42:
	RJMP _0x41
_0x43:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x33
_0x41:
	LDI  R21,LOW(0)
	CPI  R19,46
	BRNE _0x44
	LDI  R16,LOW(4)
	RJMP _0x33
_0x44:
	RJMP _0x45
_0x40:
	CPI  R30,LOW(0x4)
	BRNE _0x47
	CPI  R19,48
	BRLO _0x49
	CPI  R19,58
	BRLO _0x4A
_0x49:
	RJMP _0x48
_0x4A:
	ORI  R17,LOW(32)
	MOV  R26,R21
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R21,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R21,R30
	RJMP _0x33
_0x48:
_0x45:
	CPI  R19,108
	BRNE _0x4B
	ORI  R17,LOW(2)
	LDI  R16,LOW(5)
	RJMP _0x33
_0x4B:
	RJMP _0x4C
_0x47:
	CPI  R30,LOW(0x5)
	BREQ PC+3
	JMP _0x33
_0x4C:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x51
	CALL SUBOPT_0xC
	LD   R30,X
	CALL SUBOPT_0xD
	RJMP _0x52
_0x51:
	CPI  R30,LOW(0x45)
	BREQ _0x55
	CPI  R30,LOW(0x65)
	BRNE _0x56
_0x55:
	RJMP _0x57
_0x56:
	CPI  R30,LOW(0x66)
	BREQ PC+3
	JMP _0x58
_0x57:
	MOVW R30,R28
	ADIW R30,18
	STD  Y+6,R30
	STD  Y+6+1,R31
	CALL SUBOPT_0xC
	CALL __GETD1P
	__PUTD1S 8
	MOVW R26,R30
	MOVW R24,R22
	CALL __CPD20
	BRLT _0x59
	LDD  R26,Y+17
	CPI  R26,LOW(0x2B)
	BREQ _0x5B
	RJMP _0x5C
_0x59:
	__GETD1S 8
	CALL __ANEGF1
	CALL SUBOPT_0xE
_0x5B:
	SBRS R17,7
	RJMP _0x5D
	LDD  R30,Y+17
	CALL SUBOPT_0xD
	RJMP _0x5E
_0x5D:
	CALL SUBOPT_0x3
	LDD  R26,Y+17
	STD  Z+0,R26
_0x5E:
_0x5C:
	SBRS R17,5
	LDI  R21,LOW(5)
	CPI  R19,102
	BRNE _0x60
	CALL SUBOPT_0xF
	LDD  R30,Y+11
	LDD  R31,Y+11+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __ftoa_G3
	RJMP _0x61
_0x60:
	CALL SUBOPT_0xF
	ST   -Y,R19
	LDD  R30,Y+12
	LDD  R31,Y+12+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __ftoe_G3
_0x61:
	MOVW R30,R28
	ADIW R30,18
	CALL SUBOPT_0x10
	RJMP _0x62
_0x58:
	CPI  R30,LOW(0x73)
	BRNE _0x64
	CALL SUBOPT_0xC
	CALL __GETW1P
	CALL SUBOPT_0x10
	RJMP _0x65
_0x64:
	CPI  R30,LOW(0x70)
	BRNE _0x67
	CALL SUBOPT_0xC
	CALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x65:
	ANDI R17,LOW(127)
	CPI  R21,0
	BREQ _0x69
	CP   R21,R16
	BRLO _0x6A
_0x69:
	RJMP _0x68
_0x6A:
	MOV  R16,R21
_0x68:
_0x62:
	LDI  R21,LOW(0)
	LDI  R30,LOW(0)
	STD  Y+16,R30
	LDI  R18,LOW(0)
	RJMP _0x6B
_0x67:
	CPI  R30,LOW(0x64)
	BREQ _0x6E
	CPI  R30,LOW(0x69)
	BRNE _0x6F
_0x6E:
	ORI  R17,LOW(4)
	RJMP _0x70
_0x6F:
	CPI  R30,LOW(0x75)
	BRNE _0x71
_0x70:
	LDI  R30,LOW(10)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x72
	__GETD1N 0x3B9ACA00
	__PUTD1S 12
	LDI  R16,LOW(10)
	RJMP _0x73
_0x72:
	__GETD1N 0x2710
	__PUTD1S 12
	LDI  R16,LOW(5)
	RJMP _0x73
_0x71:
	CPI  R30,LOW(0x58)
	BRNE _0x75
	ORI  R17,LOW(8)
	RJMP _0x76
_0x75:
	CPI  R30,LOW(0x78)
	BREQ PC+3
	JMP _0xB2
_0x76:
	LDI  R30,LOW(16)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x78
	__GETD1N 0x10000000
	__PUTD1S 12
	LDI  R16,LOW(8)
	RJMP _0x73
_0x78:
	__GETD1N 0x1000
	__PUTD1S 12
	LDI  R16,LOW(4)
_0x73:
	CPI  R21,0
	BREQ _0x79
	ANDI R17,LOW(127)
	RJMP _0x7A
_0x79:
	LDI  R21,LOW(1)
_0x7A:
	SBRS R17,1
	RJMP _0x7B
	CALL SUBOPT_0xC
	CALL __GETD1P
	__PUTD1S 8
	RJMP _0x7C
_0x7B:
	SBRS R17,2
	RJMP _0x7D
	CALL SUBOPT_0xC
	CALL __GETW1P
	CALL __CWD1
	RJMP _0xB9
_0x7D:
	CALL SUBOPT_0xC
	CALL __GETW1P
	CLR  R22
	CLR  R23
_0xB9:
	__PUTD1S 8
_0x7C:
	SBRS R17,2
	RJMP _0x7F
	__GETD2S 8
	CALL __CPD20
	BRGE _0x80
	__GETD1S 8
	CALL __ANEGD1
	CALL SUBOPT_0xE
_0x80:
	LDD  R30,Y+17
	CPI  R30,0
	BREQ _0x81
	SUBI R16,-LOW(1)
	SUBI R21,-LOW(1)
	RJMP _0x82
_0x81:
	ANDI R17,LOW(251)
_0x82:
_0x7F:
	MOV  R18,R21
_0x6B:
	SBRC R17,0
	RJMP _0x83
_0x84:
	CP   R16,R20
	BRSH _0x87
	CP   R18,R20
	BRLO _0x88
_0x87:
	RJMP _0x86
_0x88:
	SBRS R17,7
	RJMP _0x89
	SBRS R17,2
	RJMP _0x8A
	ANDI R17,LOW(251)
	LDD  R19,Y+17
	SUBI R16,LOW(1)
	RJMP _0x8B
_0x8A:
	LDI  R19,LOW(48)
_0x8B:
	RJMP _0x8C
_0x89:
	LDI  R19,LOW(32)
_0x8C:
	CALL SUBOPT_0xB
	SUBI R20,LOW(1)
	RJMP _0x84
_0x86:
_0x83:
_0x8D:
	CP   R16,R21
	BRSH _0x8F
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0x90
	CALL __print_sign_G3
	SUBI R16,LOW(1)
	SUBI R21,LOW(1)
_0x90:
	LDI  R30,LOW(48)
	CALL SUBOPT_0xD
	CPI  R20,0
	BREQ _0x91
	SUBI R20,LOW(1)
_0x91:
	SUBI R21,LOW(1)
	RJMP _0x8D
_0x8F:
	MOV  R18,R16
	LDD  R30,Y+16
	CPI  R30,0
	BRNE _0x92
_0x93:
	CPI  R18,0
	BREQ _0x95
	SBRS R17,3
	RJMP _0x96
	CALL SUBOPT_0x3
	LPM  R30,Z
	RJMP _0xBA
_0x96:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xBA:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	CPI  R20,0
	BREQ _0x98
	SUBI R20,LOW(1)
_0x98:
	SUBI R18,LOW(1)
	RJMP _0x93
_0x95:
	RJMP _0x99
_0x92:
_0x9B:
	__GETD1S 12
	__GETD2S 8
	CALL __DIVD21U
	MOV  R19,R30
	LDI  R30,LOW(9)
	CP   R30,R19
	BRSH _0x9D
	SBRS R17,3
	RJMP _0x9E
	SUBI R19,-LOW(55)
	RJMP _0x9F
_0x9E:
	SUBI R19,-LOW(87)
_0x9F:
	RJMP _0xA0
_0x9D:
	SUBI R19,-LOW(48)
_0xA0:
	SBRC R17,4
	RJMP _0xA2
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0xA4
	__GETD2S 12
	__CPD2N 0x1
	BRNE _0xA3
_0xA4:
	RJMP _0xA6
_0xA3:
	CP   R21,R18
	BRLO _0xA7
	LDI  R19,LOW(48)
	RJMP _0xA6
_0xA7:
	CP   R20,R18
	BRLO _0xA9
	SBRS R17,0
	RJMP _0xAA
_0xA9:
	RJMP _0xA8
_0xAA:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0xAB
	LDI  R19,LOW(48)
_0xA6:
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0xAC
	CALL __print_sign_G3
_0xAC:
_0xAB:
_0xA2:
	CALL SUBOPT_0xB
	CPI  R20,0
	BREQ _0xAD
	SUBI R20,LOW(1)
_0xAD:
_0xA8:
	SUBI R18,LOW(1)
	__GETD1S 12
	__GETD2S 8
	CALL __MODD21U
	__PUTD1S 8
	LDD  R30,Y+16
	__GETD2S 12
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __DIVD21U
	__PUTD1S 12
	CALL __CPD10
	BREQ _0x9C
	RJMP _0x9B
_0x9C:
_0x99:
	SBRS R17,0
	RJMP _0xAE
_0xAF:
	CPI  R20,0
	BREQ _0xB1
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	CALL SUBOPT_0xD
	RJMP _0xAF
_0xB1:
_0xAE:
_0xB2:
_0x52:
	LDI  R16,LOW(0)
_0x33:
	RJMP _0x2E
_0x30:
	CALL __LOADLOCR6
	ADIW R28,40
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	CALL SUBOPT_0x11
	MOVW R26,R28
	ADIW R26,6
	CALL __ADDW2R15
	CALL __GETW1P
	STD  Y+2,R30
	STD  Y+2+1,R31
	CALL SUBOPT_0x12
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	RJMP _0xB7
_printf:
	PUSH R15
	MOV  R15,R24
	CALL SUBOPT_0x11
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
	CALL SUBOPT_0x12
_0xB7:
	LDD  R17,Y+1
	LDD  R16,Y+0
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
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
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
_lcd_read_byte0_G4:
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
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G4)
	SBCI R31,HIGH(-__base_y_G4)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	ST   -Y,R30
	CALL __lcd_write_data
	LDD  R4,Y+1
	LDD  R5,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R30,LOW(2)
	CALL SUBOPT_0x13
	LDI  R30,LOW(12)
	CALL SUBOPT_0x13
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	LDI  R30,LOW(0)
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
	BRSH _0xB4
	__lcd_putchar1:
	INC  R5
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R5
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0xB4:
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
    rcall __lcd_write_nibble      ;RD=0, write MSN
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
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
	CALL SUBOPT_0x14
	CALL SUBOPT_0x14
	CALL SUBOPT_0x14
	CALL __long_delay_G4
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G4
	CALL __long_delay_G4
	LDI  R30,LOW(40)
	CALL SUBOPT_0x15
	LDI  R30,LOW(4)
	CALL SUBOPT_0x15
	LDI  R30,LOW(133)
	CALL SUBOPT_0x15
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G4
	CPI  R30,LOW(0x5)
	BREQ _0xB5
	LDI  R30,LOW(0)
	RJMP _0xB6
_0xB5:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0xB6:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	__GETD1N 0x41200000
	CALL __DIVF21
	__GETD2N 0x3F000000
	CALL __ADDF12
	CALL __PUTPARD1
	JMP  _floor

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	CALL __DIVF21
	CALL __CFD1
	MOV  R17,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x3:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	CALL __SWAPD12
	CALL __SUBF12
	__PUTD1S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(0)
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x6:
	__GETD2S 3
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x7:
	__GETD1S 3
	__GETD2S 11
	CALL __CMPF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	__GETD2S 11
	__GETD1N 0x41200000
	CALL __DIVF21
	__PUTD1S 11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x9:
	LDD  R30,Y+7
	LDD  R31,Y+7+1
	ADIW R30,1
	STD  Y+7,R30
	STD  Y+7+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	ADIW R26,1
	STD  Y+7,R26
	STD  Y+7+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xB:
	ST   -Y,R19
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G3

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0xC:
	LDD  R26,Y+36
	LDD  R27,Y+36+1
	SBIW R26,4
	STD  Y+36,R26
	STD  Y+36+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0xD:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G3

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	__PUTD1S 8
	LDI  R30,LOW(45)
	STD  Y+17,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	__GETD1S 8
	CALL __PUTPARD1
	ST   -Y,R21
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlen
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	__PUTW2R 16,17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
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
	JMP  __print_G3

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __lcd_ready

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x14:
	CALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G4

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x15:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G4

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
