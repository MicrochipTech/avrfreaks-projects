
;CodeVisionAVR C Compiler V1.25.7 beta 5 Professional
;(C) Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8535
;Program type           : Application
;Clock frequency        : 16,000000 MHz
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
;      45 
;      46 #define tdk_diaktifkan          1
;      47 #define mgkn_diaktifkan         2
;      48 #define diaktifkan              3
;      49 #define mgkn_tdk_diaktifkan     4
;      50 
;      51 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      52 
;      53 #define t1                      5000000/20
;      54 #define t2                      2000/20
;      55 #define t3                      3000/20
;      56 #define t4                      1000000/20
;      57 #define t5                      350000/20
;      58 
;      59 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;      60 
;      61 unsigned int waktu1;
;      62 unsigned char waktu2;
;      63 unsigned char waktu3;
;      64 unsigned int waktu4;
;      65 unsigned int waktu5;
;      66 
;      67 char buff[50];
_buff:
	.BYTE 0x32
;      68 
;      69 char keadaan_pin_up;
;      70 bit tanda_pin_up;
;      71 
;      72 char keadaan_pin_down;
;      73 bit tanda_pin_down;
;      74 
;      75 bit penanda_ext_interrupt_rise;
;      76 
;      77 bit penanda_ukur_logika1;
;      78 bit penanda_sedang_ukur_logika1;
;      79 bit penanda_ukur_1periode;
;      80 bit penanda_sedang_ukur_1periode;
;      81 unsigned int durasi_logika1, n;
_durasi_logika1:
	.BYTE 0x2
_n:
	.BYTE 0x2
;      82 unsigned long int durasi_logika0;
_durasi_logika0:
	.BYTE 0x4
;      83 unsigned long int durasi_1periode;
_durasi_1periode:
	.BYTE 0x4
;      84 
;      85 unsigned int jml_pulsa;
_jml_pulsa:
	.BYTE 0x2
;      86 unsigned int rpm;
_rpm:
	.BYTE 0x2
;      87 unsigned long int timing;
_timing:
	.BYTE 0x4
;      88 unsigned int time_delay;
_time_delay:
	.BYTE 0x2
;      89 signed int time_delay_step;// -20 s.d. 60
_time_delay_step:
	.BYTE 0x2
;      90 unsigned int nilai_mutlak;
_nilai_mutlak:
	.BYTE 0x2
;      91 
;      92 signed char i;
_i:
	.BYTE 0x1
;      93 flash unsigned int data_acuan[25]=

	.CSEG
;      94 {
;      95         0,
;      96         667,
;      97         222,
;      98         133,
;      99         95,
;     100         74,
;     101         61,
;     102         51,
;     103         44,
;     104         39,
;     105         35,
;     106         32,
;     107         29,
;     108         27,
;     109         25,
;     110         23,
;     111         22,
;     112         20,
;     113         19,
;     114         18,
;     115         17,
;     116         16,
;     117         16,
;     118         15,
;     119         14
;     120 };
;     121 
;     122 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     123 
;     124 void cek_pin_up(void)
;     125 {
_cek_pin_up:
;     126         switch(keadaan_pin_up)
	MOV  R30,R13
;     127         {
;     128                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x6
;     129                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x7
;     130                                 keadaan_pin_up= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x18B
;     131                         else
_0x7:
;     132                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x18B:
	MOV  R13,R30
;     133                         break;
	RJMP _0x5
;     134                 case mgkn_diaktifkan:
_0x6:
	CPI  R30,LOW(0x2)
	BRNE _0x9
;     135                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xA
;     136                         {
;     137                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     138                                 tanda_pin_up= 1;
	SET
	BLD  R2,0
;     139                         }
;     140                         else
	RJMP _0xB
_0xA:
;     141                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     142                         break;
_0xB:
	RJMP _0x5
;     143                 case diaktifkan:
_0x9:
	CPI  R30,LOW(0x3)
	BRNE _0xC
;     144                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0xD
;     145                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x18C
;     146                         else
_0xD:
;     147                                 keadaan_pin_up= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x18C:
	MOV  R13,R30
;     148                         break;
	RJMP _0x5
;     149                 case mgkn_tdk_diaktifkan:
_0xC:
	CPI  R30,LOW(0x4)
	BRNE _0x5
;     150                         if(pin_up == 0)
	SBIC 0x13,0
	RJMP _0x10
;     151                                 keadaan_pin_up= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R13,R30
;     152                         else
	RJMP _0x11
_0x10:
;     153                         {
;     154                                 keadaan_pin_up= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R13,R30
;     155                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     156                         }
_0x11:
;     157                         break;
;     158         }
_0x5:
;     159 }
	RET
;     160 
;     161 void cek_pin_down(void)
;     162 {
_cek_pin_down:
;     163         switch(keadaan_pin_down)
	MOV  R30,R12
;     164         {
;     165                 case tdk_diaktifkan:
	CPI  R30,LOW(0x1)
	BRNE _0x15
;     166                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x16
;     167                                 keadaan_pin_down= mgkn_diaktifkan;
	LDI  R30,LOW(2)
	RJMP _0x18D
;     168                         else
_0x16:
;     169                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
_0x18D:
	MOV  R12,R30
;     170                         break;
	RJMP _0x14
;     171                 case mgkn_diaktifkan:
_0x15:
	CPI  R30,LOW(0x2)
	BRNE _0x18
;     172                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x19
;     173                         {
;     174                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     175                                 tanda_pin_down= 1;
	SET
	BLD  R2,1
;     176                         }
;     177                         else
	RJMP _0x1A
_0x19:
;     178                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     179                         break;
_0x1A:
	RJMP _0x14
;     180                 case diaktifkan:
_0x18:
	CPI  R30,LOW(0x3)
	BRNE _0x1B
;     181                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1C
;     182                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	RJMP _0x18E
;     183                         else
_0x1C:
;     184                                 keadaan_pin_down= mgkn_tdk_diaktifkan;
	LDI  R30,LOW(4)
_0x18E:
	MOV  R12,R30
;     185                         break;
	RJMP _0x14
;     186                 case mgkn_tdk_diaktifkan:
_0x1B:
	CPI  R30,LOW(0x4)
	BRNE _0x14
;     187                         if(pin_down == 0)
	SBIC 0x13,1
	RJMP _0x1F
;     188                                 keadaan_pin_down= diaktifkan;
	LDI  R30,LOW(3)
	MOV  R12,R30
;     189                         else
	RJMP _0x20
_0x1F:
;     190                         {
;     191                                 keadaan_pin_down= tdk_diaktifkan;
	LDI  R30,LOW(1)
	MOV  R12,R30
;     192                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     193                         }
_0x20:
;     194                         break;
;     195         }
_0x14:
;     196 }
	RET
;     197 
;     198 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     199 
;     200 void aktifkan_pengukur_logika1(void)
;     201 {
_aktifkan_pengukur_logika1:
;     202         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     203         TCCR1B=0x01;
	LDI  R30,LOW(1)
	OUT  0x2E,R30
;     204         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     205         TCNT1L=0x00;
	OUT  0x2C,R30
;     206         ICR1H=0x00;
	OUT  0x27,R30
;     207         ICR1L=0x00;
	OUT  0x26,R30
;     208         OCR1AH=0x00;
	OUT  0x2B,R30
;     209         OCR1AL=0x00;
	OUT  0x2A,R30
;     210         OCR1BH=0x00;
	OUT  0x29,R30
;     211         OCR1BL=0x00;
	OUT  0x28,R30
;     212 
;     213         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     214 }
	RET
;     215 
;     216 void aktifkan_pengukur_1periode(void)
;     217 {
_aktifkan_pengukur_1periode:
;     218         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     219         TCCR1B=0x03;
	LDI  R30,LOW(3)
	OUT  0x2E,R30
;     220         TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     221         TCNT1L=0x00;
	OUT  0x2C,R30
;     222         ICR1H=0x00;
	OUT  0x27,R30
;     223         ICR1L=0x00;
	OUT  0x26,R30
;     224         OCR1AH=0x00;
	OUT  0x2B,R30
;     225         OCR1AL=0x00;
	OUT  0x2A,R30
;     226         OCR1BH=0x00;
	OUT  0x29,R30
;     227         OCR1BL=0x00;
	OUT  0x28,R30
;     228 
;     229         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     230 }
	RET
;     231 
;     232 // Timer shifter
;     233 void aktifkan_timer1_cmpA(unsigned long int nilai_compare)
;     234 {
_aktifkan_timer1_cmpA:
;     235         unsigned char presc= 0x00;
;     236         unsigned int nilai_ocr= 0x0000;
;     237 
;     238         port_output= 0;
	RCALL __SAVELOCR4
;	nilai_compare -> Y+4
;	presc -> R17
;	nilai_ocr -> R18,R19
	LDI  R17,0
	LDI  R18,0
	LDI  R19,0
	CBI  0x12,7
;     239 
;     240         if(nilai_compare > 4000)
	__GETD2S 4
	__CPD2N 0xFA1
	BRLO _0x21
;     241         {
;     242                 presc= 0x03;
	LDI  R17,LOW(3)
;     243                 nilai_ocr= nilai_compare*0.25;
	__GETD1S 4
	RCALL __CDF1U
	__GETD2N 0x3E800000
	RCALL __MULF12
	RCALL __CFD1U
	RJMP _0x18F
;     244         }
;     245         else
_0x21:
;     246         {
;     247                 presc= 0x01;
	LDI  R17,LOW(1)
;     248                 nilai_ocr= nilai_compare*16;
	__GETD2S 4
	__GETD1N 0x10
	RCALL __MULD12U
_0x18F:
	MOVW R18,R30
;     249         }
;     250 
;     251         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     252         TCCR1B= presc;
	OUT  0x2E,R17
;     253         TCNT1H=0x00;
	OUT  0x2D,R30
;     254         TCNT1L=0x00;
	OUT  0x2C,R30
;     255         ICR1H=0x00;
	OUT  0x27,R30
;     256         ICR1L=0x00;
	OUT  0x26,R30
;     257         OCR1A= nilai_ocr;
	__OUTWR 18,19,42
;     258         OCR1BH=0x00;
	OUT  0x29,R30
;     259         OCR1BL=0x00;
	OUT  0x28,R30
;     260 
;     261         TIMSK=0x12;
	LDI  R30,LOW(18)
	OUT  0x39,R30
;     262 }
	RJMP _0x18A
;     263 
;     264 // Timer logika
;     265 void aktifkan_timer1_cmpB(unsigned long int nilai_compare)
;     266 {
_aktifkan_timer1_cmpB:
;     267         unsigned char presc= 0x00;
;     268         unsigned int nilai_ocr= 0x0000;
;     269 
;     270         penanda_ext_interrupt_rise= 0;
	RCALL __SAVELOCR4
;	nilai_compare -> Y+4
;	presc -> R17
;	nilai_ocr -> R18,R19
	LDI  R17,0
	LDI  R18,0
	LDI  R19,0
	CLT
	BLD  R2,2
;     271 
;     272         if(nilai_compare > 4000)
	__GETD2S 4
	__CPD2N 0xFA1
	BRLO _0x23
;     273         {
;     274                 presc= 0x03;
	LDI  R17,LOW(3)
;     275                 nilai_ocr= nilai_compare*0.25;
	__GETD1S 4
	RCALL __CDF1U
	__GETD2N 0x3E800000
	RCALL __MULF12
	RCALL __CFD1U
	RJMP _0x190
;     276         }
;     277         else
_0x23:
;     278         {
;     279                 presc= 0x01;
	LDI  R17,LOW(1)
;     280                 nilai_ocr= nilai_compare*16;
	__GETD2S 4
	__GETD1N 0x10
	RCALL __MULD12U
_0x190:
	MOVW R18,R30
;     281         }
;     282 
;     283         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     284         TCCR1B= presc;
	OUT  0x2E,R17
;     285         TCNT1H=0x00;
	OUT  0x2D,R30
;     286         TCNT1L=0x00;
	OUT  0x2C,R30
;     287         ICR1H=0x00;
	OUT  0x27,R30
;     288         ICR1L=0x00;
	OUT  0x26,R30
;     289         OCR1AH=0x00;
	OUT  0x2B,R30
;     290         OCR1AL=0x00;
	OUT  0x2A,R30
;     291         OCR1B= nilai_ocr;
	__OUTWR 18,19,40
;     292 
;     293         TIMSK=0x0A;
	LDI  R30,LOW(10)
	OUT  0x39,R30
;     294 }
_0x18A:
	RCALL __LOADLOCR4
	ADIW R28,8
	RET
;     295 
;     296 void matikan_timer1(void)
;     297 {
_matikan_timer1:
;     298         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     299         TCCR1B=0x00;
	OUT  0x2E,R30
;     300         TCNT1H=0x00;
	OUT  0x2D,R30
;     301         TCNT1L=0x00;
	OUT  0x2C,R30
;     302         ICR1H=0x00;
	OUT  0x27,R30
;     303         ICR1L=0x00;
	OUT  0x26,R30
;     304         OCR1AH=0x00;
	OUT  0x2B,R30
;     305         OCR1AL=0x00;
	OUT  0x2A,R30
;     306         OCR1BH=0x00;
	OUT  0x29,R30
;     307         OCR1BL=0x00;
	OUT  0x28,R30
;     308 
;     309         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     310 }
	RET
;     311 
;     312 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     313 
;     314 void hitung_timing(void)
;     315 {
_hitung_timing:
;     316         unsigned int tmp= 0;
;     317 
;     318         if(time_delay_step > 0)
	RCALL __SAVELOCR2
;	tmp -> R16,R17
	LDI  R16,0
	LDI  R17,0
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x25
;     319         {
;     320                 timing= (unsigned long int)time_delay*time_delay_step;
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
;     321         }
;     322         else if(time_delay_step < 0)
	RJMP _0x26
_0x25:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x27
;     323         {
;     324                 nilai_mutlak= abs(time_delay_step);
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _abs
	STS  _nilai_mutlak,R30
	STS  _nilai_mutlak+1,R31
;     325                 tmp= time_delay*nilai_mutlak;
	LDS  R26,_time_delay
	LDS  R27,_time_delay+1
	RCALL __MULW12U
	MOVW R16,R30
;     326                 if(durasi_logika1 > tmp)
	LDS  R26,_durasi_logika1
	LDS  R27,_durasi_logika1+1
	CP   R16,R26
	CPC  R17,R27
	BRSH _0x28
;     327                 {
;     328                         timing= (unsigned long int)durasi_logika1-tmp;
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	MOVW R26,R30
	MOVW R24,R22
	MOVW R30,R16
	CLR  R22
	CLR  R23
	RCALL __SUBD21
	STS  _timing,R26
	STS  _timing+1,R27
	STS  _timing+2,R24
	STS  _timing+3,R25
;     329                 }
;     330                 else
	RJMP _0x29
_0x28:
;     331                 {
;     332                         timing= 0;
	LDI  R30,0
	STS  _timing,R30
	STS  _timing+1,R30
	STS  _timing+2,R30
	STS  _timing+3,R30
;     333                 }
_0x29:
;     334         }
;     335         else if(time_delay_step == 0)
	RJMP _0x2A
_0x27:
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x2B
;     336                 timing= 0;
	LDI  R30,0
	STS  _timing,R30
	STS  _timing+1,R30
	STS  _timing+2,R30
	STS  _timing+3,R30
;     337 }
_0x2B:
_0x2A:
_0x26:
	RCALL __LOADLOCR2P
	RET
;     338 
;     339 void increment_shift(void)
;     340 {
_increment_shift:
;     341         if(time_delay_step < 60)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,60
	BRGE _0x2C
;     342         {
;     343                 ++time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	ADIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     344         }
;     345         hitung_timing();
_0x2C:
	RCALL _hitung_timing
;     346 }
	RET
;     347 
;     348 void decrement_shift(void)
;     349 {
_decrement_shift:
;     350         if(time_delay_step > -20)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	LDI  R30,LOW(65516)
	LDI  R31,HIGH(65516)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x2D
;     351         {
;     352                 --time_delay_step;
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,1
	STS  _time_delay_step,R30
	STS  _time_delay_step+1,R31
;     353         }
;     354         hitung_timing();
_0x2D:
	RCALL _hitung_timing
;     355 }
	RET
;     356 
;     357 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     358 
;     359 void hitung_rpm(void)
;     360 {
_hitung_rpm:
;     361         /**
;     362         Satu putaran adalah 1 gelombang.
;     363         jml_pulsa di sini adalah banyaknya gelombang yang terjadi setiap 1 s.
;     364         Satu menit adalah 60x (1 s).
;     365         Sehingga pendekatan perhitungan yang dicapai, rpm= (jml_pulsa*60) rotation per minute
;     366         **/
;     367         rpm= jml_pulsa*60;
	LDS  R26,_jml_pulsa
	LDS  R27,_jml_pulsa+1
	LDI  R30,LOW(60)
	RCALL __MULB1W2U
	STS  _rpm,R30
	STS  _rpm+1,R31
;     368         jml_pulsa= 0;
	LDI  R30,0
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R30
;     369 }
	RET
;     370 
;     371 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     372 
;     373 void tampil_lcd(void)
;     374 {
_tampil_lcd:
;     375         lcd_clear();
	RCALL _lcd_clear
;     376         lcd_putsf("td        RPM");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_putsf
;     377         sprintf(buff, "\n%2i        %5i", time_delay_step, rpm);
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
	LDS  R30,_rpm
	LDS  R31,_rpm+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	LDI  R24,8
	RCALL _sprintf
	ADIW R28,12
;     378         lcd_puts(buff);
	LDI  R30,LOW(_buff)
	LDI  R31,HIGH(_buff)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _lcd_puts
;     379 }
	RET
;     380 
;     381 //+++++++++++++++++++++++++++++++++++++++++++++++++++
;     382 
;     383 // External Interrupt 0 service routine
;     384 interrupt [EXT_INT0] void ext_int0_isr(void)
;     385 {
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
;     386         if(pin_data == 1)
	SBIS 0x10,2
	RJMP _0x2E
;     387         {
;     388                 if(penanda_ukur_logika1)
	SBRS R2,3
	RJMP _0x2F
;     389                 {
;     390                         if(!penanda_sedang_ukur_logika1)
	SBRC R2,4
	RJMP _0x30
;     391                         {
;     392                                 n= 0;
	LDI  R30,0
	STS  _n,R30
	STS  _n+1,R30
;     393                                 penanda_sedang_ukur_logika1= 1;
	SET
	BLD  R2,4
;     394                         }
;     395                 }
_0x30:
;     396                 else if(penanda_ukur_1periode)
	RJMP _0x31
_0x2F:
	SBRS R2,5
	RJMP _0x32
;     397                 {
;     398                         if(penanda_sedang_ukur_1periode)
	SBRS R2,6
	RJMP _0x33
;     399                         {
;     400                                 durasi_logika0= (unsigned long int)TCNT1*4;
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
;     401                                 matikan_timer1();
	RCALL _matikan_timer1
;     402                                 durasi_1periode= durasi_logika0+durasi_logika1;
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
;     403                                 penanda_ukur_1periode= 0;
	CLT
	BLD  R2,5
;     404                                 penanda_sedang_ukur_1periode= 0;
	BLD  R2,6
;     405                         }
;     406                 }
_0x33:
;     407                 else
	RJMP _0x34
_0x32:
;     408                 {
;     409                         if(time_delay_step == 0)
	LDS  R30,_time_delay_step
	LDS  R31,_time_delay_step+1
	SBIW R30,0
	BRNE _0x35
;     410                         {
;     411                                 port_output= 1;
	SBI  0x12,7
;     412                                 aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     413                         }
;     414                         else if(time_delay_step > 0)
	RJMP _0x36
_0x35:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x37
;     415                         {
;     416                                 port_output= 0;
	CBI  0x12,7
;     417                                 aktifkan_timer1_cmpA(timing);// timing shift for logic 0
	LDS  R30,_timing
	LDS  R31,_timing+1
	LDS  R22,_timing+2
	LDS  R23,_timing+3
	RJMP _0x191
;     418                         }
;     419                         else if(time_delay_step < 0)
_0x37:
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x39
;     420                         {
;     421                                 if(timing > 0)
	LDS  R26,_timing
	LDS  R27,_timing+1
	LDS  R24,_timing+2
	LDS  R25,_timing+3
	RCALL __CPD02
	BRSH _0x3A
;     422                                 {
;     423                                         port_output= 1;
	SBI  0x12,7
;     424                                         aktifkan_timer1_cmpB(timing);// timing shift for logic 1
	LDS  R30,_timing
	LDS  R31,_timing+1
	LDS  R22,_timing+2
	LDS  R23,_timing+3
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     425                                 }
;     426                                 else
	RJMP _0x3B
_0x3A:
;     427                                 {
;     428                                         port_output= 0;
	CBI  0x12,7
;     429                                         aktifkan_timer1_cmpA(durasi_logika0);
	LDS  R30,_durasi_logika0
	LDS  R31,_durasi_logika0+1
	LDS  R22,_durasi_logika0+2
	LDS  R23,_durasi_logika0+3
_0x191:
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpA
;     430                                 }
_0x3B:
;     431                         }
;     432                 }
_0x39:
_0x36:
_0x34:
_0x31:
;     433                 ++jml_pulsa;
	LDS  R30,_jml_pulsa
	LDS  R31,_jml_pulsa+1
	ADIW R30,1
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R31
;     434                 penanda_ext_interrupt_rise= 1;
	SET
	BLD  R2,2
;     435         }
;     436 }
_0x2E:
	RJMP _0x196
;     437 
;     438 // Timer 0 output compare interrupt service routine, base time 20 us
;     439 interrupt [TIM0_COMP] void timer0_comp_isr(void)
;     440 {
_timer0_comp_isr:
	ST   -Y,R0
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     441         TCNT0= 0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     442 
;     443         if(n < 64000)
	LDS  R26,_n
	LDS  R27,_n+1
	CPI  R26,LOW(0xFA00)
	LDI  R30,HIGH(0xFA00)
	CPC  R27,R30
	BRSH _0x3C
;     444                 ++n;
	LDS  R30,_n
	LDS  R31,_n+1
	ADIW R30,1
	STS  _n,R30
	STS  _n+1,R31
;     445         if(waktu1 > 0)
_0x3C:
	CLR  R0
	CP   R0,R4
	CPC  R0,R5
	BRSH _0x3D
;     446                 --waktu1;
	MOVW R30,R4
	SBIW R30,1
	MOVW R4,R30
;     447         if(waktu2 > 0)
_0x3D:
	LDI  R30,LOW(0)
	CP   R30,R7
	BRSH _0x3E
;     448                 --waktu2;
	DEC  R7
;     449         if(waktu3 > 0)
_0x3E:
	LDI  R30,LOW(0)
	CP   R30,R6
	BRSH _0x3F
;     450                 --waktu3;
	DEC  R6
;     451         if(waktu4 > 0)
_0x3F:
	CLR  R0
	CP   R0,R8
	CPC  R0,R9
	BRSH _0x40
;     452                 --waktu4;
	MOVW R30,R8
	SBIW R30,1
	MOVW R8,R30
;     453         if(waktu5 > 0)
_0x40:
	CLR  R0
	CP   R0,R10
	CPC  R0,R11
	BRSH _0x41
;     454                 --waktu5;
	MOVW R30,R10
	SBIW R30,1
	MOVW R10,R30
;     455 }
_0x41:
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R0,Y+
	RETI
;     456 
;     457 // Timer 1 output compare A interrupt service routine, timer shifter match
;     458 interrupt [TIM1_COMPA] void timer1_compa_isr(void)
;     459 {
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
;     460         port_output= 1;
	SBI  0x12,7
;     461         matikan_timer1();
	RCALL _matikan_timer1
;     462         if(time_delay_step > 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	RCALL __CPW02
	BRGE _0x42
;     463         {
;     464                 aktifkan_timer1_cmpB(durasi_logika1);
	LDS  R30,_durasi_logika1
	LDS  R31,_durasi_logika1+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpB
;     465         }
;     466 }
_0x42:
	RJMP _0x196
;     467 
;     468 // Timer 1 output compare B interrupt service routine, timer logika 1 match
;     469 interrupt [TIM1_COMPB] void timer1_compb_isr(void)
;     470 {
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
;     471         port_output= 0;
	CBI  0x12,7
;     472         matikan_timer1();
	RCALL _matikan_timer1
;     473         if(time_delay_step < 0)
	LDS  R26,_time_delay_step
	LDS  R27,_time_delay_step+1
	SBIW R26,0
	BRGE _0x43
;     474         {
;     475                 aktifkan_timer1_cmpA(durasi_logika0);
	LDS  R30,_durasi_logika0
	LDS  R31,_durasi_logika0+1
	LDS  R22,_durasi_logika0+2
	LDS  R23,_durasi_logika0+3
	RCALL __PUTPARD1
	RCALL _aktifkan_timer1_cmpA
;     476         }
;     477 }
_0x43:
_0x196:
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
;     478 
;     479 void main(void)
;     480 {
_main:
;     481         /**
;     482         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     483         Port A -- Port LCD
;     484         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     485         **/
;     486         PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     487         DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     488 
;     489         /**
;     490         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     491         Port B -- Dummies
;     492         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     493         **/
;     494         PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;     495         DDRB=0x00;
	OUT  0x17,R30
;     496 
;     497         /**
;     498         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     499         Port C -- Kontrol Shifter
;     500         C.0: Up
;     501         C.1: Down
;     502         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     503         **/
;     504         PORTC=0x03;
	LDI  R30,LOW(3)
	OUT  0x15,R30
;     505         DDRC=0x00;
	LDI  R30,LOW(0)
	OUT  0x14,R30
;     506 
;     507         /**
;     508         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     509         Port D
;     510         D.2: INT0
;     511         D.7: Shifted Output Pulse
;     512         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;     513         **/
;     514         PORTD=0x04;
	LDI  R30,LOW(4)
	OUT  0x12,R30
;     515         DDRD=0x80;
	LDI  R30,LOW(128)
	OUT  0x11,R30
;     516 
;     517         // Timer/Counter 0 initialization
;     518         // Clock source: System Clock
;     519         // Clock value: 2000.000 kHz
;     520         // Mode: Normal top=FFh
;     521         // OC0 output: Disconnected
;     522         TCCR0=0x02;
	LDI  R30,LOW(2)
	OUT  0x33,R30
;     523         TCNT0=0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;     524         OCR0=0x28;
	LDI  R30,LOW(40)
	OUT  0x3C,R30
;     525 
;     526         // Timer/Counter 1 initialization
;     527         // Clock source: System Clock
;     528         // Clock value: Timer 1 Stopped
;     529         // Mode: Normal top=FFFFh
;     530         // OC1A output: Discon.
;     531         // OC1B output: Discon.
;     532         // Noise Canceler: Off
;     533         // Input Capture on Falling Edge
;     534         // Timer 1 Overflow Interrupt: Off
;     535         // Input Capture Interrupt: Off
;     536         // Compare A Match Interrupt: Off
;     537         // Compare B Match Interrupt: Off
;     538         TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;     539         TCCR1B=0x00;
	OUT  0x2E,R30
;     540         TCNT1H=0x00;
	OUT  0x2D,R30
;     541         TCNT1L=0x00;
	OUT  0x2C,R30
;     542         ICR1H=0x00;
	OUT  0x27,R30
;     543         ICR1L=0x00;
	OUT  0x26,R30
;     544         OCR1AH=0x00;
	OUT  0x2B,R30
;     545         OCR1AL=0x00;
	OUT  0x2A,R30
;     546         OCR1BH=0x00;
	OUT  0x29,R30
;     547         OCR1BL=0x00;
	OUT  0x28,R30
;     548 
;     549         // Timer/Counter 2 initialization
;     550         // Clock source: System Clock
;     551         // Clock value: Timer 2 Stopped
;     552         // Mode: Normal top=FFh
;     553         // OC2 output: Disconnected
;     554         ASSR=0x00;
	OUT  0x22,R30
;     555         TCCR2=0x00;
	OUT  0x25,R30
;     556         TCNT2=0x00;
	OUT  0x24,R30
;     557         OCR2=0x00;
	OUT  0x23,R30
;     558 
;     559         // External Interrupt(s) initialization
;     560         // INT0: On
;     561         // INT0 Mode: Rising Edge
;     562         // INT1: Off
;     563         // INT2: Off
;     564         GICR|=0x40;
	IN   R30,0x3B
	ORI  R30,0x40
	OUT  0x3B,R30
;     565         MCUCR=0x03;
	LDI  R30,LOW(3)
	OUT  0x35,R30
;     566         MCUCSR=0x00;
	LDI  R30,LOW(0)
	OUT  0x34,R30
;     567         GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
;     568 
;     569         // Timer(s)/Counter(s) Interrupt(s) initialization
;     570         TIMSK=0x02;
	LDI  R30,LOW(2)
	OUT  0x39,R30
;     571 
;     572         // Analog Comparator initialization
;     573         // Analog Comparator: Off
;     574         // Analog Comparator Input Capture by Timer/Counter 1: Off
;     575         ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     576         SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     577 
;     578         // LCD module initialization
;     579         lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
;     580         lcd_clear();
	RCALL _lcd_clear
;     581 
;     582         // Global enable interrupts
;     583         #asm("sei")
	sei
;     584 
;     585         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,2
;     586         // Dapatkan data awal durasi pulsa logika 1
;     587         while(!penanda_ext_interrupt_rise);
_0x44:
	SBRS R2,2
	RJMP _0x44
;     588         #asm("cli")
	cli
;     589         aktifkan_pengukur_logika1();
	RCALL _aktifkan_pengukur_logika1
;     590         while(pin_data);
_0x47:
	SBIC 0x10,2
	RJMP _0x47
;     591         durasi_logika1= TCNT1/16;
	IN   R30,0x2C
	IN   R31,0x2C+1
	RCALL __LSRW4
	STS  _durasi_logika1,R30
	STS  _durasi_logika1+1,R31
;     592         matikan_timer1();
	RCALL _matikan_timer1
;     593 
;     594         // Global enable interrupts
;     595         #asm("sei")
	sei
;     596 
;     597         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,2
;     598         // Dapatkan data awal durasi 1 periode pulsa
;     599         while(!penanda_ext_interrupt_rise);
_0x4A:
	SBRS R2,2
	RJMP _0x4A
;     600         #asm("cli")
	cli
;     601         aktifkan_pengukur_1periode();
	RCALL _aktifkan_pengukur_1periode
;     602         while(pin_data);
_0x4D:
	SBIC 0x10,2
	RJMP _0x4D
;     603         while(!pin_data);
_0x50:
	SBIS 0x10,2
	RJMP _0x50
;     604         durasi_1periode= (unsigned long int)TCNT1*4;
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
;     605         matikan_timer1();
	RCALL _matikan_timer1
;     606 
;     607         // Dapatkan data awal durasi pulsa logika 0
;     608         durasi_logika0= durasi_1periode-durasi_logika1;
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
;     609 
;     610         // Global disable interrupts
;     611         #asm("cli")
	cli
;     612 
;     613         // Timer scheduling
;     614         waktu1= t1;// 3 detik
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     615         waktu2= t2;// 2 mili detik
	LDI  R30,LOW(100)
	MOV  R7,R30
;     616         waktu3= t3;// 3 mili detik
	LDI  R30,LOW(150)
	MOV  R6,R30
;     617         waktu4= t4;// 1 detik
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     618         waktu5= t5;// 400 mili detik
	LDI  R30,LOW(17500)
	LDI  R31,HIGH(17500)
	MOVW R10,R30
;     619 
;     620         // Inisialisasi data
;     621         port_output= 0;
	CBI  0x12,7
;     622         penanda_ext_interrupt_rise= 0;
	CLT
	BLD  R2,2
;     623         penanda_ukur_logika1= 0;
	BLD  R2,3
;     624         penanda_sedang_ukur_logika1= 0;
	BLD  R2,4
;     625         penanda_ukur_1periode= 0;
	BLD  R2,5
;     626         penanda_sedang_ukur_1periode= 0;
	BLD  R2,6
;     627         n= 0;
	LDI  R30,0
	STS  _n,R30
	STS  _n+1,R30
;     628         jml_pulsa= 0;
	LDI  R30,0
	STS  _jml_pulsa,R30
	STS  _jml_pulsa+1,R30
;     629         rpm= 0;
	LDI  R30,0
	STS  _rpm,R30
	STS  _rpm+1,R30
;     630         i= 1;
	LDI  R30,LOW(1)
	STS  _i,R30
;     631 
;     632         // Inisialisasi pin control
;     633         tanda_pin_up= 0;
	BLD  R2,0
;     634         keadaan_pin_up= tdk_diaktifkan;
	MOV  R13,R30
;     635         tanda_pin_down= 0;
	BLD  R2,1
;     636         keadaan_pin_down= tdk_diaktifkan;
	MOV  R12,R30
;     637 
;     638         // Global enable interrupts
;     639         #asm("sei")
	sei
;     640 
;     641         // Main eternal loop
;     642         for(;;)
_0x54:
;     643         {
;     644                 if(waktu1 == 0)
	MOV  R0,R4
	OR   R0,R5
	BRNE _0x56
;     645                 {
;     646                         waktu1= t1;
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     647                         penanda_ukur_logika1= 1;
	SET
	BLD  R2,3
;     648                 }
;     649 
;     650                 // Eksekusi kalibrasi durasi logika 1
;     651                 if(penanda_ukur_logika1)
_0x56:
	SBRS R2,3
	RJMP _0x57
;     652                 {
;     653                         if(penanda_sedang_ukur_logika1)
	SBRS R2,4
	RJMP _0x58
;     654                         {
;     655                                 if(!pin_data)
	SBIC 0x10,2
	RJMP _0x59
;     656                                 {
;     657                                         durasi_logika1= (unsigned long int)n*20;
	LDS  R30,_n
	LDS  R31,_n+1
	CLR  R22
	CLR  R23
	__GETD2N 0x14
	RCALL __MULD12U
	STS  _durasi_logika1,R30
	STS  _durasi_logika1+1,R31
;     658                                         aktifkan_pengukur_1periode();
	RCALL _aktifkan_pengukur_1periode
;     659                                         penanda_ukur_logika1= 0;
	CLT
	BLD  R2,3
;     660                                         penanda_sedang_ukur_logika1= 0;
	BLD  R2,4
;     661                                         penanda_ukur_1periode= 1;
	SET
	BLD  R2,5
;     662 
;     663                                         waktu1= t1;
	LDI  R30,LOW(250000)
	LDI  R31,HIGH(250000)
	MOVW R4,R30
;     664                                 }
;     665                         }
_0x59:
;     666                 }
_0x58:
;     667 
;     668                 // Eksekusi kalibrasi durasi 1 periode dan logika 0
;     669                 if(penanda_ukur_1periode)
_0x57:
	SBRS R2,5
	RJMP _0x5A
;     670                 {
;     671                         penanda_sedang_ukur_1periode= 1;
	SET
	BLD  R2,6
;     672                 }
;     673 
;     674                 // Scanning tombol
;     675                 if(waktu2 == 0)
_0x5A:
	TST  R7
	BRNE _0x5B
;     676                 {
;     677                         waktu2= t2;
	LDI  R30,LOW(100)
	MOV  R7,R30
;     678                         cek_pin_up();
	RCALL _cek_pin_up
;     679                         cek_pin_down();
	RCALL _cek_pin_down
;     680                 }
;     681 
;     682                 // Penentuan keadaan pin_up dan pin_down
;     683                 if(waktu3 == 0)
_0x5B:
	TST  R6
	BRNE _0x5C
;     684                 {
;     685                         waktu3= t3;
	LDI  R30,LOW(150)
	MOV  R6,R30
;     686                         if(tanda_pin_up)
	SBRS R2,0
	RJMP _0x5D
;     687                         {
;     688                                 tanda_pin_up= 0;
	CLT
	BLD  R2,0
;     689                                 increment_shift();
	RCALL _increment_shift
;     690                         }
;     691                         if(tanda_pin_down)
_0x5D:
	SBRS R2,1
	RJMP _0x5E
;     692                         {
;     693                                 tanda_pin_down= 0;
	CLT
	BLD  R2,1
;     694                                 decrement_shift();
	RCALL _decrement_shift
;     695                         }
;     696                 }
_0x5E:
;     697 
;     698                 // Hitung RPM
;     699                 if(waktu4 == 0)
_0x5C:
	MOV  R0,R8
	OR   R0,R9
	BRNE _0x5F
;     700                 {
;     701                         waktu4= t4;
	LDI  R30,LOW(50000)
	LDI  R31,HIGH(50000)
	MOVW R8,R30
;     702                         hitung_rpm();
	RCALL _hitung_rpm
;     703                 }
;     704 
;     705                 //Penentuan timing
;     706                 if(rpm > 0 && rpm <= 250)
_0x5F:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	RCALL __CPW02
	BRSH _0x61
	CPI  R26,LOW(0xFB)
	LDI  R30,HIGH(0xFB)
	CPC  R27,R30
	BRLO _0x62
_0x61:
	RJMP _0x60
_0x62:
;     707                         i= 1;
	LDI  R30,LOW(1)
	RJMP _0x192
;     708                 else if(rpm > 250 && rpm <= 750)
_0x60:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0xFB)
	LDI  R30,HIGH(0xFB)
	CPC  R27,R30
	BRLO _0x65
	CPI  R26,LOW(0x2EF)
	LDI  R30,HIGH(0x2EF)
	CPC  R27,R30
	BRLO _0x66
_0x65:
	RJMP _0x64
_0x66:
;     709                         i= 2;
	LDI  R30,LOW(2)
	RJMP _0x192
;     710                 else if(rpm > 750 && rpm <= 1250)
_0x64:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x2EF)
	LDI  R30,HIGH(0x2EF)
	CPC  R27,R30
	BRLO _0x69
	CPI  R26,LOW(0x4E3)
	LDI  R30,HIGH(0x4E3)
	CPC  R27,R30
	BRLO _0x6A
_0x69:
	RJMP _0x68
_0x6A:
;     711                         i= 3;
	LDI  R30,LOW(3)
	RJMP _0x192
;     712                 else if(rpm > 1250 && rpm <= 1750)
_0x68:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x4E3)
	LDI  R30,HIGH(0x4E3)
	CPC  R27,R30
	BRLO _0x6D
	CPI  R26,LOW(0x6D7)
	LDI  R30,HIGH(0x6D7)
	CPC  R27,R30
	BRLO _0x6E
_0x6D:
	RJMP _0x6C
_0x6E:
;     713                         i= 4;
	LDI  R30,LOW(4)
	RJMP _0x192
;     714                 else if(rpm > 1750 && rpm <= 2250)
_0x6C:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x6D7)
	LDI  R30,HIGH(0x6D7)
	CPC  R27,R30
	BRLO _0x71
	CPI  R26,LOW(0x8CB)
	LDI  R30,HIGH(0x8CB)
	CPC  R27,R30
	BRLO _0x72
_0x71:
	RJMP _0x70
_0x72:
;     715                         i= 5;
	LDI  R30,LOW(5)
	RJMP _0x192
;     716                 else if(rpm > 2250 && rpm <= 2750)
_0x70:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x8CB)
	LDI  R30,HIGH(0x8CB)
	CPC  R27,R30
	BRLO _0x75
	CPI  R26,LOW(0xABF)
	LDI  R30,HIGH(0xABF)
	CPC  R27,R30
	BRLO _0x76
_0x75:
	RJMP _0x74
_0x76:
;     717                         i= 6;
	LDI  R30,LOW(6)
	RJMP _0x192
;     718                 else if(rpm > 2750 && rpm <= 3250)
_0x74:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0xABF)
	LDI  R30,HIGH(0xABF)
	CPC  R27,R30
	BRLO _0x79
	CPI  R26,LOW(0xCB3)
	LDI  R30,HIGH(0xCB3)
	CPC  R27,R30
	BRLO _0x7A
_0x79:
	RJMP _0x78
_0x7A:
;     719                         i= 7;
	LDI  R30,LOW(7)
	RJMP _0x192
;     720                 else if(rpm > 3250 && rpm <= 3750)
_0x78:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0xCB3)
	LDI  R30,HIGH(0xCB3)
	CPC  R27,R30
	BRLO _0x7D
	CPI  R26,LOW(0xEA7)
	LDI  R30,HIGH(0xEA7)
	CPC  R27,R30
	BRLO _0x7E
_0x7D:
	RJMP _0x7C
_0x7E:
;     721                         i= 8;
	LDI  R30,LOW(8)
	RJMP _0x192
;     722                 else if(rpm > 3750 && rpm <= 4250)
_0x7C:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0xEA7)
	LDI  R30,HIGH(0xEA7)
	CPC  R27,R30
	BRLO _0x81
	CPI  R26,LOW(0x109B)
	LDI  R30,HIGH(0x109B)
	CPC  R27,R30
	BRLO _0x82
_0x81:
	RJMP _0x80
_0x82:
;     723                         i= 9;
	LDI  R30,LOW(9)
	RJMP _0x192
;     724                 else if(rpm > 4250 && rpm <= 4750)
_0x80:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x109B)
	LDI  R30,HIGH(0x109B)
	CPC  R27,R30
	BRLO _0x85
	CPI  R26,LOW(0x128F)
	LDI  R30,HIGH(0x128F)
	CPC  R27,R30
	BRLO _0x86
_0x85:
	RJMP _0x84
_0x86:
;     725                         i= 10;
	LDI  R30,LOW(10)
	RJMP _0x192
;     726                 else if(rpm > 4750 && rpm <= 5250)
_0x84:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x128F)
	LDI  R30,HIGH(0x128F)
	CPC  R27,R30
	BRLO _0x89
	CPI  R26,LOW(0x1483)
	LDI  R30,HIGH(0x1483)
	CPC  R27,R30
	BRLO _0x8A
_0x89:
	RJMP _0x88
_0x8A:
;     727                         i= 11;
	LDI  R30,LOW(11)
	RJMP _0x192
;     728                 else if(rpm > 5250 && rpm <= 5750)
_0x88:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x1483)
	LDI  R30,HIGH(0x1483)
	CPC  R27,R30
	BRLO _0x8D
	CPI  R26,LOW(0x1677)
	LDI  R30,HIGH(0x1677)
	CPC  R27,R30
	BRLO _0x8E
_0x8D:
	RJMP _0x8C
_0x8E:
;     729                         i= 12;
	LDI  R30,LOW(12)
	RJMP _0x192
;     730                 else if(rpm > 5750 && rpm <= 6250)
_0x8C:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x1677)
	LDI  R30,HIGH(0x1677)
	CPC  R27,R30
	BRLO _0x91
	CPI  R26,LOW(0x186B)
	LDI  R30,HIGH(0x186B)
	CPC  R27,R30
	BRLO _0x92
_0x91:
	RJMP _0x90
_0x92:
;     731                         i= 13;
	LDI  R30,LOW(13)
	RJMP _0x192
;     732                 else if(rpm > 6250 && rpm <= 6750)
_0x90:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x186B)
	LDI  R30,HIGH(0x186B)
	CPC  R27,R30
	BRLO _0x95
	CPI  R26,LOW(0x1A5F)
	LDI  R30,HIGH(0x1A5F)
	CPC  R27,R30
	BRLO _0x96
_0x95:
	RJMP _0x94
_0x96:
;     733                         i= 14;
	LDI  R30,LOW(14)
	RJMP _0x192
;     734                 else if(rpm > 6750 && rpm <= 7250)
_0x94:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x1A5F)
	LDI  R30,HIGH(0x1A5F)
	CPC  R27,R30
	BRLO _0x99
	CPI  R26,LOW(0x1C53)
	LDI  R30,HIGH(0x1C53)
	CPC  R27,R30
	BRLO _0x9A
_0x99:
	RJMP _0x98
_0x9A:
;     735                         i= 15;
	LDI  R30,LOW(15)
	RJMP _0x192
;     736                 else if(rpm > 7250 && rpm <= 7750)
_0x98:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x1C53)
	LDI  R30,HIGH(0x1C53)
	CPC  R27,R30
	BRLO _0x9D
	CPI  R26,LOW(0x1E47)
	LDI  R30,HIGH(0x1E47)
	CPC  R27,R30
	BRLO _0x9E
_0x9D:
	RJMP _0x9C
_0x9E:
;     737                         i= 16;
	LDI  R30,LOW(16)
	RJMP _0x192
;     738                 else if(rpm > 7750 && rpm <= 8250)
_0x9C:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x1E47)
	LDI  R30,HIGH(0x1E47)
	CPC  R27,R30
	BRLO _0xA1
	CPI  R26,LOW(0x203B)
	LDI  R30,HIGH(0x203B)
	CPC  R27,R30
	BRLO _0xA2
_0xA1:
	RJMP _0xA0
_0xA2:
;     739                         i= 17;
	LDI  R30,LOW(17)
	RJMP _0x192
;     740                 else if(rpm > 8250 && rpm <= 8750)
_0xA0:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x203B)
	LDI  R30,HIGH(0x203B)
	CPC  R27,R30
	BRLO _0xA5
	CPI  R26,LOW(0x222F)
	LDI  R30,HIGH(0x222F)
	CPC  R27,R30
	BRLO _0xA6
_0xA5:
	RJMP _0xA4
_0xA6:
;     741                         i= 18;
	LDI  R30,LOW(18)
	RJMP _0x192
;     742                 else if(rpm > 8750 && rpm <= 9250)
_0xA4:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x222F)
	LDI  R30,HIGH(0x222F)
	CPC  R27,R30
	BRLO _0xA9
	CPI  R26,LOW(0x2423)
	LDI  R30,HIGH(0x2423)
	CPC  R27,R30
	BRLO _0xAA
_0xA9:
	RJMP _0xA8
_0xAA:
;     743                         i= 19;
	LDI  R30,LOW(19)
	RJMP _0x192
;     744                 else if(rpm > 9250 && rpm <= 9750)
_0xA8:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x2423)
	LDI  R30,HIGH(0x2423)
	CPC  R27,R30
	BRLO _0xAD
	CPI  R26,LOW(0x2617)
	LDI  R30,HIGH(0x2617)
	CPC  R27,R30
	BRLO _0xAE
_0xAD:
	RJMP _0xAC
_0xAE:
;     745                         i= 20;
	LDI  R30,LOW(20)
	RJMP _0x192
;     746                 else if(rpm > 9750 && rpm <= 10250)
_0xAC:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x2617)
	LDI  R30,HIGH(0x2617)
	CPC  R27,R30
	BRLO _0xB1
	CPI  R26,LOW(0x280B)
	LDI  R30,HIGH(0x280B)
	CPC  R27,R30
	BRLO _0xB2
_0xB1:
	RJMP _0xB0
_0xB2:
;     747                         i= 21;
	LDI  R30,LOW(21)
	RJMP _0x192
;     748                 else if(rpm > 10250 && rpm <= 10750)
_0xB0:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x280B)
	LDI  R30,HIGH(0x280B)
	CPC  R27,R30
	BRLO _0xB5
	CPI  R26,LOW(0x29FF)
	LDI  R30,HIGH(0x29FF)
	CPC  R27,R30
	BRLO _0xB6
_0xB5:
	RJMP _0xB4
_0xB6:
;     749                         i= 22;
	LDI  R30,LOW(22)
	RJMP _0x192
;     750                 else if(rpm > 10750 && rpm <= 11250)
_0xB4:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x29FF)
	LDI  R30,HIGH(0x29FF)
	CPC  R27,R30
	BRLO _0xB9
	CPI  R26,LOW(0x2BF3)
	LDI  R30,HIGH(0x2BF3)
	CPC  R27,R30
	BRLO _0xBA
_0xB9:
	RJMP _0xB8
_0xBA:
;     751                         i= 23;
	LDI  R30,LOW(23)
	RJMP _0x192
;     752                 else if(rpm > 11250 && rpm <= 11750)
_0xB8:
	LDS  R26,_rpm
	LDS  R27,_rpm+1
	CPI  R26,LOW(0x2BF3)
	LDI  R30,HIGH(0x2BF3)
	CPC  R27,R30
	BRLO _0xBD
	CPI  R26,LOW(0x2DE7)
	LDI  R30,HIGH(0x2DE7)
	CPC  R27,R30
	BRLO _0xBE
_0xBD:
	RJMP _0xBC
_0xBE:
;     753                         i= 24;
	LDI  R30,LOW(24)
_0x192:
	STS  _i,R30
;     754                 time_delay= data_acuan[i];
_0xBC:
	LDS  R30,_i
	LDI  R26,LOW(_data_acuan*2)
	LDI  R27,HIGH(_data_acuan*2)
	LDI  R31,0
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	RCALL __GETW1PF
	STS  _time_delay,R30
	STS  _time_delay+1,R31
;     755 
;     756                 // Hitung timing
;     757                 hitung_timing();
	RCALL _hitung_timing
;     758 
;     759                 // Tampil LCD
;     760                 if(waktu5 == 0)
	MOV  R0,R10
	OR   R0,R11
	BRNE _0xBF
;     761                 {
;     762                         waktu5= t5;
	LDI  R30,LOW(17500)
	LDI  R31,HIGH(17500)
	MOVW R10,R30
;     763                         tampil_lcd();
	RCALL _tampil_lcd
;     764                 }
;     765         };
_0xBF:
	RJMP _0x54
;     766 }
_0xC0:
	RJMP _0xC0

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
	BREQ _0xCE
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	RCALL __GETW1P
	MOVW R16,R30
	SBIW R30,0
	BREQ _0xD0
	__CPWRN 16,17,2
	BRLO _0xD1
	MOVW R30,R16
	SBIW R30,1
	MOVW R16,R30
	ST   X+,R30
	ST   X,R31
_0xD0:
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
_0xD1:
	RJMP _0xD2
_0xCE:
	LDD  R30,Y+6
	ST   -Y,R30
	RCALL _putchar
_0xD2:
	RCALL __LOADLOCR2
	ADIW R28,7
	RET
__print_G3:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R17,0
_0xD3:
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
	RJMP _0xD5
	MOV  R30,R17
	CPI  R30,0
	BRNE _0xD9
	CPI  R18,37
	BRNE _0xDA
	LDI  R17,LOW(1)
	RJMP _0xDB
_0xDA:
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
_0xDB:
	RJMP _0xD8
_0xD9:
	CPI  R30,LOW(0x1)
	BRNE _0xDC
	CPI  R18,37
	BRNE _0xDD
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
	RJMP _0x193
_0xDD:
	LDI  R17,LOW(2)
	LDI  R20,LOW(0)
	LDI  R16,LOW(0)
	CPI  R18,45
	BRNE _0xDE
	LDI  R16,LOW(1)
	RJMP _0xD8
_0xDE:
	CPI  R18,43
	BRNE _0xDF
	LDI  R20,LOW(43)
	RJMP _0xD8
_0xDF:
	CPI  R18,32
	BRNE _0xE0
	LDI  R20,LOW(32)
	RJMP _0xD8
_0xE0:
	RJMP _0xE1
_0xDC:
	CPI  R30,LOW(0x2)
	BRNE _0xE2
_0xE1:
	LDI  R21,LOW(0)
	LDI  R17,LOW(3)
	CPI  R18,48
	BRNE _0xE3
	ORI  R16,LOW(128)
	RJMP _0xD8
_0xE3:
	RJMP _0xE4
_0xE2:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0xD8
_0xE4:
	CPI  R18,48
	BRLO _0xE7
	CPI  R18,58
	BRLO _0xE8
_0xE7:
	RJMP _0xE6
_0xE8:
	MOV  R26,R21
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R30,R18
	SUBI R30,LOW(48)
	ADD  R21,R30
	RJMP _0xD8
_0xE6:
	MOV  R30,R18
	CPI  R30,LOW(0x63)
	BRNE _0xEC
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
	RJMP _0xED
_0xEC:
	CPI  R30,LOW(0x73)
	BRNE _0xEF
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
	RJMP _0xF0
_0xEF:
	CPI  R30,LOW(0x70)
	BRNE _0xF2
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
_0xF0:
	ORI  R16,LOW(2)
	ANDI R16,LOW(127)
	LDI  R19,LOW(0)
	RJMP _0xF3
_0xF2:
	CPI  R30,LOW(0x64)
	BREQ _0xF6
	CPI  R30,LOW(0x69)
	BRNE _0xF7
_0xF6:
	ORI  R16,LOW(4)
	RJMP _0xF8
_0xF7:
	CPI  R30,LOW(0x75)
	BRNE _0xF9
_0xF8:
	LDI  R30,LOW(_tbl10_G3*2)
	LDI  R31,HIGH(_tbl10_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(5)
	RJMP _0xFA
_0xF9:
	CPI  R30,LOW(0x58)
	BRNE _0xFC
	ORI  R16,LOW(8)
	RJMP _0xFD
_0xFC:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x12E
_0xFD:
	LDI  R30,LOW(_tbl16_G3*2)
	LDI  R31,HIGH(_tbl16_G3*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R17,LOW(4)
_0xFA:
	SBRS R16,2
	RJMP _0xFF
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
	BRGE _0x100
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R20,LOW(45)
_0x100:
	CPI  R20,0
	BREQ _0x101
	SUBI R17,-LOW(1)
	RJMP _0x102
_0x101:
	ANDI R16,LOW(251)
_0x102:
	RJMP _0x103
_0xFF:
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	SBIW R26,4
	STD  Y+16,R26
	STD  Y+16+1,R27
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
_0x103:
_0xF3:
	SBRC R16,0
	RJMP _0x104
_0x105:
	CP   R17,R21
	BRSH _0x107
	SBRS R16,7
	RJMP _0x108
	SBRS R16,2
	RJMP _0x109
	ANDI R16,LOW(251)
	MOV  R18,R20
	SUBI R17,LOW(1)
	RJMP _0x10A
_0x109:
	LDI  R18,LOW(48)
_0x10A:
	RJMP _0x10B
_0x108:
	LDI  R18,LOW(32)
_0x10B:
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
	RJMP _0x105
_0x107:
_0x104:
	MOV  R19,R17
	SBRS R16,1
	RJMP _0x10C
_0x10D:
	CPI  R19,0
	BREQ _0x10F
	SBRS R16,3
	RJMP _0x110
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x194
_0x110:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x194:
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
	BREQ _0x112
	SUBI R21,LOW(1)
_0x112:
	SUBI R19,LOW(1)
	RJMP _0x10D
_0x10F:
	RJMP _0x113
_0x10C:
_0x115:
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
_0x117:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0x119
	SUBI R18,-LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	SUB  R30,R26
	SBC  R31,R27
	STD  Y+10,R30
	STD  Y+10+1,R31
	RJMP _0x117
_0x119:
	CPI  R18,58
	BRLO _0x11A
	SBRS R16,3
	RJMP _0x11B
	SUBI R18,-LOW(7)
	RJMP _0x11C
_0x11B:
	SUBI R18,-LOW(39)
_0x11C:
_0x11A:
	SBRC R16,4
	RJMP _0x11E
	CPI  R18,49
	BRSH _0x120
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0x11F
_0x120:
	RJMP _0x195
_0x11F:
	CP   R21,R19
	BRLO _0x124
	SBRS R16,0
	RJMP _0x125
_0x124:
	RJMP _0x123
_0x125:
	LDI  R18,LOW(32)
	SBRS R16,7
	RJMP _0x126
	LDI  R18,LOW(48)
_0x195:
	ORI  R16,LOW(16)
	SBRS R16,2
	RJMP _0x127
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
	BREQ _0x128
	SUBI R21,LOW(1)
_0x128:
_0x127:
_0x126:
_0x11E:
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
	BREQ _0x129
	SUBI R21,LOW(1)
_0x129:
_0x123:
	SUBI R19,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0x116
	RJMP _0x115
_0x116:
_0x113:
	SBRS R16,0
	RJMP _0x12A
_0x12B:
	CPI  R21,0
	BREQ _0x12D
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
	RJMP _0x12B
_0x12D:
_0x12A:
_0x12E:
_0xED:
_0x193:
	LDI  R17,LOW(0)
_0xD8:
	RJMP _0xD3
_0xD5:
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
	BRSH _0x17B
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
_0x17B:
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
_0x17C:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x17E
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x17C
_0x17E:
	LDD  R17,Y+0
	RJMP _0x189
_lcd_putsf:
	ST   -Y,R17
_0x17F:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x181
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x17F
_0x181:
	LDD  R17,Y+0
_0x189:
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
	BREQ _0x182
	LDI  R30,LOW(0)
	RJMP _0x188
_0x182:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	RCALL __lcd_write_data
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x188:
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
_p_S72:
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

__CPD02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
	CPC  R0,R24
	CPC  R0,R25
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

;END OF CODE MARKER
__END_OF_CODE:
