;CodeVisionAVR C Compiler V1.24.4 Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type           : ATmega32L
;Program type        : Application
;Clock frequency     : 8.000000 MHz
;Memory model        : Small
;Optimize for        : Speed
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

	.INCLUDE "crazy.vec"
	.INCLUDE "crazy.inc"

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
;      11 Date    : 6/27/2007
;      12 Author  : Osama                           
;      13 Company : Osama                           
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
;      26 
;      27 // Alphanumeric LCD Module functions
;      28 #asm
;      29    .equ __lcd_port=0x1B ;PORTA
   .equ __lcd_port=0x1B ;PORTA
;      30 #endasm
;      31 #include <lcd.h>
;      32 #include <count.h>

	.CSEG
_pauser:
	SBIC 0x10,1
	RJMP _0x6
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(15)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	RJMP _0x7
_0x6:
	SBIC 0x10,0
	RJMP _0x8
	CLR  R6
	CLR  R7
	LDI  R30,LOW(15)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,2
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(4)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,4
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
_0x8:
_0x7:
	RET
_stopper:
	SBIC 0x10,2
	RJMP _0x9
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,13
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(15)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,17
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R8
	CPC  R31,R9
	BRNE _0xA
	CLR  R12
	CLR  R13
	RJMP _0xB
_0xA:
	__MOVEWRR 12,13,4,5
_0xB:
_0x9:
	RET
_updown:
	SBIC 0x10,3
	RJMP _0xC
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	CP   R30,R8
	CPC  R31,R9
	BRNE _0xD
	__MOVEWRR 12,13,4,5
	CLR  R8
	CLR  R9
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,19
	RJMP _0x83
_0xD:
	CLR  R12
	CLR  R13
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 8,9
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,24
_0x83:
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
_0xC:
	RET
_adjust:
	SBIW R28,2
;	stra -> Y+0
	SBIC 0x10,4
	RJMP _0xF
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(98)
	LDI  R31,HIGH(98)
	CP   R30,R4
	CPC  R31,R5
	BRLT _0x10
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 4,5,30,31
	RJMP _0x11
_0x10:
	CLR  R4
	CLR  R5
_0x11:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R8
	CPC  R31,R9
	BRNE _0x12
	CLR  R12
	CLR  R13
	RJMP _0x13
_0x12:
	__MOVEWRR 12,13,4,5
_0x13:
	RJMP _0x14
_0xF:
	SBIC 0x10,5
	RJMP _0x15
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	CP   R4,R30
	CPC  R5,R31
	BRLT _0x16
	__GETW1R 4,5
	SBIW R30,1
	RJMP _0x84
_0x16:
	LDI  R30,LOW(99)
	LDI  R31,HIGH(99)
_0x84:
	__PUTW1R 4,5
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R8
	CPC  R31,R9
	BRNE _0x18
	CLR  R12
	CLR  R13
	RJMP _0x19
_0x18:
	__MOVEWRR 12,13,4,5
_0x19:
_0x15:
_0x14:
	LDI  R30,LOW(10)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,29
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 4,5
	CALL __CWD1
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_puts
	ADIW R28,2
	RET
_count:
	SBIW R28,3
;	strc -> Y+0
	CALL _pauser
	CALL _stopper
	CALL _updown
	CALL _adjust
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
	MOV  R0,R6
	OR   R0,R7
	BREQ PC+3
	JMP _0x1A
	__MOVEWRR 10,11,12,13
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,9
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R8
	CPC  R31,R9
	BRNE _0x1B
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,33
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 10,11
	ADIW R30,1
	__PUTW1R 10,11
	SBIW R30,1
	CALL __CWD1
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_puts
	__CPWRR 4,5,10,11
	BRGE _0x1C
	CLR  R10
	CLR  R11
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(4)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,37
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
_0x1C:
	RJMP _0x1D
_0x1B:
	MOV  R0,R8
	OR   R0,R9
	BRNE _0x1E
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,33
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 10,11
	SBIW R30,1
	__PUTW1R 10,11
	ADIW R30,1
	CALL __CWD1
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_puts
	CLR  R0
	CP   R10,R0
	CPC  R11,R0
	BRGE _0x1F
	__MOVEWRR 10,11,4,5
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 6,7
	LDI  R30,LOW(4)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
	__POINTW1FN _0,37
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
_0x1F:
_0x1E:
_0x1D:
	__MOVEWRR 12,13,10,11
	LDI  R30,LOW(900)
	LDI  R31,HIGH(900)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
_0x1A:
	ADIW R28,3
	RET
;      33 
;      34 // Declare your global variables here
;      35 
;      36 void main(void)
;      37 {     
_main:
;      38    char strd[3];   
;      39 // Declare your local variables here
;      40 
;      41 // Input/Output Ports initialization
;      42 // Port A initialization
;      43 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      44 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      45 PORTA=0x00;
	SBIW R28,3
;	strd -> Y+0
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;      46 DDRA=0x00;
	OUT  0x1A,R30
;      47 
;      48 // Port B initialization
;      49 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      50 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      51 PORTB=0x00;
	OUT  0x18,R30
;      52 DDRB=0x00;
	OUT  0x17,R30
;      53 
;      54 // Port C initialization
;      55 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      56 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      57 PORTC=0x00;
	OUT  0x15,R30
;      58 DDRC=0x00;
	OUT  0x14,R30
;      59 
;      60 // Port D initialization
;      61 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      62 // State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
;      63 PORTD=0xFF;
	LDI  R30,LOW(255)
	OUT  0x12,R30
;      64 DDRD=0x00;
	LDI  R30,LOW(0)
	OUT  0x11,R30
;      65 
;      66 // Timer/Counter 0 initialization
;      67 // Clock source: System Clock
;      68 // Clock value: Timer 0 Stopped
;      69 // Mode: Normal top=FFh
;      70 // OC0 output: Disconnected
;      71 TCCR0=0x00;
	OUT  0x33,R30
;      72 TCNT0=0x00;
	OUT  0x32,R30
;      73 OCR0=0x00;
	OUT  0x3C,R30
;      74 
;      75 // Timer/Counter 1 initialization
;      76 // Clock source: System Clock
;      77 // Clock value: Timer 1 Stopped
;      78 // Mode: Normal top=FFFFh
;      79 // OC1A output: Discon.
;      80 // OC1B output: Discon.
;      81 // Noise Canceler: Off
;      82 // Input Capture on Falling Edge
;      83 TCCR1A=0x00;
	OUT  0x2F,R30
;      84 TCCR1B=0x00;
	OUT  0x2E,R30
;      85 TCNT1H=0x00;
	OUT  0x2D,R30
;      86 TCNT1L=0x00;
	OUT  0x2C,R30
;      87 ICR1H=0x00;
	OUT  0x27,R30
;      88 ICR1L=0x00;
	OUT  0x26,R30
;      89 OCR1AH=0x00;
	OUT  0x2B,R30
;      90 OCR1AL=0x00;
	OUT  0x2A,R30
;      91 OCR1BH=0x00;
	OUT  0x29,R30
;      92 OCR1BL=0x00;
	OUT  0x28,R30
;      93 
;      94 // Timer/Counter 2 initialization
;      95 // Clock source: System Clock
;      96 // Clock value: Timer 2 Stopped
;      97 // Mode: Normal top=FFh
;      98 // OC2 output: Disconnected
;      99 ASSR=0x00;
	OUT  0x22,R30
;     100 TCCR2=0x00;
	OUT  0x25,R30
;     101 TCNT2=0x00;
	OUT  0x24,R30
;     102 OCR2=0x00;
	OUT  0x23,R30
;     103 
;     104 // External Interrupt(s) initialization
;     105 // INT0: Off
;     106 // INT1: Off
;     107 // INT2: Off
;     108 MCUCR=0x00;
	OUT  0x35,R30
;     109 MCUCSR=0x00;
	OUT  0x34,R30
;     110 
;     111 // Timer(s)/Counter(s) Interrupt(s) initialization
;     112 TIMSK=0x00;
	OUT  0x39,R30
;     113 
;     114 // Analog Comparator initialization
;     115 // Analog Comparator: Off
;     116 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     117 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     118 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     119 
;     120 // LCD module initialization
;     121 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     122  
;     123 lcd_gotoxy(1,0);
	LDI  R30,LOW(1)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
;     124 lcd_putsf("Made by:");
	__POINTW1FN _0,46
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     125 lcd_gotoxy(2,1);
	LDI  R30,LOW(2)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
;     126 lcd_putsf("OSAMA SALAH");
	__POINTW1FN _0,55
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     127 delay_ms(2000);
	LDI  R30,LOW(2000)
	LDI  R31,HIGH(2000)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     128 lcd_clear();
	CALL _lcd_clear
;     129  
;     130 lcd_gotoxy(0,0);   
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R30
	CALL _lcd_gotoxy
;     131 lcd_putsf("Up");
	__POINTW1FN _0,67
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     132 sprintf(strd,"%2d",end);
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,29
	ST   -Y,R31
	ST   -Y,R30
	__GETW1R 4,5
	CALL __CWD1
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
;     133 lcd_gotoxy(6,0);
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _lcd_gotoxy
;     134 lcd_putsf("End:");
	__POINTW1FN _0,70
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     135 lcd_gotoxy(6,1);
	LDI  R30,LOW(6)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL _lcd_gotoxy
;     136 lcd_putsf("  0");
	__POINTW1FN _0,13
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     137 
;     138 
;     139 while (1)
_0x20:
;     140       {     
;     141          count();         
	CALL _count
;     142       };
	RJMP _0x20
;     143 }
	ADIW R28,3
