;CodeVisionAVR C Compiler V1.24.7b Evaluation
;(C) Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8515L
;Program type           : Application
;Clock frequency        : 8.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 128 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8515L
	#pragma AVRPART MEMORY PROG_FLASH 8192
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 512
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x60

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
	.EQU MCUCSR=0x34
	.EQU MCUCR=0x35
	.EQU EMCUCR=0x36
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

	.EQU __se_bit=0x20

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

	.MACRO __ADDB1MN
	SUBI R30,LOW(-@0-(@1))
	.ENDM
	.MACRO __ADDB2MN
	SUBI R26,LOW(-@0-(@1))
	.ENDM
	.MACRO __ADDW1MN
	SUBI R30,LOW(-@0-(@1))
	SBCI R31,HIGH(-@0-(@1))
	.ENDM
	.MACRO __ADDW2MN
	SUBI R26,LOW(-@0-(@1))
	SBCI R27,HIGH(-@0-(@1))
	.ENDM
	.MACRO __ADDW1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	.ENDM
	.MACRO __ADDD1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	SBCI R24,BYTE3(-2*@0-(@1))
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

	.MACRO __PUTD2S
	STD  Y+@0,R26
	STD  Y+@0+1,R27
	STD  Y+@0+2,R24
	STD  Y+@0+3,R25
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

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	LDI  R22,BYTE3(2*@0+@1)
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

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+@3)
	LDI  R@1,HIGH(@2*2+@3)
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

	.MACRO __GETB1HMN
	LDS  R31,@0+@1
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

	.MACRO __GETB1HSX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
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

	.MACRO __PUTD2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z+,R27
	ST   Z+,R24
	ST   Z,R25
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

	.INCLUDE "rc.vec"
	.INCLUDE "rc.inc"

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
	OUT  EMCUCR,R30

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
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.7b Evaluation
;       4 Automatic Program Generator
;       5 © Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 e-mail:office@hpinfotech.com
;       8 
;       9 Project : RC Hacker
;      10 Version : 1.0
;      11 Date    : 3/16/2006
;      12 Author  : Devinder Saini
;      13 Company : 
;      14 Comments: 
;      15 
;      16 
;      17 Chip type           : ATmega8515L
;      18 Program type        : Application
;      19 Clock frequency     : 8.000000 MHz
;      20 Memory model        : Small
;      21 External SRAM size  : 0
;      22 Data Stack size     : 128
;      23 *****************************************************/
;      24 
;      25 #include <mega8515.h>
;      26 
;      27 // Standard Input/Output functions
;      28 #include <stdio.h> 
;      29 #include <delay.h>
;      30 
;      31 
;      32 // ******** Following are user definable constants which 
;      33 // ******** can be set according to the application
;      34 
;      35 #define START_EDGE FALL             // If the transducer is initially at high level 
;      36                                     // or first edge is high to low
;      37                                     // then leave this at it is otherwise set RISE
;      38                                     
;      39 #define PRESCALER 0x02              // Default prescaler is divide by 8
;      40 
;      41 #define NUMBER_OF_FUNCTIONS 4       // Total number of functions that can be recorded
;      42 #define NUMBER_OF_POINTS 28         // Maximum number of transitions allowed in a function
;      43                                     // This number must be greater than the expected number
;      44 
;      45 #define LED_IDLE PORTC.7            // Indicators
;      46 #define LED_LISTEN PORTC.6
;      47 #define LED_TRANSMIT PORTC.5  
;      48 
;      49 //********* End of user options
;      50 
;      51 
;      52 
;      53 enum mode { IDLE, LISTEN, TRANSMIT } rc; 
;      54 enum edge { FALL=1, RISE=0 } e;
;      55 enum level{ LOW, HIGH  };
;      56 enum input_state {ACTIVE,DISABLED} switches;
;      57 
;      58 bit first_fall=0;
;      59 static char counter=0;
_counter_G1:
	.BYTE 0x1
;      60 char function_index=0;
;      61 
;      62 void set_edge(enum edge x)
;      63 {

	.CSEG
_set_edge:
;      64     if(x==FALL)
	LD   R26,Y
	CPI  R26,LOW(0x1)
	BRNE _0x3
;      65         TCCR1B&=0b10111111;
	IN   R30,0x2E
	ANDI R30,0xBF
	RJMP _0x7A
;      66     else
_0x3:
;      67         TCCR1B|=0b01000000;
	IN   R30,0x2E
	ORI  R30,0x40
_0x7A:
	OUT  0x2E,R30
;      68 }
	RJMP _0x79
;      69 
;      70 // Struct representing a point of transition
;      71 eeprom struct wave_point {             
;      72               int time_stamp; 
;      73               enum level logic;
;      74             };
;      75                          
;      76 
;      77 // Struct representing a complete code           
;      78 eeprom struct code_space {             
;      79               struct wave_point point[NUMBER_OF_POINTS];
;      80              };
;      81 
;      82 eeprom struct code_space *code_ptr;                     // Pointer to currently accessed function
;      83 eeprom struct code_space function[NUMBER_OF_FUNCTIONS]; // Actual functions

	.ESEG
_function:
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
	.DB  0x0
;      84 
;      85 
;      86 
;      87 
;      88 
;      89 /* This function sets the device in a mode-IDLE,LISTEN or TRANSMIT
;      90 IDLE-       TIMER1 stopped
;      91 LISTEN-     TIMER1 on, overflow interrupt on and input capture interrupt on
;      92 TRANSMIT-   TIMER1 on, overflow interrupt on and output compare match A interrupt on*/
;      93 void set_mode ( enum mode z )  
;      94 {       

	.CSEG
_set_mode:
;      95     TCCR1B&=0xF8;
	IN   R30,0x2E
	ANDI R30,LOW(0xF8)
	OUT  0x2E,R30
;      96     switch(z)
	LD   R30,Y
;      97     {
;      98         case IDLE   :  TIMSK=0x00;          // All timer interrupts off
	CPI  R30,0
	BRNE _0x8
	LDI  R30,LOW(0)
	OUT  0x39,R30
;      99                        rc=IDLE;
	CLR  R4
;     100                        LED_IDLE=0;          // Indicators
	CBI  0x15,7
;     101                        LED_LISTEN=1; 
	SBI  0x15,6
;     102                        LED_TRANSMIT=1;
	SBI  0x15,5
;     103                        break;
	RJMP _0x7
;     104         case LISTEN :  TCNT1=0;             // Start timer from 0
_0x8:
	CPI  R30,LOW(0x1)
	BRNE _0x9
	RCALL SUBOPT_0x0
;     105                        e=START_EDGE;              
	RCALL SUBOPT_0x1
;     106                        set_edge(e);
	ST   -Y,R5
	RCALL _set_edge
;     107                        first_fall=0;
	CLT
	BLD  R2,0
;     108                        counter=0;
	LDI  R30,LOW(0)
	STS  _counter_G1,R30
;     109                        TIFR^=0x88;          // Clear interrupt flags
	IN   R30,0x38
	LDI  R26,LOW(136)
	EOR  R30,R26
	OUT  0x38,R30
;     110                        TIMSK=0x88;          // Set overflow and input capture interrupts
	RCALL SUBOPT_0x2
;     111                        rc=LISTEN;                       
	LDI  R30,LOW(1)
	MOV  R4,R30
;     112                        LED_IDLE=1;
	SBI  0x15,7
;     113                        LED_LISTEN=0;
	CBI  0x15,6
;     114                        LED_TRANSMIT=1;
	SBI  0x15,5
;     115                        break;
	RJMP _0x7
;     116         case TRANSMIT: TCNT1=0;
_0x9:
	CPI  R30,LOW(0x2)
	BRNE _0x7
	RCALL SUBOPT_0x0
;     117                        rc=TRANSMIT;          
	LDI  R30,LOW(2)
	MOV  R4,R30
;     118                        counter=1;
	LDI  R30,LOW(1)
	STS  _counter_G1,R30
;     119                        TIMSK=0b11000000;    // Set overflow and output compare match interrupts
	LDI  R30,LOW(192)
	OUT  0x39,R30
;     120                        LED_IDLE=1;
	SBI  0x15,7
;     121                        LED_LISTEN=1;
	SBI  0x15,6
;     122                        LED_TRANSMIT=0;
	CBI  0x15,5
;     123                        TCCR1B|=PRESCALER;   // Start timer
	RCALL SUBOPT_0x3
;     124                        break;
;     125     }
_0x7:
;     126 } 
_0x79:
	ADIW R28,1
	RET
;     127               
;     128 
;     129 // Timer 1 overflow interrupt service routine
;     130 interrupt [TIM1_OVF] void timer1_ovf_isr(void)
;     131 {
_timer1_ovf_isr:
	RCALL SUBOPT_0x4
;     132 // Place your code here
;     133     int i;
;     134     enum mode rctemp;
;     135     rctemp=rc;
	RCALL __SAVELOCR3
;	i -> R16,R17
;	rctemp -> R18
	MOV  R18,R4
;     136     
;     137     set_mode(IDLE);             // Set IDLE mode
	RCALL SUBOPT_0x5
;     138     
;     139     if(rctemp==LISTEN)          // If just listened to a new function, then log the data to computer
	CPI  R18,1
	BRNE _0xB
;     140     {      
;     141         putsf("\nTIMEOUT. Code written\nListing");    
	__POINTW1FN _0,0
	RCALL SUBOPT_0x6
;     142         for(i=0;i<=NUMBER_OF_POINTS;i++)
	__GETWRN 16,17,0
_0xD:
	__CPWRN 16,17,29
	BRGE _0xE
;     143         {
;     144            printf("\nPoint %d: Time- %d, Logic Time+- %d",i,code_ptr->point[i].time_stamp,code_ptr->point[i].logic);
	__POINTW1FN _0,31
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R16
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0x8
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0x9
	RCALL __EEPROMRDB
	RCALL SUBOPT_0xA
	LDI  R24,12
	RCALL _printf
	ADIW R28,14
;     145         }
	__ADDWRN 16,17,1
	RJMP _0xD
_0xE:
;     146     }
;     147     if(rctemp==TRANSMIT)        // If just transmitted, then notify computer
_0xB:
	CPI  R18,2
	BRNE _0xF
;     148     {
;     149         printf("\nTransmitted function %c",function_index);
	__POINTW1FN _0,68
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R7
	RCALL SUBOPT_0xA
	LDI  R24,4
	RCALL _printf
	ADIW R28,6
;     150          
;     151     }
;     152     switches=ACTIVE;            // Reactivate switches
_0xF:
	CLR  R6
;     153 
;     154 }
	RCALL __LOADLOCR3
	ADIW R28,3
	RCALL SUBOPT_0xB
	RETI
;     155 
;     156 // Timer 1 input capture interrupt service routine
;     157 interrupt [TIM1_CAPT] void timer1_capt_isr(void)
;     158 {
_timer1_capt_isr:
	RCALL SUBOPT_0x4
;     159 // Place your code here
;     160     if(!first_fall)             // If first capture has occured, then start timer
	SBRC R2,0
	RJMP _0x10
;     161     {
;     162         TCCR1B|=PRESCALER;
	RCALL SUBOPT_0x3
;     163         first_fall=1;
	SET
	BLD  R2,0
;     164     }
;     165   
;     166     code_ptr->point[counter].time_stamp=ICR1;       // Record the current timer value     
_0x10:
	RCALL SUBOPT_0xC
	MOVW R26,R30
	IN   R30,0x24
	IN   R31,0x24+1
	RCALL __EEPROMWRW
;     167     code_ptr->point[counter].logic=PINE.0;          // Record current pin state
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0x9
	LDI  R30,0
	SBIC 0x5,0
	LDI  R30,1
	RCALL __EEPROMWRB
;     168     set_edge(PINE.0);                               // Set other edge
	LDI  R30,0
	SBIC 0x5,0
	LDI  R30,1
	ST   -Y,R30
	RCALL _set_edge
;     169     
;     170     counter++;     
	RCALL SUBOPT_0xD
;     171 
;     172 }
	RCALL SUBOPT_0xB
	RETI
;     173 
;     174 // Timer 1 output compare match interrupt service routine
;     175 interrupt [TIM1_COMPA] void timer1_compareA(void)
;     176 {     
_timer1_compareA:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     177     PORTD.5=code_ptr->point[counter].logic;         // Set the pin value to output
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0x9
	RCALL __EEPROMRDB
	RCALL __BSTB1
	IN   R26,0x12
	BLD  R26,5
	OUT  0x12,R26
;     178     OCR1A=code_ptr->point[++counter].time_stamp;    // Set OCR1A register to the next time stamp
	RCALL SUBOPT_0xD
	LDI  R31,0
	LDI  R26,LOW(3)
	LDI  R27,HIGH(3)
	RCALL __MULW12U
	ADD  R30,R8
	ADC  R31,R9
	RCALL SUBOPT_0x8
	OUT  0x2A+1,R31
	OUT  0x2A,R30
;     179 }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;     180 
;     181 // Declare your global variables here
;     182 
;     183 void main(void)
;     184 {
_main:
;     185 // Declare your local variables here
;     186 char temp;
;     187 
;     188 // Input/Output Ports initialization
;     189 // Port A initialization
;     190 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     191 // State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
;     192 PORTA=0xFF;
;	temp -> R16
	LDI  R30,LOW(255)
	OUT  0x1B,R30
;     193 DDRA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1A,R30
;     194 
;     195 // Port B initialization
;     196 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     197 // State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
;     198 PORTB=0xFF;
	LDI  R30,LOW(255)
	OUT  0x18,R30
;     199 DDRB=0x00;
	LDI  R30,LOW(0)
	OUT  0x17,R30
;     200 
;     201 // Port C initialization
;     202 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     203 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     204 PORTC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x15,R30
;     205 DDRC=0xFF;
	OUT  0x14,R30
;     206 
;     207 // Port D initialization
;     208 // Func7=In Func6=In Func5=Out Func4=In Func3=In Func2=In Func1=In Func0=In 
;     209 // State7=P State6=P State5=1 State4=P State3=P State2=P State1=P State0=P 
;     210 PORTD=0xFF;
	OUT  0x12,R30
;     211 DDRD=0x20;
	LDI  R30,LOW(32)
	OUT  0x11,R30
;     212 
;     213 // Port E initialization
;     214 // Func2=In Func1=In Func0=In 
;     215 // State2=P State1=P State0=P 
;     216 PORTE=0x07;
	LDI  R30,LOW(7)
	OUT  0x7,R30
;     217 DDRE=0x00;
	LDI  R30,LOW(0)
	OUT  0x6,R30
;     218 
;     219 // Timer/Counter 0 initialization
;     220 // Clock source: System Clock
;     221 // Clock value: Timer 0 Stopped
;     222 // Mode: Normal top=FFh
;     223 // OC0 output: Disconnected
;     224 TCCR0=0x00;
	OUT  0x33,R30
;     225 TCNT0=0x00;
	OUT  0x32,R30
;     226 OCR0=0x00;
	OUT  0x31,R30
;     227 
;     228 // Timer/Counter 1 initialization
;     229 // Clock source: System Clock
;     230 // Clock value: 1000.000 kHz
;     231 // Mode: Normal top=FFFFh
;     232 // OC1A output: Toggle
;     233 // OC1B output: Discon.
;     234 // Noise Canceler: Off
;     235 // Input Capture on Falling Edge
;     236 // Timer 1 Overflow Interrupt: On
;     237 // Input Capture Interrupt: On
;     238 // Compare A Match Interrupt: Off
;     239 // Compare B Match Interrupt: Off
;     240 TCCR1A=0x40;
	LDI  R30,LOW(64)
	OUT  0x2F,R30
;     241 TCCR1B=0x02;
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     242 TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     243 TCNT1L=0x00;
	OUT  0x2C,R30
;     244 ICR1H=0x00;
	OUT  0x25,R30
;     245 ICR1L=0x00;
	OUT  0x24,R30
;     246 OCR1AH=0x00;
	OUT  0x2B,R30
;     247 OCR1AL=0x00;
	OUT  0x2A,R30
;     248 OCR1BH=0x00;
	OUT  0x29,R30
;     249 OCR1BL=0x00;
	OUT  0x28,R30
;     250 
;     251 
;     252 
;     253 // External Interrupt(s) initialization
;     254 // INT0: On
;     255 // INT0 Mode: Low level
;     256 // INT1: Off
;     257 // INT2: Off
;     258 GICR|=0b00000000;
	IN   R30,0x3B
	OUT  0x3B,R30
;     259 MCUCR=0x00;
	LDI  R30,LOW(0)
	OUT  0x35,R30
;     260 EMCUCR=0x00;
	OUT  0x36,R30
;     261 GIFR=0b00000000;
	OUT  0x3A,R30
;     262 
;     263 // Timer(s)/Counter(s) Interrupt(s) initialization
;     264 TIMSK=0x88;
	RCALL SUBOPT_0x2
;     265 
;     266 // USART initialization
;     267 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     268 // USART Receiver: Off
;     269 // USART Transmitter: On
;     270 // USART Mode: Asynchronous
;     271 // USART Baud rate: 9600
;     272 UCSRA=0x00;
	LDI  R30,LOW(0)
	OUT  0xB,R30
;     273 UCSRB=0x08;
	LDI  R30,LOW(8)
	OUT  0xA,R30
;     274 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     275 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     276 UBRRL=0x33;
	LDI  R30,LOW(51)
	OUT  0x9,R30
;     277 
;     278 // Analog Comparator initialization
;     279 // Analog Comparator: Off
;     280 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     281 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     282 
;     283 // Global enable interrupts
;     284 #asm("sei")
	sei
;     285 
;     286 set_mode(IDLE);
	RCALL SUBOPT_0x5
;     287 e=FALL;
	RCALL SUBOPT_0x1
;     288 code_ptr=&function[0];
	LDI  R30,LOW(_function)
	LDI  R31,HIGH(_function)
	MOVW R8,R30
;     289 
;     290 while (1)
_0x11:
;     291       {
;     292       // Place your code here 
;     293 /*      #asm
;     294         nop
;     295         nop
;     296       #endasm
;     297 */
;     298     #asm("nop");
	nop
;     299     if((PINA!=0xFF)&&(switches==ACTIVE))
	IN   R30,0x19
	CPI  R30,LOW(0xFF)
	BREQ _0x15
	LDI  R30,LOW(0)
	CP   R30,R6
	BREQ _0x16
_0x15:
	RJMP _0x14
_0x16:
;     300     {
;     301         switches=DISABLED;
	LDI  R30,LOW(1)
	MOV  R6,R30
;     302         temp=PINA;
	IN   R16,25
;     303         delay_ms(1000);  
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _delay_ms
;     304         switch(temp)
	MOV  R30,R16
;     305         {
;     306             case 0b11111110 :   function_index=++function_index%NUMBER_OF_FUNCTIONS;    // Change Function
	CPI  R30,LOW(0xFE)
	BRNE _0x1A
	INC  R7
	LDI  R30,LOW(3)
	AND  R7,R30
;     307                                 PORTC=PORTC&0xF0;
	IN   R30,0x15
	ANDI R30,LOW(0xF0)
	OUT  0x15,R30
;     308                                 PORTC|=function_index^0xFF;
	IN   R30,0x15
	MOV  R26,R30
	LDI  R30,LOW(255)
	EOR  R30,R7
	OR   R30,R26
	OUT  0x15,R30
;     309                                 code_ptr=&function[function_index]; 
	MOV  R30,R7
	LDI  R31,0
	LDI  R26,LOW(84)
	LDI  R27,HIGH(84)
	RCALL __MULW12U
	SUBI R30,LOW(-_function)
	SBCI R31,HIGH(-_function)
	MOVW R8,R30
;     310                                 switches=ACTIVE;   
	CLR  R6
;     311                                 break;
	RJMP _0x19
;     312             case 0b11111101 :   set_mode(TRANSMIT);                   //Transmit
_0x1A:
	CPI  R30,LOW(0xFD)
	BRNE _0x1B
	LDI  R30,LOW(2)
	RJMP _0x7B
;     313                                 break;
;     314             case 0b11111011 :   putsf("\n\n-----------Listening ...\n");   
_0x1B:
	CPI  R30,LOW(0xFB)
	BRNE _0x19
	__POINTW1FN _0,93
	RCALL SUBOPT_0x6
;     315                                 set_mode(LISTEN);
	LDI  R30,LOW(1)
_0x7B:
	ST   -Y,R30
	RCALL _set_mode
;     316                                 break;
;     317         } 
_0x19:
;     318    
;     319 
;     320       };
_0x14:
;     321     } 
	RJMP _0x11
;     322 }
_0x1D:
	NOP
	RJMP _0x1D

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
	RCALL SUBOPT_0xE
	SBIW R30,0
	BREQ _0x1E
	RCALL SUBOPT_0xE
	ADIW R30,1
	ST   X+,R30
	ST   X,R31
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x1F
_0x1E:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x1F:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0x20:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	RCALL SUBOPT_0xF
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x22
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x26
	CPI  R19,37
	BRNE _0x27
	LDI  R16,LOW(1)
	RJMP _0x28
_0x27:
	RCALL SUBOPT_0x10
_0x28:
	RJMP _0x25
_0x26:
	CPI  R30,LOW(0x1)
	BRNE _0x29
	CPI  R19,37
	BRNE _0x2A
	RCALL SUBOPT_0x10
	LDI  R16,LOW(0)
	RJMP _0x25
_0x2A:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x2B
	LDI  R17,LOW(1)
	RJMP _0x25
_0x2B:
	CPI  R19,43
	BRNE _0x2C
	LDI  R21,LOW(43)
	RJMP _0x25
_0x2C:
	CPI  R19,32
	BRNE _0x2D
	LDI  R21,LOW(32)
	RJMP _0x25
_0x2D:
	RJMP _0x2E
_0x29:
	CPI  R30,LOW(0x2)
	BRNE _0x2F
_0x2E:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x30
	ORI  R17,LOW(128)
	RJMP _0x25
_0x30:
	RJMP _0x31
_0x2F:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x25
_0x31:
	CPI  R19,48
	BRLO _0x34
	CPI  R19,58
	BRLO _0x35
_0x34:
	RJMP _0x33
_0x35:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOV  R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x25
_0x33:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x39
	RCALL SUBOPT_0x11
	LD   R30,X
	RCALL SUBOPT_0x12
	RJMP _0x3A
_0x39:
	CPI  R30,LOW(0x73)
	BRNE _0x3C
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x13
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x3D
_0x3C:
	CPI  R30,LOW(0x70)
	BRNE _0x3F
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x13
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x3D:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x40
_0x3F:
	CPI  R30,LOW(0x64)
	BREQ _0x43
	CPI  R30,LOW(0x69)
	BRNE _0x44
_0x43:
	ORI  R17,LOW(4)
	RJMP _0x45
_0x44:
	CPI  R30,LOW(0x75)
	BRNE _0x46
_0x45:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x47
_0x46:
	CPI  R30,LOW(0x58)
	BRNE _0x49
	ORI  R17,LOW(8)
	RJMP _0x4A
_0x49:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x78
_0x4A:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x47:
	SBRS R17,2
	RJMP _0x4C
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x14
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x4D
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x4D:
	CPI  R21,0
	BREQ _0x4E
	SUBI R16,-LOW(1)
	RJMP _0x4F
_0x4E:
	ANDI R17,LOW(251)
_0x4F:
	RJMP _0x50
_0x4C:
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x14
_0x50:
_0x40:
	SBRC R17,0
	RJMP _0x51
_0x52:
	CP   R16,R20
	BRSH _0x54
	SBRS R17,7
	RJMP _0x55
	SBRS R17,2
	RJMP _0x56
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x57
_0x56:
	LDI  R19,LOW(48)
_0x57:
	RJMP _0x58
_0x55:
	LDI  R19,LOW(32)
_0x58:
	RCALL SUBOPT_0x10
	SUBI R20,LOW(1)
	RJMP _0x52
_0x54:
_0x51:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x59
_0x5A:
	CPI  R18,0
	BREQ _0x5C
	SBRS R17,3
	RJMP _0x5D
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	RCALL SUBOPT_0xF
	RJMP _0x7C
_0x5D:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x7C:
	ST   -Y,R30
	RCALL SUBOPT_0x15
	CPI  R20,0
	BREQ _0x5F
	SUBI R20,LOW(1)
_0x5F:
	SUBI R18,LOW(1)
	RJMP _0x5A
_0x5C:
	RJMP _0x60
_0x59:
_0x62:
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
	CPI  R19,58
	BRLO _0x64
	SBRS R17,3
	RJMP _0x65
	SUBI R19,-LOW(7)
	RJMP _0x66
_0x65:
	SUBI R19,-LOW(39)
_0x66:
_0x64:
	SBRC R17,4
	RJMP _0x68
	CPI  R19,49
	BRSH _0x6A
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0x69
_0x6A:
	ORI  R17,LOW(16)
	RJMP _0x6C
_0x69:
	CP   R20,R18
	BRLO _0x6E
	SBRS R17,0
	RJMP _0x6F
_0x6E:
	RJMP _0x6D
_0x6F:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x70
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x6C:
	SBRS R17,2
	RJMP _0x71
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0x15
	CPI  R20,0
	BREQ _0x72
	SUBI R20,LOW(1)
_0x72:
_0x71:
_0x70:
_0x68:
	RCALL SUBOPT_0x10
	CPI  R20,0
	BREQ _0x73
	SUBI R20,LOW(1)
_0x73:
_0x6D:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0x63
	RJMP _0x62
_0x63:
_0x60:
	SBRS R17,0
	RJMP _0x74
_0x75:
	CPI  R20,0
	BREQ _0x77
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x12
	RJMP _0x75
_0x77:
_0x74:
_0x78:
_0x3A:
	LDI  R16,LOW(0)
_0x25:
	RJMP _0x20
_0x22:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	RCALL __SAVELOCR2
	MOVW R26,R28
	RCALL __ADDW2R15
	MOVW R16,R26
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
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x2C+1,R31
	OUT  0x2C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	LDI  R30,LOW(1)
	MOV  R5,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	LDI  R30,LOW(136)
	OUT  0x39,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	IN   R30,0x2E
	ORI  R30,2
	OUT  0x2E,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
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
SUBOPT_0x5:
	LDI  R30,LOW(0)
	ST   -Y,R30
	RJMP _set_mode

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _putsf

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	RCALL __CWD1
	RCALL __PUTPARD1
	MOVW R30,R16
	LDI  R26,LOW(3)
	LDI  R27,HIGH(3)
	RCALL __MULW12U
	ADD  R30,R8
	ADC  R31,R9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	MOVW R26,R30
	RCALL __EEPROMRDW
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x9:
	ADIW R30,2
	MOVW R26,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
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

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xC:
	LDS  R30,_counter_G1
	LDI  R31,0
	LDI  R26,LOW(3)
	LDI  R27,HIGH(3)
	RCALL __MULW12U
	ADD  R30,R8
	ADC  R31,R9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	LDS  R30,_counter_G1
	SUBI R30,-LOW(1)
	STS  _counter_G1,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xF:
	SBIW R30,1
	LPM  R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x10:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x11:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	ST   -Y,R31
	ST   -Y,R30
	RJMP __put_G2

_putsf:
	ld   r30,y+
	ld   r31,y+
__putsf0:
	lpm  r0,z+
	tst  r0
	breq __putsf1
	push r30
	push r31
	rcall __putsf2
	pop  r31
	pop  r30
	rjmp __putsf0
__putsf1:
	ldi  r22,10
	mov  r0,r22
__putsf2:
	st   -y,r0
	rjmp _putchar

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

__MULW12U:
	MUL  R31,R26
	MOV  R31,R0
	MUL  R30,R27
	ADD  R31,R0
	MUL  R30,R26
	MOV  R30,R0
	ADD  R31,R1
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

__EEPROMWRW:
	RCALL __EEPROMWRB
	ADIW R26,1
	PUSH R30
	MOV  R30,R31
	RCALL __EEPROMWRB
	POP  R30
	SBIW R26,1
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

__BSTB1:
	CLT
	CLR  R0
	CPSE R30,R0
	SET
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
