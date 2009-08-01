;CodeVisionAVR C Compiler V1.24.4 Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type           : ATmega32
;Program type        : Application
;Clock frequency     : 12.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;(s)scanf features   : int, width
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

	.INCLUDE "leds.vec"
	.INCLUDE "leds.inc"

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
;       2 Chip type           : ATmega32
;       3 Program type        : Application
;       4 Clock frequency     : 12.000000 MHz
;       5 Memory model        : Small
;       6 External SRAM size  : 0
;       7 Data Stack size     : 512
;       8 *****************************************************/
;       9 #include <mega32.h>
;      10 #include <delay.h>   
;      11 flash unsigned char PicData[14][24]=

	.CSEG
;      12 	{
;      13 		{0xFF,0xFF,0xFF,0x3F,0x3F,0x7,0xF9,0xF9,0x7,0xF9,0x31,0x7,0xFF,0xFF,0xFF,0x87,0x3,0x1,0x3,0x1,0x7,0xF,0x3F,0x7F},
;      14 		{0xFF,0xFF,0xF0,0xF,0xFF,0xF0,0x8F,0xFF,0xF0,0xFF,0x49,0xB0,0xF,0xFF,0xFF,0xF,0x6,0x0,0x0,0x0,0x0,0x80,0xC0,0xF0},
;      15 		{0xFF,0xFF,0xFF,0x3C,0xC3,0xFF,0xFF,0xFF,0x7F,0x9F,0xB2,0x8D,0x60,0x9F,0x1F,0x7E,0xF8,0xFC,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF},
;      16 		{0xE3,0x9C,0x9D,0x9E,0xCF,0xE7,0xF,0xF1,0xFE,0x7F,0x1F,0xF,0xE,0xD,0x3,0x8C,0x91,0xEF,0xC7,0xEF,0x9F,0x1F,0x8F,0x3},
;      17 		{0xFF,0xFF,0xFF,0xFF,0x1F,0xE7,0x60,0x1B,0xE7,0x18,0x60,0xF0,0x9C,0x9C,0x63,0xFF,0x7,0x1F,0x1F,0x7F,0xFF,0xFF,0xE4,0x9B},
;      18 		{0xFF,0x3F,0xC3,0x0,0xFF,0x24,0xC3,0x3C,0xFF,0x1C,0x3C,0xFF,0xFF,0x7,0xF8,0xFF,0x0,0x0,0x3F,0xC0,0xFF,0xFF,0xFF,0x24},
;      19 		{0x7,0xF8,0x7,0x0,0x1F,0xFF,0x26,0xC0,0x3F,0xE0,0xD8,0xFF,0xFF,0xFF,0x18,0xE7,0xFC,0xFF,0xFF,0xFF,0x3F,0xDF,0x27,0xD9},
;      20 		{0xF8,0x80,0x0,0x0,0x0,0x1,0xF,0x3E,0xC8,0x31,0xC7,0x89,0x39,0xBE,0xF,0x37,0x19,0x27,0x19,0x6,0x19,0x6,0x1,0x0},
;      21 		{0xFF,0xFF,0xF8,0xF0,0xC0,0xC0,0x80,0x80,0x0,0x0,0x0,0x81,0x76,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0},
;      22 		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF,0xF2,0x8C,0x6,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xC0,0xF0,0xF2,0xF1,0xF0},
;      23 		{0xFF,0xE3,0x9C,0x9D,0x7C,0xF3,0xF3,0xF0,0x8F,0x7F,0xFC,0xFC,0xF0,0x70,0x90,0x60,0x30,0x8C,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF},
;      24 		{0xFF,0xFF,0xFF,0xFF,0x1C,0xE3,0xFF,0xFF,0xFF,0xFF,0xFC,0x64,0x98,0x3,0xFC,0xFC,0xFF,0xFF,0x1F,0xF,0x7,0xF,0x7,0x1F},
;      25 		{0xFF,0xFF,0xFF,0x7,0xF8,0xFF,0x7,0xF8,0xFF,0x7,0xFF,0xDB,0x4,0xF8,0xFF,0xFF,0xFF,0xFF,0x3C,0x18,0x0,0x0,0x0,0x0},
;      26 		{0xFF,0xFF,0xFF,0xFF,0xFC,0xFC,0xE0,0xCF,0xDF,0xE0,0xDF,0xC4,0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0xF0,0xE0,0xF0,0xE0,0xF8}
;      27 	};
;      28 	
;      29 	
;      30 unsigned char satr=0;/*ãÊÛíÑ ÔãÇÑäÏå ÓØÑ*/
;      31 unsigned char hour=12,minute=58,second=30,weekday=6,monthday=7,monthyear=5,timedigit[9];

	.DSEG
_timedigit:
	.BYTE 0x9
;      32 unsigned int year=1386;
;      33 unsigned char char1;
;      34 unsigned char char2;
;      35 
;      36 interrupt [TIM1_OVF] void timer1_ovf_isr(void)
;      37 {

	.CSEG
_timer1_ovf_isr:
	ST   -Y,R1
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      38    unsigned char i=0;
;      39    TCNT1H=0xd1;
	ST   -Y,R16
;	i -> R16
	LDI  R16,0
	LDI  R30,LOW(209)
	OUT  0x2D,R30
;      40    TCNT1L=0x1f;  
	LDI  R30,LOW(31)
	OUT  0x2C,R30
;      41    PORTB.0=1;//OE inactive    
	SBI  0x18,0
;      42 
;      43    PORTD=0xff-PicData[0][satr];
	LDI  R26,LOW(_PicData*2)
	LDI  R27,HIGH(_PicData*2)
	RCALL SUBOPT_0x0
;      44    PORTC=0x20|i;
;      45    PORTC=PORTC|0x30;
;      46    i++;          
	SUBI R16,-1
;      47                          
;      48    PORTD=0xff-PicData[1][satr];
	__POINTW1FN _PicData,24
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      49    PORTC=0x20|i;
;      50    PORTC=PORTC|0x30;
;      51    i++;              
	SUBI R16,-1
;      52    
;      53    PORTD=0xff-PicData[2][satr];
	__POINTW1FN _PicData,48
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      54    PORTC=0x20|i;
;      55    PORTC=PORTC|0x30;
;      56    i++;                      
	SUBI R16,-1
;      57    
;      58    PORTD=0xff-PicData[3][satr];
	__POINTW1FN _PicData,72
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      59    PORTC=0x20|i;
;      60    PORTC=PORTC|0x30;
;      61    i++;      
	SUBI R16,-1
;      62 
;      63    PORTD=0xff-PicData[4][satr];
	__POINTW1FN _PicData,96
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      64    PORTC=0x20|i;
;      65    PORTC=PORTC|0x30;
;      66    i++;   
	SUBI R16,-1
;      67                                
;      68    PORTD=0xff-PicData[5][satr];
	__POINTW1FN _PicData,120
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      69    PORTC=0x20|i;
;      70    PORTC=PORTC|0x30;
;      71    i++;
	SUBI R16,-1
;      72 
;      73    PORTD=0xff-PicData[6][satr];
	__POINTW1FN _PicData,144
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      74    PORTC=0x20|i;
;      75    PORTC=PORTC|0x30;
;      76    i++;
	SUBI R16,-1
;      77 
;      78    PORTD=0xff-PicData[7][satr];
	__POINTW1FN _PicData,168
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      79    PORTC=0x20|i;
;      80    PORTC=PORTC|0x30;
;      81    i++;
	SUBI R16,-1
;      82 
;      83    PORTD=0xff-PicData[8][satr];
	__POINTW1FN _PicData,192
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      84    PORTC=0x20|i;
;      85    PORTC=PORTC|0x30;
;      86    i++;     
	SUBI R16,-1
;      87 
;      88    PORTD=0xff-PicData[9][satr];
	__POINTW1FN _PicData,216
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      89    PORTC=0x20|i;       
;      90    PORTC=PORTC|0x30;
;      91    i++;                      
	SUBI R16,-1
;      92    
;      93    PORTD=0xff-PicData[10][satr];
	__POINTW1FN _PicData,240
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      94    PORTC=0x20|i;
;      95    PORTC=PORTC|0x30;
;      96    i++;
	SUBI R16,-1
;      97 
;      98    PORTD=0xff-PicData[11][satr];
	__POINTW1FN _PicData,264
	MOVW R26,R30
	RCALL SUBOPT_0x0
;      99    PORTC=0x20|i;
;     100    PORTC=PORTC|0x30;
;     101    i++;
	SUBI R16,-1
;     102 
;     103    PORTD=0xff-PicData[12][satr];
	__POINTW1FN _PicData,288
	MOVW R26,R30
	RCALL SUBOPT_0x0
;     104    PORTC=0x20|i;
;     105    PORTC=PORTC|0x30;
;     106    i++;
	SUBI R16,-1
;     107 
;     108    PORTD=0xff-PicData[13][satr];
	__POINTW1FN _PicData,312
	MOVW R26,R30
	RCALL SUBOPT_0x0
;     109    PORTC=0x20|i;
;     110    PORTC=PORTC|0x30;
;     111    i++;
	SUBI R16,-1
;     112      
;     113    PORTB.0=0;   
	CBI  0x18,0
;     114    PORTC=satr;
	OUT  0x15,R4
;     115    satr++;   
	INC  R4
;     116    if(satr==24)
	LDI  R30,LOW(24)
	CP   R30,R4
	BRNE _0xA
;     117       satr=0; 
	CLR  R4
;     118 }
_0xA:
	LD   R16,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R1,Y+
	RETI
;     119 
;     120 // Timer 2 overflow interrupt service routine
;     121 interrupt [TIM2_OVF] void timer2_ovf_isr(void)
;     122 {                                             
_timer2_ovf_isr:
;     123             
;     124 }
	RETI
;     125 
;     126 // Declare your global variables here
;     127 void main(void)
;     128 {
_main:
;     129 // Declare your local variables here
;     130 
;     131 // Input/Output Ports initialization
;     132 // Port A initialization
;     133 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     134 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     135 PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     136 DDRA=0xff;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     137 
;     138 // Port B initialization
;     139 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     140 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     141 PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;     142 DDRB=0x01;
	LDI  R30,LOW(1)
	OUT  0x17,R30
;     143 
;     144 // Port C initialization
;     145 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     146 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     147 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;     148 DDRC=0xff;
	LDI  R30,LOW(255)
	OUT  0x14,R30
;     149 
;     150 // Port D initialization
;     151 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     152 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     153 PORTD=0x00;
	LDI  R30,LOW(0)
	OUT  0x12,R30
;     154 DDRD=0xff;
	LDI  R30,LOW(255)
	OUT  0x11,R30
;     155 
;     156 // Timer/Counter 0 initialization
;     157 // Clock source: System Clock
;     158 // Clock value: Timer 0 Stopped
;     159 // Mode: Normal top=FFh
;     160 // OC0 output: Disconnected
;     161 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
;     162 TCNT0=0x00;
	OUT  0x32,R30
;     163 OCR0=0x00;
	OUT  0x3C,R30
;     164 
;     165 // Timer/Counter 1 initialization
;     166 // Clock source: System Clock
;     167 // Clock value: Timer 1 Stopped
;     168 // Mode: Normal top=FFFFh
;     169 // OC1A output: Discon.
;     170 // OC1B output: Discon.
;     171 // Noise Canceler: Off
;     172 // Input Capture on Falling Edge
;     173 // Timer 1 Overflow Interrupt: Off
;     174 // Input Capture Interrupt: Off
;     175 // Compare A Match Interrupt: Off
;     176 // Compare B Match Interrupt: Off
;     177 
;     178 TCCR1A=0x00;
	OUT  0x2F,R30
;     179 TCCR1B=0x81;
	LDI  R30,LOW(129)
	OUT  0x2E,R30
;     180 TCNT1H=0xa2;
	LDI  R30,LOW(162)
	OUT  0x2D,R30
;     181 TCNT1L=0x3f;
	LDI  R30,LOW(63)
	OUT  0x2C,R30
;     182 ICR1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x27,R30
;     183 ICR1L=0x00;
	OUT  0x26,R30
;     184 OCR1AH=0x00;
	OUT  0x2B,R30
;     185 OCR1AL=0x00;
	OUT  0x2A,R30
;     186 OCR1BH=0x00;
	OUT  0x29,R30
;     187 OCR1BL=0x00;
	OUT  0x28,R30
;     188 
;     189 // Timer/Counter 2 initialization
;     190 // Clock source: TOSC1 pin
;     191 // Clock value: PCK2/128
;     192 // Mode: Normal top=FFh
;     193 // OC2 output: Disconnected
;     194 ASSR=0x08;
	LDI  R30,LOW(8)
	OUT  0x22,R30
;     195 TCCR2=0x05;
	LDI  R30,LOW(5)
	OUT  0x25,R30
;     196 TCNT2=0x00;
	LDI  R30,LOW(0)
	OUT  0x24,R30
;     197 OCR2=0x00;
	OUT  0x23,R30
;     198 
;     199 // External Interrupt(s) initialization
;     200 // INT0: Off
;     201 // INT1: Off
;     202 // INT2: Off
;     203 MCUCR=0x00;
	OUT  0x35,R30
;     204 MCUCSR=0x00;
	OUT  0x34,R30
;     205 
;     206 // Timer(s)/Counter(s) Interrupt(s) initialization
;     207 TIMSK=0x44;
	LDI  R30,LOW(68)
	OUT  0x39,R30
;     208 
;     209 // Analog Comparator initialization
;     210 // Analog Comparator: Off
;     211 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     212 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     213 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     214 
;     215 // Global enable interrupts
;     216 #asm("sei")
	sei
;     217 
;     218 while (1);
_0xB:
	RJMP _0xB
;     219 
;     220 }
_0xE:
	RJMP _0xE


;OPTIMIZER ADDED SUBROUTINE, CALLED 14 TIMES
SUBOPT_0x0:
	MOV  R30,R4
	LDI  R31,0
	ADD  R30,R26
	ADC  R31,R27
	LPM  R30,Z
	LDI  R26,LOW(255)
	CALL __SWAPB12
	SUB  R30,R26
	OUT  0x12,R30
	MOV  R30,R16
	ORI  R30,0x20
	OUT  0x15,R30
	IN   R30,0x15
	ORI  R30,LOW(0x30)
	OUT  0x15,R30
	RET

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
	RET

;END OF CODE MARKER
__END_OF_CODE:
