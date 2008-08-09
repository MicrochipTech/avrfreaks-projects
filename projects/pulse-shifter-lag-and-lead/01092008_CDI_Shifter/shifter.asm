
;CodeVisionAVR C Compiler V1.25.7 beta 5 Professional
;(C) Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8535
;Program type           : Application
;Clock frequency        : 16.000000 MHz
;Memory model           : Small
;Optimize for           : Speed
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
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8535
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
	SBCI R22,BYTE3(-2*@0-(@1))
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

	.MACRO __ANDBMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ANDI R30,HIGH(@2)
	STS  @0+@1+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ORI  R30,HIGH(@2)
	STS  @0+@1+1,R30
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

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	LDI  R22,BYTE3(2*@0+@1)
	LDI  R23,BYTE4(2*@0+@1)
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

	.MACRO __PUTBR0MN
	STS  @0+@1,R0
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
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRR0
	MULS R@0,R@1
	.ENDM

	.MACRO __MULBRRU0
	MUL  R@0,R@1
	.ENDM

	.MACRO __MULBNWRU
	LDI  R26,@2
	MUL  R26,R@0
	MOVW R30,R0
	MUL  R26,R@1
	ADD  R31,R0
	.ENDM

	.CSEG
	.ORG 0

	.INCLUDE "shifter.vec"
	.INCLUDE "shifter.inc"

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
;       1 /*****************************************************
;       2 Project : Shifter_CDI
;       3 Version : 1.5
;       4 Date    : 9/8/2007
;       5 Author  : hendyeka
;       6 Company : FreeLancer
;       7 Comments:
;       8 
;       9 
;      10 Chip type           : ATmega8535
;      11 Program type        : Application
;      12 Clock frequency     : 16.000000 MHz
;      13 Memory model        : Small
;      14 External SRAM size  : 0
;      15 Data Stack size     : 128
;      16 *****************************************************/
;      17 
;      18 #include <mega8535.h>
;      19 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;      20 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;      21 	.EQU __se_bit=0x40
	.EQU __se_bit=0x40
;      22 	.EQU __sm_mask=0xB0
	.EQU __sm_mask=0xB0
;      23 	.EQU __sm_powerdown=0x20
	.EQU __sm_powerdown=0x20
;      24 	.EQU __sm_powersave=0x30
	.EQU __sm_powersave=0x30
;      25 	.EQU __sm_standby=0xA0
	.EQU __sm_standby=0xA0
;      26 	.EQU __sm_ext_standby=0xB0
	.EQU __sm_ext_standby=0xB0
;      27 	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_adc_noise_red=0x10
;      28 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;      29 	#endif
	#endif
;      30 #include <math.h>
;      31 #include <stdio.h>
;      32 
;      33 // Alphanumeric LCD Module functions
;      34 #asm
;      35    .equ __lcd_port=0x1B ;PORTA
   .equ __lcd_port=0x1B ;PORTA
;      36 #endasm
;      37 #include <lcd.h>
;      38 
;      39 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      40 
;      41 #define pin_data                PIND.2
;      42 #define port_output             PORTD.7
;      43 #define pin_up                  PINC.0
;      44 #define pin_down                PINC.1
;      45 #define pin_timer_up            PINC.2
;      46 #define pin_timer_down          PINC.3
;      47 
;      48 #define tdk_diaktifkan          1
;      49 #define mgkn_diaktifkan         2
;      50 #define diaktifkan              3
;      51 #define mgkn_tdk_diaktifkan     4
;      52 
;      53 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      54 
;      55 #define t1                      5000000/20
;      56 #define t2                      2000/20
;      57 #define t3                      3000/20
;      58 #define t4                      1000000/20
;      59 #define t5                      350000/20
;      60 
;      61 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      62 
;      63 #define jml_timer               10
;      64 
;      65 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      66 
;      67 unsigned int waktu1;
;      68 unsigned char waktu2;
;      69 unsigned char waktu3;
;      70 unsigned int waktu4;
;      71 unsigned int waktu5;
;      72 
;      73 char buff[33];
_buff:
	.BYTE 0x21
;      74 
;      75 char keadaan_pin_up;
;      76 bit tanda_pin_up;
;      77 
;      78 char keadaan_pin_down;
;      79 bit tanda_pin_down;
;      80 
;      81 char keadaan_pin_timer_up;
_keadaan_pin_timer_up:
	.BYTE 0x1
;      82 bit tanda_pin_timer_up;
;      83 
;      84 char keadaan_pin_timer_down;
_keadaan_pin_timer_down:
	.BYTE 0x1
;      85 bit tanda_pin_timer_down;
;      86 
;      87 bit penanda_ext_interrupt_rise;
;      88 
;      89 bit penanda_ukur_logika1;
;      90 bit penanda_sedang_ukur_logika1;
;      91 bit penanda_ukur_1periode;
;      92 bit penanda_sedang_ukur_1periode;
;      93 unsigned int durasi_logika1, n;
_durasi_logika1:
	.BYTE 0x2
_n:
	.BYTE 0x2
;      94 unsigned long int durasi_logika0;
_durasi_logika0:
	.BYTE 0x4
;      95 unsigned long int durasi_1periode;
_durasi_1periode:
	.BYTE 0x4
;      96 
;      97 unsigned int jml_pulsa;
_jml_pulsa:
	.BYTE 0x2
;      98 unsigned int rpm;
_rpm:
	.BYTE 0x2
;      99 unsigned long int timing;
_timing:
	.BYTE 0x4
;     100 unsigned int time_delay;
_time_delay:
	.BYTE 0x2
;     101 signed int time_delay_step;// -20 s.d. 60
_time_delay_step:
	.BYTE 0x2
;     102 unsigned int nilai_mutlak;
_nilai_mutlak:
	.BYTE 0x2
;     103 
;     104 signed char i;
_i:
	.BYTE 0x1
;     105 unsigned int tmp= 0;
_tmp:
	.BYTE 0x2
;     106 
;     107 // flash unsigned int data_acuan_us[7]=
;     108 // {
;     109 //         0,
;     110 //         5,
;     111 //         20,
;     112 //         50,
;     113 //         100,
;     114 //         250,
;     115 //         500
;     116 // };
;     117 
;     118 flash unsigned int data_acuan_us[jml_timer]=

	.CSEG
;     119 {
;     120         0,
;     121         1,
;     122         3,
;     123         5,
;     124         10,
;     125         25,
;     126         50,
;     127         100,
;     128         125,
;     129         250
;     130 };
;     131 
;     132 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     133 
;     134 void cek_pin_up(void)
;     135 {
_cek_pin_up:
;     136         switch(keadaan_pin_up)
	MOV  R30,R13
;     137         {
;     138                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x6
;     139                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x7
;     140                                 keadaan_pin_up= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x14F
;     141                         else
_0x7:
;     142                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x14F:
	MOV  R13,R30
;     143                         break;
	RJMP _0x5
;     144                 case mgkn_diaktifkan:
_0x6:
	CPI  R30,LOW(0x2)
	BRNE _0x9
;     145                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xA
;     146                         {
;     147                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     148                                 tanda_pin_up= 1;
	SET
	BLD  R2,0
;     149                         }
;     150                         else
	RJMP _0xB
_0xA:
;     151                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     152                         break;
_0xB:
	RJMP _0x5
;     153                 case diaktifkan:
_0x9:
	CPI  R30,LOW(0x3)
	BRNE _0xC
;     154                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xD
;     155                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x150
;     156                         else
_0xD:
;     157                                 keadaan_pin_up= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x150:
	MOV  R13,R30
;     158                         break;
	RJMP _0x5
;     159                 case mgkn_tdk_diaktifkan:
_0xC:
	CPI  R30,LOW(0x4)
	BRNE _0x5
;     160                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x10
;     161                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     162                         else
	RJMP _0x11
_0x10:
;     163                         {
;     164                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     165                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     166                         }
_0x11:
;     167                         break;
;     168         }
_0x5:
;     169 }
	RET
;     170 
;     171 void cek_pin_down(void)
;     172 {
_cek_pin_down:
;     173         switch(keadaan_pin_down)
	MOV  R30,R12
;     174         {
;     175                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x15
;     176                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x16
;     177                                 keadaan_pin_down= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x151
;     178                         else
_0x16:
;     179                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x151:
	MOV  R12,R30
;     180                         break;
	RJMP _0x14
;     181                 case mgkn_diaktifkan:
_0x15:
	CPI  R30,LOW(0x2)
	BRNE _0x18
;     182                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x19
;     183                         {
;     184                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     185                                 tanda_pin_down= 1;
	SET
	BLD  R2,1
;     186                         }
;     187                         else
	RJMP _0x1A
_0x19:
;     188                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     189                         break;
_0x1A:
	RJMP _0x14
;     190                 case diaktifkan:
_0x18:
	CPI  R30,LOW(0x3)
	BRNE _0x1B
;     191                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1C
;     192                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x152
;     193                         else
_0x1C:
;     194                                 keadaan_pin_down= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x152:
	MOV  R12,R30
;     195                         break;
	RJMP _0x14
;     196                 case mgkn_tdk_diaktifkan:
_0x1B:
	CPI  R30,LOW(0x4)
	BRNE _0x14
;     197                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1F
;     198                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     199                         else
	RJMP _0x20
_0x1F:
;     200                         {
;     201                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     202                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     203                         }
_0x20:
;     204                         break;
;     205         }
_0x14:
;     206 }
	RET
;     207 
;     208 void cek_pin_timer_up(void)
;     209 {
_cek_pin_timer_up:
;     210         switch(keadaan_pin_timer_up)
	LDS  R30,_keadaan_pin_timer_up
;     211         {
;     212                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x24
;     213                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x25
;     214                                 keadaan_pin_timer_up= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x153
;     215                         else
_0x25:
;     216                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x153:
	STS  _keadaan_pin_timer_up,R30
;     217                         break;
	RJMP _0x23
;     218                 case mgkn_diaktifkan:
_0x24:
	CPI  R30,LOW(0x2)
	BRNE _0x27
;     219                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x28
;     220                         {
;     221                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_up,R30
;     222                                 tanda_pin_timer_up= 1;
	SET
	BLD  R2,2
;     223                         }
;     224                         else
	RJMP _0x29
_0x28:
;     225                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_up,R30
;     226                         break;
_0x29:
	RJMP _0x23
;     227                 case diaktifkan:
_0x27:
	CPI  R30,LOW(0x3)
	BRNE _0x2A
;     228                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x2B
;     229                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x154
;     230                         else
_0x2B:
;     231                                 keadaan_pin_timer_up= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x154:
	STS  _keadaan_pin_timer_up,R30
;     232                         break;
	RJMP _0x23
;     233                 case mgkn_tdk_diaktifkan:
_0x2A:
	CPI  R30,LOW(0x4)
	BRNE _0x23
;     234                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x2E
;     235                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_up,R30
;     236                         else
	RJMP _0x2F
_0x2E:
;     237                         {
;     238                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_up,R30
;     239                                 tanda_pin_timer_up= 0;
	CLT
	BLD  R2,2
;     240                         }
_0x2F:
;     241                         break;
;     242         }
_0x23:
;     243 }
	RET
;     244 
;     245 void cek_pin_timer_down(void)
;     246 {
_cek_pin_timer_down:
;     247         switch(keadaan_pin_timer_down)
	LDS  R30,_keadaan_pin_timer_down
;     248         {
;     249                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x33
;     250                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x34
;     251                                 keadaan_pin_timer_down= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x155
;     252                         else
_0x34:
;     253                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x155:
	STS  _keadaan_pin_timer_down,R30
;     254                         break;
	RJMP _0x32
;     255                 case mgkn_diaktifkan:
_0x33:
	CPI  R30,LOW(0x2)
	BRNE _0x36
;     256                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x37
;     257                         {
;     258                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_down,R30
;     259                                 tanda_pin_timer_down= 1;
	SET
	BLD  R2,3
;     260                         }
;     261                         else
	RJMP _0x38
_0x37:
;     262                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_down,R30
;     263                         break;
_0x38:
	RJMP _0x32
;     264                 case diaktifkan:
_0x36:
	CPI  R30,LOW(0x3)
	BRNE _0x39
;     265                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x3A
;     266                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x156
;     267                         else
_0x3A:
;     268                                 keadaan_pin_timer_down= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x156:
	STS  _keadaan_pin_timer_down,R30
;     269                         break;
	RJMP _0x32
;     270                 case mgkn_tdk_diaktifkan:
_0x39:
	CPI  R30,LOW(0x4)
	BRNE _0x32
;     271                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x3D
;     272                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_down,R30
;     273                         else
	RJMP _0x3E
_0x3D:
;     274                         {
;     275                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_down,R30
;     276                                 tanda_pin_timer_down= 0;
	CLT
	BLD  R2,3
;     277                         }
_0x3E:
;     278                         break;
;     279         }
_0x32:
;     280 }
	RET
;     281 
;     282 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     283 
;     284 void aktifkan_pengukur_logika1(void)
;     285 {
_aktifkan_pengukur_logika1:
;     286         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     287         TCCR1B=0x01;
	LDI  R30,LOW(1)
	OUT  0x2E,R30
;     288         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     289         TCNT1L=0x00;
	OUT  0x2C,R30
;     290         ICR1H=0x00;
	OUT  0x27,R30
;     291         ICR1L=0x00;
	OUT  0x26,R30
;     292         OCR1AH=0x00;
	OUT  0x2B,R30
;     293         OCR1AL=0x00;
	OUT  0x2A,R30
;     294         OCR1BH=0x00;
	OUT  0x29,R30
;     295         OCR1BL=0x00;
	OUT  0x28,R30
;     296 
;     297         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     298 }
	RET
;     299 
;     300 void aktifkan_pengukur_1periode(void)
;     301 {
_aktifkan_pengukur_1periode:
;     302         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     303         TCCR1B=0x03;
	LDI  R30,LOW(3)
	OUT  0x2E,R30
;     304         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     305         TCNT1L=0x00;
	OUT  0x2C,R30
;     306         ICR1H=0x00;
	OUT  0x27,R30
;     307         ICR1L=0x00;
	OUT  0x26,R30
;     308         OCR1AH=0x00;
	OUT  0x2B,R30
;     309         OCR1AL=0x00;
	OUT  0x2A,R30
;     310         OCR1BH=0x00;
	OUT  0x29,R30
;     311         OCR1BL=0x00;
	OUT  0x28,R30
;     312 
;     313         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     314 }
	RET
;     315 
;     316 // Timer shifter
;     317 void aktifkan_timer1_cmpA(unsigned long int nilai_compare)
;     318 {
_aktifkan_timer1_cmpA:
;     319         unsigned char presc= 0x00;
;     320         unsigned int nilai_ocr= 0x0000;
;     321 
;     322         port_output= 0;
	RCALL __SAVELOCR4
;	nilai_compare -> Y+4
;	presc -> R17
;	nilai_ocr -> R18,R19
	LDI  R17,0
	LDI  R18,0
	LDI  R19,0
	CBI  0x12,7
;     323 
;     324         if(nilai_compare > 4000)
	__GETD2S 4
	__CPD2N 0xFA1
	BRLO _0x3F
;     325         {
;     326                 presc= 0x03;
	LDI  R17,LOW(3)
;     327                 nilai_ocr= nilai_compare*0.25;
	__GETD1S 4
	RCALL __CDF1U
	__GETD2N 0x3E800000
	RCALL __MULF12
	RCALL __CFD1U
	RJMP _0x157
;     328         }
;     329         else
_0x3F:
;     330         {
;     331                 presc= 0x01;
	LDI  R17,LOW(1)
;     332                 nilai_ocr= nilai_compare*16;
	__GETD2S 4
	__GETD1N 0x10
	RCALL __MULD12U
_0x157:
	MOVW R18,R30
;     333         }
;     334 
;     335         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     336         TCCR1B= presc;
	OUT  0x2E,R17
;     337         TCNT1H=0x00;
	OUT  0x2D,R30
;     338         TCNT1L=0x00;
	OUT  0x2C,R30
;     339         ICR1H=0x00;
	OUT  0x27,R30
;     340         ICR1L=0x00;
	OUT  0x26,R30
;     341         OCR1A= nilai_ocr;
	__OUTWR 18,19,42
;     342         OCR1BH=0x00;
	OUT  0x29,R30
;     343         OCR1BL=0x00;
	OUT  0x28,R30
;     344 
;     345         TIMSK=0x12;
	LDI  R30,LOW(18)
	OUT  0x39,R30
;     346 }
	RJMP _0x14E
;     347 
;     348 // Timer logika
;     349 void aktifkan_timer1_cmpB(unsigned long int nilai_compare)
;     350 {
_aktifkan_timer1_cmpB:
;     351         unsigned char presc= 0x00;
;     352         unsigned int nilai_ocr= 0x0000;
;     353 
;     354         penanda_ext_interrupt_rise= 0;
	RCALL __SAVELOCR4
;	nilai_compare -> Y+4
;	presc -> R17
;	nilai_ocr -> R18,R19
	LDI  R17,0
	LDI  R18,0
	LDI  R19,0
	CLT
	BLD  R2,4
;     355 
;     356         if(nilai_compare > 4000)
	__GETD2S 4
	__CPD2N 0xFA1
	BRLO _0x41
;     357         {
;     358                 presc= 0x03;
	LDI  R17,LOW(3)
;     359                 nilai_ocr= nilai_compare*0.25;
	__GETD1S 4
	RCALL __CDF1U
	__GETD2N 0x3E800000
	RCALL __MULF12
	RCALL __CFD1U
	RJMP _0x158
;     360         }
;     361         else
_0x41:
;     362         {
;     363                 presc= 0x01;
	LDI  R17,LOW(1)
;     364                 nilai_ocr= nilai_compare*16;
	__GETD2S 4
	__GETD1N 0x10
	RCALL __MULD12U
_0x158:
	MOVW R18,R30
;     365         }
;     366 
;     367         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     368         TCCR1B= presc;
	OUT  0x2E,R17
;     369         TCNT1H=0x00;
	OUT  0x2D,R30
;     370         TCNT1L=0x00;
	OUT  0x2C,R30
;     371         ICR1H=0x00;
	OUT  0x27,R30
;     372         ICR1L=0x00;
	OUT  0x26,R30
;     373         OCR1AH=0x00;
	OUT  0x2B,R30
;     374         OCR1AL=0x00;
	OUT  0x2A,R30
;     375         OCR1B= nilai_ocr;
	__OUTWR 18,19,40
;     376 
;     377         TIMSK=0x0A;
	LDI  R30,LOW(10)
	OUT  0x39,R30
;     378 }
_0x14E:
	RCALL __LOADLOCR4
	ADIW R28,8
	RET
;     379 
;     380 void matikan_timer1(void)
;     381 {
_matikan_timer1:
;     382         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     383         TCCR1B=0x00;
	OUT  0x2E,R30
;     384         TCNT1H=0x00;
	OUT  0x2D,R30
;     385         TCNT1L=0x00;
	OUT  0x2C,R30
;     386         ICR1H=0x00;
	OUT  0x27,R30
;     387         ICR1L=0x00;
	OUT  0x26,R30
;     388         OCR1AH=0x00;
	OUT  0x2B,R30
;     389         OCR1AL=0x00;
	OUT  0x2A,R30
;     390         OCR1BH=0x00;
	OUT  0x29,R30
;     391         OCR1BL=0x00;
	OUT  0x28,R30
;     392 
;     393         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     394 }
	RET
;     395 
;     396 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     397 
;     398 void hitung_timing(void)
;     399 {
_hitung_timing:
;     400         if(time_delay_step > 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x43
;     401         {
;     402                 timing= (unsigned long int)time_delay*time_delay_step;
	LDS  R30,_time_delay
	LDS  R31,_time_delay+1
	CLR  R22
	CLR  R23
	MOVW R26,R30
	MOVW R24,R22
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	RCALL __CWD1
	RCALL __MULD12U
	STS  _timing,R30
	STS  _timing+1,R31
	STS  _timing+2,R22
	STS  _timing+3,R23
;     403         }
;     404         else if(time_delay_step < 0)
	RJMP _0x44
_0x43:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRLT PC+2
	RJMP _0x45
;     405         {
;     406                 nilai_mutlak= abs(time_delay_step);
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _abs
	STS  _nilai_mutlak,R30
	STS  _nilai_mutlak+1,R31
;     407                 tmp= time_delay*nilai_mutlak;
	LDS  R26,_time_delay
	LDS  R27,_time_delay+1
	RCALL __MULW12U
	STS  _tmp,R30
	STS  _tmp+1,R31
;     408                 if(durasi_logika1 > tmp)
	LDS  R26,_durasi_logika1
	LDS  R27,_durasi_logika1+1
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x46
;     409                 {
;     410                         timing= (unsigned long int)durasi_logika1-tmp;
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	MOVW R26,R30
	MOVW R24,R22
	LDS  R30,_tmp
	LDS  R31,_tmp+1
	CLR  R22
	CLR  R23
	RCALL __SUBD21
	STS  _timing,R26
	STS  _timing+1,R27
	STS  _timing+2,R24
	STS  _timing+3,R25
;     411                 }
;     412                 else
	RJMP _0x47
_0x46:
;     413                 {
;     414                         timing= 0;
	LDI  R30,0
	STS  _timing,R30
	STS  _timing+1,R30
	STS  _timing+2,R30
	STS  _timing+3,R30
;     415                 }
_0x47:
;     416         }
;     417         else if(time_delay_step == 0)
	RJMP _0x48
_0x45:
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x49
;     418                 timing= 0;
	LDI  R30,0
	STS  _timing,R30
	STS  _timing+1,R30
	STS  _timing+2,R30
	STS  _timing+3,R30
;     419 }
_0x49:
_0x48:
_0x44:
	RET
;     420 
;     421 void increment_shift(void)
;     422 {
_increment_shift:
;     423         if(time_delay_step < 60)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,60
	BRGE _0x4A
;     424         {
;     425                 ++time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ADIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     426         }
;     427         hitung_timing();
_0x4A:
	RCALL _hitung_timing
;     428 }
	RET
;     429 
;     430 void decrement_shift(void)
;     431 {
_decrement_shift:
;     432         if(time_delay_step > -20)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	LDI  R30,LOW(65516)
	LDI  R31,HIGH(65516)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x4B
;     433         {
;     434                 --time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     435         }
;     436         hitung_timing();
_0x4B:
	RCALL _hitung_timing
;     437 }
	RET
;     438 
;     439 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     440 
;     441 void hitung_rpm(void)
;     442 {
_hitung_rpm:
;     443         /**
;     444         Satu putaran adalah 1 gelombang.
;     445         jml_pulsa di sini adalah banyaknya gelombang yang terjadi setiap 1 s.
;     446         Satu menit adalah 60x (1 s).
;     447         Sehingga pendekatan perhitungan yang dicapai, rpm= (jml_pulsa*60) rotation per minute
;     448         **/
;     449         rpm= jml_pulsa*60;
	LDS  R26,_jml_pulsa
	LDS  R27,_jml_pulsa+1
	LDI  R30,LOW(60)
	RCALL __MULB1W2U
	STS  _rpm,R30
	STS  _rpm+1,R31
;     450         jml_pulsa= 0;
	LDI  R30,0
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R30
;     451 }
	RET
;     452 
;     453 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     454 
;     455 void tampil_lcd(void)
;     456 {
_tampil_lcd:
;     457         lcd_clear();
	RCALL _lcd_clear
;     458         lcd_putsf("td  d     RPM");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     459         sprintf(buff, "\n%2i %3i   %5i", time_delay_step, time_delay, rpm);
	LDI  R30,LOW(_buff)
	LDI  R31,HIGH(_buff)
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,14
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	RCALL __CWD1
	RCALL __PUTPARD1
	LDS  R30,_time_delay
	LDS  R31,_time_delay+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	LDS  R30,_rpm
	LDS  R31,_rpm+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	LDI  R24,12
	RCALL _sprintf
	ADIW R28,16
;     460         lcd_puts(buff);
	LDI  R30,LOW(_buff)
	LDI  R31,HIGH(_buff)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     461 }
	RET
;     462 
;     463 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     464 
;     465 // External Interrupt 0 service routine
;     466 interrupt [EXT_INT0] void ext_int0_isr(void)
;     467 {
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
;     468         if(pin_data == 1)
	SBIS 0x10,2
	RJMP _0x4C
;     469         {
;     470                 if(penanda_ukur_logika1)
	SBRS R2,5
	RJMP _0x4D
;     471                 {
;     472                         if(!penanda_sedang_ukur_logika1)
	SBRC R2,6
	RJMP _0x4E
;     473                         {
;     474                                 n= 0;
	LDI  R30,0
	STS  _n,R30
	STS  _n+1,R30
;     475                                 penanda_sedang_ukur_logika1= 1;
	SET
	BLD  R2,6
;     476                         }
;     477                 }
_0x4E:
;     478                 else if(penanda_ukur_1periode)
	RJMP _0x4F
_0x4D:
	SBRS R2,7
	RJMP _0x50
;     479                 {
;     480                         if(penanda_sedang_ukur_1periode)
	SBRS R3,0
	RJMP _0x51
;     481                         {
;     482                                 durasi_logika0= (unsigned long int)TCNT1*4;
	IN   R30,0x2C
	IN   R31,0x2C+1
	CLR  R22
	CLR  R23
	RCALL __LSLD1
	RCALL __LSLD1
	STS  _durasi_logika0,R30
	STS  _durasi_logika0+1,R31
	STS  _durasi_logika0+2,R22
	STS  _durasi_logika0+3,R23
;     483                                 matikan_timer1();
	RCALL _matikan_timer1
;     484                                 durasi_1periode= durasi_logika0+durasi_logika1;
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	LDS  R26,_durasi_logika0
	LDS  R27,_durasi_logika0+1
	LDS  R24,_durasi_logika0+2
	LDS  R25,_durasi_logika0+3
	CLR  R22
	CLR  R23
	RCALL __ADDD12
	STS  _durasi_1periode,R30
	STS  _durasi_1periode+1,R31
	STS  _durasi_1periode+2,R22
	STS  _durasi_1periode+3,R23
;     485                                 penanda_ukur_1periode= 0;
	CLT
	BLD  R2,7
;     486                                 penanda_sedang_ukur_1periode= 0;
	BLD  R3,0
;     487                         }
;     488                 }
_0x51:
;     489                 else
	RJMP _0x52
_0x50:
;     490                 {
;     491                         if(time_delay_step == 0)
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x53
;     492                         {
;     493                                 port_output= 1;
	SBI  0x12,7
;     494                                 aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     495                         }
;     496                         else if(time_delay_step > 0)
	RJMP _0x54
_0x53:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x55
;     497                         {
;     498                                 port_output= 0;
	CBI  0x12,7
;     499                                 aktifkan_timer1_cmpA(timing);// timing shift for logic 0
	LDS  R30,_timing
	LDS  R31,_timing+1
	LDS  R22,_timing+2
	LDS  R23,_timing+3
	RJMP _0x159
;     500                         }
;     501                         else if(time_delay_step < 0)
_0x55:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRLT PC+2
	RJMP _0x57
;     502                         {
;     503                                 /*if(timing > 0)
;     504                                 {
;     505                                         port_output= 1;
;     506                                         aktifkan_timer1_cmpB(timing);// timing shift for logic 1
;     507                                 }
;     508                                 else
;     509                                 {
;     510                                         port_output= 0;
;     511                                         aktifkan_timer1_cmpA(durasi_logika0);
;     512                                 }*/
;     513 
;     514                                 if(durasi_logika1 > tmp)
	LDS  R30,_tmp
	LDS  R31,_tmp+1
	LDS  R26,_durasi_logika1
	LDS  R27,_durasi_logika1+1
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x58
;     515                                 {
;     516                                         port_output= 1;
	SBI  0x12,7
;     517                                         aktifkan_timer1_cmpB(timing);// timing shift for logic 1
	LDS  R30,_timing
	LDS  R31,_timing+1
	LDS  R22,_timing+2
	LDS  R23,_timing+3
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     518                                 }
;     519                                 else if(durasi_logika1 < tmp)
	RJMP _0x59
_0x58:
	LDS  R30,_tmp
	LDS  R31,_tmp+1
	LDS  R26,_durasi_logika1
	LDS  R27,_durasi_logika1+1
	CP   R26,R30
	CPC  R27,R31
	BRSH _0x5A
;     520 				{
;     521                                         port_output= 0;
	CBI  0x12,7
;     522                                         aktifkan_timer1_cmpA(durasi_logika0-tmp);
	LDS  R26,_durasi_logika0
	LDS  R27,_durasi_logika0+1
	LDS  R24,_durasi_logika0+2
	LDS  R25,_durasi_logika0+3
	CLR  R22
	CLR  R23
	RCALL __SUBD21
	RCALL __PUTPARD2
	RJMP _0x15A
;     523                                 }
;     524 				else
_0x5A:
;     525                                 {
;     526                                         port_output= 0;
	CBI  0x12,7
;     527                                         aktifkan_timer1_cmpA(durasi_logika0);
	LDS  R30,_durasi_logika0
	LDS  R31,_durasi_logika0+1
	LDS  R22,_durasi_logika0+2
	LDS  R23,_durasi_logika0+3
_0x159:
	RCALL __PUTPARD1
_0x15A:
	RCALL _aktifkan_timer1_cmpA
;     528                                 }
_0x59:
;     529                         }
;     530                 }
_0x57:
_0x54:
_0x52:
_0x4F:
;     531                 ++jml_pulsa;
	LDS  R30,_jml_pulsa
	LDS  R31,_jml_pulsa+1
	ADIW R30,1
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R31
;     532                 penanda_ext_interrupt_rise= 1;
	SET
	BLD  R2,4
;     533         }
;     534 }
_0x4C:
	RJMP _0x15E
;     535 
;     536 // Timer 0 output compare interrupt service routine, base time 20 us
;     537 interrupt [TIM0_COMP] void timer0_comp_isr(void)
;     538 {
_timer0_comp_isr:
	ST   -Y,R0
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     539         TCNT0= 0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     540 
;     541         if(n < 64000)
	LDS  R26,_n
	LDS  R27,_n+1
	CPI  R26,LOW(0xFA00)
	LDI  R30,HIGH(0xFA00)
	CPC  R27,R30
	BRSH _0x5C
;     542                 ++n;
	LDS  R30,_n
	LDS  R31,_n+1
	ADIW R30,1
	STS  _n,R30
	STS  _n+1,R31
;     543         if(waktu1 > 0)
_0x5C:
	CLR  R0
	CP   R0,R4
	CPC  R0,R5
	BRSH _0x5D
;     544                 --waktu1;
	MOVW R30,R4
	SBIW R30,1
	MOVW R4,R30
;     545         if(waktu2 > 0)
_0x5D:
	LDI  R30,LOW(0)
	CP   R30,R7
	BRSH _0x5E
;     546                 --waktu2;
	DEC  R7
;     547         if(waktu3 > 0)
_0x5E:
	LDI  R30,LOW(0)
	CP   R30,R6
	BRSH _0x5F
;     548                 --waktu3;
	DEC  R6
;     549         if(waktu4 > 0)
_0x5F:
	CLR  R0
	CP   R0,R8
	CPC  R0,R9
	BRSH _0x60
;     550                 --waktu4;
	MOVW R30,R8
	SBIW R30,1
	MOVW R8,R30
;     551         if(waktu5 > 0)
_0x60:
	CLR  R0
	CP   R0,R10
	CPC  R0,R11
	BRSH _0x61
;     552                 --waktu5;
	MOVW R30,R10
	SBIW R30,1
	MOVW R10,R30
;     553 }
_0x61:
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R0,Y+
	RETI
;     554 
;     555 // Timer 1 output compare A interrupt service routine, timer shifter match
;     556 interrupt [TIM1_COMPA] void timer1_compa_isr(void)
;     557 {
_timer1_compa_isr:
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
;     558         port_output= 1;
	SBI  0x12,7
;     559         matikan_timer1();
	RCALL _matikan_timer1
;     560         //if(time_delay_step > 0)
;     561         //{
;     562                 aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     563         //}
;     564 }
	RJMP _0x15E
;     565 
;     566 // Timer 1 output compare B interrupt service routine, timer logika 1 match
;     567 interrupt [TIM1_COMPB] void timer1_compb_isr(void)
;     568 {
_timer1_compb_isr:
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
;     569         port_output= 0;
	CBI  0x12,7
;     570         matikan_timer1();
	RCALL _matikan_timer1
;     571         if(time_delay_step < 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x62
;     572         {
;     573                 aktifkan_timer1_cmpA(durasi_logika0);
	LDS  R30,_durasi_logika0
	LDS  R31,_durasi_logika0+1
	LDS  R22,_durasi_logika0+2
	LDS  R23,_durasi_logika0+3
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpA
;     574         }
;     575 }
_0x62:
_0x15E:
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
;     576 
;     577 void main(void)
;     578 {
_main:
;     579         /**
;     580         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     581         Port A -- Port LCD
;     582         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     583         **/
;     584         PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     585         DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     586 
;     587         /**
;     588         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     589         Port B -- Dummies
;     590         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     591         **/
;     592         PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;     593         DDRB=0x00;
	OUT  0x17,R30
;     594 
;     595         /**
;     596         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     597         Port C -- Kontrol Shifter
;     598         C.0: Up
;     599         C.1: Down
;     600         C.2: Timer Up
;     601         C.3: Timer Down
;     602         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     603         **/
;     604         PORTC=0x0F;
	LDI  R30,LOW(15)
	OUT  0x15,R30
;     605         DDRC=0x00;
	LDI  R30,LOW(0)
	OUT  0x14,R30
;     606 
;     607         /**
;     608         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     609         Port D
;     610         D.2: INT0
;     611         D.7: Shifted Output Pulse
;     612         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     613         **/
;     614         PORTD=0x04;
	LDI  R30,LOW(4)
	OUT  0x12,R30
;     615         DDRD=0x80;
	LDI  R30,LOW(128)
	OUT  0x11,R30
;     616 
;     617         // Timer/Counter 0 initialization
;     618         // Clock source: System Clock
;     619         // Clock value: 2000.000 kHz
;     620         // Mode: Normal top=FFh
;     621         // OC0 output: Disconnected
;     622         TCCR0=0x02;
	LDI  R30,LOW(2)
	OUT  0x33,R30
;     623         TCNT0=0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     624         OCR0=0x28;
	LDI  R30,LOW(40)
	OUT  0x3C,R30
;     625 
;     626         // Timer/Counter 1 initialization
;     627         // Clock source: System Clock
;     628         // Clock value: Timer 1 Stopped
;     629         // Mode: Normal top=FFFFh
;     630         // OC1A output: Discon.
;     631         // OC1B output: Discon.
;     632         // Noise Canceler: Off
;     633         // Input Capture on Falling Edge
;     634         // Timer 1 Overflow Interrupt: Off
;     635         // Input Capture Interrupt: Off
;     636         // Compare A Match Interrupt: Off
;     637         // Compare B Match Interrupt: Off
;     638         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     639         TCCR1B=0x00;
	OUT  0x2E,R30
;     640         TCNT1H=0x00;
	OUT  0x2D,R30
;     641         TCNT1L=0x00;
	OUT  0x2C,R30
;     642         ICR1H=0x00;
	OUT  0x27,R30
;     643         ICR1L=0x00;
	OUT  0x26,R30
;     644         OCR1AH=0x00;
	OUT  0x2B,R30
;     645         OCR1AL=0x00;
	OUT  0x2A,R30
;     646         OCR1BH=0x00;
	OUT  0x29,R30
;     647         OCR1BL=0x00;
	OUT  0x28,R30
;     648 
;     649         // Timer/Counter 2 initialization
;     650         // Clock source: System Clock
;     651         // Clock value: Timer 2 Stopped
;     652         // Mode: Normal top=FFh
;     653         // OC2 output: Disconnected
;     654         ASSR=0x00;
	OUT  0x22,R30
;     655         TCCR2=0x00;
	OUT  0x25,R30
;     656         TCNT2=0x00;
	OUT  0x24,R30
;     657         OCR2=0x00;
	OUT  0x23,R30
;     658 
;     659         // External Interrupt(s) initialization
;     660         // INT0: On
;     661         // INT0 Mode: Rising Edge
;     662         // INT1: Off
;     663         // INT2: Off
;     664         GICR|=0x40;
	IN   R30,0x3B
	ORI  R30,0x40
	OUT  0x3B,R30
;     665         MCUCR=0x03;
	LDI  R30,LOW(3)
	OUT  0x35,R30
;     666         MCUCSR=0x00;
	LDI  R30,LOW(0)
	OUT  0x34,R30
;     667         GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
;     668 
;     669         // Timer(s)/Counter(s) Interrupt(s) initialization
;     670         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     671 
;     672         // Analog Comparator initialization
;     673         // Analog Comparator: Off
;     674         // Analog Comparator Input Capture by Timer/Counter 1: Off
;     675         ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     676         SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     677 
;     678         // LCD module initialization
;     679         lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
;     680         lcd_clear();
	RCALL _lcd_clear
;     681 
;     682         // Global enable interrupts
;     683         #asm("sei")
	sei
;     684 
;     685         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,4
;     686         // Dapatkan data awal durasi pulsa logika 1
;     687         while(!penanda_ext_interrupt_rise);
_0x63:
	SBRS R2,4
	RJMP _0x63
;     688         #asm("cli")
	cli
;     689         aktifkan_pengukur_logika1();
	RCALL _aktifkan_pengukur_logika1
;     690         while(pin_data);
_0x66:
	SBIC 0x10,2
	RJMP _0x66
;     691         durasi_logika1= TCNT1/16;
	IN   R30,0x2C
	IN   R31,0x2C+1
	RCALL __LSRW4
	STS  _durasi_logika1,R30
	STS  _durasi_logika1+1,R31
;     692         matikan_timer1();
	RCALL _matikan_timer1
;     693 
;     694         // Global enable interrupts
;     695         #asm("sei")
	sei
;     696 
;     697         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,4
;     698         // Dapatkan data awal durasi 1 periode pulsa
;     699         while(!penanda_ext_interrupt_rise);
_0x69:
	SBRS R2,4
	RJMP _0x69
;     700         #asm("cli")
	cli
;     701         aktifkan_pengukur_1periode();
	RCALL _aktifkan_pengukur_1periode
;     702         while(pin_data);
_0x6C:
	SBIC 0x10,2
	RJMP _0x6C
;     703         while(!pin_data);
_0x6F:
	SBIS 0x10,2
	RJMP _0x6F
;     704         durasi_1periode= (unsigned long int)TCNT1*4;
	IN   R30,0x2C
	IN   R31,0x2C+1
	CLR  R22
	CLR  R23
	RCALL __LSLD1
	RCALL __LSLD1
	STS  _durasi_1periode,R30
	STS  _durasi_1periode+1,R31
	STS  _durasi_1periode+2,R22
	STS  _durasi_1periode+3,R23
;     705         matikan_timer1();
	RCALL _matikan_timer1
;     706 
;     707         // Dapatkan data awal durasi pulsa logika 0
;     708         durasi_logika0= durasi_1periode-durasi_logika1;
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	LDS  R26,_durasi_1periode
	LDS  R27,_durasi_1periode+1
	LDS  R24,_durasi_1periode+2
	LDS  R25,_durasi_1periode+3
	CLR  R22
	CLR  R23
	RCALL __SUBD21
	STS  _durasi_logika0,R26
	STS  _durasi_logika0+1,R27
	STS  _durasi_logika0+2,R24
	STS  _durasi_logika0+3,R25
;     709 
;     710         // Global disable interrupts
;     711         #asm("cli")
	cli
;     712 
;     713         // Timer scheduling
;     714         waktu1= t1;// 3 detik
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     715         waktu2= t2;// 2 mili detik
	LDI  R30,LOW(100)
	MOV  R7,R30
;     716         waktu3= t3;// 3 mili detik
	LDI  R30,LOW(150)
	MOV  R6,R30
;     717         waktu4= t4;// 1 detik
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     718         waktu5= t5;// 400 mili detik
	LDI  R30,LOW(17500)
	LDI  R31,HIGH(17500)
	MOVW R10,R30
;     719 
;     720         // Inisialisasi data
;     721         port_output= 0;
	CBI  0x12,7
;     722         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,4
;     723         penanda_ukur_logika1= 0;
	BLD  R2,5
;     724         penanda_sedang_ukur_logika1= 0;
	BLD  R2,6
;     725         penanda_ukur_1periode= 0;
	BLD  R2,7
;     726         penanda_sedang_ukur_1periode= 0;
	BLD  R3,0
;     727         n= 0;
	LDI  R30,0
	STS  _n,R30
	STS  _n+1,R30
;     728         jml_pulsa= 0;
	LDI  R30,0
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R30
;     729         rpm= 0;
	LDI  R30,0
	STS  _rpm,R30
	STS  _rpm+1,R30
;     730         i= 0;
	LDI  R30,LOW(0)
	STS  _i,R30
;     731 
;     732         // Inisialisasi pin control
;     733         tanda_pin_up= 0;
	BLD  R2,0
;     734         keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     735         tanda_pin_down= 0;
	BLD  R2,1
;     736         keadaan_pin_down= tdk_diaktifkan;
	MOV  R12,R30
;     737 
;     738         // Inisialisasi pin timer
;     739         tanda_pin_timer_up= 0;
	BLD  R2,2
;     740         keadaan_pin_timer_up= tdk_diaktifkan;
	STS  _keadaan_pin_timer_up,R30
;     741         tanda_pin_timer_down= 0;
	BLD  R2,3
;     742         keadaan_pin_timer_down= tdk_diaktifkan;
	STS  _keadaan_pin_timer_down,R30
;     743 
;     744         // Global enable interrupts
;     745         #asm("sei")
	sei
;     746 
;     747         // Main eternal loop
;     748         for(;;)
_0x73:
;     749         {
;     750                 if(waktu1 == 0)
	MOV  R0,R4
	OR   R0,R5
	BRNE _0x75
;     751                 {
;     752                         waktu1= t1;
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     753                         penanda_ukur_logika1= 1;
	SET
	BLD  R2,5
;     754                 }
;     755 
;     756                 // Eksekusi kalibrasi durasi logika 1
;     757                 if(penanda_ukur_logika1)
_0x75:
	SBRS R2,5
	RJMP _0x76
;     758                 {
;     759                         if(penanda_sedang_ukur_logika1)
	SBRS R2,6
	RJMP _0x77
;     760                         {
;     761                                 if(!pin_data)
	SBIC 0x10,2
	RJMP _0x78
;     762                                 {
;     763                                         durasi_logika1= (unsigned long int)n*20;
	LDS  R30,_n
	LDS  R31,_n+1
	CLR  R22
	CLR  R23
	__GETD2N 0x14
	RCALL __MULD12U
	STS  _durasi_logika1,R30
	STS  _durasi_logika1+1,R31
;     764                                         aktifkan_pengukur_1periode();
	RCALL _aktifkan_pengukur_1periode
;     765                                         penanda_ukur_logika1= 0;
	CLT
	BLD  R2,5
;     766                                         penanda_sedang_ukur_logika1= 0;
	BLD  R2,6
;     767                                         penanda_ukur_1periode= 1;
	SET
	BLD  R2,7
;     768                                 }
;     769                         }
_0x78:
;     770                 }
_0x77:
;     771 
;     772                 // Eksekusi kalibrasi durasi 1 periode dan logika 0
;     773                 if(penanda_ukur_1periode)
_0x76:
	SBRS R2,7
	RJMP _0x79
;     774                 {
;     775                         penanda_sedang_ukur_1periode= 1;
	SET
	BLD  R3,0
;     776                 }
;     777 
;     778                 // Scanning tombol
;     779                 if(waktu2 == 0)
_0x79:
	TST  R7
	BRNE _0x7A
;     780                 {
;     781                         waktu2= t2;
	LDI  R30,LOW(100)
	MOV  R7,R30
;     782                         cek_pin_up();
	RCALL _cek_pin_up
;     783                         cek_pin_down();
	RCALL _cek_pin_down
;     784                         cek_pin_timer_up();
	RCALL _cek_pin_timer_up
;     785                         cek_pin_timer_down();
	RCALL _cek_pin_timer_down
;     786                 }
;     787 
;     788                 // Penentuan keadaan pin_up, pin_down, pin_timer_up dan pin_timer_down
;     789                 if(waktu3 == 0)
_0x7A:
	TST  R6
	BRNE _0x7B
;     790                 {
;     791                         waktu3= t3;
	LDI  R30,LOW(150)
	MOV  R6,R30
;     792                         if(tanda_pin_up)
	SBRS R2,0
	RJMP _0x7C
;     793                         {
;     794                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     795                                 increment_shift();
	RCALL _increment_shift
;     796                         }
;     797                         if(tanda_pin_down)
_0x7C:
	SBRS R2,1
	RJMP _0x7D
;     798                         {
;     799                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     800                                 decrement_shift();
	RCALL _decrement_shift
;     801                         }
;     802 
;     803                         if(tanda_pin_timer_up)
_0x7D:
	SBRS R2,2
	RJMP _0x7E
;     804                         {
;     805                                 tanda_pin_timer_up= 0;
	CLT
	BLD  R2,2
;     806                                 if(i < (jml_timer-1))
	LDS  R26,_i
	CPI  R26,LOW(0x9)
	BRGE _0x7F
;     807                                         i++;
	LDS  R30,_i
	SUBI R30,-LOW(1)
	STS  _i,R30
;     808                         }
_0x7F:
;     809                         if(tanda_pin_timer_down)
_0x7E:
	SBRS R2,3
	RJMP _0x80
;     810                         {
;     811                                 tanda_pin_timer_down= 0;
	CLT
	BLD  R2,3
;     812                                 if(i > 0)
	LDS  R26,_i
	CPI  R26,LOW(0x1)
	BRLT _0x81
;     813                                         i--;
	LDS  R30,_i
	SUBI R30,LOW(1)
	STS  _i,R30
;     814                         }
_0x81:
;     815                 }
_0x80:
;     816 
;     817                 //Penentuan timing
;     818                 time_delay= data_acuan_us[i];
_0x7B:
	LDS  R30,_i
	LDI  R26,LOW(_data_acuan_us*2)
	LDI  R27,HIGH(_data_acuan_us*2)
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	RCALL __GETW1PF
	STS  _time_delay,R30
	STS  _time_delay+1,R31
;     819 
;     820                 // Hitung timing
;     821                 hitung_timing();
	RCALL _hitung_timing
;     822 
;     823                 // Hitung RPM
;     824                 if(waktu4 == 0)
	MOV  R0,R8
	OR   R0,R9
	BRNE _0x82
;     825                 {
;     826                         waktu4= t4;
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     827                         hitung_rpm();
	RCALL _hitung_rpm
;     828                 }
;     829 
;     830                 // Tampil LCD
;     831                 if(waktu5 == 0)
_0x82:
	MOV  R0,R10
	OR   R0,R11
	BRNE _0x83
;     832                 {
;     833                         waktu5= t5;
	LDI  R30,LOW(17500)
	LDI  R31,HIGH(17500)
	MOVW R10,R30
;     834                         tampil_lcd();
	RCALL _tampil_lcd
;     835                 }
;     836         };
_0x83:
	RJMP _0x73
;     837 }
_0x84:
	RJMP _0x84

_abs:
    ld   r30,y+
    ld   r31,y+
    sbiw r30,0
    brpl __abs0
    com  r30
    com  r31
    adiw r30,1
__abs0:
    ret
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
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
	RCALL __SAVELOCR2
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	RCALL __GETW1P
	SBIW R30,0
	BREQ _0x92
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	RCALL __GETW1P
	MOVW R16,R30
	SBIW R30,0
	BREQ _0x94
	__CPWRN 16,17,2
	BRLO _0x95
	MOVW R30,R16
	SBIW R30,1
	MOVW R16,R30
	ST   X+,R30
	ST   X,R31
_0x94:
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	LD   R30,X+
	LD   R31,X+
	ADIW R30,1
	ST   -X,R31
	ST   -X,R30
	SBIW R30,1
	LDD  R26,Y+6
	STD  Z+0,R26
_0x95:
	RJMP _0x96
_0x92:
	LDD  R30,Y+6
	ST   -Y,R30
	RCALL _putchar
_0x96:
	RCALL __LOADLOCR2
	ADIW R28,7
	RET
__print_G3:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R17,0
_0x97:
	LDD  R30,Y+18
	LDD  R31,Y+18+1
	ADIW R30,1
	STD  Y+18,R30
	STD  Y+18+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R18,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x99
	MOV  R30,R17
	CPI  R30,0
	BRNE _0x9D
	CPI  R18,37
	BRNE _0x9E
	LDI  R17,LOW(1)
	RJMP _0x9F
_0x9E:
	ST   -Y,R18
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
_0x9F:
	RJMP _0x9C
_0x9D:
	CPI  R30,LOW(0x1)
	BRNE _0xA0
	CPI  R18,37
	BRNE _0xA1
	ST   -Y,R18
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	RJMP _0x15B
_0xA1:
	LDI  R17,LOW(2)
	LDI  R20,LOW(0)
	LDI  R16,LOW(0)
	CPI  R18,45
	BRNE _0xA2
	LDI  R16,LOW(1)
	RJMP _0x9C
_0xA2:
	CPI  R18,43
	BRNE _0xA3
	LDI  R20,LOW(43)
	RJMP _0x9C
_0xA3:
	CPI  R18,32
	BRNE _0xA4
	LDI  R20,LOW(32)
	RJMP _0x9C
_0xA4:
	RJMP _0xA5
_0xA0:
	CPI  R30,LOW(0x2)
	BRNE _0xA6
_0xA5:
	LDI  R21,LOW(0)
	LDI  R17,LOW(3)
	CPI  R18,48
	BRNE _0xA7
	ORI  R16,LOW(128)
	RJMP _0x9C
_0xA7:
	RJMP _0xA8
_0xA6:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x9C
_0xA8:
	CPI  R18,48
	BRLO _0xAB
	CPI  R18,58
	BRLO _0xAC
_0xAB:
	RJMP _0xAA
_0xAC:
	MOV  R26,R21
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R21,R30
	RJMP _0x9C
_0xAA:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0xB0
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	LD   R30,X
	ST   -Y,R30
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	RJMP _0xB1
_0xB0:
	CPI  R30,LOW(0x73)
	BRNE _0xB3
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlen
	MOV  R17,R30
	RJMP _0xB4
_0xB3:
	CPI  R30,LOW(0x70)
	BRNE _0xB6
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlenf
	MOV  R17,R30
	ORI  R16,LOW(8)
_0xB4:
	ORI  R16,LOW(2)
	ANDI R16,LOW(127)
	LDI  R19,LOW(0)
	RJMP _0xB7
_0xB6:
	CPI  R30,LOW(0x64)
	BREQ _0xBA
	CPI  R30,LOW(0x69)
	BRNE _0xBB
_0xBA:
	ORI  R16,LOW(4)
	RJMP _0xBC
_0xBB:
	CPI  R30,LOW(0x75)
	BRNE _0xBD
_0xBC:
	LDI  R30,LOW(_tbl10_G3*2)
	LDI  R31,HIGH(_tbl10_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(5)
	RJMP _0xBE
_0xBD:
	CPI  R30,LOW(0x58)
	BRNE _0xC0
	ORI  R16,LOW(8)
	RJMP _0xC1
_0xC0:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0xF2
_0xC1:
	LDI  R30,LOW(_tbl16_G3*2)
	LDI  R31,HIGH(_tbl16_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(4)
_0xBE:
	SBRS R16,2
	RJMP _0xC3
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0xC4
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R20,LOW(45)
_0xC4:
	CPI  R20,0
	BREQ _0xC5
	SUBI R17,-LOW(1)
	RJMP _0xC6
_0xC5:
	ANDI R16,LOW(251)
_0xC6:
	RJMP _0xC7
_0xC3:
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0xC7:
_0xB7:
	SBRC R16,0
	RJMP _0xC8
_0xC9:
	CP   R17,R21
	BRSH _0xCB
	SBRS R16,7
	RJMP _0xCC
	SBRS R16,2
	RJMP _0xCD
	ANDI R16,LOW(251)
	MOV  R18,R20
	SUBI R17,LOW(1)
	RJMP _0xCE
_0xCD:
	LDI  R18,LOW(48)
_0xCE:
	RJMP _0xCF
_0xCC:
	LDI  R18,LOW(32)
_0xCF:
	ST   -Y,R18
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	SUBI R21,LOW(1)
	RJMP _0xC9
_0xCB:
_0xC8:
	MOV  R19,R17
	SBRS R16,1
	RJMP _0xD0
_0xD1:
	CPI  R19,0
	BREQ _0xD3
	SBRS R16,3
	RJMP _0xD4
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x15C
_0xD4:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x15C:
	ST   -Y,R30
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	CPI  R21,0
	BREQ _0xD6
	SUBI R21,LOW(1)
_0xD6:
	SUBI R19,LOW(1)
	RJMP _0xD1
_0xD3:
	RJMP _0xD7
_0xD0:
_0xD9:
	LDI  R18,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,2
	RCALL __GETW1PF
	STD  Y+8,R30
	STD  Y+8+1,R31
_0xDB:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0xDD
	SUBI R18,-LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	SUB  R30,R26
	SBC  R31,R27
	STD  Y+10,R30
	STD  Y+10+1,R31
	RJMP _0xDB
_0xDD:
	CPI  R18,58
	BRLO _0xDE
	SBRS R16,3
	RJMP _0xDF
	SUBI R18,-LOW(7)
	RJMP _0xE0
_0xDF:
	SUBI R18,-LOW(39)
_0xE0:
_0xDE:
	SBRC R16,4
	RJMP _0xE2
	CPI  R18,49
	BRSH _0xE4
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0xE3
_0xE4:
	RJMP _0x15D
_0xE3:
	CP   R21,R19
	BRLO _0xE8
	SBRS R16,0
	RJMP _0xE9
_0xE8:
	RJMP _0xE7
_0xE9:
	LDI  R18,LOW(32)
	SBRS R16,7
	RJMP _0xEA
	LDI  R18,LOW(48)
_0x15D:
	ORI  R16,LOW(16)
	SBRS R16,2
	RJMP _0xEB
	ANDI R16,LOW(251)
	ST   -Y,R20
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	CPI  R21,0
	BREQ _0xEC
	SUBI R21,LOW(1)
_0xEC:
_0xEB:
_0xEA:
_0xE2:
	ST   -Y,R18
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	CPI  R21,0
	BREQ _0xED
	SUBI R21,LOW(1)
_0xED:
_0xE7:
	SUBI R19,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0xDA
	RJMP _0xD9
_0xDA:
_0xD7:
	SBRS R16,0
	RJMP _0xEE
_0xEF:
	CPI  R21,0
	BREQ _0xF1
	SUBI R21,LOW(1)
	LDI  R30,LOW(32)
	ST   -Y,R30
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,15
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G3
	RJMP _0xEF
_0xF1:
_0xEE:
_0xF2:
_0xB1:
_0x15B:
	LDI  R17,LOW(0)
_0x9C:
	RJMP _0x97
_0x99:
	RCALL __LOADLOCR6
	ADIW R28,20
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	RCALL __SAVELOCR2
	MOVW R26,R28
	RCALL __ADDW2R15
	MOVW R16,R26
	MOVW R26,R28
	ADIW R26,6
	RCALL __ADDW2R15
	RCALL __GETW1P
	STD  Y+2,R30
	STD  Y+2+1,R31
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
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G3
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	RCALL __LOADLOCR2
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
__lcd_x:
	.BYTE 0x1
__lcd_y:
	.BYTE 0x1
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
	RCALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G4:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G4
    ld    r26,y
    swap  r26
	RCALL __lcd_write_nibble_G4
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G4:
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G4
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G4
    andi  r30,0xf0
	RET
_lcd_read_byte0_G4:
	RCALL __lcd_delay_G4
	RCALL __lcd_read_nibble_G4
    mov   r26,r30
	RCALL __lcd_read_nibble_G4
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	RCALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G4)
	SBCI R31,HIGH(-__base_y_G4)
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
	LDI  R30,LOW(12)
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
	BRSH _0x13F
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
_0x13F:
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
	ST   -Y,R17
_0x140:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x142
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x140
_0x142:
	LDD  R17,Y+0
	RJMP _0x14D
_lcd_putsf:
	ST   -Y,R17
_0x143:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x145
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x143
_0x145:
	LDD  R17,Y+0
_0x14D:
	ADIW R28,3
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
	RCALL __lcd_write_nibble_G4
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
	RCALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G4
	RCALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G4
	RCALL __long_delay_G4
	LDI  R30,LOW(48)
	ST   -Y,R30
	RCALL __lcd_init_write_G4
	RCALL __long_delay_G4
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL __lcd_init_write_G4
	RCALL __long_delay_G4
	LDI  R30,LOW(40)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G4
	LDI  R30,LOW(4)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G4
	LDI  R30,LOW(133)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL __long_delay_G4
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RCALL _lcd_read_byte0_G4
	CPI  R30,LOW(0x5)
	BREQ _0x146
	LDI  R30,LOW(0)
	RJMP _0x14C
_0x146:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x14C:
	ADIW R28,1
	RET
_strlen:
    ld   r26,y+
    ld   r27,y+
    clr  r30
    clr  r31
strlen0:
    ld   r22,x+
    tst  r22
    breq strlen1
    adiw r30,1
    rjmp strlen0
strlen1:
    ret
_strlenf:
    clr  r26
    clr  r27
    ld   r30,y+
    ld   r31,y+
strlenf0:
    lpm  r0,z+
    tst  r0
    breq strlenf1
    adiw r26,1
    rjmp strlenf0
strlenf1:
    movw r30,r26
    ret

	.DSEG
_p_S74:
	.BYTE 0x2

	.CSEG

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

__ADDD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	ADC  R23,R25
	RET

__SUBD21:
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SBC  R25,R23
	RET

__ANEGW1:
	NEG  R31
	NEG  R30
	SBCI R31,0
	RET

__ANEGD1:
	COM  R31
	COM  R22
	COM  R23
	NEG  R30
	SBCI R31,-1
	SBCI R22,-1
	SBCI R23,-1
	RET

__LSRW4:
	LSR  R31
	ROR  R30
__LSRW3:
	LSR  R31
	ROR  R30
__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
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

__MULW12U:
	MUL  R31,R26
	MOV  R31,R0
	MUL  R30,R27
	ADD  R31,R0
	MUL  R30,R26
	MOV  R30,R0
	ADD  R31,R1
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

__MULB1W2U:
	MOV  R22,R30
	MUL  R22,R26
	MOVW R30,R0
	MUL  R22,R27
	ADD  R31,R0
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

__PUTPARD2:
	ST   -Y,R25
	ST   -Y,R24
	ST   -Y,R27
	ST   -Y,R26
	RET

__ROUND_REPACK:
	TST  R21
	BRPL __REPACK
	CPI  R21,0x80
	BRNE __ROUND_REPACK0
	SBRS R30,0
	RJMP __REPACK
__ROUND_REPACK0:
	ADIW R30,1
	ADC  R22,R25
	ADC  R23,R25
	BRVS __REPACK1

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

__CFD1U:
	SET
	RJMP __CFD1U0
__CFD1:
	CLT
__CFD1U0:
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
	BRTC __CFD19
	CPI  R21,9
__CFD19:
	BRLO __CFD17
	SER  R30
	SER  R31
	SER  R22
	LDI  R23,0x7F
	BLD  R23,7
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

__ZERORES:
	CLR  R30
	CLR  R31
	CLR  R22
	CLR  R23
	POP  R21
	RET

__MINRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	SER  R23
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
	PUSH R0
	PUSH R17
	PUSH R18
	PUSH R19
	PUSH R20
	CLR  R17
	CLR  R18
	CLR  R25
	MUL  R22,R24
	MOVW R20,R0
	MUL  R24,R31
	MOV  R19,R0
	ADD  R20,R1
	ADC  R21,R25
	MUL  R22,R27
	ADD  R19,R0
	ADC  R20,R1
	ADC  R21,R25
	MUL  R24,R30
	RCALL __MULF126
	MUL  R27,R31
	RCALL __MULF126
	MUL  R22,R26
	RCALL __MULF126
	MUL  R27,R30
	RCALL __MULF127
	MUL  R26,R31
	RCALL __MULF127
	MUL  R26,R30
	ADD  R17,R1
	ADC  R18,R25
	ADC  R19,R25
	ADC  R20,R25
	ADC  R21,R25
	MOV  R30,R19
	MOV  R31,R20
	MOV  R22,R21
	MOV  R21,R18
	POP  R20
	POP  R19
	POP  R18
	POP  R17
	POP  R0
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
	RCALL __ROUND_REPACK
	POP  R21
	RET

__MULF127:
	ADD  R17,R0
	ADC  R18,R1
	ADC  R19,R25
	RJMP __MULF128
__MULF126:
	ADD  R18,R0
	ADC  R19,R1
__MULF128:
	ADC  R20,R25
	ADC  R21,R25
	RET

__CPW02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
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
