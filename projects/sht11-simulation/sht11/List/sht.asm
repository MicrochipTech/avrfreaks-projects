
;CodeVisionAVR C Compiler V2.03.4 Standard
;(C) Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8535
;Program type           : Application
;Clock frequency        : 11,059200 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External RAM size      : 0
;Data Stack size        : 128 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : Yes
;char is unsigned       : Yes
;global const stored in FLASH  : No
;8 bit enums            : Yes
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8535
	#pragma AVRPART MEMORY PROG_FLASH 8192
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 512
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x60

	.LISTMAC
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

	.MACRO __ANDD2N
	ANDI R26,LOW(@0)
	ANDI R27,HIGH(@0)
	ANDI R24,BYTE3(@0)
	ANDI R25,BYTE4(@0)
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

	.MACRO __ORD2N
	ORI  R26,LOW(@0)
	ORI  R27,HIGH(@0)
	ORI  R24,BYTE3(@0)
	ORI  R25,BYTE4(@0)
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

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRD
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

;NAME DEFINITIONS FOR GLOBAL VARIABLES ALLOCATED TO REGISTERS
	.DEF _TimeOut=R5
	.DEF _AckBit=R4
	.DEF _DataTempSHT=R6
	.DEF _DataRHSHT=R8
	.DEF _DataRead=R10
	.DEF __lcd_x=R13
	.DEF __lcd_y=R12

	.CSEG
	.ORG 0x00

;INTERRUPT VECTORS
	RJMP __RESET
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00

_tbl10_G100:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G100:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0

_0x0:
	.DB  0x20,0x52,0x48,0x20,0x20,0x54,0x65,0x6D
	.DB  0x70,0x0,0x25,0x30,0x2E,0x32,0x66,0x20
	.DB  0x25,0x30,0x2E,0x32,0x66,0x0
_0x2020003:
	.DB  0x80,0xC0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G101
	.DW  _0x2020003*2

_0xFFFFFFFF:
	.DW  0

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
	LDI  R24,(14-2)+1
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

	.CSEG
;/*****************************************************
;This program was produced by the
;CodeWizardAVR V2.03.4 Standard
;Automatic Program Generator
;© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;
;Project :
;Version :
;Date    : 30/03/2013
;Author  :
;Company :
;Comments:
;
;
;Chip type           : ATmega8535
;Program type        : Application
;Clock frequency     : 11,059200 MHz
;Memory model        : Small
;External RAM size   : 0
;Data Stack size     : 128
;*****************************************************/
;
;#include <mega8535.h>
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
;#include <stdio.h>
;#include <delay.h>
;
;// Alphanumeric LCD Module functions
;#asm
   .equ __lcd_port=0x15 ;PORTC
; 0000 001F #endasm
;#include <lcd.h>
;sfrb PORTA=0x1b;
;sfrb PINA=0x19;
;#define SDAOut PORTA.0
;#define SDAIn PINA.0
;#define SCLK PORTA.1
;#define out PORTB.0
;
;// Declare your global variables here
;unsigned char TimeOut,AckBit,buf[33];  //Ackbit : '0' (ACK), '1' (NOACK)
;unsigned int DataTempSHT,DataRHSHT,DataRead;
;float RH,Temp;
;
;/*Membuat kondisi "start" ke SHT11 ("Transmission Start" sequence)
;          _____         _____
; ShtData       |_______|
;              ___     ___
; ShtClock ___|   |___|   |___
; Clock         1       2         */
;
; void StartSignal (void)
; 0000 0035 {

	.CSEG
_StartSignal:
; 0000 0036 unsigned char DDRATemp;
; 0000 0037     DDRATemp = DDRA;
	ST   -Y,R17
;	DDRATemp -> R17
	IN   R17,26
; 0000 0038     DDRA |= 0x01;       // PortA.0 sbg Output
	RCALL SUBOPT_0x0
; 0000 0039     SDAOut = 1;
; 0000 003A     SCLK = 0;
; 0000 003B     SCLK = 1;           //Clock pertama
	SBI  0x1B,1
; 0000 003C     SDAOut = 0;
	CBI  0x1B,0
; 0000 003D     SCLK = 0;
	CBI  0x1B,1
; 0000 003E     SCLK = 1;           //Clock kedua
	SBI  0x1B,1
; 0000 003F     SDAOut = 1;
	SBI  0x1B,0
; 0000 0040     SCLK = 0;           //Pin Clock = '0'
	CBI  0x1B,1
; 0000 0041     DDRA = DDRATemp;
	OUT  0x1A,R17
; 0000 0042 }
	LD   R17,Y+
	RET
;
;//Reset komunikasi: 9 clock cyle dengan ShtData '1', lalu kondisi start
;void ResetSHT (void)
; 0000 0046 {
_ResetSHT:
; 0000 0047 unsigned char i,DDRATemp;
; 0000 0048     DDRATemp = DDRA;
	RCALL __SAVELOCR2
;	i -> R17
;	DDRATemp -> R16
	IN   R16,26
; 0000 0049     DDRA |= 0x01;
	RCALL SUBOPT_0x0
; 0000 004A     SDAOut = 1;
; 0000 004B     SCLK = 0;
; 0000 004C     for (i=0; i<=8; i++)
	LDI  R17,LOW(0)
_0x18:
	CPI  R17,9
	BRSH _0x19
; 0000 004D       {
; 0000 004E         SCLK = 1;     //Kirim Data (ShtClock rising edge), 9 kali
	SBI  0x1B,1
; 0000 004F         SCLK = 0;
	CBI  0x1B,1
; 0000 0050       }
	SUBI R17,-1
	RJMP _0x18
_0x19:
; 0000 0051     StartSignal();        //Transmission Start
	RCALL _StartSignal
; 0000 0052     DDRA = DDRATemp;
	RJMP _0x2080004
; 0000 0053 }
;
;//Tunggu sampai SHT11 selesai melakukan pengukuran (pin Data = '0')
;//Timeout pengukuran sekitar 1/4 detik (TimeOut = '0' --> measure OK)
;void SHTWait (void)
; 0000 0058 {
_SHTWait:
; 0000 0059     unsigned char i,DDRATemp;
; 0000 005A     DDRATemp = DDRA;
	RCALL SUBOPT_0x1
;	i -> R17
;	DDRATemp -> R16
; 0000 005B     DDRA |= 0x01;
; 0000 005C     SDAOut=1;          //Pin ShtData sebagai input
	RCALL SUBOPT_0x2
; 0000 005D     DDRA &= 0xFE;
; 0000 005E     for (i=0; i<250; i++)
_0x21:
	CPI  R17,250
	BRSH _0x22
; 0000 005F       {
; 0000 0060         TimeOut=SDAIn;        //Jika pin ShtData = '0' --> pengukuran selesai
	RCALL SUBOPT_0x3
	MOV  R5,R30
; 0000 0061         if (TimeOut==0) goto ExitSHT_Wait;
	TST  R5
	BREQ _0x24
; 0000 0062         delay_ms(1);
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	RCALL SUBOPT_0x4
; 0000 0063       }
	SUBI R17,-1
	RJMP _0x21
_0x22:
; 0000 0064     ExitSHT_Wait:
_0x24:
; 0000 0065     DDRA = DDRATemp;
	RJMP _0x2080004
; 0000 0066 }
;
;// Transmit Data dan ambil bit Acknowledge
;void SHTWriteByte (unsigned char data)
; 0000 006A {
_SHTWriteByte:
; 0000 006B     unsigned char i,DDRATemp;
; 0000 006C     DDRATemp = DDRA;
	RCALL SUBOPT_0x1
;	data -> Y+2
;	i -> R17
;	DDRATemp -> R16
; 0000 006D     DDRA |= 0x01;
; 0000 006E     for (i=0; i<8; i++)
	LDI  R17,LOW(0)
_0x26:
	CPI  R17,8
	BRSH _0x27
; 0000 006F       {
; 0000 0070         if ((data>>7)==1) SDAOut = 1;    //Kirim MSB first
	RCALL SUBOPT_0x5
	RCALL __ASRW3
	RCALL __ASRW4
	SBIW R30,1
	BRNE _0x28
	SBI  0x1B,0
; 0000 0071         else SDAOut = 0;
	RJMP _0x2B
_0x28:
	CBI  0x1B,0
; 0000 0072         SCLK = 1;                 //Kirim Data (ShtClock rising edge)
_0x2B:
	SBI  0x1B,1
; 0000 0073         SCLK = 0;
	CBI  0x1B,1
; 0000 0074         data <<= 1;             // geser data kekiri 1 bit
	RCALL SUBOPT_0x5
	LSL  R30
	ROL  R31
	STD  Y+2,R30
; 0000 0075       }
	SUBI R17,-1
	RJMP _0x26
_0x27:
; 0000 0076     SDAOut = 1;                  //Pin ShtData sebagai input
	SBI  0x1B,0
; 0000 0077     SCLK = 1;
	SBI  0x1B,1
; 0000 0078     DDRA &= 0xFE;
	RCALL SUBOPT_0x6
	ANDI R30,LOW(0xFE)
	ANDI R31,HIGH(0xFE)
	OUT  0x1A,R30
; 0000 0079     AckBit = SDAIn;             //Ambil sinyal acknowledge
	RCALL SUBOPT_0x3
	MOV  R4,R30
; 0000 007A     SCLK = 0;
	CBI  0x1B,1
; 0000 007B     DDRA = DDRATemp;
	OUT  0x1A,R16
; 0000 007C }
	RCALL __LOADLOCR2
	RJMP _0x2080003
;
;//Receive Data dan kirim bit "AckBit" ('0' untuk ACK atau '1' untuk NACK)
;void SHTReadByte (void)
; 0000 0080 {
_SHTReadByte:
; 0000 0081     unsigned char i,DDRATemp;
; 0000 0082     DataRead = 0x00;
	RCALL __SAVELOCR2
;	i -> R17
;	DDRATemp -> R16
	CLR  R10
	CLR  R11
; 0000 0083     DDRATemp = DDRA;
	IN   R16,26
; 0000 0084     DDRA |= 0x01;
	RCALL SUBOPT_0x6
	ORI  R30,1
	OUT  0x1A,R30
; 0000 0085     SDAOut = 1;          //Pin ShtData sebagai input
	RCALL SUBOPT_0x2
; 0000 0086     DDRA &= 0xFE;
; 0000 0087     for (i=0; i<8; i++)
_0x3B:
	CPI  R17,8
	BRSH _0x3C
; 0000 0088       {
; 0000 0089         DataRead<<=1;
	LSL  R10
	ROL  R11
; 0000 008A         SCLK = 1;
	SBI  0x1B,1
; 0000 008B         DataRead |= SDAIn;  //Ambil Data (MSB first)
	RCALL SUBOPT_0x3
	RCALL SUBOPT_0x7
	__ORWRR 10,11,30,31
; 0000 008C         SCLK = 0;
	CBI  0x1B,1
; 0000 008D       }
	SUBI R17,-1
	RJMP _0x3B
_0x3C:
; 0000 008E     DDRA |= 0x01;
	RCALL SUBOPT_0x6
	ORI  R30,1
	OUT  0x1A,R30
; 0000 008F     if (AckBit==1) SDAOut = 1;   //Kirim Noacknowledge
	LDI  R30,LOW(1)
	CP   R30,R4
	BRNE _0x41
	SBI  0x1B,0
; 0000 0090     else SDAOut = 0;             //Kirim Acknowledge
	RJMP _0x44
_0x41:
	CBI  0x1B,0
; 0000 0091     SCLK = 1;
_0x44:
	SBI  0x1B,1
; 0000 0092     SCLK = 0;
	CBI  0x1B,1
; 0000 0093     SDAOut = 1;          //Pin ShtData sebagai input
	SBI  0x1B,0
; 0000 0094     DDRA = DDRATemp;
_0x2080004:
	OUT  0x1A,R16
; 0000 0095 }
	RCALL __LOADLOCR2P
	RET
;
;// Pembacaan Temperature dari SHt11
;void SHTReadTemp (void)
; 0000 0099 {
_SHTReadTemp:
; 0000 009A     StartSignal();
	RCALL _StartSignal
; 0000 009B     SHTWriteByte(0x03);    		//Command Measure Temperature
	LDI  R30,LOW(3)
	ST   -Y,R30
	RCALL _SHTWriteByte
; 0000 009C     if (AckBit==0)
	TST  R4
	BRNE _0x4D
; 0000 009D       {
; 0000 009E         SHTWait();         		//Tunggu sampai pengukuran selesai
	RCALL _SHTWait
; 0000 009F         if (TimeOut==0)
	TST  R5
	BRNE _0x4E
; 0000 00A0           {
; 0000 00A1            AckBit=0;                    //Kirim ACK untuk menerima byte berikutnya
	CLR  R4
; 0000 00A2             SHTReadByte();              // Ambli Byte MSB
	RCALL _SHTReadByte
; 0000 00A3             DataTempSHT = DataRead;
	MOVW R6,R10
; 0000 00A4             DataTempSHT <<= 8;
	MOV  R7,R6
	CLR  R6
; 0000 00A5             AckBit=1;                   //Kirim NACK untuk mengakhiri pengambilan data
	LDI  R30,LOW(1)
	MOV  R4,R30
; 0000 00A6             SHTReadByte();
	RCALL _SHTReadByte
; 0000 00A7             DataTempSHT |= DataRead;   //Ambil byte LSB
	__ORWRR 6,7,10,11
; 0000 00A8 
; 0000 00A9             DataRead = DataTempSHT;
	MOVW R10,R6
; 0000 00AA           }
; 0000 00AB       }
_0x4E:
; 0000 00AC }
_0x4D:
	RET
;
;void SHTReadHumidity (void)
; 0000 00AF {
_SHTReadHumidity:
; 0000 00B0     StartSignal();
	RCALL _StartSignal
; 0000 00B1     SHTWriteByte(0x05);   //Command Measure Humidity
	LDI  R30,LOW(5)
	ST   -Y,R30
	RCALL _SHTWriteByte
; 0000 00B2     if (AckBit==0)
	TST  R4
	BRNE _0x4F
; 0000 00B3       {
; 0000 00B4         SHTWait();
	RCALL _SHTWait
; 0000 00B5         if (TimeOut==0)
	TST  R5
	BRNE _0x50
; 0000 00B6           {
; 0000 00B7             AckBit=0;
	CLR  R4
; 0000 00B8             SHTReadByte();
	RCALL _SHTReadByte
; 0000 00B9             DataRHSHT = DataRead;
	MOVW R8,R10
; 0000 00BA             DataRHSHT <<= 8;
	MOV  R9,R8
	CLR  R8
; 0000 00BB             AckBit=1;
	LDI  R30,LOW(1)
	MOV  R4,R30
; 0000 00BC             SHTReadByte();
	RCALL _SHTReadByte
; 0000 00BD             DataRHSHT |= DataRead;
	__ORWRR 8,9,10,11
; 0000 00BE 
; 0000 00BF             DataRead = DataRHSHT;
	MOVW R10,R8
; 0000 00C0           }
; 0000 00C1       }
_0x50:
; 0000 00C2 }
_0x4F:
	RET
;
;
;void main(void)
; 0000 00C6 {
_main:
; 0000 00C7 // Declare your local variables here
; 0000 00C8 
; 0000 00C9 // Input/Output Ports initialization
; 0000 00CA // Port A initialization
; 0000 00CB // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 00CC // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 00CD PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
; 0000 00CE DDRA=0x02;
	LDI  R30,LOW(2)
	OUT  0x1A,R30
; 0000 00CF 
; 0000 00D0 // Port B initialization
; 0000 00D1 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 00D2 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 00D3 PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
; 0000 00D4 DDRB=0x08;
	LDI  R30,LOW(8)
	OUT  0x17,R30
; 0000 00D5 
; 0000 00D6 // Port C initialization
; 0000 00D7 // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out
; 0000 00D8 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0
; 0000 00D9 PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
; 0000 00DA DDRC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x14,R30
; 0000 00DB 
; 0000 00DC // Port D initialization
; 0000 00DD // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 00DE // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 00DF PORTD=0x00;
	LDI  R30,LOW(0)
	OUT  0x12,R30
; 0000 00E0 DDRD=0x04;
	LDI  R30,LOW(4)
	OUT  0x11,R30
; 0000 00E1 
; 0000 00E2 // Timer/Counter 0 initialization
; 0000 00E3 // Clock source: System Clock
; 0000 00E4 // Clock value: Timer 0 Stopped
; 0000 00E5 // Mode: Normal top=FFh
; 0000 00E6 // OC0 output: Disconnected
; 0000 00E7 TCCR0=0x71;
	LDI  R30,LOW(113)
	OUT  0x33,R30
; 0000 00E8 TCNT0=0x00;
	LDI  R30,LOW(0)
	OUT  0x32,R30
; 0000 00E9 OCR0=0x00;
	OUT  0x3C,R30
; 0000 00EA 
; 0000 00EB // Timer/Counter 1 initialization
; 0000 00EC // Clock source: System Clock
; 0000 00ED // Clock value: Timer 1 Stopped
; 0000 00EE // Mode: Normal top=FFFFh
; 0000 00EF // OC1A output: Discon.
; 0000 00F0 // OC1B output: Discon.
; 0000 00F1 // Noise Canceler: Off
; 0000 00F2 // Input Capture on Falling Edge
; 0000 00F3 // Timer 1 Overflow Interrupt: Off
; 0000 00F4 // Input Capture Interrupt: Off
; 0000 00F5 // Compare A Match Interrupt: Off
; 0000 00F6 // Compare B Match Interrupt: Off
; 0000 00F7 TCCR1A=0x00;
	OUT  0x2F,R30
; 0000 00F8 TCCR1B=0x00;
	OUT  0x2E,R30
; 0000 00F9 TCNT1H=0x00;
	OUT  0x2D,R30
; 0000 00FA TCNT1L=0x00;
	OUT  0x2C,R30
; 0000 00FB ICR1H=0x00;
	OUT  0x27,R30
; 0000 00FC ICR1L=0x00;
	OUT  0x26,R30
; 0000 00FD OCR1AH=0x00;
	OUT  0x2B,R30
; 0000 00FE OCR1AL=0x00;
	OUT  0x2A,R30
; 0000 00FF OCR1BH=0x00;
	OUT  0x29,R30
; 0000 0100 OCR1BL=0x00;
	OUT  0x28,R30
; 0000 0101 
; 0000 0102 // Timer/Counter 2 initialization
; 0000 0103 // Clock source: System Clock
; 0000 0104 // Clock value: Timer 2 Stopped
; 0000 0105 // Mode: Normal top=FFh
; 0000 0106 // OC2 output: Disconnected
; 0000 0107 ASSR=0x00;
	OUT  0x22,R30
; 0000 0108 TCCR2=0x00;
	OUT  0x25,R30
; 0000 0109 TCNT2=0x00;
	OUT  0x24,R30
; 0000 010A OCR2=0x00;
	OUT  0x23,R30
; 0000 010B 
; 0000 010C // External Interrupt(s) initialization
; 0000 010D // INT0: Off
; 0000 010E // INT1: Off
; 0000 010F // INT2: Off
; 0000 0110 MCUCR=0x00;
	OUT  0x35,R30
; 0000 0111 MCUCSR=0x00;
	OUT  0x34,R30
; 0000 0112 
; 0000 0113 // Timer(s)/Counter(s) Interrupt(s) initialization
; 0000 0114 TIMSK=0x00;
	OUT  0x39,R30
; 0000 0115 
; 0000 0116 // Analog Comparator initialization
; 0000 0117 // Analog Comparator: Off
; 0000 0118 // Analog Comparator Input Capture by Timer/Counter 1: Off
; 0000 0119 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
; 0000 011A SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
; 0000 011B 
; 0000 011C // LCD module initialization
; 0000 011D lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	RCALL _lcd_init
; 0000 011E delay_ms(1000);
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RCALL SUBOPT_0x4
; 0000 011F ResetSHT();
	RCALL _ResetSHT
; 0000 0120 
; 0000 0121 while (1)
_0x51:
; 0000 0122       {
; 0000 0123       // Place your code here
; 0000 0124       SHTReadHumidity();
	RCALL _SHTReadHumidity
; 0000 0125       RH=((float)(DataRead*0.0405)-(DataRead*DataRead*0.0000028)-4);
	MOVW R30,R10
	CLR  R22
	CLR  R23
	RCALL __CDF1
	__GETD2N 0x3D25E354
	RCALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	MOVW R30,R10
	MOVW R26,R10
	RCALL __MULW12U
	CLR  R22
	CLR  R23
	RCALL __CDF1
	__GETD2N 0x363BE7A2
	RCALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __SWAPD12
	RCALL __SUBF12
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x40800000
	RCALL __SWAPD12
	RCALL __SUBF12
	STS  _RH,R30
	STS  _RH+1,R31
	STS  _RH+2,R22
	STS  _RH+3,R23
; 0000 0126       ResetSHT();
	RCALL _ResetSHT
; 0000 0127       SHTReadTemp();
	RCALL _SHTReadTemp
; 0000 0128       Temp=((float)(DataRead-4000))/100;
	MOVW R30,R10
	SUBI R30,LOW(4000)
	SBCI R31,HIGH(4000)
	RCALL __CWD1
	RCALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x42C80000
	RCALL __DIVF21
	STS  _Temp,R30
	STS  _Temp+1,R31
	STS  _Temp+2,R22
	STS  _Temp+3,R23
; 0000 0129       ResetSHT();
	RCALL _ResetSHT
; 0000 012A       lcd_clear();
	RCALL _lcd_clear
; 0000 012B       lcd_gotoxy (0,0);
	RCALL SUBOPT_0x8
	RCALL SUBOPT_0x8
	RCALL _lcd_gotoxy
; 0000 012C       lcd_putsf(" RH  Temp");
	__POINTW1FN _0x0,0
	RCALL SUBOPT_0x9
	RCALL _lcd_putsf
; 0000 012D       lcd_gotoxy(0,1);
	RCALL SUBOPT_0x8
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_gotoxy
; 0000 012E       sprintf(buf,"%0.2f %0.2f",RH,Temp);
	LDI  R30,LOW(_buf)
	LDI  R31,HIGH(_buf)
	RCALL SUBOPT_0x9
	__POINTW1FN _0x0,10
	RCALL SUBOPT_0x9
	LDS  R30,_RH
	LDS  R31,_RH+1
	LDS  R22,_RH+2
	LDS  R23,_RH+3
	RCALL __PUTPARD1
	LDS  R30,_Temp
	LDS  R31,_Temp+1
	LDS  R22,_Temp+2
	LDS  R23,_Temp+3
	RCALL __PUTPARD1
	LDI  R24,8
	RCALL _sprintf
	ADIW R28,12
; 0000 012F       lcd_puts(buf);
	LDI  R30,LOW(_buf)
	LDI  R31,HIGH(_buf)
	RCALL SUBOPT_0x9
	RCALL _lcd_puts
; 0000 0130       delay_ms(1000);
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RCALL SUBOPT_0x4
; 0000 0131 
; 0000 0132       };
	RJMP _0x51
; 0000 0133 }
_0x54:
	RJMP _0x54
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

	.CSEG
_putchar:
     sbis usr,udre
     rjmp _putchar
     ld   r30,y
     out  udr,r30
	RJMP _0x2080001
__put_G100:
	RCALL __SAVELOCR2
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	RCALL __GETW1P
	SBIW R30,0
	BREQ _0x2000010
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	RCALL __GETW1P
	MOVW R16,R30
	SBIW R30,0
	BREQ _0x2000012
	__CPWRN 16,17,2
	BRLO _0x2000013
	MOVW R30,R16
	SBIW R30,1
	MOVW R16,R30
	ST   X+,R30
	ST   X,R31
_0x2000012:
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
_0x2000013:
	RJMP _0x2000014
_0x2000010:
	LDD  R30,Y+6
	ST   -Y,R30
	RCALL _putchar
_0x2000014:
	RCALL __LOADLOCR2
	ADIW R28,7
	RET
__print_G100:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R17,0
_0x2000015:
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
	RJMP _0x2000017
	MOV  R30,R17
	RCALL SUBOPT_0x7
	SBIW R30,0
	BRNE _0x200001B
	CPI  R18,37
	BRNE _0x200001C
	LDI  R17,LOW(1)
	RJMP _0x200001D
_0x200001C:
	RCALL SUBOPT_0xA
_0x200001D:
	RJMP _0x200001A
_0x200001B:
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BRNE _0x200001E
	CPI  R18,37
	BRNE _0x200001F
	RCALL SUBOPT_0xA
	RJMP _0x20000BC
_0x200001F:
	LDI  R17,LOW(2)
	LDI  R20,LOW(0)
	LDI  R16,LOW(0)
	CPI  R18,45
	BRNE _0x2000020
	LDI  R16,LOW(1)
	RJMP _0x200001A
_0x2000020:
	CPI  R18,43
	BRNE _0x2000021
	LDI  R20,LOW(43)
	RJMP _0x200001A
_0x2000021:
	CPI  R18,32
	BRNE _0x2000022
	LDI  R20,LOW(32)
	RJMP _0x200001A
_0x2000022:
	RJMP _0x2000023
_0x200001E:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BRNE _0x2000024
_0x2000023:
	LDI  R21,LOW(0)
	LDI  R17,LOW(3)
	CPI  R18,48
	BRNE _0x2000025
	RCALL SUBOPT_0xB
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	RCALL SUBOPT_0xC
	RJMP _0x200001A
_0x2000025:
	RJMP _0x2000026
_0x2000024:
	CPI  R30,LOW(0x3)
	LDI  R26,HIGH(0x3)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0x200001A
_0x2000026:
	CPI  R18,48
	BRLO _0x2000029
	CPI  R18,58
	BRLO _0x200002A
_0x2000029:
	RJMP _0x2000028
_0x200002A:
	MOV  R26,R21
	RCALL SUBOPT_0xD
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	MULS R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R22,R21
	CLR  R23
	MOV  R26,R18
	RCALL SUBOPT_0xD
	LDI  R30,LOW(48)
	LDI  R31,HIGH(48)
	RCALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	MOVW R26,R22
	ADD  R30,R26
	MOV  R21,R30
	RJMP _0x200001A
_0x2000028:
	MOV  R30,R18
	RCALL SUBOPT_0x7
	CPI  R30,LOW(0x63)
	LDI  R26,HIGH(0x63)
	CPC  R31,R26
	BRNE _0x200002E
	RCALL SUBOPT_0xE
	RCALL SUBOPT_0xF
	RCALL SUBOPT_0xE
	LDD  R26,Z+4
	ST   -Y,R26
	RCALL SUBOPT_0x10
	RJMP _0x200002F
_0x200002E:
	CPI  R30,LOW(0x73)
	LDI  R26,HIGH(0x73)
	CPC  R31,R26
	BRNE _0x2000031
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x12
	RCALL _strlen
	MOV  R17,R30
	RJMP _0x2000032
_0x2000031:
	CPI  R30,LOW(0x70)
	LDI  R26,HIGH(0x70)
	CPC  R31,R26
	BRNE _0x2000034
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x12
	RCALL _strlenf
	MOV  R17,R30
	RCALL SUBOPT_0xB
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	RCALL SUBOPT_0xC
_0x2000032:
	RCALL SUBOPT_0xB
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xB
	LDI  R30,LOW(65407)
	LDI  R31,HIGH(65407)
	RCALL SUBOPT_0x13
	LDI  R19,LOW(0)
	RJMP _0x2000035
_0x2000034:
	CPI  R30,LOW(0x64)
	LDI  R26,HIGH(0x64)
	CPC  R31,R26
	BREQ _0x2000038
	CPI  R30,LOW(0x69)
	LDI  R26,HIGH(0x69)
	CPC  R31,R26
	BRNE _0x2000039
_0x2000038:
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x14
	RCALL SUBOPT_0xC
	RJMP _0x200003A
_0x2000039:
	CPI  R30,LOW(0x75)
	LDI  R26,HIGH(0x75)
	CPC  R31,R26
	BRNE _0x200003B
_0x200003A:
	LDI  R30,LOW(_tbl10_G100*2)
	LDI  R31,HIGH(_tbl10_G100*2)
	RCALL SUBOPT_0x15
	LDI  R17,LOW(5)
	RJMP _0x200003C
_0x200003B:
	CPI  R30,LOW(0x58)
	LDI  R26,HIGH(0x58)
	CPC  R31,R26
	BRNE _0x200003E
	RCALL SUBOPT_0xB
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	RCALL SUBOPT_0xC
	RJMP _0x200003F
_0x200003E:
	CPI  R30,LOW(0x78)
	LDI  R26,HIGH(0x78)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0x2000070
_0x200003F:
	LDI  R30,LOW(_tbl16_G100*2)
	LDI  R31,HIGH(_tbl16_G100*2)
	RCALL SUBOPT_0x15
	LDI  R17,LOW(4)
_0x200003C:
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x16
	BREQ _0x2000041
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x17
	LDD  R26,Y+11
	TST  R26
	BRPL _0x2000042
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R20,LOW(45)
_0x2000042:
	CPI  R20,0
	BREQ _0x2000043
	SUBI R17,-LOW(1)
	RJMP _0x2000044
_0x2000043:
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x18
_0x2000044:
	RJMP _0x2000045
_0x2000041:
	RCALL SUBOPT_0x11
	RCALL SUBOPT_0x17
_0x2000045:
_0x2000035:
	RCALL SUBOPT_0x19
	BRNE _0x2000046
_0x2000047:
	CP   R17,R21
	BRSH _0x2000049
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x1A
	BREQ _0x200004A
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x16
	BREQ _0x200004B
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x18
	MOV  R18,R20
	SUBI R17,LOW(1)
	RJMP _0x200004C
_0x200004B:
	LDI  R18,LOW(48)
_0x200004C:
	RJMP _0x200004D
_0x200004A:
	LDI  R18,LOW(32)
_0x200004D:
	RCALL SUBOPT_0xA
	SUBI R21,LOW(1)
	RJMP _0x2000047
_0x2000049:
_0x2000046:
	MOV  R19,R17
	RCALL SUBOPT_0xB
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	RCALL SUBOPT_0x1B
	BREQ _0x200004E
_0x200004F:
	CPI  R19,0
	BREQ _0x2000051
	RCALL SUBOPT_0xB
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	RCALL SUBOPT_0x1B
	BREQ _0x2000052
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	RCALL SUBOPT_0x15
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x20000BD
_0x2000052:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0x20000BD:
	ST   -Y,R30
	RCALL SUBOPT_0x10
	CPI  R21,0
	BREQ _0x2000054
	SUBI R21,LOW(1)
_0x2000054:
	SUBI R19,LOW(1)
	RJMP _0x200004F
_0x2000051:
	RJMP _0x2000055
_0x200004E:
_0x2000057:
	LDI  R18,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	RCALL __GETW1PF
	STD  Y+8,R30
	STD  Y+8+1,R31
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	RCALL SUBOPT_0x15
_0x2000059:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0x200005B
	SUBI R18,-LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	SUB  R30,R26
	SBC  R31,R27
	STD  Y+10,R30
	STD  Y+10+1,R31
	RJMP _0x2000059
_0x200005B:
	CPI  R18,58
	BRLO _0x200005C
	RCALL SUBOPT_0xB
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	RCALL SUBOPT_0x1B
	BREQ _0x200005D
	MOV  R30,R18
	RCALL SUBOPT_0x7
	ADIW R30,7
	RJMP _0x20000BE
_0x200005D:
	MOV  R30,R18
	RCALL SUBOPT_0x7
	ADIW R30,39
_0x20000BE:
	MOV  R18,R30
_0x200005C:
	RCALL SUBOPT_0xB
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	RCALL SUBOPT_0x1B
	BRNE _0x2000060
	CPI  R18,49
	BRSH _0x2000062
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0x2000061
_0x2000062:
	RJMP _0x20000BF
_0x2000061:
	CP   R21,R19
	BRLO _0x2000066
	RCALL SUBOPT_0x19
	BREQ _0x2000067
_0x2000066:
	RJMP _0x2000065
_0x2000067:
	LDI  R18,LOW(32)
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x1A
	BREQ _0x2000068
	LDI  R18,LOW(48)
_0x20000BF:
	MOV  R26,R16
	RCALL SUBOPT_0xD
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x16
	BREQ _0x2000069
	RCALL SUBOPT_0xB
	RCALL SUBOPT_0x18
	ST   -Y,R20
	RCALL SUBOPT_0x10
	CPI  R21,0
	BREQ _0x200006A
	SUBI R21,LOW(1)
_0x200006A:
_0x2000069:
_0x2000068:
_0x2000060:
	RCALL SUBOPT_0xA
	CPI  R21,0
	BREQ _0x200006B
	SUBI R21,LOW(1)
_0x200006B:
_0x2000065:
	SUBI R19,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0x2000058
	RJMP _0x2000057
_0x2000058:
_0x2000055:
	RCALL SUBOPT_0x19
	BREQ _0x200006C
_0x200006D:
	CPI  R21,0
	BREQ _0x200006F
	SUBI R21,LOW(1)
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL SUBOPT_0x10
	RJMP _0x200006D
_0x200006F:
_0x200006C:
_0x2000070:
_0x200002F:
_0x20000BC:
	LDI  R17,LOW(0)
_0x200001A:
	RJMP _0x2000015
_0x2000017:
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
	RCALL SUBOPT_0x9
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	RCALL SUBOPT_0x9
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	RCALL SUBOPT_0x9
	RCALL __print_G100
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

	.CSEG
__lcd_delay_G101:
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
	RCALL __lcd_delay_G101
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G101
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G101
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G101
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G101:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G101
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G101
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G101
    ld    r26,y
    swap  r26
	RCALL __lcd_write_nibble_G101
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RJMP _0x2080001
__lcd_read_nibble_G101:
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G101
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G101
    andi  r30,0xf0
	RET
_lcd_read_byte0_G101:
	RCALL __lcd_delay_G101
	RCALL __lcd_read_nibble_G101
    mov   r26,r30
	RCALL __lcd_read_nibble_G101
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	RCALL __lcd_ready
	LD   R30,Y
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-__base_y_G101)
	SBCI R31,HIGH(-__base_y_G101)
	LD   R30,Z
	RCALL SUBOPT_0x7
	MOVW R26,R30
	LDD  R30,Y+1
	RCALL SUBOPT_0x7
	ADD  R30,R26
	ADC  R31,R27
	RCALL SUBOPT_0x1C
	LDD  R13,Y+1
	LDD  R12,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	RCALL __lcd_ready
	LDI  R30,LOW(2)
	RCALL SUBOPT_0x1C
	RCALL __lcd_ready
	LDI  R30,LOW(12)
	RCALL SUBOPT_0x1C
	RCALL __lcd_ready
	LDI  R30,LOW(1)
	RCALL SUBOPT_0x1C
	LDI  R30,LOW(0)
	MOV  R12,R30
	MOV  R13,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R13
	LDS  R30,__lcd_maxx
	CP   R30,R13
	BRSH _0x2020004
	__lcd_putchar1:
	INC  R12
	RCALL SUBOPT_0x8
	ST   -Y,R12
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0x2020004:
    rcall __lcd_ready
    sbi  __lcd_port,__lcd_rs ;RS=1
    ld   r26,y
    st   -y,r26
    rcall __lcd_write_data
__lcd_putchar0:
    pop  r31
    pop  r30
	RJMP _0x2080001
_lcd_puts:
	ST   -Y,R17
_0x2020005:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x2020007
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x2020005
_0x2020007:
	RJMP _0x2080002
_lcd_putsf:
	ST   -Y,R17
_0x2020008:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x202000A
	ST   -Y,R17
	RCALL _lcd_putchar
	RJMP _0x2020008
_0x202000A:
_0x2080002:
	LDD  R17,Y+0
_0x2080003:
	ADIW R28,3
	RET
__long_delay_G101:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G101:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G101
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RJMP _0x2080001
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LD   R30,Y
	STS  __lcd_maxx,R30
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-128)
	SBCI R31,HIGH(-128)
	__PUTB1MN __base_y_G101,2
	LD   R30,Y
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-192)
	SBCI R31,HIGH(-192)
	__PUTB1MN __base_y_G101,3
	RCALL SUBOPT_0x1D
	RCALL SUBOPT_0x1D
	RCALL SUBOPT_0x1D
	RCALL __long_delay_G101
	LDI  R30,LOW(32)
	ST   -Y,R30
	RCALL __lcd_init_write_G101
	RCALL __long_delay_G101
	LDI  R30,LOW(40)
	RCALL SUBOPT_0x1C
	RCALL __long_delay_G101
	LDI  R30,LOW(4)
	RCALL SUBOPT_0x1C
	RCALL __long_delay_G101
	LDI  R30,LOW(133)
	RCALL SUBOPT_0x1C
	RCALL __long_delay_G101
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RCALL _lcd_read_byte0_G101
	CPI  R30,LOW(0x5)
	BREQ _0x202000B
	LDI  R30,LOW(0)
	RJMP _0x2080001
_0x202000B:
	RCALL __lcd_ready
	LDI  R30,LOW(6)
	RCALL SUBOPT_0x1C
	RCALL _lcd_clear
	LDI  R30,LOW(1)
_0x2080001:
	ADIW R28,1
	RET

	.CSEG

	.CSEG
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
_buf:
	.BYTE 0x21
_RH:
	.BYTE 0x4
_Temp:
	.BYTE 0x4
__base_y_G101:
	.BYTE 0x4
__lcd_maxx:
	.BYTE 0x1
_p_S1030024:
	.BYTE 0x2

	.CSEG
;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x0:
	IN   R30,0x1A
	LDI  R31,0
	ORI  R30,1
	OUT  0x1A,R30
	SBI  0x1B,0
	CBI  0x1B,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x1:
	RCALL __SAVELOCR2
	IN   R16,26
	IN   R30,0x1A
	LDI  R31,0
	ORI  R30,1
	OUT  0x1A,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0x2:
	SBI  0x1B,0
	IN   R30,0x1A
	LDI  R31,0
	ANDI R30,LOW(0xFE)
	ANDI R31,HIGH(0xFE)
	OUT  0x1A,R30
	LDI  R17,LOW(0)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x3:
	LDI  R30,0
	SBIC 0x19,0
	LDI  R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x4:
	ST   -Y,R31
	ST   -Y,R30
	RJMP _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x5:
	LDD  R30,Y+2
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x6:
	IN   R30,0x1A
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 13 TIMES, CODE SIZE REDUCTION:22 WORDS
SUBOPT_0x7:
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	LDI  R30,LOW(0)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 25 TIMES, CODE SIZE REDUCTION:22 WORDS
SUBOPT_0x9:
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:19 WORDS
SUBOPT_0xA:
	ST   -Y,R18
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	RCALL SUBOPT_0x9
	MOVW R30,R28
	ADIW R30,15
	RCALL SUBOPT_0x9
	RJMP __put_G100

;OPTIMIZER ADDED SUBROUTINE, CALLED 18 TIMES, CODE SIZE REDUCTION:49 WORDS
SUBOPT_0xB:
	MOV  R26,R16
	LDI  R27,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xC:
	OR   R30,R26
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0xD:
	LDI  R27,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xE:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0xF:
	SBIW R30,4
	STD  Y+16,R30
	STD  Y+16+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:16 WORDS
SUBOPT_0x10:
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	RCALL SUBOPT_0x9
	MOVW R30,R28
	ADIW R30,15
	RCALL SUBOPT_0x9
	RJMP __put_G100

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x11:
	RCALL SUBOPT_0xE
	RJMP SUBOPT_0xF

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0x12:
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	RJMP SUBOPT_0x9

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x13:
	AND  R30,R26
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x14:
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x15:
	STD  Y+6,R30
	STD  Y+6+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x16:
	RCALL SUBOPT_0x14
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x17:
	LDD  R26,Y+16
	LDD  R27,Y+16+1
	ADIW R26,4
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x18:
	LDI  R30,LOW(65531)
	LDI  R31,HIGH(65531)
	RJMP SUBOPT_0x13

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x19:
	MOV  R30,R16
	RCALL SUBOPT_0x7
	ANDI R30,LOW(0x1)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x1A:
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x1B:
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 8 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x1C:
	ST   -Y,R30
	RJMP __lcd_write_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x1D:
	RCALL __long_delay_G101
	LDI  R30,LOW(48)
	ST   -Y,R30
	RJMP __lcd_init_write_G101


	.CSEG
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

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
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

__ASRW4:
	ASR  R31
	ROR  R30
__ASRW3:
	ASR  R31
	ROR  R30
__ASRW2:
	ASR  R31
	ROR  R30
	ASR  R31
	ROR  R30
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

__SWAPD12:
	MOV  R1,R24
	MOV  R24,R22
	MOV  R22,R1
	MOV  R1,R25
	MOV  R25,R23
	MOV  R23,R1

__SWAPW12:
	MOV  R1,R27
	MOV  R27,R31
	MOV  R31,R1

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
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

__SWAPACC:
	PUSH R20
	MOVW R20,R30
	MOVW R30,R26
	MOVW R26,R20
	MOVW R20,R22
	MOVW R22,R24
	MOVW R24,R20
	MOV  R20,R0
	MOV  R0,R1
	MOV  R1,R20
	POP  R20
	RET

__UADD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	RET

__NEGMAN1:
	COM  R30
	COM  R31
	COM  R22
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	RET

__SUBF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129
	LDI  R21,0x80
	EOR  R1,R21

__ADDF120:
	CPI  R23,0x80
	BREQ __ADDF128
__ADDF121:
	MOV  R21,R23
	SUB  R21,R25
	BRVS __ADDF129
	BRPL __ADDF122
	RCALL __SWAPACC
	RJMP __ADDF121
__ADDF122:
	CPI  R21,24
	BRLO __ADDF123
	CLR  R26
	CLR  R27
	CLR  R24
__ADDF123:
	CPI  R21,8
	BRLO __ADDF124
	MOV  R26,R27
	MOV  R27,R24
	CLR  R24
	SUBI R21,8
	RJMP __ADDF123
__ADDF124:
	TST  R21
	BREQ __ADDF126
__ADDF125:
	LSR  R24
	ROR  R27
	ROR  R26
	DEC  R21
	BRNE __ADDF125
__ADDF126:
	MOV  R21,R0
	EOR  R21,R1
	BRMI __ADDF127
	RCALL __UADD12
	BRCC __ADDF129
	ROR  R22
	ROR  R31
	ROR  R30
	INC  R23
	BRVC __ADDF129
	RJMP __MAXRES
__ADDF128:
	RCALL __SWAPACC
__ADDF129:
	RCALL __REPACK
	POP  R21
	RET
__ADDF127:
	SUB  R30,R26
	SBC  R31,R27
	SBC  R22,R24
	BREQ __ZERORES
	BRCC __ADDF1210
	COM  R0
	RCALL __NEGMAN1
__ADDF1210:
	TST  R22
	BRMI __ADDF129
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVC __ADDF1210

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

__DIVF21:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BRNE __DIVF210
	TST  R1
__DIVF211:
	BRPL __DIVF219
	RJMP __MINRES
__DIVF219:
	RJMP __MAXRES
__DIVF210:
	CPI  R25,0x80
	BRNE __DIVF218
__DIVF217:
	RJMP __ZERORES
__DIVF218:
	EOR  R0,R1
	SEC
	SBC  R25,R23
	BRVC __DIVF216
	BRLT __DIVF217
	TST  R0
	RJMP __DIVF211
__DIVF216:
	MOV  R23,R25
	PUSH R17
	PUSH R18
	PUSH R19
	PUSH R20
	CLR  R1
	CLR  R17
	CLR  R18
	CLR  R19
	CLR  R20
	CLR  R21
	LDI  R25,32
__DIVF212:
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	CPC  R20,R17
	BRLO __DIVF213
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SBC  R20,R17
	SEC
	RJMP __DIVF214
__DIVF213:
	CLC
__DIVF214:
	ROL  R21
	ROL  R18
	ROL  R19
	ROL  R1
	ROL  R26
	ROL  R27
	ROL  R24
	ROL  R20
	DEC  R25
	BRNE __DIVF212
	MOVW R30,R18
	MOV  R22,R1
	POP  R20
	POP  R19
	POP  R18
	POP  R17
	TST  R22
	BRMI __DIVF215
	LSL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVS __DIVF217
__DIVF215:
	RCALL __ROUND_REPACK
	POP  R21
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
