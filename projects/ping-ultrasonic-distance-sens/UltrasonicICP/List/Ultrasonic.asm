
;CodeVisionAVR C Compiler V2.03.4 Standard
;(C) Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega32
;Program type           : Application
;Clock frequency        : 8.000000 MHz
;Memory model           : Small
;Optimize for           : Speed
;(s)printf features     : float, width, precision
;(s)scanf features      : long, width
;External RAM size      : 0
;Data Stack size        : 512 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : Yes
;char is unsigned       : Yes
;global const stored in FLASH  : No
;8 bit enums            : Yes
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega32
	#pragma AVRPART MEMORY PROG_FLASH 32768
	#pragma AVRPART MEMORY EEPROM 1024
	#pragma AVRPART MEMORY INT_SRAM SIZE 2048
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
	CALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	CALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	CALL __EEPROMWRD
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

;NAME DEFINITIONS FOR GLOBAL VARIABLES ALLOCATED TO REGISTERS
	.DEF _run=R5

	.CSEG
	.ORG 0x00

;INTERRUPT VECTORS
	JMP  __RESET
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  _timer1_capt_isr
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00

_0xD:
	.DB  0x3
_0x0:
	.DB  0x57,0x65,0x6C,0x63,0x6F,0x6D,0x65,0x0
	.DB  0xA,0xD,0x47,0x45,0x54,0x0,0xA,0xD
	.DB  0x54,0x43,0x4E,0x54,0x31,0x20,0x3D,0x20
	.DB  0x25,0x75,0x20,0x2D,0x2D,0x20,0x44,0x69
	.DB  0x73,0x74,0x61,0x6E,0x63,0x65,0x20,0x3D
	.DB  0x20,0x25,0x66,0x0
_0x2000000:
	.DB  0x2D,0x4E,0x41,0x4E,0x0
_0x208005F:
	.DB  0x1
_0x2080000:
	.DB  0x2D,0x4E,0x41,0x4E,0x0

__GLOBAL_INI_TBL:
	.DW  0x01
	.DW  0x05
	.DW  _0xD*2

	.DW  0x01
	.DW  __seed_G104
	.DW  _0x208005F*2

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
;Date    : 2/11/2009
;Author  :
;Company :
;Comments:
;
;
;Chip type           : ATmega32
;Program type        : Application
;Clock frequency     : 8.000000 MHz
;Memory model        : Small
;External RAM size   : 0
;Data Stack size     : 512
;*****************************************************/
;//#define DEBUG
;#define DEBUG2
;
;#include <mega32.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
;#include <delay.h>
;
;// Standard Input/Output functions
;#include <stdio.h>
;
;unsigned char run=3;
;float time=0;
;void getDist();
;void sendData();
;
;// Timer 1 input capture interrupt service routine
;interrupt [TIM1_CAPT] void timer1_capt_isr(void)
; 0000 0027 {

	.CSEG
_timer1_capt_isr:
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
; 0000 0028     #ifdef DEBUG
; 0000 0029        printf("\n\rICP");
; 0000 002A     #endif
; 0000 002B     if(run == 0)
	TST  R5
	BRNE _0x3
; 0000 002C     {
; 0000 002D         #ifdef DEBUG
; 0000 002E             printf("\n\rStart");
; 0000 002F         #endif
; 0000 0030         TCCR1B |= 0x02; //Start Timer 1
	IN   R30,0x2E
	LDI  R31,0
	ORI  R30,2
	OUT  0x2E,R30
; 0000 0031         TCCR1B &= 0xBF; //Falling edge
	IN   R30,0x2E
	LDI  R31,0
	ANDI R30,LOW(0xBF)
	ANDI R31,HIGH(0xBF)
	OUT  0x2E,R30
; 0000 0032         run = 1;
	LDI  R30,LOW(1)
	MOV  R5,R30
; 0000 0033         #ifdef DEBUG
; 0000 0034             printf("\n\r1Run = %d",run);
; 0000 0035         #endif
; 0000 0036     }
; 0000 0037     else if(run == 1)
	RJMP _0x4
_0x3:
	LDI  R30,LOW(1)
	CP   R30,R5
	BRNE _0x5
; 0000 0038     {
; 0000 0039         time = TCNT1;
	IN   R30,0x2C
	IN   R31,0x2C+1
	LDI  R26,LOW(_time)
	LDI  R27,HIGH(_time)
	CLR  R22
	CLR  R23
	CALL __CDF1
	CALL __PUTDP1
; 0000 003A         TCCR1B &= 0xF8; // Disable Timer 1
	IN   R30,0x2E
	LDI  R31,0
	ANDI R30,LOW(0xF8)
	ANDI R31,HIGH(0xF8)
	OUT  0x2E,R30
; 0000 003B         TCNT1 = 0x0000; //Clear Timer 1
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x2C+1,R31
	OUT  0x2C,R30
; 0000 003C         TIMSK &= 0xDF; // Disable ICP
	IN   R30,0x39
	LDI  R31,0
	ANDI R30,LOW(0xDF)
	ANDI R31,HIGH(0xDF)
	OUT  0x39,R30
; 0000 003D         DDRD |= 0x40; //Set PIND.6 as out
	IN   R30,0x11
	LDI  R31,0
	ORI  R30,0x40
	OUT  0x11,R30
; 0000 003E         TCCR1B |= 0x40; //Rising edge
	IN   R30,0x2E
	LDI  R31,0
	ORI  R30,0x40
	OUT  0x2E,R30
; 0000 003F         run = 3;
	LDI  R30,LOW(3)
	MOV  R5,R30
; 0000 0040         #ifdef DEBUG
; 0000 0041             printf("\n\rEND TCNT = %f 2Run = %d",time,run);
; 0000 0042         #endif
; 0000 0043         #ifdef DEBUG2
; 0000 0044             sendData();
	RCALL _sendData
; 0000 0045         #endif
; 0000 0046     }
; 0000 0047     else
	RJMP _0x6
_0x5:
; 0000 0048     {
; 0000 0049         #ifdef DEBUG
; 0000 004A             printf("\n\rrun = %d",run);
; 0000 004B         #endif
; 0000 004C         run = 0;
	CLR  R5
; 0000 004D         #ifdef DEBUG
; 0000 004E             printf("\n\rrun = %d",run);
; 0000 004F         #endif
; 0000 0050     }
_0x6:
_0x4:
; 0000 0051 }
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
;
;// Declare your global variables here
;
;void main(void)
; 0000 0056 {
_main:
; 0000 0057 // Declare your local variables here
; 0000 0058 
; 0000 0059 // Input/Output Ports initialization
; 0000 005A // Port A initialization
; 0000 005B // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 005C // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 005D PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
; 0000 005E DDRA=0x00;
	OUT  0x1A,R30
; 0000 005F 
; 0000 0060 // Port B initialization
; 0000 0061 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out
; 0000 0062 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=0
; 0000 0063 PORTB=0x00;
	OUT  0x18,R30
; 0000 0064 DDRB=0x00;
	OUT  0x17,R30
; 0000 0065 
; 0000 0066 // Port C initialization
; 0000 0067 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 0068 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 0069 PORTC=0x00;
	OUT  0x15,R30
; 0000 006A DDRC=0x00;
	OUT  0x14,R30
; 0000 006B 
; 0000 006C // Port D initialization
; 0000 006D // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 006E // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 006F PORTD=0x00;
	OUT  0x12,R30
; 0000 0070 DDRD=0x40;
	LDI  R30,LOW(64)
	OUT  0x11,R30
; 0000 0071 
; 0000 0072 // Timer/Counter 0 initialization
; 0000 0073 // Clock source: System Clock
; 0000 0074 // Clock value: Timer 0 Stopped
; 0000 0075 // Mode: Normal top=FFh
; 0000 0076 // OC0 output: Disconnected
; 0000 0077 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
; 0000 0078 TCNT0=0x00;
	OUT  0x32,R30
; 0000 0079 OCR0=0x00;
	OUT  0x3C,R30
; 0000 007A 
; 0000 007B // Timer/Counter 1 initialization
; 0000 007C // Clock source: System Clock
; 0000 007D // Clock value: 1000 kHz
; 0000 007E // Mode: Normal top=FFFFh
; 0000 007F // OC1A output: Discon.
; 0000 0080 // OC1B output: Discon.
; 0000 0081 // Noise Canceler: Off
; 0000 0082 // Input Capture on Rising Edge
; 0000 0083 // Timer 1 Overflow Interrupt: Off
; 0000 0084 // Input Capture Interrupt: On
; 0000 0085 // Compare A Match Interrupt: Off
; 0000 0086 // Compare B Match Interrupt: Off
; 0000 0087 TCCR1A=0x00;
	OUT  0x2F,R30
; 0000 0088 TCCR1B=0x40; //Initially disable TMR1
	LDI  R30,LOW(64)
	OUT  0x2E,R30
; 0000 0089 TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
; 0000 008A TCNT1L=0x00;
	OUT  0x2C,R30
; 0000 008B ICR1H=0x00;
	OUT  0x27,R30
; 0000 008C ICR1L=0x00;
	OUT  0x26,R30
; 0000 008D OCR1AH=0x00;
	OUT  0x2B,R30
; 0000 008E OCR1AL=0x00;
	OUT  0x2A,R30
; 0000 008F OCR1BH=0x00;
	OUT  0x29,R30
; 0000 0090 OCR1BL=0x00;
	OUT  0x28,R30
; 0000 0091 
; 0000 0092 // Timer/Counter 2 initialization
; 0000 0093 // Clock source: System Clock
; 0000 0094 // Clock value: Timer 2 Stopped
; 0000 0095 // Mode: Normal top=FFh
; 0000 0096 // OC2 output: Disconnected
; 0000 0097 ASSR=0x00;
	OUT  0x22,R30
; 0000 0098 TCCR2=0x00;
	OUT  0x25,R30
; 0000 0099 TCNT2=0x00;
	OUT  0x24,R30
; 0000 009A OCR2=0x00;
	OUT  0x23,R30
; 0000 009B 
; 0000 009C // External Interrupt(s) initialization
; 0000 009D // INT0: Off
; 0000 009E // INT1: Off
; 0000 009F // INT2: Off
; 0000 00A0 MCUCR=0x00;
	OUT  0x35,R30
; 0000 00A1 MCUCSR=0x00;
	OUT  0x34,R30
; 0000 00A2 
; 0000 00A3 // Timer(s)/Counter(s) Interrupt(s) initialization
; 0000 00A4 TIMSK=0x00;
	OUT  0x39,R30
; 0000 00A5 
; 0000 00A6 // USART initialization
; 0000 00A7 // Communication Parameters: 8 Data, 1 Stop, No Parity
; 0000 00A8 // USART Receiver: On
; 0000 00A9 // USART Transmitter: On
; 0000 00AA // USART Mode: Asynchronous
; 0000 00AB // USART Baud Rate: 9600
; 0000 00AC UCSRA=0x00;
	OUT  0xB,R30
; 0000 00AD UCSRB=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
; 0000 00AE UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
; 0000 00AF UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
; 0000 00B0 UBRRL=0x33;
	LDI  R30,LOW(51)
	OUT  0x9,R30
; 0000 00B1 
; 0000 00B2 // Analog Comparator initialization
; 0000 00B3 // Analog Comparator: Off
; 0000 00B4 // Analog Comparator Input Capture by Timer/Counter 1: Off
; 0000 00B5 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
; 0000 00B6 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
; 0000 00B7 
; 0000 00B8 // Global enable interrupts
; 0000 00B9 #asm("sei")
	sei
; 0000 00BA printf("Welcome");
	__POINTW1FN _0x0,0
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
; 0000 00BB 
; 0000 00BC while (1)
_0x7:
; 0000 00BD       {
; 0000 00BE         delay_ms(500);
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
; 0000 00BF         getDist();
	RCALL _getDist
; 0000 00C0         #ifdef DEBUG
; 0000 00C1             printf("\n\r3Run = %d",run);
; 0000 00C2         #endif
; 0000 00C3       };
	RJMP _0x7
; 0000 00C4 }
_0xA:
	RJMP _0xA
;
;void getDist()
; 0000 00C7 {
_getDist:
; 0000 00C8     #ifdef DEBUG2
; 0000 00C9         printf("\n\rGET");
	__POINTW1FN _0x0,8
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	CALL _printf
	ADIW R28,2
; 0000 00CA     #endif
; 0000 00CB     // Output Burst signal
; 0000 00CC     TIMSK &= 0xDF; // Disable ICP           //Problem2--
	IN   R30,0x39
	LDI  R31,0
	ANDI R30,LOW(0xDF)
	ANDI R31,HIGH(0xDF)
	OUT  0x39,R30
; 0000 00CD     DDRD |= 0x40; //Set PIND.6 as out       //--Problem2
	IN   R30,0x11
	LDI  R31,0
	ORI  R30,0x40
	OUT  0x11,R30
; 0000 00CE     PORTD |= 0x40;
	IN   R30,0x12
	LDI  R31,0
	ORI  R30,0x40
	OUT  0x12,R30
; 0000 00CF     delay_us(5);
	__DELAY_USB 13
; 0000 00D0     PORTD.6 &= 0xBF;
	LDI  R26,0
	SBIC 0x12,6
	LDI  R26,1
	LDI  R30,LOW(191)
	AND  R30,R26
	BRNE _0xB
	CBI  0x12,6
	RJMP _0xC
_0xB:
	SBI  0x12,6
_0xC:
; 0000 00D1     DDRD &= 0xBF; // Set PORTD.6 as in
	IN   R30,0x11
	LDI  R31,0
	ANDI R30,LOW(0xBF)
	ANDI R31,HIGH(0xBF)
	OUT  0x11,R30
; 0000 00D2     PORTD &= 0xBF; //Tri-state
	IN   R30,0x12
	LDI  R31,0
	ANDI R30,LOW(0xBF)
	ANDI R31,HIGH(0xBF)
	OUT  0x12,R30
; 0000 00D3     run = 2;
	LDI  R30,LOW(2)
	MOV  R5,R30
; 0000 00D4     #ifdef DEBUG
; 0000 00D5         printf("\n\r4Run = %d",run);
; 0000 00D6     #endif
; 0000 00D7     TIMSK |= 0x20; // Enable ICP  //Problem1
	IN   R30,0x39
	LDI  R31,0
	ORI  R30,0x20
	OUT  0x39,R30
; 0000 00D8 }
	RET
;
;void sendData()
; 0000 00DB {
_sendData:
; 0000 00DC     printf("\n\rTCNT1 = %u -- Distance = %f",TCNT1,(float)(time*340.0/2000000.0));
	__POINTW1FN _0x0,14
	ST   -Y,R31
	ST   -Y,R30
	IN   R30,0x2C
	IN   R31,0x2C+1
	CLR  R22
	CLR  R23
	CALL __PUTPARD1
	LDS  R26,_time
	LDS  R27,_time+1
	LDS  R24,_time+2
	LDS  R25,_time+3
	__GETD1N 0x43AA0000
	CALL __MULF12
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x49F42400
	CALL __DIVF21
	CALL __PUTPARD1
	LDI  R24,8
	CALL _printf
	ADIW R28,10
; 0000 00DD }
	RET
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif

	.CSEG
_putchar:
     sbis usr,udre
     rjmp _putchar
     ld   r30,y
     out  udr,r30
	ADIW R28,1
	RET
__put_G100:
	ST   -Y,R17
	ST   -Y,R16
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x2000010
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	CALL __GETW1P
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
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,7
	RET
__ftoe_G100:
	SBIW R28,4
	LDI  R30,LOW(0)
	ST   Y,R30
	STD  Y+1,R30
	LDI  R30,LOW(128)
	STD  Y+2,R30
	LDI  R30,LOW(63)
	STD  Y+3,R30
	CALL __SAVELOCR4
	LDD  R30,Y+14
	LDD  R31,Y+14+1
	CPI  R30,LOW(0xFFFF)
	LDI  R26,HIGH(0xFFFF)
	CPC  R31,R26
	BRNE _0x2000018
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0x2000000,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _strcpyf
	RJMP _0x20A0004
_0x2000018:
	CPI  R30,LOW(0x7FFF)
	LDI  R26,HIGH(0x7FFF)
	CPC  R31,R26
	BRNE _0x2000017
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0x2000000,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _strcpyf
	RJMP _0x20A0004
_0x2000017:
	LDD  R26,Y+11
	CPI  R26,LOW(0x7)
	BRLO _0x200001A
	LDI  R30,LOW(6)
	STD  Y+11,R30
_0x200001A:
	LDD  R17,Y+11
_0x200001B:
	MOV  R30,R17
	SUBI R17,1
	CPI  R30,0
	BREQ _0x200001D
	__GETD2S 4
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 4
	RJMP _0x200001B
_0x200001D:
	__GETD1S 12
	CALL __CPD10
	BRNE _0x200001E
	LDI  R19,LOW(0)
	__GETD2S 4
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 4
	RJMP _0x200001F
_0x200001E:
	LDD  R19,Y+11
	__GETD1S 4
	__GETD2S 12
	CALL __CMPF12
	BREQ PC+2
	BRCC PC+3
	JMP  _0x2000020
	__GETD2S 4
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 4
_0x2000021:
	__GETD1S 4
	__GETD2S 12
	CALL __CMPF12
	BRLO _0x2000023
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	__PUTD1S 12
	SUBI R19,-LOW(1)
	RJMP _0x2000021
_0x2000023:
	RJMP _0x2000024
_0x2000020:
_0x2000025:
	__GETD1S 4
	__GETD2S 12
	CALL __CMPF12
	BRSH _0x2000027
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 12
	SUBI R19,LOW(1)
	RJMP _0x2000025
_0x2000027:
	__GETD2S 4
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 4
_0x2000024:
	__GETD1S 12
	__GETD2N 0x3F000000
	CALL __ADDF12
	__PUTD1S 12
	__GETD1S 4
	__GETD2S 12
	CALL __CMPF12
	BRLO _0x2000028
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	__PUTD1S 12
	SUBI R19,-LOW(1)
_0x2000028:
_0x200001F:
	LDI  R17,LOW(0)
_0x2000029:
	LDD  R30,Y+11
	CP   R30,R17
	BRSH PC+3
	JMP _0x200002B
	__GETD2S 4
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	__GETD2N 0x3F000000
	CALL __ADDF12
	CALL __PUTPARD1
	CALL _floor
	__PUTD1S 4
	__GETD2S 12
	CALL __DIVF21
	CALL __CFD1U
	MOV  R16,R30
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	ADIW R26,1
	STD  Y+8,R26
	STD  Y+8+1,R27
	SBIW R26,1
	MOV  R30,R16
	LDI  R31,0
	ADIW R30,48
	ST   X,R30
	MOV  R30,R16
	CALL __CBD1
	CALL __CDF1
	__GETD2S 4
	CALL __MULF12
	__GETD2S 12
	CALL __SWAPD12
	CALL __SUBF12
	__PUTD1S 12
	MOV  R30,R17
	SUBI R17,-1
	CPI  R30,0
	BREQ _0x200002C
	RJMP _0x2000029
_0x200002C:
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	ADIW R26,1
	STD  Y+8,R26
	STD  Y+8+1,R27
	SBIW R26,1
	LDI  R30,LOW(46)
	ST   X,R30
	RJMP _0x2000029
_0x200002B:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	ADIW R30,1
	STD  Y+8,R30
	STD  Y+8+1,R31
	SBIW R30,1
	LDD  R26,Y+10
	STD  Z+0,R26
	CPI  R19,0
	BRGE _0x200002D
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	ADIW R26,1
	STD  Y+8,R26
	STD  Y+8+1,R27
	SBIW R26,1
	LDI  R30,LOW(45)
	ST   X,R30
	NEG  R19
_0x200002D:
	CPI  R19,10
	BRLT _0x200002E
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	ADIW R30,1
	STD  Y+8,R30
	STD  Y+8+1,R31
	SBIW R30,1
	MOVW R22,R30
	MOV  R26,R19
	LDI  R27,0
	SBRC R26,7
	SER  R27
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __DIVW21
	ADIW R30,48
	MOVW R26,R22
	ST   X,R30
_0x200002E:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	ADIW R30,1
	STD  Y+8,R30
	STD  Y+8+1,R31
	SBIW R30,1
	MOVW R22,R30
	MOV  R26,R19
	LDI  R27,0
	SBRC R26,7
	SER  R27
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __MODW21
	ADIW R30,48
	MOVW R26,R22
	ST   X,R30
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(0)
	ST   X,R30
_0x20A0004:
	CALL __LOADLOCR4
	ADIW R28,16
	RET
__print_G100:
	SBIW R28,63
	SBIW R28,15
	CALL __SAVELOCR6
	LDI  R17,0
	__GETW1SX 84
	STD  Y+16,R30
	STD  Y+16+1,R31
_0x200002F:
	MOVW R26,R28
	SUBI R26,LOW(-(90))
	SBCI R27,HIGH(-(90))
	LD   R30,X+
	LD   R31,X+
	ADIW R30,1
	ST   -X,R31
	ST   -X,R30
	SBIW R30,1
	LPM  R30,Z
	MOV  R18,R30
	CPI  R30,0
	BRNE PC+3
	JMP _0x2000031
	MOV  R30,R17
	LDI  R31,0
	SBIW R30,0
	BRNE _0x2000035
	CPI  R18,37
	BRNE _0x2000036
	LDI  R17,LOW(1)
	RJMP _0x2000037
_0x2000036:
	ST   -Y,R18
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
_0x2000037:
	RJMP _0x2000034
_0x2000035:
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BRNE _0x2000038
	CPI  R18,37
	BRNE _0x2000039
	ST   -Y,R18
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	RJMP _0x2000104
_0x2000039:
	LDI  R17,LOW(2)
	LDI  R30,LOW(0)
	STD  Y+19,R30
	LDI  R16,LOW(0)
	CPI  R18,45
	BRNE _0x200003A
	LDI  R16,LOW(1)
	RJMP _0x2000034
_0x200003A:
	CPI  R18,43
	BRNE _0x200003B
	LDI  R30,LOW(43)
	STD  Y+19,R30
	RJMP _0x2000034
_0x200003B:
	CPI  R18,32
	BRNE _0x200003C
	LDI  R30,LOW(32)
	STD  Y+19,R30
	RJMP _0x2000034
_0x200003C:
	RJMP _0x200003D
_0x2000038:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BRNE _0x200003E
_0x200003D:
	LDI  R21,LOW(0)
	LDI  R17,LOW(3)
	CPI  R18,48
	BRNE _0x200003F
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	OR   R30,R26
	MOV  R16,R30
	RJMP _0x2000034
_0x200003F:
	RJMP _0x2000040
_0x200003E:
	CPI  R30,LOW(0x3)
	LDI  R26,HIGH(0x3)
	CPC  R31,R26
	BRNE _0x2000041
_0x2000040:
	CPI  R18,48
	BRLO _0x2000043
	CPI  R18,58
	BRLO _0x2000044
_0x2000043:
	RJMP _0x2000042
_0x2000044:
	MOV  R26,R21
	LDI  R27,0
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	MULS R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R22,R21
	CLR  R23
	MOV  R26,R18
	LDI  R27,0
	LDI  R30,LOW(48)
	LDI  R31,HIGH(48)
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	MOVW R26,R22
	ADD  R30,R26
	MOV  R21,R30
	RJMP _0x2000034
_0x2000042:
	LDI  R20,LOW(0)
	CPI  R18,46
	BRNE _0x2000045
	LDI  R17,LOW(4)
	RJMP _0x2000034
_0x2000045:
	RJMP _0x2000046
_0x2000041:
	CPI  R30,LOW(0x4)
	LDI  R26,HIGH(0x4)
	CPC  R31,R26
	BRNE _0x2000048
	CPI  R18,48
	BRLO _0x200004A
	CPI  R18,58
	BRLO _0x200004B
_0x200004A:
	RJMP _0x2000049
_0x200004B:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(32)
	LDI  R31,HIGH(32)
	OR   R30,R26
	MOV  R16,R30
	MOV  R26,R20
	LDI  R27,0
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	MULS R30,R26
	MOVW R30,R0
	MOV  R20,R30
	MOV  R22,R20
	CLR  R23
	MOV  R26,R18
	LDI  R27,0
	LDI  R30,LOW(48)
	LDI  R31,HIGH(48)
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	MOVW R26,R22
	ADD  R30,R26
	MOV  R20,R30
	RJMP _0x2000034
_0x2000049:
_0x2000046:
	CPI  R18,108
	BRNE _0x200004C
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	OR   R30,R26
	MOV  R16,R30
	LDI  R17,LOW(5)
	RJMP _0x2000034
_0x200004C:
	RJMP _0x200004D
_0x2000048:
	CPI  R30,LOW(0x5)
	LDI  R26,HIGH(0x5)
	CPC  R31,R26
	BREQ PC+3
	JMP _0x2000034
_0x200004D:
	MOV  R30,R18
	LDI  R31,0
	CPI  R30,LOW(0x63)
	LDI  R26,HIGH(0x63)
	CPC  R31,R26
	BRNE _0x2000052
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	LDD  R26,Z+4
	ST   -Y,R26
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	RJMP _0x2000053
_0x2000052:
	CPI  R30,LOW(0x45)
	LDI  R26,HIGH(0x45)
	CPC  R31,R26
	BREQ _0x2000056
	CPI  R30,LOW(0x65)
	LDI  R26,HIGH(0x65)
	CPC  R31,R26
	BRNE _0x2000057
_0x2000056:
	RJMP _0x2000058
_0x2000057:
	CPI  R30,LOW(0x66)
	LDI  R26,HIGH(0x66)
	CPC  R31,R26
	BREQ PC+3
	JMP _0x2000059
_0x2000058:
	MOVW R30,R28
	ADIW R30,20
	STD  Y+10,R30
	STD  Y+10+1,R31
	__GETW2SX 88
	CALL __GETD1P
	__PUTD1S 6
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	LDD  R26,Y+9
	TST  R26
	BRMI _0x200005A
	LDD  R26,Y+19
	CPI  R26,LOW(0x2B)
	BREQ _0x200005C
	RJMP _0x200005D
_0x200005A:
	__GETD1S 6
	CALL __ANEGF1
	__PUTD1S 6
	LDI  R30,LOW(45)
	STD  Y+19,R30
_0x200005C:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x200005E
	LDD  R30,Y+19
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	RJMP _0x200005F
_0x200005E:
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	ADIW R30,1
	STD  Y+10,R30
	STD  Y+10+1,R31
	SBIW R30,1
	LDD  R26,Y+19
	STD  Z+0,R26
_0x200005F:
_0x200005D:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(32)
	LDI  R31,HIGH(32)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BRNE _0x2000060
	LDI  R20,LOW(6)
_0x2000060:
	CPI  R18,102
	BRNE _0x2000061
	__GETD1S 6
	CALL __PUTPARD1
	ST   -Y,R20
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	CALL _ftoa
	RJMP _0x2000062
_0x2000061:
	__GETD1S 6
	CALL __PUTPARD1
	ST   -Y,R20
	ST   -Y,R18
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __ftoe_G100
_0x2000062:
	MOVW R30,R28
	ADIW R30,20
	STD  Y+10,R30
	STD  Y+10+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlen
	MOV  R17,R30
	RJMP _0x2000063
_0x2000059:
	CPI  R30,LOW(0x73)
	LDI  R26,HIGH(0x73)
	CPC  R31,R26
	BRNE _0x2000065
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	__GETW2SX 88
	ADIW R26,4
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlen
	MOV  R17,R30
	RJMP _0x2000066
_0x2000065:
	CPI  R30,LOW(0x70)
	LDI  R26,HIGH(0x70)
	CPC  R31,R26
	BRNE _0x2000068
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	__GETW2SX 88
	ADIW R26,4
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlenf
	MOV  R17,R30
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	OR   R30,R26
	MOV  R16,R30
_0x2000066:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65407)
	LDI  R31,HIGH(65407)
	AND  R30,R26
	MOV  R16,R30
	CPI  R20,0
	BREQ _0x200006A
	CP   R20,R17
	BRLO _0x200006B
_0x200006A:
	RJMP _0x2000069
_0x200006B:
	MOV  R17,R20
_0x2000069:
_0x2000063:
	LDI  R20,LOW(0)
	LDI  R30,LOW(0)
	STD  Y+18,R30
	LDI  R19,LOW(0)
	RJMP _0x200006C
_0x2000068:
	CPI  R30,LOW(0x64)
	LDI  R26,HIGH(0x64)
	CPC  R31,R26
	BREQ _0x200006F
	CPI  R30,LOW(0x69)
	LDI  R26,HIGH(0x69)
	CPC  R31,R26
	BRNE _0x2000070
_0x200006F:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	OR   R30,R26
	MOV  R16,R30
	RJMP _0x2000071
_0x2000070:
	CPI  R30,LOW(0x75)
	LDI  R26,HIGH(0x75)
	CPC  R31,R26
	BRNE _0x2000072
_0x2000071:
	LDI  R30,LOW(10)
	STD  Y+18,R30
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x2000073
	__GETD1N 0x3B9ACA00
	__PUTD1S 12
	LDI  R17,LOW(10)
	RJMP _0x2000074
_0x2000073:
	__GETD1N 0x2710
	__PUTD1S 12
	LDI  R17,LOW(5)
	RJMP _0x2000074
_0x2000072:
	CPI  R30,LOW(0x58)
	LDI  R26,HIGH(0x58)
	CPC  R31,R26
	BRNE _0x2000076
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	OR   R30,R26
	MOV  R16,R30
	RJMP _0x2000077
_0x2000076:
	CPI  R30,LOW(0x78)
	LDI  R26,HIGH(0x78)
	CPC  R31,R26
	BREQ PC+3
	JMP _0x20000B5
_0x2000077:
	LDI  R30,LOW(16)
	STD  Y+18,R30
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x2000079
	__GETD1N 0x10000000
	__PUTD1S 12
	LDI  R17,LOW(8)
	RJMP _0x2000074
_0x2000079:
	__GETD1N 0x1000
	__PUTD1S 12
	LDI  R17,LOW(4)
_0x2000074:
	CPI  R20,0
	BREQ _0x200007A
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65407)
	LDI  R31,HIGH(65407)
	AND  R30,R26
	MOV  R16,R30
	RJMP _0x200007B
_0x200007A:
	LDI  R20,LOW(1)
_0x200007B:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x200007C
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	__GETW2SX 88
	ADIW R26,4
	CALL __GETD1P
	RJMP _0x2000105
_0x200007C:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x200007E
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	__GETW2SX 88
	ADIW R26,4
	CALL __GETW1P
	CALL __CWD1
	RJMP _0x2000105
_0x200007E:
	__GETW1SX 88
	SBIW R30,4
	__PUTW1SX 88
	__GETW2SX 88
	ADIW R26,4
	CALL __GETW1P
	CLR  R22
	CLR  R23
_0x2000105:
	__PUTD1S 6
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x2000080
	LDD  R26,Y+9
	TST  R26
	BRPL _0x2000081
	__GETD1S 6
	CALL __ANEGD1
	__PUTD1S 6
	LDI  R30,LOW(45)
	STD  Y+19,R30
_0x2000081:
	LDD  R30,Y+19
	CPI  R30,0
	BREQ _0x2000082
	SUBI R17,-LOW(1)
	SUBI R20,-LOW(1)
	RJMP _0x2000083
_0x2000082:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65531)
	LDI  R31,HIGH(65531)
	AND  R30,R26
	MOV  R16,R30
_0x2000083:
_0x2000080:
	MOV  R19,R20
_0x200006C:
	MOV  R30,R16
	LDI  R31,0
	ANDI R30,LOW(0x1)
	BRNE _0x2000084
_0x2000085:
	CP   R17,R21
	BRSH _0x2000088
	CP   R19,R21
	BRLO _0x2000089
_0x2000088:
	RJMP _0x2000087
_0x2000089:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x200008A
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x200008B
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65531)
	LDI  R31,HIGH(65531)
	AND  R30,R26
	MOV  R16,R30
	LDD  R18,Y+19
	SUBI R17,LOW(1)
	RJMP _0x200008C
_0x200008B:
	LDI  R18,LOW(48)
_0x200008C:
	RJMP _0x200008D
_0x200008A:
	LDI  R18,LOW(32)
_0x200008D:
	ST   -Y,R18
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	SUBI R21,LOW(1)
	RJMP _0x2000085
_0x2000087:
_0x2000084:
_0x200008E:
	CP   R17,R20
	BRLO PC+3
	JMP _0x2000090
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	OR   R30,R26
	MOV  R16,R30
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x2000091
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65531)
	LDI  R31,HIGH(65531)
	AND  R30,R26
	MOV  R16,R30
	LDD  R30,Y+19
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	SUBI R30,LOW(-(87))
	SBCI R31,HIGH(-(87))
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	CPI  R21,0
	BREQ _0x2000092
	SUBI R21,LOW(1)
_0x2000092:
	SUBI R17,LOW(1)
	SUBI R20,LOW(1)
_0x2000091:
	LDI  R30,LOW(48)
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	CPI  R21,0
	BREQ _0x2000093
	SUBI R21,LOW(1)
_0x2000093:
	SUBI R20,LOW(1)
	RJMP _0x200008E
_0x2000090:
	MOV  R19,R17
	LDD  R30,Y+18
	CPI  R30,0
	BRNE _0x2000094
_0x2000095:
	CPI  R19,0
	BREQ _0x2000097
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x2000098
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	ADIW R30,1
	STD  Y+10,R30
	STD  Y+10+1,R31
	SBIW R30,1
	LPM  R30,Z
	RJMP _0x2000106
_0x2000098:
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	LD   R30,X+
	STD  Y+10,R26
	STD  Y+10+1,R27
_0x2000106:
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	CPI  R21,0
	BREQ _0x200009A
	SUBI R21,LOW(1)
_0x200009A:
	SUBI R19,LOW(1)
	RJMP _0x2000095
_0x2000097:
	RJMP _0x200009B
_0x2000094:
_0x200009D:
	__GETD1S 12
	__GETD2S 6
	CALL __DIVD21U
	MOV  R18,R30
	CPI  R18,10
	BRLO _0x200009F
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x20000A0
	MOV  R30,R18
	LDI  R31,0
	ADIW R30,55
	RJMP _0x2000107
_0x20000A0:
	MOV  R30,R18
	LDI  R31,0
	SUBI R30,LOW(-87)
	SBCI R31,HIGH(-87)
_0x2000107:
	MOV  R18,R30
	RJMP _0x20000A2
_0x200009F:
	MOV  R30,R18
	LDI  R31,0
	ADIW R30,48
	MOV  R18,R30
_0x20000A2:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x20000A3
	RJMP _0x20000A4
_0x20000A3:
	CPI  R18,49
	BRSH _0x20000A6
	__GETD2S 12
	__CPD2N 0x1
	BRNE _0x20000A5
_0x20000A6:
	RJMP _0x20000A8
_0x20000A5:
	CP   R20,R19
	BRSH _0x2000108
	CP   R21,R19
	BRLO _0x20000AB
	MOV  R30,R16
	LDI  R31,0
	ANDI R30,LOW(0x1)
	BREQ _0x20000AC
_0x20000AB:
	RJMP _0x20000AA
_0x20000AC:
	LDI  R18,LOW(32)
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(128)
	LDI  R31,HIGH(128)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x20000AD
_0x2000108:
	LDI  R18,LOW(48)
_0x20000A8:
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	OR   R30,R26
	MOV  R16,R30
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	AND  R30,R26
	AND  R31,R27
	SBIW R30,0
	BREQ _0x20000AE
	MOV  R26,R16
	LDI  R27,0
	LDI  R30,LOW(65531)
	LDI  R31,HIGH(65531)
	AND  R30,R26
	MOV  R16,R30
	LDD  R30,Y+19
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	SUBI R30,LOW(-(87))
	SBCI R31,HIGH(-(87))
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	CPI  R21,0
	BREQ _0x20000AF
	SUBI R21,LOW(1)
_0x20000AF:
_0x20000AE:
_0x20000AD:
_0x20000A4:
	ST   -Y,R18
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	CPI  R21,0
	BREQ _0x20000B0
	SUBI R21,LOW(1)
_0x20000B0:
_0x20000AA:
	SUBI R19,LOW(1)
	__GETD1S 12
	__GETD2S 6
	CALL __MODD21U
	__PUTD1S 6
	LDD  R30,Y+18
	LDI  R31,0
	__GETD2S 12
	CALL __CWD1
	CALL __DIVD21U
	__PUTD1S 12
	CALL __CPD10
	BREQ _0x200009E
	RJMP _0x200009D
_0x200009E:
_0x200009B:
	MOV  R30,R16
	LDI  R31,0
	ANDI R30,LOW(0x1)
	BREQ _0x20000B1
_0x20000B2:
	CPI  R21,0
	BREQ _0x20000B4
	SUBI R21,LOW(1)
	LDI  R30,LOW(32)
	ST   -Y,R30
	__GETW1SX 87
	ST   -Y,R31
	ST   -Y,R30
	MOVW R30,R28
	ADIW R30,19
	ST   -Y,R31
	ST   -Y,R30
	RCALL __put_G100
	RJMP _0x20000B2
_0x20000B4:
_0x20000B1:
_0x20000B5:
_0x2000053:
_0x2000104:
	LDI  R17,LOW(0)
_0x2000034:
	RJMP _0x200002F
_0x2000031:
	CALL __LOADLOCR6
	ADIW R28,63
	ADIW R28,29
	RET
_printf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	MOVW R16,R26
	LDI  R30,0
	STD  Y+2,R30
	STD  Y+2+1,R30
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
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G100
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	POP  R15
	RET

	.CSEG

	.CSEG
_strcpyf:
    ld   r30,y+
    ld   r31,y+
    ld   r26,y+
    ld   r27,y+
    movw r24,r26
strcpyf0:
	lpm  r0,z+
    st   x+,r0
    tst  r0
    brne strcpyf0
    movw r30,r24
    ret
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

	.CSEG
_ftrunc:
   ldd  r23,y+3
   ldd  r22,y+2
   ldd  r31,y+1
   ld   r30,y
   bst  r23,7
   lsl  r23
   sbrc r22,7
   sbr  r23,1
   mov  r25,r23
   subi r25,0x7e
   breq __ftrunc0
   brcs __ftrunc0
   cpi  r25,24
   brsh __ftrunc1
   clr  r26
   clr  r27
   clr  r24
__ftrunc2:
   sec
   ror  r24
   ror  r27
   ror  r26
   dec  r25
   brne __ftrunc2
   and  r30,r26
   and  r31,r27
   and  r22,r24
   rjmp __ftrunc1
__ftrunc0:
   clt
   clr  r23
   clr  r30
   clr  r31
   clr  r22
__ftrunc1:
   cbr  r22,0x80
   lsr  r23
   brcc __ftrunc3
   sbr  r22,0x80
__ftrunc3:
   bld  r23,7
   ld   r26,y+
   ld   r27,y+
   ld   r24,y+
   ld   r25,y+
   cp   r30,r26
   cpc  r31,r27
   cpc  r22,r24
   cpc  r23,r25
   bst  r25,7
   ret
_floor:
	__GETD1S 0
	CALL __PUTPARD1
	CALL _ftrunc
	__PUTD1S 0
    brne __floor1
__floor0:
	__GETD1S 0
	RJMP _0x20A0003
__floor1:
    brtc __floor0
	__GETD2S 0
	__GETD1N 0x3F800000
	CALL __SWAPD12
	CALL __SUBF12
_0x20A0003:
	ADIW R28,4
	RET

	.CSEG
_ftoa:
	SBIW R28,4
	LDI  R30,LOW(0)
	ST   Y,R30
	STD  Y+1,R30
	STD  Y+2,R30
	LDI  R30,LOW(63)
	STD  Y+3,R30
	ST   -Y,R17
	ST   -Y,R16
	LDD  R30,Y+11
	LDD  R31,Y+11+1
	CPI  R30,LOW(0xFFFF)
	LDI  R26,HIGH(0xFFFF)
	CPC  R31,R26
	BRNE _0x208000D
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0x2080000,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _strcpyf
	RJMP _0x20A0002
_0x208000D:
	CPI  R30,LOW(0x7FFF)
	LDI  R26,HIGH(0x7FFF)
	CPC  R31,R26
	BRNE _0x208000C
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0x2080000,1
	ST   -Y,R31
	ST   -Y,R30
	CALL _strcpyf
	RJMP _0x20A0002
_0x208000C:
	LDD  R26,Y+12
	TST  R26
	BRPL _0x208000F
	__GETD1S 9
	CALL __ANEGF1
	__PUTD1S 9
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	LDI  R30,LOW(45)
	ST   X,R30
_0x208000F:
	LDD  R26,Y+8
	CPI  R26,LOW(0x7)
	BRLO _0x2080010
	LDI  R30,LOW(6)
	STD  Y+8,R30
_0x2080010:
	LDD  R17,Y+8
_0x2080011:
	MOV  R30,R17
	SUBI R17,1
	CPI  R30,0
	BREQ _0x2080013
	__GETD2S 2
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	__PUTD1S 2
	RJMP _0x2080011
_0x2080013:
	__GETD1S 2
	__GETD2S 9
	CALL __ADDF12
	__PUTD1S 9
	LDI  R17,LOW(0)
	__GETD1N 0x3F800000
	__PUTD1S 2
_0x2080014:
	__GETD1S 2
	__GETD2S 9
	CALL __CMPF12
	BRLO _0x2080016
	__GETD2S 2
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 2
	SUBI R17,-LOW(1)
	RJMP _0x2080014
_0x2080016:
	CPI  R17,0
	BRNE _0x2080017
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	LDI  R30,LOW(48)
	ST   X,R30
	RJMP _0x2080018
_0x2080017:
_0x2080019:
	MOV  R30,R17
	SUBI R17,1
	CPI  R30,0
	BRNE PC+3
	JMP _0x208001B
	__GETD2S 2
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	__GETD2N 0x3F000000
	CALL __ADDF12
	CALL __PUTPARD1
	CALL _floor
	__PUTD1S 2
	__GETD2S 9
	CALL __DIVF21
	CALL __CFD1U
	MOV  R16,R30
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	MOV  R30,R16
	LDI  R31,0
	ADIW R30,48
	ST   X,R30
	MOV  R30,R16
	LDI  R31,0
	__GETD2S 2
	CALL __CWD1
	CALL __CDF1
	CALL __MULF12
	__GETD2S 9
	CALL __SWAPD12
	CALL __SUBF12
	__PUTD1S 9
	RJMP _0x2080019
_0x208001B:
_0x2080018:
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0x208001C
	RJMP _0x20A0001
_0x208001C:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	LDI  R30,LOW(46)
	ST   X,R30
_0x208001D:
	LDD  R30,Y+8
	SUBI R30,LOW(1)
	STD  Y+8,R30
	SUBI R30,-LOW(1)
	BREQ _0x208001F
	__GETD2S 9
	__GETD1N 0x41200000
	CALL __MULF12
	__PUTD1S 9
	CALL __CFD1U
	MOV  R16,R30
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	MOV  R30,R16
	LDI  R31,0
	ADIW R30,48
	ST   X,R30
	MOV  R30,R16
	LDI  R31,0
	__GETD2S 9
	CALL __CWD1
	CALL __CDF1
	CALL __SWAPD12
	CALL __SUBF12
	__PUTD1S 9
	RJMP _0x208001D
_0x208001F:
_0x20A0001:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(0)
	ST   X,R30
_0x20A0002:
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,13
	RET

	.DSEG

	.CSEG

	.DSEG
_time:
	.BYTE 0x4
_p_S1020024:
	.BYTE 0x2
__seed_G104:
	.BYTE 0x4

	.CSEG

	.CSEG
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

__CBD1:
	MOV  R31,R30
	ADD  R31,R31
	SBC  R31,R31
	MOV  R22,R31
	MOV  R23,R31
	RET

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
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

__DIVW21:
	RCALL __CHKSIGNW
	RCALL __DIVW21U
	BRTC __DIVW211
	RCALL __ANEGW1
__DIVW211:
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

__MODW21:
	CLT
	SBRS R27,7
	RJMP __MODW211
	COM  R26
	COM  R27
	ADIW R26,1
	SET
__MODW211:
	SBRC R31,7
	RCALL __ANEGW1
	RCALL __DIVW21U
	MOVW R30,R26
	BRTC __MODW212
	RCALL __ANEGW1
__MODW212:
	RET

__MODD21U:
	RCALL __DIVD21U
	MOVW R30,R26
	MOVW R22,R24
	RET

__CHKSIGNW:
	CLT
	SBRS R31,7
	RJMP __CHKSW1
	RCALL __ANEGW1
	SET
__CHKSW1:
	SBRS R27,7
	RJMP __CHKSW2
	COM  R26
	COM  R27
	ADIW R26,1
	BLD  R0,0
	INC  R0
	BST  R0,0
__CHKSW2:
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

__ANEGF1:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	BREQ __ANEGF10
	SUBI R23,0x80
__ANEGF10:
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

	RJMP __ADDF120

__ADDF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129

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

__CMPF12:
	TST  R25
	BRMI __CMPF120
	TST  R23
	BRMI __CMPF121
	CP   R25,R23
	BRLO __CMPF122
	BRNE __CMPF121
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __CMPF122
	BREQ __CMPF123
__CMPF121:
	CLZ
	CLC
	RET
__CMPF122:
	CLZ
	SEC
	RET
__CMPF123:
	SEZ
	CLC
	RET
__CMPF120:
	TST  R23
	BRPL __CMPF122
	CP   R25,R23
	BRLO __CMPF121
	BRNE __CMPF122
	CP   R30,R26
	CPC  R31,R27
	CPC  R22,R24
	BRLO __CMPF122
	BREQ __CMPF123
	RJMP __CMPF121

__CPD10:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
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
