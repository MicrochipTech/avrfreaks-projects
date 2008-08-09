
;CodeVisionAVR C Compiler V1.25.7 beta 5 Professional
;(C) Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega162
;Program type           : Application
;Clock frequency        : 11,059200 MHz
;Memory model           : Small
;Optimize for           : Speed
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 256 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega162
	#pragma AVRPART MEMORY PROG_FLASH 16384
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 1024
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x100

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
	LDI  R24,LOW(0x400)
	LDI  R25,HIGH(0x400)
	LDI  R26,LOW(0x100)
	LDI  R27,HIGH(0x100)
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
	LDI  R30,LOW(0x4FF)
	OUT  SPL,R30
	LDI  R30,HIGH(0x4FF)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x200)
	LDI  R29,HIGH(0x200)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x200
;       1 /*****************************************************
;       2 Project : Final Assignment
;       3 Version :
;       4 Date    : 18/08/2008
;       5 Author  : BaSu-Key
;       6 Company : kulirobot
;       7 Comments:
;       8 Kesimpulan sementara sampai dengan program ini dibuat adalah sebagai berikut:
;       9 1. Baudrate yang digunakan adalah 2400 bps karena keterbatasan hardware IR.
;      10 2. Satu byte data terbaca dalam waktu 5 ms dengan baudrate 2400 bps.
;      11 3. Delay perbyte data yang dikirim minimal adalah 6 ms untuk menghindari kesalahan pembacaan berdasarkan poin nomor 2.
;      12 4. Delay perpaket data yang dikirim adalah 8 ms untuk memberikan kesempatan sisi penerima data untuk memproses data.
;      13 5. PANJANG_PAKET yang pernah dicoba dengan hasil memuaskan adalah 32 byte, 64 byte dan yang terakhir adalah 128 byte.
;      14 6. Penentuan USART_TIMEOUT dihitung dengan menggunakan perhitungan (HEADER+PANJANG_PAKET+CRC)*(delay perpaket+delay perbyte).
;      15 7. Fclk SCK low speed yang digunakan adalah 86,400 kHz khusus pada devais baru atau devais yang memiliki clock rendah.
;      16 8. Fclk SCK high speed yang digunakan adalah 691,200 kHz. Uji coba fclk high speed dengan ATmega8535L dan diperoleh fakta
;      17    bahwa jika fclk yang digunakan adalah 2764,800 kHz maka terjadi kesalahan pada penulisan devais.
;      18 9. Operasi SPI-ISP yang telah berhasil dilakukan adalah sebagai berikut:
;      19     -->> Enter Programming Mode                     <OK> <IR>
;      20     -->> Read Devais Signature                      <OK> <IR>
;      21     -->> Read Fuse Low Byte                         <OK> <IR>
;      22     -->> Read Fuse High Byte                        <OK> <IR>
;      23     -->> Write Fuse Low Byte                        <OK> <IR>
;      24     -->> Write Fuse High Byte                       <OK> <IR>
;      25     -->> Erasing Chip                               <OK> <IR>
;      26     -->> Writting Flash Program Memory Page Access  <OK> <IR>
;      27     -->> Writting Flash Program Memory Byte Access  <NO>
;      28     -->> Polling Flash Program Memory Byte Access   <OK> <IR>
;      29     -->> Verifikasi Flash (Read Flash)              <OK> <IR>
;      30     -->> Writting EEPROM Data Memory Byte Access    <OK> <IR>
;      31     -->> Writting EEPROM Data Memory Page Access    <NO>
;      32     -->> Polling EEPROM Program Memory Byte Access  <OK> <IR>
;      33     -->> Verifikasi EEPROM (Read EEPROM)            <OK> <IR>
;      34     -->> Autoincrement Addressing Page Mode         <OK> <IR>
;      35     -->> Leave Programming Mode                     <OK> <IR>
;      36 
;      37 Chip type           : ATmega162
;      38 Program type        : Application
;      39 Clock frequency     : 11,059200 MHz
;      40 Memory model        : Small
;      41 External SRAM size  : 0
;      42 Data Stack size     : 512
;      43 *****************************************************/
;      44 
;      45 #include <mega162.h>
;      46 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;      47 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;      48 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;      49 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;      50 	#endif
	#endif
;      51 #include <delay.h>
;      52 #include "crc/crc.h"
;      53 #include "debug/debug.h"
;      54 #include "eeprom/eeprom.h"
;      55 #include "extsram/extsram.h"
;      56 #include "flash/flash.h"
;      57 #include "fuse/fuse.h"
;      58 #include "isp/isp.h"
;      59 #include "timer/timer.h"
;      60 #include "usart/usart.h"
;      61 
;      62 #define TDK_PERLU_KONFIRMASI	0
;      63 #define KONFIRMASI_OK			1
;      64 #define KONFIRMASI_GAGAL		2
;      65 #define KONFIRMASI_PAKET_OK		3
;      66 #define KONFIRMASI_IDLE			4
;      67 #define AKHIR_PROSES            5
;      68 
;      69 #define TULIS_FLASH_MEMORY		0x46  //'F'
;      70 #define BACA_FLASH_MEMORY		0x66  //'f'
;      71 #define TULIS_EEPROM     		0x45  //'E'
;      72 #define BACA_EEPROM     		0x65  //'e'
;      73 #define TULIS_FUSE_BITS    		0x42  //'B'
;      74 #define BACA_FUSE_BITS     		0x62  //'b'
;      75 #define HAPUS_CHIP              0x48  //'H'
;      76 
;      77 #define PANJANG_PAKET           128   //64  //32
;      78 
;      79 //Kapasitas SRAM maksimal yang digunakan pada perangkat
;      80 //sisi target adalah 8 kbyte
;      81 #define FAKTOR_PENGALI          8                     //8 --> 8 KByte
;      82 #define KAPASITAS_SRAM          1024*FAKTOR_PENGALI   //Dalam orde kbyte
;      83 
;      84 #define SEI()                   #asm("sei")
;      85 #define CLI()                   #asm("cli")
;      86 
;      87 //D-Latch
;      88 #define AKTIFKAN_LATCH()        PORTD &= ~(1<<2)
;      89 #define NON_AKTIFKAN_LATCH()    PORTD |= (1<<2)
;      90 
;      91 #define LED_BIRU_ON()           PORTD &= ~(1<<3)
;      92 #define LED_BIRU_OFF()          PORTD |= (1<<3)
;      93 #define LED_MERAH_ON()          PORTE &= ~(1<<0)
;      94 #define LED_MERAH_OFF()         PORTE |= (1<<0)
;      95 
;      96 /******************************************************/
;      97 bit awal_akses_devais;          //bit status untuk awal akses devais MK target
;      98 bit akhir_akses_devais;         //bit status untuk akhir akses devais MK target
;      99 //bit data_ukuran_besar;        //bit status untuk penanda data lebih dari kapasitas SRAM
;     100 bit status_paket_perintah;
;     101 bit status_paket_data;
;     102 bit status_paket_akhir;
;     103 bit status_konfirmasi;
;     104 bit tunggu_konfirmasi;
;     105 bit akhir_transmisi;
;     106 bit status_perintah_tulis;
;     107 bit status_tulis_bertahap;
;     108 bit status_baca_bertahap;
;     109 bit status_data_baru_sram;
;     110 /******************************************************/
;     111 unsigned char buffer_konfirmasi[3];
_buffer_konfirmasi:
	.BYTE 0x3
;     112 unsigned char data_load[PANJANG_PAKET];                  //Menyimpan data yang akan diload ke bufferPaketData
_data_load:
	.BYTE 0x80
;     113 unsigned char fuse_bits[3];                              //Variabel yang berisi fuse bits (lfuse, hfuse, exfuse)
_fuse_bits:
	.BYTE 0x3
;     114 unsigned char ID_paket;
;     115 unsigned char ID_paket_gagal;
;     116 unsigned char ID_paket_OK;
;     117 unsigned char ID_perintah;
;     118 unsigned char indek_konfirmasi;
;     119 unsigned char indek_data;
;     120 unsigned char jml_paket;
;     121 unsigned char devais_signature[3];                       //Variabel yang berisi devais signature
_devais_signature:
	.BYTE 0x3
;     122 unsigned char kode_devais[3];                            //Kode devais dari pemrogram
_kode_devais:
	.BYTE 0x3
;     123 unsigned char status_paket;
;     124 unsigned char instruksi_pemrogram;
;     125 /******************************************************/
;     126 unsigned int alamat_awal;
_alamat_awal:
	.BYTE 0x2
;     127 unsigned int alamat_akhir;                              //Alamat terakhir akses memori flash
_alamat_akhir:
	.BYTE 0x2
;     128 unsigned int CRC;
_CRC:
	.BYTE 0x2
;     129 unsigned int idxBufExtSRAM;                             //Indek untuk akses buffer data pada SRAM
_idxBufExtSRAM:
	.BYTE 0x2
;     130 unsigned int jml_byte_data;                             //Jumlah byte data yang diterima -> deteksi paket sram
_jml_byte_data:
	.BYTE 0x2
;     131 unsigned int jml_page_data;
_jml_page_data:
	.BYTE 0x2
;     132 unsigned int time1;
_time1:
	.BYTE 0x2
;     133 
;     134 /***********************************************/
;     135 //Fungsi-Fungsi Interrupt Terima Data Serial IR
;     136 /***********************************************/
;     137 /************************************************************************************************************/
;     138 // USART0 Receiver buffer
;     139 #define RX_BUFFER_SIZE0 256
;     140 char rx_buffer0[RX_BUFFER_SIZE0];
_rx_buffer0:
	.BYTE 0x100
;     141 unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
_rx_wr_index0:
	.BYTE 0x2
_rx_rd_index0:
	.BYTE 0x2
_rx_counter0:
	.BYTE 0x2
;     142 /************************************************************************************************************/
;     143 // USART0 Receiver interrupt service routine
;     144 interrupt [USART0_RXC] void usart0_rx_isr(void)
;     145 {

	.CSEG
_usart0_rx_isr:
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;     146     unsigned char status,data;
;     147     status=UCSR0A;
	ST   -Y,R17
	ST   -Y,R16
;	status -> R17
;	data -> R16
	IN   R17,11
;     148     data=UDR0;
	IN   R16,12
;     149     if ((status & ((FRAMING_ERROR)|(DATA_OVERRUN)))==0)
	MOV  R30,R17
	ANDI R30,LOW(0x18)
	BRNE _0x3
;     150        {
;     151            rx_buffer0[rx_wr_index0]= data;
	LDS  R30,_rx_wr_index0
	LDS  R31,_rx_wr_index0+1
	SUBI R30,LOW(-_rx_buffer0)
	SBCI R31,HIGH(-_rx_buffer0)
	ST   Z,R16
;     152            if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0= 0;
	LDS  R26,_rx_wr_index0
	LDS  R27,_rx_wr_index0+1
	ADIW R26,1
	STS  _rx_wr_index0,R26
	STS  _rx_wr_index0+1,R27
	CPI  R26,LOW(0x100)
	LDI  R30,HIGH(0x100)
	CPC  R27,R30
	BRNE _0x4
	LDI  R30,0
	STS  _rx_wr_index0,R30
	STS  _rx_wr_index0+1,R30
;     153            if (++rx_counter0 == RX_BUFFER_SIZE0) rx_counter0= 0;
_0x4:
	LDS  R26,_rx_counter0
	LDS  R27,_rx_counter0+1
	ADIW R26,1
	STS  _rx_counter0,R26
	STS  _rx_counter0+1,R27
	CPI  R26,LOW(0x100)
	LDI  R30,HIGH(0x100)
	CPC  R27,R30
	BRNE _0x5
	LDI  R30,0
	STS  _rx_counter0,R30
	STS  _rx_counter0+1,R30
;     154        }
_0x5:
;     155 }
_0x3:
	LD   R16,Y+
	LD   R17,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	RETI
;     156 /************************************************************************************************************/
;     157 // Fungsi Pembacaan Data Serial pada Buffer Interrupt RXC
;     158 unsigned char bacaDataSerialIR(void)
;     159 {
_bacaDataSerialIR:
;     160     unsigned char data;
;     161 
;     162     while (rx_counter0==0);
	ST   -Y,R17
;	data -> R17
_0x6:
	LDS  R30,_rx_counter0
	LDS  R31,_rx_counter0+1
	SBIW R30,0
	BREQ _0x6
;     163     data= rx_buffer0[rx_rd_index0];
	LDS  R30,_rx_rd_index0
	LDS  R31,_rx_rd_index0+1
	SUBI R30,LOW(-_rx_buffer0)
	SBCI R31,HIGH(-_rx_buffer0)
	LD   R17,Z
;     164     if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0= 0;
	LDS  R26,_rx_rd_index0
	LDS  R27,_rx_rd_index0+1
	ADIW R26,1
	STS  _rx_rd_index0,R26
	STS  _rx_rd_index0+1,R27
	CPI  R26,LOW(0x100)
	LDI  R30,HIGH(0x100)
	CPC  R27,R30
	BRNE _0x9
	LDI  R30,0
	STS  _rx_rd_index0,R30
	STS  _rx_rd_index0+1,R30
;     165     CLI();
_0x9:
	cli
;     166     --rx_counter0;
	LDS  R30,_rx_counter0
	LDS  R31,_rx_counter0+1
	SBIW R30,1
	STS  _rx_counter0,R30
	STS  _rx_counter0+1,R31
;     167     SEI();
	sei
;     168     return data;
	MOV  R30,R17
	LD   R17,Y+
	RET
;     169 }
;     170 /************************************************************************************************************/
;     171 /*****************************************************************/
;     172 //Fungsi-Fungsi Protokol Komunikasi untuk Operasi Pengiriman Data
;     173 /*****************************************************************/
;     174 /************************************************************************************************************/
;     175 // Fungsi Load Data Transmisi
;     176 void loadDataBaru(unsigned char indek_paket)
;     177 {
_loadDataBaru:
;     178 	unsigned char i;
;     179 	//unsigned int jml_byte_data_sram;
;     180 	unsigned int indek_data;
;     181 
;     182 	if(status_baca_bertahap)
	CALL __SAVELOCR4
;	indek_paket -> Y+4
;	i -> R17
;	indek_data -> R18,R19
	SBRS R3,2
	RJMP _0xA
;     183 	{
;     184     	// Pengindekan paket (ID Paket) dimulai dari 0
;     185     	indek_paket= indek_paket-1;
	LDD  R30,Y+4
	SUBI R30,LOW(1)
	STD  Y+4,R30
;     186 
;     187     	// Pengambilan data dari SRAM Eksternal sama dengan panjang paket dengan indek mulai 0
;     188     	indek_data= (unsigned int)indek_paket*PANJANG_PAKET;
	LDI  R31,0
	CALL __LSLW3
	CALL __LSLW4
	MOVW R18,R30
;     189 
;     190     	for(i=0;i<PANJANG_PAKET;i++)
	LDI  R17,LOW(0)
_0xC:
	CPI  R17,128
	BRSH _0xD
;     191         {
;     192         	data_load[i]= bacaExtSRAM(indek_data++);     //Ambil data dari SRAM Eksternal
	MOV  R30,R17
	LDI  R31,0
	SUBI R30,LOW(-_data_load)
	SBCI R31,HIGH(-_data_load)
	PUSH R31
	PUSH R30
	MOVW R30,R18
	__ADDWRN 18,19,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	POP  R26
	POP  R27
	ST   X,R30
;     193         	jml_byte_data++;
	LDS  R30,_jml_byte_data
	LDS  R31,_jml_byte_data+1
	ADIW R30,1
	STS  _jml_byte_data,R30
	STS  _jml_byte_data+1,R31
;     194         }
	SUBI R17,-1
	RJMP _0xC
_0xD:
;     195 	}
;     196 	else
	RJMP _0xE
_0xA:
;     197 	{
;     198         // Pengindekan paket (ID Paket) dimulai dari 0
;     199     	indek_paket= indek_paket-1;
	LDD  R30,Y+4
	SUBI R30,LOW(1)
	STD  Y+4,R30
;     200 
;     201     	// Pengambilan data dari SRAM Eksternal sama dengan panjang paket dengan indek mulai 0
;     202     	indek_data= (unsigned int)indek_paket*PANJANG_PAKET;
	LDI  R31,0
	CALL __LSLW3
	CALL __LSLW4
	MOVW R18,R30
;     203 
;     204     	for(i=0;i<PANJANG_PAKET;i++)
	LDI  R17,LOW(0)
_0x10:
	CPI  R17,128
	BRSH _0x11
;     205         	data_load[i]= bacaExtSRAM(indek_data++);     //Ambil data dari SRAM Eksternal
	MOV  R30,R17
	LDI  R31,0
	SUBI R30,LOW(-_data_load)
	SBCI R31,HIGH(-_data_load)
	PUSH R31
	PUSH R30
	MOVW R30,R18
	__ADDWRN 18,19,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	POP  R26
	POP  R27
	ST   X,R30
;     206     }
	SUBI R17,-1
	RJMP _0x10
_0x11:
_0xE:
;     207     if(jml_byte_data>KAPASITAS_SRAM)
	LDS  R26,_jml_byte_data
	LDS  R27,_jml_byte_data+1
	CPI  R26,LOW(0x2001)
	LDI  R30,HIGH(0x2001)
	CPC  R27,R30
	BRLO _0x12
;     208     {
;     209         status_data_baru_sram= 0;
	CLT
	BLD  R3,3
;     210         indek_data= 0;
	__GETWRN 18,19,0
;     211     }
;     212 }
_0x12:
	CALL __LOADLOCR4
	ADIW R28,5
	RET
;     213 /************************************************************************************************************/
;     214 // Fungsi Pengiriman Paket Awal Transmisi
;     215 // ID_perintah sama dengan yang dikirim oleh pemrogram
;     216 void kirimPaketPerintah(void)
;     217 {
_kirimPaketPerintah:
;     218     unsigned char i;
;     219     unsigned char buffer_paket_data[9];
;     220 
;     221     CLI();
	SBIW R28,9
	ST   -Y,R17
;	i -> R17
;	buffer_paket_data -> Y+1
	cli
;     222 
;     223     buffer_paket_data[0]= '<';
	LDI  R30,LOW(60)
	STD  Y+1,R30
;     224     buffer_paket_data[1]= 0x46;       // ID Sisi Target
	LDI  R30,LOW(70)
	STD  Y+2,R30
;     225     buffer_paket_data[2]= '>';
	LDI  R30,LOW(62)
	STD  Y+3,R30
;     226     buffer_paket_data[3]= ID_perintah;
	MOVW R30,R28
	ADIW R30,4
	ST   Z,R6
;     227 
;     228     // Informasi jumlah paket yang dikirimkan
;     229     buffer_paket_data[4]= '|';
	LDI  R30,LOW(124)
	STD  Y+5,R30
;     230     buffer_paket_data[5]= jml_paket;  // Jumlah Paket yang akan dikirimkan
	MOVW R30,R28
	ADIW R30,6
	ST   Z,R11
;     231     buffer_paket_data[6]= '|';
	LDI  R30,LOW(124)
	STD  Y+7,R30
;     232     CRC= hitungCRC16(buffer_paket_data, 7);
	MOVW R30,R28
	ADIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(7)
	ST   -Y,R30
	CALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;     233     buffer_paket_data[7]= (unsigned char) CRC;
	MOVW R30,R28
	ADIW R30,8
	LDS  R26,_CRC
	STD  Z+0,R26
;     234     buffer_paket_data[8]= (unsigned char)(CRC>>8);
	LDS  R30,_CRC+1
	STD  Y+9,R30
;     235 
;     236     //Persiapan pengiriman data
;     237     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     238     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     239     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     240     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     241     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     242 
;     243     for(i= 0; i <=8 ; i++)
	LDI  R17,LOW(0)
_0x14:
	CPI  R17,9
	BRSH _0x15
;     244         kirimDataSerialIR(buffer_paket_data[i]);
	MOV  R30,R17
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,1
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     245 
;     246     SEI();
	SUBI R17,-1
	RJMP _0x14
_0x15:
	sei
;     247 }
	LDD  R17,Y+0
	ADIW R28,10
	RET
;     248 /************************************************************************************************************/
;     249 // Fungsi Pengiriman Paket Data Transmisi
;     250 void kirimPaketData(void)
;     251 {
_kirimPaketData:
;     252     unsigned char i, j;
;     253     unsigned char buffer_paket_data[PANJANG_PAKET+9];      // Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)]
;     254 
;     255     CLI();
	SBIW R28,63
	SBIW R28,63
	SBIW R28,11
	ST   -Y,R17
	ST   -Y,R16
;	i -> R17
;	j -> R16
;	buffer_paket_data -> Y+2
	cli
;     256 
;     257     if(!status_konfirmasi) ID_paket= ID_paket_gagal;
	SBRC R2,5
	RJMP _0x16
	MOV  R5,R4
;     258     else ID_paket++;
	RJMP _0x17
_0x16:
	INC  R5
;     259 
;     260     loadDataBaru(ID_paket);
_0x17:
	ST   -Y,R5
	CALL _loadDataBaru
;     261 
;     262     // Header Paket Data
;     263     buffer_paket_data[0]= '<';
	LDI  R30,LOW(60)
	STD  Y+2,R30
;     264     buffer_paket_data[1]= 0x46;
	LDI  R30,LOW(70)
	STD  Y+3,R30
;     265     buffer_paket_data[2]= '>';
	LDI  R30,LOW(62)
	STD  Y+4,R30
;     266     buffer_paket_data[3]= ID_perintah; // Berisi informasi ID Perintah
	MOVW R30,R28
	ADIW R30,5
	ST   Z,R6
;     267     buffer_paket_data[4]= '|';
	LDI  R30,LOW(124)
	STD  Y+6,R30
;     268     buffer_paket_data[5]= ID_paket;    // Berisi informasi ID Paket atau Indek Paket
	MOVW R30,R28
	ADIW R30,7
	ST   Z,R5
;     269     buffer_paket_data[6]= '|';
	LDI  R30,LOW(124)
	STD  Y+8,R30
;     270 
;     271     // Data yang dikirim
;     272 	for(i=0; i<PANJANG_PAKET; i++)
	LDI  R17,LOW(0)
_0x19:
	CPI  R17,128
	BRSH _0x1A
;     273 	{
;     274 		j= i+7;
	MOV  R30,R17
	SUBI R30,-LOW(7)
	MOV  R16,R30
;     275 		buffer_paket_data[j]= data_load[i];
	MOV  R30,R16
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,2
	ADD  R26,R30
	ADC  R27,R31
	MOV  R30,R17
	LDI  R31,0
	SUBI R30,LOW(-_data_load)
	SBCI R31,HIGH(-_data_load)
	LD   R30,Z
	ST   X,R30
;     276 	};
	SUBI R17,-1
	RJMP _0x19
_0x1A:
;     277 
;     278     CRC= hitungCRC16(buffer_paket_data, (PANJANG_PAKET+7));           //HEADER(7)+DATA(PANJANG_PAKET)
	MOVW R30,R28
	ADIW R30,2
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(135)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;     279     buffer_paket_data[PANJANG_PAKET+7]= (unsigned char)  CRC;         //HEADER(7)+DATA(PANJANG_PAKET)+CRC(low byte)
	MOVW R30,R28
	ADIW R30,2
	SUBI R30,LOW(-135)
	SBCI R31,HIGH(-135)
	LDS  R26,_CRC
	STD  Z+0,R26
;     280     buffer_paket_data[PANJANG_PAKET+8]= (unsigned char) (CRC>>8);     //HEADER(7)+DATA(PANJANG_PAKET)+CRC(high byte)
	MOVW R30,R28
	ADIW R30,2
	SUBI R30,LOW(-136)
	SBCI R31,HIGH(-136)
	MOVW R26,R30
	LDS  R30,_CRC+1
	ANDI R31,HIGH(0x0)
	ST   X,R30
;     281 
;     282     //Persiapan pengiriman data
;     283     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     284     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     285     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     286     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     287     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     288 
;     289     for(i= 0; i <= (PANJANG_PAKET+8) ; i++)
	LDI  R17,LOW(0)
_0x1C:
	CPI  R17,137
	BRSH _0x1D
;     290         kirimDataSerialIR(buffer_paket_data[i]);
	MOV  R30,R17
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,2
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     291 
;     292 	SEI();
	SUBI R17,-1
	RJMP _0x1C
_0x1D:
	sei
;     293 }
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,63
	ADIW R28,63
	ADIW R28,13
	RET
;     294 /************************************************************************************************************/
;     295 // Fungsi Pengiriman Paket Akhir Transmisi
;     296 void kirimPaketAkhir(void)
;     297 {
_kirimPaketAkhir:
;     298 	unsigned char i;
;     299 	unsigned char buffer_paket_data[9];
;     300 
;     301 	CLI();
	SBIW R28,9
	ST   -Y,R17
;	i -> R17
;	buffer_paket_data -> Y+1
	cli
;     302     buffer_paket_data[0]= '<';
	LDI  R30,LOW(60)
	STD  Y+1,R30
;     303     buffer_paket_data[1]= 0x46;
	LDI  R30,LOW(70)
	STD  Y+2,R30
;     304     buffer_paket_data[2]= '>';
	LDI  R30,LOW(62)
	STD  Y+3,R30
;     305 
;     306     // Informasi penanda paket akhir
;     307     buffer_paket_data[3]= '\n';
	LDI  R30,LOW(10)
	STD  Y+4,R30
;     308     buffer_paket_data[4]= '|';
	LDI  R30,LOW(124)
	STD  Y+5,R30
;     309     buffer_paket_data[5]= '\r';
	LDI  R30,LOW(13)
	STD  Y+6,R30
;     310     buffer_paket_data[6]= '|';
	LDI  R30,LOW(124)
	STD  Y+7,R30
;     311     CRC= hitungCRC16(buffer_paket_data, 7);
	MOVW R30,R28
	ADIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(7)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;     312     buffer_paket_data[7]= (unsigned char) CRC;
	MOVW R30,R28
	ADIW R30,8
	LDS  R26,_CRC
	STD  Z+0,R26
;     313     buffer_paket_data[8]= (unsigned char) (CRC>>8);
	LDS  R30,_CRC+1
	STD  Y+9,R30
;     314 
;     315     //Persiapan pengiriman data
;     316     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     317     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     318     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     319     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     320     kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     321 
;     322     for(i= 0; i <=8 ; i++)
	LDI  R17,LOW(0)
_0x1F:
	CPI  R17,9
	BRSH _0x20
;     323         kirimDataSerialIR(buffer_paket_data[i]);
	MOV  R30,R17
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,1
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     324 
;     325 	SEI();
	SUBI R17,-1
	RJMP _0x1F
_0x20:
	sei
;     326 }
	LDD  R17,Y+0
	ADIW R28,10
	RET
;     327 /************************************************************************************************************/
;     328 // Fungsi Cek Konfirmasi Paket Awal dan Akhir
;     329 void olahKonfirmasiPaketPerintahDanAkhir(void)
;     330 {
_olahKonfirmasiPaketPerintahDanAkhir:
;     331 	unsigned char data_konfirmasi= 0;
;     332 
;     333 	while(tunggu_konfirmasi)
	ST   -Y,R17
;	data_konfirmasi -> R17
	LDI  R17,0
_0x21:
	SBRS R2,6
	RJMP _0x23
;     334 	{
;     335 		data_konfirmasi= bacaDataSerialIR();
	CALL _bacaDataSerialIR
	MOV  R17,R30
;     336 		//kirimDataSerial(data_konfirmasi);
;     337 		buffer_konfirmasi[indek_konfirmasi++]= data_konfirmasi;
	MOV  R30,R9
	INC  R9
	LDI  R31,0
	SUBI R30,LOW(-_buffer_konfirmasi)
	SBCI R31,HIGH(-_buffer_konfirmasi)
	ST   Z,R17
;     338 
;     339 		if((indek_konfirmasi == 1 && data_konfirmasi != '|') || (indek_konfirmasi == 3 && data_konfirmasi != '|'))
	LDI  R30,LOW(1)
	CP   R30,R9
	BRNE _0x25
	CPI  R17,124
	BRNE _0x27
_0x25:
	LDI  R30,LOW(3)
	CP   R30,R9
	BRNE _0x28
	CPI  R17,124
	BRNE _0x27
_0x28:
	RJMP _0x24
_0x27:
;     340 	            indek_konfirmasi= 0;
	RJMP _0x16B
;     341 		else if(indek_konfirmasi == 4)
_0x24:
	LDI  R30,LOW(4)
	CP   R30,R9
	BRNE _0x2C
;     342 		{
;     343 			if((buffer_konfirmasi[1] == 0xAA) && (data_konfirmasi == 0xAA))
	__GETB1MN _buffer_konfirmasi,1
	CPI  R30,LOW(0xAA)
	BRNE _0x2E
	CPI  R17,170
	BREQ _0x2F
_0x2E:
	RJMP _0x2D
_0x2F:
;     344 			{
;     345 				status_konfirmasi= 1;
	SET
	BLD  R2,5
;     346 				tunggu_konfirmasi= 0;
	CLT
	BLD  R2,6
;     347 				if(status_paket_perintah)
	SBRS R2,2
	RJMP _0x30
;     348 				{
;     349 				    status_paket_perintah= 0;
	BLD  R2,2
;     350 				    status_paket_data= 1;
	SET
	BLD  R2,3
;     351 				}
;     352 				else if(status_paket_akhir) status_paket_akhir= 0;
	RJMP _0x31
_0x30:
	SBRS R2,4
	RJMP _0x32
	CLT
	BLD  R2,4
;     353 			}
_0x32:
_0x31:
;     354     		else tunggu_konfirmasi= 0;
	RJMP _0x33
_0x2D:
	CLT
	BLD  R2,6
;     355 		    indek_konfirmasi= 0;
_0x33:
_0x16B:
	CLR  R9
;     356 		}
;     357 	};
_0x2C:
	RJMP _0x21
_0x23:
;     358 }
	RJMP _0x16A
;     359 /************************************************************************************************************/
;     360 // Fungsi Cek Konfirmasi Paket Data
;     361 void olahKonfirmasiPaketData(void)
;     362 {
_olahKonfirmasiPaketData:
;     363 	unsigned char data_konfirmasi= 0;
;     364 
;     365 	while(tunggu_konfirmasi)
	ST   -Y,R17
;	data_konfirmasi -> R17
	LDI  R17,0
_0x34:
	SBRS R2,6
	RJMP _0x36
;     366 	{
;     367 		data_konfirmasi= bacaDataSerialIR();
	CALL _bacaDataSerialIR
	MOV  R17,R30
;     368 		//kirimDataSerial(data_konfirmasi);
;     369 		buffer_konfirmasi[indek_konfirmasi++]= data_konfirmasi;
	MOV  R30,R9
	INC  R9
	LDI  R31,0
	SUBI R30,LOW(-_buffer_konfirmasi)
	SBCI R31,HIGH(-_buffer_konfirmasi)
	ST   Z,R17
;     370 
;     371 		if((indek_konfirmasi == 1 && data_konfirmasi != '|') || (indek_konfirmasi == 3 && data_konfirmasi != '|'))
	LDI  R30,LOW(1)
	CP   R30,R9
	BRNE _0x38
	CPI  R17,124
	BRNE _0x3A
_0x38:
	LDI  R30,LOW(3)
	CP   R30,R9
	BRNE _0x3B
	CPI  R17,124
	BRNE _0x3A
_0x3B:
	RJMP _0x37
_0x3A:
;     372 	            indek_konfirmasi= 0;
	RJMP _0x16C
;     373 		else if(indek_konfirmasi == 4)
_0x37:
	LDI  R30,LOW(4)
	CP   R30,R9
	BRNE _0x3F
;     374 		{
;     375 			if((buffer_konfirmasi[1] == 0xAA) && (data_konfirmasi == ID_paket))
	__GETB1MN _buffer_konfirmasi,1
	CPI  R30,LOW(0xAA)
	BRNE _0x41
	CP   R5,R17
	BREQ _0x42
_0x41:
	RJMP _0x40
_0x42:
;     376 			{
;     377 				status_konfirmasi= 1;
	SET
	RJMP _0x16D
;     378 				tunggu_konfirmasi= 0;
;     379 			}
;     380 			else
_0x40:
;     381 			{
;     382 				if(buffer_konfirmasi[1] == 0x55)
	__GETB1MN _buffer_konfirmasi,1
	CPI  R30,LOW(0x55)
	BRNE _0x44
;     383 				{
;     384 					status_konfirmasi= 0;
	CLT
	BLD  R2,5
;     385 					tunggu_konfirmasi= 0;
	BLD  R2,6
;     386 
;     387 					ID_paket_gagal= data_konfirmasi;
	MOV  R4,R17
;     388 				}
;     389 				else
	RJMP _0x45
_0x44:
;     390 				{
;     391 					status_konfirmasi= 0;
	CLT
_0x16D:
	BLD  R2,5
;     392 					tunggu_konfirmasi= 0;
	CLT
	BLD  R2,6
;     393 				}
_0x45:
;     394 			}
;     395 			indek_konfirmasi= 0;
_0x16C:
	CLR  R9
;     396 		}
;     397 	};
_0x3F:
	RJMP _0x34
_0x36:
;     398 }
_0x16A:
	LD   R17,Y+
	RET
;     399 /************************************************************************************************************/
;     400 /******************************************************************/
;     401 //Fungsi-Fungsi Protokol Komunikasi untuk Operasi Penerimaan Data
;     402 /******************************************************************/
;     403 // Fungsi Pengiriman Kondisi IDLE
;     404 void kirimKonfirmasiIDLE(void)
;     405 {
_kirimKonfirmasiIDLE:
;     406 	MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     407 	MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     408 	CLI();
	cli
;     409 
;     410 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     411 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     412 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     413 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     414 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     415 
;     416 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     417 	kirimDataSerialIR('\n');
	LDI  R30,LOW(10)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     418 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     419 	kirimDataSerialIR('\r');
	LDI  R30,LOW(13)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     420 
;     421 	SEI();
	sei
;     422 	AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     423 	AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     424 }
	RET
;     425 /************************************************************************************************************/
;     426 // Fungsi Pengiriman Konfirmasi Paket OK
;     427 void kirimKonfirmasiPaketOK(void)
;     428 {
_kirimKonfirmasiPaketOK:
;     429 	MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     430 	MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     431 	CLI();
	cli
;     432 
;     433 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     434 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     435 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     436 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     437 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     438 
;     439 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     440 	kirimDataSerialIR(0xAA);
	LDI  R30,LOW(170)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     441 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     442 	//Berisi informasi ID Paket yang Berhasil
;     443 	kirimDataSerialIR(ID_paket_OK);
	ST   -Y,R7
	CALL _kirimDataSerialIR
;     444 
;     445 	SEI();
	sei
;     446 	AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     447 	AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     448 }
	RET
;     449 /************************************************************************************************************/
;     450 // Fungsi Pengiriman Konfirmasi OK
;     451 void kirimKonfirmasiOK(void)
;     452 {
_kirimKonfirmasiOK:
;     453 	MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     454 	MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     455 	CLI();
	cli
;     456 
;     457 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     458 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     459 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     460 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     461 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     462 
;     463 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     464 	kirimDataSerialIR(0xAA);
	LDI  R30,LOW(170)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     465 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     466 	kirimDataSerialIR(0xAA);
	LDI  R30,LOW(170)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     467 
;     468 	SEI();
	sei
;     469 	AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     470 	AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     471 }
	RET
;     472 /************************************************************************************************************/
;     473 // Fungsi Pengiriman Konfirmasi Paket Gagal
;     474 void kirimKonfirmasiGAGAL(void)
;     475 {
_kirimKonfirmasiGAGAL:
;     476 	MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     477 	MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     478 	CLI();
	cli
;     479 
;     480 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     481 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     482 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     483 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     484 	kirimDataSerialIR(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     485 
;     486 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     487 	kirimDataSerialIR(0x55);
	LDI  R30,LOW(85)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     488 	kirimDataSerialIR('|');
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _kirimDataSerialIR
;     489 	//Berisi informasi ID Paket yang Gagal
;     490 	kirimDataSerialIR(ID_paket);
	ST   -Y,R5
	CALL _kirimDataSerialIR
;     491 
;     492 	SEI();
	sei
;     493 	AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     494 	AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     495 }
	RET
;     496 /************************************************************************************************************/
;     497 // Timer 1 output compare A interrupt service routine
;     498 // Interrupt setiap 20 ms
;     499 interrupt [TIM1_COMPA] void timer1_compa_isr(void)
;     500 {
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
;     501       TCNT1L= 0x00;
	LDI  R30,LOW(0)
	OUT  0x2C,R30
;     502       TCNT1H= 0x00;
	OUT  0x2D,R30
;     503 
;     504       if(time1 > 0)
	LDS  R26,_time1
	LDS  R27,_time1+1
	CALL __CPW02
	BRSH _0x46
;     505       --time1;
	LDS  R30,_time1
	LDS  R31,_time1+1
	SBIW R30,1
	STS  _time1,R30
	STS  _time1+1,R31
;     506 
;     507       if(time1 == 0)
_0x46:
	LDS  R30,_time1
	LDS  R31,_time1+1
	SBIW R30,0
	BRNE _0x47
;     508       {
;     509             time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;     510 
;     511             if(status_paket == KONFIRMASI_GAGAL)
	LDI  R30,LOW(2)
	CP   R30,R10
	BRNE _0x48
;     512             {
;     513                   //Reset indek data pada buffer data sementara
;     514                   indek_data= 0;
	CLR  R8
;     515                   kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;     516             }
;     517             else if(status_paket == KONFIRMASI_OK)
	RJMP _0x49
_0x48:
	LDI  R30,LOW(1)
	CP   R30,R10
	BRNE _0x4A
;     518                   kirimKonfirmasiOK();
	CALL _kirimKonfirmasiOK
;     519             else if(status_paket == KONFIRMASI_PAKET_OK)
	RJMP _0x4B
_0x4A:
	LDI  R30,LOW(3)
	CP   R30,R10
	BRNE _0x4C
;     520                   kirimKonfirmasiPaketOK();
	CALL _kirimKonfirmasiPaketOK
;     521             else if(status_paket == AKHIR_PROSES)
	RJMP _0x4D
_0x4C:
	LDI  R30,LOW(5)
	CP   R30,R10
	BRNE _0x4E
;     522                   kirimKonfirmasiIDLE();
	CALL _kirimKonfirmasiIDLE
;     523       }
_0x4E:
_0x4D:
_0x4B:
_0x49:
;     524 }
_0x47:
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
;     525 /************************************************************************************************************/
;     526 //Rutin inisialisasi hardware dan variabel
;     527 void inisialisasi(void)
;     528 {
_inisialisasi:
;     529     unsigned char eksistensi_devais;
;     530 
;     531     //ID_paket mulai dari 1 sampai dengan jumlah paket maksimal
;     532     //Diincrement pada saat pemanggilan fungsi load data
;     533     ID_paket= 0;
	ST   -Y,R17
;	eksistensi_devais -> R17
	CLR  R5
;     534     ID_paket_gagal= 1;
	LDI  R30,LOW(1)
	MOV  R4,R30
;     535 
;     536     //Jumlah paket data maksimal
;     537     jml_paket= 0;
	CLR  R11
;     538 
;     539     //Indek buffer konfirmasi
;     540     indek_konfirmasi= 0;
	CLR  R9
;     541     ID_paket_OK= 0;
	CLR  R7
;     542 
;     543     //Status konfirmasi
;     544     //OK= 1, GAGAL= 0
;     545     status_konfirmasi= 0;
	CLT
	BLD  R2,5
;     546     tunggu_konfirmasi= 0;
	BLD  R2,6
;     547 
;     548     //Status Paket Data Transmisi
;     549     status_paket_perintah= 1;
	SET
	BLD  R2,2
;     550     status_paket_data= 0;
	CLT
	BLD  R2,3
;     551     status_paket_akhir= 0;
	BLD  R2,4
;     552 
;     553     //Untuk timer scheduling
;     554     time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;     555 
;     556     //Status paket data yang diterima
;     557     status_paket= TDK_PERLU_KONFIRMASI;
	CLR  R10
;     558 
;     559     //Indek untuk pengalamatan SRAM eksternal
;     560     idxBufExtSRAM= 0;
	LDI  R30,0
	STS  _idxBufExtSRAM,R30
	STS  _idxBufExtSRAM+1,R30
;     561 
;     562     //IndekData untuk pengindekan buffer_paket_data
;     563     indek_data= 0;
	CLR  R8
;     564     akhir_transmisi= 0;
	BLD  R2,7
;     565 
;     566     //Set bit status penulisan data
;     567     status_tulis_bertahap= 0;
	BLD  R3,1
;     568 
;     569     //Set alamat akhir memori devais yang diakses
;     570     alamat_awal= 0;
	LDI  R30,0
	STS  _alamat_awal,R30
	STS  _alamat_awal+1,R30
;     571     alamat_akhir= 0;
	LDI  R30,0
	STS  _alamat_akhir,R30
	STS  _alamat_akhir+1,R30
;     572 
;     573     //Set kondisi akses devais
;     574     awal_akses_devais= 1;
	SET
	BLD  R2,0
;     575     akhir_akses_devais= 0;
	CLT
	BLD  R2,1
;     576 
;     577     //Set jumlah data byte yang diterima
;     578     jml_byte_data= 0;
	LDI  R30,0
	STS  _jml_byte_data,R30
	STS  _jml_byte_data+1,R30
;     579 
;     580     //Set LED indikator;
;     581     LED_MERAH_OFF();
	SBI  0x7,0
;     582     LED_BIRU_OFF();
	SBI  0x12,3
;     583 
;     584     //Cek eksistensi devais yang terpasang pada sisi target
;     585     eksistensi_devais= cekEksistensiDevais();
	CALL _cekEksistensiDevais
	MOV  R17,R30
;     586     if(eksistensi_devais) LED_BIRU_ON();
	CPI  R17,0
	BREQ _0x4F
	CBI  0x12,3
;     587 
;     588     //Baca Devais Signature untuk dibandingkan dengan kode devais dari pemrogram
;     589     ISPReadDevaisSignature();
_0x4F:
	CALL _ISPReadDevaisSignature
;     590 
;     591     //Aktifkan Global Interrupt Enable
;     592     SEI();
	sei
;     593 }
	RJMP _0x169
;     594 /************************************************************************************************************/
;     595 // Fungsi pengiriman data ke pemrogram
;     596 void kirimDataKePemrogram(void)
;     597 {
_kirimDataKePemrogram:
;     598     MATIKAN_SCHEDULLING();
	LDI  R30,LOW(0)
	OUT  0x2E,R30
;     599     MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     600     AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     601 
;     602     //Inisialisasi ID Paket mulai nol
;     603     //ID Paket yang dikirim mulai dari satu
;     604     ID_paket= 0;
	CLR  R5
;     605 
;     606     //Status Paket Data Transmisi
;     607     status_paket_perintah= 1;
	SET
	BLD  R2,2
;     608     status_paket_data= 0;
	CLT
	BLD  R2,3
;     609     status_paket_akhir= 0;
	BLD  R2,4
;     610 
;     611     //Status konfirmasi
;     612     //OK= 1, GAGAL= 0
;     613     status_konfirmasi= 0;
	BLD  R2,5
;     614     tunggu_konfirmasi= 0;
	BLD  R2,6
;     615 
;     616     //Pengiriman Paket Perintah
;     617     while(status_paket_perintah==1)
_0x50:
	SBRS R2,2
	RJMP _0x52
;     618     {
;     619         kirimPaketPerintah();
	CALL _kirimPaketPerintah
;     620         tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     621         AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     622         AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     623         olahKonfirmasiPaketPerintahDanAkhir();
	CALL _olahKonfirmasiPaketPerintahDanAkhir
;     624         MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     625         MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     626     };
	RJMP _0x50
_0x52:
;     627     //Pengiriman Paket Data
;     628     while(status_paket_data==1)
_0x53:
	SBRS R2,3
	RJMP _0x55
;     629     {
;     630         //debugging program
;     631         kirimDataSerial(ID_paket);  //Debug log serial USB
	ST   -Y,R5
	CALL _kirimDataSerial
;     632         kirimPaketData();
	CALL _kirimPaketData
;     633         tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     634         AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     635         AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     636         olahKonfirmasiPaketData();
	CALL _olahKonfirmasiPaketData
;     637         MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     638         MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     639         if(ID_paket==jml_paket)
	CP   R11,R5
	BRNE _0x56
;     640         {
;     641             status_paket_data= 0;
	CLT
	BLD  R2,3
;     642             status_paket_akhir= 1;
	SET
	BLD  R2,4
;     643         }
;     644     };
_0x56:
	RJMP _0x53
_0x55:
;     645 
;     646     //Reset indek buffer interrupt RXC
;     647     rx_wr_index0= 0;
	LDI  R30,0
	STS  _rx_wr_index0,R30
	STS  _rx_wr_index0+1,R30
;     648     rx_rd_index0= 0;
	LDI  R30,0
	STS  _rx_rd_index0,R30
	STS  _rx_rd_index0+1,R30
;     649     rx_counter0= 0;
	LDI  R30,0
	STS  _rx_counter0,R30
	STS  _rx_counter0+1,R30
;     650 
;     651     //Pengiriman Paket Akhir
;     652     while(status_paket_akhir==1)
_0x57:
	SBRS R2,4
	RJMP _0x59
;     653     {
;     654         kirimPaketAkhir();
	CALL _kirimPaketAkhir
;     655         tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     656         AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     657         AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     658         olahKonfirmasiPaketPerintahDanAkhir();
	CALL _olahKonfirmasiPaketPerintahDanAkhir
;     659         MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     660         MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     661     };
	RJMP _0x57
_0x59:
;     662 
;     663     AKTIFKAN_SCHEDULLING();
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     664 }
	RET
;     665 /************************************************************************************************************/
;     666 // Fungsi pengiriman data berukuran besar ke pemrogram
;     667 void kirimDataFlashBesarKePemrogram(void)
;     668 {
_kirimDataFlashBesarKePemrogram:
;     669     unsigned char counter= 1;
;     670 
;     671     MATIKAN_SCHEDULLING();
	ST   -Y,R17
;	counter -> R17
	LDI  R17,1
	LDI  R30,LOW(0)
	OUT  0x2E,R30
;     672     MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     673     AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     674 
;     675     //Inisialisasi ID Paket mulai nol
;     676     //ID Paket yang dikirim mulai dari satu
;     677     ID_paket= 0;
	CLR  R5
;     678 
;     679     //Status Paket Data Transmisi
;     680     status_paket_perintah= 1;
	SET
	BLD  R2,2
;     681     status_paket_data= 0;
	CLT
	BLD  R2,3
;     682     status_paket_akhir= 0;
	BLD  R2,4
;     683 
;     684     //Status konfirmasi
;     685     //OK= 1, GAGAL= 0
;     686     status_konfirmasi= 0;
	BLD  R2,5
;     687     tunggu_konfirmasi= 0;
	BLD  R2,6
;     688 
;     689     status_baca_bertahap= 1;
	SET
	BLD  R3,2
;     690     alamat_awal= 0;
	LDI  R30,0
	STS  _alamat_awal,R30
	STS  _alamat_awal+1,R30
;     691 
;     692     //Pengiriman Paket Perintah
;     693     while(status_paket_perintah==1)
_0x5A:
	SBRS R2,2
	RJMP _0x5C
;     694     {
;     695         kirimPaketPerintah();
	CALL _kirimPaketPerintah
;     696         tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     697         AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     698         AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     699         olahKonfirmasiPaketPerintahDanAkhir();
	CALL _olahKonfirmasiPaketPerintahDanAkhir
;     700         MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     701         MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     702     };
	RJMP _0x5A
_0x5C:
;     703     //Pengiriman Paket Data
;     704     while(status_paket_data==1)
_0x5D:
	SBRS R2,3
	RJMP _0x5F
;     705     {
;     706         if(status_baca_bertahap)
	SBRS R3,2
	RJMP _0x60
;     707         {
;     708             LED_BIRU_ON();
	CBI  0x12,3
;     709             alamat_akhir= ISPReadHugeFlashMemory(KAPASITAS_SRAM, alamat_awal);
	LDI  R30,LOW(8192)
	LDI  R31,HIGH(8192)
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_alamat_awal
	LDS  R31,_alamat_awal+1
	ST   -Y,R31
	ST   -Y,R30
	CALL _ISPReadHugeFlashMemory
	STS  _alamat_akhir,R30
	STS  _alamat_akhir+1,R31
;     710             alamat_awal= alamat_akhir;
	STS  _alamat_awal,R30
	STS  _alamat_awal+1,R31
;     711             status_data_baru_sram= 1;
	SET
	BLD  R3,3
;     712             LED_BIRU_OFF();
	SBI  0x12,3
;     713         }
;     714         while(status_data_baru_sram)
_0x60:
_0x61:
	SBRS R3,3
	RJMP _0x63
;     715         {
;     716             //debugging program
;     717             //kirimDataSerial(counter++);  //Debug log serial USB
;     718             kirimPaketData();
	CALL _kirimPaketData
;     719             tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     720             AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     721             AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     722             olahKonfirmasiPaketData();
	CALL _olahKonfirmasiPaketData
;     723             MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     724             MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     725             if(ID_paket==jml_paket+1)
	MOV  R30,R11
	SUBI R30,-LOW(1)
	CP   R30,R5
	BRNE _0x64
;     726             {
;     727                 status_baca_bertahap= 0;
	CLT
	BLD  R3,2
;     728                 status_paket_data= 0;
	BLD  R2,3
;     729                 status_paket_akhir= 1;
	SET
	BLD  R2,4
;     730                 status_data_baru_sram= 0;
	CLT
	BLD  R3,3
;     731             }
;     732         };
_0x64:
	RJMP _0x61
_0x63:
;     733     };
	RJMP _0x5D
_0x5F:
;     734 
;     735     //Reset indek buffer interrupt RXC
;     736     rx_wr_index0= 0;
	LDI  R30,0
	STS  _rx_wr_index0,R30
	STS  _rx_wr_index0+1,R30
;     737     rx_rd_index0= 0;
	LDI  R30,0
	STS  _rx_rd_index0,R30
	STS  _rx_rd_index0+1,R30
;     738     rx_counter0= 0;
	LDI  R30,0
	STS  _rx_counter0,R30
	STS  _rx_counter0+1,R30
;     739 
;     740     //Pengiriman Paket Akhir
;     741     while(status_paket_akhir==1)
_0x65:
	SBRS R2,4
	RJMP _0x67
;     742     {
;     743         kirimPaketAkhir();
	CALL _kirimPaketAkhir
;     744         tunggu_konfirmasi= 1;
	SET
	BLD  R2,6
;     745         AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;     746         AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;     747         olahKonfirmasiPaketPerintahDanAkhir();
	CALL _olahKonfirmasiPaketPerintahDanAkhir
;     748         MATIKAN_RECEIVER_INTERRUPT_IR();
	CBI  0xA,7
;     749         MATIKAN_RECEIVER_IR();
	CBI  0xA,4
;     750     };
	RJMP _0x65
_0x67:
;     751 
;     752     AKTIFKAN_SCHEDULLING();
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     753 }
_0x169:
	LD   R17,Y+
	RET
;     754 /************************************************************************************************************/
;     755 // Fungsi Pembacaan Ukuran Flash untuk mengetahui ukuran flash dan EEPROM maksimal
;     756 unsigned int bacaUkuranFlash(void)
;     757 {
_bacaUkuranFlash:
;     758     // Ukuran memori flash berdasarkan devais signature
;     759     // 0x90 -->  1024 byte
;     760     // 0x91 -->  2048 byte
;     761     // 0x92 -->  4096 byte
;     762     // 0x93 -->  8192 byte
;     763     // 0x94 --> 16384 byte
;     764     // 0x95 --> 32768 byte
;     765     // 0x96 --> 65536 byte
;     766 
;     767     unsigned char kode_devais;
;     768     unsigned int ukuran_flash;
;     769 
;     770     kode_devais= devais_signature[1];
	CALL __SAVELOCR4
;	kode_devais -> R17
;	ukuran_flash -> R18,R19
	__GETBRMN 17,_devais_signature,1
;     771     CLI();
	cli
;     772     switch(kode_devais)
	MOV  R30,R17
;     773     {
;     774         case 0x90:
	CPI  R30,LOW(0x90)
	BRNE _0x6B
;     775              ukuran_flash= 1024;
	__GETWRN 18,19,1024
;     776              break;
	RJMP _0x6A
;     777         case 0x91:
_0x6B:
	CPI  R30,LOW(0x91)
	BRNE _0x6C
;     778              ukuran_flash= 2048;
	__GETWRN 18,19,2048
;     779              break;
	RJMP _0x6A
;     780         case 0x92:
_0x6C:
	CPI  R30,LOW(0x92)
	BRNE _0x6D
;     781              ukuran_flash= 4096;
	__GETWRN 18,19,4096
;     782              break;
	RJMP _0x6A
;     783         case 0x93:
_0x6D:
	CPI  R30,LOW(0x93)
	BRNE _0x6E
;     784              ukuran_flash= 8192;
	__GETWRN 18,19,8192
;     785              break;
	RJMP _0x6A
;     786         case 0x94:
_0x6E:
	CPI  R30,LOW(0x94)
	BRNE _0x6F
;     787              ukuran_flash= 16384;
	__GETWRN 18,19,16384
;     788              break;
	RJMP _0x6A
;     789         case 0x95:
_0x6F:
	CPI  R30,LOW(0x95)
	BRNE _0x70
;     790              ukuran_flash= 32768;
	__GETWRN 18,19,32768
;     791              break;
	RJMP _0x6A
;     792         case 0x96:
_0x70:
	CPI  R30,LOW(0x96)
	BRNE _0x72
;     793              ukuran_flash= 65536;
	__GETWRN 18,19,65536
;     794              break;
	RJMP _0x6A
;     795         default : ukuran_flash= 0;
_0x72:
	__GETWRN 18,19,0
;     796     }
_0x6A:
;     797     return ukuran_flash;
	MOVW R30,R18
	CALL __LOADLOCR4
	ADIW R28,4
	RET
;     798 }
;     799 /************************************************************************************************************/
;     800 // Fungsi Tulis Flash, data dari kiriman pemrogram
;     801 void tulisMemoriFlash(void)
;     802 {
_tulisMemoriFlash:
;     803     unsigned int jml_page_data;
;     804     jml_page_data= 0;
	ST   -Y,R17
	ST   -Y,R16
;	jml_page_data -> R16,R17
	__GETWRN 16,17,0
;     805 
;     806     CLI();
	cli
;     807     jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK
	LDS  R30,_idxBufExtSRAM
	LDS  R31,_idxBufExtSRAM+1
	LSR  R31
	ROR  R30
	MOVW R16,R30
;     808     LED_BIRU_ON();
	CBI  0x12,3
;     809     enterProgrammingMode();
	CALL _enterProgrammingMode
;     810     chipErase();
	CALL _chipErase
;     811     ISPWriteFlashMemory(jml_page_data);
	ST   -Y,R17
	ST   -Y,R16
	CALL _ISPWriteFlashMemory
;     812     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     813     LED_BIRU_OFF();
	SBI  0x12,3
;     814     SEI();
	sei
;     815 }
	LD   R16,Y+
	LD   R17,Y+
	RET
;     816 /************************************************************************************************************/
;     817 // Fungsi Tulis EEPROM, data dari kiriman pemrogram
;     818 void tulisEEPROM(void)
;     819 {
_tulisEEPROM:
;     820     unsigned int jml_byte_data;
;     821     jml_byte_data= 0;
	ST   -Y,R17
	ST   -Y,R16
;	jml_byte_data -> R16,R17
	__GETWRN 16,17,0
;     822 
;     823     CLI();
	cli
;     824     jml_byte_data= idxBufExtSRAM;
	__GETWRMN 16,17,0,_idxBufExtSRAM
;     825     LED_BIRU_ON();
	CBI  0x12,3
;     826     enterProgrammingMode();
	CALL _enterProgrammingMode
;     827     ISPWriteEEPROM(jml_byte_data);
	ST   -Y,R17
	ST   -Y,R16
	CALL _ISPWriteEEPROM
;     828     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     829     LED_BIRU_OFF();
	SBI  0x12,3
;     830     SEI();
	sei
;     831 }
	LD   R16,Y+
	LD   R17,Y+
	RET
;     832 /************************************************************************************************************/
;     833 // Fungsi Baca Flash, hasil pembacaan kirim ke pemrogram
;     834 void bacaMemoriFlash(void)
;     835 {
_bacaMemoriFlash:
;     836     unsigned int flash_page_max;
;     837     unsigned int ukuran_flash_max;
;     838 
;     839     CLI();
	CALL __SAVELOCR4
;	flash_page_max -> R16,R17
;	ukuran_flash_max -> R18,R19
	cli
;     840     //Baca flash memory kemudian data disimpan dalam SRAM Eksternal
;     841     ukuran_flash_max= bacaUkuranFlash();
	CALL _bacaUkuranFlash
	MOVW R18,R30
;     842     flash_page_max= ukuran_flash_max/2; //OK sudah bisa
	MOVW R30,R18
	LSR  R31
	ROR  R30
	MOVW R16,R30
;     843     jml_paket= (unsigned char) (ukuran_flash_max/PANJANG_PAKET);
	MOVW R26,R18
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	CALL __DIVW21U
	MOV  R11,R30
;     844     if(ukuran_flash_max>KAPASITAS_SRAM) status_tulis_bertahap= 1;
	__CPWRN 18,19,8193
	BRLO _0x73
	SET
	BLD  R3,1
;     845     //Pengiriman data pembacaan flash ke sisi pemrogram
;     846     if(status_tulis_bertahap)
_0x73:
	SBRS R3,1
	RJMP _0x74
;     847     {
;     848         //data flash melebihi kapasitas SRAM (ukuran flash besar)
;     849         //debugging program
;     850         kirimDataSerial(0x11); //Debug log serial USB
	LDI  R30,LOW(17)
	ST   -Y,R30
	CALL _kirimDataSerial
;     851         kirimDataFlashBesarKePemrogram();
	CALL _kirimDataFlashBesarKePemrogram
;     852     }
;     853     else
	RJMP _0x75
_0x74:
;     854     {
;     855         //Kapasitas SRAM cukup untuk sekali tampung data flash (ukuran flash kecil)
;     856         LED_BIRU_ON();
	CBI  0x12,3
;     857         enterProgrammingMode();
	CALL _enterProgrammingMode
;     858         ISPReadFlashMemory(flash_page_max);
	ST   -Y,R17
	ST   -Y,R16
	CALL _ISPReadFlashMemory
;     859         leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     860         LED_BIRU_OFF();
	SBI  0x12,3
;     861         //debugging program
;     862         kirimDataSerial(0x22); //Debug log serial USB
	LDI  R30,LOW(34)
	ST   -Y,R30
	CALL _kirimDataSerial
;     863         kirimDataKePemrogram();
	CALL _kirimDataKePemrogram
;     864     }
_0x75:
;     865     SEI();
	sei
;     866 }
	CALL __LOADLOCR4
	ADIW R28,4
	RET
;     867 /************************************************************************************************************/
;     868 // Fungsi Baca EEPROM, hasil pembacaan kirim ke pemrogram
;     869 void bacaEEPROM(void)
;     870 {
_bacaEEPROM:
;     871     unsigned int ukuran_flash_max;
;     872     unsigned int EEPROM_size_max;
;     873 
;     874     CLI();
	CALL __SAVELOCR4
;	ukuran_flash_max -> R16,R17
;	EEPROM_size_max -> R18,R19
	cli
;     875     //Baca EEPROM memory kemudian data disimpan dalam SRAM Eksternal
;     876     ukuran_flash_max= bacaUkuranFlash();
	CALL _bacaUkuranFlash
	MOVW R16,R30
;     877     EEPROM_size_max= (unsigned int)(ukuran_flash_max/16); //OK sudah bisa
	MOVW R30,R16
	CALL __LSRW4
	MOVW R18,R30
;     878     LED_BIRU_ON();
	CBI  0x12,3
;     879     enterProgrammingMode();
	CALL _enterProgrammingMode
;     880     ISPReadEEPROM(EEPROM_size_max);
	ST   -Y,R19
	ST   -Y,R18
	CALL _ISPReadEEPROM
;     881     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     882     LED_BIRU_OFF();
	SBI  0x12,3
;     883     SEI();
	sei
;     884 
;     885     //Pengiriman data pembacaan EEPROM ke sisi pemrogram
;     886     jml_paket= (unsigned int)(EEPROM_size_max/PANJANG_PAKET);
	MOVW R26,R18
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	CALL __DIVW21U
	MOV  R11,R30
;     887     kirimDataKePemrogram();
	CALL _kirimDataKePemrogram
;     888 }
	CALL __LOADLOCR4
	ADIW R28,4
	RET
;     889 /************************************************************************************************************/
;     890 // Fungsi Baca Fuse Bits
;     891 void bacaFuseBits(void)
;     892 {
_bacaFuseBits:
;     893     CLI();
	cli
;     894     LED_BIRU_ON();
	CBI  0x12,3
;     895     /*** Set SCK dengan frekuensi rendah ***/
;     896     inisialisasiSPILowSpeed();
	CALL _inisialisasiSPILowSpeed
;     897     enterProgrammingMode();
	CALL _enterProgrammingMode
;     898     fuse_bits[0]= ISPReadLowFuseBits();
	CALL _ISPReadLowFuseBits
	STS  _fuse_bits,R30
;     899     fuse_bits[1]= ISPReadHighFuseBits();
	CALL _ISPReadHighFuseBits
	__PUTB1MN _fuse_bits,1
;     900     fuse_bits[2]= ISPReadExtendedFuseBits();
	CALL _ISPReadExtendedFuseBits
	__PUTB1MN _fuse_bits,2
;     901     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     902     LED_BIRU_OFF();
	SBI  0x12,3
;     903     SEI();
	sei
;     904 
;     905     // Pengiriman data fuse bits sesuai dengan format protokol dengan pemrogram
;     906     tulisExtSRAM(0, 0xFF);           //ID Fuse Bits (Diabaikan Pemrogram)
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(255)
	ST   -Y,R30
	CALL _tulisExtSRAM
;     907     tulisExtSRAM(1, '|');
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _tulisExtSRAM
;     908     tulisExtSRAM(2, fuse_bits[0]);   //Low Fuse
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_fuse_bits
	ST   -Y,R30
	CALL _tulisExtSRAM
;     909     tulisExtSRAM(3, '|');
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _tulisExtSRAM
;     910     tulisExtSRAM(4, fuse_bits[1]);   //High Fuse
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	ST   -Y,R31
	ST   -Y,R30
	__GETB1MN _fuse_bits,1
	ST   -Y,R30
	CALL _tulisExtSRAM
;     911     tulisExtSRAM(5, '|');
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(124)
	ST   -Y,R30
	CALL _tulisExtSRAM
;     912     tulisExtSRAM(6, fuse_bits[2]);   //Extended Fuse
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	ST   -Y,R31
	ST   -Y,R30
	__GETB1MN _fuse_bits,2
	ST   -Y,R30
	CALL _tulisExtSRAM
;     913 
;     914     //Pengiriman data pembacaan flash ke sisi pemrogram
;     915     jml_paket= 1;
	LDI  R30,LOW(1)
	MOV  R11,R30
;     916     kirimDataKePemrogram();
	CALL _kirimDataKePemrogram
;     917 }
	RET
;     918 /************************************************************************************************************/
;     919 // Fungsi Tulis Fuse Bits
;     920 void tulisFuseBits(void)
;     921 {
_tulisFuseBits:
	PUSH R15
;     922     unsigned char data_fuse[7];
;     923     unsigned char informasi_fuse;
;     924     unsigned char i;
;     925     bit status_data_fuse;
;     926     bit status_low_fuse;
;     927     bit status_high_fuse;
;     928     bit status_extended_fuse;
;     929 
;     930     CLI();
	SBIW R28,7
	ST   -Y,R17
	ST   -Y,R16
;	data_fuse -> Y+2
;	informasi_fuse -> R17
;	i -> R16
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	cli
;     931     status_data_fuse= 0;
	CLT
	BLD  R15,0
;     932     status_low_fuse= 0;
	BLD  R15,1
;     933     status_high_fuse= 0;
	BLD  R15,2
;     934     status_extended_fuse= 0;
	BLD  R15,3
;     935     for(i=0;i<7;i++) data_fuse[i]= bacaExtSRAM(i);
	LDI  R16,LOW(0)
_0x77:
	CPI  R16,7
	BRSH _0x78
	MOV  R30,R16
	LDI  R31,0
	MOVW R0,R30
	MOVW R26,R28
	ADIW R26,2
	ADD  R30,R26
	ADC  R31,R27
	PUSH R31
	PUSH R30
	ST   -Y,R1
	ST   -Y,R0
	CALL _bacaExtSRAM
	POP  R26
	POP  R27
	ST   X,R30
;     936     informasi_fuse= data_fuse[0];
	SUBI R16,-1
	RJMP _0x77
_0x78:
	LDD  R17,Y+2
;     937     switch(informasi_fuse)
	MOV  R30,R17
;     938     {
;     939         case 0x00:
;	data_fuse -> Y+2
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	CPI  R30,0
	BRNE _0x7C
;     940              {
;     941                 status_low_fuse= 1;
;	data_fuse -> Y+2
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	SET
	BLD  R15,1
;     942                 status_data_fuse= 1;
	RJMP _0x16E
;     943              }
;     944              break;
;     945         case 0x01:
_0x7C:
	CPI  R30,LOW(0x1)
	BRNE _0x7D
;     946              {
;     947                 status_low_fuse= 1;
;	data_fuse -> Y+2
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	SET
	BLD  R15,1
;     948                 status_high_fuse= 1;
	BLD  R15,2
;     949                 status_data_fuse= 1;
	RJMP _0x16E
;     950              }
;     951              break;
;     952         case 0x02:
_0x7D:
	CPI  R30,LOW(0x2)
	BRNE _0x7F
;     953              {
;     954                 status_low_fuse= 1;
;	data_fuse -> Y+2
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	SET
	BLD  R15,1
;     955                 status_high_fuse= 1;
	BLD  R15,2
;     956                 status_extended_fuse= 1;
	BLD  R15,3
;     957                 status_data_fuse= 1;
	RJMP _0x16E
;     958              }
;     959              break;
;     960         default : status_data_fuse= 0;
_0x7F:
	CLT
_0x16E:
	BLD  R15,0
;     961     }
;     962 
;     963     if(status_data_fuse)
	SBRS R15,0
	RJMP _0x80
;     964     {
;     965         LED_BIRU_ON();
;	data_fuse -> Y+2
;	status_data_fuse -> R15.0
;	status_low_fuse -> R15.1
;	status_high_fuse -> R15.2
;	status_extended_fuse -> R15.3
	CBI  0x12,3
;     966         /*** Set SCK dengan frekuensi rendah ***/
;     967         inisialisasiSPILowSpeed();
	CALL _inisialisasiSPILowSpeed
;     968         enterProgrammingMode();
	CALL _enterProgrammingMode
;     969         if(status_low_fuse) ISPWriteLowFuseBits(data_fuse[2]);
	SBRS R15,1
	RJMP _0x81
	LDD  R30,Y+4
	ST   -Y,R30
	CALL _ISPWriteLowFuseBits
;     970         if(status_high_fuse) ISPWriteHighFuseBits(data_fuse[4]);
_0x81:
	SBRS R15,2
	RJMP _0x82
	LDD  R30,Y+6
	ST   -Y,R30
	CALL _ISPWriteHighFuseBits
;     971         if(status_extended_fuse) ISPWriteExtendedBits(data_fuse[6]);
_0x82:
	SBRS R15,3
	RJMP _0x83
	LDD  R30,Y+8
	ST   -Y,R30
	CALL _ISPWriteExtendedBits
;     972         leaveProgrammingMode();
_0x83:
	CALL _leaveProgrammingMode
;     973         LED_BIRU_OFF();
	SBI  0x12,3
;     974     }
;     975     SEI();
_0x80:
	sei
;     976 }
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,9
	POP  R15
	RET
;     977 /************************************************************************************************************/
;     978 // Fungsi Hapus Chip
;     979 void hapusMemoriDevais(void)
;     980 {
_hapusMemoriDevais:
;     981     CLI();
	cli
;     982     enterProgrammingMode();
	CALL _enterProgrammingMode
;     983     chipErase();
	CALL _chipErase
;     984     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;     985     SEI();
	sei
;     986 }
	RET
;     987 /************************************************************************************************************/
;     988 // Fungsi Menu ISP (Baca dan Tulis Memori)
;     989 void menuISP(unsigned char menu_pilihan)
;     990 {
_menuISP:
;     991 	status_paket= TDK_PERLU_KONFIRMASI;
;	menu_pilihan -> Y+0
	CLR  R10
;     992 	switch(menu_pilihan)
	LD   R30,Y
;     993 	{
;     994 		case BACA_FLASH_MEMORY:
	CPI  R30,LOW(0x66)
	BRNE _0x87
;     995 			bacaMemoriFlash();
	CALL _bacaMemoriFlash
;     996 			break;
	RJMP _0x86
;     997 		case TULIS_FLASH_MEMORY:
_0x87:
	CPI  R30,LOW(0x46)
	BRNE _0x88
;     998 			tulisMemoriFlash();
	CALL _tulisMemoriFlash
;     999 			break;
	RJMP _0x86
;    1000 		case BACA_EEPROM:
_0x88:
	CPI  R30,LOW(0x65)
	BRNE _0x89
;    1001 			bacaEEPROM();
	CALL _bacaEEPROM
;    1002 			break;
	RJMP _0x86
;    1003 		case TULIS_EEPROM:
_0x89:
	CPI  R30,LOW(0x45)
	BRNE _0x8A
;    1004 			tulisEEPROM();
	CALL _tulisEEPROM
;    1005 			break;
	RJMP _0x86
;    1006 		case BACA_FUSE_BITS:
_0x8A:
	CPI  R30,LOW(0x62)
	BRNE _0x8B
;    1007 		     bacaFuseBits();
	CALL _bacaFuseBits
;    1008 		     break;
	RJMP _0x86
;    1009 		case TULIS_FUSE_BITS:
_0x8B:
	CPI  R30,LOW(0x42)
	BRNE _0x8C
;    1010 		     tulisFuseBits();
	CALL _tulisFuseBits
;    1011 		     break;
	RJMP _0x86
;    1012 		case HAPUS_CHIP:
_0x8C:
	CPI  R30,LOW(0x48)
	BRNE _0x86
;    1013 		     hapusMemoriDevais();
	CALL _hapusMemoriDevais
;    1014 		     break;
;    1015 	}
_0x86:
;    1016 }
	ADIW R28,1
	RET
;    1017 /************************************************************************************************************/
;    1018 void olahPaketPerintah(void)
;    1019 {
_olahPaketPerintah:
	PUSH R15
;    1020     bit selesai_terima_perintah;
;    1021     unsigned char data;
;    1022     unsigned char buffer_paket_data[11];   // Ukuran buffer adalah [HEADER(7) + Data(kode devais/jumlah paket) + CRC(2)]
;    1023 
;    1024     while(!selesai_terima_perintah)
	SBIW R28,11
	ST   -Y,R17
;	selesai_terima_perintah -> R15.0
;	data -> R17
;	buffer_paket_data -> Y+1
_0x8E:
	SBRC R15,0
	RJMP _0x90
;    1025     {
;    1026         data= bacaDataSerialIR();
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	CALL _bacaDataSerialIR
	MOV  R17,R30
;    1027         //kirimDataSerial(data); //Debug log serial USB
;    1028         buffer_paket_data[indek_data++]= data;
	MOV  R30,R8
	INC  R8
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,1
	ADD  R30,R26
	ADC  R31,R27
	ST   Z,R17
;    1029 
;    1030         if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0x92
	CPI  R17,60
	BRNE _0x94
_0x92:
	LDI  R30,LOW(2)
	CP   R30,R8
	BRNE _0x95
	CPI  R17,69
	BRNE _0x94
_0x95:
	LDI  R30,LOW(3)
	CP   R30,R8
	BRNE _0x97
	CPI  R17,62
	BRNE _0x94
_0x97:
	RJMP _0x91
_0x94:
;    1031             indek_data= 0;
	RJMP _0x16F
;    1032         else if(indek_data == 5 && data != '|')
_0x91:
	LDI  R30,LOW(5)
	CP   R30,R8
	BRNE _0x9C
	CPI  R17,124
	BRNE _0x9D
_0x9C:
	RJMP _0x9B
_0x9D:
;    1033         	indek_data= 0;
	RJMP _0x16F
;    1034         else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
_0x9B:
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0x9F
;    1035             time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;    1036         else if(indek_data == 11)  // Deteksi awal kiriman paket
	RJMP _0xA0
_0x9F:
	LDI  R30,LOW(11)
	CP   R30,R8
	BREQ PC+3
	JMP _0xA1
;    1037         {
;    1038               CRC= hitungCRC16(buffer_paket_data, 9);
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	MOVW R30,R28
	ADIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(9)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;    1039               if(((unsigned char) CRC == buffer_paket_data[9]) && ((unsigned char) (CRC>>8) == buffer_paket_data[10]))
	LDD  R30,Y+10
	LDS  R26,_CRC
	CP   R30,R26
	BRNE _0xA3
	LDS  R30,_CRC+1
	ANDI R31,HIGH(0x0)
	MOV  R26,R30
	LDD  R30,Y+11
	CP   R30,R26
	BREQ _0xA4
_0xA3:
	RJMP _0xA2
_0xA4:
;    1040               {
;    1041                   instruksi_pemrogram= buffer_paket_data[3];
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	LDD  R13,Y+4
;    1042                   //Operasi Tulis dan Hapus
;    1043                   if(instruksi_pemrogram==0x46 || instruksi_pemrogram==0x45 || instruksi_pemrogram==0x42 || instruksi_pemrogram==0x48)
	LDI  R30,LOW(70)
	CP   R30,R13
	BREQ _0xA6
	LDI  R30,LOW(69)
	CP   R30,R13
	BREQ _0xA6
	LDI  R30,LOW(66)
	CP   R30,R13
	BREQ _0xA6
	LDI  R30,LOW(72)
	CP   R30,R13
	BRNE _0xA5
_0xA6:
;    1044                   {
;    1045                       LED_BIRU_OFF();
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	SBI  0x12,3
;    1046                       LED_MERAH_ON();
	CBI  0x7,0
;    1047 
;    1048                   	  if(instruksi_pemrogram!=0x48)    // Bukan operasi hapus
	LDI  R30,LOW(72)
	CP   R30,R13
	BREQ _0xA8
;    1049                       {
;    1050                           jml_paket= buffer_paket_data[7];
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	LDD  R11,Y+8
;    1051                           status_paket= KONFIRMASI_OK;
	LDI  R30,LOW(1)
	MOV  R10,R30
;    1052                       	  kirimKonfirmasiOK();
	CALL _kirimKonfirmasiOK
;    1053                           ID_paket= 1;
	LDI  R30,LOW(1)
	MOV  R5,R30
;    1054                           status_perintah_tulis= 1;
	SET
	BLD  R3,0
;    1055                           selesai_terima_perintah= 1;  // Status perlu simpan data yang akan ditulis
	BLD  R15,0
;    1056                           jml_byte_data= (unsigned int) jml_paket*PANJANG_PAKET;
	MOV  R26,R11
	LDI  R30,LOW(128)
	MUL  R30,R26
	MOVW R30,R0
	STS  _jml_byte_data,R30
	STS  _jml_byte_data+1,R31
;    1057                           if(jml_byte_data>KAPASITAS_SRAM) status_tulis_bertahap= 1; //Deteksi data ukuran besar
	LDS  R26,_jml_byte_data
	LDS  R27,_jml_byte_data+1
	CPI  R26,LOW(0x2001)
	LDI  R30,HIGH(0x2001)
	CPC  R27,R30
	BRLO _0xA9
	BLD  R3,1
;    1058                       }
_0xA9:
;    1059                       else                             // Operasi hapus tidak perlu data transmisi lagi
	RJMP _0xAA
_0xA8:
;    1060         			  {
;    1061                           status_paket= KONFIRMASI_OK;
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	LDI  R30,LOW(1)
	MOV  R10,R30
;    1062                           kirimKonfirmasiOK();
	CALL _kirimKonfirmasiOK
;    1063                           selesai_terima_perintah= 1;
	SET
	BLD  R15,0
;    1064                           status_perintah_tulis= 0;
	CLT
	BLD  R3,0
;    1065         			  }
_0xAA:
;    1066     			 }
;    1067 
;    1068     			 // Operasi Baca
;    1069     			 else if (instruksi_pemrogram==0x66 || instruksi_pemrogram==0x65 || instruksi_pemrogram==0x62)
	RJMP _0xAB
_0xA5:
	LDI  R30,LOW(102)
	CP   R30,R13
	BREQ _0xAD
	LDI  R30,LOW(101)
	CP   R30,R13
	BREQ _0xAD
	LDI  R30,LOW(98)
	CP   R30,R13
	BRNE _0xAC
_0xAD:
;    1070     			 {
;    1071                      LED_BIRU_OFF();
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	SBI  0x12,3
;    1072                      LED_MERAH_ON();
	CBI  0x7,0
;    1073 
;    1074                      kode_devais[0]= buffer_paket_data[5];
	LDD  R30,Y+6
	STS  _kode_devais,R30
;    1075                      kode_devais[1]= buffer_paket_data[6];
	LDD  R30,Y+7
	__PUTB1MN _kode_devais,1
;    1076                      kode_devais[2]= buffer_paket_data[7];
	LDD  R30,Y+8
	__PUTB1MN _kode_devais,2
;    1077                      ID_paket= 1;
	LDI  R30,LOW(1)
	MOV  R5,R30
;    1078                      selesai_terima_perintah= 1;
	SET
	BLD  R15,0
;    1079 
;    1080                      if(kode_devais[0]==devais_signature[0] && kode_devais[1]==devais_signature[1] && kode_devais[2]==devais_signature[2])
	LDS  R30,_devais_signature
	LDS  R26,_kode_devais
	CP   R30,R26
	BRNE _0xB0
	__GETB2MN _kode_devais,1
	__GETB1MN _devais_signature,1
	CP   R30,R26
	BRNE _0xB0
	__GETB2MN _kode_devais,2
	__GETB1MN _devais_signature,2
	CP   R30,R26
	BREQ _0xB1
_0xB0:
	RJMP _0xAF
_0xB1:
;    1081                      {
;    1082                          ID_perintah= instruksi_pemrogram;
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	MOV  R6,R13
;    1083                          status_paket= KONFIRMASI_OK;
	LDI  R30,LOW(1)
	MOV  R10,R30
;    1084                          kirimKonfirmasiOK();
	CALL _kirimKonfirmasiOK
;    1085                          status_perintah_tulis= 0;
	CLT
	BLD  R3,0
;    1086                      }
;    1087                      else
	RJMP _0xB2
_0xAF:
;    1088                      {
;    1089                          selesai_terima_perintah= 0;
;	selesai_terima_perintah -> R15.0
;	buffer_paket_data -> Y+1
	CLT
	BLD  R15,0
;    1090                          status_paket= KONFIRMASI_GAGAL;
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1091                          kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1092                          inisialisasi();
	CALL _inisialisasi
;    1093                      }
_0xB2:
;    1094                 }
;    1095 			}
_0xAC:
_0xAB:
;    1096 			indek_data= 0;
_0xA2:
_0x16F:
	CLR  R8
;    1097 		}
;    1098     };
_0xA1:
_0xA0:
	RJMP _0x8E
_0x90:
;    1099 }
	LDD  R17,Y+0
	RJMP _0x168
;    1100 /************************************************************************************************************/
;    1101 void olahPaketData(void)
;    1102 {
_olahPaketData:
	PUSH R15
;    1103     bit selesai_terima_data;
;    1104     unsigned char data;
;    1105     unsigned char i;
;    1106     unsigned char buffer_paket_data[PANJANG_PAKET+9];   //Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)]
;    1107     unsigned int jml_data_page;                         //Jumlah data page yang akan ditulis ke flash
;    1108     unsigned char jml_paket_sram_tertulis;              //Counter paket sram yang tertulis pada flash
;    1109 
;    1110     if(!status_tulis_bertahap) //Data yang di tulis antara 1 sampai dengan 8 kbyte (KAPASITAS_SRAM)
	SBIW R28,63
	SBIW R28,63
	SBIW R28,11
	CALL __SAVELOCR6
;	selesai_terima_data -> R15.0
;	data -> R17
;	i -> R16
;	buffer_paket_data -> Y+6
;	jml_data_page -> R18,R19
;	jml_paket_sram_tertulis -> R21
	SBRC R3,1
	RJMP _0xB3
;    1111     {
;    1112         while(!selesai_terima_data)
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
_0xB4:
	SBRC R15,0
	RJMP _0xB6
;    1113         {
;    1114             data= bacaDataSerialIR();
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	CALL _bacaDataSerialIR
	MOV  R17,R30
;    1115             //kirimDataSerial(data); //Debug log serial USB
;    1116             buffer_paket_data[indek_data++]= data;
	MOV  R30,R8
	INC  R8
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,6
	ADD  R30,R26
	ADC  R31,R27
	ST   Z,R17
;    1117 
;    1118             if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0xB8
	CPI  R17,60
	BRNE _0xBA
_0xB8:
	LDI  R30,LOW(2)
	CP   R30,R8
	BRNE _0xBB
	CPI  R17,69
	BRNE _0xBA
_0xBB:
	LDI  R30,LOW(3)
	CP   R30,R8
	BRNE _0xBD
	CPI  R17,62
	BRNE _0xBA
_0xBD:
	RJMP _0xB7
_0xBA:
;    1119                 indek_data= 0;
	RJMP _0x170
;    1120             else if(indek_data == 5 && data != '|')
_0xB7:
	LDI  R30,LOW(5)
	CP   R30,R8
	BRNE _0xC2
	CPI  R17,124
	BRNE _0xC3
_0xC2:
	RJMP _0xC1
_0xC3:
;    1121             	indek_data= 0;
	RJMP _0x170
;    1122             else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
_0xC1:
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0xC5
;    1123                 time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;    1124             else if((indek_data == PANJANG_PAKET+9) && ID_paket <= jml_paket) //Penerimaan Data
	RJMP _0xC6
_0xC5:
	LDI  R30,LOW(137)
	CP   R30,R8
	BRNE _0xC8
	CP   R11,R5
	BRSH _0xC9
_0xC8:
	RJMP _0xC7
_0xC9:
;    1125     		{
;    1126     			CRC= hitungCRC16(buffer_paket_data, PANJANG_PAKET+7);
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(135)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;    1127 
;    1128     			if(((unsigned char) CRC == buffer_paket_data[PANJANG_PAKET+7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[PANJANG_PAKET+8]))
	__GETB1SX 141
	LDS  R26,_CRC
	CP   R30,R26
	BRNE _0xCB
	LDS  R30,_CRC+1
	ANDI R31,HIGH(0x0)
	MOV  R26,R30
	__GETB1SX 142
	CP   R30,R26
	BREQ _0xCC
_0xCB:
	RJMP _0xCA
_0xCC:
;    1129     			{
;    1130     				if(ID_paket == buffer_paket_data[5])
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDD  R30,Y+11
	CP   R30,R5
	BRNE _0xCD
;    1131     				{
;    1132     					ID_paket_OK= ID_paket;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	MOV  R7,R5
;    1133     					status_paket= KONFIRMASI_PAKET_OK;
	LDI  R30,LOW(3)
	MOV  R10,R30
;    1134     					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
;    1135     					for(i= 7; i<(PANJANG_PAKET+7); i++)
	LDI  R16,LOW(7)
_0xCF:
	CPI  R16,135
	BRSH _0xD0
;    1136     						tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]);
	LDS  R30,_idxBufExtSRAM
	LDS  R31,_idxBufExtSRAM+1
	ADIW R30,1
	STS  _idxBufExtSRAM,R30
	STS  _idxBufExtSRAM+1,R31
	SBIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R16
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,8
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _tulisExtSRAM
;    1137 
;    1138     					ID_paket++;
	SUBI R16,-1
	RJMP _0xCF
_0xD0:
	INC  R5
;    1139     					kirimKonfirmasiPaketOK();
	CALL _kirimKonfirmasiPaketOK
;    1140     					if(ID_paket == (jml_paket+1))
	MOV  R30,R11
	SUBI R30,-LOW(1)
	CP   R30,R5
	BRNE _0xD1
;    1141     						selesai_terima_data= 1;
	SET
	BLD  R15,0
;    1142     				}
_0xD1:
;    1143     				else
	RJMP _0xD2
_0xCD:
;    1144     				{
;    1145     					status_paket= KONFIRMASI_GAGAL;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1146     					kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1147     				}
_0xD2:
;    1148     			}
;    1149     			else
	RJMP _0xD3
_0xCA:
;    1150     			{
;    1151     				status_paket= KONFIRMASI_GAGAL;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1152     				kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1153     			}
_0xD3:
;    1154     			indek_data= 0;
_0x170:
	CLR  R8
;    1155     		}
;    1156         };
_0xC7:
_0xC6:
	RJMP _0xB4
_0xB6:
;    1157     }
;    1158 
;    1159     else if(status_tulis_bertahap) //Data yang di tulis melebihi KAPASITAS_SRAM
	RJMP _0xD4
_0xB3:
	SBRS R3,1
	RJMP _0xD5
;    1160     {
;    1161         while(!selesai_terima_data)
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
_0xD6:
	SBRC R15,0
	RJMP _0xD8
;    1162         {
;    1163             PORTB.7=0;          //Jaga kondisi SCK tanpa Pulsa
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	CBI  0x18,7
;    1164             data= bacaDataSerialIR();
	CALL _bacaDataSerialIR
	MOV  R17,R30
;    1165             //kirimDataSerial(data); //Debug log serial USB
;    1166             buffer_paket_data[indek_data++]= data;
	MOV  R30,R8
	INC  R8
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,6
	ADD  R30,R26
	ADC  R31,R27
	ST   Z,R17
;    1167 
;    1168             if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0xDA
	CPI  R17,60
	BRNE _0xDC
_0xDA:
	LDI  R30,LOW(2)
	CP   R30,R8
	BRNE _0xDD
	CPI  R17,69
	BRNE _0xDC
_0xDD:
	LDI  R30,LOW(3)
	CP   R30,R8
	BRNE _0xDF
	CPI  R17,62
	BRNE _0xDC
_0xDF:
	RJMP _0xD9
_0xDC:
;    1169                 indek_data= 0;
	RJMP _0x171
;    1170             else if(indek_data == 5 && data != '|')
_0xD9:
	LDI  R30,LOW(5)
	CP   R30,R8
	BRNE _0xE4
	CPI  R17,124
	BRNE _0xE5
_0xE4:
	RJMP _0xE3
_0xE5:
;    1171             	indek_data= 0;
	RJMP _0x171
;    1172             else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
_0xE3:
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0xE7
;    1173                 time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;    1174             else if((indek_data == PANJANG_PAKET+9) && ID_paket <= jml_paket) //Penerimaan Data
	RJMP _0xE8
_0xE7:
	LDI  R30,LOW(137)
	CP   R30,R8
	BRNE _0xEA
	CP   R11,R5
	BRSH _0xEB
_0xEA:
	RJMP _0xE9
_0xEB:
;    1175     		{
;    1176     			CRC= hitungCRC16(buffer_paket_data, PANJANG_PAKET+7);
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	MOVW R30,R28
	ADIW R30,6
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(135)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;    1177 
;    1178     			if(((unsigned char) CRC == buffer_paket_data[PANJANG_PAKET+7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[PANJANG_PAKET+8]))
	__GETB1SX 141
	LDS  R26,_CRC
	CP   R30,R26
	BRNE _0xED
	LDS  R30,_CRC+1
	ANDI R31,HIGH(0x0)
	MOV  R26,R30
	__GETB1SX 142
	CP   R30,R26
	BREQ _0xEE
_0xED:
	RJMP _0xEC
_0xEE:
;    1179     			{
;    1180     				if(ID_paket == buffer_paket_data[5])
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDD  R30,Y+11
	CP   R30,R5
	BREQ PC+3
	JMP _0xEF
;    1181     				{
;    1182     					ID_paket_OK= ID_paket;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	MOV  R7,R5
;    1183     					status_paket= KONFIRMASI_PAKET_OK;
	LDI  R30,LOW(3)
	MOV  R10,R30
;    1184     					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
;    1185     					for(i= 7; i<(PANJANG_PAKET+7); i++)
	LDI  R16,LOW(7)
_0xF1:
	CPI  R16,135
	BRSH _0xF2
;    1186     						    tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]);
	LDS  R30,_idxBufExtSRAM
	LDS  R31,_idxBufExtSRAM+1
	ADIW R30,1
	STS  _idxBufExtSRAM,R30
	STS  _idxBufExtSRAM+1,R31
	SBIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	MOV  R30,R16
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,8
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	ST   -Y,R30
	CALL _tulisExtSRAM
;    1187 
;    1188     					jml_data_page= (unsigned int) idxBufExtSRAM/2;
	SUBI R16,-1
	RJMP _0xF1
_0xF2:
	LDS  R30,_idxBufExtSRAM
	LDS  R31,_idxBufExtSRAM+1
	LSR  R31
	ROR  R30
	MOVW R18,R30
;    1189     					//Tulis jika isi buffer SRAM penuh maka segera tulis ke Flash
;    1190     					if(idxBufExtSRAM>KAPASITAS_SRAM)
	LDS  R26,_idxBufExtSRAM
	LDS  R27,_idxBufExtSRAM+1
	CPI  R26,LOW(0x2001)
	LDI  R30,HIGH(0x2001)
	CPC  R27,R30
	BRLO _0xF3
;    1191     					{
;    1192         					idxBufExtSRAM= 0;      //Reset indek buffer
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDI  R30,0
	STS  _idxBufExtSRAM,R30
	STS  _idxBufExtSRAM+1,R30
;    1193         					status_paket= KONFIRMASI_GAGAL;
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1194         					kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1195         					LED_BIRU_ON();
	CBI  0x12,3
;    1196         					alamat_akhir= ISPWriteHugeFlashMemory(jml_data_page, alamat_awal);
	ST   -Y,R19
	ST   -Y,R18
	LDS  R30,_alamat_awal
	LDS  R31,_alamat_awal+1
	ST   -Y,R31
	ST   -Y,R30
	CALL _ISPWriteHugeFlashMemory
	STS  _alamat_akhir,R30
	STS  _alamat_akhir+1,R31
;    1197         					alamat_awal= alamat_akhir;
	STS  _alamat_awal,R30
	STS  _alamat_awal+1,R31
;    1198         					LED_BIRU_OFF();
	SBI  0x12,3
;    1199     					}
;    1200 
;    1201     					ID_paket++;
_0xF3:
	INC  R5
;    1202     					kirimKonfirmasiPaketOK();
	CALL _kirimKonfirmasiPaketOK
;    1203     					if(ID_paket == (jml_paket+1))
	MOV  R30,R11
	SUBI R30,-LOW(1)
	CP   R30,R5
	BRNE _0xF4
;    1204     						selesai_terima_data= 1;
	SET
	BLD  R15,0
;    1205     				}
_0xF4:
;    1206     				else
	RJMP _0xF5
_0xEF:
;    1207     				{
;    1208     					status_paket= KONFIRMASI_GAGAL;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1209     					kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1210     				}
_0xF5:
;    1211     			}
;    1212     			else
	RJMP _0xF6
_0xEC:
;    1213     			{
;    1214     				status_paket= KONFIRMASI_GAGAL;
;	selesai_terima_data -> R15.0
;	buffer_paket_data -> Y+6
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1215     				kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1216     			}
_0xF6:
;    1217     			indek_data= 0;
_0x171:
	CLR  R8
;    1218     		}
;    1219         };
_0xE9:
_0xE8:
	RJMP _0xD6
_0xD8:
;    1220     }
;    1221 }
_0xD5:
_0xD4:
	CALL __LOADLOCR6
	ADIW R28,63
	ADIW R28,63
	ADIW R28,17
	POP  R15
	RET
;    1222 /************************************************************************************************************/
;    1223 void olahPaketAkhir(void)
;    1224 {
_olahPaketAkhir:
	PUSH R15
;    1225     bit selesai_terima_akhir;
;    1226     unsigned char data;
;    1227     unsigned char buffer_paket_data[11];   // Ukuran buffer adalah [HEADER(7) + Data + CRC(2)]
;    1228                                            // Data dapat berupa kode devais atau jumlah paket
;    1229 
;    1230     while(!selesai_terima_akhir)
	SBIW R28,11
	ST   -Y,R17
;	selesai_terima_akhir -> R15.0
;	data -> R17
;	buffer_paket_data -> Y+1
_0xF7:
	SBRC R15,0
	RJMP _0xF9
;    1231     {
;    1232         data= bacaDataSerialIR();
;	selesai_terima_akhir -> R15.0
;	buffer_paket_data -> Y+1
	CALL _bacaDataSerialIR
	MOV  R17,R30
;    1233         //kirimDataSerial(data); //Debug log serial USB
;    1234         buffer_paket_data[indek_data++]= data;
	MOV  R30,R8
	INC  R8
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,1
	ADD  R30,R26
	ADC  R31,R27
	ST   Z,R17
;    1235 
;    1236         if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0xFB
	CPI  R17,60
	BRNE _0xFD
_0xFB:
	LDI  R30,LOW(2)
	CP   R30,R8
	BRNE _0xFE
	CPI  R17,69
	BRNE _0xFD
_0xFE:
	LDI  R30,LOW(3)
	CP   R30,R8
	BRNE _0x100
	CPI  R17,62
	BRNE _0xFD
_0x100:
	RJMP _0xFA
_0xFD:
;    1237             indek_data= 0;
	RJMP _0x172
;    1238         else if(indek_data == 5 && data != '|')
_0xFA:
	LDI  R30,LOW(5)
	CP   R30,R8
	BRNE _0x105
	CPI  R17,124
	BRNE _0x106
_0x105:
	RJMP _0x104
_0x106:
;    1239         	indek_data= 0;
	RJMP _0x172
;    1240         else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
_0x104:
	LDI  R30,LOW(1)
	CP   R30,R8
	BRNE _0x108
;    1241             time1= USART_TIMEOUT;
	LDI  R30,LOW(68)
	LDI  R31,HIGH(68)
	STS  _time1,R30
	STS  _time1+1,R31
;    1242         else if(status_perintah_tulis && indek_data == 9)// Deteksi akhir kiriman
	RJMP _0x109
_0x108:
	SBRS R3,0
	RJMP _0x10B
	LDI  R30,LOW(9)
	CP   R30,R8
	BREQ _0x10C
_0x10B:
	RJMP _0x10A
_0x10C:
;    1243 		{
;    1244 			CRC= hitungCRC16(buffer_paket_data, 7);
;	selesai_terima_akhir -> R15.0
;	buffer_paket_data -> Y+1
	MOVW R30,R28
	ADIW R30,1
	ST   -Y,R31
	ST   -Y,R30
	LDI  R30,LOW(7)
	ST   -Y,R30
	RCALL _hitungCRC16
	STS  _CRC,R30
	STS  _CRC+1,R31
;    1245 
;    1246 			if(((unsigned char) CRC == buffer_paket_data[7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[8]))
	LDD  R30,Y+8
	LDS  R26,_CRC
	CP   R30,R26
	BRNE _0x10E
	LDS  R30,_CRC+1
	ANDI R31,HIGH(0x0)
	MOV  R26,R30
	LDD  R30,Y+9
	CP   R30,R26
	BREQ _0x10F
_0x10E:
	RJMP _0x10D
_0x10F:
;    1247 			{
;    1248 				if((buffer_paket_data[3] == '\n') && (buffer_paket_data[5] == '\r'))
;	selesai_terima_akhir -> R15.0
;	buffer_paket_data -> Y+1
	LDD  R26,Y+4
	CPI  R26,LOW(0xA)
	BRNE _0x111
	LDD  R26,Y+6
	CPI  R26,LOW(0xD)
	BREQ _0x112
_0x111:
	RJMP _0x110
_0x112:
;    1249       				selesai_terima_akhir= 1;
	SET
	BLD  R15,0
;    1250 				else
	RJMP _0x113
_0x110:
;    1251 				{
;    1252 					status_paket= KONFIRMASI_GAGAL;
;	selesai_terima_akhir -> R15.0
;	buffer_paket_data -> Y+1
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1253 					kirimKonfirmasiGAGAL();
	CALL _kirimKonfirmasiGAGAL
;    1254 				}
_0x113:
;    1255 			}
;    1256 			indek_data= 0;
_0x10D:
_0x172:
	CLR  R8
;    1257 		}
;    1258     };
_0x10A:
_0x109:
	RJMP _0xF7
_0xF9:
;    1259 }
	LDD  R17,Y+0
_0x168:
	ADIW R28,12
	POP  R15
	RET
;    1260 /************************************************************************************************************/
;    1261 void terimaDataDariPemrogram(void)
;    1262 {
_terimaDataDariPemrogram:
;    1263     olahPaketPerintah();
	CALL _olahPaketPerintah
;    1264     if(status_perintah_tulis)
	SBRS R3,0
	RJMP _0x114
;    1265     {
;    1266         olahPaketData();
	CALL _olahPaketData
;    1267         olahPaketAkhir();
	CALL _olahPaketAkhir
;    1268     }
;    1269 }
_0x114:
	RET
;    1270 /************************************************************************************************************/
;    1271 // Fungsi Main
;    1272 void main (void)
;    1273 {
_main:
;    1274     unsigned char i;
;    1275 
;    1276     // Crystal Oscillator division factor: 1
;    1277     #pragma optsize-
;    1278     CLKPR=0x80;
;	i -> R17
	LDI  R30,LOW(128)
	STS  97,R30
;    1279     CLKPR=0x00;
	LDI  R30,LOW(0)
	STS  97,R30
;    1280     #ifdef _OPTIMIZE_SIZE_
;    1281     #pragma optsize+
;    1282     #endif
;    1283 
;    1284     // Input/Output Ports initialization
;    1285     // Port A initialization
;    1286     // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;    1287     // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;    1288     PORTA=0x00;
	OUT  0x1B,R30
;    1289     DDRA=0x00;
	OUT  0x1A,R30
;    1290 
;    1291     // Port B initialization
;    1292     // Func7=Out Func6=In Func5=Out Func4=Out Func3=In Func2=In Func1=Out Func0=Out
;    1293     // State7=0 State6=T State5=0 State4=0 State3=T State2=T State1=0 State0=0
;    1294     PORTB=0x10;
	LDI  R30,LOW(16)
	OUT  0x18,R30
;    1295     DDRB=0xB3;
	LDI  R30,LOW(179)
	OUT  0x17,R30
;    1296 
;    1297     // Port C initialization
;    1298     // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;    1299     // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;    1300     PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;    1301     DDRC=0x00;
	OUT  0x14,R30
;    1302 
;    1303     // Port D initialization
;    1304     // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;    1305     // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;    1306     PORTD=0b11110011;
	LDI  R30,LOW(243)
	OUT  0x12,R30
;    1307     DDRD=0b11001110;
	LDI  R30,LOW(206)
	OUT  0x11,R30
;    1308 
;    1309     // Port E initialization
;    1310     // Func2=In Func1=In Func0=In
;    1311     // State2=T State1=T State0=T
;    1312     PORTE=0x01;
	LDI  R30,LOW(1)
	OUT  0x7,R30
;    1313     DDRE=0x01;
	OUT  0x6,R30
;    1314 
;    1315     // Analog Comparator initialization
;    1316     // Analog Comparator: Off
;    1317     // Analog Comparator Input Capture by Timer/Counter 1: Off
;    1318     ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;    1319 
;    1320     //Inisialisasi Hardware
;    1321     inisialisasiUSART();
	CALL _inisialisasiUSART
;    1322     inisialisasiSPILowSpeed();
	CALL _inisialisasiSPILowSpeed
;    1323     inisialisasiSPIHighSpeed();
	CALL _inisialisasiSPIHighSpeed
;    1324     inisialisasiTimer();
	CALL _inisialisasiTimer
;    1325     inisialisasiExtSRAM();
	CALL _inisialisasiExtSRAM
;    1326     AKTIFKAN_LATCH();
	CBI  0x12,2
;    1327 
;    1328     //Pengkondisian awal transmisi
;    1329     AKTIFKAN_TRANSMITTER_IR();
	SBI  0xA,3
;    1330     AKTIFKAN_SINYAL_CARRIER();
	LDI  R30,LOW(25)
	OUT  0x27,R30
;    1331     AKTIFKAN_RECEIVER_IR();
	SBI  0xA,4
;    1332     AKTIFKAN_RECEIVER_INTERRUPT_IR();
	SBI  0xA,7
;    1333 
;    1334     for(;;)
_0x116:
;    1335     {
;    1336         inisialisasi();
	CALL _inisialisasi
;    1337         terimaDataDariPemrogram();
	CALL _terimaDataDariPemrogram
;    1338         if(!status_tulis_bertahap) menuISP(instruksi_pemrogram);
	SBRC R3,1
	RJMP _0x118
	ST   -Y,R13
	CALL _menuISP
;    1339         //Penulisan bertahap untuk flash berukuran besar melebihi kapasitas SRAM
;    1340         else if(status_tulis_bertahap)
	RJMP _0x119
_0x118:
	SBRS R3,1
	RJMP _0x11A
;    1341         {
;    1342     	    status_paket= TDK_PERLU_KONFIRMASI;
	CLR  R10
;    1343         	jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK
	LDS  R30,_idxBufExtSRAM
	LDS  R31,_idxBufExtSRAM+1
	LSR  R31
	ROR  R30
	STS  _jml_page_data,R30
	STS  _jml_page_data+1,R31
;    1344         	LED_BIRU_ON();
	CBI  0x12,3
;    1345         	alamat_akhir= ISPWriteHugeFlashMemory(jml_page_data, alamat_awal);
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_alamat_awal
	LDS  R31,_alamat_awal+1
	ST   -Y,R31
	ST   -Y,R30
	CALL _ISPWriteHugeFlashMemory
	STS  _alamat_akhir,R30
	STS  _alamat_akhir+1,R31
;    1346         	LED_BIRU_OFF();
	SBI  0x12,3
;    1347     	}
;    1348         status_paket= AKHIR_PROSES; //Set penanda akhir proses
_0x11A:
_0x119:
	LDI  R30,LOW(5)
	MOV  R10,R30
;    1349         kirimKonfirmasiIDLE();
	CALL _kirimKonfirmasiIDLE
;    1350         for(i=0;i<4;i++)
	LDI  R17,LOW(0)
_0x11C:
	CPI  R17,4
	BRSH _0x11D
;    1351         {
;    1352             LED_MERAH_ON();
	CBI  0x7,0
;    1353             LED_BIRU_OFF();
	SBI  0x12,3
;    1354             delay_ms(200);
	LDI  R30,LOW(200)
	LDI  R31,HIGH(200)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1355             LED_MERAH_OFF();
	SBI  0x7,0
;    1356             LED_BIRU_ON();
	CBI  0x12,3
;    1357             delay_ms(200);
	LDI  R30,LOW(200)
	LDI  R31,HIGH(200)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1358         };
	SUBI R17,-1
	RJMP _0x11C
_0x11D:
;    1359     };
	RJMP _0x116
;    1360 }
_0x11E:
	NOP
	RJMP _0x11E
;    1361 #include "crc.h"
;    1362 
;    1363 unsigned int hitungCRC16(unsigned char *data, unsigned char len)
;    1364 {
_hitungCRC16:
;    1365 	unsigned int sum= 0;
;    1366 	unsigned char j;
;    1367 
;    1368 	for(j= 0; j <= len-1; ++j)
	CALL __SAVELOCR4
;	*data -> Y+5
;	len -> Y+4
;	sum -> R16,R17
;	j -> R19
	LDI  R16,0
	LDI  R17,0
	LDI  R19,LOW(0)
_0x120:
	LDD  R30,Y+4
	SUBI R30,LOW(1)
	CP   R30,R19
	BRLO _0x121
;    1369 	{
;    1370 		unsigned char i;
;    1371 
;    1372 		sum^= *data++;
	SBIW R28,1
;	*data -> Y+6
;	len -> Y+5
;	i -> Y+0
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
	LDI  R31,0
	__EORWRR 16,17,30,31
;    1373 
;    1374 		for(i= 0; i < 8; ++i)
	LDI  R30,LOW(0)
	ST   Y,R30
_0x123:
	LD   R26,Y
	CPI  R26,LOW(0x8)
	BRSH _0x124
;    1375 		{
;    1376 			if(sum & 1)
	SBRS R16,0
	RJMP _0x125
;    1377 				sum= (sum >> 1) ^ POLYNOM;
	MOVW R30,R16
	LSR  R31
	ROR  R30
	LDI  R26,LOW(40961)
	LDI  R27,HIGH(40961)
	EOR  R30,R26
	EOR  R31,R27
	MOVW R16,R30
;    1378 			else
	RJMP _0x126
_0x125:
;    1379 				sum= (sum >> 1);
	LSR  R17
	ROR  R16
;    1380 		};
_0x126:
	LD   R30,Y
	SUBI R30,-LOW(1)
	ST   Y,R30
	RJMP _0x123
_0x124:
;    1381 	};
	ADIW R28,1
	SUBI R19,-LOW(1)
	RJMP _0x120
_0x121:
;    1382 
;    1383 	return sum;
	MOVW R30,R16
	CALL __LOADLOCR4
	ADIW R28,7
	RET
;    1384 }
;    1385 #include "debug.h"
;    1386 
;    1387 //debugging program
;    1388 unsigned int alamat_EEPROM;

	.DSEG
_alamat_EEPROM:
	.BYTE 0x2
;    1389 
;    1390 void tulis_EEPROM(unsigned char data)
;    1391 {

	.CSEG
;    1392 
;    1393   /* Tunggu sampai syarat kondisi terpenuhi  */
;    1394   if(alamat_EEPROM<EEPROM_ADDRESS_MAX)
;	data -> Y+0
;    1395     {
;    1396       /* Tunggu sampai proses penulisan EEPROM sebelumnya selesai*/
;    1397       while(EECR & (1<<EEWE));
;    1398 
;    1399       /* Setting alamat dan data register EEPROM */
;    1400       EEAR = alamat_EEPROM;
;    1401       EEDR = data;
;    1402 
;    1403      /* Mengaktifkan EEPROM Master Write Enable (EEMWE) */
;    1404       EECR |= (1<<EEMWE);
;    1405 
;    1406      /* Mulai menuliskan data dengan mengaktifkan EEPROM Write Enable (EEWE) */
;    1407       EECR |= (1<<EEWE);
;    1408 
;    1409      /* Penambahan (increment) alamat EEPROM */
;    1410       alamat_EEPROM++;
;    1411     }
;    1412 }
;    1413 #include "eeprom.h"
;    1414 #include "extsram/extsram.h"
;    1415 #include "isp/isp.h"
;    1416 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    1417 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    1418 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    1419 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    1420 	#endif
	#endif
;    1421 
;    1422 void ISPWriteEEPROM(unsigned int jml_byte_data)
;    1423 {
_ISPWriteEEPROM:
;    1424     //Variabel Tulis Data EEPROM Memory Byte Mode
;    1425     unsigned char data;
;    1426     unsigned int i;
;    1427     unsigned int addr_EEPROM_memory;
;    1428 
;    1429     addr_EEPROM_memory= 0;
	CALL __SAVELOCR6
;	jml_byte_data -> Y+6
;	data -> R17
;	i -> R18,R19
;	addr_EEPROM_memory -> R20,R21
	__GETWRN 20,21,0
;    1430     i=0;
	__GETWRN 18,19,0
;    1431 
;    1432     while(jml_byte_data>0)
_0x12B:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	CALL __CPW02
	BRSH _0x12D
;    1433     {
;    1434         /** Instruksi Write EEPROM Memory pada Serial Programming **/
;    1435         data= bacaExtSRAM(i++);
	MOVW R30,R18
	__ADDWRN 18,19,1
	ST   -Y,R31
	ST   -Y,R30
	RCALL _bacaExtSRAM
	MOV  R17,R30
;    1436         if(data!=0xFF)
	CPI  R17,255
	BREQ _0x12E
;    1437         {
;    1438             SPIMasterKirimData(0xC0);
	LDI  R30,LOW(192)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1439             SPIMasterKirimData(addr_EEPROM_memory>>8);
	MOV  R30,R21
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1440             SPIMasterKirimData(addr_EEPROM_memory);
	ST   -Y,R20
	CALL _SPIMasterKirimData
;    1441             SPIMasterKirimData(data);
	ST   -Y,R17
	CALL _SPIMasterKirimData
;    1442 
;    1443             /*** Polling Data EEPROM Memory ***/
;    1444             ISPPollingEEPROM(addr_EEPROM_memory);
	ST   -Y,R21
	ST   -Y,R20
	RCALL _ISPPollingEEPROM
;    1445         }
;    1446 
;    1447         /* Autoincrement alamat untuk writing EEPROM memory dan parameter polling EEPROM memory */
;    1448         addr_EEPROM_memory++;
_0x12E:
	__ADDWRN 20,21,1
;    1449 
;    1450         jml_byte_data--;
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	SBIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
;    1451     }
	RJMP _0x12B
_0x12D:
;    1452 }
	RJMP _0x167
;    1453 
;    1454 void ISPReadEEPROM(unsigned int addr_EEPROM_memory_max)
;    1455 {
_ISPReadEEPROM:
;    1456     unsigned char data_EEPROM;
;    1457     unsigned int addr_EEPROM_memory;
;    1458     unsigned int i;
;    1459 
;    1460     addr_EEPROM_memory= 0;
	CALL __SAVELOCR6
;	addr_EEPROM_memory_max -> Y+6
;	data_EEPROM -> R17
;	addr_EEPROM_memory -> R18,R19
;	i -> R20,R21
	__GETWRN 18,19,0
;    1461     i= 0;
	__GETWRN 20,21,0
;    1462 
;    1463     while(addr_EEPROM_memory<addr_EEPROM_memory_max)
_0x12F:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	CP   R18,R30
	CPC  R19,R31
	BRSH _0x131
;    1464     {
;    1465         /*** Instruksi Read EEPROM Memory pada Serial Programming ***/
;    1466         //Read EEPROM Memory Low Byte
;    1467         SPIMasterKirimData(0xA0);
	LDI  R30,LOW(160)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1468         SPIMasterKirimData(addr_EEPROM_memory>>8);
	MOV  R30,R19
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1469         SPIMasterKirimData(addr_EEPROM_memory);
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1470         data_EEPROM=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1471         tulisExtSRAM(i++, data_EEPROM);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	RCALL _tulisExtSRAM
;    1472 
;    1473         /* Autoincrement alamat EEPROM memory */
;    1474         addr_EEPROM_memory++;
	__ADDWRN 18,19,1
;    1475     };
	RJMP _0x12F
_0x131:
;    1476 }
_0x167:
	CALL __LOADLOCR6
	ADIW R28,8
	RET
;    1477 
;    1478 void ISPPollingEEPROM(unsigned int addr_EEPROM_memory)
;    1479 {
_ISPPollingEEPROM:
;    1480     unsigned char ulang_polling_EEPROM;
;    1481     unsigned char data_EEPROM_memory;
;    1482 
;    1483     //Reload jumlah pengulangan polling data EEPROM memory
;    1484     ulang_polling_EEPROM= ULANG_POLLING_EEPROM_MAX;
	ST   -Y,R17
	ST   -Y,R16
;	addr_EEPROM_memory -> Y+2
;	ulang_polling_EEPROM -> R17
;	data_EEPROM_memory -> R16
	LDI  R17,LOW(2)
;    1485 
;    1486     while(ulang_polling_EEPROM>0)
_0x132:
	CPI  R17,1
	BRLO _0x134
;    1487     {
;    1488         /*** Instruksi Read EEPROM pada Serial Programming ***/
;    1489         SPIMasterKirimData(0xA0);
	LDI  R30,LOW(160)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1490         SPIMasterKirimData(addr_EEPROM_memory>>8);
	LDD  R30,Y+3
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1491         SPIMasterKirimData(addr_EEPROM_memory);
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1492         data_EEPROM_memory=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R16,R30
;    1493         if(data_EEPROM_memory==0xFF) delay_ms(9);
	CPI  R16,255
	BRNE _0x135
	LDI  R30,LOW(9)
	LDI  R31,HIGH(9)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1494         ulang_polling_EEPROM--;
_0x135:
	SUBI R17,1
;    1495     }
	RJMP _0x132
_0x134:
;    1496 }
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	RET
;    1497 #include "extsram.h"
;    1498 
;    1499 /************************************************************************************************************/
;    1500 // Fungsi Inisialisasi SRAM Eksternal
;    1501 void inisialisasiExtSRAM(void)
;    1502 {
_inisialisasiExtSRAM:
;    1503     // External Interrupt(s) initialization
;    1504     // INT0: Off
;    1505     // INT1: Off
;    1506     // INT2: Off
;    1507     // Interrupt on any change on pins PCINT0-7: Off
;    1508     // Interrupt on any change on pins PCINT8-15: Off
;    1509     // External SRAM page configuration:
;    1510     //              -              / 0000h - 1FFFh
;    1511     // Lower page wait state(s): None
;    1512     // Upper page wait state(s): None
;    1513     MCUCR=0x80;
	LDI  R30,LOW(128)
	OUT  0x35,R30
;    1514     EMCUCR=0x00;
	LDI  R30,LOW(0)
	OUT  0x36,R30
;    1515 }
	RET
;    1516 /************************************************************************************************************/
;    1517 // Fungsi Pembacaan SRAM Eksternal
;    1518 unsigned char bacaExtSRAM(unsigned int addr_read_ext_SRAM)
;    1519 {
_bacaExtSRAM:
;    1520     unsigned char *ptr_data = (unsigned char *) (addr_read_ext_SRAM + OFFSET_ADDR_EXT_SRAM);
;    1521     unsigned char data;
;    1522     DDRC = 0xFF;
	CALL __SAVELOCR4
;	addr_read_ext_SRAM -> Y+4
;	*ptr_data -> R16,R17
;	data -> R19
	LDD  R30,Y+4
	LDD  R31,Y+4+1
	SUBI R30,LOW(-1280)
	SBCI R31,HIGH(-1280)
	MOVW R16,R30
	LDI  R30,LOW(255)
	OUT  0x14,R30
;    1523     PORTC = 0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;    1524     data= *ptr_data;
	MOVW R26,R16
	LD   R19,X
;    1525     return data;
	MOV  R30,R19
	CALL __LOADLOCR4
	ADIW R28,6
	RET
;    1526 }
;    1527 /************************************************************************************************************/
;    1528 // Fungsi Penulisan SRAM Eksternal
;    1529 void tulisExtSRAM(unsigned int addr_write_ext_SRAM, unsigned char data)
;    1530 {
_tulisExtSRAM:
;    1531     unsigned char *ptr_data = (unsigned char *) (addr_write_ext_SRAM + OFFSET_ADDR_EXT_SRAM);
;    1532     DDRC = 0xFF;
	ST   -Y,R17
	ST   -Y,R16
;	addr_write_ext_SRAM -> Y+3
;	data -> Y+2
;	*ptr_data -> R16,R17
	LDD  R30,Y+3
	LDD  R31,Y+3+1
	SUBI R30,LOW(-1280)
	SBCI R31,HIGH(-1280)
	MOVW R16,R30
	LDI  R30,LOW(255)
	OUT  0x14,R30
;    1533     PORTC = 0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
;    1534     *ptr_data= data;
	LDD  R30,Y+2
	MOVW R26,R16
	ST   X,R30
;    1535 }
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,5
	RET
;    1536 #include "flash.h"
;    1537 #include "extsram/extsram.h"
;    1538 #include "isp/isp.h"
;    1539 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    1540 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    1541 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    1542 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    1543 	#endif
	#endif
;    1544 
;    1545 /***  Operasi Penulisan Memori Flash Page Mode***/
;    1546 void ISPWriteFlashMemory(unsigned int jml_page_data)
;    1547 {
_ISPWriteFlashMemory:
;    1548     //Variabel Tulis Data Flash Memory
;    1549     unsigned int addr_flash_memory_page;
;    1550     unsigned char data_low_byte, data_high_byte;
;    1551     unsigned int i;
;    1552 
;    1553     addr_flash_memory_page=0x00;
	CALL __SAVELOCR6
;	jml_page_data -> Y+6
;	addr_flash_memory_page -> R16,R17
;	data_low_byte -> R19
;	data_high_byte -> R18
;	i -> R20,R21
	__GETWRN 16,17,0
;    1554     i=0;
	__GETWRN 20,21,0
;    1555 
;    1556     while(jml_page_data>0)
_0x136:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	CALL __CPW02
	BRLO PC+3
	JMP _0x138
;    1557     {
;    1558         //Baca data memori dari SRAM Eksternal
;    1559         data_low_byte=bacaExtSRAM(i++);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	MOV  R19,R30
;    1560         data_high_byte=bacaExtSRAM(i++);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	MOV  R18,R30
;    1561 
;    1562         /*** Instruksi Load Program Memory Page pada Serial Programming ***/
;    1563         //Load Program Memory Low Byte
;    1564         SPIMasterKirimData(0x40);
	LDI  R30,LOW(64)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1565         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1566         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1567         SPIMasterKirimData(data_low_byte);  //data low byte
	ST   -Y,R19
	CALL _SPIMasterKirimData
;    1568 
;    1569         /*** Instruksi Load Program Memory Page pada Serial Programming ***/
;    1570         //Load Program Memory High Byte
;    1571         SPIMasterKirimData(0x48);
	LDI  R30,LOW(72)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1572         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1573         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1574         SPIMasterKirimData(data_high_byte);  //data high byte
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1575 
;    1576         /*** Instruksi Write Program Memory Page pada Serial Programming ***/
;    1577         SPIMasterKirimData(0x4C);
	LDI  R30,LOW(76)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1578         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1579         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1580         SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1581 
;    1582         /*** Polling Data Flash Memory ***/
;    1583         ISPPollingFlashMemory(addr_flash_memory_page);
	ST   -Y,R17
	ST   -Y,R16
	RCALL _ISPPollingFlashMemory
;    1584 
;    1585         /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */
;    1586         addr_flash_memory_page++;
	__ADDWRN 16,17,1
;    1587 
;    1588         jml_page_data--;
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	SBIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
;    1589     };
	RJMP _0x136
_0x138:
;    1590 }
	RJMP _0x166
;    1591 
;    1592 /***  Operasi Penulisan Memori Flash Page Mode Secara Bertahap***/
;    1593 unsigned int ISPWriteHugeFlashMemory(unsigned int jml_page_data, unsigned int addr_page_flash_terakhir)
;    1594 {
_ISPWriteHugeFlashMemory:
;    1595     //Variabel Tulis Data Flash Memory
;    1596     unsigned int addr_flash_memory_page;
;    1597     unsigned char data_low_byte, data_high_byte;
;    1598     unsigned int i;
;    1599 
;    1600     addr_flash_memory_page=addr_page_flash_terakhir;
	CALL __SAVELOCR6
;	jml_page_data -> Y+8
;	addr_page_flash_terakhir -> Y+6
;	addr_flash_memory_page -> R16,R17
;	data_low_byte -> R19
;	data_high_byte -> R18
;	i -> R20,R21
	__GETWRS 16,17,6
;    1601     i=0;
	__GETWRN 20,21,0
;    1602 
;    1603     if(awal_akses_devais)
	SBRS R2,0
	RJMP _0x139
;    1604     {
;    1605         enterProgrammingMode();
	CALL _enterProgrammingMode
;    1606         chipErase();
	CALL _chipErase
;    1607         awal_akses_devais=0;
	CLT
	BLD  R2,0
;    1608     }
;    1609 
;    1610     //enterProgrammingMode();
;    1611     while(jml_page_data>0)
_0x139:
_0x13A:
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CALL __CPW02
	BRLO PC+3
	JMP _0x13C
;    1612     {
;    1613         //Baca data memori dari SRAM Eksternal
;    1614         data_low_byte=bacaExtSRAM(i++);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	MOV  R19,R30
;    1615         data_high_byte=bacaExtSRAM(i++);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _bacaExtSRAM
	MOV  R18,R30
;    1616 
;    1617         /*** Instruksi Load Program Memory Page pada Serial Programming ***/
;    1618         //Load Program Memory Low Byte
;    1619         SPIMasterKirimData(0x40);
	LDI  R30,LOW(64)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1620         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1621         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1622         SPIMasterKirimData(data_low_byte);  //data low byte
	ST   -Y,R19
	CALL _SPIMasterKirimData
;    1623 
;    1624         /*** Instruksi Load Program Memory Page pada Serial Programming ***/
;    1625         //Load Program Memory High Byte
;    1626         SPIMasterKirimData(0x48);
	LDI  R30,LOW(72)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1627         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1628         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1629         SPIMasterKirimData(data_high_byte);  //data high byte
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1630 
;    1631         /*** Instruksi Write Program Memory Page pada Serial Programming ***/
;    1632         SPIMasterKirimData(0x4C);
	LDI  R30,LOW(76)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1633         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R17
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1634         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R16
	CALL _SPIMasterKirimData
;    1635         SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1636 
;    1637         /*** Polling Data Flash Memory ***/
;    1638         ISPPollingFlashMemory(addr_flash_memory_page);
	ST   -Y,R17
	ST   -Y,R16
	RCALL _ISPPollingFlashMemory
;    1639 
;    1640         /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */
;    1641         addr_flash_memory_page++;
	__ADDWRN 16,17,1
;    1642 
;    1643         //Simpan alamat halaman flash terakhir yang telah diakses
;    1644         addr_page_flash_terakhir= addr_flash_memory_page;
	__PUTWSR 16,17,6
;    1645 
;    1646         //Decrement
;    1647         jml_page_data--;
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	SBIW R30,1
	STD  Y+8,R30
	STD  Y+8+1,R31
;    1648     };
	RJMP _0x13A
_0x13C:
;    1649 
;    1650     if(akhir_akses_devais)leaveProgrammingMode();
	SBRS R2,1
	RJMP _0x13D
	CALL _leaveProgrammingMode
;    1651     return addr_page_flash_terakhir;
_0x13D:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	RJMP _0x165
;    1652 }
;    1653 
;    1654 /***  Operasi Pembacaan Memori Flash Page Mode ***/
;    1655 void ISPReadFlashMemory(unsigned int page_flash_max)
;    1656 {
_ISPReadFlashMemory:
;    1657      unsigned char data_flash;
;    1658      unsigned int addr_flash_memory_page;
;    1659      unsigned int i;
;    1660 
;    1661      data_flash= 0;
	CALL __SAVELOCR6
;	page_flash_max -> Y+6
;	data_flash -> R17
;	addr_flash_memory_page -> R18,R19
;	i -> R20,R21
	LDI  R17,LOW(0)
;    1662      addr_flash_memory_page= 0;
	__GETWRN 18,19,0
;    1663      i= 0;
	__GETWRN 20,21,0
;    1664 
;    1665      while(addr_flash_memory_page<page_flash_max)
_0x13E:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	CP   R18,R30
	CPC  R19,R31
	BRSH _0x140
;    1666      {
;    1667           /*** Instruksi Read Program Memory pada Serial Programming ***/
;    1668           //Read Program Memory Low Byte
;    1669           SPIMasterKirimData(0x20);
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1670           SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R19
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1671           SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1672           data_flash= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1673           tulisExtSRAM(i++, data_flash);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	CALL _tulisExtSRAM
;    1674 
;    1675           //Read Program Memory High Byte
;    1676           SPIMasterKirimData(0x28);
	LDI  R30,LOW(40)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1677           SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R19
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1678           SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1679           data_flash= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1680           tulisExtSRAM(i++, data_flash);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	CALL _tulisExtSRAM
;    1681 
;    1682           /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */
;    1683           addr_flash_memory_page++;
	__ADDWRN 18,19,1
;    1684      };
	RJMP _0x13E
_0x140:
;    1685 }
_0x166:
	CALL __LOADLOCR6
	ADIW R28,8
	RET
;    1686 /***  Operasi Pembacaan Memori Flash Page Mode ***/
;    1687 unsigned int ISPReadHugeFlashMemory(unsigned int jml_halaman_flash, unsigned int addr_page_flash_terakhir)
;    1688 {
_ISPReadHugeFlashMemory:
;    1689      unsigned char data_flash;
;    1690      unsigned int addr_flash_memory_page;
;    1691      unsigned int i;
;    1692 
;    1693      addr_flash_memory_page= addr_page_flash_terakhir;
	CALL __SAVELOCR6
;	jml_halaman_flash -> Y+8
;	addr_page_flash_terakhir -> Y+6
;	data_flash -> R17
;	addr_flash_memory_page -> R18,R19
;	i -> R20,R21
	__GETWRS 18,19,6
;    1694      i= 0;
	__GETWRN 20,21,0
;    1695 
;    1696      if(awal_akses_devais)
	SBRS R2,0
	RJMP _0x141
;    1697     {
;    1698         enterProgrammingMode();
	CALL _enterProgrammingMode
;    1699         awal_akses_devais=0;
	CLT
	BLD  R2,0
;    1700     }
;    1701 
;    1702      while(jml_halaman_flash>0)
_0x141:
_0x142:
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CALL __CPW02
	BRSH _0x144
;    1703      {
;    1704           /*** Instruksi Read Program Memory pada Serial Programming ***/
;    1705           //Read Program Memory Low Byte
;    1706           SPIMasterKirimData(0x20);
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1707           SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R19
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1708           SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1709           data_flash= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1710           tulisExtSRAM(i++, data_flash);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	CALL _tulisExtSRAM
;    1711 
;    1712           //Read Program Memory High Byte
;    1713           SPIMasterKirimData(0x28);
	LDI  R30,LOW(40)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1714           SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	MOV  R30,R19
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1715           SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	ST   -Y,R18
	CALL _SPIMasterKirimData
;    1716           data_flash= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1717           tulisExtSRAM(i++, data_flash);
	MOVW R30,R20
	__ADDWRN 20,21,1
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R17
	CALL _tulisExtSRAM
;    1718 
;    1719           /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */
;    1720           addr_flash_memory_page++;
	__ADDWRN 18,19,1
;    1721 
;    1722           //Simpan alamat halaman flash terakhir yang telah diakses
;    1723           addr_page_flash_terakhir= addr_flash_memory_page;
	__PUTWSR 18,19,6
;    1724 
;    1725          //Decrement
;    1726          jml_halaman_flash--;
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	SBIW R30,1
	STD  Y+8,R30
	STD  Y+8+1,R31
;    1727      };
	RJMP _0x142
_0x144:
;    1728      if(akhir_akses_devais)leaveProgrammingMode();
	SBRS R2,1
	RJMP _0x145
	CALL _leaveProgrammingMode
;    1729      return addr_page_flash_terakhir;
_0x145:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
_0x165:
	CALL __LOADLOCR6
	ADIW R28,10
	RET
;    1730 }
;    1731 
;    1732 /***  Operasi Polling Memori Flash Byte Mode untuk Menunggu Kesiapan Penulisan Byte Berikutnya***/
;    1733 void ISPPollingFlashMemory(unsigned int addr_flash_memory_page)
;    1734 {
_ISPPollingFlashMemory:
;    1735     unsigned char ulang_polling_flash;
;    1736     unsigned char data_flash_memory;
;    1737 
;    1738     //Reload jumlah pengulangan polling data flash memory
;    1739     ulang_polling_flash=ULANG_POLLING_FLASH_MAX;
	ST   -Y,R17
	ST   -Y,R16
;	addr_flash_memory_page -> Y+2
;	ulang_polling_flash -> R17
;	data_flash_memory -> R16
	LDI  R17,LOW(2)
;    1740 
;    1741     //Polling Address Low Byte
;    1742     while(ulang_polling_flash>0)
_0x146:
	CPI  R17,1
	BRLO _0x148
;    1743     {
;    1744         /*** Instruksi Read Program Memory pada Serial Programming ***/
;    1745         SPIMasterKirimData(0x20);
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1746         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	LDD  R30,Y+3
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1747         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1748         data_flash_memory=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R16,R30
;    1749         ulang_polling_flash--;
	SUBI R17,1
;    1750         if(data_flash_memory==0xFF)
	CPI  R16,255
	BRNE _0x149
;    1751         {
;    1752             delay_ms(4.5);
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1753             ulang_polling_flash= 0;
	LDI  R17,LOW(0)
;    1754         }
;    1755     };
_0x149:
	RJMP _0x146
_0x148:
;    1756 
;    1757     //Reload jumlah pengulangan polling data flash memory
;    1758     ulang_polling_flash=ULANG_POLLING_FLASH_MAX;
	LDI  R17,LOW(2)
;    1759 
;    1760     //Polling Address High Byte
;    1761     while(ulang_polling_flash>0)
_0x14A:
	CPI  R17,1
	BRLO _0x14C
;    1762     {
;    1763         /*** Instruksi Read Program Memory pada Serial Programming ***/
;    1764         SPIMasterKirimData(0x28);
	LDI  R30,LOW(40)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1765         SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));
	LDD  R30,Y+3
	ANDI R31,HIGH(0x0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1766         SPIMasterKirimData((unsigned char) addr_flash_memory_page);
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1767         data_flash_memory=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R16,R30
;    1768         ulang_polling_flash--;
	SUBI R17,1
;    1769         if(data_flash_memory==0xFF)
	CPI  R16,255
	BRNE _0x14D
;    1770         {
;    1771             delay_ms(4.5);
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1772             ulang_polling_flash= 0;
	LDI  R17,LOW(0)
;    1773         }
;    1774     };
_0x14D:
	RJMP _0x14A
_0x14C:
;    1775 }
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	RET
;    1776 #include "fuse.h"
;    1777 #include "extsram/extsram.h"
;    1778 #include "isp/isp.h"
;    1779 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    1780 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    1781 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    1782 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    1783 	#endif
	#endif
;    1784 
;    1785 /*** Operasi Baca Kode Devais ***/
;    1786 void ISPReadDevaisSignature(void)
;    1787 {
_ISPReadDevaisSignature:
;    1788     unsigned char i;
;    1789 
;    1790     /*** Instruksi Read Signature Byte pada Serial Programming ***/
;    1791     //Devais Signature Byte ATmega8535
;    1792     //0x00 0x1E
;    1793     //0x01 0x93
;    1794     //0x02 0x08
;    1795     inisialisasiSPILowSpeed();
	ST   -Y,R17
;	i -> R17
	CALL _inisialisasiSPILowSpeed
;    1796     enterProgrammingMode();
	CALL _enterProgrammingMode
;    1797     for(i=0;i<3;i++)
	LDI  R17,LOW(0)
_0x14F:
	CPI  R17,3
	BRSH _0x150
;    1798     {
;    1799         SPIMasterKirimData(0x30);
	LDI  R30,LOW(48)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1800         SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1801         SPIMasterKirimData(i);
	ST   -Y,R17
	CALL _SPIMasterKirimData
;    1802         devais_signature[i]=SPIMasterKirimData(0x00);
	MOV  R30,R17
	LDI  R31,0
	SUBI R30,LOW(-_devais_signature)
	SBCI R31,HIGH(-_devais_signature)
	PUSH R31
	PUSH R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	POP  R26
	POP  R27
	ST   X,R30
;    1803     }
	SUBI R17,-1
	RJMP _0x14F
_0x150:
;    1804     leaveProgrammingMode();
	CALL _leaveProgrammingMode
;    1805     inisialisasiSPIHighSpeed();
	CALL _inisialisasiSPIHighSpeed
;    1806 }
	RJMP _0x164
;    1807 
;    1808 /****************************************************************************************/
;    1809 //0xD9 -->> unprog clk (int clk), unprog EESAVE, prog BOOTSz1 & BOOTSz1, unprog BOOTRST
;    1810 //0xC9 -->> prog clk (ext clk), unprog EESAVE, prog BOOTSz1 & BOOTSz1, unprog BOOTRST
;    1811 //0xE4 -->> programmed clk 8MHz, programmed SUT0
;    1812 //0xEF -->> unprogrammed clk, programmed SUT0
;    1813 /****************************************************************************************/
;    1814 
;    1815 //************** Operasi Fuse Low Byte **************//
;    1816 /*
;    1817 Konfigurasi Fuse Low Byte
;    1818 Bit 7 -->> BODLEVEL  |  Bit 3 -->> CKSEL3
;    1819 Bit 6 -->> BODEN     |  Bit 2 -->> CKSEL2
;    1820 Bit 5 -->> SUT1      |  Bit 1 -->> CKSEL1
;    1821 Bit 4 -->> SUT0      |  Bit 0 -->> CKSEL1
;    1822 "0" = programmed
;    1823 "1" = unprogrammed
;    1824 */
;    1825 
;    1826 /*** Operasi Baca Fuse Low Byte ***/
;    1827 unsigned char ISPReadLowFuseBits(void)
;    1828 {
_ISPReadLowFuseBits:
;    1829     unsigned char fuse_low_byte;
;    1830 
;    1831     /*** Instruksi Read Fuse Low Bits pada Serial Programming ***/
;    1832     SPIMasterKirimData(0x50);
	ST   -Y,R17
;	fuse_low_byte -> R17
	LDI  R30,LOW(80)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1833     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1834     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1835     fuse_low_byte=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1836     return fuse_low_byte;
	MOV  R30,R17
	RJMP _0x164
;    1837 }
;    1838 
;    1839 /*** Operasi Tulis Fuse Low Byte ***/
;    1840 void ISPWriteLowFuseBits(unsigned char fuse_low_byte)
;    1841 {
_ISPWriteLowFuseBits:
;    1842     /*** Instruksi Write Fuse Low Bits pada Serial Programming ***/
;    1843     SPIMasterKirimData(0xAC);
;	fuse_low_byte -> Y+0
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1844     SPIMasterKirimData(0xA0);
	LDI  R30,LOW(160)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1845     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1846     SPIMasterKirimData(fuse_low_byte);
	LD   R30,Y
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1847     delay_ms(5);
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1848 }
	RJMP _0x163
;    1849 
;    1850 //************** Operasi Fuse High Byte **************//
;    1851 /*
;    1852 Konfigurasi Fuse High Byte
;    1853 Bit 7 -->> @@@@@     |  Bit 3 -->> EESAVE
;    1854 Bit 6 -->> @@@@@     |  Bit 2 -->> BOOTSZ1
;    1855 Bit 5 -->> SPIEN     |  Bit 1 -->> BOOTSZ0
;    1856 Bit 4 -->> CKOPT     |  Bit 0 -->> BOOTRST
;    1857 "0" = programmed
;    1858 "1" = unprogrammed
;    1859 Ket: @@@@@ Tergantung Devais
;    1860 */
;    1861 
;    1862 /*** Operasi Baca Fuse High Byte ***/
;    1863 unsigned char ISPReadHighFuseBits(void)
;    1864 {
_ISPReadHighFuseBits:
;    1865     unsigned char fuse_high_byte;
;    1866 
;    1867     /*** Instruksi Read Fuse High Bits pada Serial Programming ***/
;    1868     SPIMasterKirimData(0x58);
	ST   -Y,R17
;	fuse_high_byte -> R17
	LDI  R30,LOW(88)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1869     SPIMasterKirimData(0x08);
	LDI  R30,LOW(8)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1870     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1871     fuse_high_byte=SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1872     return fuse_high_byte;
	MOV  R30,R17
	RJMP _0x164
;    1873 }
;    1874 
;    1875 /*** Operasi Tulis Fuse High Byte ***/
;    1876 void ISPWriteHighFuseBits(unsigned char fuse_high_byte)
;    1877 {
_ISPWriteHighFuseBits:
;    1878     /*** Instruksi Write Fuse High Bits pada Serial Programming ***/
;    1879     SPIMasterKirimData(0xAC);
;	fuse_high_byte -> Y+0
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1880     SPIMasterKirimData(0xA8);
	LDI  R30,LOW(168)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1881     SPIMasterKirimData(0xFF);
	LDI  R30,LOW(255)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1882     SPIMasterKirimData(fuse_high_byte);
	LD   R30,Y
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1883     delay_ms(5);
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1884 }
	RJMP _0x163
;    1885 
;    1886 unsigned char ISPReadExtendedFuseBits(void)
;    1887 {
_ISPReadExtendedFuseBits:
;    1888     unsigned char extended_fuse_byte;
;    1889 
;    1890     /*** Instruksi Read Extended Fuse Byte pada Serial Programming ***/
;    1891     SPIMasterKirimData(0x50);
	ST   -Y,R17
;	extended_fuse_byte -> R17
	LDI  R30,LOW(80)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1892     SPIMasterKirimData(0x08);
	LDI  R30,LOW(8)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1893     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1894     extended_fuse_byte= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R17,R30
;    1895     return extended_fuse_byte;
	MOV  R30,R17
_0x164:
	LD   R17,Y+
	RET
;    1896 }
;    1897 
;    1898 void ISPWriteExtendedBits(unsigned char fuse_extended_byte)
;    1899 {
_ISPWriteExtendedBits:
;    1900     /*** Instruksi Write Fuse High Bits pada Serial Programming ***/
;    1901     SPIMasterKirimData(0xAC);
;	fuse_extended_byte -> Y+0
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1902     SPIMasterKirimData(0xA4);
	LDI  R30,LOW(164)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1903     SPIMasterKirimData(0xFF);
	LDI  R30,LOW(255)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1904     SPIMasterKirimData(fuse_extended_byte);
	LD   R30,Y
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1905     delay_ms(5);
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1906 }
_0x163:
	ADIW R28,1
	RET
;    1907 #include "isp.h"
;    1908 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    1909 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    1910 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    1911 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    1912 	#endif
	#endif
;    1913 
;    1914 void inisialisasiSPILowSpeed(void)
;    1915 {
_inisialisasiSPILowSpeed:
;    1916     // SPI initialization
;    1917     // SPI Type: Master
;    1918     // SPI Clock Rate: 86,400 kHz
;    1919     // SPI Clock Phase: Cycle Half
;    1920     // SPI Clock Polarity: Low
;    1921     // SPI Data Order: MSB First
;    1922     SPCR=0x53;
	LDI  R30,LOW(83)
	OUT  0xD,R30
;    1923     SPSR=0x00;
	LDI  R30,LOW(0)
	OUT  0xE,R30
;    1924 }
	RET
;    1925 
;    1926 void inisialisasiSPIHighSpeed(void)
;    1927 {
_inisialisasiSPIHighSpeed:
;    1928     // SPI initialization
;    1929     // SPI Type: Master
;    1930     // SPI Clock Rate: 2*172,800 kHz
;    1931     // SPI Clock Phase: Cycle Half
;    1932     // SPI Clock Polarity: Low
;    1933     // SPI Data Order: MSB First
;    1934     SPCR=0x52;
	LDI  R30,LOW(82)
	OUT  0xD,R30
;    1935     SPSR=0x01;
	LDI  R30,LOW(1)
	OUT  0xE,R30
;    1936 }
	RET
;    1937 
;    1938 unsigned char SPIMasterKirimData(unsigned char data)
;    1939 {
_SPIMasterKirimData:
;    1940     //Start Transmission
;    1941     SPDR = data;
;	data -> Y+0
	LD   R30,Y
	OUT  0xF,R30
;    1942     //Tunggu sampai SPI Interrupt Flag set
;    1943     while(!(SPSR & (1<<SPIF)));
_0x151:
	SBIS 0xE,7
	RJMP _0x151
;    1944     return SPDR;
	IN   R30,0xF
	ADIW R28,1
	RET
;    1945 }
;    1946 
;    1947 void enterProgrammingMode(void)
;    1948 {
_enterProgrammingMode:
;    1949     /*** Instruksi Programming Enable pada Serial Programming ***/
;    1950     //Reset harus diberi pulsa positif minimal selama 2 siklus clock CPU setelah pin SCK diset 0
;    1951     SCK=0;
	CBI  0x18,7
;    1952     RESET_SLAVE_NONAKTIF();
	SBI  0x18,4
;    1953     delay_ms(1);
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1954     RESET_SLAVE_AKTIF();
	IN   R30,0x18
	ANDI R30,LOW(0x0)
	OUT  0x18,R30
;    1955     //Menunggu waktu tunda minimal selama 20 ms
;    1956     delay_ms(20);
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1957     SPIMasterKirimData(0xAC);
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1958     SPIMasterKirimData(0x53);
	LDI  R30,LOW(83)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1959     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1960     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1961 }
	RET
;    1962 
;    1963 void chipErase(void)
;    1964 {
_chipErase:
;    1965     SPIMasterKirimData(0xAC);
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1966     SPIMasterKirimData(0x80);
	LDI  R30,LOW(128)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1967     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1968     SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1969     //Menunggu waktu tunda minimal selama 10 ms
;    1970     delay_ms(10);
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;    1971 }
	RET
;    1972 
;    1973 void leaveProgrammingMode(void)
;    1974 {
_leaveProgrammingMode:
;    1975     RESET_SLAVE_NONAKTIF();
	SBI  0x18,4
;    1976     //Indikator Leave Programming Mode yaitu matinya LED reset pada MK Target
;    1977 }
	RET
;    1978 /************************************************************************************************************/
;    1979 unsigned char cekEksistensiDevais(void)
;    1980 {
_cekEksistensiDevais:
;    1981    unsigned char status_eksistensi_devais;
;    1982    unsigned char respon_devais;
;    1983 
;    1984    do
	ST   -Y,R17
	ST   -Y,R16
;	status_eksistensi_devais -> R17
;	respon_devais -> R16
_0x155:
;    1985    {
;    1986        enterProgrammingMode();
	CALL _enterProgrammingMode
;    1987        SPIMasterKirimData(0xAC);
	LDI  R30,LOW(172)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1988        SPIMasterKirimData(0x53);
	LDI  R30,LOW(83)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1989        respon_devais= SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
	MOV  R16,R30
;    1990        SPIMasterKirimData(0x00);
	LDI  R30,LOW(0)
	ST   -Y,R30
	CALL _SPIMasterKirimData
;    1991        if(respon_devais!=0x53) kirimKonfirmasiGAGAL();
	CPI  R16,83
	BREQ _0x157
	CALL _kirimKonfirmasiGAGAL
;    1992        else status_eksistensi_devais= 1;
	RJMP _0x158
_0x157:
	LDI  R17,LOW(1)
;    1993        leaveProgrammingMode();
_0x158:
	CALL _leaveProgrammingMode
;    1994    }
;    1995    while(!status_eksistensi_devais);
	CPI  R17,0
	BREQ _0x155
;    1996    return status_eksistensi_devais;
	MOV  R30,R17
	LD   R16,Y+
	LD   R17,Y+
	RET
;    1997 }
;    1998 #include "timer.h"
;    1999 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    2000 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    2001 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    2002 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    2003 	#endif
	#endif
;    2004 
;    2005 void inisialisasiTimer(void)
;    2006 {
_inisialisasiTimer:
;    2007     // Timer/Counter 0 initialization
;    2008     // Clock source: System Clock
;    2009     // Clock value: 11059,200 kHz
;    2010     // Mode: CTC top=OCR0
;    2011     // OC0 output: Toggle on compare match
;    2012     TCCR0=0x19;
	LDI  R30,LOW(25)
	OUT  0x33,R30
;    2013     TCNT0=0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
;    2014     OCR0=0x04;
	LDI  R30,LOW(4)
	OUT  0x31,R30
;    2015 
;    2016     // Timer/Counter 1 initialization
;    2017     // Clock source: System Clock
;    2018     // Clock value: 1382,400 kHz
;    2019     // Mode: Normal top=FFFFh
;    2020     // OC1A output: Discon.
;    2021     // OC1B output: Discon.
;    2022     // Noise Canceler: Off
;    2023     // Input Capture on Falling Edge
;    2024     // Timer 1 Overflow Interrupt: Off
;    2025     // Input Capture Interrupt: Off
;    2026     // Compare A Match Interrupt: On
;    2027     // Compare B Match Interrupt: Off
;    2028     TCCR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2F,R30
;    2029     TCCR1B=0x02;
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;    2030     TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;    2031     TCNT1L=0x00;
	OUT  0x2C,R30
;    2032     ICR1H=0x00;
	OUT  0x25,R30
;    2033     ICR1L=0x00;
	OUT  0x24,R30
;    2034     OCR1AH=0x6B;
	LDI  R30,LOW(107)
	OUT  0x2B,R30
;    2035     OCR1AL=0xFF;
	LDI  R30,LOW(255)
	OUT  0x2A,R30
;    2036     OCR1BH=0x00;
	LDI  R30,LOW(0)
	OUT  0x29,R30
;    2037     OCR1BL=0x00;
	OUT  0x28,R30
;    2038 
;    2039     // Timer/Counter 2 initialization
;    2040     // Clock source: System Clock
;    2041     // Clock value: 11059,200 kHz
;    2042     // Mode: CTC top=OCR2
;    2043     // OC2 output: Toggle on compare match
;    2044     ASSR=0x00;
	OUT  0x26,R30
;    2045     TCCR2=0x19;
	LDI  R30,LOW(25)
	OUT  0x27,R30
;    2046     TCNT2=0x00;
	LDI  R30,LOW(0)
	OUT  0x23,R30
;    2047     OCR2=0x91;
	LDI  R30,LOW(145)
	OUT  0x22,R30
;    2048 
;    2049     // Timer/Counter 3 initialization
;    2050     // Clock value: Timer 3 Stopped
;    2051     // Mode: Normal top=FFFFh
;    2052     // Noise Canceler: Off
;    2053     // Input Capture on Falling Edge
;    2054     // OC3A output: Discon.
;    2055     // OC3B output: Discon.
;    2056     // Timer 3 Overflow Interrupt: Off
;    2057     // Input Capture Interrupt: Off
;    2058     // Compare A Match Interrupt: Off
;    2059     // Compare B Match Interrupt: Off
;    2060     TCCR3A=0x00;
	LDI  R30,LOW(0)
	STS  139,R30
;    2061     TCCR3B=0x00;
	STS  138,R30
;    2062     TCNT3H=0x00;
	STS  137,R30
;    2063     TCNT3L=0x00;
	STS  136,R30
;    2064     ICR3H=0x00;
	STS  129,R30
;    2065     ICR3L=0x00;
	STS  128,R30
;    2066     OCR3AH=0x00;
	STS  135,R30
;    2067     OCR3AL=0x00;
	STS  134,R30
;    2068     OCR3BH=0x00;
	STS  133,R30
;    2069     OCR3BL=0x00;
	STS  132,R30
;    2070 
;    2071     // Timer(s)/Counter(s) Interrupt(s) initialization
;    2072     TIMSK=0x40;
	LDI  R30,LOW(64)
	OUT  0x39,R30
;    2073     ETIMSK=0x00;
	LDI  R30,LOW(0)
	STS  125,R30
;    2074 }
	RET
;    2075 #include "usart.h"
;    2076 	#ifndef __SLEEP_DEFINED__
	#ifndef __SLEEP_DEFINED__
;    2077 	#define __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
;    2078 	.EQU __se_bit=0x20
	.EQU __se_bit=0x20
;    2079 	.SET power_ctrl_reg=mcucr
	.SET power_ctrl_reg=mcucr
;    2080 	#endif
	#endif
;    2081 
;    2082 void inisialisasiUSART()
;    2083 {
_inisialisasiUSART:
;    2084     // USART0 initialization
;    2085     // Communication Parameters: 8 Data, 2 Stop, No Parity
;    2086     // USART0 Receiver: On
;    2087     // USART0 Transmitter: On
;    2088     // USART0 Mode: Asynchronous
;    2089     // USART0 Baud Rate: 2400
;    2090     UCSR0A=0x00;
	LDI  R30,LOW(0)
	OUT  0xB,R30
;    2091     UCSR0B=0x98;
	LDI  R30,LOW(152)
	OUT  0xA,R30
;    2092     UCSR0C=0x8E;
	LDI  R30,LOW(142)
	OUT  0x20,R30
;    2093     UBRR0H=0x01;
	LDI  R30,LOW(1)
	OUT  0x20,R30
;    2094     UBRR0L=0x1F;
	LDI  R30,LOW(31)
	OUT  0x9,R30
;    2095 
;    2096     // USART1 initialization
;    2097     // Communication Parameters: 8 Data, 2 Stop, No Parity
;    2098     // USART1 Receiver: Off
;    2099     // USART1 Transmitter: On
;    2100     // USART1 Mode: Asynchronous
;    2101     // USART1 Baud Rate: 2400
;    2102     UCSR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2,R30
;    2103     UCSR1B=0x08;
	LDI  R30,LOW(8)
	OUT  0x1,R30
;    2104     UCSR1C=0x8E;
	LDI  R30,LOW(142)
	OUT  0x3C,R30
;    2105     UBRR1H=0x01;
	LDI  R30,LOW(1)
	OUT  0x3C,R30
;    2106     UBRR1L=0x1F;
	LDI  R30,LOW(31)
	OUT  0x0,R30
;    2107 }
	RET
;    2108 
;    2109 void kirimDataSerialIR(unsigned char data)
;    2110 {
_kirimDataSerialIR:
;    2111     //Wait for empty transmit buffer
;    2112     while (!(UCSR0A & (1<<UDRE)));
;	data -> Y+0
_0x159:
	SBIS 0xB,5
	RJMP _0x159
;    2113     UDR0=data;
	LD   R30,Y
	OUT  0xC,R30
;    2114 }
	RJMP _0x162
;    2115 
;    2116 //Pengiriman data serial untuk debugging program
;    2117 void kirimDataSerial(unsigned char data)
;    2118 {
_kirimDataSerial:
;    2119     //Wait for empty transmit buffer
;    2120     while (!(UCSR1A & (1<<UDRE)));
;	data -> Y+0
_0x15C:
	SBIS 0x2,5
	RJMP _0x15C
;    2121     UDR1=data;
	LD   R30,Y
	OUT  0x3,R30
;    2122 }
_0x162:
	ADIW R28,1
	RET

	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.SET power_ctrl_reg=mcucr
	#endif

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0xACD
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__LSLW4:
	LSL  R30
	ROL  R31
__LSLW3:
	LSL  R30
	ROL  R31
__LSLW2:
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
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
