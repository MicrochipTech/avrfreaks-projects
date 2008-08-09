
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
;       9 Chip type           : ATmega8535
;      10 Program type        : Application
;      11 Clock frequency     : 16.000000 MHz
;      12 Memory model        : Small
;      13 External SRAM size  : 0
;      14 Data Stack size     : 128
;      15 *****************************************************/
;      16 
;      17 #include <mega8535.h>
;      18 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;      19 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;      20 	.EQU __se_bit=0x40
	.EQU __se_bit=0x40
;      21 	.EQU __sm_mask=0xB0
	.EQU __sm_mask=0xB0
;      22 	.EQU __sm_powerdown=0x20
	.EQU __sm_powerdown=0x20
;      23 	.EQU __sm_powersave=0x30
	.EQU __sm_powersave=0x30
;      24 	.EQU __sm_standby=0xA0
	.EQU __sm_standby=0xA0
;      25 	.EQU __sm_ext_standby=0xB0
	.EQU __sm_ext_standby=0xB0
;      26 	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_adc_noise_red=0x10
;      27 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;      28 	#endif
	#endif
;      29 #include <math.h>
;      30 #include <stdio.h>
;      31 
;      32 // Alphanumeric LCD Module functions
;      33 #asm
;      34    .equ __lcd_port=0x1B ;PORTA
   .equ __lcd_port=0x1B ;PORTA
;      35 #endasm
;      36 #include <lcd.h>
;      37 
;      38 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      39 
;      40 #define pin_data                PIND.2
;      41 #define port_output             PORTD.7
;      42 #define pin_up                  PINC.0
;      43 #define pin_down                PINC.1
;      44 #define pin_timer_up            PINC.2
;      45 #define pin_timer_down          PINC.3
;      46 
;      47 #define tdk_diaktifkan          1
;      48 #define mgkn_diaktifkan         2
;      49 #define diaktifkan              3
;      50 #define mgkn_tdk_diaktifkan     4
;      51 
;      52 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      53 
;      54 #define t1                      5000000/20
;      55 #define t2                      1000/20
;      56 #define t3                      4000/20
;      57 #define t4                      1000000/20
;      58 #define t5                      750000/20
;      59 
;      60 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      61 
;      62 #define jml_timer               7
;      63 
;      64 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      65 
;      66 unsigned int waktu1;
;      67 unsigned char waktu2;
;      68 unsigned char waktu3;
;      69 unsigned int waktu4;
;      70 unsigned int waktu5;
;      71 
;      72 char buff[33];
_buff:
	.BYTE 0x21
;      73 
;      74 char keadaan_pin_up;
;      75 bit tanda_pin_up;
;      76 
;      77 char keadaan_pin_down;
;      78 bit tanda_pin_down;
;      79 
;      80 char keadaan_pin_timer_up;
_keadaan_pin_timer_up:
	.BYTE 0x1
;      81 bit tanda_pin_timer_up;
;      82 
;      83 char keadaan_pin_timer_down;
_keadaan_pin_timer_down:
	.BYTE 0x1
;      84 bit tanda_pin_timer_down;
;      85 
;      86 bit penanda_ukur_logika1;
;      87 bit penanda_ukur_logika0;
;      88 bit penanda_selesai_ukur_logika;
;      89 unsigned int durasi_logika1;
_durasi_logika1:
	.BYTE 0x2
;      90 unsigned long int durasi_logika0;
_durasi_logika0:
	.BYTE 0x4
;      91 unsigned long int durasi_1periode;
_durasi_1periode:
	.BYTE 0x4
;      92 
;      93 unsigned int rpm;
_rpm:
	.BYTE 0x2
;      94 unsigned long int timing;
_timing:
	.BYTE 0x4
;      95 unsigned int time_delay;
_time_delay:
	.BYTE 0x2
;      96 signed int time_delay_step;// -60 s.d. 60
_time_delay_step:
	.BYTE 0x2
;      97 unsigned int nilai_mutlak;
_nilai_mutlak:
	.BYTE 0x2
;      98 
;      99 signed char i;
_i:
	.BYTE 0x1
;     100 
;     101 flash unsigned int data_acuan_lag_us[jml_timer]=

	.CSEG
;     102 {
;     103         0,
;     104         15,
;     105         20,
;     106         25,
;     107         75,
;     108         125,
;     109         250
;     110 };
;     111 
;     112 flash unsigned int data_acuan_lead_us[jml_timer]=
;     113 {
;     114         0,
;     115         250,
;     116         125,
;     117         75,
;     118         25,
;     119         20,
;     120         15
;     121 };
;     122 
;     123 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     124 
;     125 void cek_pin_up(void)
;     126 {
_cek_pin_up:
;     127         switch(keadaan_pin_up)
	MOV  R30,R13
;     128         {
;     129                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x6
;     130                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x7
;     131                                 keadaan_pin_up= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x13B
;     132                         else
_0x7:
;     133                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x13B:
	MOV  R13,R30
;     134                         break;
	RJMP _0x5
;     135                 case mgkn_diaktifkan:
_0x6:
	CPI  R30,LOW(0x2)
	BRNE _0x9
;     136                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xA
;     137                         {
;     138                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     139                                 tanda_pin_up= 1;
	SET
	BLD  R2,0
;     140                         }
;     141                         else
	RJMP _0xB
_0xA:
;     142                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     143                         break;
_0xB:
	RJMP _0x5
;     144                 case diaktifkan:
_0x9:
	CPI  R30,LOW(0x3)
	BRNE _0xC
;     145                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xD
;     146                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x13C
;     147                         else
_0xD:
;     148                                 keadaan_pin_up= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x13C:
	MOV  R13,R30
;     149                         break;
	RJMP _0x5
;     150                 case mgkn_tdk_diaktifkan:
_0xC:
	CPI  R30,LOW(0x4)
	BRNE _0x5
;     151                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x10
;     152                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     153                         else
	RJMP _0x11
_0x10:
;     154                         {
;     155                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     156                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     157                         }
_0x11:
;     158                         break;
;     159         }
_0x5:
;     160 }
	RET
;     161 
;     162 void cek_pin_down(void)
;     163 {
_cek_pin_down:
;     164         switch(keadaan_pin_down)
	MOV  R30,R12
;     165         {
;     166                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x15
;     167                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x16
;     168                                 keadaan_pin_down= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x13D
;     169                         else
_0x16:
;     170                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x13D:
	MOV  R12,R30
;     171                         break;
	RJMP _0x14
;     172                 case mgkn_diaktifkan:
_0x15:
	CPI  R30,LOW(0x2)
	BRNE _0x18
;     173                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x19
;     174                         {
;     175                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     176                                 tanda_pin_down= 1;
	SET
	BLD  R2,1
;     177                         }
;     178                         else
	RJMP _0x1A
_0x19:
;     179                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     180                         break;
_0x1A:
	RJMP _0x14
;     181                 case diaktifkan:
_0x18:
	CPI  R30,LOW(0x3)
	BRNE _0x1B
;     182                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1C
;     183                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x13E
;     184                         else
_0x1C:
;     185                                 keadaan_pin_down= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x13E:
	MOV  R12,R30
;     186                         break;
	RJMP _0x14
;     187                 case mgkn_tdk_diaktifkan:
_0x1B:
	CPI  R30,LOW(0x4)
	BRNE _0x14
;     188                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1F
;     189                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     190                         else
	RJMP _0x20
_0x1F:
;     191                         {
;     192                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     193                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     194                         }
_0x20:
;     195                         break;
;     196         }
_0x14:
;     197 }
	RET
;     198 
;     199 void cek_pin_timer_up(void)
;     200 {
_cek_pin_timer_up:
;     201         switch(keadaan_pin_timer_up)
	LDS  R30,_keadaan_pin_timer_up
;     202         {
;     203                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x24
;     204                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x25
;     205                                 keadaan_pin_timer_up= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x13F
;     206                         else
_0x25:
;     207                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x13F:
	STS  _keadaan_pin_timer_up,R30
;     208                         break;
	RJMP _0x23
;     209                 case mgkn_diaktifkan:
_0x24:
	CPI  R30,LOW(0x2)
	BRNE _0x27
;     210                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x28
;     211                         {
;     212                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_up,R30
;     213                                 tanda_pin_timer_up= 1;
	SET
	BLD  R2,2
;     214                         }
;     215                         else
	RJMP _0x29
_0x28:
;     216                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_up,R30
;     217                         break;
_0x29:
	RJMP _0x23
;     218                 case diaktifkan:
_0x27:
	CPI  R30,LOW(0x3)
	BRNE _0x2A
;     219                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x2B
;     220                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x140
;     221                         else
_0x2B:
;     222                                 keadaan_pin_timer_up= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x140:
	STS  _keadaan_pin_timer_up,R30
;     223                         break;
	RJMP _0x23
;     224                 case mgkn_tdk_diaktifkan:
_0x2A:
	CPI  R30,LOW(0x4)
	BRNE _0x23
;     225                         if(pin_timer_up == 0)
	SBIC 0x13,2
	RJMP _0x2E
;     226                                 keadaan_pin_timer_up= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_up,R30
;     227                         else
	RJMP _0x2F
_0x2E:
;     228                         {
;     229                                 keadaan_pin_timer_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_up,R30
;     230                                 tanda_pin_timer_up= 0;
	CLT
	BLD  R2,2
;     231                         }
_0x2F:
;     232                         break;
;     233         }
_0x23:
;     234 }
	RET
;     235 
;     236 void cek_pin_timer_down(void)
;     237 {
_cek_pin_timer_down:
;     238         switch(keadaan_pin_timer_down)
	LDS  R30,_keadaan_pin_timer_down
;     239         {
;     240                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x33
;     241                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x34
;     242                                 keadaan_pin_timer_down= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x141
;     243                         else
_0x34:
;     244                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x141:
	STS  _keadaan_pin_timer_down,R30
;     245                         break;
	RJMP _0x32
;     246                 case mgkn_diaktifkan:
_0x33:
	CPI  R30,LOW(0x2)
	BRNE _0x36
;     247                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x37
;     248                         {
;     249                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_down,R30
;     250                                 tanda_pin_timer_down= 1;
	SET
	BLD  R2,3
;     251                         }
;     252                         else
	RJMP _0x38
_0x37:
;     253                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_down,R30
;     254                         break;
_0x38:
	RJMP _0x32
;     255                 case diaktifkan:
_0x36:
	CPI  R30,LOW(0x3)
	BRNE _0x39
;     256                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x3A
;     257                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x142
;     258                         else
_0x3A:
;     259                                 keadaan_pin_timer_down= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x142:
	STS  _keadaan_pin_timer_down,R30
;     260                         break;
	RJMP _0x32
;     261                 case mgkn_tdk_diaktifkan:
_0x39:
	CPI  R30,LOW(0x4)
	BRNE _0x32
;     262                         if(pin_timer_down == 0)
	SBIC 0x13,3
	RJMP _0x3D
;     263                                 keadaan_pin_timer_down= diaktifkan;
	LDI  R30,LOW(3)
	STS  _keadaan_pin_timer_down,R30
;     264                         else
	RJMP _0x3E
_0x3D:
;     265                         {
;     266                                 keadaan_pin_timer_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	STS  _keadaan_pin_timer_down,R30
;     267                                 tanda_pin_timer_down= 0;
	CLT
	BLD  R2,3
;     268                         }
_0x3E:
;     269                         break;
;     270         }
_0x32:
;     271 }
	RET
;     272 
;     273 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     274 
;     275 // Timer untuk mengukur logika
;     276 void aktifkan_pengukur_logika(void)
;     277 {
_aktifkan_pengukur_logika:
;     278         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     279         TCCR1B= 0x03;
	LDI  R30,LOW(3)
	OUT  0x2E,R30
;     280         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     281         TCNT1L=0x00;
	OUT  0x2C,R30
;     282         OCR1AH=0x00;
	OUT  0x2B,R30
;     283         OCR1AL=0x00;
	OUT  0x2A,R30
;     284         OCR1BH=0x00;
	OUT  0x29,R30
;     285         OCR1BL=0x00;
	OUT  0x28,R30
;     286 
;     287         TIMSK= 0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     288 }
	RET
;     289 
;     290 // Timer shifter
;     291 void aktifkan_timer1_cmpA(unsigned long int nilai_compare)
;     292 {
_aktifkan_timer1_cmpA:
;     293         unsigned int nilai_ocr= 0x0000;
;     294 
;     295         port_output= 0;
	RCALL __SAVELOCR2
;	nilai_compare -> Y+2
;	nilai_ocr -> R16,R17
	LDI  R16,0
	LDI  R17,0
	CBI  0x12,7
;     296 
;     297         if(time_delay_step < 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x3F
;     298                 nilai_ocr= ((nilai_compare-8)*2);// Penyesuaian sinyal lead
	__GETD1S 2
	__SUBD1N 8
	RJMP _0x143
;     299         else
_0x3F:
;     300                 nilai_ocr= ((nilai_compare-12)*2);// Penyesuaian sinyal lag
	__GETD1S 2
	__SUBD1N 12
_0x143:
	RCALL __LSLD1
	MOVW R16,R30
;     301 
;     302         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     303         TCCR1B= 0x02;
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     304         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     305         TCNT1L=0x00;
	OUT  0x2C,R30
;     306         OCR1A= nilai_ocr;
	__OUTWR 16,17,42
;     307         OCR1BH=0x00;
	OUT  0x29,R30
;     308         OCR1BL=0x00;
	OUT  0x28,R30
;     309 
;     310         TIMSK=0x12;
	LDI  R30,LOW(18)
	OUT  0x39,R30
;     311 }
	RJMP _0x13A
;     312 
;     313 // Timer logika
;     314 void aktifkan_timer1_cmpB(unsigned long int nilai_compare)
;     315 {
_aktifkan_timer1_cmpB:
;     316         unsigned int nilai_ocr= 0x0000;
;     317 
;     318         nilai_ocr= (nilai_compare*2);
	RCALL __SAVELOCR2
;	nilai_compare -> Y+2
;	nilai_ocr -> R16,R17
	LDI  R16,0
	LDI  R17,0
	__GETD1S 2
	RCALL __LSLD1
	MOVW R16,R30
;     319 
;     320         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     321         TCCR1B= 0x02;
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     322         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     323         TCNT1L=0x00;
	OUT  0x2C,R30
;     324         OCR1AH=0x00;
	OUT  0x2B,R30
;     325         OCR1AL=0x00;
	OUT  0x2A,R30
;     326         OCR1B= nilai_ocr;
	__OUTWR 16,17,40
;     327 
;     328         TIMSK=0x0A;
	LDI  R30,LOW(10)
	OUT  0x39,R30
;     329 }
_0x13A:
	RCALL __LOADLOCR2
	ADIW R28,6
	RET
;     330 
;     331 // Matikan timer
;     332 void matikan_timer1(void)
;     333 {
_matikan_timer1:
;     334         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     335 
;     336         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     337         TCCR1B=0x00;
	OUT  0x2E,R30
;     338         TCNT1H=0x00;
	OUT  0x2D,R30
;     339         TCNT1L=0x00;
	OUT  0x2C,R30
;     340         OCR1AH=0x00;
	OUT  0x2B,R30
;     341         OCR1AL=0x00;
	OUT  0x2A,R30
;     342         OCR1BH=0x00;
	OUT  0x29,R30
;     343         OCR1BL=0x00;
	OUT  0x28,R30
;     344 }
	RET
;     345 
;     346 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     347 
;     348 void hitung_timing(void)
;     349 {
_hitung_timing:
;     350         if(time_delay_step > 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x41
;     351         {
;     352                 timing= (unsigned long int)time_delay*time_delay_step;
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
;     353         }
;     354         else if(time_delay_step < 0)
	RJMP _0x42
_0x41:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x43
;     355         {
;     356                 nilai_mutlak= abs(time_delay_step);
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _abs
	STS  _nilai_mutlak,R30
	STS  _nilai_mutlak+1,R31
;     357                 timing= (unsigned long int)time_delay*nilai_mutlak;
	LDS  R30,_time_delay
	LDS  R31,_time_delay+1
	CLR  R22
	CLR  R23
	MOVW R26,R30
	MOVW R24,R22
	LDS  R30,_nilai_mutlak
	LDS  R31,_nilai_mutlak+1
	CLR  R22
	CLR  R23
	RCALL __MULD12U
	STS  _timing,R30
	STS  _timing+1,R31
	STS  _timing+2,R22
	STS  _timing+3,R23
;     358         }
;     359         else if(time_delay_step == 0)
	RJMP _0x44
_0x43:
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x45
;     360                 timing= 0;
	LDI  R30,0
	STS  _timing,R30
	STS  _timing+1,R30
	STS  _timing+2,R30
	STS  _timing+3,R30
;     361 }
_0x45:
_0x44:
_0x42:
	RET
;     362 
;     363 void increment_shift(void)
;     364 {
_increment_shift:
;     365         if(time_delay_step < 60)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,60
	BRGE _0x46
;     366         {
;     367                 ++time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ADIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     368         }
;     369         hitung_timing();
_0x46:
	RCALL _hitung_timing
;     370 }
	RET
;     371 
;     372 void decrement_shift(void)
;     373 {
_decrement_shift:
;     374         if(time_delay_step > -60)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	LDI  R30,LOW(65476)
	LDI  R31,HIGH(65476)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x47
;     375         {
;     376                 --time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     377         }
;     378         hitung_timing();
_0x47:
	RCALL _hitung_timing
;     379 }
	RET
;     380 
;     381 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     382 
;     383 void hitung_rpm(void)
;     384 {
_hitung_rpm:
;     385         if(durasi_1periode != 0)
	LDS  R30,_durasi_1periode
	LDS  R31,_durasi_1periode+1
	LDS  R22,_durasi_1periode+2
	LDS  R23,_durasi_1periode+3
	RCALL __CPD10
	BREQ _0x48
;     386                 rpm= 60000000/durasi_1periode;
	__GETD2N 0x3938700
	RCALL __DIVD21U
	STS  _rpm,R30
	STS  _rpm+1,R31
;     387         else
	RJMP _0x49
_0x48:
;     388                 rpm= 0;
	LDI  R30,0
	STS  _rpm,R30
	STS  _rpm+1,R30
;     389 }
_0x49:
	RET
;     390 
;     391 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     392 
;     393 void tampil_lcd(void)
;     394 {
_tampil_lcd:
;     395         lcd_clear();
	RCALL _lcd_clear
;     396         lcd_putsf("td  d     RPM");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     397         sprintf(buff, "\n%2i %3i   %5u", time_delay_step, time_delay, rpm);
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
;     398         lcd_puts(buff);
	LDI  R30,LOW(_buff)
	LDI  R31,HIGH(_buff)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     399 }
	RET
;     400 
;     401 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     402 
;     403 // External Interrupt 0 service routine
;     404 interrupt [EXT_INT0] void ext_int0_isr(void)
;     405 {
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
;     406         if(pin_data == 1)
	SBIS 0x10,2
	RJMP _0x4A
;     407         {
;     408                 // Ganti ke interrupt fall
;     409                 MCUCR&= ~(1<<0);
	IN   R30,0x35
	ANDI R30,0xFE
	OUT  0x35,R30
;     410 
;     411                 if(penanda_ukur_logika1)
	SBRS R2,4
	RJMP _0x4B
;     412                 {
;     413                         aktifkan_pengukur_logika();
	RCALL _aktifkan_pengukur_logika
;     414                 }
;     415                 else if(penanda_ukur_logika0)
	RJMP _0x4C
_0x4B:
	SBRS R2,5
	RJMP _0x4D
;     416                 {
;     417                         // Ambil data durasi 1 periode
;     418                         TCCR1B= 0x00;
	LDI  R30,LOW(0)
	OUT  0x2E,R30
;     419                         durasi_1periode= (unsigned long int)TCNT1*4;
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
;     420                         matikan_timer1();
	RCALL _matikan_timer1
;     421                         penanda_ukur_logika0= 0;
	CLT
	BLD  R2,5
;     422                         penanda_selesai_ukur_logika= 1;
	SET
	BLD  R2,6
;     423                 }
;     424                 else
	RJMP _0x4E
_0x4D:
;     425                 {
;     426                         if(time_delay_step == 0)
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x4F
;     427                         {
;     428                                 port_output= 1;
	SBI  0x12,7
;     429                                 aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     430                         }
;     431                         else if(time_delay_step > 0)
	RJMP _0x50
_0x4F:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x51
;     432                         {
;     433                                 port_output= 0;
	CBI  0x12,7
;     434                                 aktifkan_timer1_cmpA(timing);// timing shift for logic 0
	LDS  R30,_timing
	LDS  R31,_timing+1
	LDS  R22,_timing+2
	LDS  R23,_timing+3
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpA
;     435                         }
;     436                 }
_0x51:
_0x50:
_0x4E:
_0x4C:
;     437         }
;     438         else if(pin_data == 0)
	RJMP _0x52
_0x4A:
	SBIC 0x10,2
	RJMP _0x53
;     439         {
;     440                 // Ganti ke interrupt rise
;     441                 MCUCR|= 1<<0;
	IN   R30,0x35
	ORI  R30,1
	OUT  0x35,R30
;     442 
;     443                 if(penanda_ukur_logika1)
	SBRS R2,4
	RJMP _0x54
;     444                 {
;     445                         // Ambil data durasi logika 1
;     446                         durasi_logika1= TCNT1*4;
	IN   R30,0x2C
	IN   R31,0x2C+1
	RCALL __LSLW2
	STS  _durasi_logika1,R30
	STS  _durasi_logika1+1,R31
;     447                         penanda_ukur_logika1= 0;
	CLT
	BLD  R2,4
;     448                         penanda_ukur_logika0= 1;
	SET
	BLD  R2,5
;     449                 }
;     450                 else
	RJMP _0x55
_0x54:
;     451                 {
;     452                         if(time_delay_step < 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x56
;     453                         {
;     454                                 port_output= 0;
	CBI  0x12,7
;     455                                 aktifkan_timer1_cmpA(durasi_logika0-timing);// timing shift for logic 0
	LDS  R26,_timing
	LDS  R27,_timing+1
	LDS  R24,_timing+2
	LDS  R25,_timing+3
	LDS  R30,_durasi_logika0
	LDS  R31,_durasi_logika0+1
	LDS  R22,_durasi_logika0+2
	LDS  R23,_durasi_logika0+3
	RCALL __SUBD12
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpA
;     456                         }
;     457                 }
_0x56:
_0x55:
;     458         }
;     459 }
_0x53:
_0x52:
	RJMP _0x148
;     460 
;     461 // Timer 0 output compare interrupt service routine, base time 20 us
;     462 interrupt [TIM0_COMP] void timer0_comp_isr(void)
;     463 {
_timer0_comp_isr:
	ST   -Y,R0
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     464         TCNT0= 0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     465 
;     466         if(waktu1 > 0)
	CLR  R0
	CP   R0,R4
	CPC  R0,R5
	BRSH _0x57
;     467                 --waktu1;
	MOVW R30,R4
	SBIW R30,1
	MOVW R4,R30
;     468         if(waktu2 > 0)
_0x57:
	LDI  R30,LOW(0)
	CP   R30,R7
	BRSH _0x58
;     469                 --waktu2;
	DEC  R7
;     470         if(waktu3 > 0)
_0x58:
	LDI  R30,LOW(0)
	CP   R30,R6
	BRSH _0x59
;     471                 --waktu3;
	DEC  R6
;     472         if(waktu4 > 0)
_0x59:
	CLR  R0
	CP   R0,R8
	CPC  R0,R9
	BRSH _0x5A
;     473                 --waktu4;
	MOVW R30,R8
	SBIW R30,1
	MOVW R8,R30
;     474         if(waktu5 > 0)
_0x5A:
	CLR  R0
	CP   R0,R10
	CPC  R0,R11
	BRSH _0x5B
;     475                 --waktu5;
	MOVW R30,R10
	SBIW R30,1
	MOVW R10,R30
;     476 }
_0x5B:
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R0,Y+
	RETI
;     477 
;     478 // Timer 1 output compare A interrupt service routine, timer shifter match
;     479 interrupt [TIM1_COMPA] void timer1_compa_isr(void)
;     480 {
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
;     481         port_output= 1;
	SBI  0x12,7
;     482         aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     483 }
_0x148:
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
;     484 
;     485 // Timer 1 output compare B interrupt service routine, timer logika 1 match
;     486 interrupt [TIM1_COMPB] void timer1_compb_isr(void)
;     487 {
_timer1_compb_isr:
;     488         port_output= 0;
	CBI  0x12,7
;     489 }
	RETI
;     490 
;     491 void inisialisasi_durasi_pulsa(void)
;     492 {
_inisialisasi_durasi_pulsa:
;     493         // Global enable interrupts
;     494         #asm("sei")
	sei
;     495 
;     496         penanda_ukur_logika1= 1;
	SET
	BLD  R2,4
;     497         while(!penanda_selesai_ukur_logika);
_0x5C:
	SBRS R2,6
	RJMP _0x5C
;     498         durasi_logika0= durasi_1periode-durasi_logika1;
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
;     499         penanda_selesai_ukur_logika= 0;
	CLT
	BLD  R2,6
;     500 
;     501         // Global disable interrupts
;     502         #asm("cli")
	cli
;     503 }
	RET
;     504 
;     505 void inisialisasi(void)
;     506 {
_inisialisasi:
;     507         /**
;     508         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     509         Port A -- Port LCD
;     510         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     511         **/
;     512         PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     513         DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     514 
;     515         /**
;     516         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     517         Port B -- Dummies
;     518         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     519         **/
;     520         PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;     521         DDRB=0x00;
	OUT  0x17,R30
;     522 
;     523         /**
;     524         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     525         Port C -- Kontrol Shifter
;     526         C.0: Up
;     527         C.1: Down
;     528         C.2: Timer Up
;     529         C.3: Timer Down
;     530         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     531         **/
;     532         PORTC=0x0F;
	LDI  R30,LOW(15)
	OUT  0x15,R30
;     533         DDRC=0x00;
	LDI  R30,LOW(0)
	OUT  0x14,R30
;     534 
;     535         /**
;     536         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     537         Port D
;     538         D.2: INT0
;     539         D.7: Shifted Output Pulse
;     540         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     541         **/
;     542         PORTD=0x04;
	LDI  R30,LOW(4)
	OUT  0x12,R30
;     543         DDRD=0x80;
	LDI  R30,LOW(128)
	OUT  0x11,R30
;     544 
;     545         // Timer/Counter 0 initialization
;     546         // Clock source: System Clock
;     547         // Clock value: 2000.000 kHz
;     548         // Mode: Normal top=FFh
;     549         // OC0 output: Disconnected
;     550         TCCR0=0x02;
	LDI  R30,LOW(2)
	OUT  0x33,R30
;     551         TCNT0=0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     552         OCR0=0x28;
	LDI  R30,LOW(40)
	OUT  0x3C,R30
;     553 
;     554         // Timer/Counter 1 initialization
;     555         // Clock source: System Clock
;     556         // Clock value: Timer 1 Stopped
;     557         // Mode: Normal top=FFFFh
;     558         // OC1A output: Discon.
;     559         // OC1B output: Discon.
;     560         // Noise Canceler: Off
;     561         // Input Capture on Falling Edge
;     562         // Timer 1 Overflow Interrupt: Off
;     563         // Input Capture Interrupt: Off
;     564         // Compare A Match Interrupt: Off
;     565         // Compare B Match Interrupt: Off
;     566         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     567         TCCR1B=0x00;
	OUT  0x2E,R30
;     568         TCNT1H=0x00;
	OUT  0x2D,R30
;     569         TCNT1L=0x00;
	OUT  0x2C,R30
;     570         ICR1H=0x00;
	OUT  0x27,R30
;     571         ICR1L=0x00;
	OUT  0x26,R30
;     572         OCR1AH=0x00;
	OUT  0x2B,R30
;     573         OCR1AL=0x00;
	OUT  0x2A,R30
;     574         OCR1BH=0x00;
	OUT  0x29,R30
;     575         OCR1BL=0x00;
	OUT  0x28,R30
;     576 
;     577         // Timer/Counter 2 initialization
;     578         // Clock source: System Clock
;     579         // Clock value: Timer 2 Stopped
;     580         // Mode: Normal top=FFh
;     581         // OC2 output: Disconnected
;     582         ASSR=0x00;
	OUT  0x22,R30
;     583         TCCR2=0x00;
	OUT  0x25,R30
;     584         TCNT2=0x00;
	OUT  0x24,R30
;     585         OCR2=0x00;
	OUT  0x23,R30
;     586 
;     587         // External Interrupt(s) initialization
;     588         // INT0: On
;     589         // INT0 Mode: Rising Edge
;     590         // INT1: Off
;     591         // INT2: Off
;     592         GICR|=0x40;
	IN   R30,0x3B
	ORI  R30,0x40
	OUT  0x3B,R30
;     593         MCUCR=0x03;
	LDI  R30,LOW(3)
	OUT  0x35,R30
;     594         MCUCSR=0x00;
	LDI  R30,LOW(0)
	OUT  0x34,R30
;     595         GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
;     596 
;     597         // Timer(s)/Counter(s) Interrupt(s) initialization
;     598         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     599 
;     600         // Analog Comparator initialization
;     601         // Analog Comparator: Off
;     602         // Analog Comparator Input Capture by Timer/Counter 1: Off
;     603         ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     604         SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     605 
;     606         // LCD module initialization
;     607         lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
;     608         lcd_clear();
	RCALL _lcd_clear
;     609 
;     610         // Inisialisasi durasi pulsa
;     611         inisialisasi_durasi_pulsa();
	RCALL _inisialisasi_durasi_pulsa
;     612 
;     613         // Timer scheduling
;     614         waktu1= t1;// 3 detik
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     615         waktu2= t2;// 2 mili detik
	LDI  R30,LOW(50)
	MOV  R7,R30
;     616         waktu3= t3;// 3 mili detik
	LDI  R30,LOW(200)
	MOV  R6,R30
;     617         waktu4= t4;// 1 detik
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     618         waktu5= t5;// 400 mili detik
	LDI  R30,LOW(37500)
	LDI  R31,HIGH(37500)
	MOVW R10,R30
;     619 
;     620         // Inisialisasi data
;     621         port_output= 0;
	CBI  0x12,7
;     622         penanda_ukur_logika1= 0;
	CLT
	BLD  R2,4
;     623         penanda_ukur_logika0= 0;
	BLD  R2,5
;     624         penanda_selesai_ukur_logika= 0;
	BLD  R2,6
;     625         rpm= 0;
	LDI  R30,0
	STS  _rpm,R30
	STS  _rpm+1,R30
;     626         i= 0;
	LDI  R30,LOW(0)
	STS  _i,R30
;     627 
;     628         // Inisialisasi pin control
;     629         tanda_pin_up= 0;
	BLD  R2,0
;     630         keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     631         tanda_pin_down= 0;
	BLD  R2,1
;     632         keadaan_pin_down= tdk_diaktifkan;
	MOV  R12,R30
;     633 
;     634         // Inisialisasi pin timer
;     635         tanda_pin_timer_up= 0;
	BLD  R2,2
;     636         keadaan_pin_timer_up= tdk_diaktifkan;
	STS  _keadaan_pin_timer_up,R30
;     637         tanda_pin_timer_down= 0;
	BLD  R2,3
;     638         keadaan_pin_timer_down= tdk_diaktifkan;
	STS  _keadaan_pin_timer_down,R30
;     639 
;     640         // Global enable interrupts
;     641         #asm("sei")
	sei
;     642 }
	RET
;     643 
;     644 void main(void)
;     645 {
_main:
;     646         // Inisialisasi program
;     647         inisialisasi();
	RCALL _inisialisasi
;     648 
;     649         // Main eternal loop
;     650         for(;;)
_0x60:
;     651         {
;     652                 // Selesai eksekusi kalibrasi logika
;     653                 if(penanda_selesai_ukur_logika)
	SBRS R2,6
	RJMP _0x62
;     654                 {
;     655                         durasi_logika0= durasi_1periode-durasi_logika1;
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
;     656                         penanda_selesai_ukur_logika= 0;
	CLT
	BLD  R2,6
;     657                 }
;     658 
;     659                 // Eksekusi kalibrasi logika
;     660                 if(waktu1 == 0)
_0x62:
	MOV  R0,R4
	OR   R0,R5
	BRNE _0x63
;     661                 {
;     662                         waktu1= t1;
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     663                         penanda_ukur_logika1= 1;
	SET
	BLD  R2,4
;     664                 }
;     665 
;     666                 // Scanning tombol
;     667                 if(waktu2 == 0)
_0x63:
	TST  R7
	BRNE _0x64
;     668                 {
;     669                         waktu2= t2;
	LDI  R30,LOW(50)
	MOV  R7,R30
;     670                         cek_pin_up();
	RCALL _cek_pin_up
;     671                         cek_pin_down();
	RCALL _cek_pin_down
;     672                         cek_pin_timer_up();
	RCALL _cek_pin_timer_up
;     673                         cek_pin_timer_down();
	RCALL _cek_pin_timer_down
;     674                 }
;     675 
;     676                 // Penentuan keadaan pin_up, pin_down, pin_timer_up dan pin_timer_down
;     677                 if(waktu3 == 0)
_0x64:
	TST  R6
	BRNE _0x65
;     678                 {
;     679                         waktu3= t3;
	LDI  R30,LOW(200)
	MOV  R6,R30
;     680                         if(tanda_pin_up)
	SBRS R2,0
	RJMP _0x66
;     681                         {
;     682                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     683                                 increment_shift();
	RCALL _increment_shift
;     684                         }
;     685                         if(tanda_pin_down)
_0x66:
	SBRS R2,1
	RJMP _0x67
;     686                         {
;     687                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     688                                 decrement_shift();
	RCALL _decrement_shift
;     689                         }
;     690 
;     691                         if(tanda_pin_timer_up)
_0x67:
	SBRS R2,2
	RJMP _0x68
;     692                         {
;     693                                 tanda_pin_timer_up= 0;
	CLT
	BLD  R2,2
;     694                                 if(i < (jml_timer-1))
	LDS  R26,_i
	CPI  R26,LOW(0x6)
	BRGE _0x69
;     695                                         i++;
	LDS  R30,_i
	SUBI R30,-LOW(1)
	STS  _i,R30
;     696                         }
_0x69:
;     697                         if(tanda_pin_timer_down)
_0x68:
	SBRS R2,3
	RJMP _0x6A
;     698                         {
;     699                                 tanda_pin_timer_down= 0;
	CLT
	BLD  R2,3
;     700                                 if(i > 0)
	LDS  R26,_i
	CPI  R26,LOW(0x1)
	BRLT _0x6B
;     701                                         i--;
	LDS  R30,_i
	SUBI R30,LOW(1)
	STS  _i,R30
;     702                         }
_0x6B:
;     703                 }
_0x6A:
;     704 
;     705                 //Penentuan timing
;     706                 if(time_delay_step < 0)
_0x65:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x6C
;     707                         time_delay= data_acuan_lead_us[i];
	LDS  R30,_i
	LDI  R26,LOW(_data_acuan_lead_us*2)
	LDI  R27,HIGH(_data_acuan_lead_us*2)
	RJMP _0x144
;     708                 else
_0x6C:
;     709                         time_delay= data_acuan_lag_us[i];
	LDS  R30,_i
	LDI  R26,LOW(_data_acuan_lag_us*2)
	LDI  R27,HIGH(_data_acuan_lag_us*2)
_0x144:
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	RCALL __GETW1PF
	STS  _time_delay,R30
	STS  _time_delay+1,R31
;     710 
;     711                 // Hitung timing
;     712                 hitung_timing();
	RCALL _hitung_timing
;     713 
;     714                 // Hitung RPM
;     715                 if(waktu4 == 0)
	MOV  R0,R8
	OR   R0,R9
	BRNE _0x6E
;     716                 {
;     717                         waktu4= t4;
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     718                         hitung_rpm();
	RCALL _hitung_rpm
;     719                 }
;     720 
;     721                 // Tampil LCD
;     722                 if(waktu5 == 0)
_0x6E:
	MOV  R0,R10
	OR   R0,R11
	BRNE _0x6F
;     723                 {
;     724                         waktu5= t5;
	LDI  R30,LOW(37500)
	LDI  R31,HIGH(37500)
	MOVW R10,R30
;     725                         tampil_lcd();
	RCALL _tampil_lcd
;     726                 }
;     727         };
_0x6F:
	RJMP _0x60
;     728 }
_0x70:
	RJMP _0x70

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
	BREQ _0x7E
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	RCALL __GETW1P
	MOVW R16,R30
	SBIW R30,0
	BREQ _0x80
	__CPWRN 16,17,2
	BRLO _0x81
	MOVW R30,R16
	SBIW R30,1
	MOVW R16,R30
	ST   X+,R30
	ST   X,R31
_0x80:
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
_0x81:
	RJMP _0x82
_0x7E:
	LDD  R30,Y+6
	ST   -Y,R30
	RCALL _putchar
_0x82:
	RCALL __LOADLOCR2
	ADIW R28,7
	RET
__print_G3:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R17,0
_0x83:
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
	RJMP _0x85
	MOV  R30,R17
	CPI  R30,0
	BRNE _0x89
	CPI  R18,37
	BRNE _0x8A
	LDI  R17,LOW(1)
	RJMP _0x8B
_0x8A:
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
_0x8B:
	RJMP _0x88
_0x89:
	CPI  R30,LOW(0x1)
	BRNE _0x8C
	CPI  R18,37
	BRNE _0x8D
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
	RJMP _0x145
_0x8D:
	LDI  R17,LOW(2)
	LDI  R20,LOW(0)
	LDI  R16,LOW(0)
	CPI  R18,45
	BRNE _0x8E
	LDI  R16,LOW(1)
	RJMP _0x88
_0x8E:
	CPI  R18,43
	BRNE _0x8F
	LDI  R20,LOW(43)
	RJMP _0x88
_0x8F:
	CPI  R18,32
	BRNE _0x90
	LDI  R20,LOW(32)
	RJMP _0x88
_0x90:
	RJMP _0x91
_0x8C:
	CPI  R30,LOW(0x2)
	BRNE _0x92
_0x91:
	LDI  R21,LOW(0)
	LDI  R17,LOW(3)
	CPI  R18,48
	BRNE _0x93
	ORI  R16,LOW(128)
	RJMP _0x88
_0x93:
	RJMP _0x94
_0x92:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x88
_0x94:
	CPI  R18,48
	BRLO _0x97
	CPI  R18,58
	BRLO _0x98
_0x97:
	RJMP _0x96
_0x98:
	MOV  R26,R21
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R21,R30
	RJMP _0x88
_0x96:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0x9C
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
	RJMP _0x9D
_0x9C:
	CPI  R30,LOW(0x73)
	BRNE _0x9F
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
	RJMP _0xA0
_0x9F:
	CPI  R30,LOW(0x70)
	BRNE _0xA2
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
_0xA0:
	ORI  R16,LOW(2)
	ANDI R16,LOW(127)
	LDI  R19,LOW(0)
	RJMP _0xA3
_0xA2:
	CPI  R30,LOW(0x64)
	BREQ _0xA6
	CPI  R30,LOW(0x69)
	BRNE _0xA7
_0xA6:
	ORI  R16,LOW(4)
	RJMP _0xA8
_0xA7:
	CPI  R30,LOW(0x75)
	BRNE _0xA9
_0xA8:
	LDI  R30,LOW(_tbl10_G3*2)
	LDI  R31,HIGH(_tbl10_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(5)
	RJMP _0xAA
_0xA9:
	CPI  R30,LOW(0x58)
	BRNE _0xAC
	ORI  R16,LOW(8)
	RJMP _0xAD
_0xAC:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0xDE
_0xAD:
	LDI  R30,LOW(_tbl16_G3*2)
	LDI  R31,HIGH(_tbl16_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(4)
_0xAA:
	SBRS R16,2
	RJMP _0xAF
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
	BRGE _0xB0
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R20,LOW(45)
_0xB0:
	CPI  R20,0
	BREQ _0xB1
	SUBI R17,-LOW(1)
	RJMP _0xB2
_0xB1:
	ANDI R16,LOW(251)
_0xB2:
	RJMP _0xB3
_0xAF:
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0xB3:
_0xA3:
	SBRC R16,0
	RJMP _0xB4
_0xB5:
	CP   R17,R21
	BRSH _0xB7
	SBRS R16,7
	RJMP _0xB8
	SBRS R16,2
	RJMP _0xB9
	ANDI R16,LOW(251)
	MOV  R18,R20
	SUBI R17,LOW(1)
	RJMP _0xBA
_0xB9:
	LDI  R18,LOW(48)
_0xBA:
	RJMP _0xBB
_0xB8:
	LDI  R18,LOW(32)
_0xBB:
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
	RJMP _0xB5
_0xB7:
_0xB4:
	MOV  R19,R17
	SBRS R16,1
	RJMP _0xBC
_0xBD:
	CPI  R19,0
	BREQ _0xBF
	SBRS R16,3
	RJMP _0xC0
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x146
_0xC0:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x146:
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
	BREQ _0xC2
	SUBI R21,LOW(1)
_0xC2:
	SUBI R19,LOW(1)
	RJMP _0xBD
_0xBF:
	RJMP _0xC3
_0xBC:
_0xC5:
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
_0xC7:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0xC9
	SUBI R18,-LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	SUB  R30,R26
	SBC  R31,R27
	STD  Y+10,R30
	STD  Y+10+1,R31
	RJMP _0xC7
_0xC9:
	CPI  R18,58
	BRLO _0xCA
	SBRS R16,3
	RJMP _0xCB
	SUBI R18,-LOW(7)
	RJMP _0xCC
_0xCB:
	SUBI R18,-LOW(39)
_0xCC:
_0xCA:
	SBRC R16,4
	RJMP _0xCE
	CPI  R18,49
	BRSH _0xD0
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0xCF
_0xD0:
	RJMP _0x147
_0xCF:
	CP   R21,R19
	BRLO _0xD4
	SBRS R16,0
	RJMP _0xD5
_0xD4:
	RJMP _0xD3
_0xD5:
	LDI  R18,LOW(32)
	SBRS R16,7
	RJMP _0xD6
	LDI  R18,LOW(48)
_0x147:
	ORI  R16,LOW(16)
	SBRS R16,2
	RJMP _0xD7
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
	BREQ _0xD8
	SUBI R21,LOW(1)
_0xD8:
_0xD7:
_0xD6:
_0xCE:
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
	BREQ _0xD9
	SUBI R21,LOW(1)
_0xD9:
_0xD3:
	SUBI R19,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0xC6
	RJMP _0xC5
_0xC6:
_0xC3:
	SBRS R16,0
	RJMP _0xDA
_0xDB:
	CPI  R21,0
	BREQ _0xDD
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
	RJMP _0xDB
_0xDD:
_0xDA:
_0xDE:
_0x9D:
_0x145:
	LDI  R17,LOW(0)
_0x88:
	RJMP _0x83
_0x85:
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
	BRSH _0x12B
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
_0x12B:
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
_0x12C:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x12E
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x12C
_0x12E:
	LDD  R17,Y+0
	RJMP _0x139
_lcd_putsf:
	ST   -Y,R17
_0x12F:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x131
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x12F
_0x131:
	LDD  R17,Y+0
_0x139:
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
	BREQ _0x132
	LDI  R30,LOW(0)
	RJMP _0x138
_0x132:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x138:
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
_p_S75:
	.BYTE 0x2

	.CSEG

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

__SUBD12:
	SUB  R30,R26
	SBC  R31,R27
	SBC  R22,R24
	SBC  R23,R25
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

__LSLW2:
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
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
	CLR  R20
	CLR  R21
	LDI  R19,32
__DIVD21U1:
	LSL  R26
	ROL  R27
	ROL  R24
	ROL  R25
	ROL  R0
	ROL  R1
	ROL  R20
	ROL  R21
	SUB  R0,R30
	SBC  R1,R31
	SBC  R20,R22
	SBC  R21,R23
	BRCC __DIVD21U2
	ADD  R0,R30
	ADC  R1,R31
	ADC  R20,R22
	ADC  R21,R23
	RJMP __DIVD21U3
__DIVD21U2:
	SBR  R26,1
__DIVD21U3:
	DEC  R19
	BRNE __DIVD21U1
	MOVW R30,R26
	MOVW R22,R24
	MOVW R26,R0
	MOVW R24,R20
	POP  R21
	POP  R20
	POP  R19
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