_0x23:
	RJMP _0x23

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
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R30,LOW(12)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
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
	BRSH _0x25
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
_0x25:
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
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __long_delay_G2
	LDI  R30,LOW(4)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __long_delay_G2
	LDI  R30,LOW(133)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __long_delay_G2
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x26
	LDI  R30,LOW(0)
	RJMP _0x82
_0x26:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x82:
	ADIW R28,1
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
__put_G3:
	put:
	LD   R26,Y
	LDD  R27,Y+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x27
	CALL __GETW1P
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x28
_0x27:
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _putchar
_0x28:
	ADIW R28,3
	RET
__print_G3:
	SBIW R28,6
	CALL __SAVELOCR6
	LDI  R16,0
_0x29:
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
	JMP _0x2B
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x2F
	CPI  R19,37
	BRNE _0x30
	LDI  R16,LOW(1)
	RJMP _0x31
_0x30:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
_0x31:
	RJMP _0x2E
_0x2F:
	CPI  R30,LOW(0x1)
	BRNE _0x32
	CPI  R19,37
	BRNE _0x33
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	LDI  R16,LOW(0)
	RJMP _0x2E
_0x33:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x34
	LDI  R17,LOW(1)
	RJMP _0x2E
_0x34:
	CPI  R19,43
	BRNE _0x35
	LDI  R21,LOW(43)
	RJMP _0x2E
_0x35:
	CPI  R19,32
	BRNE _0x36
	LDI  R21,LOW(32)
	RJMP _0x2E
_0x36:
	RJMP _0x37
_0x32:
	CPI  R30,LOW(0x2)
	BRNE _0x38
_0x37:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x39
	ORI  R17,LOW(128)
	RJMP _0x2E
_0x39:
	RJMP _0x3A
_0x38:
	CPI  R30,LOW(0x3)
	BREQ PC+3
	JMP _0x2E
_0x3A:
	CPI  R19,48
	BRLO _0x3D
	CPI  R19,58
	BRLO _0x3E
_0x3D:
	RJMP _0x3C
_0x3E:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x2E
_0x3C:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x42
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	LD   R30,X
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	RJMP _0x43
_0x42:
	CPI  R30,LOW(0x73)
	BRNE _0x45
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	CALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlen
	MOV  R16,R30
	RJMP _0x46
_0x45:
	CPI  R30,LOW(0x70)
	BRNE _0x48
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	CALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x46:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x49
_0x48:
	CPI  R30,LOW(0x64)
	BREQ _0x4C
	CPI  R30,LOW(0x69)
	BRNE _0x4D
_0x4C:
	ORI  R17,LOW(4)
	RJMP _0x4E
_0x4D:
	CPI  R30,LOW(0x75)
	BRNE _0x4F
_0x4E:
	LDI  R30,LOW(_tbl10_G3*2)
	LDI  R31,HIGH(_tbl10_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x50
_0x4F:
	CPI  R30,LOW(0x58)
	BRNE _0x52
	ORI  R17,LOW(8)
	RJMP _0x53
_0x52:
	CPI  R30,LOW(0x78)
	BREQ PC+3
	JMP _0x81
_0x53:
	LDI  R30,LOW(_tbl16_G3*2)
	LDI  R31,HIGH(_tbl16_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x50:
	SBRS R17,2
	RJMP _0x55
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x56
	CALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x56:
	CPI  R21,0
	BREQ _0x57
	SUBI R16,-LOW(1)
	RJMP _0x58
_0x57:
	ANDI R17,LOW(251)
_0x58:
	RJMP _0x59
_0x55:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0x59:
_0x49:
	SBRC R17,0
	RJMP _0x5A
_0x5B:
	CP   R16,R20
	BRSH _0x5D
	SBRS R17,7
	RJMP _0x5E
	SBRS R17,2
	RJMP _0x5F
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x60
_0x5F:
	LDI  R19,LOW(48)
_0x60:
	RJMP _0x61
_0x5E:
	LDI  R19,LOW(32)
_0x61:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	SUBI R20,LOW(1)
	RJMP _0x5B
_0x5D:
_0x5A:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x62
_0x63:
	CPI  R18,0
	BREQ _0x65
	SBRS R17,3
	RJMP _0x66
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x85
_0x66:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x85:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	CPI  R20,0
	BREQ _0x68
	SUBI R20,LOW(1)
_0x68:
	SUBI R18,LOW(1)
	RJMP _0x63
_0x65:
	RJMP _0x69
_0x62:
_0x6B:
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
	BRSH _0x6D
	SBRS R17,3
	RJMP _0x6E
	SUBI R19,-LOW(7)
	RJMP _0x6F
_0x6E:
	SUBI R19,-LOW(39)
_0x6F:
_0x6D:
	SBRC R17,4
	RJMP _0x71
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x73
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x72
_0x73:
	ORI  R17,LOW(16)
	RJMP _0x75
_0x72:
	CP   R20,R18
	BRLO _0x77
	SBRS R17,0
	RJMP _0x78
_0x77:
	RJMP _0x76
_0x78:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x79
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x75:
	SBRS R17,2
	RJMP _0x7A
	ANDI R17,LOW(251)
	ST   -Y,R21
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	CPI  R20,0
	BREQ _0x7B
	SUBI R20,LOW(1)
_0x7B:
_0x7A:
_0x79:
_0x71:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	CPI  R20,0
	BREQ _0x7C
	SUBI R20,LOW(1)
_0x7C:
_0x76:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x6C
	RJMP _0x6B
_0x6C:
_0x69:
	SBRS R17,0
	RJMP _0x7D
_0x7E:
	CPI  R20,0
	BREQ _0x80
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	RJMP _0x7E
_0x80:
_0x7D:
_0x81:
_0x43:
	LDI  R16,LOW(0)
_0x2E:
	RJMP _0x29
_0x2B:
	CALL __LOADLOCR6
	ADIW R28,18
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	__PUTW2R 16,17
	MOVW R26,R28
	ADIW R26,6
	CALL __ADDW2R15
	CALL __GETW1P
	STD  Y+2,R30
	STD  Y+2+1,R31
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
	CALL __print_G3
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	POP  R15
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

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
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
