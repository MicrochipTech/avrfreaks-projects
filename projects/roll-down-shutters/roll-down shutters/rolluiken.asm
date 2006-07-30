;CodeVisionAVR C Compiler V1.24.4 Standard
;(C) Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;e-mail:office@hpinfotech.com

;Chip type           : AT90S8515
;Clock frequency     : 4,000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;(s)scanf features   : int, width
;External SRAM size  : 0
;Data Stack size     : 55 byte(s)
;Heap size           : 0 byte(s)
;Promote char to int : No
;char is unsigned    : Yes
;8 bit enums         : Yes
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

	.INCLUDE "rolluiken.vec"
	.INCLUDE "rolluiken.inc"

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
	LDI  R28,LOW(0x97)
	LDI  R29,HIGH(0x97)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x97
;       1 /*********************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.0 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2003 HP InfoTech s.r.l.
;       6 http://www.hpinfotech.ro
;       7 e-mail:office@hpinfotech.ro
;       8 
;       9 Project :roll-down shutters
;      10 Version :3.7c : 
;      11 Date    : 09/01/2002
;      12 Author  : Johan Cardoen                   
;      13 Company : 8920 Poelkapelle (BELGIUM)      
;      14 Comments: with LCD support on PORT C                                                        12/05/2003
;      15           now with powerdown function incorporated                                          28/11/2003
;      16           with Version 1.24.0 automatic detection of LCD on startup (new lcd_init function) 24/12/2003
;      17           relais drivers with  PCF8574 & ULN2803                                            26/12/2003
;      18 
;      19 Chip type           : AT90S8515
;      20 Clock frequency     : 4,000000 MHz
;      21 Memory model        : Small
;      22 External SRAM size  : 0
;      23 Data Stack size     : 128
;      24 *********************************************/
;      25 
;      26 #include <90s8515.h> 
;      27 #include <delay.h>
;      28 #include <sleep.h> 
;      29 #include <lcd.h>   
;      30 #include <i2c.h>
;      31 #include <stdio.h>
;      32 
;      33 // Alphanumeric LCD Module functions
;      34 #asm
;      35    .equ __lcd_port=0x15
   .equ __lcd_port=0x15
;      36 #endasm
;      37 
;      38 
;      39 
;      40 // I2C Bus functions
;      41 #asm
;      42    .equ __i2c_port=0x12
   .equ __i2c_port=0x12
;      43    .equ __sda_bit=1
   .equ __sda_bit=1
;      44    .equ __scl_bit=3
   .equ __scl_bit=3
;      45 #endasm
;      46 
;      47 
;      48 #define lowbyte(int)                     (unsigned char) (*((char*)&(int)+0))
;      49 #define highbyte(int)                    (unsigned char) (*((char*)&(int)+1))     
;      50 
;      51 #define stop(x)                          (!(rolluik[x].status_reg & 0x03))
;      52 #define up(x)                            (rolluik[x].status_reg & 0x01)
;      53 #define down(x)                          (rolluik[x].status_reg & 0x02)
;      54 #define previous_state_up(x)             (rolluik[x].status_reg & 0x04)
;      55 #define previous_state_down(x)           (rolluik[x].status_reg & 0x08)
;      56 #define blocked(x)                       (rolluik[x].status_reg & 0x10)
;      57 #define reverse(x)                       (rolluik[x].status_reg & 0x20)
;      58 #define going_up(x)                      (rolluik[x].status_reg & 0x40)
;      59 #define going_down(x)                    (rolluik[x].status_reg & 0x80)
;      60 #define set_stop(x)                      (rolluik[x].status_reg & 0x3c)
;      61 #define set_up(x)                        ((rolluik[x].status_reg & 0xf0)| 0x05)
;      62 #define set_down(x)                      ((rolluik[x].status_reg & 0xf0)| 0x0a)
;      63 #define set_blocked(x)                   ((rolluik[x].status_reg & 0x0c)| 0x10)
;      64 #define reset_blocked(x)                 (rolluik[x].status_reg & 0xef)
;      65 #define set_reverse(x)                   ((rolluik[x].status_reg & 0xdf)| 0x20)
;      66 #define reset_reverse(x)                 (rolluik[x].status_reg & 0xdf)
;      67 #define set_going_up(x)                  ((rolluik[x].status_reg & 0x3f)| 0x40) 
;      68 #define set_going_down(x)                ((rolluik[x].status_reg & 0x3f)| 0x80)
;      69                                    
;      70 #define nr_of_shutters                   16         // number of implemented roll-down shutters
;      71 
;      72 #define PCF8574_1     0b01110010       // adress of PCF8574P managing the outputs 1 to 4
;      73 #define PCF8574_2     0b01111110       // adress of PCF8574P managing the outputs 4 to 8
;      74 #define PCF8574_3     0b01110100       // adress of PCF8574P managing the outputs 8 to 12
;      75 #define PCF8574_4     0b01111000       // adress of PCF8574P managing the outputs 12 to 16 
;      76 #define PCF8574_5     0b01110110       // adress of PCF8574P managing the outputs 16 to 20
;      77 #define PCF8574_6     0b01111010       // adress of PCF8574P managing the outputs 20 to 24
;      78 
;      79 #define PCF8574A_1     0b01001110       // adress of PCF8574AP managing the inputs 1 to 8
;      80 #define PCF8574A_2     0b01001100       // adress of PCF8574AP managing the inputs 8 to 16
;      81 #define PCF8574A_3     0b01000100       // adress of PCF8574AP managing the inputs 16 to 24    
;      82 
;      83 #define LCD_ATTACHED
;      84 
;      85 // Declare your global variables here 
;      86 
;      87   
;      88 typedef unsigned char byte;
;      89 
;      90 // table for the user defined characters
;      91 
;      92 eeprom byte char0[8]={

	.ESEG
_char0:
;      93 0b0011111,
;      94 0b0010001,
;      95 0b0010001,
;      96 0b0010001,             // char code for "open window"
;      97 0b0010001,
;      98 0b0010001,
;      99 0b0010001,
;     100 0b0011111};     
	.DB  0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0x1F
;     101 
;     102 eeprom byte char1[8]={
_char1:
;     103 0b0011111,
;     104 0b0011111,
;     105 0b0011111,
;     106 0b0011111,            // char code for "closed window"
;     107 0b0011111,
;     108 0b0011111,
;     109 0b0011111,
;     110 0b0011111};
	.DB  0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F
;     111 
;     112 eeprom byte char2[8]={
_char2:
;     113 0b0011111,
;     114 0b0011111,
;     115 0b0011111,
;     116 0b0011111,            // char code for "half-open window"
;     117 0b0010001,
;     118 0b0010001,
;     119 0b0010001,
;     120 0b0011111}; 
	.DB  0x1F,0x1F,0x1F,0x1F,0x11,0x11,0x11,0x1F
;     121 
;     122 eeprom byte char3[8]={
_char3:
;     123 0b0000100,
;     124 0b0001110,
;     125 0b0011111,
;     126 0b0000100,            // char code for "going up"
;     127 0b0000100,
;     128 0b0000100,
;     129 0b0000100,
;     130 0b0000100};
	.DB  0x4,0xE,0x1F,0x4,0x4,0x4,0x4,0x4
;     131 
;     132 eeprom byte char4[8]={
_char4:
;     133 0b0000100,
;     134 0b0000100,
;     135 0b0000100,
;     136 0b0000100,            // char code for "going down"
;     137 0b0000100,
;     138 0b0011111,
;     139 0b0001110,
;     140 0b0000100}; 
	.DB  0x4,0x4,0x4,0x4,0x4,0x1F,0xE,0x4
;     141 
;     142 eeprom byte char5[8]={
_char5:
;     143 0b0000100,
;     144 0b0001110,
;     145 0b0011111,
;     146 0b0000000,            // char code for "reverse"
;     147 0b0000000,
;     148 0b0011111,
;     149 0b0001110,
;     150 0b0000100}; 
	.DB  0x4,0xE,0x1F,0x0,0x0,0x1F,0xE,0x4
;     151 
;     152 eeprom byte char6[8]={
_char6:
;     153 0b0010001,
;     154 0b0010001,
;     155 0b0001010,
;     156 0b0000100,            // char code for "blocked"
;     157 0b0001010,
;     158 0b0010001,
;     159 0b0010001,
;     160 0b0010001};
	.DB  0x11,0x11,0xA,0x4,0xA,0x11,0x11,0x11
;     161 
;     162 //function used to define user characters
;     163 
;     164 void define_char ( byte eeprom *pc,byte char_code)
;     165 {

	.CSEG
_define_char:
;     166  byte i,a;
;     167  a=(char_code<<3) | 0x40;
	RCALL __SAVELOCR2
;	*pc -> Y+3
;	char_code -> Y+2
;	i -> R16
;	a -> R17
	LDD  R30,Y+2
	RCALL __LSLB3
	ORI  R30,0x40
	MOV  R17,R30
;     168  for (i=0; i<8; i++) lcd_write_byte(a++,*pc++);
	LDI  R16,LOW(0)
_0x4:
	CPI  R16,8
	BRSH _0x5
	ST   -Y,R17
	INC  R17
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	ADIW R26,1
	STD  Y+4,R26
	STD  Y+4+1,R27
	SBIW R26,1
	RCALL __EEPROMRDB
	ST   -Y,R30
	RCALL _lcd_write_byte
;     169 }
	SUBI R16,-1
	RJMP _0x4
_0x5:
	RCALL __LOADLOCR2
	ADIW R28,5
	RET
;     170 
;     171 
;     172 
;     173 const char message1[]="ROLLUIKMODULE";  
;     174 const char message2[]="XF 16";
;     175 const char message3[]="version 3.7c";    
;     176 const char error_message[]=" Internal Error";   
;     177 char reversetimer,aux,nr,k,x,y,z,output_changed; 
;     178 unsigned char lcd_attached;
;     179 unsigned long int edgemarker_1,edgemarker_2,rising_edge_mask,falling_edge_mask,debounced_input;  

	.DSEG
_edgemarker_1:
	.BYTE 0x4
_edgemarker_2:
	.BYTE 0x4
_rising_edge_mask:
	.BYTE 0x4
_falling_edge_mask:
	.BYTE 0x4
_debounced_input:
	.BYTE 0x4
;     180 unsigned long int blockmask, temp;                                                              
_blockmask:
	.BYTE 0x4
_temp:
	.BYTE 0x4
;     181 unsigned int timerruns_to_go;
;     182 int max_pos[nr_of_shutters],mask; 
_max_pos:
	.BYTE 0x20
_mask:
	.BYTE 0x2
;     183 
;     184 union datastruct {
;     185    
;     186         char      fraction[4];
;     187         long int  entire;
;     188                  }downwards, upwards,copy_downwards,copy_upwards;
_downwards:
	.BYTE 0x4
_upwards:
	.BYTE 0x4
_copy_downwards:
	.BYTE 0x4
_copy_upwards:
	.BYTE 0x4
;     189                  
;     190 char PCF8574[8];
_PCF8574:
	.BYTE 0x8
;     191                  
;     192 struct device {
;     193        
;     194         int position;
;     195         char frametimer;
;     196         char keypresses_in_frame;
;     197         char status_reg;
;     198         char level[15];
;     199                };
;     200                
;     201 struct device rolluik [nr_of_shutters];
_rolluik:
	.BYTE 0x140
;     202 
;     203 eeprom struct device_def {
;     204                  int position_def;
;     205                  char frametimer_def;
;     206                  char keypresses_in_frame_def;
;     207                  char status_reg_def;
;     208                  char level_def[15];
;     209                          };
;     210 // position_def = time needed for roll-down shutter to open en close
;     211 // time = position_def * 64 msec      500 * 64msec = 46.912 sec
;     212 
;     213 eeprom struct device_def rolluik_def [nr_of_shutters] =

	.ESEG
_rolluik_def:
;     214    
;     215    {{ 500,0,0,4,{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}},  //rolluik voordeur
_0rolluik_def:
	.DW  0x1F4
_1rolluik_def:
	.DB  0x0
_2rolluik_def:
	.DB  0x0
_3rolluik_def:
	.DB  0x4
_4rolluik_def:
	.DB  0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9
	.DB  0xA,0xB,0xC,0xD,0xE,0xF,0x10
;     216    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik bureau beneden  
_5rolluik_def:
	.DW  0x1F4
_6rolluik_def:
	.DB  0x0
_7rolluik_def:
	.DB  0x0
_8rolluik_def:
	.DB  0x4
_9rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     217    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik schuifraam living NOORD
_Arolluik_def:
	.DW  0x1F4
_Brolluik_def:
	.DB  0x0
_Crolluik_def:
	.DB  0x0
_Drolluik_def:
	.DB  0x4
_Erolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     218    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik raam living
_Frolluik_def:
	.DW  0x1F4
_10rolluik_def:
	.DB  0x0
_11rolluik_def:
	.DB  0x0
_12rolluik_def:
	.DB  0x4
_13rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     219    {  500,0,0,4,{1,2,3,4,6,7,8,9,10,11,12,13,14,15,16}},  //rolluik schuifraam living OOST 
_14rolluik_def:
	.DW  0x1F4
_15rolluik_def:
	.DB  0x0
_16rolluik_def:
	.DB  0x0
_17rolluik_def:
	.DB  0x4
_18rolluik_def:
	.DB  0x1,0x2,0x3,0x4,0x6,0x7,0x8,0x9
	.DB  0xA,0xB,0xC,0xD,0xE,0xF,0x10
;     220    {  500,0,0,4,{1,2,3,4,5,7,8,9,10,11,12,13,14,15,16}},  //rolluik schuifraam keuken
_19rolluik_def:
	.DW  0x1F4
_1Arolluik_def:
	.DB  0x0
_1Brolluik_def:
	.DB  0x0
_1Crolluik_def:
	.DB  0x4
_1Drolluik_def:
	.DB  0x1,0x2,0x3,0x4,0x5,0x7,0x8,0x9
	.DB  0xA,0xB,0xC,0xD,0xE,0xF,0x10
;     221    {  500,0,0,4,{1,2,3,4,5,6,8,9,10,11,12,13,14,15,16}},  //rolluik raam keuken 
_1Erolluik_def:
	.DW  0x1F4
_1Frolluik_def:
	.DB  0x0
_20rolluik_def:
	.DB  0x0
_21rolluik_def:
	.DB  0x4
_22rolluik_def:
	.DB  0x1,0x2,0x3,0x4,0x5,0x6,0x8,0x9
	.DB  0xA,0xB,0xC,0xD,0xE,0xF,0x10
;     222    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik achterdeur
_23rolluik_def:
	.DW  0x1F4
_24rolluik_def:
	.DB  0x0
_25rolluik_def:
	.DB  0x0
_26rolluik_def:
	.DB  0x4
_27rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     223    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik raam washok
_28rolluik_def:
	.DW  0x1F4
_29rolluik_def:
	.DB  0x0
_2Arolluik_def:
	.DB  0x0
_2Brolluik_def:
	.DB  0x4
_2Crolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     224    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik slaapkamer ouders 
_2Drolluik_def:
	.DW  0x1F4
_2Erolluik_def:
	.DB  0x0
_2Frolluik_def:
	.DB  0x0
_30rolluik_def:
	.DB  0x4
_31rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     225    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik slaapkamer 1 NOORD 
_32rolluik_def:
	.DW  0x1F4
_33rolluik_def:
	.DB  0x0
_34rolluik_def:
	.DB  0x0
_35rolluik_def:
	.DB  0x4
_36rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     226    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik slaapkamer 2 OOST
_37rolluik_def:
	.DW  0x1F4
_38rolluik_def:
	.DB  0x0
_39rolluik_def:
	.DB  0x0
_3Arolluik_def:
	.DB  0x4
_3Brolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     227    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik slaapkamer 3 VOORDEUR 
_3Crolluik_def:
	.DW  0x1F4
_3Drolluik_def:
	.DB  0x0
_3Erolluik_def:
	.DB  0x0
_3Frolluik_def:
	.DB  0x4
_40rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     228    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //rolluik badkamer 
_41rolluik_def:
	.DW  0x1F4
_42rolluik_def:
	.DB  0x0
_43rolluik_def:
	.DB  0x0
_44rolluik_def:
	.DB  0x4
_45rolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     229    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}},         //not implemented 
_46rolluik_def:
	.DW  0x1F4
_47rolluik_def:
	.DB  0x0
_48rolluik_def:
	.DB  0x0
_49rolluik_def:
	.DB  0x4
_4Arolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     230    {  500,0,0,4,{6,7,8,9,9,0,0,0,0,0,1,9,0,0,0}}};        //not implemented
_4Brolluik_def:
	.DW  0x1F4
_4Crolluik_def:
	.DB  0x0
_4Drolluik_def:
	.DB  0x0
_4Erolluik_def:
	.DB  0x4
_4Frolluik_def:
	.DB  0x6,0x7,0x8,0x9,0x9,0x0,0x0,0x0
	.DB  0x0,0x0,0x1,0x9,0x0,0x0,0x0
;     231    
;     232                  
;     233 
;     234  void set_output (unsigned char adress, unsigned char data )
;     235  {

	.CSEG
_set_output:
;     236   i2c_start();
	RCALL _i2c_start
;     237   i2c_write(adress);
	RCALL SUBOPT_0x0
	RCALL _i2c_write
;     238   i2c_write(data);
	RCALL SUBOPT_0x1
	RCALL _i2c_write
;     239   i2c_stop();
	RCALL _i2c_stop
;     240  }
	ADIW R28,2
	RET
;     241 
;     242 int read_input ( unsigned char adress)
;     243 {
_read_input:
;     244  int data;
;     245  
;     246  i2c_start();
	RCALL __SAVELOCR2
;	adress -> Y+2
;	data -> R16,R17
	RCALL _i2c_start
;     247  i2c_write(adress | 1);
	LDD  R30,Y+2
	ORI  R30,1
	ST   -Y,R30
	RCALL _i2c_write
;     248  data=i2c_read(0);
	RCALL SUBOPT_0x2
	RCALL _i2c_read
	LDI  R31,0
	__PUTW1R 16,17
;     249  i2c_stop();
	RCALL _i2c_stop
;     250  return data;
	__GETW1R 16,17
	RCALL __LOADLOCR2
	ADIW R28,3
	RET
;     251 }
;     252 
;     253 void init_i2c_devices(void)
;     254 {   
_init_i2c_devices:
;     255  set_output(PCF8574A_1,0xff);       //set I/O to 1 for input mode
	RCALL SUBOPT_0x3
	RCALL SUBOPT_0x4
;     256  set_output(PCF8574A_2,0xff);       //set I/O to 1 for input mode
	RCALL SUBOPT_0x5
	RCALL SUBOPT_0x4
;     257  set_output(PCF8574A_3,0xff);       //set I/O to 1 for input mode
	RCALL SUBOPT_0x6
	RCALL SUBOPT_0x4
;     258 
;     259 }
	RET
;     260  
;     261 void update_rolluik ( char nr,char pressed,char released)
;     262 {
_update_rolluik:
;     263  char i;
;     264  temp=1;
	ST   -Y,R16
;	nr -> Y+3
;	pressed -> Y+2
;	released -> Y+1
;	i -> R16
	RCALL SUBOPT_0x7
;     265  k=0;
	CLR  R7
;     266    
;     267   
;     268  if (pressed)
	LDD  R30,Y+2
	CPI  R30,0
	BREQ _0x6
;     269      {
;     270       if (!rolluik[nr].keypresses_in_frame)
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	CPI  R30,0
	BRNE _0x7
;     271            {rolluik[nr].frametimer = 50;}        // 50* 64 msec  = 3.2 sec timeframe  
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,2
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(50)
	ST   X,R30
;     272       ++rolluik[nr].keypresses_in_frame;
_0x7:
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	SUBI R30,-LOW(1)
	ST   X,R30
;     273       }
;     274       
;     275  if ((released) && (rolluik[nr].frametimer == 0) && ( rolluik[nr].keypresses_in_frame == 0))
_0x6:
	RCALL SUBOPT_0xB
	BREQ _0x9
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,2
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xC
	BRNE _0x9
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xC
	BREQ _0xA
_0x9:
	RJMP _0x8
_0xA:
;     276  
;     277      {
;     278       if (blocked(nr))
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x10)
	BREQ _0xB
;     279         {
;     280          rolluik[nr].status_reg = reset_blocked(nr);
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,0xEF
	POP  R26
	POP  R27
	RCALL SUBOPT_0xD
;     281          temp<<=nr;
;     282          blockmask &= ~temp;
	RCALL __COMD1
	LDS  R26,_blockmask
	LDS  R27,_blockmask+1
	LDS  R24,_blockmask+2
	LDS  R25,_blockmask+3
	RCALL __ANDD12
	RJMP _0x79
;     283          released = 0;
;     284         }
;     285        else
_0xB:
;     286         {
;     287          rolluik[nr].status_reg = set_blocked(nr);
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0xC)
	ORI  R30,0x10
	POP  R26
	POP  R27
	RCALL SUBOPT_0xD
;     288          temp<<=nr;
;     289          blockmask |= temp;
	LDS  R26,_blockmask
	LDS  R27,_blockmask+1
	LDS  R24,_blockmask+2
	LDS  R25,_blockmask+3
	RCALL __ORD12
_0x79:
	STS  _blockmask,R30
	STS  _blockmask+1,R31
	STS  _blockmask+2,R22
	STS  _blockmask+3,R23
;     290          released = 0;
	LDI  R30,LOW(0)
	STD  Y+1,R30
;     291         }
;     292       }
;     293  if (released)
_0x8:
	RCALL SUBOPT_0xB
	BRNE PC+2
	RJMP _0xD
;     294      {
;     295       switch (rolluik[nr].keypresses_in_frame)
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
;     296       {
;     297       case 1 :
	CPI  R30,LOW(0x1)
	BREQ PC+2
	RJMP _0x11
;     298         {
;     299          if (!blocked(nr))
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x10)
	BREQ PC+2
	RJMP _0x12
;     300           {
;     301            if (up(nr) || down(nr))
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x1)
	BRNE _0x14
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x2)
	BREQ _0x13
_0x14:
;     302             { 
;     303              rolluik[nr].status_reg = set_stop(nr);}
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	ST   X,R30
;     304             else
	RJMP _0x16
_0x13:
;     305             {
;     306              if (stop(nr) && previous_state_up(nr))
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3)
	BRNE _0x18
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x4)
	BRNE _0x19
_0x18:
	RJMP _0x17
_0x19:
;     307               {rolluik[nr].status_reg = set_down(nr);}
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xE
	POP  R26
	POP  R27
	RJMP _0x7A
;     308               else
_0x17:
;     309               {rolluik[nr].status_reg = set_up(nr);}
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xF
	POP  R26
	POP  R27
_0x7A:
	ST   X,R30
;     310             }
_0x16:
;     311           } 
;     312           break;
_0x12:
	RJMP _0x10
;     313         }
;     314        case 4 :
_0x11:
	CPI  R30,LOW(0x4)
	BRNE _0x1B
;     315          { k+=5;}
	LDI  R30,LOW(5)
	ADD  R7,R30
;     316        case 3 :
	RJMP _0x1C
_0x1B:
	CPI  R30,LOW(0x3)
	BRNE _0x1D
_0x1C:
;     317          { k+=5;}
	LDI  R30,LOW(5)
	ADD  R7,R30
;     318        case 2 :
	RJMP _0x1E
_0x1D:
	CPI  R30,LOW(0x2)
	BREQ PC+2
	RJMP _0x10
_0x1E:
;     319          { 
;     320           for(i=k;((i<(k+5)) && (!(rolluik[nr].level[i] == 0)));++i)
	MOV  R16,R7
_0x21:
	MOV  R30,R7
	SUBI R30,-LOW(5)
	CP   R16,R30
	BRSH _0x23
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,5
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x10
	RCALL SUBOPT_0xC
	BRNE _0x24
_0x23:
	RJMP _0x22
_0x24:
;     321            {
;     322              aux = (rolluik[nr].level[i])-1;
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,5
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x10
	RCALL SUBOPT_0xA
	SUBI R30,LOW(1)
	MOV  R5,R30
;     323              if (!blocked(aux))
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x10)
	BREQ PC+2
	RJMP _0x25
;     324               {
;     325                if (stop(nr))
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3)
	BRNE _0x26
;     326                 {rolluik[aux].status_reg = set_stop(aux);}
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	ST   X,R30
;     327                 
;     328                if (down(nr))
_0x26:
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x2)
	BRNE PC+2
	RJMP _0x27
;     329                 {
;     330                  if (going_up(aux))
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x40)
	BRNE PC+2
	RJMP _0x28
;     331                   {
;     332                    rolluik[aux].status_reg = set_stop(aux);
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	RCALL SUBOPT_0x12
;     333                    rolluik[aux].status_reg = set_reverse(aux);
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0x13
	POP  R26
	POP  R27
	RCALL SUBOPT_0x14
;     334                    reversetimer = 25;     // 25 * 64 msec = 1.6 sec
;     335                   }
;     336                  else
	RJMP _0x29
_0x28:
;     337                   {rolluik[aux].status_reg = set_down(aux);}
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xE
	POP  R26
	POP  R27
	ST   X,R30
_0x29:
;     338                 }
;     339                
;     340                if (up(nr))
_0x27:
	RCALL SUBOPT_0x8
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x1)
	BRNE PC+2
	RJMP _0x2A
;     341                 {
;     342                  if (going_down(aux))
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x80)
	BRNE PC+2
	RJMP _0x2B
;     343                   {
;     344                    rolluik[aux].status_reg = set_stop(aux);
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	RCALL SUBOPT_0x12
;     345                    rolluik[aux].status_reg = set_reverse(aux);
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0x13
	POP  R26
	POP  R27
	RCALL SUBOPT_0x14
;     346                    reversetimer = 25;     // 25 * 64 msec = 1.6 sec
;     347                   }
;     348                  else
	RJMP _0x2C
_0x2B:
;     349                   {rolluik[aux].status_reg = set_up(aux);}
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x11
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xF
	POP  R26
	POP  R27
	ST   X,R30
_0x2C:
;     350                 }
;     351               }
_0x2A:
;     352             }
_0x25:
	SUBI R16,-LOW(1)
	RJMP _0x21
_0x22:
;     353            break;
;     354            }
;     355        }
_0x10:
;     356       }
;     357 }
_0xD:
	LDD  R16,Y+0
	ADIW R28,4
	RET
;     358   
;     359 void process_input()               //takes 41µsec
;     360 {
_process_input:
;     361  debounced_input = 0x0000;
	LDI  R30,0
	STS  _debounced_input,R30
	STS  _debounced_input+1,R30
	STS  _debounced_input+2,R30
	STS  _debounced_input+3,R30
;     362  debounced_input |= (read_input(PCF8574A_3));
	RCALL SUBOPT_0x6
	RCALL SUBOPT_0x15
;     363  debounced_input<<=8;
;     364  debounced_input |= (read_input(PCF8574A_2)); 
	RCALL SUBOPT_0x5
	RCALL SUBOPT_0x15
;     365  debounced_input<<=8;
;     366  debounced_input |= (read_input(PCF8574A_1));  
	RCALL SUBOPT_0x3
	RCALL _read_input
	LDS  R26,_debounced_input
	LDS  R27,_debounced_input+1
	LDS  R24,_debounced_input+2
	LDS  R25,_debounced_input+3
	RCALL __CWD1
	RCALL __ORD12
	STS  _debounced_input,R30
	STS  _debounced_input+1,R31
	STS  _debounced_input+2,R22
	STS  _debounced_input+3,R23
;     367  
;     368  debounced_input = (~(debounced_input));  
	RCALL SUBOPT_0x16
	STS  _debounced_input,R30
	STS  _debounced_input+1,R31
	STS  _debounced_input+2,R22
	STS  _debounced_input+3,R23
;     369  
;     370  rising_edge_mask = ~edgemarker_1 & debounced_input;
	LDS  R30,_edgemarker_1
	LDS  R31,_edgemarker_1+1
	LDS  R22,_edgemarker_1+2
	LDS  R23,_edgemarker_1+3
	RCALL __COMD1
	LDS  R26,_debounced_input
	LDS  R27,_debounced_input+1
	LDS  R24,_debounced_input+2
	LDS  R25,_debounced_input+3
	RCALL __ANDD12
	STS  _rising_edge_mask,R30
	STS  _rising_edge_mask+1,R31
	STS  _rising_edge_mask+2,R22
	STS  _rising_edge_mask+3,R23
;     371  edgemarker_1 |= rising_edge_mask;
	RCALL SUBOPT_0x17
	STS  _edgemarker_1,R30
	STS  _edgemarker_1+1,R31
	STS  _edgemarker_1+2,R22
	STS  _edgemarker_1+3,R23
;     372  edgemarker_1 = rising_edge_mask | debounced_input;
	LDS  R30,_debounced_input
	LDS  R31,_debounced_input+1
	LDS  R22,_debounced_input+2
	LDS  R23,_debounced_input+3
	LDS  R26,_rising_edge_mask
	LDS  R27,_rising_edge_mask+1
	LDS  R24,_rising_edge_mask+2
	LDS  R25,_rising_edge_mask+3
	RCALL __ORD12
	STS  _edgemarker_1,R30
	STS  _edgemarker_1+1,R31
	STS  _edgemarker_1+2,R22
	STS  _edgemarker_1+3,R23
;     373  
;     374  falling_edge_mask = edgemarker_2 & ~debounced_input;
	RCALL SUBOPT_0x16
	LDS  R26,_edgemarker_2
	LDS  R27,_edgemarker_2+1
	LDS  R24,_edgemarker_2+2
	LDS  R25,_edgemarker_2+3
	RCALL __ANDD12
	STS  _falling_edge_mask,R30
	STS  _falling_edge_mask+1,R31
	STS  _falling_edge_mask+2,R22
	STS  _falling_edge_mask+3,R23
;     375  edgemarker_2 = edgemarker_1 | debounced_input;
	LDS  R30,_debounced_input
	LDS  R31,_debounced_input+1
	LDS  R22,_debounced_input+2
	LDS  R23,_debounced_input+3
	RCALL SUBOPT_0x17
	STS  _edgemarker_2,R30
	STS  _edgemarker_2+1,R31
	STS  _edgemarker_2+2,R22
	STS  _edgemarker_2+3,R23
;     376  edgemarker_2 |= debounced_input;   
	LDS  R30,_debounced_input
	LDS  R31,_debounced_input+1
	LDS  R22,_debounced_input+2
	LDS  R23,_debounced_input+3
	LDS  R26,_edgemarker_2
	LDS  R27,_edgemarker_2+1
	LDS  R24,_edgemarker_2+2
	LDS  R25,_edgemarker_2+3
	RCALL __ORD12
	STS  _edgemarker_2,R30
	STS  _edgemarker_2+1,R31
	STS  _edgemarker_2+2,R22
	STS  _edgemarker_2+3,R23
;     377  
;     378  
;     379 }   
	RET
;     380  
;     381  
;     382 void serialise()                   // takes 0.1 sec
;     383 {
_serialise:
;     384  int mask = 0x01;
;     385  char i;
;     386  for ( i=0;i<nr_of_shutters;++i)
	RCALL __SAVELOCR3
;	mask -> R16,R17
;	i -> R18
	LDI  R16,1
	LDI  R17,0
	LDI  R18,LOW(0)
_0x2E:
	CPI  R18,16
	BRSH _0x2F
;     387     {
;     388      if (falling_edge_mask & mask)
	__GETW1R 16,17
	LDS  R26,_falling_edge_mask
	LDS  R27,_falling_edge_mask+1
	LDS  R24,_falling_edge_mask+2
	LDS  R25,_falling_edge_mask+3
	RCALL SUBOPT_0x18
	BREQ _0x30
;     389       { update_rolluik(i,0,1);}
	ST   -Y,R18
	RCALL SUBOPT_0x2
	RCALL SUBOPT_0x19
	RCALL _update_rolluik
;     390      if (rising_edge_mask & mask)
_0x30:
	__GETW1R 16,17
	LDS  R26,_rising_edge_mask
	LDS  R27,_rising_edge_mask+1
	LDS  R24,_rising_edge_mask+2
	LDS  R25,_rising_edge_mask+3
	RCALL SUBOPT_0x18
	BREQ _0x31
;     391       { update_rolluik(i,1,0);}
	ST   -Y,R18
	RCALL SUBOPT_0x19
	RCALL SUBOPT_0x2
	RCALL _update_rolluik
;     392      mask <<= 1;
_0x31:
	LSL  R16
	ROL  R17
;     393     }
	SUBI R18,-LOW(1)
	RJMP _0x2E
_0x2F:
;     394 }
	RCALL __LOADLOCR3
	ADIW R28,3
	RET
;     395 
;     396                      
;     397 
;     398 // External Interrupt 0 service routine
;     399 interrupt [EXT_INT0] void ext_int0_isr(void)
;     400 {
_ext_int0_isr:
	RCALL SUBOPT_0x1A
;     401  sleep_disable();
	RCALL _sleep_disable
;     402  delay_ms(10);
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	RCALL SUBOPT_0x1B
;     403  process_input();
	RCALL _process_input
;     404  serialise();
	RCALL _serialise
;     405  timerruns_to_go = 600;   // > then the longest time to open or close greatest roll-down shutter
	LDI  R30,LOW(600)
	LDI  R31,HIGH(600)
	__PUTW1R 13,14
;     406  TCCR0 = 0x05;             // Start timer T0 : interrupts every 64 msec
	LDI  R30,LOW(5)
	RCALL SUBOPT_0x1C
;     407  TCNT0 = 0x06;
;     408 
;     409 }
	RCALL SUBOPT_0x1D
	RETI
;     410 
;     411 // Timer 0 overflow interrupt service routine
;     412 interrupt [TIM0_OVF] void timer0_ovf_isr(void)
;     413 {
_timer0_ovf_isr:
	RCALL SUBOPT_0x1A
;     414    
;     415  // Reinitialise Timer 0 value
;     416  TCNT0 = 0x06; 
	LDI  R30,LOW(6)
	OUT  0x32,R30
;     417  
;     418  output_changed=0;
	CLR  R11
;     419  if ( timerruns_to_go)
	MOV  R0,R13
	OR   R0,R14
	BREQ _0x32
;     420      { timerruns_to_go--;}
	__GETW1R 13,14
	SBIW R30,1
	__PUTW1R 13,14
;     421  else
	RJMP _0x33
_0x32:
;     422      {
;     423       TCCR0 = 0x00;              // Stop timer
	LDI  R30,LOW(0)
	OUT  0x33,R30
;     424       sleep_enable();
	RCALL _sleep_enable
;     425      }
_0x33:
;     426      
;     427  if(reversetimer > 0)
	LDI  R30,LOW(0)
	CP   R30,R4
	BRSH _0x34
;     428     { reversetimer--;}
	DEC  R4
;     429  temp=1;
_0x34:
	RCALL SUBOPT_0x7
;     430  for ( nr=0 ; nr <nr_of_shutters ; ++nr)
	CLR  R6
_0x36:
	RCALL SUBOPT_0x1E
	BRLO PC+2
	RJMP _0x37
;     431      { 
;     432      
;     433       if(rolluik[nr].frametimer > 0)
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,2
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R26,R30
	ADC  R27,R31
	LD   R26,X
	LDI  R30,LOW(0)
	CP   R30,R26
	BRSH _0x38
;     434        {rolluik[nr].frametimer--;}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,2
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	SUBI R30,LOW(1)
	RJMP _0x7B
;     435        else
_0x38:
;     436        {rolluik[nr].keypresses_in_frame =0;}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x20
_0x7B:
	ST   X,R30
;     437        
;     438       if (!(reversetimer) && ( reverse(nr)))
	TST  R4
	BRNE _0x3B
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x20)
	BRNE _0x3C
_0x3B:
	RJMP _0x3A
_0x3C:
;     439         {
;     440          rolluik[nr].status_reg = reset_reverse(nr);
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,0xDF
	POP  R26
	POP  R27
	RCALL SUBOPT_0x21
;     441          if (rolluik[nr].status_reg == previous_state_up(nr))
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x4)
	POP  R26
	CP   R30,R26
	BRNE _0x3D
;     442           {rolluik[nr].status_reg = set_down(nr);}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xE
	POP  R26
	POP  R27
	RJMP _0x7C
;     443          else
_0x3D:
;     444           {rolluik[nr].status_reg = set_up(nr) ;}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xF
	POP  R26
	POP  R27
_0x7C:
	ST   X,R30
;     445         }
;     446         
;     447       if (going_up(nr))
_0x3A:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x40)
	BREQ _0x3F
;     448         {
;     449          if(rolluik[nr].position < max_pos[nr])
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x23
	POP  R26
	POP  R27
	CP   R26,R30
	CPC  R27,R31
	BRGE _0x40
;     450           {rolluik[nr].position++;}
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
;     451          else
	RJMP _0x41
_0x40:
;     452           {rolluik[nr].status_reg = set_stop(nr);}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	ST   X,R30
_0x41:
;     453         }
;     454         
;     455       if (going_down(nr))
_0x3F:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x80)
	BREQ _0x42
;     456         {
;     457          if (rolluik[nr].position > 0)
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	MOV  R26,R30
	MOV  R27,R31
	RCALL __CPW02
	BRGE _0x43
;     458           {rolluik[nr].position--;}
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	SBIW R30,1
	ST   X+,R30
	ST   X,R31
;     459          else
	RJMP _0x44
_0x43:
;     460           {rolluik[nr].status_reg = set_stop(nr);}
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3C)
	POP  R26
	POP  R27
	ST   X,R30
_0x44:
;     461         }
;     462         
;     463       if (up(nr) && (!(going_up(nr))))
_0x42:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x1)
	BREQ _0x46
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x40)
	BREQ _0x47
_0x46:
	RJMP _0x45
_0x47:
;     464         {
;     465          rolluik[nr].status_reg = set_going_up(nr) ;
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3F)
	ORI  R30,0x40
	POP  R26
	POP  R27
	ST   X,R30
;     466          upwards.entire |=temp; 
	LDI  R26,LOW(_upwards)
	LDI  R27,HIGH(_upwards)
	PUSH R27
	PUSH R26
	RCALL SUBOPT_0x24
	POP  R26
	POP  R27
	RCALL SUBOPT_0x25
;     467          output_changed=1;
;     468         }
;     469         
;     470       if (down(nr) && (!(going_down(nr))))
_0x45:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x2)
	BREQ _0x49
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x80)
	BREQ _0x4A
_0x49:
	RJMP _0x48
_0x4A:
;     471         {
;     472          rolluik[nr].status_reg = set_going_down(nr);
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3F)
	ORI  R30,0x80
	POP  R26
	POP  R27
	ST   X,R30
;     473          downwards.entire |= temp;
	LDI  R26,LOW(_downwards)
	LDI  R27,HIGH(_downwards)
	PUSH R27
	PUSH R26
	RCALL SUBOPT_0x24
	POP  R26
	POP  R27
	RCALL SUBOPT_0x25
;     474          output_changed=1;
;     475         }
;     476       
;     477       if (stop(nr))
_0x48:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3)
	BRNE _0x4B
;     478         {
;     479           upwards.entire   &=~temp;
	LDI  R26,LOW(_upwards)
	LDI  R27,HIGH(_upwards)
	PUSH R27
	PUSH R26
	RCALL __GETD1P
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x26
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __ANDD12
	POP  R26
	POP  R27
	RCALL SUBOPT_0x27
;     480           downwards.entire &=~temp; 
	PUSH R27
	PUSH R26
	RCALL __GETD1P
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x26
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __ANDD12
	POP  R26
	POP  R27
	RCALL SUBOPT_0x25
;     481           output_changed=1;
;     482         }
;     483 #ifdef LCD_ATTACHED     
;     484 if (lcd_attached)
_0x4B:
	TST  R12
	BRNE PC+2
	RJMP _0x4C
;     485     { 
;     486       if (blocked(nr))
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x10)
	BREQ _0x4D
;     487        {
;     488         lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     489         lcd_putchar(6);
	RCALL SUBOPT_0x29
	RCALL _lcd_putchar
;     490        }
;     491       else
	RJMP _0x4E
_0x4D:
;     492        {
;     493          if (stop(nr))
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x3)
	BRNE _0x4F
;     494           {
;     495             if (rolluik[nr].position == 0)
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	SBIW R30,0
	BRNE _0x50
;     496              {
;     497               lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     498               lcd_putchar(1);
	RCALL SUBOPT_0x19
	RCALL _lcd_putchar
;     499              }
;     500             else
	RJMP _0x51
_0x50:
;     501              {
;     502               if (rolluik[nr].position == max_pos[nr])
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x23
	POP  R26
	POP  R27
	CP   R30,R26
	CPC  R31,R27
	BRNE _0x52
;     503                  {
;     504                    lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     505                    lcd_putchar(0);
	LDI  R30,LOW(0)
	RJMP _0x7D
;     506                  }
;     507               else
_0x52:
;     508                  {
;     509                    lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     510                    lcd_putchar(2);
	LDI  R30,LOW(2)
_0x7D:
	ST   -Y,R30
	RCALL _lcd_putchar
;     511                               
;     512                  }
;     513              } 
_0x51:
;     514            }
;     515          if (reverse(nr))
_0x4F:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x20)
	BREQ _0x54
;     516           { 
;     517            lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     518            lcd_putchar(5);
	RCALL SUBOPT_0x2A
	RCALL _lcd_putchar
;     519           }
;     520          if (going_up(nr))
_0x54:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x40)
	BREQ _0x55
;     521           {
;     522            lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     523            lcd_putchar(3); 
	RCALL SUBOPT_0x2B
	RCALL _lcd_putchar
;     524           }
;     525          if (going_down(nr))
_0x55:
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0xA
	ANDI R30,LOW(0x80)
	BREQ _0x56
;     526           {
;     527            lcd_gotoxy(nr+2,1);
	RCALL SUBOPT_0x28
	RCALL _lcd_gotoxy
;     528            lcd_putchar(4); 
	RCALL SUBOPT_0x2C
	RCALL _lcd_putchar
;     529           }
;     530          }
_0x56:
_0x4E:
;     531       }
;     532 #endif
;     533  temp <<=1;
_0x4C:
	LDS  R30,_temp
	LDS  R31,_temp+1
	LDS  R22,_temp+2
	LDS  R23,_temp+3
	RCALL __LSLD1
	STS  _temp,R30
	STS  _temp+1,R31
	STS  _temp+2,R22
	STS  _temp+3,R23
;     534    }
	INC  R6
	RJMP _0x36
_0x37:
;     535   upwards.entire    &= ~blockmask;
	LDI  R26,LOW(_upwards)
	LDI  R27,HIGH(_upwards)
	PUSH R27
	PUSH R26
	RCALL __GETD1P
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x2D
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __ANDD12
	POP  R26
	POP  R27
	RCALL SUBOPT_0x27
;     536   downwards.entire  &= ~blockmask;
	PUSH R27
	PUSH R26
	RCALL __GETD1P
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x2D
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __ANDD12
	POP  R26
	POP  R27
	RCALL __PUTDP1
;     537  
;     538   
;     539 #ifdef LCD_ATTACHED   
;     540   if (upwards.entire & downwards.entire)
	LDS  R30,_downwards
	LDS  R31,_downwards+1
	LDS  R22,_downwards+2
	LDS  R23,_downwards+3
	LDS  R26,_upwards
	LDS  R27,_upwards+1
	LDS  R24,_upwards+2
	LDS  R25,_upwards+3
	RCALL __ANDD12
	RCALL __CPD10
	BREQ _0x57
;     541       {
;     542        while(1)
_0x58:
;     543         {
;     544          lcd_gotoxy(9,2);
	LDI  R30,LOW(9)
	ST   -Y,R30
	RCALL SUBOPT_0x2E
	RCALL _lcd_gotoxy
;     545          lcd_putsf(error_message);  
	LDI  R30,LOW(_error_message*2)
	LDI  R31,HIGH(_error_message*2)
	RCALL SUBOPT_0x2F
;     546         }
	RJMP _0x58
;     547        }  
;     548 #endif      
;     549   
;     550   if (output_changed)
_0x57:
	TST  R11
	BRNE PC+2
	RJMP _0x5B
;     551    {
;     552     copy_downwards = downwards;
	LDI  R30,LOW(_downwards)
	LDI  R31,HIGH(_downwards)
	LDI  R26,LOW(_copy_downwards)
	LDI  R27,HIGH(_copy_downwards)
	LDI  R24,4
	RCALL __COPYMML
;     553     copy_upwards = upwards;
	LDI  R30,LOW(_upwards)
	LDI  R31,HIGH(_upwards)
	LDI  R26,LOW(_copy_upwards)
	LDI  R27,HIGH(_copy_upwards)
	LDI  R24,4
	RCALL __COPYMML
;     554     mask=0x0001;
	RCALL SUBOPT_0x30
;     555     x=0;
	CLR  R8
;     556     for ( z=0; z<4; z++ )
	CLR  R10
_0x5D:
	LDI  R30,LOW(4)
	CP   R10,R30
	BRLO PC+2
	RJMP _0x5E
;     557        {
;     558          mask = 0x0001;
	RCALL SUBOPT_0x30
;     559          k=0;
	CLR  R7
;     560          for ( y=0; y<8 ; y++ )
	CLR  R9
_0x60:
	LDI  R30,LOW(8)
	CP   R9,R30
	BRSH _0x61
;     561               {
;     562                 if (y==4)
	LDI  R30,LOW(4)
	CP   R30,R9
	BRNE _0x62
;     563                   {
;     564                     x++;
	INC  R8
;     565                     mask=0x0001;
	RCALL SUBOPT_0x30
;     566                     copy_downwards.fraction[z]>>=4;
	RCALL SUBOPT_0x31
	SUBI R30,LOW(-_copy_downwards)
	SBCI R31,HIGH(-_copy_downwards)
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x32
	POP  R26
	POP  R27
	ST   X,R30
;     567                     copy_upwards.fraction[z]>>=4;
	RCALL SUBOPT_0x31
	SUBI R30,LOW(-_copy_upwards)
	SBCI R31,HIGH(-_copy_upwards)
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x32
	POP  R26
	POP  R27
	RCALL SUBOPT_0x33
;     568                     k=0;
;     569                   } 
;     570                 PCF8574[x] |= ((copy_downwards.fraction[z] << k) & mask);
_0x62:
	RCALL SUBOPT_0x34
	PUSH R31
	PUSH R30
	LD   R30,Z
	PUSH R30
	RCALL SUBOPT_0x31
	SUBI R30,LOW(-_copy_downwards)
	SBCI R31,HIGH(-_copy_downwards)
	LD   R30,Z
	MOV  R26,R30
	MOV  R30,R7
	RCALL SUBOPT_0x35
	POP  R26
	RCALL SUBOPT_0x36
	POP  R26
	POP  R27
	RCALL SUBOPT_0x37
;     571                 mask <<= 1;
;     572                 PCF8574[x] |= ((copy_upwards.fraction[z] << (k+1)) & mask);
	RCALL SUBOPT_0x34
	PUSH R31
	PUSH R30
	LD   R30,Z
	PUSH R30
	RCALL SUBOPT_0x31
	SUBI R30,LOW(-_copy_upwards)
	SBCI R31,HIGH(-_copy_upwards)
	LD   R30,Z
	PUSH R30
	MOV  R30,R7
	SUBI R30,-LOW(1)
	POP  R26
	RCALL SUBOPT_0x35
	POP  R26
	RCALL SUBOPT_0x36
	POP  R26
	POP  R27
	RCALL SUBOPT_0x37
;     573                 mask <<= 1;                                         
;     574                 k++;
	INC  R7
;     575               }
	INC  R9
	RJMP _0x60
_0x61:
;     576              x++;
	INC  R8
;     577          }
	INC  R10
	RJMP _0x5D
_0x5E:
;     578          
;     579          set_output(PCF8574_1,PCF8574[0]);   
	RCALL SUBOPT_0x38
	LDS  R30,_PCF8574
	RCALL SUBOPT_0x39
;     580          set_output(PCF8574_2,PCF8574[1]); 
	RCALL SUBOPT_0x3A
	__GETB1MN _PCF8574,1
	RCALL SUBOPT_0x39
;     581          set_output(PCF8574_3,PCF8574[2]); 
	RCALL SUBOPT_0x3B
	__GETB1MN _PCF8574,2
	RCALL SUBOPT_0x39
;     582          set_output(PCF8574_4,PCF8574[3]);
	RCALL SUBOPT_0x3C
	__GETB1MN _PCF8574,3
	RCALL SUBOPT_0x39
;     583          set_output(PCF8574_5,PCF8574[4]);
	RCALL SUBOPT_0x3D
	__GETB1MN _PCF8574,4
	RCALL SUBOPT_0x39
;     584          set_output(PCF8574_6,PCF8574[5]);
	RCALL SUBOPT_0x3E
	__GETB1MN _PCF8574,5
	RCALL SUBOPT_0x39
;     585             
;     586          PCF8574[0]=0x00;
	LDI  R30,LOW(0)
	STS  _PCF8574,R30
;     587          PCF8574[1]=0x00;
	__PUTB1MN _PCF8574,1
;     588          PCF8574[2]=0x00;
	__PUTB1MN _PCF8574,2
;     589          PCF8574[3]=0x00;
	__PUTB1MN _PCF8574,3
;     590          PCF8574[4]=0x00;
	__PUTB1MN _PCF8574,4
;     591          PCF8574[5]=0x00;
	__PUTB1MN _PCF8574,5
;     592   
;     593    }     
;     594       
;     595        
;     596 }       
_0x5B:
	RCALL SUBOPT_0x1D
	RETI
;     597 
;     598 void init_devices()
;     599 {  
_init_devices:
;     600  
;     601  for (nr=0; nr<nr_of_shutters; ++nr)
	CLR  R6
_0x64:
	RCALL SUBOPT_0x1E
	BRLO PC+2
	RJMP _0x65
;     602   {
;     603    rolluik[nr].position = rolluik_def.position_def;
	RCALL SUBOPT_0x1F
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	SUBI R30,LOW(-_rolluik)
	SBCI R31,HIGH(-_rolluik)
	PUSH R31
	PUSH R30
	LDI  R26,LOW(_rolluik_def)
	LDI  R27,HIGH(_rolluik_def)
	RCALL __EEPROMRDW
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;     604    max_pos[nr] = rolluik[nr].position;
	MOV  R30,R6
	LDI  R26,LOW(_max_pos)
	LDI  R27,HIGH(_max_pos)
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	RCALL SUBOPT_0x22
	POP  R26
	POP  R27
	ST   X+,R30
	ST   X,R31
;     605    rolluik[nr].frametimer = 0;
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,2
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x21
;     606    rolluik[nr].keypresses_in_frame = 0;
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,3
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x20
	RCALL SUBOPT_0x21
;     607    rolluik[nr].status_reg = rolluik_def[nr].status_reg_def;
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik_def,4
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x3F
	POP  R26
	POP  R27
	RCALL SUBOPT_0x33
;     608    
;     609     for(k=0;k<15;k++)
_0x67:
	LDI  R30,LOW(15)
	CP   R7,R30
	BRSH _0x68
;     610      {
;     611       rolluik[nr].level[k] = rolluik_def[nr].level_def[k];
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik,5
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x40
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x1F
	PUSH R31
	PUSH R30
	__POINTW1MN _rolluik_def,5
	POP  R26
	POP  R27
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x9
	POP  R26
	POP  R27
	RCALL SUBOPT_0x40
	RCALL SUBOPT_0x3F
	POP  R26
	POP  R27
	ST   X,R30
;     612      }
	INC  R7
	RJMP _0x67
_0x68:
;     613    }
	INC  R6
	RJMP _0x64
_0x65:
;     614 }
	RET
;     615 
;     616 #ifdef LCD_ATTACHED
;     617 void init_screen(void)
;     618 {
_init_screen:
;     619  lcd_clear();
	RCALL _lcd_clear
;     620  define_char(char0,0);
	LDI  R30,LOW(_char0)
	LDI  R31,HIGH(_char0)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x2
	RCALL _define_char
;     621  define_char(char1,1);
	LDI  R30,LOW(_char1)
	LDI  R31,HIGH(_char1)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x19
	RCALL _define_char
;     622  define_char(char2,2);
	LDI  R30,LOW(_char2)
	LDI  R31,HIGH(_char2)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x2E
	RCALL _define_char
;     623  define_char(char3,3);
	LDI  R30,LOW(_char3)
	LDI  R31,HIGH(_char3)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x2B
	RCALL _define_char
;     624  define_char(char4,4);
	LDI  R30,LOW(_char4)
	LDI  R31,HIGH(_char4)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x2C
	RCALL _define_char
;     625  define_char(char5,5);
	LDI  R30,LOW(_char5)
	LDI  R31,HIGH(_char5)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x2A
	RCALL _define_char
;     626  define_char(char6,6); 
	LDI  R30,LOW(_char6)
	LDI  R31,HIGH(_char6)
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x29
	RCALL _define_char
;     627  
;     628  lcd_gotoxy(2,1);
	RCALL SUBOPT_0x2E
	RCALL SUBOPT_0x19
	RCALL _lcd_gotoxy
;     629  for (k=0;k<nr_of_shutters;k++)
	CLR  R7
_0x6A:
	LDI  R30,LOW(16)
	CP   R7,R30
	BRSH _0x6B
;     630   {
;     631    lcd_putchar(0);
	RCALL SUBOPT_0x2
	RCALL _lcd_putchar
;     632   }
	INC  R7
	RJMP _0x6A
_0x6B:
;     633 }
	RET
;     634 #endif
;     635 
;     636 void initial_status(void)
;     637 {
_initial_status:
;     638  
;     639  set_output(PCF8574_1,0b10101010);      // All shutters open for initialisation  
	RCALL SUBOPT_0x38
	RCALL SUBOPT_0x41
;     640  set_output(PCF8574_2,0b10101010);  
	RCALL SUBOPT_0x3A
	RCALL SUBOPT_0x41
;     641  set_output(PCF8574_3,0b10101010); 
	RCALL SUBOPT_0x3B
	RCALL SUBOPT_0x41
;     642  set_output(PCF8574_4,0b10101010);
	RCALL SUBOPT_0x3C
	RCALL SUBOPT_0x41
;     643  set_output(PCF8574_5,0b10101010);
	RCALL SUBOPT_0x3D
	RCALL SUBOPT_0x41
;     644  set_output(PCF8574_6,0b10101010); 
	RCALL SUBOPT_0x3E
	RCALL SUBOPT_0x41
;     645  
;     646  for (k=0;k<4;++k)                     // # seconds for the longest shutter to go up
	CLR  R7
_0x6D:
	LDI  R30,LOW(4)
	CP   R7,R30
	BRSH _0x6E
;     647   {
;     648    delay_ms(999);
	LDI  R30,LOW(999)
	LDI  R31,HIGH(999)
	RCALL SUBOPT_0x1B
;     649   } 
	INC  R7
	RJMP _0x6D
_0x6E:
;     650     
;     651  set_output(PCF8574_1,0b00000000);    // stop all shutters 
	RCALL SUBOPT_0x38
	RCALL SUBOPT_0x2
	RCALL _set_output
;     652  set_output(PCF8574_2,0b00000000); 
	RCALL SUBOPT_0x3A
	RCALL SUBOPT_0x2
	RCALL _set_output
;     653  set_output(PCF8574_3,0b00000000);
	RCALL SUBOPT_0x3B
	RCALL SUBOPT_0x2
	RCALL _set_output
;     654  set_output(PCF8574_4,0b00000000);
	RCALL SUBOPT_0x3C
	RCALL SUBOPT_0x2
	RCALL _set_output
;     655  set_output(PCF8574_5,0b00000000);
	RCALL SUBOPT_0x3D
	RCALL SUBOPT_0x2
	RCALL _set_output
;     656  set_output(PCF8574_6,0b00000000);   
	RCALL SUBOPT_0x3E
	RCALL SUBOPT_0x2
	RCALL _set_output
;     657 }
	RET
;     658 #ifdef LCD_ATTACHED
;     659 void version(void)
;     660 {
_version:
;     661  
;     662  lcd_gotoxy(3,0);
	RCALL SUBOPT_0x2B
	RCALL SUBOPT_0x2
	RCALL _lcd_gotoxy
;     663  lcd_putsf(message1);
	LDI  R30,LOW(_message1*2)
	LDI  R31,HIGH(_message1*2)
	RCALL SUBOPT_0x2F
;     664  lcd_gotoxy(7,1);
	LDI  R30,LOW(7)
	ST   -Y,R30
	RCALL SUBOPT_0x19
	RCALL _lcd_gotoxy
;     665  lcd_putsf(message2);
	LDI  R30,LOW(_message2*2)
	LDI  R31,HIGH(_message2*2)
	RCALL SUBOPT_0x2F
;     666  lcd_gotoxy(3,3);
	RCALL SUBOPT_0x2B
	RCALL SUBOPT_0x2B
	RCALL _lcd_gotoxy
;     667  lcd_putsf(message3);   
	LDI  R30,LOW(_message3*2)
	LDI  R31,HIGH(_message3*2)
	RCALL SUBOPT_0x2F
;     668  
;     669 }
	RET
;     670 #endif 
;     671 // Declare your global variables here
;     672 
;     673 void main(void)
;     674 {
_main:
;     675 
;     676 // Input/Output Ports initialization
;     677 // Port A initialization
;     678 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     679 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     680 PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     681 DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     682 
;     683 // Port B initialization
;     684 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     685 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     686 PORTB=0xFF;
	OUT  0x18,R30
;     687 DDRB=0xFF;
	OUT  0x17,R30
;     688 
;     689 // Port C initialization
;     690 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     691 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     692 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;     693 DDRC=0x00;
	OUT  0x14,R30
;     694 
;     695 // Port D initialization
;     696 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     697 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     698 PORTD=0x00;
	OUT  0x12,R30
;     699 DDRD=0x00;
	OUT  0x11,R30
;     700 
;     701 // Timer/Counter 0 initialization
;     702 // Clock source: System Clock
;     703 // Clock value: 3,906 kHz
;     704 TCCR0=0x00;                    //Stop timer till after init_devices
	RCALL SUBOPT_0x1C
;     705 TCNT0=0x06;                    // set timer to 10 msec
;     706 
;     707 // Timer/Counter 1 initialization
;     708 // Clock source: System Clock
;     709 // Clock value: Timer 1 Stopped
;     710 // Mode: Normal top=FFFFh
;     711 // OC1A output: Discon.
;     712 // OC1B output: Discon.
;     713 // Noise Canceler: Off
;     714 // Input Capture on Falling Edge
;     715 TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     716 TCCR1B=0x00;
	OUT  0x2E,R30
;     717 TCNT1H=0x00;
	OUT  0x2D,R30
;     718 TCNT1L=0x00;
	OUT  0x2C,R30
;     719 OCR1AH=0x00;
	OUT  0x2B,R30
;     720 OCR1AL=0x00;
	OUT  0x2A,R30
;     721 OCR1BH=0x00;
	OUT  0x29,R30
;     722 OCR1BL=0x00;
	OUT  0x28,R30
;     723 
;     724 // External Interrupt(s) initialization
;     725 // INT0: On
;     726 // INT0 Mode: Low level (changed from falling edge to be able to go in powerdown mode)
;     727 // INT1: Off
;     728 GIMSK=0x40;
	LDI  R30,LOW(64)
	OUT  0x3B,R30
;     729 MCUCR=0x00;
	LDI  R30,LOW(0)
	OUT  0x35,R30
;     730 GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
;     731 
;     732 // Timer(s)/Counter(s) Interrupt(s) initialization
;     733 TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     734 
;     735 // Analog Comparator initialization
;     736 // Analog Comparator: Off
;     737 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     738 // Analog Comparator Output: Off
;     739 ACSR=0x80;  
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     740 
;     741 // I2C Bus initialization
;     742 i2c_init();
	RCALL _i2c_init
;     743 
;     744 // LCD module initialization  
;     745 #ifdef LCD_ATTACHED
;     746 lcd_attached=lcd_init(20); 
	LDI  R30,LOW(20)
	ST   -Y,R30
	RCALL _lcd_init
	MOV  R12,R30
;     747 #endif   
;     748 init_devices(); 
	RCALL _init_devices
;     749 init_i2c_devices(); 
	RCALL _init_i2c_devices
;     750 #ifdef LCD_ATTACHED
;     751 if (lcd_attached)
	TST  R12
	BREQ _0x6F
;     752  {
;     753   version(); 
	RCALL _version
;     754  } 
;     755 #endif
;     756 initial_status();   
_0x6F:
	RCALL _initial_status
;     757  
;     758 
;     759 // Global enable interrupts
;     760 
;     761 #asm("sei") 
	sei
;     762  
;     763 #ifdef LCD_ATTACHED  
;     764 if (lcd_attached)
	TST  R12
	BREQ _0x70
;     765  {
;     766   init_screen();
	RCALL _init_screen
;     767  }
;     768 #endif
;     769 sleep_enable();
_0x70:
	RCALL _sleep_enable
;     770 
;     771 
;     772 while (1)
_0x71:
;     773       {
;     774        PORTB=0xFE;
	LDI  R30,LOW(254)
	OUT  0x18,R30
;     775        powerdown();
	RCALL _powerdown
;     776        PORTB=0B11111101;
	LDI  R30,LOW(253)
	OUT  0x18,R30
;     777       };
	RJMP _0x71
;     778 }
_0x74:
	RJMP _0x74

    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G2:
	.BYTE 0x4
__lcd_x:
	.BYTE 0x1
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
_lcd_write_byte:
	RCALL __lcd_ready
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x42
    sbi   __lcd_port,__lcd_rs     ;RS=1
	RCALL SUBOPT_0x1
	RCALL __lcd_write_data
	ADIW R28,2
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
	RCALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G2)
	SBCI R31,HIGH(-__base_y_G2)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	RCALL SUBOPT_0x43
	LDD  R30,Y+1
	STS  __lcd_x,R30
	LD   R30,Y
	STS  __lcd_y,R30
	ADIW R28,2
	RET
_lcd_clear:
	RCALL __lcd_ready
	RCALL SUBOPT_0x2E
	RCALL SUBOPT_0x42
	LDI  R30,LOW(12)
	ST   -Y,R30
	RCALL SUBOPT_0x42
	RCALL SUBOPT_0x19
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
	BRSH _0x76
	__lcd_putchar1:
	LDS  R30,__lcd_y
	SUBI R30,-LOW(1)
	STS  __lcd_y,R30
	RCALL SUBOPT_0x2
	LDS  R30,__lcd_y
	ST   -Y,R30
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0x76:
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
	RCALL SUBOPT_0x44
	RCALL SUBOPT_0x44
	RCALL SUBOPT_0x44
	RCALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL __lcd_init_write_G2
	RCALL __long_delay_G2
	LDI  R30,LOW(40)
	RCALL SUBOPT_0x43
	RCALL __long_delay_G2
	RCALL SUBOPT_0x2C
	RCALL __lcd_write_data
	RCALL __long_delay_G2
	LDI  R30,LOW(133)
	RCALL SUBOPT_0x43
	RCALL __long_delay_G2
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RCALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x77
	LDI  R30,LOW(0)
	RJMP _0x78
_0x77:
	RCALL __lcd_ready
	RCALL SUBOPT_0x29
	RCALL __lcd_write_data
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x78:
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

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	LDD  R30,Y+1
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	LD   R30,Y
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 13 TIMES
SUBOPT_0x2:
	LDI  R30,LOW(0)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	LDI  R30,LOW(78)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x4:
	LDI  R30,LOW(255)
	ST   -Y,R30
	RJMP _set_output

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	LDI  R30,LOW(76)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	LDI  R30,LOW(68)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	__GETD1N 0x1
	STS  _temp,R30
	STS  _temp+1,R31
	STS  _temp+2,R22
	STS  _temp+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 27 TIMES
SUBOPT_0x8:
	LDD  R30,Y+3
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 82 TIMES
SUBOPT_0x9:
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
	RCALL __MULW12U
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 51 TIMES
SUBOPT_0xA:
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	LDD  R30,Y+1
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xC:
	ADD  R26,R30
	ADC  R27,R31
	LD   R26,X
	CPI  R26,LOW(0x0)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	ST   X,R30
	LDD  R30,Y+3
	LDS  R26,_temp
	LDS  R27,_temp+1
	LDS  R24,_temp+2
	LDS  R25,_temp+3
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __LSLD12
	STS  _temp,R30
	STS  _temp+1,R31
	STS  _temp+2,R22
	STS  _temp+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xE:
	ANDI R30,LOW(0xF0)
	ORI  R30,LOW(0xA)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xF:
	ANDI R30,LOW(0xF0)
	ORI  R30,LOW(0x5)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	ADD  R26,R30
	ADC  R27,R31
	MOV  R30,R16
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 17 TIMES
SUBOPT_0x11:
	MOV  R30,R5
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	ST   X,R30
	RJMP SUBOPT_0x11

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	ANDI R30,0xDF
	ORI  R30,0x20
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	ST   X,R30
	LDI  R30,LOW(25)
	MOV  R4,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	RCALL _read_input
	LDS  R26,_debounced_input
	LDS  R27,_debounced_input+1
	LDS  R24,_debounced_input+2
	LDS  R25,_debounced_input+3
	RCALL __CWD1
	RCALL __ORD12
	STS  _debounced_input,R30
	STS  _debounced_input+1,R31
	STS  _debounced_input+2,R22
	STS  _debounced_input+3,R23
	LDS  R26,_debounced_input
	LDS  R27,_debounced_input+1
	LDS  R24,_debounced_input+2
	LDS  R25,_debounced_input+3
	LDI  R30,LOW(8)
	RCALL __LSLD12
	STS  _debounced_input,R30
	STS  _debounced_input+1,R31
	STS  _debounced_input+2,R22
	STS  _debounced_input+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x16:
	LDS  R30,_debounced_input
	LDS  R31,_debounced_input+1
	LDS  R22,_debounced_input+2
	LDS  R23,_debounced_input+3
	RCALL __COMD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x17:
	LDS  R26,_edgemarker_1
	LDS  R27,_edgemarker_1+1
	LDS  R24,_edgemarker_1+2
	LDS  R25,_edgemarker_1+3
	RCALL __ORD12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x18:
	RCALL __CWD1
	RCALL __ANDD12
	RCALL __CPD10
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 14 TIMES
SUBOPT_0x19:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1A:
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
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1B:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1C:
	OUT  0x33,R30
	LDI  R30,LOW(6)
	OUT  0x32,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1D:
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
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1E:
	LDI  R30,LOW(16)
	CP   R6,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 46 TIMES
SUBOPT_0x1F:
	MOV  R30,R6
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x20:
	ADD  R26,R30
	ADC  R27,R31
	LDI  R30,LOW(0)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x21:
	ST   X,R30
	RJMP SUBOPT_0x1F

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x22:
	LDI  R26,LOW(20)
	LDI  R27,HIGH(20)
	RCALL __MULW12U
	SUBI R30,LOW(-_rolluik)
	SBCI R31,HIGH(-_rolluik)
	MOV  R26,R30
	MOV  R27,R31
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x23:
	MOV  R30,R6
	LDI  R26,LOW(_max_pos)
	LDI  R27,HIGH(_max_pos)
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x24:
	RCALL __GETD1P
	LDS  R26,_temp
	LDS  R27,_temp+1
	LDS  R24,_temp+2
	LDS  R25,_temp+3
	RCALL __ORD12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x25:
	RCALL __PUTDP1
	LDI  R30,LOW(1)
	MOV  R11,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x26:
	LDS  R30,_temp
	LDS  R31,_temp+1
	LDS  R22,_temp+2
	LDS  R23,_temp+3
	RCALL __COMD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x27:
	RCALL __PUTDP1
	LDI  R26,LOW(_downwards)
	LDI  R27,HIGH(_downwards)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x28:
	MOV  R30,R6
	SUBI R30,-LOW(2)
	ST   -Y,R30
	RJMP SUBOPT_0x19

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x29:
	LDI  R30,LOW(6)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2A:
	LDI  R30,LOW(5)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x2B:
	LDI  R30,LOW(3)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x2C:
	LDI  R30,LOW(4)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2D:
	LDS  R30,_blockmask
	LDS  R31,_blockmask+1
	LDS  R22,_blockmask+2
	LDS  R23,_blockmask+3
	RCALL __COMD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x2E:
	LDI  R30,LOW(2)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x2F:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _lcd_putsf

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x30:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _mask,R30
	STS  _mask+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x31:
	MOV  R30,R10
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x32:
	LD   R30,Z
	SWAP R30
	ANDI R30,0xF
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x33:
	ST   X,R30
	CLR  R7
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x34:
	MOV  R30,R8
	LDI  R31,0
	SUBI R30,LOW(-_PCF8574)
	SBCI R31,HIGH(-_PCF8574)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x35:
	RCALL __LSLB12
	MOV  R26,R30
	LDS  R30,_mask
	LDS  R31,_mask+1
	LDI  R27,0
	AND  R30,R26
	AND  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x36:
	LDI  R27,0
	OR   R30,R26
	OR   R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x37:
	ST   X,R30
	LDS  R30,_mask
	LDS  R31,_mask+1
	LSL  R30
	ROL  R31
	STS  _mask,R30
	STS  _mask+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x38:
	LDI  R30,LOW(114)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 12 TIMES
SUBOPT_0x39:
	ST   -Y,R30
	RJMP _set_output

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3A:
	LDI  R30,LOW(126)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3B:
	LDI  R30,LOW(116)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3C:
	LDI  R30,LOW(120)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3D:
	LDI  R30,LOW(118)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x3E:
	LDI  R30,LOW(122)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3F:
	ADD  R26,R30
	ADC  R27,R31
	RCALL __EEPROMRDB
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x40:
	ADD  R26,R30
	ADC  R27,R31
	MOV  R30,R7
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x41:
	LDI  R30,LOW(170)
	RJMP SUBOPT_0x39

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x42:
	RCALL __lcd_write_data
	RJMP __lcd_ready

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x43:
	ST   -Y,R30
	RJMP __lcd_write_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x44:
	RCALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	RJMP __lcd_init_write_G2

	.equ __i2c_dir=__i2c_port-1
	.equ __i2c_pin=__i2c_port-2
_i2c_init:
	cbi  __i2c_port,__scl_bit
	cbi  __i2c_port,__sda_bit
	sbi  __i2c_dir,__scl_bit
	cbi  __i2c_dir,__sda_bit
	rjmp __i2c_delay2
_i2c_start:
	cbi  __i2c_dir,__sda_bit
	cbi  __i2c_dir,__scl_bit
	clr  r30
	nop
	sbis __i2c_pin,__sda_bit
	ret
	sbis __i2c_pin,__scl_bit
	ret
	rcall __i2c_delay1
	sbi  __i2c_dir,__sda_bit
	rcall __i2c_delay1
	sbi  __i2c_dir,__scl_bit
	ldi  r30,1
__i2c_delay1:
	ldi  r22,7
	rjmp __i2c_delay2l
_i2c_stop:
	sbi  __i2c_dir,__sda_bit
	sbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	cbi  __i2c_dir,__scl_bit
	rcall __i2c_delay1
	cbi  __i2c_dir,__sda_bit
__i2c_delay2:
	ldi  r22,13
__i2c_delay2l:
	dec  r22
	brne __i2c_delay2l
	ret
_i2c_read:
	ldi  r23,8
__i2c_read0:
	cbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	clc
	sbic __i2c_pin,__sda_bit
	sec
	sbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	rol  r30
	dec  r23
	brne __i2c_read0
	ld   r23,y+
	tst  r23
	brne __i2c_read1
	cbi  __i2c_dir,__sda_bit
	rjmp __i2c_read2
__i2c_read1:
	sbi  __i2c_dir,__sda_bit
__i2c_read2:
	rcall __i2c_delay1
	cbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	sbi  __i2c_dir,__scl_bit
	rcall __i2c_delay1
	cbi  __i2c_dir,__sda_bit
	rjmp __i2c_delay1

_i2c_write:
	ld   r30,y+
	ldi  r23,8
__i2c_write0:
	lsl  r30
	brcc __i2c_write1
	cbi  __i2c_dir,__sda_bit
	rjmp __i2c_write2
__i2c_write1:
	sbi  __i2c_dir,__sda_bit
__i2c_write2:
	rcall __i2c_delay2
	cbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	sbi  __i2c_dir,__scl_bit
	dec  r23
	brne __i2c_write0
	cbi  __i2c_dir,__sda_bit
	rcall __i2c_delay1
	cbi  __i2c_dir,__scl_bit
	rcall __i2c_delay2
	ldi  r30,1
	sbic __i2c_pin,__sda_bit
	clr  r30
	sbi  __i2c_dir,__scl_bit
	ret

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x3E8
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

_sleep_enable:
	in   r30,mcucr
	sbr  r30,__se_bit
	out  mcucr,r30
	ret

_sleep_disable:
	in   r30,mcucr
	cbr  r30,__se_bit
	out  mcucr,r30
	ret

_powerdown:
	in   r30,mcucr
	sbr  r30,__sm_mask
	out  mcucr,r30
	sleep
	ret

__ANDD12:
	AND  R30,R26
	AND  R31,R27
	AND  R22,R24
	AND  R23,R25
	RET

__ORD12:
	OR   R30,R26
	OR   R31,R27
	OR   R22,R24
	OR   R23,R25
	RET

__LSLB12:
	TST  R30
	MOV  R0,R30
	MOV  R30,R26
	BREQ __LSLB12R
__LSLB12L:
	LSL  R30
	DEC  R0
	BRNE __LSLB12L
__LSLB12R:
	RET

__LSLD12:
	TST  R30
	MOV  R0,R30
	MOV  R30,R26
	MOV  R31,R27
	MOV  R22,R24
	MOV  R23,R25
	BREQ __LSLD12R
__LSLD12L:
	LSL  R30
	ROL  R31
	ROL  R22
	ROL  R23
	DEC  R0
	BRNE __LSLD12L
__LSLD12R:
	RET

__LSLD1:
	LSL  R30
	ROL  R31
	ROL  R22
	ROL  R23
	RET

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
	RET

__COMD1:
	COM  R30
	COM  R31
	COM  R22
	COM  R23
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

__GETD1P:
	LD   R30,X+
	LD   R31,X+
	LD   R22,X+
	LD   R23,X
	SBIW R26,3
	RET

__PUTDP1:
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	RET

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

__COPYMML:
	CLR  R25
__COPYMM:
	LD   R22,Z+
	ST   X+,R22
	SBIW R24,1
	BRNE __COPYMM
	RET

__CPD10:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
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

__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR3:
	LDD  R18,Y+2
__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

;END OF CODE MARKER
__END_OF_CODE:
