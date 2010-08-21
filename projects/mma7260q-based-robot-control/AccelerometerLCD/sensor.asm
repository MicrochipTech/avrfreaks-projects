;CodeVisionAVR C Compiler V1.24.6 Professional
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : ATmega32
;Program type           : Application
;Clock frequency        : 16.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 512 byte(s)
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

	.INCLUDE "sensor.vec"
	.INCLUDE "sensor.inc"

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
;       1 //Created by: Sagun Man Singh
;       2 //Taking full advantage of May Day, 2010 (Maoist's Indefinite Strike) to do hardcore coding!
;       3 //Freescale MMA7260Q 3-Axis Accelerometer ported to ATmega32 microcontroller
;       4 //Started on: Thursday, May 6, 2010
;       5 //Update: LCD interface at PortC [Started: Friday, May 7, 2010; 7:00 PM]
;       6 
;       7 #include <mega32.h>
;       8 #include <stdio.h>
;       9 #include <stdlib.h>
;      10 
;      11 #asm(".equ __lcd_port=0x15")
	.equ __lcd_port=0x15
;      12 #include <lcd.h> 
;      13 
;      14 #define LCDwidth 16 
;      15 char lcd_buffer[17];
_lcd_buffer:
	.BYTE 0x11
;      16 char adc_itoa[7];
_adc_itoa:
	.BYTE 0x7
;      17 
;      18 unsigned short oldADCpin, ADCpin; 
;      19 unsigned int ADCtemp;
;      20 unsigned int ADCarray[3];
_ADCarray:
	.BYTE 0x6
;      21 unsigned int xyzOrigin[3];
_xyzOrigin:
	.BYTE 0x6
;      22 char xyzSpeed[3];
_xyzSpeed:
	.BYTE 0x3
;      23 
;      24 unsigned char xyzsampledflag;
;      25 unsigned char avgOrgflag;
;      26 unsigned char result;
;      27 
;      28                               
;      29 void initialize(void);
;      30 void displayOrientation(char val, unsigned char axis); 
;      31 
;      32 void main(void) { 

	.CSEG
_main:
;      33  
;      34         initialize();
	RCALL _initialize
;      35         
;      36         for (;;)  {     // main loop
_0x4:
;      37                 if ((ADCSRA & (1<<6)) == 0) {	// If ADC conversion has finished
	SBIC 0x6,6
	RJMP _0x6
;      38                         ADCtemp = ADCH;
	IN   R30,0x5
	LDI  R31,0
	__PUTW1R 8,9
;      39         
;      40                         if (++ADCpin > 2) {      //if all x,y,z values are sampled, return to the first x value
	__GETW1R 6,7
	ADIW R30,1
	__PUTW1R 6,7
	MOVW R26,R30
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x7
;      41                                 ADCpin = 0;
	CLR  R6
	CLR  R7
;      42                                 xyzsampledflag = 1;
	LDI  R30,LOW(1)
	MOV  R10,R30
;      43                         }
;      44                                 
;      45                         ADMUX = (1<<5)|(5 - ADCpin); 
_0x7:
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	SUB  R30,R6
	SBC  R31,R7
	ORI  R30,0x20
	OUT  0x7,R30
;      46                         ADCSRA |= (1<<6);	// Start new ADC conversion
	SBI  0x6,6
;      47                         ADCarray[oldADCpin] =  ADCtemp;
	__GETW1R 4,5
	LDI  R26,LOW(_ADCarray)
	LDI  R27,HIGH(_ADCarray)
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	ST   X+,R8
	ST   X,R9
;      48                 	oldADCpin = ADCpin;
	__MOVEWRR 4,5,6,7
;      49                         if(xyzsampledflag == 1) {       //Print only after sampling of all 3 values x,y,z are complete and put in array
	LDI  R30,LOW(1)
	CP   R30,R10
	BREQ PC+3
	JMP _0x8
;      50                 	        if(avgOrgflag < 3) {
	LDI  R30,LOW(3)
	CP   R11,R30
	BRSH _0x9
;      51                                         xyzOrigin[0] += ADCarray[0];
	LDI  R26,LOW(_xyzOrigin)
	LDI  R27,HIGH(_xyzOrigin)
	PUSH R27
	PUSH R26
	CALL __GETW1P
	LDS  R26,_ADCarray
	LDS  R27,_ADCarray+1
	ADD  R30,R26
	ADC  R31,R27
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      52                                         xyzOrigin[1] += ADCarray[1];               
	__POINTW2MN _xyzOrigin,2
	PUSH R27
	PUSH R26
	CALL __GETW1P
	MOVW R26,R30
	__GETW1MN _ADCarray,2
	ADD  R30,R26
	ADC  R31,R27
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      53                                         xyzOrigin[2] += ADCarray[2];
	__POINTW2MN _xyzOrigin,4
	PUSH R27
	PUSH R26
	CALL __GETW1P
	MOVW R26,R30
	__GETW1MN _ADCarray,4
	ADD  R30,R26
	ADC  R31,R27
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      54                                         avgOrgflag++;
	INC  R11
;      55                                 }
;      56                                 else if(avgOrgflag == 3) {
	RJMP _0xA
_0x9:
	LDI  R30,LOW(3)
	CP   R30,R11
	BREQ PC+3
	JMP _0xB
;      57                                         xyzOrigin[0] /= 3;
	LDI  R26,LOW(_xyzOrigin)
	LDI  R27,HIGH(_xyzOrigin)
	PUSH R27
	PUSH R26
	CALL SUBOPT_0x0
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      58                                         xyzOrigin[1] /= 3;
	__POINTW2MN _xyzOrigin,2
	PUSH R27
	PUSH R26
	CALL SUBOPT_0x0
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      59                                         xyzOrigin[2] /= 3;
	__POINTW2MN _xyzOrigin,4
	PUSH R27
	PUSH R26
	CALL SUBOPT_0x0
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;      60                                         printf("\r\nOrigin [%d,%d,%d]",xyzOrigin[0],xyzOrigin[1],xyzOrigin[2]);
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_xyzOrigin
	LDS  R31,_xyzOrigin+1
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETW1MN _xyzOrigin,2
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETW1MN _xyzOrigin,4
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,12
	CALL _printf
	ADIW R28,14
;      61                                         avgOrgflag++;
	INC  R11
;      62                                 }
;      63                                 else {
	RJMP _0xC
_0xB:
;      64                         	        //
;      65                         	        result =  ADCarray[0];
	LDS  R12,_ADCarray
;      66                                         if(result > xyzOrigin[0]) {
	CALL SUBOPT_0x1
	CP   R30,R26
	CPC  R31,R27
	BRSH _0xD
;      67                                 	        xyzSpeed[0] = result - xyzOrigin[0] ;
	CALL SUBOPT_0x1
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	RJMP _0x81
;      68                                         }
;      69                                         else {
_0xD:
;      70                                 	        xyzSpeed[0] = xyzOrigin[0] - result ;
	MOV  R30,R12
	LDS  R26,_xyzOrigin
	LDS  R27,_xyzOrigin+1
	CALL SUBOPT_0x2
	STS  _xyzSpeed,R30
;      71                                 		xyzSpeed[0] |= 0x80; // set to negative number
	ORI  R30,0x80
_0x81:
	STS  _xyzSpeed,R30
;      72                                         }
;      73                                         //printf("\nX=%d",xyzSpeed[0]);
;      74                                         //
;      75                                         result =  ADCarray[1]; // read Y
	__GETBRMN 12,_ADCarray,2
;      76                                         if(result > xyzOrigin[1]) {
	__GETW1MN _xyzOrigin,2
	CALL SUBOPT_0x3
	BRSH _0xF
;      77                                 		xyzSpeed[1] = result - xyzOrigin[1];
	__GETW1MN _xyzOrigin,2
	CALL SUBOPT_0x4
	__PUTB1MN _xyzSpeed,1
;      78                                         }
;      79                                         
;      80                                         else{
	RJMP _0x10
_0xF:
;      81                                 		xyzSpeed[1] = xyzOrigin[1] - result;
	__GETW2MN _xyzOrigin,2
	MOV  R30,R12
	CALL SUBOPT_0x2
	__PUTB1MN _xyzSpeed,1
;      82                                 		xyzSpeed[1] |= 0x80;
	__POINTW1MN _xyzSpeed,1
	CALL SUBOPT_0x5
;      83                                         }
_0x10:
;      84                                         //printf("\nY=%d",xyzSpeed[1]);
;      85                                         //
;      86                                         result =  ADCarray[2]; // read Z
	__GETBRMN 12,_ADCarray,4
;      87                                         if(result > xyzOrigin[2]){
	__GETW1MN _xyzOrigin,4
	CALL SUBOPT_0x3
	BRSH _0x11
;      88                                 		xyzSpeed[2] = result - xyzOrigin[2];
	__GETW1MN _xyzOrigin,4
	CALL SUBOPT_0x4
	__PUTB1MN _xyzSpeed,2
;      89                                         }
;      90                                         
;      91                                         else{
	RJMP _0x12
_0x11:
;      92                                 		xyzSpeed[2] = xyzOrigin[2] - result;
	__GETW2MN _xyzOrigin,4
	MOV  R30,R12
	CALL SUBOPT_0x2
	__PUTB1MN _xyzSpeed,2
;      93                                 		xyzSpeed[2] |= 0x80;
	__POINTW1MN _xyzSpeed,2
	CALL SUBOPT_0x5
;      94                                         }
_0x12:
;      95                                         //printf("\nZ=%d",xyzSpeed[2]);
;      96                                         printf("\tSpeed [%d,%d,%d]",xyzSpeed[0],xyzSpeed[1],xyzSpeed[2]);
	__POINTW1FN _0,20
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_xyzSpeed
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETB1MN _xyzSpeed,1
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETB1MN _xyzSpeed,2
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,12
	CALL _printf
	ADIW R28,14
;      97                                 }
_0xC:
_0xA:
;      98                                 
;      99                                 printf("\r\n[%d,%d,%d]",ADCarray[0],ADCarray[1],ADCarray[2]);
	__POINTW1FN _0,38
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_ADCarray
	LDS  R31,_ADCarray+1
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETW1MN _ADCarray,2
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	__GETW1MN _ADCarray,4
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDI  R24,12
	CALL _printf
	ADIW R28,14
;     100                 	        xyzsampledflag = 0;
	CLR  R10
;     101                 	        
;     102                 	        displayOrientation(xyzSpeed[0], 0);
	LDS  R30,_xyzSpeed
	CALL SUBOPT_0x6
	RCALL _displayOrientation
;     103                 	        displayOrientation(xyzSpeed[1], 1); 
	__GETB1MN _xyzSpeed,1
	CALL SUBOPT_0x7
	RCALL _displayOrientation
;     104                 	           
;     105                 	}
;     106                 }
_0x8:
;     107 
;     108         }
_0x6:
	RJMP _0x4
;     109 }
_0x13:
	RJMP _0x13
;     110 
;     111 void initialize() {              
_initialize:
;     112         UCSRB = 0x18 ;    // UART to setup TX and Rx
	LDI  R30,LOW(24)
	OUT  0xA,R30
;     113         UBRRL = 103 ;     // Baud Rate for mega32.
	LDI  R30,LOW(103)
	OUT  0x9,R30
;     114          
;     115         ADCpin = 0;  // X, Y, Z:  0, 1, 2
	CLR  R6
	CLR  R7
;     116         oldADCpin = 0;
	CLR  R4
	CLR  R5
;     117         xyzsampledflag = 0;
	CLR  R10
;     118         avgOrgflag = 0;
	CLR  R11
;     119         xyzOrigin[0] = 0;
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	STS  _xyzOrigin,R30
	STS  _xyzOrigin+1,R31
;     120         xyzOrigin[1] = 0;
	__PUTW1MN _xyzOrigin,2
;     121         xyzOrigin[2] = 0;
	__PUTW1MN _xyzOrigin,4
;     122         
;     123         xyzSpeed[0] = 0;
	STS  _xyzSpeed,R30
;     124         xyzSpeed[1] = 0;
	__PUTB1MN _xyzSpeed,1
;     125         xyzSpeed[2] = 0;
	__PUTB1MN _xyzSpeed,2
;     126         
;     127         
;     128         //ADC pin 3-5
;     129         ADMUX = (1<<5)|5;		// Channel 5 only
	LDI  R30,LOW(37)
	OUT  0x7,R30
;     130         ADCSR = 0b11000111;
	LDI  R30,LOW(199)
	OUT  0x6,R30
;     131         
;     132         lcd_init(LCDwidth);       //initialize the display
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     133         lcd_clear();
	CALL _lcd_clear
;     134         lcd_gotoxy(2,0); 
	LDI  R30,LOW(2)
	CALL SUBOPT_0x6
	CALL _lcd_gotoxy
;     135         lcd_putsf("MMA7260 TEST");
	__POINTW1FN _0,51
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     136 	printf("\nInitialized\n");
	__POINTW1FN _0,64
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	RJMP _0x80
;     137 }
;     138 
;     139 void displayOrientation(char val, unsigned char axis) {          //Axis: x=0, y=1, z=2
_displayOrientation:
;     140 
;     141 	if( !(val & 0x80) && val ) {    // if positive
	LDD  R30,Y+1
	ANDI R30,LOW(0x80)
	BRNE _0x15
	LDD  R30,Y+1
	CPI  R30,0
	BRNE _0x16
_0x15:
	RJMP _0x14
_0x16:
;     142 		// print on the right
;     143 		if( val > 10 ) {         //PORTB = 0x0C;
	CALL SUBOPT_0x8
	BRSH _0x17
;     144 		        if(axis == 1) {
	LD   R26,Y
	CPI  R26,LOW(0x1)
	BRNE _0x18
;     145         		        lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     146                                 lcd_putsf("FRONT");
	__POINTW1FN _0,78
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     147                         }
;     148                         if(axis == 0) {
_0x18:
	LD   R30,Y
	CPI  R30,0
	BRNE _0x19
;     149                                 lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     150                                 lcd_putsf("LEFT ");
	__POINTW1FN _0,84
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     151                         } 
;     152                 }
_0x19:
;     153                 /* 		
;     154 		else {          //PORTB = 0x08;
;     155 	                lcd_gotoxy(0,1); 
;     156                         lcd_putsf("STOP ");
;     157                 }
;     158                 */        
;     159 	}
_0x17:
;     160 	else if(val) {            // if negative
	RJMP _0x1A
_0x14:
	LDD  R30,Y+1
	CPI  R30,0
	BREQ _0x1B
;     161 		val = val & 0x7F;
	ANDI R30,0x7F
	STD  Y+1,R30
;     162 		// print on le left
;     163 		if( val > 10 ) {   //PORTB = 0x30;
	CALL SUBOPT_0x8
	BRSH _0x1C
;     164 			if(axis == 1) {
	LD   R26,Y
	CPI  R26,LOW(0x1)
	BRNE _0x1D
;     165         		        lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     166                                 lcd_putsf("BACK ");
	__POINTW1FN _0,90
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     167                         }
;     168                         if(axis == 0) {
_0x1D:
	LD   R30,Y
	CPI  R30,0
	BRNE _0x1E
;     169                                 lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     170                                 lcd_putsf("RIGHT");
	__POINTW1FN _0,96
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     171                         } 
;     172                 }
_0x1E:
;     173 		else {          //PORTB = 0x10;
	RJMP _0x1F
_0x1C:
;     174 	                lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     175                         lcd_putsf("STOP ");
	__POINTW1FN _0,102
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     176                 }
_0x1F:
;     177                  
;     178 	}
;     179 	//else PORTB = 0;            // if zeros
;     180 	
;     181 	else { 
	RJMP _0x20
_0x1B:
;     182                 lcd_gotoxy(0,1); 
	LDI  R30,LOW(0)
	CALL SUBOPT_0x7
	CALL _lcd_gotoxy
;     183                 lcd_putsf("STOP ");
	__POINTW1FN _0,102
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     184         } 
_0x20:
_0x1A:
;     185 }
_0x80:
	ADIW R28,2
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
	LD   R26,Y
	LDD  R27,Y+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x21
	CALL __GETW1P
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x22
_0x21:
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _putchar
_0x22:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	CALL __SAVELOCR6
	LDI  R16,0
_0x23:
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
	JMP _0x25
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x29
	CPI  R19,37
	BRNE _0x2A
	LDI  R16,LOW(1)
	RJMP _0x2B
_0x2A:
	CALL SUBOPT_0x9
_0x2B:
	RJMP _0x28
_0x29:
	CPI  R30,LOW(0x1)
	BRNE _0x2C
	CPI  R19,37
	BRNE _0x2D
	CALL SUBOPT_0x9
	LDI  R16,LOW(0)
	RJMP _0x28
_0x2D:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x2E
	LDI  R17,LOW(1)
	RJMP _0x28
_0x2E:
	CPI  R19,43
	BRNE _0x2F
	LDI  R21,LOW(43)
	RJMP _0x28
_0x2F:
	CPI  R19,32
	BRNE _0x30
	LDI  R21,LOW(32)
	RJMP _0x28
_0x30:
	RJMP _0x31
_0x2C:
	CPI  R30,LOW(0x2)
	BRNE _0x32
_0x31:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x33
	ORI  R17,LOW(128)
	RJMP _0x28
_0x33:
	RJMP _0x34
_0x32:
	CPI  R30,LOW(0x3)
	BREQ PC+3
	JMP _0x28
_0x34:
	CPI  R19,48
	BRLO _0x37
	CPI  R19,58
	BRLO _0x38
_0x37:
	RJMP _0x36
_0x38:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x28
_0x36:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x3C
	CALL SUBOPT_0xA
	LD   R30,X
	CALL SUBOPT_0xB
	RJMP _0x3D
_0x3C:
	CPI  R30,LOW(0x73)
	BRNE _0x3F
	CALL SUBOPT_0xA
	CALL SUBOPT_0xC
	CALL _strlen
	MOV  R16,R30
	RJMP _0x40
_0x3F:
	CPI  R30,LOW(0x70)
	BRNE _0x42
	CALL SUBOPT_0xA
	CALL SUBOPT_0xC
	CALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x40:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x43
_0x42:
	CPI  R30,LOW(0x64)
	BREQ _0x46
	CPI  R30,LOW(0x69)
	BRNE _0x47
_0x46:
	ORI  R17,LOW(4)
	RJMP _0x48
_0x47:
	CPI  R30,LOW(0x75)
	BRNE _0x49
_0x48:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x4A
_0x49:
	CPI  R30,LOW(0x58)
	BRNE _0x4C
	ORI  R17,LOW(8)
	RJMP _0x4D
_0x4C:
	CPI  R30,LOW(0x78)
	BREQ PC+3
	JMP _0x7B
_0x4D:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x4A:
	SBRS R17,2
	RJMP _0x4F
	CALL SUBOPT_0xA
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x50
	CALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x50:
	CPI  R21,0
	BREQ _0x51
	SUBI R16,-LOW(1)
	RJMP _0x52
_0x51:
	ANDI R17,LOW(251)
_0x52:
	RJMP _0x53
_0x4F:
	CALL SUBOPT_0xA
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0x53:
_0x43:
	SBRC R17,0
	RJMP _0x54
_0x55:
	CP   R16,R20
	BRSH _0x57
	SBRS R17,7
	RJMP _0x58
	SBRS R17,2
	RJMP _0x59
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x5A
_0x59:
	LDI  R19,LOW(48)
_0x5A:
	RJMP _0x5B
_0x58:
	LDI  R19,LOW(32)
_0x5B:
	CALL SUBOPT_0x9
	SUBI R20,LOW(1)
	RJMP _0x55
_0x57:
_0x54:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x5C
_0x5D:
	CPI  R18,0
	BREQ _0x5F
	SBRS R17,3
	RJMP _0x60
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x82
_0x60:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x82:
	ST   -Y,R30
	CALL SUBOPT_0xD
	CPI  R20,0
	BREQ _0x62
	SUBI R20,LOW(1)
_0x62:
	SUBI R18,LOW(1)
	RJMP _0x5D
_0x5F:
	RJMP _0x63
_0x5C:
_0x65:
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
	BRSH _0x67
	SBRS R17,3
	RJMP _0x68
	SUBI R19,-LOW(7)
	RJMP _0x69
_0x68:
	SUBI R19,-LOW(39)
_0x69:
_0x67:
	SBRC R17,4
	RJMP _0x6B
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x6D
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x6C
_0x6D:
	ORI  R17,LOW(16)
	RJMP _0x6F
_0x6C:
	CP   R20,R18
	BRLO _0x71
	SBRS R17,0
	RJMP _0x72
_0x71:
	RJMP _0x70
_0x72:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x73
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x6F:
	SBRS R17,2
	RJMP _0x74
	ANDI R17,LOW(251)
	ST   -Y,R21
	CALL SUBOPT_0xD
	CPI  R20,0
	BREQ _0x75
	SUBI R20,LOW(1)
_0x75:
_0x74:
_0x73:
_0x6B:
	CALL SUBOPT_0x9
	CPI  R20,0
	BREQ _0x76
	SUBI R20,LOW(1)
_0x76:
_0x70:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x66
	RJMP _0x65
_0x66:
_0x63:
	SBRS R17,0
	RJMP _0x77
_0x78:
	CPI  R20,0
	BREQ _0x7A
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	CALL SUBOPT_0xB
	RJMP _0x78
_0x7A:
_0x77:
_0x7B:
_0x3D:
	LDI  R16,LOW(0)
_0x28:
	RJMP _0x23
_0x25:
	CALL __LOADLOCR6
	ADIW R28,18
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	__PUTW2R 16,17
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
	CALL __print_G2
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
	CALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G4
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G4:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G4
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G4
    ld    r26,y
    swap  r26
	CALL __lcd_write_nibble_G4
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G4:
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G4
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G4
    andi  r30,0xf0
	RET
_lcd_read_byte0_G4:
	CALL __lcd_delay_G4
	CALL __lcd_read_nibble_G4
    mov   r26,r30
	CALL __lcd_read_nibble_G4
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
	LDD  R13,Y+1
	LDD  R14,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R30,LOW(2)
	CALL SUBOPT_0xE
	LDI  R30,LOW(12)
	CALL SUBOPT_0xE
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	LDI  R30,LOW(0)
	MOV  R14,R30
	MOV  R13,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R13
	LDS  R30,__lcd_maxx
	CP   R30,R13
	BRSH _0x7D
	__lcd_putchar1:
	INC  R14
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R14
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x7D:
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
	CALL __lcd_write_nibble_G4
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
	CALL SUBOPT_0xF
	CALL SUBOPT_0xF
	CALL SUBOPT_0xF
	CALL __long_delay_G4
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G4
	CALL __long_delay_G4
	LDI  R30,LOW(40)
	CALL SUBOPT_0x10
	LDI  R30,LOW(4)
	CALL SUBOPT_0x10
	LDI  R30,LOW(133)
	CALL SUBOPT_0x10
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G4
	CPI  R30,LOW(0x5)
	BREQ _0x7E
	LDI  R30,LOW(0)
	RJMP _0x7F
_0x7E:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x7F:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x0:
	CALL __GETW1P
	MOVW R26,R30
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CALL __DIVW21U
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	LDS  R30,_xyzOrigin
	LDS  R31,_xyzOrigin+1
	MOV  R26,R12
	LDI  R27,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x2:
	LDI  R31,0
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	MOV  R26,R12
	LDI  R27,0
	CP   R30,R26
	CPC  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	MOV  R26,R12
	LDI  R27,0
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	MOVW R26,R30
	LD   R30,X
	ORI  R30,0x80
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x7:
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	LDD  R26,Y+1
	LDI  R30,LOW(10)
	CP   R30,R26
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x9:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G2

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
	JMP  __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	CALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __lcd_ready

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xF:
	CALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G4

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x10:
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

__SWAPW12:
	MOV  R1,R27
	MOV  R27,R31
	MOV  R31,R1

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
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
