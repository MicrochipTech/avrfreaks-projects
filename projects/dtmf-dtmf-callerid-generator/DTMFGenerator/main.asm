
;CodeVisionAVR C Compiler V1.25.5 Professional
;(C) Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8
;Program type           : Application
;Clock frequency        : 8.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 256 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : No
;8 bit enums            : No
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8
	#pragma AVRPART MEMORY PROG_FLASH 8192
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 1024
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

	.INCLUDE "main.vec"
	.INCLUDE "main.inc"

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
	LDI  R24,LOW(0x400)
	LDI  R25,HIGH(0x400)
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
	LDI  R30,LOW(0x45F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x45F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x160)
	LDI  R29,HIGH(0x160)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x160
;       1 //******************************************************
;       2 // behzad khazama 2007/06/04 SABZEVAR-IRAN
;       3 // www.khazama.com
;       4 //
;       5 // to use this project as DTMF caller id generator
;       6 // connect tel lines as bellow
;       7 //
;       8 //  use 2 series capacitor as bellow
;       9 //  TEL(+) --||--*--||-- PORTB.1 (OC1A)
;      10 //          +  -   -  +
;      11 //       (1uf 50v) (1uf 50v)
;      12 //
;      13 //  TEL(-) -- connect to GND
;      14 //
;      15 //******************************************************
;      16 
;      17 
;      18 //******************************************************
;      19 // DTMF caller id signals generate before first ring
;      20 // it starts with DTMF signal D (A or B may use too)
;      21 // and terminate with DTMF signal C
;      22 // between D and C 'caller number' will generate
;      23 //
;      24 // D(number...)C ------- first ring
;      25 //******************************************************
;      26 
;      27 #include <mega8.h>
;      28 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;      29 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;      30 	.EQU __se_bit=0x80
	.EQU __se_bit=0x80
;      31 	.EQU __sm_mask=0x70
	.EQU __sm_mask=0x70
;      32 	.EQU __sm_powerdown=0x20
	.EQU __sm_powerdown=0x20
;      33 	.EQU __sm_powersave=0x30
	.EQU __sm_powersave=0x30
;      34 	.EQU __sm_standby=0x60
	.EQU __sm_standby=0x60
;      35 	.EQU __sm_ext_standby=0x70
	.EQU __sm_ext_standby=0x70
;      36 	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_adc_noise_red=0x10
;      37 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;      38 	#endif
	#endif
;      39 #include <string.h>
;      40 #include "dtmf.h"
;      41 
;      42 void main(void){

	.CSEG
_main:
;      43    char num[15];
;      44         dtmf_init();
	SBIW R28,15
;	num -> Y+0
	RCALL _dtmf_init
;      45         strcpyf(num,"D09155714862C");
	RCALL SUBOPT_0x0
	__POINTW1FN _0,0
	RCALL SUBOPT_0x1
	RCALL _strcpyf
;      46         Send_DTMF(num);
	RCALL SUBOPT_0x0
	RCALL _Send_DTMF
;      47         while(1);
_0x3:
	RJMP _0x3
;      48 }
_0x6:
	RJMP _0x6
;      49 //**************************************************************************
;      50 // This Module has ported from Atmel applivation note 314 for atmega8
;      51 // I have modifed that app note a bit to remove
;      52 // free runnig osilation between two medulated signal.
;      53 // behzad khazama 2007/06/04 SABZEVAR-IRAN
;      54 // www.khazama.com
;      55 //**************************************************************************
;      56 
;      57 #include <string.h>
;      58 #include <mega8.h>
;      59 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;      60 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;      61 	.EQU __se_bit=0x80
	.EQU __se_bit=0x80
;      62 	.EQU __sm_mask=0x70
	.EQU __sm_mask=0x70
;      63 	.EQU __sm_powerdown=0x20
	.EQU __sm_powerdown=0x20
;      64 	.EQU __sm_powersave=0x30
	.EQU __sm_powersave=0x30
;      65 	.EQU __sm_standby=0x60
	.EQU __sm_standby=0x60
;      66 	.EQU __sm_ext_standby=0x70
	.EQU __sm_ext_standby=0x70
;      67 	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_adc_noise_red=0x10
;      68 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;      69 	#endif
	#endif
;      70 #include <delay.h>
;      71 
;      72 //************************** SIN TABLE *************************************
;      73 // Samples table : one period sampled on 128 samples and
;      74 // quantized on 7 bit
;      75 //**************************************************************************
;      76 flash unsigned char auc_SinParam [128] = {
;      77 64,67,
;      78 70,73,
;      79 76,79,
;      80 82,85,
;      81 88,91,
;      82 94,96,
;      83 99,102,
;      84 104,106,
;      85 109,111,
;      86 113,115,
;      87 117,118,
;      88 120,121,
;      89 123,124,
;      90 125,126,
;      91 126,127,
;      92 127,127,
;      93 127,127,
;      94 127,127,
;      95 126,126,
;      96 125,124,
;      97 123,121,
;      98 120,118,
;      99 117,115,
;     100 113,111,
;     101 109,106,
;     102 104,102,
;     103 99,96,
;     104 94,91,
;     105 88,85,
;     106 82,79,
;     107 76,73,
;     108 70,67,
;     109 64,60,
;     110 57,54,
;     111 51,48,
;     112 45,42,
;     113 39,36,
;     114 33,31,
;     115 28,25,
;     116 23,21,
;     117 18,16,
;     118 14,12,
;     119 10,9,
;     120 7,6,
;     121 4,3,
;     122 2,1,
;     123 1,0,
;     124 0,0,
;     125 0,0,
;     126 0,0,
;     127 1,1,
;     128 2,3,
;     129 4,6,
;     130 7,9,
;     131 10,12,
;     132 14,16,
;     133 18,21,
;     134 23,25,
;     135 28,31,
;     136 33,36,
;     137 39,42,
;     138 45,48,
;     139 51,54,
;     140 57,60};
;     141 
;     142 //***************************  x_SW  ***************************************
;     143 //Table of x_SW (excess 8): x_SW = ROUND(8*N_samples*f*510/Fck)
;     144 //**************************************************************************
;     145 
;     146 //high frequency (coloun)
;     147 //1209hz  ---> x_SW = 79
;     148 //1336hz  ---> x_SW = 87
;     149 //1477hz  ---> x_SW = 96
;     150 //1633hz  ---> x_SW = 107
;     151 
;     152 const unsigned char auc_frequencyH [4] = {
;     153 107,96,
;     154 87,79};
;     155 
;     156 //low frequency (row)
;     157 //697hz  ---> x_SW = 46
;     158 //770hz  ---> x_SW = 50
;     159 //852hz  ---> x_SW = 56
;     160 //941hz  ---> x_SW = 61
;     161 
;     162 const unsigned char auc_frequencyL [4] = {
;     163 61,56,
;     164 50,46};
;     165 
;     166 
;     167 //**************************  global variables  ****************************
;     168 unsigned char x_SWa = 0x00;               // step width of high frequency
;     169 unsigned char x_SWb = 0x00;               // step width of low frequency
;     170 unsigned int  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
;     171 unsigned int  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
;     172 unsigned int  i_TmpSinValA;               // position freq. A in LUT (actual position)
;     173 unsigned int  i_TmpSinValB;               // position freq. B in LUT (actual position)
;     174 
;     175 void stop_dtmf(void){
_stop_dtmf:
;     176  x_SWb=0;
	CLR  R4
;     177  x_SWb=0;
	CLR  R4
;     178  TCCR1A &= 127;
	IN   R30,0x2F
	ANDI R30,0x7F
	OUT  0x2F,R30
;     179  }
	RET
;     180 
;     181 void start_dtmf(void){
_start_dtmf:
;     182  TCCR1A |= 128 ;
	IN   R30,0x2F
	ORI  R30,0x80
	OUT  0x2F,R30
;     183 }
	RET
;     184 
;     185 //**************************************************************************
;     186 // Timer overflow interrupt service routine
;     187 //**************************************************************************
;     188 interrupt [TIM1_OVF] void timer1_ovf_isr(void)
;     189 {
_timer1_ovf_isr:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R26
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     190   // move Pointer about step width aheaed
;     191   i_CurSinValA += x_SWa;
	MOV  R30,R5
	RCALL SUBOPT_0x2
	__ADDWRR 6,7,30,31
;     192   i_CurSinValB += x_SWb;
	MOV  R30,R4
	RCALL SUBOPT_0x2
	__ADDWRR 8,9,30,31
;     193   // normalize Temp-Pointer
;     194   i_TmpSinValA  =  (char)(((i_CurSinValA+4) >> 3)&(0x007F));
	MOVW R30,R6
	RCALL SUBOPT_0x3
	MOVW R10,R30
;     195   i_TmpSinValB  =  (char)(((i_CurSinValB+4) >> 3)&(0x007F));
	MOVW R30,R8
	RCALL SUBOPT_0x3
	MOVW R12,R30
;     196   // calculate PWM value: high frequency value + 3/4 low frequency value
;     197   OCR1A = (auc_SinParam[i_TmpSinValA] + (auc_SinParam[i_TmpSinValB]-(auc_SinParam[i_TmpSinValB]>>2)));
	MOVW R30,R10
	SUBI R30,LOW(-_auc_SinParam*2)
	SBCI R31,HIGH(-_auc_SinParam*2)
	LPM  R0,Z
	RCALL SUBOPT_0x4
	MOV  R26,R30
	RCALL SUBOPT_0x4
	LSR  R30
	LSR  R30
	RCALL __SWAPB12
	SUB  R30,R26
	ADD  R30,R0
	RCALL SUBOPT_0x2
	OUT  0x2A+1,R31
	OUT  0x2A,R30
;     198 }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R26,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;     199 
;     200 //**************************************************************************
;     201 // Initialization
;     202 //**************************************************************************
;     203 void dtmf_init (void)
;     204 {
_dtmf_init:
;     205     TIMSK  = 0x04;                     // Int T1 Overflow enabled
	LDI  R30,LOW(4)
	OUT  0x39,R30
;     206     TCCR1A=0x81;
	LDI  R30,LOW(129)
	OUT  0x2F,R30
;     207     TCCR1B=0x01;
	LDI  R30,LOW(1)
	OUT  0x2E,R30
;     208     DDRB   = 0x02 ;                    // PORTB.1 (OC1A) as output
	LDI  R30,LOW(2)
	OUT  0x17,R30
;     209     stop_dtmf();
	RCALL _stop_dtmf
;     210     #asm("sei");                       // Interrupts enabled
	sei
;     211 }
	RET
;     212 
;     213 void Send_DTMF (char * number)
;     214 {
_Send_DTMF:
;     215   char ch=0,i=0;
;     216   for(i=0;i<strlen(number);i++){
	RCALL __SAVELOCR2
;	*number -> Y+2
;	ch -> R17
;	i -> R16
	LDI  R16,0
	LDI  R17,0
	LDI  R16,LOW(0)
_0x8:
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	RCALL SUBOPT_0x1
	RCALL _strlen
	MOV  R26,R16
	LDI  R27,0
	SBRC R26,7
	SER  R27
	CP   R26,R30
	CPC  R27,R31
	BRLO PC+2
	RJMP _0x9
;     217     ch=number[i];
	MOV  R30,R16
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	RCALL SUBOPT_0x2
	ADD  R26,R30
	ADC  R27,R31
	LD   R17,X
;     218         if(ch=='1'){
	CPI  R17,49
	BRNE _0xA
;     219                 x_SWb = auc_frequencyL[3];
	RCALL SUBOPT_0x5
;     220                 x_SWa = auc_frequencyH[3];
	RCALL SUBOPT_0x6
	RJMP _0x29
;     221         }
;     222         else if(ch=='2'){
_0xA:
	CPI  R17,50
	BRNE _0xC
;     223                 x_SWb = auc_frequencyL[3];
	RCALL SUBOPT_0x5
;     224                 x_SWa = auc_frequencyH[2];
	RCALL SUBOPT_0x7
	RJMP _0x29
;     225         }
;     226         else if(ch=='3'){
_0xC:
	CPI  R17,51
	BRNE _0xE
;     227                 x_SWb = auc_frequencyL[3];
	RCALL SUBOPT_0x5
;     228                 x_SWa = auc_frequencyH[1];
	RCALL SUBOPT_0x8
	RJMP _0x29
;     229         }
;     230         else if(ch=='4'){
_0xE:
	CPI  R17,52
	BRNE _0x10
;     231                 x_SWb = auc_frequencyL[2];
	RCALL SUBOPT_0x9
;     232                 x_SWa = auc_frequencyH[3];
	RCALL SUBOPT_0x6
	RJMP _0x29
;     233         }
;     234         else if(ch=='5'){
_0x10:
	CPI  R17,53
	BRNE _0x12
;     235                 x_SWb = auc_frequencyL[2];
	RCALL SUBOPT_0x9
;     236                 x_SWa = auc_frequencyH[2];
	RCALL SUBOPT_0x7
	RJMP _0x29
;     237         }
;     238         else if(ch=='6'){
_0x12:
	CPI  R17,54
	BRNE _0x14
;     239                 x_SWb = auc_frequencyL[2];
	RCALL SUBOPT_0x9
;     240                 x_SWa = auc_frequencyH[1];
	RCALL SUBOPT_0x8
	RJMP _0x29
;     241         }
;     242         else if(ch=='7'){
_0x14:
	CPI  R17,55
	BRNE _0x16
;     243                 x_SWb = auc_frequencyL[1];
	RCALL SUBOPT_0xA
;     244                 x_SWa = auc_frequencyH[3];
	RCALL SUBOPT_0x6
	RJMP _0x29
;     245         }
;     246         else if(ch=='8'){
_0x16:
	CPI  R17,56
	BRNE _0x18
;     247                 x_SWb = auc_frequencyL[1];
	RCALL SUBOPT_0xA
;     248                 x_SWa = auc_frequencyH[2];
	RCALL SUBOPT_0x7
	RJMP _0x29
;     249         }
;     250         else if(ch=='9'){
_0x18:
	CPI  R17,57
	BRNE _0x1A
;     251                 x_SWb = auc_frequencyL[1];
	RCALL SUBOPT_0xA
;     252                 x_SWa = auc_frequencyH[1];
	RCALL SUBOPT_0x8
	RJMP _0x29
;     253         }
;     254         else if(ch=='0'){
_0x1A:
	CPI  R17,48
	BRNE _0x1C
;     255                 x_SWb = auc_frequencyL[0];
	RCALL SUBOPT_0xB
;     256                 x_SWa = auc_frequencyH[2];
	RCALL SUBOPT_0x7
	RJMP _0x29
;     257         }
;     258         else if(ch=='*'){
_0x1C:
	CPI  R17,42
	BRNE _0x1E
;     259                 x_SWb = auc_frequencyL[0];
	RCALL SUBOPT_0xB
;     260                 x_SWa = auc_frequencyH[3];
	RCALL SUBOPT_0x6
	RJMP _0x29
;     261         }
;     262         else if(ch=='#'){
_0x1E:
	CPI  R17,35
	BRNE _0x20
;     263                 x_SWb = auc_frequencyL[0];
	RCALL SUBOPT_0xB
;     264                 x_SWa = auc_frequencyH[1];
	RCALL SUBOPT_0x8
	RJMP _0x29
;     265         }
;     266         else if(ch=='A'){
_0x20:
	CPI  R17,65
	BRNE _0x22
;     267                 x_SWb = auc_frequencyL[3];
	__POINTW1FN _auc_frequencyL,3
	RJMP _0x2A
;     268                 x_SWa = auc_frequencyH[0];
;     269         }
;     270         else if(ch=='B'){
_0x22:
	CPI  R17,66
	BRNE _0x24
;     271                 x_SWb = auc_frequencyL[2];
	__POINTW1FN _auc_frequencyL,2
	RJMP _0x2A
;     272                 x_SWa = auc_frequencyH[0];
;     273         }
;     274         else if(ch=='C'){
_0x24:
	CPI  R17,67
	BRNE _0x26
;     275                 x_SWb = auc_frequencyL[1];
	__POINTW1FN _auc_frequencyL,1
	RJMP _0x2A
;     276                 x_SWa = auc_frequencyH[0];
;     277         }
;     278         else if(ch=='D'){
_0x26:
	CPI  R17,68
	BRNE _0x28
;     279                 x_SWb = auc_frequencyL[0];
	LDI  R30,LOW(_auc_frequencyL*2)
	LDI  R31,HIGH(_auc_frequencyL*2)
_0x2A:
	LPM  R4,Z
;     280                 x_SWa = auc_frequencyH[0];
	LDI  R30,LOW(_auc_frequencyH*2)
	LDI  R31,HIGH(_auc_frequencyH*2)
_0x29:
	LPM  R5,Z
;     281         }
;     282 
;     283         start_dtmf();
_0x28:
	RCALL _start_dtmf
;     284         delay_ms(150);
	RCALL SUBOPT_0xC
;     285         stop_dtmf();
	RCALL _stop_dtmf
;     286         delay_ms(150);
	RCALL SUBOPT_0xC
;     287     }; // end of for
	SUBI R16,-1
	RJMP _0x8
_0x9:
;     288  stop_dtmf();
	RCALL _stop_dtmf
;     289 }
	RCALL __LOADLOCR2
	ADIW R28,4
	RET
;     290 


;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x0:
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x1:
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x2:
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x3:
	ADIW R30,4
	RCALL __LSRW3
	ANDI R30,LOW(0x7F)
	ANDI R31,HIGH(0x7F)
	LDI  R31,0
	SBRC R30,7
	SER  R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x4:
	MOVW R30,R12
	SUBI R30,LOW(-_auc_SinParam*2)
	SBCI R31,HIGH(-_auc_SinParam*2)
	LPM  R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x5:
	__POINTW1FN _auc_frequencyL,3
	LPM  R4,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x6:
	__POINTW1FN _auc_frequencyH,3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x7:
	__POINTW1FN _auc_frequencyH,2
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	__POINTW1FN _auc_frequencyH,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x9:
	__POINTW1FN _auc_frequencyL,2
	LPM  R4,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0xA:
	__POINTW1FN _auc_frequencyL,1
	LPM  R4,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0xB:
	LDI  R30,LOW(_auc_frequencyL*2)
	LDI  R31,HIGH(_auc_frequencyL*2)
	LPM  R4,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xC:
	LDI  R30,LOW(150)
	LDI  R31,HIGH(150)
	RCALL SUBOPT_0x1
	RJMP _delay_ms

_strcpyf:
	ld   r30,y+
	ld   r31,y+
	ld   r26,y+
	ld   r27,y+
	movw r24,r26
__strcpyf0:
	lpm  r0,z+
	st   x+,r0
	tst  r0
	brne __strcpyf0
	movw r30,r24
	ret

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

__LSRW3:
	LSR  R31
	ROR  R30
__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
	RET

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
	RET

__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

;END OF CODE MARKER
__END_OF_CODE:
