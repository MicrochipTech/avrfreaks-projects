;CodeVisionAVR C Compiler V1.24.5 Light
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type              : AT90S8515
;Clock frequency        : 7.372800 MHz
;Memory model           : Small
;Optimize for           : Speed
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 128 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : No
;Word align FLASH struct: No
;Automatic register allocation : On

	.DEVICE AT90S8515
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
	.EQU RAMPZ=0x3B
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
	.EQU __sm_mask=0x10

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
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	RCALL __PUTDP1
	.ENDM


	.MACRO __GETB1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETW1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOV  R30,R0
	MOV  R31,R1
	.ENDM

	.MACRO __GETB2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOV  R26,R0
	MOV  R27,R1
	.ENDM

	.MACRO __GETBRSX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __LSLW8SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __CLRD1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __PUTB2SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTBSRX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R@1
	.ENDM

	.MACRO __PUTWSRX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOV  R26,R28
	MOV  R27,R29
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
	MOV  R26,R28
	MOV  R27,R29
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
	MOV  R26,R28
	MOV  R27,R29
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

	.CSEG
	.ORG 0

	.INCLUDE "main.vec"
	.INCLUDE "main.inc"

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30
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
	LPM
	ADIW R30,1
	MOV  R24,R0
	LPM
	ADIW R30,1
	MOV  R25,R0
	SBIW R24,0
	BREQ __GLOBAL_INI_END
	LPM
	ADIW R30,1
	MOV  R26,R0
	LPM
	ADIW R30,1
	MOV  R27,R0
	LPM
	ADIW R30,1
	MOV  R1,R0
	LPM
	ADIW R30,1
	MOV  R22,R30
	MOV  R23,R31
	MOV  R31,R0
	MOV  R30,R1
__GLOBAL_INI_LOOP:
	LPM
	ADIW R30,1
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOV  R30,R22
	MOV  R31,R23
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
;       1 // Remember to include avr313.xcl as your linker file. (Modified for AT90S8515)
;       2 
;       3 //#include <pgmspace.h>
;       4 //#include <stdio.h>
;       5 //#include <stdlib.h>
;       6 #include <90s8515.h>
;       7 #include <delay.h>
;       8 
;       9 //#include "serial.h"
;      10 #include "gpr.h"
;      11 #include "kb.h"
;      12 //#include "capture.h"
;      13 // Alphanumeric LCD Module functions
;      14 #asm
;      15    .equ __lcd_port=0x18 ;PORTB  
   .equ __lcd_port=0x18 ;PORTB  
;      16 #endasm
;      17 #include <lcd.h>
;      18 
;      19 #define backlight PORTD.5
;      20 #define RXB8 1
;      21 #define TXB8 0
;      22 #define OVR 3
;      23 #define FE 4
;      24 #define UDRE 5
;      25 #define RXC 7
;      26 
;      27 #define FRAMING_ERROR (1<<FE)
;      28 #define DATA_OVERRUN (1<<OVR)
;      29 #define DATA_REGISTER_EMPTY (1<<UDRE)
;      30 #define RX_COMPLETE (1<<RXC)
;      31 
;      32 // UART Receiver buffer
;      33 #define RX_BUFFER_SIZE 96
;      34 char rx_buffer[RX_BUFFER_SIZE];
_rx_buffer:
	.BYTE 0x60
;      35 
;      36 #if RX_BUFFER_SIZE<256
;      37 unsigned char rx_wr_index,rx_rd_index,rx_counter;
;      38 #else
;      39 unsigned int rx_wr_index,rx_rd_index,rx_counter;
;      40 #endif
;      41 
;      42 // This flag is set on UART Receiver buffer overflow
;      43 bit rx_buffer_overflow;
;      44 
;      45 // UART Receiver interrupt service routine
;      46 interrupt [UART_RXC] void uart_rx_isr(void)
;      47 {

	.CSEG
_uart_rx_isr:
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	IN   R30,SREG
	ST   -Y,R30
;      48 char status,data;
;      49 status=USR;
	RCALL __SAVELOCR2
;	status -> R16
;	data -> R17
	IN   R16,11
;      50 data=UDR;
	IN   R17,12
;      51 if ((status & (FRAMING_ERROR | DATA_OVERRUN))==0)
	MOV  R30,R16
	ANDI R30,LOW(0x18)
	BRNE _0x3
;      52    {
;      53    rx_buffer[rx_wr_index]=data;
	MOV  R26,R4
	LDI  R27,0
	SUBI R26,LOW(-_rx_buffer)
	SBCI R27,HIGH(-_rx_buffer)
	ST   X,R17
;      54    if (++rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
	INC  R4
	LDI  R30,LOW(96)
	CP   R30,R4
	BRNE _0x4
	CLR  R4
;      55    if (++rx_counter == RX_BUFFER_SIZE)
_0x4:
	INC  R6
	LDI  R30,LOW(96)
	CP   R30,R6
	BRNE _0x5
;      56       {
;      57       rx_counter=0;
	CLR  R6
;      58       rx_buffer_overflow=1;
	SET
	BLD  R2,0
;      59       };
_0x5:
;      60    };
_0x3:
;      61 }
	RCALL __LOADLOCR2P
	LD   R30,Y+
	OUT  SREG,R30
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	RETI
;      62 
;      63 #ifndef _DEBUG_TERMINAL_IO_
;      64 // Get a character from the UART Receiver buffer
;      65 #define _ALTERNATE_GETCHAR_
;      66 #pragma used+
;      67 char getchar(void)
;      68 {
_getchar:
;      69 char data;
;      70 //while (rx_counter==0);
;      71 if(rx_counter!=0)
	ST   -Y,R16
;	data -> R16
	TST  R6
	BREQ _0x6
;      72 {
;      73 data=rx_buffer[rx_rd_index];
	MOV  R30,R5
	LDI  R31,0
	SUBI R30,LOW(-_rx_buffer)
	SBCI R31,HIGH(-_rx_buffer)
	LD   R16,Z
;      74 if (++rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
	INC  R5
	LDI  R30,LOW(96)
	CP   R30,R5
	BRNE _0x7
	CLR  R5
;      75 #asm("cli")
_0x7:
	cli
;      76 --rx_counter;
	DEC  R6
;      77 #asm("sei")
	sei
;      78 return data;
	MOV  R30,R16
	RJMP _0x15A
;      79 }
;      80 else
_0x6:
;      81 return 0;
	LDI  R30,LOW(0)
;      82 }
_0x15A:
	LD   R16,Y+
	RET
;      83 #pragma used-
;      84 #endif
;      85 
;      86 // UART Transmitter buffer
;      87 #define TX_BUFFER_SIZE 8
;      88 char tx_buffer[TX_BUFFER_SIZE];

	.DSEG
_tx_buffer:
	.BYTE 0x8
;      89 
;      90 #if TX_BUFFER_SIZE<256
;      91 unsigned char tx_wr_index,tx_rd_index,tx_counter;
;      92 #else
;      93 unsigned int tx_wr_index,tx_rd_index,tx_counter;
;      94 #endif
;      95 
;      96 // UART Transmitter interrupt service routine
;      97 interrupt [UART_TXC] void uart_tx_isr(void)
;      98 {

	.CSEG
_uart_tx_isr:
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      99 if (tx_counter)
	TST  R9
	BREQ _0x9
;     100    {
;     101    --tx_counter;
	DEC  R9
;     102    UDR=tx_buffer[tx_rd_index];
	MOV  R30,R8
	LDI  R31,0
	SUBI R30,LOW(-_tx_buffer)
	SBCI R31,HIGH(-_tx_buffer)
	LD   R30,Z
	OUT  0xC,R30
;     103    if (++tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
	INC  R8
	LDI  R30,LOW(8)
	CP   R30,R8
	BRNE _0xA
	CLR  R8
;     104    };
_0xA:
_0x9:
;     105 }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	RETI
;     106 
;     107 #ifndef _DEBUG_TERMINAL_IO_
;     108 // Write a character to the UART Transmitter buffer
;     109 #define _ALTERNATE_PUTCHAR_
;     110 #pragma used+
;     111 void putchar(char c)
;     112 {
_putchar:
;     113 while (tx_counter == TX_BUFFER_SIZE);
_0xB:
	LDI  R30,LOW(8)
	CP   R30,R9
	BREQ _0xB
;     114 #asm("cli")
	cli
;     115 if (tx_counter || ((USR & DATA_REGISTER_EMPTY)==0))
	TST  R9
	BRNE _0xF
	SBIC 0xB,5
	RJMP _0xE
_0xF:
;     116    {
;     117    tx_buffer[tx_wr_index]=c;
	MOV  R30,R7
	LDI  R31,0
	SUBI R30,LOW(-_tx_buffer)
	SBCI R31,HIGH(-_tx_buffer)
	LD   R26,Y
	STD  Z+0,R26
;     118    if (++tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
	INC  R7
	LDI  R30,LOW(8)
	CP   R30,R7
	BRNE _0x11
	CLR  R7
;     119    ++tx_counter;
_0x11:
	INC  R9
;     120    }
;     121 else
	RJMP _0x12
_0xE:
;     122    UDR=c;
	LD   R30,Y
	OUT  0xC,R30
;     123 #asm("sei")
_0x12:
	sei
;     124 }
	RJMP _0x158
;     125 #pragma used-
;     126 #endif
;     127 
;     128 // Standard Input/Output functions
;     129 #include <stdio.h>
;     130 #include <stdlib.h>
;     131  
;     132 flash unsigned char menuline[6][20]={
;     133 " A)9200  B)14400 ",
;     134 " C)19200 D)38400 ",
;     135 " E)56000 F)57600 ",
;     136 "     G)115200    ",
;     137 " M) Manual UBRR  ",
;     138 "Z)Backligh on/off"};
;     139  
;     140 flash unsigned char  uparrow[8]={
;     141 0b0000100,
;     142 0b0001110,
;     143 0b0011111,
;     144 0b0000100,
;     145 0b0000100,
;     146 0b0000100,
;     147 0b0000100,
;     148 0b0000100};
;     149 
;     150 flash unsigned char  Downarrow[8]={
;     151 0b0000100,
;     152 0b0000100,
;     153 0b0000100,
;     154 0b0000100,
;     155 0b0000100,
;     156 0b0011111,
;     157 0b0001110,
;     158 0b0000100};
;     159 
;     160 flash unsigned char  f2[8]={
;     161 0b0011111,
;     162 0b0010000,
;     163 0b0011100,
;     164 0b0010111,
;     165 0b0010001,
;     166 0b0010010,
;     167 0b0010100,
;     168 0b0010111}; 
;     169 
;     170 flash unsigned char  f3[8]={
;     171 0b0011111,
;     172 0b0010000,
;     173 0b0011100,
;     174 0b0010111,
;     175 0b0010001,
;     176 0b0010011,
;     177 0b0010001,
;     178 0b0010111};
;     179 
;     180 
;     181 
;     182 void define_char(unsigned char flash *pc,unsigned char char_code)
;     183 {
_define_char:
;     184 unsigned int i1,a1;
;     185 a1=(char_code<<3) | 0x40;
	RCALL __SAVELOCR4
;	*pc -> Y+5
;	char_code -> Y+4
;	i1 -> R16,R17
;	a1 -> R18,R19
	LDD  R30,Y+4
	RCALL __LSLB3
	ORI  R30,0x40
	LDI  R31,0
	__PUTW1R 18,19
;     186 for (i1=0; i1<8; i1++) lcd_write_byte(a1++,*pc++);
	__GETWRN 16,17,0
_0x14:
	__CPWRN 16,17,8
	BRSH _0x15
	__GETW1R 18,19
	__ADDWRN 18,19,1
	ST   -Y,R30
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM
	MOV  R30,R0
	ST   -Y,R30
	RCALL _lcd_write_byte
;     187 }
	__ADDWRN 16,17,1
	RJMP _0x14
_0x15:
	RCALL __LOADLOCR4
	ADIW R28,7
	RET
;     188 
;     189 
;     190 
;     191 void menu (void);                     
;     192 int comunicate (void);
;     193 void cap_init(void);
;     194 void low_level_init(void);
;     195 void do_states (char);
;     196 void main(void)
;     197 
;     198 { 
_main:
;     199 
;     200 int display;
;     201 PORTD=0x00;
;	display -> R16,R17
	LDI  R30,LOW(0)
	OUT  0x12,R30
;     202 DDRD=0x20;
	LDI  R30,LOW(32)
	OUT  0x11,R30
;     203 
;     204 
;     205 
;     206      
;     207     low_level_init();
	RCALL _low_level_init
;     208     lcd_init(20);
	LDI  R30,LOW(20)
	ST   -Y,R30
	RCALL _lcd_init
;     209     _lcd_ready();
	RCALL __lcd_ready
;     210     _lcd_write_data(0xf);
	LDI  R30,LOW(15)
	ST   -Y,R30
	RCALL __lcd_write_data
;     211     
;     212     
;     213     
;     214    // init_uart();    // Initializes the UART transmit buffer
;     215     init_kb();        // Initialize keyboard reception
	RCALL _init_kb
;     216     cap_init();
	RCALL _cap_init
;     217    
;     218         
;     219     menu();
	RCALL _menu
;     220     while(1)
_0x16:
;     221     {   
;     222         
;     223              
;     224         display=1;
	__GETWRN 16,17,1
;     225         while(display==1) 
_0x19:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R16
	CPC  R31,R17
	BRNE _0x1B
;     226         {   
;     227                 
;     228                 display=comunicate();
	RCALL _comunicate
	__PUTW1R 16,17
;     229         }
	RJMP _0x19
_0x1B:
;     230         menu();
	RCALL _menu
;     231         
;     232     }
	RJMP _0x16
;     233 }
_0x1C:
	RJMP _0x1C
;     234      
;     235 void menu (void)
;     236 
;     237 { 
_menu:
;     238         //backlight=0;
;     239         char  key1,out,i,i1,i2,h1,h2,loop=0;
;     240         out=1;
	SBIW R28,2
	LDI  R24,1
	LDI  R26,LOW(0)
	LDI  R27,HIGH(0)
	LDI  R30,LOW(_0x1D*2)
	LDI  R31,HIGH(_0x1D*2)
	RCALL __INITLOCB
	RCALL __SAVELOCR6
;	key1 -> R16
;	out -> R17
;	i -> R18
;	i1 -> R19
;	i2 -> R20
;	h1 -> R21
;	h2 -> Y+7
;	loop -> Y+6
	LDI  R17,LOW(1)
;     241         i=2;
	LDI  R18,LOW(2)
;     242         define_char(uparrow,0);
	LDI  R30,LOW(_uparrow*2)
	LDI  R31,HIGH(_uparrow*2)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	RCALL _define_char
;     243         define_char(f2,1);
	LDI  R30,LOW(_f2*2)
	LDI  R31,HIGH(_f2*2)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _define_char
;     244         define_char(f3,2);
	LDI  R30,LOW(_f3*2)
	LDI  R31,HIGH(_f3*2)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _define_char
;     245         define_char(Downarrow,3);
	LDI  R30,LOW(_Downarrow*2)
	LDI  R31,HIGH(_Downarrow*2)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _define_char
;     246         
;     247 
;     248         
;     249         lcd_clear();
	RCALL _lcd_clear
;     250         lcd_putchar(0);
	LDI  R30,LOW(0)
	ST   -Y,R30
	RCALL _lcd_putchar
;     251         lcd_putsf("  WELCOME TO DANS");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     252         lcd_gotoxy(0,1);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     253         lcd_putchar(1);
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_putchar
;     254         lcd_putsf("   LCD TERMINAL");
	__POINTW1FN _0,18
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     255         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     256         lcd_putchar(2);
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_putchar
;     257         lcd_putsf( menuline[i]);      
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     258         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     259         lcd_putchar(3);
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_putchar
;     260         i++;
	SUBI R18,-1
;     261         lcd_putsf( menuline[i]);
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     262         _lcd_ready();
	RCALL __lcd_ready
;     263        _lcd_write_data(0xc);
	LDI  R30,LOW(12)
	ST   -Y,R30
	RCALL __lcd_write_data
;     264         while (out==1)
_0x1E:
	CPI  R17,1
	BREQ PC+2
	RJMP _0x20
;     265         {
;     266                 key1=getchar_kb();
	RCALL _getchar_kb
	MOV  R16,R30
;     267                 while (key1==0)
_0x21:
	CPI  R16,0
	BRNE _0x23
;     268                 {         
;     269                 key1=getchar_kb();
	RCALL _getchar_kb
	MOV  R16,R30
;     270                 }
	RJMP _0x21
_0x23:
;     271         
;     272         
;     273                 switch (key1)
	MOV  R30,R16
;     274                 {
;     275                         case 'a':
	CPI  R30,LOW(0x61)
	BRNE _0x27
;     276                         UBRR=0x2f;          // 9600bps @ 3.6864 MHz
	LDI  R30,LOW(47)
	OUT  0x9,R30
;     277                         out=0;
	LDI  R17,LOW(0)
;     278                         break;
	RJMP _0x26
;     279                 
;     280                         case 'b':
_0x27:
	CPI  R30,LOW(0x62)
	BRNE _0x28
;     281                         UBRR=0x1F;          // 14400bps @ 3.6864 MHz
	LDI  R30,LOW(31)
	OUT  0x9,R30
;     282                         out=0;
	LDI  R17,LOW(0)
;     283                         break;
	RJMP _0x26
;     284                 
;     285                         case 'c':
_0x28:
	CPI  R30,LOW(0x63)
	BRNE _0x29
;     286                         UBRR=0x17;          //19200 @ 3.6864 MHz
	LDI  R30,LOW(23)
	OUT  0x9,R30
;     287                         out=0;
	LDI  R17,LOW(0)
;     288                         break;
	RJMP _0x26
;     289                 
;     290                         case 'd':
_0x29:
	CPI  R30,LOW(0x64)
	BRNE _0x2A
;     291                         UBRR=0x0b;
	LDI  R30,LOW(11)
	OUT  0x9,R30
;     292                         out=0;
	LDI  R17,LOW(0)
;     293                         break;
	RJMP _0x26
;     294                 
;     295                         case 'e':
_0x2A:
	CPI  R30,LOW(0x65)
	BRNE _0x2B
;     296                         UBRR=0x07;
	LDI  R30,LOW(7)
	OUT  0x9,R30
;     297                         out=0;
	LDI  R17,LOW(0)
;     298                         break;
	RJMP _0x26
;     299                 
;     300                         case 'f':
_0x2B:
	CPI  R30,LOW(0x66)
	BRNE _0x2C
;     301                         UBRR=0x07;
	LDI  R30,LOW(7)
	OUT  0x9,R30
;     302                         out=0;
	LDI  R17,LOW(0)
;     303                         break;
	RJMP _0x26
;     304                 
;     305                         case 'g':
_0x2C:
	CPI  R30,LOW(0x67)
	BRNE _0x2D
;     306                         UBRR=0x03;
	LDI  R30,LOW(3)
	OUT  0x9,R30
;     307                         out=0;
	LDI  R17,LOW(0)
;     308                         break;
	RJMP _0x26
;     309                 
;     310                        
;     311                 
;     312                         case 0xff:
_0x2D:
	CPI  R30,LOW(0xFF)
	BRNE _0x2E
;     313                         key1=0;
	LDI  R16,LOW(0)
;     314                                 if (i-1>0)
	MOV  R26,R18
	SUBI R26,LOW(1)
	LDI  R30,LOW(0)
	CP   R30,R26
	BRSH _0x2F
;     315                                 {
;     316                                         i--;
	SUBI R18,1
;     317                                         i--;
	SUBI R18,1
;     318                                         lcd_gotoxy(1,2);
	LDI  R30,LOW(1)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     319                                         lcd_putsf( menuline[i]);
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     320                                         i++;
	SUBI R18,-1
;     321                                         lcd_gotoxy(1,3);
	LDI  R30,LOW(1)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     322                                         lcd_putsf( menuline[i]);
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     323                                         break;
	RJMP _0x26
;     324                                 }
;     325                                 else
_0x2F:
;     326                                  break;
	RJMP _0x26
;     327                         
;     328                                 
;     329                 
;     330                 
;     331                 
;     332                         case 0xfe:
	RJMP _0x31
_0x2E:
	CPI  R30,LOW(0xFE)
	BRNE _0x32
_0x31:
;     333                         key1=0;
	LDI  R16,LOW(0)
;     334                                 if (i<7)
	CPI  R18,7
	BRSH _0x33
;     335                                 {
;     336                                         
;     337                                         
;     338                                         lcd_gotoxy(1,2);
	LDI  R30,LOW(1)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     339                                         lcd_putsf( menuline[i]);
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     340                                         i++;
	SUBI R18,-1
;     341                                         lcd_gotoxy(1,3);
	LDI  R30,LOW(1)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     342                                         lcd_putsf( menuline[i]);
	LDI  R26,LOW(_menuline*2)
	LDI  R27,HIGH(_menuline*2)
	MOV  R30,R18
	LDI  R31,0
	PUSH R27
	PUSH R26
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     343                                         break;
	RJMP _0x26
;     344                                 }
;     345                                 else
_0x33:
;     346                                  break;
	RJMP _0x26
;     347                         
;     348                 
;     349                         break;
	RJMP _0x26
;     350                 
;     351                         case 0:
_0x32:
	CPI  R30,0
	BRNE _0x35
;     352                         out=1;
	LDI  R17,LOW(1)
;     353                         break;
	RJMP _0x26
;     354                         
;     355                         case 'm':
_0x35:
	CPI  R30,LOW(0x6D)
	BRNE _0x36
;     356                         out=2;
	LDI  R17,LOW(2)
;     357                         loop=0;
	LDI  R30,LOW(0)
	STD  Y+6,R30
;     358                         UBRR=0x17;
	LDI  R30,LOW(23)
	OUT  0x9,R30
;     359                         break;
	RJMP _0x26
;     360                                  
;     361                         case 'z':
_0x36:
	CPI  R30,LOW(0x7A)
	BRNE _0x38
;     362                        backlight=!backlight;
	CLT
	SBIS 0x12,5
	SET
	IN   R26,0x12
	BLD  R26,5
	OUT  0x12,R26
;     363                         break;
	RJMP _0x26
;     364                 
;     365                         default:
_0x38:
;     366                         out=1;
	LDI  R17,LOW(1)
;     367                         break;
;     368                 }
_0x26:
;     369         }       
	RJMP _0x1E
_0x20:
;     370         lcd_clear();
	RCALL _lcd_clear
;     371         if(out==0)
	CPI  R17,0
	BRNE _0x39
;     372         return;
	RJMP _0x159
;     373         else
_0x39:
;     374         { 
;     375                 if(out==2)
	CPI  R17,2
	BREQ PC+2
	RJMP _0x3B
;     376                 
;     377                 {
;     378                 while(loop!=1)
_0x3C:
	LDD  R26,Y+6
	CPI  R26,LOW(0x1)
	BRNE PC+2
	RJMP _0x3E
;     379                 {
;     380                         lcd_clear();
	RCALL _lcd_clear
;     381                         lcd_putchar(0);
	LDI  R30,LOW(0)
	ST   -Y,R30
	RCALL _lcd_putchar
;     382                         lcd_putsf(" PlEASE ENTER THE");
	__POINTW1FN _0,34
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     383                         lcd_gotoxy(0,1);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     384                         lcd_putchar(1);
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_putchar
;     385                         lcd_putsf("  HEX UBRR U WANT");
	__POINTW1FN _0,52
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     386                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     387                         lcd_putchar(2);      
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_putchar
;     388                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     389                         lcd_putchar(3);
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_putchar
;     390                         lcd_putsf("  UBRR=0x" );
	__POINTW1FN _0,70
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     391                         _lcd_ready();
	RCALL __lcd_ready
;     392                         _lcd_write_data(0xf);
	LDI  R30,LOW(15)
	ST   -Y,R30
	RCALL __lcd_write_data
;     393                         key1=getchar_kb();
	RCALL _getchar_kb
	MOV  R16,R30
;     394                         while (key1==0)
_0x3F:
	CPI  R16,0
	BRNE _0x41
;     395                         {         
;     396                         key1=getchar_kb();
	RCALL _getchar_kb
	MOV  R16,R30
;     397                         }
	RJMP _0x3F
_0x41:
;     398                         lcd_putchar(key1);
	ST   -Y,R16
	RCALL _lcd_putchar
;     399                         i1=key1;
	MOV  R19,R16
;     400                         key1=0;
	LDI  R16,LOW(0)
;     401                         while (key1==0)
_0x42:
	CPI  R16,0
	BRNE _0x44
;     402                         {         
;     403                         key1=getchar_kb();
	RCALL _getchar_kb
	MOV  R16,R30
;     404                         }
	RJMP _0x42
_0x44:
;     405                         lcd_putchar(key1);
	ST   -Y,R16
	RCALL _lcd_putchar
;     406                         i2=key1;
	MOV  R20,R16
;     407                         lcd_clear();
	RCALL _lcd_clear
;     408                         lcd_putchar(0);
	LDI  R30,LOW(0)
	ST   -Y,R30
	RCALL _lcd_putchar
;     409                         lcd_putsf(" YOU HAVE ENTERED");
	__POINTW1FN _0,80
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     410                         lcd_gotoxy(0,1);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     411                         lcd_putchar(1);
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_putchar
;     412                         lcd_putsf(" '0x");
	__POINTW1FN _0,98
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     413                         lcd_putchar(i1);
	ST   -Y,R19
	RCALL _lcd_putchar
;     414                         lcd_putchar(i2);
	ST   -Y,R20
	RCALL _lcd_putchar
;     415                         lcd_putsf("' PRESS ENTER");
	__POINTW1FN _0,103
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     416                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     417                         lcd_putchar(2);
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_putchar
;     418                         lcd_putsf(" TO USE THIS UBRR");      
	__POINTW1FN _0,117
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     419                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     420                         lcd_putchar(3);
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_putchar
;     421                         lcd_putsf(" OR F5 TO RE-ENTER");
	__POINTW1FN _0,135
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     422                         _lcd_ready();
	RCALL __lcd_ready
;     423                         _lcd_write_data(0xc);
	LDI  R30,LOW(12)
	ST   -Y,R30
	RCALL __lcd_write_data
;     424                         while (key1!=0xfd)
_0x45:
	CPI  R16,253
	BREQ _0x47
;     425                         {
;     426                         if(key1==0x0d)
	CPI  R16,13
	BRNE _0x48
;     427                         {       
;     428                                 h1=i1-0x30;
	MOV  R30,R19
	SUBI R30,LOW(48)
	MOV  R21,R30
;     429                                 h2=i2-0x30;
	MOV  R30,R20
	SUBI R30,LOW(48)
	STD  Y+7,R30
;     430                                 i1=h1*16;
	MOV  R30,R21
	SWAP R30
	ANDI R30,0xF0
	MOV  R19,R30
;     431                                 h1=i1+h2; 
	LDD  R30,Y+7
	ADD  R30,R19
	MOV  R21,R30
;     432                                 UBRR=0x17;
	LDI  R30,LOW(23)
	OUT  0x9,R30
;     433                                 loop=1;
	LDI  R30,LOW(1)
	STD  Y+6,R30
;     434                                 lcd_clear();
	RCALL _lcd_clear
;     435                                 i=0;
	LDI  R18,LOW(0)
;     436                                 return;
	RJMP _0x159
;     437                         }         
;     438                         key1=getchar_kb();
_0x48:
	RCALL _getchar_kb
	MOV  R16,R30
;     439                         }
	RJMP _0x45
_0x47:
;     440                         
;     441                         }
	RJMP _0x3C
_0x3E:
;     442                         
;     443                         
;     444                 }
;     445                     
;     446         }
_0x3B:
;     447         
;     448         
;     449         
;     450         
;     451         
;     452 } 
_0x159:
	RCALL __LOADLOCR6
	ADIW R28,8
	RET
;     453 
;     454 #include "gpr.h"
;     455 
;     456 void print_hexbyte(unsigned char i)
;     457 {
_print_hexbyte:
;     458     unsigned char h, l;
;     459     
;     460     h = i & 0xF0;               // High nibble
	RCALL __SAVELOCR2
;	i -> Y+2
;	h -> R16
;	l -> R17
	LDD  R30,Y+2
	ANDI R30,LOW(0xF0)
	MOV  R16,R30
;     461     h = h>>4;
	SWAP R16
	ANDI R16,0xF
;     462     h = h + '0';
	SUBI R16,-LOW(48)
;     463     
;     464     if (h > '9')
	LDI  R30,LOW(57)
	CP   R30,R16
	BRSH _0x49
;     465         h = h + 7;
	SUBI R16,-LOW(7)
;     466 
;     467     l = (i & 0x0F)+'0';         // Low nibble
_0x49:
	LDD  R30,Y+2
	ANDI R30,LOW(0xF)
	SUBI R30,-LOW(48)
	MOV  R17,R30
;     468     if (l > '9')
	LDI  R30,LOW(57)
	CP   R30,R17
	BRSH _0x4A
;     469         l = l + 7;
	SUBI R17,-LOW(7)
;     470 
;     471 
;     472     putchar(h);
_0x4A:
	ST   -Y,R16
	RCALL _putchar
;     473     putchar(l);
	ST   -Y,R17
	RCALL _putchar
;     474 }
	RCALL __LOADLOCR2
	ADIW R28,3
	RET
;     475 
;     476 
;     477 //void delay(char d)
;     478 //{
;     479 //    char i,j,k;
;     480 //    for(i=0; i<d; i++)
;     481 //        for(j=0; j<40; j++)
;     482 //            for(k=0; k<176; k++);
;     483 //}
;     484 
;     485 //#include <pgmspace.h>
;     486 #include "kb.h"
;     487 //#include "serial.h"
;     488 #include "gpr.h"
;     489 
;     490 #include "scancodes.h"
;     491 
;     492 #define BUFF_SIZE 64
;     493 
;     494 unsigned char edge, bitcount;                // 0 = neg.  1 = pos.
;     495 
;     496 unsigned char kb_buffer[BUFF_SIZE];

	.DSEG
_kb_buffer:
	.BYTE 0x40
;     497 unsigned char *inpt, *outpt;
_outpt:
	.BYTE 0x2
;     498 unsigned char buffcnt;
;     499 
;     500 
;     501 void init_kb(void)
;     502 {

	.CSEG
_init_kb:
;     503     inpt =  kb_buffer;                        // Initialize buffer
	LDI  R30,LOW(_kb_buffer)
	LDI  R31,HIGH(_kb_buffer)
	__PUTW1R 12,13
;     504     outpt = kb_buffer;
	LDI  R30,LOW(_kb_buffer)
	LDI  R31,HIGH(_kb_buffer)
	STS  _outpt,R30
	STS  _outpt+1,R31
;     505     buffcnt = 0;
	CLR  R14
;     506 
;     507     MCUCR = 2;                                // INT0 interrupt on falling edge
	LDI  R30,LOW(2)
	OUT  0x35,R30
;     508     edge = 0;                                // 0 = falling edge  1 = rising edge
	CLR  R10
;     509     bitcount = 11;
	LDI  R30,LOW(11)
	MOV  R11,R30
;     510 }
	RET
;     511 
;     512 interrupt [EXT_INT0] void ext_int0_isr(void)
;     513 {
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
;     514     static unsigned char data;                // Holds the received scan code

	.DSEG
_data_S9:
	.BYTE 0x1

	.CSEG
;     515 
;     516     if (!edge)                                // Routine entered at falling edge
	TST  R10
	BRNE _0x4B
;     517     {
;     518         if(bitcount < 11 && bitcount > 2)    // Bit 3 to 10 is data. Parity bit,
	LDI  R30,LOW(11)
	CP   R11,R30
	BRSH _0x4D
	LDI  R30,LOW(2)
	CP   R30,R11
	BRLO _0x4E
_0x4D:
	RJMP _0x4C
_0x4E:
;     519         {                                    // start and stop bits are ignored.
;     520             data = (data >> 1);
	LDS  R30,_data_S9
	LSR  R30
	STS  _data_S9,R30
;     521             if(PIND & 8)
	SBIS 0x10,3
	RJMP _0x4F
;     522                 data = data | 0x80;            // Store a '1'
	LDS  R30,_data_S9
	ORI  R30,0x80
	STS  _data_S9,R30
;     523         }
_0x4F:
;     524 
;     525         MCUCR = 3;                            // Set interrupt on rising edge
_0x4C:
	LDI  R30,LOW(3)
	OUT  0x35,R30
;     526         edge = 1;
	LDI  R30,LOW(1)
	MOV  R10,R30
;     527         
;     528     } else {                                // Routine entered at rising edge
	RJMP _0x50
_0x4B:
;     529 
;     530         MCUCR = 2;                            // Set interrupt on falling edge
	LDI  R30,LOW(2)
	OUT  0x35,R30
;     531         edge = 0;
	CLR  R10
;     532 
;     533         if(--bitcount == 0)                    // All bits received
	DEC  R11
	BRNE _0x51
;     534         {
;     535             decode(data);
	LDS  R30,_data_S9
	ST   -Y,R30
	RCALL _decode
;     536             bitcount = 11;
	LDI  R30,LOW(11)
	MOV  R11,R30
;     537         }
;     538     }
_0x51:
_0x50:
;     539 }
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
;     540 
;     541 
;     542 void decode(unsigned char sc)
;     543 {
_decode:
;     544     static unsigned char is_up=0, shift = 0, mode = 0, scrollup = 0,ctrl =0;

	.DSEG
_is_up_SA:
	.BYTE 0x1
_shift_SA:
	.BYTE 0x1
_mode_SA:
	.BYTE 0x1
_scrollup_SA:
	.BYTE 0x1
_ctrl_SA:
	.BYTE 0x1

	.CSEG
;     545     unsigned char i;
;     546 
;     547     if (!is_up)                // Last data received was the up-key identifier
	ST   -Y,R16
;	sc -> Y+1
;	i -> R16
	LDS  R30,_is_up_SA
	CPI  R30,0
	BREQ PC+2
	RJMP _0x52
;     548     {
;     549         switch (sc)
	LDD  R30,Y+1
;     550         {
;     551               
;     552                 
;     553           case 0xF0 :        // The up-key identifier
	CPI  R30,LOW(0xF0)
	BRNE _0x56
;     554             is_up = 1;
	LDI  R30,LOW(1)
	STS  _is_up_SA,R30
;     555             break;
	RJMP _0x55
;     556                    
;     557           case 0x14 :       //ctrl
_0x56:
	CPI  R30,LOW(0x14)
	BRNE _0x57
;     558             ctrl = 1;
	LDI  R30,LOW(1)
	STS  _ctrl_SA,R30
;     559             break;
	RJMP _0x55
;     560             
;     561           case 0x12 :        // Left SHIFT
_0x57:
	CPI  R30,LOW(0x12)
	BRNE _0x58
;     562             shift = 1;
	LDI  R30,LOW(1)
	STS  _shift_SA,R30
;     563             break;
	RJMP _0x55
;     564 
;     565           case 0x59 :        // Right SHIFT
_0x58:
	CPI  R30,LOW(0x59)
	BRNE _0x59
;     566             shift = 1;
	LDI  R30,LOW(1)
	STS  _shift_SA,R30
;     567             break;
	RJMP _0x55
;     568 
;     569           case 0x05 :        // F1
_0x59:
	CPI  R30,LOW(0x5)
	BRNE _0x5D
;     570             if(mode == 0)
	LDS  R30,_mode_SA
	CPI  R30,0
	BRNE _0x5B
;     571                 mode = 1;    // Enter scan code mode
	LDI  R30,LOW(1)
	STS  _mode_SA,R30
;     572             if(mode == 2)
_0x5B:
	LDS  R26,_mode_SA
	CPI  R26,LOW(0x2)
	BRNE _0x5C
;     573                 mode = 3;    // Leave scan code mode
	LDI  R30,LOW(3)
	STS  _mode_SA,R30
;     574             break;
_0x5C:
	RJMP _0x55
;     575 
;     576           default:
_0x5D:
;     577             if(mode == 0 || mode == 3)        // If ASCII mode
	LDS  R26,_mode_SA
	CPI  R26,LOW(0x0)
	BREQ _0x5F
	CPI  R26,LOW(0x3)
	BREQ _0x5F
	RJMP _0x5E
_0x5F:
;     578             {  
;     579                 if(ctrl == 1)
	LDS  R26,_ctrl_SA
	CPI  R26,LOW(0x1)
	BRNE _0x61
;     580                 {   
;     581                     if(sc == 0x21)
	LDD  R26,Y+1
	CPI  R26,LOW(0x21)
	BRNE _0x62
;     582                     putchar(0x03);
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _putchar
;     583                 }
_0x62:
;     584                 if(!shift )                    // If shift not pressed,
_0x61:
	LDS  R30,_shift_SA
	CPI  R30,0
	BREQ PC+2
	RJMP _0x63
;     585                 {                            // do a table look-up
;     586                     for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
	LDI  R16,LOW(0)
_0x65:
	LDI  R26,LOW(_unshifted*2)
	LDI  R27,HIGH(_unshifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	MOV  R26,R30
	LDD  R30,Y+1
	CP   R30,R26
	BREQ _0x67
	LDI  R26,LOW(_unshifted*2)
	LDI  R27,HIGH(_unshifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	CPI  R30,0
	BRNE _0x68
_0x67:
	RJMP _0x66
_0x68:
	SUBI R16,-1
	RJMP _0x65
_0x66:
;     587                     if (unshifted[i][0] == sc) 
	LDI  R26,LOW(_unshifted*2)
	LDI  R27,HIGH(_unshifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	MOV  R26,R30
	LDD  R30,Y+1
	CP   R30,R26
	BRNE _0x69
;     588                     {
;     589                         put_kbbuff(unshifted[i][1]);
	LDI  R26,LOW(_unshifted*2)
	LDI  R27,HIGH(_unshifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	ADIW R30,1
	LPM
	MOV  R30,R0
	ST   -Y,R30
	RCALL _put_kbbuff
;     590                     }
;     591                 } 
_0x69:
;     592                 else 
	RJMP _0x6A
_0x63:
;     593                 {   
;     594                     
;     595                                     // If shift pressed
;     596                         for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
	LDI  R16,LOW(0)
_0x6C:
	LDI  R26,LOW(_shifted*2)
	LDI  R27,HIGH(_shifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	MOV  R26,R30
	LDD  R30,Y+1
	CP   R30,R26
	BREQ _0x6E
	LDI  R26,LOW(_shifted*2)
	LDI  R27,HIGH(_shifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	CPI  R30,0
	BRNE _0x6F
_0x6E:
	RJMP _0x6D
_0x6F:
	SUBI R16,-1
	RJMP _0x6C
_0x6D:
;     597                         if (shifted[i][0] == sc) 
	LDI  R26,LOW(_shifted*2)
	LDI  R27,HIGH(_shifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	LPM
	MOV  R30,R0
	MOV  R26,R30
	LDD  R30,Y+1
	CP   R30,R26
	BRNE _0x70
;     598                             {
;     599                                 put_kbbuff(shifted[i][1]);
	LDI  R26,LOW(_shifted*2)
	LDI  R27,HIGH(_shifted*2)
	MOV  R30,R16
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	ADIW R30,1
	LPM
	MOV  R30,R0
	ST   -Y,R30
	RCALL _put_kbbuff
;     600                             }
;     601 
;     602                     
;     603                     
;     604                 }
_0x70:
_0x6A:
;     605             } else{                            // Scan code mode
	RJMP _0x71
_0x5E:
;     606                 print_hexbyte(sc);            // Print scan code
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _print_hexbyte
;     607                 put_kbbuff(' ');
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL _put_kbbuff
;     608                 put_kbbuff(' ');
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL _put_kbbuff
;     609             }
_0x71:
;     610             break;
;     611         }
_0x55:
;     612     } else {
	RJMP _0x72
_0x52:
;     613         is_up = 0;
	LDI  R30,LOW(0)
	STS  _is_up_SA,R30
;     614                                     // Two 0xF0 in a row not allowed
;     615         switch (sc)
	LDD  R30,Y+1
;     616         {
;     617         case 0x14:              //left  ctrl
	CPI  R30,LOW(0x14)
	BRNE _0x76
;     618         ctrl = 0;
	LDI  R30,LOW(0)
	STS  _ctrl_SA,R30
;     619         break;
	RJMP _0x75
;     620         
;     621           case 0x12 :                        // Left SHIFT
_0x76:
	CPI  R30,LOW(0x12)
	BRNE _0x77
;     622             shift = 0;
	LDI  R30,LOW(0)
	STS  _shift_SA,R30
;     623             break;
	RJMP _0x75
;     624             
;     625           case 0x59 :                        // Right SHIFT
_0x77:
	CPI  R30,LOW(0x59)
	BRNE _0x78
;     626             shift = 0;
	LDI  R30,LOW(0)
	STS  _shift_SA,R30
;     627             break;
	RJMP _0x75
;     628           case 0x77:
_0x78:
	CPI  R30,LOW(0x77)
	BRNE _0x79
;     629           back=!back;
	CLT
	SBIS 0x12,5
	SET
	IN   R26,0x12
	BLD  R26,5
	OUT  0x12,R26
;     630           break;
	RJMP _0x75
;     631           
;     632           case 0x05 :                        // F1
_0x79:
	CPI  R30,LOW(0x5)
	BRNE _0x7A
;     633             if(mode == 1)
	LDS  R26,_mode_SA
	CPI  R26,LOW(0x1)
	BRNE _0x7B
;     634                 mode = 2;
	LDI  R30,LOW(2)
	STS  _mode_SA,R30
;     635             if(mode == 3)
_0x7B:
	LDS  R26,_mode_SA
	CPI  R26,LOW(0x3)
	BRNE _0x7C
;     636                 mode = 0;
	LDI  R30,LOW(0)
	STS  _mode_SA,R30
;     637             break;
_0x7C:
	RJMP _0x75
;     638           case 0x06 :                        // F2
_0x7A:
	CPI  R30,LOW(0x6)
	BRNE _0x7D
;     639             put_kbbuff(0xff);
	LDI  R30,LOW(255)
	ST   -Y,R30
	RCALL _put_kbbuff
;     640            // printf("scroll up");
;     641            break;
	RJMP _0x75
;     642            case 0x04:
_0x7D:
	CPI  R30,LOW(0x4)
	BRNE _0x7E
;     643             put_kbbuff(0xfe);
	LDI  R30,LOW(254)
	ST   -Y,R30
	RCALL _put_kbbuff
;     644             break;
	RJMP _0x75
;     645             case 0x03:
_0x7E:
	CPI  R30,LOW(0x3)
	BRNE _0x75
;     646             put_kbbuff(0xfd);
	LDI  R30,LOW(253)
	ST   -Y,R30
	RCALL _put_kbbuff
;     647             break;      
;     648         } 
_0x75:
;     649     }    
_0x72:
;     650 } 
	LDD  R16,Y+0
	ADIW R28,2
	RET
;     651 
;     652 void put_kbbuff(unsigned char c)
;     653 {
_put_kbbuff:
;     654     if (buffcnt<BUFF_SIZE)                        // If buffer not full
	LDI  R30,LOW(64)
	CP   R14,R30
	BRSH _0x80
;     655     {
;     656         *inpt = c;                                // Put character into buffer
	LD   R30,Y
	__GETW2R 12,13
	ST   X,R30
;     657         inpt++;                                    // Increment pointer
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__ADDWRR 12,13,30,31
;     658 
;     659         buffcnt++;
	INC  R14
;     660 
;     661         if (inpt >= kb_buffer + BUFF_SIZE)        // Pointer wrapping
	__POINTW1MN _kb_buffer,64
	CP   R12,R30
	CPC  R13,R31
	BRLO _0x81
;     662             inpt = kb_buffer;
	LDI  R30,LOW(_kb_buffer)
	LDI  R31,HIGH(_kb_buffer)
	__PUTW1R 12,13
;     663     }
_0x81:
;     664 }
_0x80:
_0x158:
	ADIW R28,1
	RET
;     665 
;     666 int getchar_kb(void)
;     667 {
_getchar_kb:
;     668     int byte;  
;     669    
;     670     //while(buffcnt == 0);                        // Wait for data
;     671     if(buffcnt == 0)
	RCALL __SAVELOCR2
;	byte -> R16,R17
	TST  R14
	BRNE _0x82
;     672     {         
;     673     byte=0;
	__GETWRN 16,17,0
;     674     return byte;
	__GETW1R 16,17
	RJMP _0x157
;     675     }
;     676     else
_0x82:
;     677     { 
;     678     byte = *outpt;                                // Get byte
	LDS  R26,_outpt
	LDS  R27,_outpt+1
	LD   R30,X
	LDI  R31,0
	__PUTW1R 16,17
;     679     outpt++;                                    // Increment pointer
	LDS  R30,_outpt
	LDS  R31,_outpt+1
	ADIW R30,1
	STS  _outpt,R30
	STS  _outpt+1,R31
;     680 
;     681     if (outpt >= kb_buffer + BUFF_SIZE)            // Pointer wrapping
	__POINTW1MN _kb_buffer,64
	LDS  R26,_outpt
	LDS  R27,_outpt+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0x84
;     682         outpt = kb_buffer;
	LDI  R30,LOW(_kb_buffer)
	LDI  R31,HIGH(_kb_buffer)
	STS  _outpt,R30
	STS  _outpt+1,R31
;     683     
;     684     buffcnt--;                                    // Decrement buffer count
_0x84:
	DEC  R14
;     685 
;     686     return byte;
	__GETW1R 16,17
;     687     }
;     688 }
_0x157:
	RCALL __LOADLOCR2P
	RET
;     689 
;     690 
;     691 //#include <ina90.h>
;     692 #include <90s8515.h>
;     693 
;     694 void low_level_init(void)
;     695 {
_low_level_init:
;     696         
;     697         UBRR=0x2f;          // 19200bps @ 4 MHz
	LDI  R30,LOW(47)
	OUT  0x9,R30
;     698     //UCR  = 0x08;        // RX enable, TX enable
;     699     GIMSK= 0x40;        // Enable INT0 interrupt
	LDI  R30,LOW(64)
	OUT  0x3B,R30
;     700     //UCR=0x58; 
;     701     UCR=0xD8; 
	LDI  R30,LOW(216)
	OUT  0xA,R30
;     702 
;     703 #asm("sei")
	sei
;     704     //return 1;
;     705 }
	RET
;     706 
;     707 #include "capture.h"

	.DSEG
_s:
	.BYTE 0x1
_istore:
	.BYTE 0x2
_j:
	.BYTE 0x2
_b:
	.BYTE 0x2
_i:
	.BYTE 0x2
_x:
	.BYTE 0x2
_state:
	.BYTE 0x2
_jstore:
	.BYTE 0x2
_line:
	.BYTE 0x93
_lineflag:
	.BYTE 0x2
;     708 void cap_init (void)
;     709 {

	.CSEG
_cap_init:
;     710 j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;     711 i=0;
	LDI  R30,0
	STS  _i,R30
	STS  _i+1,R30
;     712 }
	RET
;     713 void do_states (char s)
;     714 {
_do_states:
;     715 
;     716            
;     717                 //if(up_BN==0)
;     718                 //////printf("it worked");            
;     719               
;     720                   
;     721                 state=bla;
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	STS  _state,R30
	STS  _state+1,R31
;     722                 if(s==0x0a )
	LD   R26,Y
	CPI  R26,LOW(0xA)
	BRNE _0x85
;     723                 state=crlf;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _state,R30
	STS  _state+1,R31
;     724                 if(s==0x08)
_0x85:
	LD   R26,Y
	CPI  R26,LOW(0x8)
	BRNE _0x86
;     725                 state=bkspce;
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	STS  _state,R30
	STS  _state+1,R31
;     726                 if(s==0x0d || s==0)
_0x86:
	LD   R26,Y
	CPI  R26,LOW(0xD)
	BREQ _0x88
	CPI  R26,LOW(0x0)
	BRNE _0x87
_0x88:
;     727                 state=ignor;
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	STS  _state,R30
	STS  _state+1,R31
;     728                 if(s==0xff)
_0x87:
	LD   R26,Y
	CPI  R26,LOW(0xFF)
	BRNE _0x8A
;     729                 state=scrollup;
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	STS  _state,R30
	STS  _state+1,R31
;     730                 if(s==0xfe)
_0x8A:
	LD   R26,Y
	CPI  R26,LOW(0xFE)
	BRNE _0x8B
;     731                 state=scrolldown;
	LDI  R30,LOW(7)
	LDI  R31,HIGH(7)
	STS  _state,R30
	STS  _state+1,R31
;     732                 if(s==0x7e)
_0x8B:
	LD   R26,Y
	CPI  R26,LOW(0x7E)
	BRNE _0x8C
;     733                 state=backlighttoggle;
	LDI  R30,0
	STS  _state,R30
	STS  _state+1,R30
;     734                 switch(state)
_0x8C:
	LDS  R30,_state
	LDS  R31,_state+1
;     735                 { 
;     736                         case backlighttoggle:
	SBIW R30,0
	BRNE _0x90
;     737                         backlight=!backlight;
	CLT
	SBIS 0x12,5
	SET
	IN   R26,0x12
	BLD  R26,5
	OUT  0x12,R26
;     738                         break;
	RJMP _0x8F
;     739                         
;     740                         case crlf:
_0x90:
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0x91
;     741                         if(wasscroll==1)
	SBRS R2,1
	RJMP _0x92
;     742                         {
;     743                                 //printf("wasscroll just got cleared by crlf");
;     744                                 j=jstore;
	LDS  R30,_jstore
	LDS  R31,_jstore+1
	STS  _j,R30
	STS  _j+1,R31
;     745                                 i=istore;
	LDS  R30,_istore
	LDS  R31,_istore+1
	STS  _i,R30
	STS  _i+1,R31
;     746                                 lcd_clear();
	RCALL _lcd_clear
;     747                                         
;     748                                         if((j-3)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,3
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x93
;     749                                         {
;     750                                                  lcd_puts(line[j-3]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x15B
;     751                                         }
;     752                                         else
_0x93:
;     753                                         {
;     754                                                  lcd_puts(line[buffersize+j-3]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x15B:
	SBIW R30,3
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     755                                         }
;     756                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     757                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x95
;     758                                         {
;     759                                                  lcd_puts(line[j-2]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x15C
;     760                                         }
;     761                                         else
_0x95:
;     762                                         {
;     763                                                  lcd_puts(line[buffersize+j-2]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x15C:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     764                                         }
;     765                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     766                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x97
;     767                                         {
;     768                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x15D
;     769                                         }
;     770                                         else
_0x97:
;     771                                         {
;     772                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x15D:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     773                                         }
;     774                                                                  
;     775                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     776                                         lcd_puts(line[j]);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     777                                         wasscroll=0;
	CLT
	BLD  R2,1
;     778                         }
;     779                         
;     780                         ////printf("CRLF\n\r");
;     781                         for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
_0x92:
	LDS  R26,_i
	LDS  R27,_i+1
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
	SUB  R30,R26
	SBC  R31,R27
	STS  _x,R30
	STS  _x+1,R31
_0x9A:
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,0
	BREQ _0x9B
;     782                         {                                                                                                                                                                                                                                                                                                
;     783                                 line[j][i]=0x20;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(32)
	ST   X,R30
;     784                                 i++;
	LDS  R30,_i
	LDS  R31,_i+1
	ADIW R30,1
	STS  _i,R30
	STS  _i+1,R31
;     785                                 ////printf("%i",i);   
;     786                         }
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,1
	STS  _x,R30
	STS  _x+1,R31
	RJMP _0x9A
_0x9B:
;     787                         i=0;
	LDI  R30,0
	STS  _i,R30
	STS  _i+1,R30
;     788                         if (lineflag==3)
	LDS  R26,_lineflag
	LDS  R27,_lineflag+1
	CPI  R26,LOW(0x3)
	LDI  R30,HIGH(0x3)
	CPC  R27,R30
	BREQ PC+2
	RJMP _0x9C
;     789                         {
;     790                                 
;     791                                 
;     792                                         lcd_clear();
	RCALL _lcd_clear
;     793                                         
;     794                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x9D
;     795                                         {
;     796                                                  lcd_puts(line[j-2]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x15E
;     797                                         }
;     798                                         else
_0x9D:
;     799                                         {
;     800                                                  lcd_puts(line[buffersize+j-2]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x15E:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     801                                         }
;     802                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     803                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x9F
;     804                                         {
;     805                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x15F
;     806                                         }
;     807                                         else
_0x9F:
;     808                                         {
;     809                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x15F:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     810                                         }
;     811                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     812                                         lcd_puts(line[j]);                         
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     813                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     814                                 ////printf("crlf line 1\n\r'%s'\n\r'                '\n\r",line[j]);
;     815                                 ////printf("line number is %i\ni is %i\n\r",j,i);
;     816                                 if((j+1)<buffersize)
	LDS  R26,_j
	LDS  R27,_j+1
	ADIW R26,1
	CPI  R26,LOW(0x7)
	LDI  R30,HIGH(0x7)
	CPC  R27,R30
	BRGE _0xA1
;     817                                 j++;
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,1
	STS  _j,R30
	STS  _j+1,R31
;     818                                 else
	RJMP _0xA2
_0xA1:
;     819                                 j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;     820                                 lcd_gotoxy(0,3);
_0xA2:
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     821                                 break;
	RJMP _0x8F
;     822                                 
;     823                         }
;     824                         else 
_0x9C:
;     825                         
;     826                         {
;     827                                 //lcd_clear();
;     828                                 ////printf("crlf line 0\n\r'%s'\n\r'                '\n\r",line[j]);
;     829                                 ////printf("line number is %i\ni is %i\n\r",j,i);
;     830                                 if((j+1)<buffersize)
	LDS  R26,_j
	LDS  R27,_j+1
	ADIW R26,1
	CPI  R26,LOW(0x7)
	LDI  R30,HIGH(0x7)
	CPC  R27,R30
	BRGE _0xA4
;     831                                 j++;
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,1
	STS  _j,R30
	STS  _j+1,R31
;     832                                 else
	RJMP _0xA5
_0xA4:
;     833                                 j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;     834                                 lineflag++;
_0xA5:
	LDS  R30,_lineflag
	LDS  R31,_lineflag+1
	ADIW R30,1
	STS  _lineflag,R30
	STS  _lineflag+1,R31
;     835                                 lcd_gotoxy(i,lineflag);
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     836                         } 
;     837                               
;     838                         break; 
	RJMP _0x8F
;     839                         
;     840                         case bla:
_0x91:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0xA6
;     841                         if(wasscroll==1)
	SBRS R2,1
	RJMP _0xA7
;     842                         {
;     843                                 //printf("wasscroll just got cleared by crlf");
;     844                                 j=jstore;
	LDS  R30,_jstore
	LDS  R31,_jstore+1
	STS  _j,R30
	STS  _j+1,R31
;     845                                 i=istore;
	LDS  R30,_istore
	LDS  R31,_istore+1
	STS  _i,R30
	STS  _i+1,R31
;     846                                 lcd_clear();
	RCALL _lcd_clear
;     847                                         
;     848                                         if((j-3)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,3
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xA8
;     849                                         {
;     850                                                  lcd_puts(line[j-3]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x160
;     851                                         }
;     852                                         else
_0xA8:
;     853                                         {
;     854                                                  lcd_puts(line[buffersize+j-3]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x160:
	SBIW R30,3
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     855                                         }
;     856                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     857                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xAA
;     858                                         {
;     859                                                  lcd_puts(line[j-2]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x161
;     860                                         }
;     861                                         else
_0xAA:
;     862                                         {
;     863                                                  lcd_puts(line[buffersize+j-2]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x161:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     864                                         }
;     865                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     866                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xAC
;     867                                         {
;     868                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x162
;     869                                         }
;     870                                         else
_0xAC:
;     871                                         {
;     872                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x162:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     873                                         }
;     874                                                                  
;     875                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     876                                         lcd_puts(line[j]);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     877                                         wasscroll=0;
	CLT
	BLD  R2,1
;     878                         }
;     879                         
;     880                         if(s<0x20)
_0xA7:
	LD   R26,Y
	CPI  R26,LOW(0x20)
	BRSH _0xAE
;     881                         break;
	RJMP _0x8F
;     882                         if(i<20)
_0xAE:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x14)
	LDI  R30,HIGH(0x14)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0xAF
;     883                         {       
;     884                                 line[j][i]=s;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R30,R26
	ADC  R31,R27
	LD   R26,Y
	STD  Z+0,R26
;     885                                 ////printf("buffer was updated j=%i i=%i s=%c\n\r",j,i,s); 
;     886                                 lcd_gotoxy(i,lineflag);
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     887                                 lcd_putchar(s);
	LD   R30,Y
	ST   -Y,R30
	RCALL _lcd_putchar
;     888                                 if(i==19)
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x13)
	LDI  R30,HIGH(0x13)
	CPC  R27,R30
	BRNE _0xB0
;     889                                 {
;     890                                 if(lineflag!=3)
	LDS  R26,_lineflag
	LDS  R27,_lineflag+1
	CPI  R26,LOW(0x3)
	LDI  R30,HIGH(0x3)
	CPC  R27,R30
	BREQ _0xB1
;     891                                 lcd_gotoxy(0,lineflag+1);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDS  R30,_lineflag
	LDS  R31,_lineflag+1
	ADIW R30,1
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     892                                 else
	RJMP _0xB2
_0xB1:
;     893                                 {
;     894                                 _lcd_ready();
	RCALL __lcd_ready
;     895                                 _lcd_write_data(0x0c);
	LDI  R30,LOW(12)
	ST   -Y,R30
	RCALL __lcd_write_data
;     896                                 }
_0xB2:
;     897                                 }
;     898                         
;     899                                 
;     900                                 ////printf("buffer contains \n\r'%s'\n\r",line[j]);
;     901                                 i++;  
_0xB0:
	LDS  R30,_i
	LDS  R31,_i+1
	ADIW R30,1
	STS  _i,R30
	STS  _i+1,R31
;     902                                 ////printf("%i",i);
;     903                         }  
;     904                         else
	RJMP _0xB3
_0xAF:
;     905                         {       
;     906                                 if(lineflag!=3)
	LDS  R26,_lineflag
	LDS  R27,_lineflag+1
	CPI  R26,LOW(0x3)
	LDI  R30,HIGH(0x3)
	CPC  R27,R30
	BREQ _0xB4
;     907                                 {
;     908                                         if((j+1)<buffersize)
	LDS  R26,_j
	LDS  R27,_j+1
	ADIW R26,1
	CPI  R26,LOW(0x7)
	LDI  R30,HIGH(0x7)
	CPC  R27,R30
	BRGE _0xB5
;     909                                             j++;
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,1
	STS  _j,R30
	STS  _j+1,R31
;     910                                         else
	RJMP _0xB6
_0xB5:
;     911                                             j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;     912                                         ////printf("line down j is %i\n\r",j);
;     913                                         
;     914                                         lineflag++;
_0xB6:
	LDS  R30,_lineflag
	LDS  R31,_lineflag+1
	ADIW R30,1
	STS  _lineflag,R30
	STS  _lineflag+1,R31
;     915                                         //printf("%i",lineflag);
;     916                                         lcd_gotoxy(0,lineflag);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     917                                         lcd_putchar(s);
	LD   R30,Y
	ST   -Y,R30
	RCALL _lcd_putchar
;     918                                         i=1; 
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _i,R30
	STS  _i+1,R31
;     919                                         line[j][0]=s;                                        
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LD   R26,Y
	STD  Z+0,R26
;     920                                         break;
	RJMP _0x8F
;     921                                 }
;     922                                 else
_0xB4:
;     923                                 {      
;     924 
;     925 
;     926                                 
;     927                                         lcd_clear();
	RCALL _lcd_clear
;     928                                         
;     929                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xB8
;     930                                         {
;     931                                                  lcd_puts(line[j-2]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x163
;     932                                         }
;     933                                         else
_0xB8:
;     934                                         {
;     935                                                  lcd_puts(line[buffersize+j-2]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x163:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     936                                         }
;     937                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     938                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xBA
;     939                                         {
;     940                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x164
;     941                                         }
;     942                                         else
_0xBA:
;     943                                         {
;     944                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x164:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     945                                         }
;     946                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     947                                         lcd_puts(line[j]);                         
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     948                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     949                                         
;     950                                         
;     951                                         ////printf("j is currently %i\n\r",j);
;     952                                         ////printf("buffern contains \n\r%s\n\r",line[j]);
;     953                                         
;     954                                         if((j+1)<buffersize)
	LDS  R26,_j
	LDS  R27,_j+1
	ADIW R26,1
	CPI  R26,LOW(0x7)
	LDI  R30,HIGH(0x7)
	CPC  R27,R30
	BRGE _0xBC
;     955                                         j++;
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,1
	STS  _j,R30
	STS  _j+1,R31
;     956                                         else
	RJMP _0xBD
_0xBC:
;     957                                         j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;     958                                         lcd_putchar(s);
_0xBD:
	LD   R30,Y
	ST   -Y,R30
	RCALL _lcd_putchar
;     959                                         line[j][0]=s;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LD   R26,Y
	STD  Z+0,R26
;     960                                         i=1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _i,R30
	STS  _i+1,R31
;     961                                         
;     962                                         ////printf("line over flow \n%s\n\r",line[j]);
;     963                                         ////printf("line number is %i\ni is %i\n\r",j,i);       
;     964                                 }
;     965                         }
_0xB3:
;     966                         
;     967                         break;
	RJMP _0x8F
;     968                         case bkspce:
_0xA6:
	CPI  R30,LOW(0x3)
	LDI  R26,HIGH(0x3)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0xBE
;     969                         if(wasscroll==1)
	SBRS R2,1
	RJMP _0xBF
;     970                         {
;     971                                 //printf("wasscroll just got cleared by crlf");
;     972                                 j=jstore;
	LDS  R30,_jstore
	LDS  R31,_jstore+1
	STS  _j,R30
	STS  _j+1,R31
;     973                                 i=istore;
	LDS  R30,_istore
	LDS  R31,_istore+1
	STS  _i,R30
	STS  _i+1,R31
;     974                                 lcd_clear();
	RCALL _lcd_clear
;     975                                         
;     976                                         if((j-3)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,3
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xC0
;     977                                         {
;     978                                                  lcd_puts(line[j-3]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x165
;     979                                         }
;     980                                         else
_0xC0:
;     981                                         {
;     982                                                  lcd_puts(line[buffersize+j-3]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x165:
	SBIW R30,3
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     983                                         }
;     984                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     985                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xC2
;     986                                         {
;     987                                                  lcd_puts(line[j-2]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x166
;     988                                         }
;     989                                         else
_0xC2:
;     990                                         {
;     991                                                  lcd_puts(line[buffersize+j-2]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x166:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     992                                         }
;     993                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;     994                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xC4
;     995                                         {
;     996                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x167
;     997                                         }
;     998                                         else
_0xC4:
;     999                                         {
;    1000                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x167:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1001                                         }
;    1002                                                                  
;    1003                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1004                                         lcd_puts(line[j]);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1005                                         wasscroll=0;
	CLT
	BLD  R2,1
;    1006                         }
;    1007                         if(i!=0) //if you are not at the start of a line
_0xBF:
	LDS  R30,_i
	LDS  R31,_i+1
	SBIW R30,0
	BREQ _0xC6
;    1008                         {      
;    1009                                 
;    1010                                 i--;
	SBIW R30,1
	STS  _i,R30
	STS  _i+1,R31
;    1011                                 
;    1012                                 lcd_gotoxy(i,lineflag);
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1013                                 lcd_putchar(0x20);
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL _lcd_putchar
;    1014                                 lcd_gotoxy(i,lineflag);
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1015                                 line[j][i]=(0x20);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(32)
	ST   X,R30
;    1016                         }
;    1017                         else
	RJMP _0xC7
_0xC6:
;    1018                         {     printf("%i",lineflag);          
	__POINTW1FN _0,154
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_lineflag
	LDS  R31,_lineflag+1
	RCALL __CWD1
	RCALL __PUTPARD1
	LDI  R24,4
	RCALL _printf
	ADIW R28,6
;    1019                                         if(lineflag!=0)
	LDS  R30,_lineflag
	LDS  R31,_lineflag+1
	SBIW R30,0
	BREQ _0xC8
;    1020                                         lineflag--;
	SBIW R30,1
	STS  _lineflag,R30
	STS  _lineflag+1,R31
;    1021                                         else
	RJMP _0xC9
_0xC8:
;    1022                                         {
;    1023                                                 if(j>0)
	LDS  R26,_j
	LDS  R27,_j+1
	RCALL __CPW02
	BRGE _0xCA
;    1024                                                 j--;
	LDS  R30,_j
	LDS  R31,_j+1
	SBIW R30,1
	RJMP _0x168
;    1025                                                 else
_0xCA:
;    1026                                                 j=buffersize-1;
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
_0x168:
	STS  _j,R30
	STS  _j+1,R31
;    1027                                                 lcd_puts(line[j]);
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1028                                         }
_0xC9:
;    1029                                         i=19;
	LDI  R30,LOW(19)
	LDI  R31,HIGH(19)
	STS  _i,R30
	STS  _i+1,R31
;    1030                                         lcd_gotoxy(i,lineflag);
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1031                                         lcd_putchar(0x20);
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL _lcd_putchar
;    1032                                         line[j][i]=0x20;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(32)
	ST   X,R30
;    1033                                         lcd_gotoxy(i,lineflag);      
	LDS  R30,_i
	ST   -Y,R30
	LDS  R30,_lineflag
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1034                                         if(j>0)
	LDS  R26,_j
	LDS  R27,_j+1
	RCALL __CPW02
	BRGE _0xCC
;    1035                                         j--;
	LDS  R30,_j
	LDS  R31,_j+1
	SBIW R30,1
	RJMP _0x169
;    1036                                         else
_0xCC:
;    1037                                         j=buffersize-1;
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
_0x169:
	STS  _j,R30
	STS  _j+1,R31
;    1038                                         
;    1039                                         
;    1040                                         
;    1041                                 
;    1042                                 
;    1043                         }
_0xC7:
;    1044                         
;    1045                                 break;
	RJMP _0x8F
;    1046 
;    1047                         
;    1048                         case ignor:
_0xBE:
	CPI  R30,LOW(0x5)
	LDI  R26,HIGH(0x5)
	CPC  R31,R26
	BRNE _0xCE
;    1049                         break;
	RJMP _0x8F
;    1050  
;    1051                         case scrolldown :
_0xCE:
	CPI  R30,LOW(0x7)
	LDI  R26,HIGH(0x7)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0xCF
;    1052                         
;    1053                         
;    1054                                 if (wasscroll==0)
	SBRC R2,1
	RJMP _0xD0
;    1055                                 { 
;    1056                                 //printf("wasscroll just got set");
;    1057                                       wasscroll=1;      
	SET
	BLD  R2,1
;    1058                                 
;    1059                                         istore=i;
	LDS  R30,_i
	LDS  R31,_i+1
	STS  _istore,R30
	STS  _istore+1,R31
;    1060                                         jstore=j; 
	LDS  R30,_j
	LDS  R31,_j+1
	STS  _jstore,R30
	STS  _jstore+1,R31
;    1061                                         for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
	LDS  R26,_i
	LDS  R27,_i+1
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
	SUB  R30,R26
	SBC  R31,R27
	STS  _x,R30
	STS  _x+1,R31
_0xD2:
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,0
	BREQ _0xD3
;    1062                         {                                                                                                                                                                                                                                                                                                
;    1063                                 line[j][i]=0x20;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(32)
	ST   X,R30
;    1064                                 i++;
	LDS  R30,_i
	LDS  R31,_i+1
	ADIW R30,1
	STS  _i,R30
	STS  _i+1,R31
;    1065                                 ////printf("%i",i);   
;    1066                         }
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,1
	STS  _x,R30
	STS  _x+1,R31
	RJMP _0xD2
_0xD3:
;    1067 
;    1068                                 }
;    1069                                         
;    1070                                 
;    1071                             if((j+1)<buffersize)
_0xD0:
	LDS  R26,_j
	LDS  R27,_j+1
	ADIW R26,1
	CPI  R26,LOW(0x7)
	LDI  R30,HIGH(0x7)
	CPC  R27,R30
	BRGE _0xD4
;    1072                                         j++;
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,1
	STS  _j,R30
	STS  _j+1,R31
;    1073                             else
	RJMP _0xD5
_0xD4:
;    1074                                         j=0;
	LDI  R30,0
	STS  _j,R30
	STS  _j+1,R30
;    1075                              lcd_clear();
_0xD5:
	RCALL _lcd_clear
;    1076                              
;    1077                                 //printf("\n\r%s \n\r",line[j-1]);
;    1078                                        lcd_clear();
	RCALL _lcd_clear
;    1079                                         
;    1080                                         if((j-3)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,3
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xD6
;    1081                                         {
;    1082                                                  lcd_puts(line[j-3]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x16A
;    1083                                         }
;    1084                                         else
_0xD6:
;    1085                                         {
;    1086                                                  lcd_puts(line[buffersize+j-3]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x16A:
	SBIW R30,3
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1087                                         }
;    1088                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1089                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xD8
;    1090                                         {
;    1091                                                  lcd_puts(line[j-2]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x16B
;    1092                                         }
;    1093                                         else
_0xD8:
;    1094                                         {
;    1095                                                  lcd_puts(line[buffersize+j-2]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x16B:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1096                                         }
;    1097                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1098                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xDA
;    1099                                         {
;    1100                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x16C
;    1101                                         }
;    1102                                         else
_0xDA:
;    1103                                         {
;    1104                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x16C:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1105                                         }
;    1106                                                                  
;    1107                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1108                                         lcd_puts(line[j]);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1109                              //printf("%s \n\r",line[j]);
;    1110                              //printf("j is %i\n\r",j);
;    1111                              break;
	RJMP _0x8F
;    1112                              
;    1113                              
;    1114                              case scrollup:
_0xCF:
	CPI  R30,LOW(0x6)
	LDI  R26,HIGH(0x6)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0xE9
;    1115                         
;    1116                         
;    1117                         
;    1118                                 if (wasscroll==0)
	SBRC R2,1
	RJMP _0xDD
;    1119                                 { 
;    1120                                 //printf("wasscroll just got set");
;    1121                                       wasscroll=1;      
	SET
	BLD  R2,1
;    1122                                 
;    1123                                         istore=i;
	LDS  R30,_i
	LDS  R31,_i+1
	STS  _istore,R30
	STS  _istore+1,R31
;    1124                                         jstore=j;
	LDS  R30,_j
	LDS  R31,_j+1
	STS  _jstore,R30
	STS  _jstore+1,R31
;    1125                                         for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
	LDS  R26,_i
	LDS  R27,_i+1
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
	SUB  R30,R26
	SBC  R31,R27
	STS  _x,R30
	STS  _x+1,R31
_0xDF:
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,0
	BREQ _0xE0
;    1126                         {                                                                                                                                                                                                                                                                                                
;    1127                                 line[j][i]=0x20;
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	LDS  R26,_i
	LDS  R27,_i+1
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(32)
	ST   X,R30
;    1128                                 i++;
	LDS  R30,_i
	LDS  R31,_i+1
	ADIW R30,1
	STS  _i,R30
	STS  _i+1,R31
;    1129                                 ////printf("%i",i);   
;    1130                         }
	LDS  R30,_x
	LDS  R31,_x+1
	SBIW R30,1
	STS  _x,R30
	STS  _x+1,R31
	RJMP _0xDF
_0xE0:
;    1131                                 }
;    1132                                         
;    1133                                 
;    1134                              if(j>0)   
_0xDD:
	LDS  R26,_j
	LDS  R27,_j+1
	RCALL __CPW02
	BRGE _0xE1
;    1135                                     j--;
	LDS  R30,_j
	LDS  R31,_j+1
	SBIW R30,1
	RJMP _0x16D
;    1136                              else
_0xE1:
;    1137                                    j=buffersize-1;
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
_0x16D:
	STS  _j,R30
	STS  _j+1,R31
;    1138                                     
;    1139                              lcd_clear();
	RCALL _lcd_clear
;    1140                                         
;    1141                                         if((j-3)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,3
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xE3
;    1142                                         {
;    1143                                                  lcd_puts(line[j-3]);      
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x16E
;    1144                                         }
;    1145                                         else
_0xE3:
;    1146                                         {
;    1147                                                  lcd_puts(line[buffersize+j-3]);                                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x16E:
	SBIW R30,3
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1148                                         }
;    1149                                         lcd_gotoxy(0,1);               
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1150                                         if((j-2)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,2
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xE5
;    1151                                         {
;    1152                                                  lcd_puts(line[j-2]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x16F
;    1153                                         }
;    1154                                         else
_0xE5:
;    1155                                         {
;    1156                                                  lcd_puts(line[buffersize+j-2]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x16F:
	SBIW R30,2
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1157                                         }
;    1158                                         lcd_gotoxy(0,2);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1159                                         if((j-1)>-1)
	LDS  R26,_j
	LDS  R27,_j+1
	SBIW R26,1
	LDI  R30,LOW(65535)
	LDI  R31,HIGH(65535)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xE7
;    1160                                         {
;    1161                                                  lcd_puts(line[j-1]);
	LDS  R30,_j
	LDS  R31,_j+1
	RJMP _0x170
;    1162                                         }
;    1163                                         else
_0xE7:
;    1164                                         {
;    1165                                                  lcd_puts(line[buffersize+j-1]);                               
	LDS  R30,_j
	LDS  R31,_j+1
	ADIW R30,7
_0x170:
	SBIW R30,1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1166                                         }
;    1167                                                                  
;    1168                                         lcd_gotoxy(0,3);
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _lcd_gotoxy
;    1169                                         lcd_puts(line[j]);
	LDS  R30,_j
	LDS  R31,_j+1
	LDI  R26,LOW(21)
	LDI  R27,HIGH(21)
	RCALL __MULW12U
	SUBI R30,LOW(-_line)
	SBCI R31,HIGH(-_line)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;    1170                              //printf("%s \n\r",line[j]);
;    1171                              //printf("j is %i\n\r",j);
;    1172                              break;        
;    1173                         default:
_0xE9:
;    1174                         break;
;    1175                 }
_0x8F:
;    1176                         
;    1177              
;    1178       
;    1179       
;    1180 
;    1181       
;    1182 }
	ADIW R28,1
	RET
;    1183  unsigned char key;

	.DSEG
_key:
	.BYTE 0x1
;    1184 int comunicate (void)
;    1185 
;    1186 {

	.CSEG
_comunicate:
;    1187        key=getchar_kb();
	RCALL _getchar_kb
	STS  _key,R30
;    1188        if(key==0xfd)
	LDS  R26,_key
	CPI  R26,LOW(0xFD)
	BRNE _0xEA
;    1189        return 0;
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	RET
;    1190        else
_0xEA:
;    1191        
;    1192        if(key==0xff || key == 0xfe)
	LDS  R26,_key
	CPI  R26,LOW(0xFF)
	BREQ _0xED
	CPI  R26,LOW(0xFE)
	BRNE _0xEC
_0xED:
;    1193         do_states(key);
	LDS  R30,_key
	ST   -Y,R30
	RCALL _do_states
;    1194        else
	RJMP _0xEF
_0xEC:
;    1195        {
;    1196        if(key!=0 )
	LDS  R30,_key
	CPI  R30,0
	BREQ _0xF0
;    1197        {
;    1198         //putchar(key);
;    1199         //key=getchar();
;    1200         if(key==13)
	LDS  R26,_key
	CPI  R26,LOW(0xD)
	BRNE _0xF1
;    1201         {
;    1202                 //putchar(13);
;    1203                 putchar(10);
	LDI  R30,LOW(10)
	RJMP _0x171
;    1204         }
;    1205         else 
_0xF1:
;    1206         
;    1207         putchar(key);       
	LDS  R30,_key
_0x171:
	ST   -Y,R30
	RCALL _putchar
;    1208         //do_states(key);
;    1209        }
;    1210        } 
_0xF0:
_0xEF:
;    1211        //key=getchar();
;    1212        //if(key!=1)
;    1213        //{
;    1214        
;    1215        key=getchar();
	RCALL _getchar
	STS  _key,R30
;    1216        if(key==0x1b)
	LDS  R26,_key
	CPI  R26,LOW(0x1B)
	BREQ _0xF4
;    1217               {        
;    1218                 //while (key!=0x6d)
;    1219                 //{
;    1220                   //      key=getchar();
;    1221                 //}
;    1222        }
;    1223        else
;    1224        {
;    1225                if(key!=0 && key!=0xff && key!=0x0fe)
	CPI  R26,LOW(0x0)
	BREQ _0xF6
	CPI  R26,LOW(0xFF)
	BREQ _0xF6
	CPI  R26,LOW(0xFE)
	BRNE _0xF7
_0xF6:
	RJMP _0xF5
_0xF7:
;    1226                //printf("%c",key);
;    1227                {
;    1228                do_states(key);
	LDS  R30,_key
	ST   -Y,R30
	RCALL _do_states
;    1229                }
;    1230        }  
_0xF5:
_0xF4:
;    1231        
;    1232        return 1;
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	RET
;    1233     }

    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G7:
	.BYTE 0x4
__lcd_x:
	.BYTE 0x1
__lcd_y:
	.BYTE 0x1
__lcd_maxx:
	.BYTE 0x1

	.CSEG
__lcd_delay_G7:
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
	RCALL __lcd_delay_G7
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G7
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G7
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G7
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G7:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G7
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G7
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G7
    ld    r26,y
    swap  r26
	RCALL __lcd_write_nibble_G7
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_write_byte:
	RCALL __lcd_ready
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __lcd_ready
    sbi   __lcd_port,__lcd_rs     ;RS=1
	LD   R30,Y
	ST   -Y,R30
	RCALL __lcd_write_data
	ADIW R28,2
	RET
__lcd_read_nibble_G7:
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G7
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G7
    andi  r30,0xf0
	RET
_lcd_read_byte0_G7:
	RCALL __lcd_delay_G7
	RCALL __lcd_read_nibble_G7
    mov   r26,r30
	RCALL __lcd_read_nibble_G7
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	RCALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G7)
	SBCI R31,HIGH(-__base_y_G7)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	ST   -Y,R30
	RCALL __lcd_write_data
	LDD  R30,Y+1
	STS  __lcd_x,R30
	LD   R30,Y
	STS  __lcd_y,R30
	ADIW R28,2
	RET
_lcd_clear:
	RCALL __lcd_ready
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __lcd_ready
	LDI  R30,LOW(15)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __lcd_ready
	LDI  R30,LOW(1)
	ST   -Y,R30
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
	BRSH _0xF9
	__lcd_putchar1:
	LDS  R30,__lcd_y
	SUBI R30,-LOW(1)
	STS  __lcd_y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	LDS  R30,__lcd_y
	ST   -Y,R30
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0xF9:
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
__long_delay_G7:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G7:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G7
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LD   R30,Y
	STS  __lcd_maxx,R30
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G7,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G7,3
	RCALL __long_delay_G7
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G7
	RCALL __long_delay_G7
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G7
	RCALL __long_delay_G7
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G7
	RCALL __long_delay_G7
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL __lcd_init_write_G7
	RCALL __long_delay_G7
	LDI  R30,LOW(40)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G7
	LDI  R30,LOW(4)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G7
	LDI  R30,LOW(133)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G7
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RCALL _lcd_read_byte0_G7
	CPI  R30,LOW(0x5)
	BREQ _0xFA
	LDI  R30,LOW(0)
	RJMP _0x156
_0xFA:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x156:
	ADIW R28,1
	RET
__put_G8:
	put:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	SBIW R30,0
	BREQ _0xFB
	RCALL __GETW1P
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0xFC
_0xFB:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0xFC:
	ADIW R28,3
	RET
__print_G8:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0xFD:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	SBIW R30,1
	LPM
	MOV  R30,R0
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0xFF
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x103
	CPI  R19,37
	BRNE _0x104
	LDI  R16,LOW(1)
	RJMP _0x105
_0x104:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
_0x105:
	RJMP _0x102
_0x103:
	CPI  R30,LOW(0x1)
	BRNE _0x106
	CPI  R19,37
	BRNE _0x107
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	LDI  R16,LOW(0)
	RJMP _0x102
_0x107:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x108
	LDI  R17,LOW(1)
	RJMP _0x102
_0x108:
	CPI  R19,43
	BRNE _0x109
	LDI  R21,LOW(43)
	RJMP _0x102
_0x109:
	CPI  R19,32
	BRNE _0x10A
	LDI  R21,LOW(32)
	RJMP _0x102
_0x10A:
	RJMP _0x10B
_0x106:
	CPI  R30,LOW(0x2)
	BRNE _0x10C
_0x10B:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x10D
	ORI  R17,LOW(128)
	RJMP _0x102
_0x10D:
	RJMP _0x10E
_0x10C:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x102
_0x10E:
	CPI  R19,48
	BRLO _0x111
	CPI  R19,58
	BRLO _0x112
_0x111:
	RJMP _0x110
_0x112:
	MOV  R18,R20
	LSL  R20
	LSL  R20
	ADD  R20,R18
	LSL  R20
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x102
_0x110:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x116
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
	RCALL __put_G8
	RJMP _0x117
_0x116:
	CPI  R30,LOW(0x73)
	BRNE _0x119
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x11A
_0x119:
	CPI  R30,LOW(0x70)
	BRNE _0x11C
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x11A:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x11D
_0x11C:
	CPI  R30,LOW(0x64)
	BREQ _0x120
	CPI  R30,LOW(0x69)
	BRNE _0x121
_0x120:
	ORI  R17,LOW(4)
	RJMP _0x122
_0x121:
	CPI  R30,LOW(0x75)
	BRNE _0x123
_0x122:
	LDI  R30,LOW(_tbl10_G8*2)
	LDI  R31,HIGH(_tbl10_G8*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x124
_0x123:
	CPI  R30,LOW(0x58)
	BRNE _0x126
	ORI  R17,LOW(8)
	RJMP _0x127
_0x126:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x155
_0x127:
	LDI  R30,LOW(_tbl16_G8*2)
	LDI  R31,HIGH(_tbl16_G8*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x124:
	SBRS R17,2
	RJMP _0x129
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x12A
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x12A:
	CPI  R21,0
	BREQ _0x12B
	SUBI R16,-LOW(1)
	RJMP _0x12C
_0x12B:
	ANDI R17,LOW(251)
_0x12C:
	RJMP _0x12D
_0x129:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0x12D:
_0x11D:
	SBRC R17,0
	RJMP _0x12E
_0x12F:
	CP   R16,R20
	BRSH _0x131
	SBRS R17,7
	RJMP _0x132
	SBRS R17,2
	RJMP _0x133
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x134
_0x133:
	LDI  R19,LOW(48)
_0x134:
	RJMP _0x135
_0x132:
	LDI  R19,LOW(32)
_0x135:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	SUBI R20,LOW(1)
	RJMP _0x12F
_0x131:
_0x12E:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x136
_0x137:
	CPI  R18,0
	BREQ _0x139
	SBRS R17,3
	RJMP _0x13A
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM
	MOV  R30,R0
	RJMP _0x172
_0x13A:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x172:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	CPI  R20,0
	BREQ _0x13C
	SUBI R20,LOW(1)
_0x13C:
	SUBI R18,LOW(1)
	RJMP _0x137
_0x139:
	RJMP _0x13D
_0x136:
_0x13F:
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
	BRSH _0x141
	SBRS R17,3
	RJMP _0x142
	SUBI R19,-LOW(7)
	RJMP _0x143
_0x142:
	SUBI R19,-LOW(39)
_0x143:
_0x141:
	SBRC R17,4
	RJMP _0x145
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x147
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x146
_0x147:
	ORI  R17,LOW(16)
	RJMP _0x149
_0x146:
	CP   R20,R18
	BRLO _0x14B
	SBRS R17,0
	RJMP _0x14C
_0x14B:
	RJMP _0x14A
_0x14C:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x14D
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x149:
	SBRS R17,2
	RJMP _0x14E
	ANDI R17,LOW(251)
	ST   -Y,R21
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	CPI  R20,0
	BREQ _0x14F
	SUBI R20,LOW(1)
_0x14F:
_0x14E:
_0x14D:
_0x145:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	CPI  R20,0
	BREQ _0x150
	SUBI R20,LOW(1)
_0x150:
_0x14A:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x140
	RJMP _0x13F
_0x140:
_0x13D:
	SBRS R17,0
	RJMP _0x151
_0x152:
	CPI  R20,0
	BREQ _0x154
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G8
	RJMP _0x152
_0x154:
_0x151:
_0x155:
_0x117:
	LDI  R16,LOW(0)
_0x102:
	RJMP _0xFD
_0xFF:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	RCALL __SAVELOCR2
	MOV  R26,R28
	MOV  R27,R29
	RCALL __ADDW2R15
	__PUTW2R 16,17
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
	MOV  R26,R28
	MOV  R27,R29
	ADIW R26,4
	RCALL __ADDW2R15
	RCALL __GETW1P
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	ST   -Y,R16
	MOV  R30,R28
	MOV  R31,R29
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G8
	RCALL __LOADLOCR2
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
	lpm 
	tst  r0
	breq __strlenf1
	adiw r26,1
	adiw r30,1
	rjmp __strlenf0
__strlenf1:
	mov  r30,r26
	mov  r31,r27
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

__MULW12U:
	MOV  R0,R26
	MOV  R1,R27
	LDI  R24,17
	CLR  R26
	SUB  R27,R27
	RJMP __MULW12U1
__MULW12U3:
	BRCC __MULW12U2
	ADD  R26,R0
	ADC  R27,R1
__MULW12U2:
	LSR  R27
	ROR  R26
__MULW12U1:
	ROR  R31
	ROR  R30
	DEC  R24
	BRNE __MULW12U3
	RET

__GETW1P:
	LD   R30,X+
	LD   R31,X
	SBIW R26,1
	RET

__GETW1PF:
	LPM
	ADIW R30,1
	MOV  R1,R0
	LPM 
	MOV  R31,R0
	MOV  R30,R1
	RET

__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
	RET

__CPW02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
	RET

__LSLB3:
	LSL  R30
	LSL  R30
	LSL  R30
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

__LOADLOCR2P:
	LD   R16,Y+
	LD   R17,Y+
	RET

__INITLOCB:
__INITLOCW:
	ADD R26,R28
	ADC R27,R29
__INITLOC0:
	LPM
	ADIW R30,1
	ST   X+,R0
	DEC  R24
	BRNE __INITLOC0
	RET

;END OF CODE MARKER
__END_OF_CODE:
