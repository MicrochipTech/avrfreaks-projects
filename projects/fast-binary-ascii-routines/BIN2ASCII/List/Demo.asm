
;CodeVisionAVR C Compiler V2.03.4 Standard
;(C) Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega32
;Program type           : Application
;Clock frequency        : 16.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External RAM size      : 0
;Data Stack size        : 512 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;global const stored in FLASH  : Yes
;8 bit enums            : Yes
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : Off

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

	.CSEG
	.ORG 0x00

;INTERRUPT VECTORS
	JMP  __RESET
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
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00

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
;#include "bin2ascii.h"
;
;// Important: You have to disable the autamtic global register allocation in the compiler settings.
;void main()
; 0000 0006 {

	.CSEG
_main:
; 0000 0007     unsigned int num16=0xffff;
; 0000 0008     unsigned long num32=0xffffffff;
; 0000 0009     unsigned char buffer[10];
; 0000 000A 
; 0000 000B     utoa16(num16,buffer);
	SBIW R28,14
	LDI  R30,LOW(255)
	STD  Y+10,R30
	STD  Y+11,R30
	STD  Y+12,R30
	STD  Y+13,R30
;	num16 -> R16,R17
;	num32 -> Y+10
;	buffer -> Y+0
	__GETWRN 16,17,65535
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,2
	ST   -Y,R31
	ST   -Y,R30
	RCALL _utoa16
; 0000 000C     utoa32(num32,buffer);
	__GETD1S 10
	CALL __PUTPARD1
	MOVW R30,R28
	ADIW R30,4
	ST   -Y,R31
	ST   -Y,R30
	CALL _utoa32
; 0000 000D 
; 0000 000E while(1)
_0x3:
; 0000 000F     {
; 0000 0010 
; 0000 0011     #asm nop
; 0000 0012     #endasm
; 0000 0013 
; 0000 0014     }
	RJMP _0x3
; 0000 0015 
; 0000 0016 }
_0x6:
	RJMP _0x6
;
;/**** A P P L I C A T I O N   N O T E	A V R ? ? ? ************************
;;*
;;* Title:		32-bit Arithmetic Routines with Macrolibrary
;;* Project:		Math32
;;* Version:		2.3
;;* Last updated:	2003.09.15
;;* Create Date: 	1999.10.25
;;* Target MCU:		AT90S8515 (as well as others AVR uC)
;;*			(C) ATMEL Corporation (mailto:avr@atmel.com)
;;* Originator:		(C) 1999-2003 Andre Birua (mailto:birua@hotmail.com)
;;*			This Application Note absolutely free in use anybody
;;* INTERPRETATION
;;* This package of assembler subprograms is developed for integer arithmetic
;;* with tracing of sign bit in 32 bits calculations and data reloads.
;;* It is based on microcontroller register file to the maximum.
;;* In real users projects available abundant digit capacity allows to avoid
;;* overflow and reduces inaccuracy of rounding errors in chain calculations.
;;* Included macro definitions will increase readability of assembler source
;;* at bit by bit and multibyte data operations inside AVR software model
;;*
;;* DESCRIPTION
;;* This Application Note lists:
;;*   i) Math32 subroutines for the following:
;;*	Add/Subtract/Multiply/Divide/Complement 32 bits operands,
;;*	Binary 16 & 24 bits operand to/back BCD conversion,
;;*	Binary 32 bits operand to BCD conversion,
;;*	Initialization of data memory on a pattern,
;;*	Load/Store group of registers from/to data space;
;;*  ii) macro definitions call mathematical and data transfer subroutines;
;;* iii) useful general macroinstructions for the AVR 8-Bit RISC family
;;*
;;* "ADD32"      Add without Carry       Rd32 = Rd32 + Rr32
;;* "SUB32"      Subtract without Carry  Rd32 = Rd32 - Rr32
;;* "MUL32"      Multiply Unsigned       Rd64 = Rd32 * Rr32
;;* "DIV32"      Divide Unsigned         Rd32 = Rd32 / Rr32 (Rd64)
;;* "COM32"      One's Complement        Rd32 = 0xffffffff - Rd32
;;* "NEG32"      Two's Complement        Rd32 = 0x00000000 - Rd32
;;* "BCD2bin"    BCD to Binary 16        Rd16 = Rd24|Rr24
;;* "BCD3bin"    BCD to Binary 24        Rd24 = Rd32|Rr32
;;* "Bin2BCD"    Binary 16 to BCD        Rd24 = Rd16|Rr16
;;* "Bin3BCD"    Binary 24 to BCD        Rd32 = Rd24|Rr24
;;* "Bin4BCD"    Binary 32 to BCD        Rd40 = Rd32|Rr32 || hwrd(Rr32)&Rd16
;;* "MathMem"    Init Data Memory        (MA) = 0x00|0xff
;;* "MathLoad"   Load Registers          Rd32|Rr32 = (MA)
;;* "MathSave"   Store Registers         (MA) = Rd32|Rd64
;;*
;;* Rd64: destination registers (8) in the register file
;;* Rd32: destination (and source) registers (4) in the register file
;;* Rr32: source registers (4) in the register file
;;* (MA): address for access to variable in the internal memory (SRAM)
;;* Note: Math32 use high registers, r0 and lower 512 bytes of data space,
;;*    so Rd64=r20:r27, Rd32=r20:r23, Rd24=r20:r22, Rd16=r20:r21,
;;*	 Rd40=r20:r24, Rr32=r16:r19, Rr24=r16:r18, Rr16=r16:r17, MA=0:511
;;*
;;* Number of words & cycles (Min|Max)		c o m m e n t s
;;* "ADD32"      6    4|5    Size of Add32sign
;;* "SUB32"     16    6|15   Size of Sub32sign
;;* "MUL32"     24  460|556  Size of Mul32b, based on AVR200 16x16 unsigned
;;* "DIV32"     28  528|688  Size of Div32b, based on AVR200 16/16 unsigned
;;* "COM32"      5    4|4    Part of Sub32
;;* "NEG32"      9    8|8    Part of Sub32
;;* "BCD2bin"   26   86|89   Equivalent of AVR204, but smaller & quicker
;;* "BCD3bin"   43   38|402  Different from BCD2bin translation algorithm
;;* "Bin2BCD"   22   19|177  Equivalent of AVR204, but smaller & much faster
;;* "Bin3BCD"   21   36|366  In the form of preamble for Bin2BCD
;;* "Bin3BCD"   40   36|333  All-sufficient expansion of Bin2BCD
;;* "Bin4BCD"   37  515|671  Based on AVR204 16-bit Bin to BCD conversion
;;* "Bin4BCD"   48  874|878  All-sufficient transform instead of pre-Bin4BCD
;;* "MathMem"   10    7|645  Size of MathMemLimit, max cycle for 128 bytes
;;* "MathLoad"  15   41|46   Size and max cycle for Rr32 load
;;* "MathSave"  14   13|78   Size and max cycle for Rd64 save
;;* In total:  350 words     Usually +7 cycles: rcall & ret
;;*
;;* All routines are Code Size` optimized implementations and debugged with
;;* macrocode for AVR macro assembler version 1.30 (Jan 27 1999 01:30:00) &
;;*	       AVR32 macro assembler version 1.30 (Sep	8 1999 01:30:00).
;;*    However, AVR32 macro assembler version 1.54 (Nov 14 2001 14:05:48) &
;;*	       AVR32 macro assembler version 1.56 (May	6 2002 14:54:01)
;;* generate dummy warnings: Register already defined by the .DEF directive
;;* (command option for disable this kind of warning as yet is absent...)
;;*			   CheckIt with AVR Studio !
;;* NOTE
;;* ` Bin4BCD transformations has partial loop optimization for speed-up
;;* While using Math32, it is important to consider the allocation of the
;;* microcontroller resources available for the program. It is required:
;;* - to use r0,r16..r31 with Math32;
;;* - to allocate variables used in calculation in the bottom of the memory;
;;* - to use T flag as a sign bit (input, output and temporary),
;;*   if you need to operate negative numbers or up-down overflow error
;;*
;;* VERSION
;;* 1.0 Original version (in use starting with 1999.12.22)
;;* 1.1 Fixed precedence bugs if macroparameter is an assembler expression
;;* 1.2 Modify CBF & SBF & IBF macrocalls
;;* 1.3 Full modification mathematical and data transfer macronotation
;;* 1.4 Optimaze for speed and code size Mul32 & Div32 & BCD2bin & Bin2BCD
;;* 2.0 Version for publication (added description, note and demo sections)
;;* 2.1 Updated Bin2BCD, added Bin4BCD conversion & XCH macrocall
;;* 2.2 Added functionally closed modifications of Bin3&4BCD translation
;;* 2.3 Added BCD3bin conversion, normalize the comment of Bin3&4BCD
;;*
;;* DEMO
;;* section below is a sample of macrocalls and not an ordinary Math32 usage
;*/
;
;#include	 <mega32.h>
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
;#include	"bin2ascii.h"
;
;
;void utoa16(unsigned int num,char *buff)
; 0001 0070 {

	.CSEG
_utoa16:
; 0001 0071 /****************************************************************************
; 0001 0072 ;*
; 0001 0073 ;* Bin2BCD == 16-bit Binary to BCD conversion
; 0001 0074 ;*
; 0001 0075 ;* Important: You have to disable the autamtic global register allocation in the compiler settings.
; 0001 0076 ;*            Only (R0,R1,R22,R23 ,R24,R25 ,R26,R27,R30,R31) can be used in ASM routines.
; 0001 0077 ;*            you have to save the other registers in the stack if wanna use them.
; 0001 0078 ;***************************************************************************/
; 0001 0079 #asm
;	num -> Y+2
;	*buff -> Y+0
; 0001 007A 

; 0001 007B #define	fbinL	r30	; binary value Low byte
#define	fbinL	r30	; binary value Low byte
; 0001 007C #define	fbinH	r31	; binary value High byte
#define	fbinH	r31	; binary value High byte
; 0001 007D #define	temp    r22
#define	temp    r22
; 0001 007E #define	tBCD0	r24	; BCD value digits 0 and 1
#define	tBCD0	r24	; BCD value digits 0 and 1
; 0001 007F #define	tBCD1	r25	; BCD value digits 2 and 3
#define	tBCD1	r25	; BCD value digits 2 and 3
; 0001 0080 #define	tBCD2	r26	; BCD value digit 4 (MSD is lowermost nibble)
#define	tBCD2	r26	; BCD value digit 4 (MSD is lowermost nibble)
; 0001 0081 

; 0001 0082     ldd     r31,Y+3 ;//loading the unsigned integer
    ldd     r31,Y+3 ;//loading the unsigned integer
; 0001 0083     ldd     r30,y+2 ;
    ldd     r30,y+2 ;
; 0001 0084 Bin2BCD:
Bin2BCD:
; 0001 0085 Bin2BCD16:	ldi	tBCD2,0xff	;initialize digit 4
Bin2BCD16:	ldi	tBCD2,0xff	;initialize digit 4
; 0001 0086 binbcd_4:	inc	tBCD2		;
binbcd_4:	inc	tBCD2		;
; 0001 0087 		subi	fbinL,0x10;low(10000);subiw fbin,10000
		subi	fbinL,0x10;low(10000);subiw fbin,10000
; 0001 0088 		sbci	fbinH,0x27;high(10000)
		sbci	fbinH,0x27;high(10000)
; 0001 0089 		brcc	binbcd_4	;
		brcc	binbcd_4	;
; 0001 008A 		ldi	tBCD1,0x9f	;initialize digits 3 and 2
		ldi	tBCD1,0x9f	;initialize digits 3 and 2
; 0001 008B binbcd_3:	subi	tBCD1,0x10	;
binbcd_3:	subi	tBCD1,0x10	;
; 0001 008C 		subi	fbinL,0x18;low(-1000);addiw fbin,1000
		subi	fbinL,0x18;low(-1000);addiw fbin,1000
; 0001 008D 		sbci	fbinH,0xfc;high(-1000)
		sbci	fbinH,0xfc;high(-1000)
; 0001 008E 		brcs	binbcd_3	;
		brcs	binbcd_3	;
; 0001 008F binbcd_2:	inc	tBCD1		;
binbcd_2:	inc	tBCD1		;
; 0001 0090 		subi	fbinL,0x64;low(100)	;subiw fbin,100
		subi	fbinL,0x64;low(100)	;subiw fbin,100
; 0001 0091 		sbci	fbinH,0x00;high(100) ;
		sbci	fbinH,0x00;high(100) ;
; 0001 0092 		brcc	binbcd_2	;
		brcc	binbcd_2	;
; 0001 0093 		ldi	tBCD0,0xa0	;initialize digits 1 and 0
		ldi	tBCD0,0xa0	;initialize digits 1 and 0
; 0001 0094 binbcd_1:	subi	tBCD0,0x10	;
binbcd_1:	subi	tBCD0,0x10	;
; 0001 0095 		subi	fbinL,-10	;addi fbin,10
		subi	fbinL,-10	;addi fbin,10
; 0001 0096 		brcs	binBCD_1	;
		brcs	binBCD_1	;
; 0001 0097 		add	tBCD0,fbinL	;LSD
		add	tBCD0,fbinL	;LSD
; 0001 0098 binBCD_ret:
binBCD_ret:
; 0001 0099 

; 0001 009A         ldd     r31,y+1// loading buffer's address
        ldd     r31,y+1// loading buffer's address
; 0001 009B         ld      r30,y
        ld      r30,y
; 0001 009C 

; 0001 009D 		MOV		temp,tBCD0
		MOV		temp,tBCD0
; 0001 009E 		ANDI	tBCD0,0X0F
		ANDI	tBCD0,0X0F
; 0001 009F 		SUBI	tBCD0,-0x30
		SUBI	tBCD0,-0x30
; 0001 00A0         push    tBCD0
        push    tBCD0
; 0001 00A1 

; 0001 00A2 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 00A3 		SWAP	temp
		SWAP	temp
; 0001 00A4 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 00A5         push    temp
        push    temp
; 0001 00A6 

; 0001 00A7 		MOV		temp,tBCD1
		MOV		temp,tBCD1
; 0001 00A8 		ANDI	tBCD1,0X0F
		ANDI	tBCD1,0X0F
; 0001 00A9 		SUBI	tBCD1,-0x30
		SUBI	tBCD1,-0x30
; 0001 00AA         push    tBCD1
        push    tBCD1
; 0001 00AB 

; 0001 00AC 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 00AD 		SWAP	temp
		SWAP	temp
; 0001 00AE 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 00AF         push    temp
        push    temp
; 0001 00B0 

; 0001 00B1 		MOV		temp,tBCD2
		MOV		temp,tBCD2
; 0001 00B2 		ANDI	tBCD2,0X0F
		ANDI	tBCD2,0X0F
; 0001 00B3 		SUBI	tBCD2,-0x30
		SUBI	tBCD2,-0x30
; 0001 00B4         push    tBCD2
        push    tBCD2
; 0001 00B5 

; 0001 00B6         pop     temp
        pop     temp
; 0001 00B7         st      z+,temp
        st      z+,temp
; 0001 00B8         pop     temp
        pop     temp
; 0001 00B9         st      z+,temp
        st      z+,temp
; 0001 00BA         pop     temp
        pop     temp
; 0001 00BB         st      z+,temp
        st      z+,temp
; 0001 00BC         pop     temp
        pop     temp
; 0001 00BD         st      z+,temp
        st      z+,temp
; 0001 00BE         pop     temp
        pop     temp
; 0001 00BF         st      z+,temp
        st      z+,temp
; 0001 00C0 

; 0001 00C1 #endasm
; 0001 00C2 }
	ADIW R28,4
	RET
;
;void utoa32(unsigned long num,char *buff)
; 0001 00C5 {
_utoa32:
; 0001 00C6 /***************************************************************************
; 0001 00C7 ;*
; 0001 00C8 ;* Bin4BCD == 32-bit Binary to BCD conversion
; 0001 00C9 ;*
; 0001 00CA ;* Important:   You have to disable the autamtic global register allocation in the compiler settings.
; 0001 00CB ;*              Only (R0,R1,R22,R23 ,R24,R25 ,R26,R27,R30,R31) can be used in ASM routines.
; 0001 00CC ;*              you have to save the other registers in the stack if wanna use them.
; 0001 00CD ;*
; 0001 00CE ;**************************************************************************/
; 0001 00CF //R0,R1, R22,R23  R24,R25 ,R26,R27   R30,R31
; 0001 00D0 #asm
;	num -> Y+2
;	*buff -> Y+0
; 0001 00D1 #define	fbin0	r22	; binary value byte 0 (LSB)
#define	fbin0	r22	; binary value byte 0 (LSB)
; 0001 00D2 #define	fbin1	r23	; binary value byte 1
#define	fbin1	r23	; binary value byte 1
; 0001 00D3 #define	fbin2	r24	; binary value byte 2
#define	fbin2	r24	; binary value byte 2
; 0001 00D4 #define	fbin3	r25	; binary value byte 3 (MSB)
#define	fbin3	r25	; binary value byte 3 (MSB)
; 0001 00D5 #define	tBCD0	r16	; BCD value digits 0 and 1
#define	tBCD0	r16	; BCD value digits 0 and 1
; 0001 00D6 #define	tBCD1	r17	; BCD value digits 2 and 3
#define	tBCD1	r17	; BCD value digits 2 and 3
; 0001 00D7 #define	tBCD2	r18	; BCD value digits 4 and 5
#define	tBCD2	r18	; BCD value digits 4 and 5
; 0001 00D8 #define	tBCD3	r19	; BCD value digits 6 and 7
#define	tBCD3	r19	; BCD value digits 6 and 7
; 0001 00D9 #define	tBCD4	r20	; BCD value digits 8 and 9 (MSD)
#define	tBCD4	r20	; BCD value digits 8 and 9 (MSD)
; 0001 00DA #define	TEMP    r22
#define	TEMP    r22
; 0001 00DB 

; 0001 00DC 

; 0001 00DD Bin4BCD:
Bin4BCD:
; 0001 00DE         push    r16
        push    r16
; 0001 00DF         push    r17
        push    r17
; 0001 00E0         push    r18
        push    r18
; 0001 00E1         push    r19
        push    r19
; 0001 00E2         push    r20
        push    r20
; 0001 00E3 

; 0001 00E4         ldd     fbin3,Y+5 ;//loading the unsigned long
        ldd     fbin3,Y+5 ;//loading the unsigned long
; 0001 00E5         ldd     fbin2,y+4 ;
        ldd     fbin2,y+4 ;
; 0001 00E6         ldd     fbin1,Y+3 ;
        ldd     fbin1,Y+3 ;
; 0001 00E7         ldd     fbin0,y+2 ;
        ldd     fbin0,y+2 ;
; 0001 00E8 

; 0001 00E9 Bin4BCD16:	clr	tBCD0		;initial result (5 bytes)
Bin4BCD16:	clr	tBCD0		;initial result (5 bytes)
; 0001 00EA 		clr	tBCD1		;	& shift
		clr	tBCD1		;	& shift
; 0001 00EB 		clr	tBCD2		;	       loop
		clr	tBCD2		;	       loop
; 0001 00EC 		ldi	tBCD3,0xfe	;		   counter
		ldi	tBCD3,0xfe	;		   counter
; 0001 00ED 		ldi	tBCD4,0xff	;			  too
		ldi	tBCD4,0xff	;			  too
; 0001 00EE 		rjmp	binbcd_jump	;for speed-up and skip of MSD corr
		rjmp	binbcd_jump	;for speed-up and skip of MSD corr
; 0001 00EF binbcd_876:	subi	tBCD4,-0x03	;add 0x03 to digit 8 only
binbcd_876:	subi	tBCD4,-0x03	;add 0x03 to digit 8 only
; 0001 00F0 		sbrs	tBCD4,3 	;if bit 3 clear
		sbrs	tBCD4,3 	;if bit 3 clear
; 0001 00F1 		subi	tBCD4,0x03	;	sub 3
		subi	tBCD4,0x03	;	sub 3
; 0001 00F2 		subi	tBCD3,-0x33	;add 0x33 to digits 7 and 6
		subi	tBCD3,-0x33	;add 0x33 to digits 7 and 6
; 0001 00F3 		sbrs	tBCD3,3 	;if bit 3 clear
		sbrs	tBCD3,3 	;if bit 3 clear
; 0001 00F4 		subi	tBCD3,0x03	;	sub 3
		subi	tBCD3,0x03	;	sub 3
; 0001 00F5 		sbrs	tBCD3,7 	;if bit 7 clear
		sbrs	tBCD3,7 	;if bit 7 clear
; 0001 00F6 		subi	tBCD3,0x30	;	sub $30
		subi	tBCD3,0x30	;	sub $30
; 0001 00F7 binbcd_54:	subi	tBCD2,-0x33	;add 0x33 to digits 5 and 4
binbcd_54:	subi	tBCD2,-0x33	;add 0x33 to digits 5 and 4
; 0001 00F8 		sbrs	tBCD2,3 	;if bit 3 clear
		sbrs	tBCD2,3 	;if bit 3 clear
; 0001 00F9 		subi	tBCD2,0x03	;	sub 3
		subi	tBCD2,0x03	;	sub 3
; 0001 00FA 		sbrs	tBCD2,7 	;if bit 7 clear
		sbrs	tBCD2,7 	;if bit 7 clear
; 0001 00FB 		subi	tBCD2,0x30	;	sub $30
		subi	tBCD2,0x30	;	sub $30
; 0001 00FC binbcd_3210:	subi	tBCD1,-0x33	;add 0x33 to digits 3 and 2
binbcd_3210:	subi	tBCD1,-0x33	;add 0x33 to digits 3 and 2
; 0001 00FD 		sbrs	tBCD1,3 	;if bit 3 clear
		sbrs	tBCD1,3 	;if bit 3 clear
; 0001 00FE 		subi	tBCD1,0x03	;	sub 3
		subi	tBCD1,0x03	;	sub 3
; 0001 00FF 		sbrs	tBCD1,7 	;if bit 7 clear
		sbrs	tBCD1,7 	;if bit 7 clear
; 0001 0100 		subi	tBCD1,0x30	;	sub $30
		subi	tBCD1,0x30	;	sub $30
; 0001 0101 		subi	tBCD0,-0x33	;add 0x33 to digits 1 and 0
		subi	tBCD0,-0x33	;add 0x33 to digits 1 and 0
; 0001 0102 		sbrs	tBCD0,3 	;if bit 3 clear
		sbrs	tBCD0,3 	;if bit 3 clear
; 0001 0103 		subi	tBCD0,0x03	;	sub 3
		subi	tBCD0,0x03	;	sub 3
; 0001 0104 		sbrs	tBCD0,7 	;if bit 7 clear
		sbrs	tBCD0,7 	;if bit 7 clear
; 0001 0105 		subi	tBCD0,0x30	;	sub $30
		subi	tBCD0,0x30	;	sub $30
; 0001 0106 binbcd_jump:	lsl	fbin0		;
binbcd_jump:	lsl	fbin0		;
; 0001 0107 		rol	fbin1		;
		rol	fbin1		;
; 0001 0108 		rol	fbin2		;
		rol	fbin2		;
; 0001 0109 		rol	fbin3		;shift input value
		rol	fbin3		;shift input value
; 0001 010A 		rol	tBCD0		;through all bytes
		rol	tBCD0		;through all bytes
; 0001 010B 		rol	tBCD1		;
		rol	tBCD1		;
; 0001 010C 		rol	tBCD2		;
		rol	tBCD2		;
; 0001 010D 		rol	tBCD3		;
		rol	tBCD3		;
; 0001 010E 		rol	tBCD4		;
		rol	tBCD4		;
; 0001 010F 		brcs	binbcd_3210	;16_lsl w/o correction of dig_87654
		brcs	binbcd_3210	;16_lsl w/o correction of dig_87654
; 0001 0110 		inc	fbin0		;
		inc	fbin0		;
; 0001 0111 		brpl	binbcd_54	;+7_lsl w/o correction of dig_876
		brpl	binbcd_54	;+7_lsl w/o correction of dig_876
; 0001 0112 		sbrs	fbin2,0 	;
		sbrs	fbin2,0 	;
; 0001 0113 		rjmp	binbcd_876	;32_lsl in total (fbin = 0x1ffff)
		rjmp	binbcd_876	;32_lsl in total (fbin = 0x1ffff)
; 0001 0114 

; 0001 0115 

; 0001 0116         ldd     r31,y+1//loading the buffer's address
        ldd     r31,y+1//loading the buffer's address
; 0001 0117         ld      r30,y
        ld      r30,y
; 0001 0118 

; 0001 0119 		MOV		temp,tBCD0
		MOV		temp,tBCD0
; 0001 011A 		ANDI	tBCD0,0X0F
		ANDI	tBCD0,0X0F
; 0001 011B 		SUBI	tBCD0,-0x30
		SUBI	tBCD0,-0x30
; 0001 011C         push    tBCD0
        push    tBCD0
; 0001 011D 

; 0001 011E 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 011F 		SWAP	temp
		SWAP	temp
; 0001 0120 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 0121         push    temp
        push    temp
; 0001 0122 

; 0001 0123 		MOV		temp,tBCD1
		MOV		temp,tBCD1
; 0001 0124 		ANDI	tBCD1,0X0F
		ANDI	tBCD1,0X0F
; 0001 0125 		SUBI	tBCD1,-0x30
		SUBI	tBCD1,-0x30
; 0001 0126         push    tBCD1
        push    tBCD1
; 0001 0127 

; 0001 0128 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 0129 		SWAP	temp
		SWAP	temp
; 0001 012A 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 012B         push    temp
        push    temp
; 0001 012C 

; 0001 012D 		MOV		temp,tBCD2
		MOV		temp,tBCD2
; 0001 012E 		ANDI	tBCD2,0X0F
		ANDI	tBCD2,0X0F
; 0001 012F 		SUBI	tBCD2,-0x30
		SUBI	tBCD2,-0x30
; 0001 0130         push    tBCD2
        push    tBCD2
; 0001 0131 

; 0001 0132 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 0133 		SWAP	temp
		SWAP	temp
; 0001 0134 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 0135         push    temp
        push    temp
; 0001 0136 

; 0001 0137 		MOV		temp,tBCD3
		MOV		temp,tBCD3
; 0001 0138 		ANDI	tBCD3,0X0F
		ANDI	tBCD3,0X0F
; 0001 0139 		SUBI	tBCD3,-0x30
		SUBI	tBCD3,-0x30
; 0001 013A         push    tBCD3
        push    tBCD3
; 0001 013B 

; 0001 013C 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 013D 		SWAP	temp
		SWAP	temp
; 0001 013E 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 013F         push    temp
        push    temp
; 0001 0140 

; 0001 0141 		MOV		temp,tBCD4
		MOV		temp,tBCD4
; 0001 0142 		ANDI	tBCD4,0X0F
		ANDI	tBCD4,0X0F
; 0001 0143 		SUBI	tBCD4,-0x30
		SUBI	tBCD4,-0x30
; 0001 0144         push    tBCD4
        push    tBCD4
; 0001 0145 

; 0001 0146 		ANDI	temp,0XF0
		ANDI	temp,0XF0
; 0001 0147 		SWAP	temp
		SWAP	temp
; 0001 0148 		SUBI	temp,-0x30
		SUBI	temp,-0x30
; 0001 0149         push    temp
        push    temp
; 0001 014A 

; 0001 014B         pop     temp
        pop     temp
; 0001 014C         st      z+,temp
        st      z+,temp
; 0001 014D         pop     temp
        pop     temp
; 0001 014E         st      z+,temp
        st      z+,temp
; 0001 014F         pop     temp
        pop     temp
; 0001 0150         st      z+,temp
        st      z+,temp
; 0001 0151         pop     temp
        pop     temp
; 0001 0152         st      z+,temp
        st      z+,temp
; 0001 0153         pop     temp
        pop     temp
; 0001 0154         st      z+,temp
        st      z+,temp
; 0001 0155         pop     temp
        pop     temp
; 0001 0156         st      z+,temp
        st      z+,temp
; 0001 0157         pop     temp
        pop     temp
; 0001 0158         st      z+,temp
        st      z+,temp
; 0001 0159         pop     temp
        pop     temp
; 0001 015A         st      z+,temp
        st      z+,temp
; 0001 015B         pop     temp
        pop     temp
; 0001 015C         st      z+,temp
        st      z+,temp
; 0001 015D         pop     temp
        pop     temp
; 0001 015E         st      z+,temp
        st      z+,temp
; 0001 015F //Recovering the registers //Other registers will recover by the compiler
//Recovering the registers //Other registers will recover by the compiler
; 0001 0160         pop     r20
        pop     r20
; 0001 0161         pop     r19
        pop     r19
; 0001 0162         pop     r18
        pop     r18
; 0001 0163         pop     r17
        pop     r17
; 0001 0164         pop     r16
        pop     r16
; 0001 0165 #endasm
; 0001 0166 }
	ADIW R28,6
	RET

	.CSEG

	.CSEG
__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
	RET

;END OF CODE MARKER
__END_OF_CODE:
