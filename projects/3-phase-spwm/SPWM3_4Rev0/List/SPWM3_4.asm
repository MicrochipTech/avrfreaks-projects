
;CodeVisionAVR C Compiler V2.05.0 Professional
;(C) Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type                : ATmega64
;Program type             : Application
;Clock frequency          : 8.000000 MHz
;Memory model             : Small
;Optimize for             : Speed
;(s)printf features       : int, width
;(s)scanf features        : int, width
;External RAM size        : 0
;Data Stack size          : 1024 byte(s)
;Heap size                : 0 byte(s)
;Promote 'char' to 'int'  : Yes
;'char' is unsigned       : Yes
;8 bit enums              : No
;global 'const' stored in FLASH: Yes
;Enhanced core instructions    : On
;Smart register allocation     : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega64
	#pragma AVRPART MEMORY PROG_FLASH 65536
	#pragma AVRPART MEMORY EEPROM 2048
	#pragma AVRPART MEMORY INT_SRAM SIZE 4351
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x100

	#define CALL_SUPPORTED 1

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
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F
	.EQU XMCRA=0x6D
	.EQU XMCRB=0x6C

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

	.EQU __SRAM_START=0x0100
	.EQU __SRAM_END=0x10FF
	.EQU __DSTACK_SIZE=0x0400
	.EQU __HEAP_SIZE=0x0000
	.EQU __CLEAR_SRAM_SIZE=__SRAM_END-__SRAM_START+1

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
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ANDI R30,HIGH(@2)
	STS  @0+(@1)+1,R30
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
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ORI  R30,HIGH(@2)
	STS  @0+(@1)+1,R30
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

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
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

	.MACRO __PUTDZ2
	STD  Z+@0,R26
	STD  Z+@0+1,R27
	STD  Z+@0+2,R24
	STD  Z+@0+3,R25
	.ENDM

	.MACRO __CLRD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+(@1))
	LDI  R31,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	LDI  R22,BYTE3(2*@0+(@1))
	LDI  R23,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+(@2))
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+(@3))
	LDI  R@1,HIGH(@2+(@3))
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+(@3))
	LDI  R@1,HIGH(@2*2+(@3))
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

	.MACRO __GETB1MN
	LDS  R30,@0+(@1)
	.ENDM

	.MACRO __GETB1HMN
	LDS  R31,@0+(@1)
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	LDS  R22,@0+(@1)+2
	LDS  R23,@0+(@1)+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@0,@1+(@2)
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+(@3)
	LDS  R@1,@2+(@3)+1
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
	LDS  R26,@0+(@1)
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	LDS  R24,@0+(@1)+2
	LDS  R25,@0+(@1)+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+(@1),R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	STS  @0+(@1)+2,R22
	STS  @0+(@1)+3,R23
	.ENDM

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRD
	.ENDM

	.MACRO __PUTBR0MN
	STS  @0+(@1),R0
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+(@1),R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+(@1),R@2
	STS  @0+(@1)+1,R@3
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

	.MACRO __PUTBSR
	STD  Y+@1,R@0
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
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	ICALL
	.ENDM

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	CALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
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

	.MACRO __GETD1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X+
	LD   R22,X
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

	.MACRO __GETBRSX2
	MOVW R26,R28
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	LD   R@0,X
	.ENDM

	.MACRO __GETWRSX2
	MOVW R26,R28
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	LD   R@0,X+
	LD   R@1,X
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
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R30
	.ENDM

	.MACRO __CLRD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R30
	ST   X+,R30
	ST   X,R30
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
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	ST   Z,R@0
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

;START OF CODE MARKER
__START_OF_CODE:

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
	JMP  _timer2_compare_isr
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
	JMP  _ADC_INT_isr
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  _timer3_comparea_isr
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00

_sine:
	.DB  0x81,0x84,0x87,0x8A,0x8E,0x91,0x94,0x97
	.DB  0x9A,0x9D,0xA0,0xA3,0xA6,0xA9,0xAC,0xAF
	.DB  0xB2,0xB5,0xB7,0xBA,0xBD,0xC0,0xC2,0xC5
	.DB  0xC8,0xCA,0xCD,0xCF,0xD2,0xD4,0xD6,0xD9
	.DB  0xDB,0xDD,0xDF,0xE1,0xE3,0xE5,0xE7,0xE9
	.DB  0xEA,0xEC,0xEE,0xEF,0xF1,0xF2,0xF3,0xF5
	.DB  0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFC
	.DB  0xFD,0xFD,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF
	.DB  0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,0xFD,0xFD
	.DB  0xFC,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6
	.DB  0xF5,0xF3,0xF2,0xF1,0xEF,0xEE,0xEC,0xEA
	.DB  0xE9,0xE7,0xE5,0xE3,0xE1,0xDF,0xDD,0xDB
	.DB  0xD9,0xD6,0xD4,0xD2,0xCF,0xCD,0xCA,0xC8
	.DB  0xC5,0xC2,0xC0,0xBD,0xBA,0xB7,0xB5,0xB2
	.DB  0xAF,0xAC,0xA9,0xA6,0xA3,0xA0,0x9D,0x9A
	.DB  0x97,0x94,0x91,0x8E,0x8A,0x87,0x84,0x81
	.DB  0x7E,0x7B,0x78,0x75,0x71,0x6E,0x6B,0x68
	.DB  0x65,0x62,0x5F,0x5C,0x59,0x56,0x53,0x50
	.DB  0x4D,0x4A,0x48,0x45,0x42,0x3F,0x3D,0x3A
	.DB  0x37,0x35,0x32,0x30,0x2D,0x2B,0x29,0x26
	.DB  0x24,0x22,0x20,0x1E,0x1C,0x1A,0x18,0x16
	.DB  0x15,0x13,0x11,0x10,0xE,0xD,0xC,0xA
	.DB  0x9,0x8,0x7,0x6,0x5,0x4,0x3,0x3
	.DB  0x2,0x2,0x1,0x1,0x0,0x0,0x0,0x0
	.DB  0x0,0x0,0x0,0x0,0x1,0x1,0x2,0x2
	.DB  0x3,0x3,0x4,0x5,0x6,0x7,0x8,0x9
	.DB  0xA,0xC,0xD,0xE,0x10,0x11,0x13,0x15
	.DB  0x16,0x18,0x1A,0x1C,0x1E,0x20,0x22,0x24
	.DB  0x26,0x29,0x2B,0x2D,0x30,0x32,0x35,0x37
	.DB  0x3A,0x3D,0x3F,0x42,0x45,0x48,0x4A,0x4D
	.DB  0x50,0x53,0x56,0x59,0x5C,0x5F,0x62,0x65
	.DB  0x68,0x6B,0x6E,0x71,0x75,0x78,0x7B,0x7E
_OCR_value:
	.DB  0x23,0xF4,0x72,0xCB,0x62,0xAE,0x95,0x98
	.DB  0xA1,0x87,0x11,0x7A,0xF8,0x6E,0xB9,0x65
	.DB  0xE5,0x5D,0x30,0x57,0x60,0x51,0x4A,0x4C
	.DB  0xCD,0x47,0xD0,0x43,0x3E,0x40,0x8,0x3D
	.DB  0x20,0x3A,0x7C,0x37,0x12,0x35,0xDC,0x32
	.DB  0xD3,0x30,0xF2,0x2E,0x35,0x2D,0x98,0x2B
	.DB  0x17,0x2A,0xB0,0x28,0x60,0x27,0x25,0x26
	.DB  0xFD,0x24,0xE6,0x23,0xE0,0x22,0xE8,0x21
	.DB  0xFD,0x20,0x1F,0x20,0x4C,0x1F,0x83,0x1E
	.DB  0xC5,0x1D,0xF,0x1D,0x62,0x1C,0xBD,0x1B
	.DB  0x1F,0x1B,0x88,0x1A,0xF8,0x19,0x6D,0x19
	.DB  0xE9,0x18,0x69,0x18,0xEE,0x17,0x79,0x17
	.DB  0x7,0x17,0x9A,0x16,0x31,0x16,0xCB,0x15
	.DB  0x69,0x15,0xB,0x15,0xB0,0x14,0x57,0x14
	.DB  0x2,0x14,0xAF,0x13,0x5F,0x13,0x12,0x13
	.DB  0xC7,0x12,0x7E,0x12,0x37,0x12,0xF3,0x11
	.DB  0xB0,0x11,0x6F,0x11,0x30,0x11,0xF3,0x10
	.DB  0xB8,0x10,0x7E,0x10,0x46,0x10,0xF,0x10
	.DB  0xD9,0xF,0xA5,0xF,0x73,0xF,0x41,0xF
	.DB  0x11,0xF,0xE2,0xE,0xB4,0xE,0x87,0xE
	.DB  0x5B,0xE,0x31,0xE,0x7,0xE,0xDE,0xD
	.DB  0xB6,0xD,0x8F,0xD,0x69,0xD,0x44,0xD
	.DB  0x1F,0xD,0xFB,0xC,0xD8,0xC,0xB6,0xC
	.DB  0x95,0xC,0x74,0xC,0x54,0xC,0x34,0xC
	.DB  0x15,0xC,0xF7,0xB,0xD9,0xB,0xBC,0xB
	.DB  0x9F,0xB,0x83,0xB,0x68,0xB,0x4D,0xB
	.DB  0x32,0xB,0x18,0xB,0xFE,0xA,0xE5,0xA
	.DB  0xCC,0xA,0xB4,0xA,0x9C,0xA,0x85,0xA
	.DB  0x6E,0xA,0x57,0xA,0x41,0xA,0x2B,0xA
	.DB  0x16,0xA,0x0,0xA,0xEC,0x9,0xD7,0x9
	.DB  0xC3,0x9,0xAF,0x9,0x9C,0x9,0x88,0x9
	.DB  0x75,0x9,0x63,0x9,0x50,0x9,0x3E,0x9
	.DB  0x2D,0x9,0x1B,0x9,0xA,0x9,0xF9,0x8
	.DB  0xE8,0x8,0xD7,0x8,0xC7,0x8,0xB7,0x8
	.DB  0xA7,0x8,0x98,0x8,0x88,0x8,0x79,0x8
	.DB  0x6A,0x8,0x5B,0x8,0x4D,0x8,0x3E,0x8
	.DB  0x30,0x8,0x22,0x8,0x15,0x8,0x7,0x8
	.DB  0xF9,0x7,0xEC,0x7,0xDF,0x7,0xD2,0x7
	.DB  0xC5,0x7,0xB9,0x7,0xAC,0x7,0xA0,0x7
	.DB  0x94,0x7,0x88,0x7,0x7C,0x7,0x70,0x7
	.DB  0x65,0x7,0x5A,0x7,0x4E,0x7,0x43,0x7
	.DB  0x38,0x7,0x2D,0x7,0x22,0x7,0x18,0x7
	.DB  0xD,0x7,0x3,0x7,0xF9,0x6,0xEF,0x6
	.DB  0xE5,0x6,0xDB,0x6,0xD1,0x6,0xC7,0x6
	.DB  0xBE,0x6,0xB4,0x6,0xAB,0x6,0xA1,0x6
	.DB  0x98,0x6,0x8F,0x6,0x86,0x6,0x7D,0x6
	.DB  0x74,0x6,0x6C,0x6,0x63,0x6,0x5B,0x6
	.DB  0x52,0x6,0x4A,0x6,0x42,0x6,0x39,0x6
	.DB  0x31,0x6,0x29,0x6,0x21,0x6,0x34,0xC
	.DB  0x24,0xC,0x15,0xC,0x6,0xC,0xF7,0xB
	.DB  0xE8,0xB,0xD9,0xB,0xCA,0xB,0xBC,0xB
	.DB  0xAD,0xB,0x9F,0xB,0x91,0xB,0x83,0xB
	.DB  0x75,0xB,0x68,0xB,0x5A,0xB,0x4D,0xB
	.DB  0x3F,0xB,0x32,0xB,0x25,0xB,0x18,0xB
	.DB  0xB,0xB,0xFE,0xA,0xF2,0xA,0xE5,0xA
	.DB  0xD9,0xA,0xCC,0xA,0xC0,0xA,0xB4,0xA
	.DB  0xA8,0xA,0x9C,0xA,0x91,0xA,0x85,0xA
	.DB  0x79,0xA,0x6E,0xA,0x63,0xA,0x57,0xA
	.DB  0x4C,0xA,0x41,0xA,0x36,0xA,0x2B,0xA
	.DB  0x20,0xA,0x16,0xA,0xB,0xA,0x0,0xA
	.DB  0xF6,0x9,0xEC,0x9,0xE1,0x9,0xD7,0x9
	.DB  0xCD,0x9,0xC3,0x9,0xB9,0x9,0xAF,0x9
	.DB  0xA5,0x9,0x9C,0x9,0x92,0x9,0x88,0x9
	.DB  0x7F,0x9,0x75,0x9,0x6C,0x9,0x63,0x9
	.DB  0x5A,0x9,0x50,0x9,0x47,0x9,0x3E,0x9
	.DB  0x35,0x9,0x2D,0x9,0x24,0x9,0x1B,0x9
	.DB  0x12,0x9,0xA,0x9,0x1,0x9,0xF9,0x8
	.DB  0xF0,0x8,0xE8,0x8,0xE0,0x8,0xD7,0x8
	.DB  0xCF,0x8,0xC7,0x8,0xBF,0x8,0xB7,0x8
	.DB  0xAF,0x8,0xA7,0x8,0x9F,0x8,0x98,0x8
	.DB  0x90,0x8,0x88,0x8,0x81,0x8,0x79,0x8
	.DB  0x72,0x8,0x6A,0x8,0x63,0x8,0x5B,0x8
	.DB  0x54,0x8,0x4D,0x8,0x46,0x8,0x3E,0x8
	.DB  0x37,0x8,0x30,0x8,0x29,0x8,0x22,0x8
	.DB  0x1B,0x8,0x15,0x8,0xE,0x8,0x7,0x8
	.DB  0x0,0x8,0xF9,0x7,0xF3,0x7,0xEC,0x7
	.DB  0xE6,0x7,0xDF,0x7,0xD9,0x7,0xD2,0x7
	.DB  0xCC,0x7,0xC5,0x7,0xBF,0x7,0xB9,0x7
	.DB  0xB3,0x7,0xAC,0x7,0xA6,0x7,0xA0,0x7
	.DB  0x9A,0x7,0x94,0x7,0x8E,0x7,0x88,0x7
	.DB  0x82,0x7,0x7C,0x7,0x76,0x7,0x70,0x7
	.DB  0x6B,0x7,0x65,0x7,0x5F,0x7,0x5A,0x7
	.DB  0x54,0x7,0x4E,0x7,0x49,0x7,0x43,0x7
	.DB  0x3E,0x7,0x38,0x7,0x33,0x7,0x2D,0x7
	.DB  0x28,0x7,0x22,0x7,0x1D,0x7,0x18,0x7
	.DB  0x13,0x7,0xD,0x7,0x8,0x7,0x3,0x7
	.DB  0xFE,0x6,0xF9,0x6,0xF4,0x6,0xEF,0x6
	.DB  0xEA,0x6,0xE5,0x6,0xE0,0x6,0xDB,0x6
	.DB  0xD6,0x6,0xD1,0x6,0xCC,0x6,0xC7,0x6
	.DB  0xC2,0x6,0xBE,0x6,0xB9,0x6,0xB4,0x6
	.DB  0xAF,0x6,0xAB,0x6,0xA6,0x6,0xA1,0x6
	.DB  0x9D,0x6,0x98,0x6,0x94,0x6,0x8F,0x6
	.DB  0x8B,0x6,0x86,0x6,0x82,0x6,0x7D,0x6
	.DB  0x79,0x6,0x74,0x6,0x70,0x6,0x6C,0x6
	.DB  0x67,0x6,0x63,0x6,0x5F,0x6,0x5B,0x6
	.DB  0x56,0x6,0x52,0x6,0x4E,0x6,0x4A,0x6
	.DB  0x46,0x6,0x42,0x6,0x3D,0x6,0x39,0x6
	.DB  0x35,0x6,0x31,0x6,0x2D,0x6,0x29,0x6
	.DB  0x25,0x6,0x21,0x6,0x1D,0x6,0x34,0xC
	.DB  0x2C,0xC,0x24,0xC,0x1D,0xC,0x15,0xC
	.DB  0xD,0xC,0x6,0xC,0xFE,0xB,0xF7,0xB
	.DB  0xEF,0xB,0xE8,0xB,0xE0,0xB,0xD9,0xB
	.DB  0xD2,0xB,0xCA,0xB,0xC3,0xB,0xBC,0xB
	.DB  0xB5,0xB,0xAD,0xB,0xA6,0xB,0x9F,0xB
	.DB  0x98,0xB,0x91,0xB,0x8A,0xB,0x83,0xB
	.DB  0x7C,0xB,0x75,0xB,0x6E,0xB,0x68,0xB
	.DB  0x61,0xB,0x5A,0xB,0x53,0xB,0x4D,0xB
	.DB  0x46,0xB,0x3F,0xB,0x39,0xB,0x32,0xB
	.DB  0x2B,0xB,0x25,0xB,0x1E,0xB,0x18,0xB
	.DB  0x11,0xB,0xB,0xB,0x5,0xB,0xFE,0xA
	.DB  0xF8,0xA,0xF2,0xA,0xEB,0xA,0xE5,0xA
	.DB  0xDF,0xA,0xD9,0xA,0xD3,0xA,0xCC,0xA
	.DB  0xC6,0xA,0xC0,0xA,0xBA,0xA,0xB4,0xA
	.DB  0xAE,0xA,0xA8,0xA,0xA2,0xA,0x9C,0xA
	.DB  0x96,0xA,0x91,0xA,0x8B,0xA,0x85,0xA
	.DB  0x7F,0xA,0x79,0xA,0x74,0xA,0x6E,0xA
	.DB  0x68,0xA,0x63,0xA,0x5D,0xA,0x57,0xA
	.DB  0x52,0xA,0x4C,0xA,0x47,0xA,0x41,0xA
	.DB  0x3C,0xA,0x36,0xA,0x31,0xA,0x2B,0xA
	.DB  0x26,0xA,0x20,0xA,0x1B,0xA,0x16,0xA
	.DB  0x10,0xA,0xB,0xA,0x6,0xA,0x0,0xA
	.DB  0xFB,0x9,0xF6,0x9,0xF1,0x9,0xEC,0x9
	.DB  0xE6,0x9,0xE1,0x9,0xDC,0x9,0xD7,0x9
	.DB  0xD2,0x9,0xCD,0x9,0xC8,0x9,0xC3,0x9
	.DB  0xBE,0x9,0xB9,0x9,0xB4,0x9,0xAF,0x9
	.DB  0xAA,0x9,0xA5,0x9,0xA0,0x9,0x9C,0x9
	.DB  0x97,0x9,0x92,0x9,0x8D,0x9,0x88,0x9
	.DB  0x84,0x9,0x7F,0x9,0x7A,0x9,0x75,0x9
	.DB  0x71,0x9,0x6C,0x9,0x67,0x9,0x63,0x9
	.DB  0x5E,0x9,0x5A,0x9,0x55,0x9,0x50,0x9
	.DB  0x4C,0x9,0x47,0x9,0x43,0x9,0x3E,0x9
	.DB  0x3A,0x9,0x35,0x9,0x31,0x9,0x2D,0x9
	.DB  0x28,0x9,0x24,0x9,0x1F,0x9,0x1B,0x9
	.DB  0x17,0x9,0x12,0x9,0xE,0x9,0xA,0x9
	.DB  0x6,0x9,0x1,0x9,0xFD,0x8,0xF9,0x8
	.DB  0xF5,0x8,0xF0,0x8,0xEC,0x8,0xE8,0x8
	.DB  0xE4,0x8,0xE0,0x8,0xDC,0x8,0xD7,0x8
	.DB  0xD3,0x8,0xCF,0x8,0xCB,0x8,0xC7,0x8
	.DB  0xC3,0x8,0xBF,0x8,0xBB,0x8,0xB7,0x8
	.DB  0xB3,0x8,0xAF,0x8,0xAB,0x8,0xA7,0x8
	.DB  0xA3,0x8,0x9F,0x8,0x9C,0x8,0x98,0x8
	.DB  0x94,0x8,0x90,0x8,0x8C,0x8,0x88,0x8
	.DB  0x85,0x8,0x81,0x8,0x7D,0x8,0x79,0x8
	.DB  0x75,0x8,0x72,0x8,0x6E,0x8,0x6A,0x8
	.DB  0x66,0x8,0x63,0x8,0x5F,0x8,0x5B,0x8
	.DB  0x58,0x8,0x54,0x8,0x50,0x8,0x4D,0x8
	.DB  0x49,0x8,0x46,0x8,0x42,0x8,0x3E,0x8
	.DB  0x3B,0x8,0x37,0x8,0x34,0x8,0x30,0x8
	.DB  0x2D,0x8,0x29,0x8,0x26,0x8,0x46,0x10
	.DB  0x3F,0x10,0x38,0x10,0x31,0x10,0x2A,0x10
	.DB  0x23,0x10,0x1C,0x10,0x16,0x10,0xF,0x10
	.DB  0x8,0x10,0x1,0x10,0xFB,0xF,0xF4,0xF
	.DB  0xED,0xF,0xE7,0xF,0xE0,0xF,0xD9,0xF
	.DB  0xD3,0xF,0xCC,0xF,0xC6,0xF,0xBF,0xF
	.DB  0xB9,0xF,0xB2,0xF,0xAC,0xF,0xA5,0xF
	.DB  0x9F,0xF,0x99,0xF,0x92,0xF,0x8C,0xF
	.DB  0x86,0xF,0x7F,0xF,0x79,0xF,0x73,0xF
	.DB  0x6C,0xF,0x66,0xF,0x60,0xF,0x5A,0xF
	.DB  0x54,0xF,0x4D,0xF,0x47,0xF,0x41,0xF
	.DB  0x3B,0xF,0x35,0xF,0x2F,0xF,0x29,0xF
	.DB  0x23,0xF,0x1D,0xF,0x17,0xF,0x11,0xF
	.DB  0xB,0xF,0x5,0xF,0xFF,0xE,0xF9,0xE
	.DB  0xF3,0xE,0xEE,0xE,0xE8,0xE,0xE2,0xE
	.DB  0xDC,0xE,0xD6,0xE,0xD1,0xE,0xCB,0xE
	.DB  0xC5,0xE,0xBF,0xE,0xBA,0xE,0xB4,0xE
	.DB  0xAE,0xE,0xA9,0xE,0xA3,0xE,0x9E,0xE
	.DB  0x98,0xE,0x92,0xE,0x8D,0xE,0x87,0xE
	.DB  0x82,0xE,0x7C,0xE,0x77,0xE,0x71,0xE
	.DB  0x6C,0xE,0x66,0xE,0x61,0xE,0x5B,0xE
	.DB  0x56,0xE,0x51,0xE,0x4B,0xE,0x46,0xE
	.DB  0x41,0xE,0x3B,0xE,0x36,0xE,0x31,0xE
	.DB  0x2B,0xE,0x26,0xE,0x21,0xE,0x1C,0xE
	.DB  0x17,0xE,0x11,0xE,0xC,0xE,0x7,0xE
	.DB  0x2,0xE,0xFD,0xD,0xF8,0xD,0xF2,0xD
	.DB  0xED,0xD,0xE8,0xD,0xE3,0xD,0xDE,0xD
	.DB  0xD9,0xD,0xD4,0xD,0xCF,0xD,0xCA,0xD
	.DB  0xC5,0xD,0xC0,0xD,0xBB,0xD,0xB6,0xD
	.DB  0xB1,0xD,0xAC,0xD,0xA8,0xD,0xA3,0xD
	.DB  0x9E,0xD,0x99,0xD,0x94,0xD,0x8F,0xD
	.DB  0x8A,0xD,0x86,0xD,0x81,0xD,0x7C,0xD
	.DB  0x77,0xD,0x73,0xD,0x6E,0xD,0x69,0xD
	.DB  0x64,0xD,0x60,0xD,0x5B,0xD,0x56,0xD
	.DB  0x52,0xD,0x4D,0xD,0x48,0xD,0x44,0xD
	.DB  0x3F,0xD,0x3B,0xD,0x36,0xD,0x31,0xD
	.DB  0x2D,0xD,0x28,0xD,0x24,0xD,0x1F,0xD
	.DB  0x1B,0xD,0x16,0xD,0x12,0xD,0xD,0xD
	.DB  0x9,0xD,0x4,0xD,0x0,0xD,0xFB,0xC
	.DB  0xF7,0xC,0xF3,0xC,0xEE,0xC,0xEA,0xC
	.DB  0xE6,0xC,0xE1,0xC,0xDD,0xC,0xD8,0xC
	.DB  0xD4,0xC,0xD0,0xC,0xCC,0xC,0xC7,0xC
	.DB  0xC3,0xC,0xBF,0xC,0xBA,0xC,0xB6,0xC
	.DB  0xB2,0xC,0xAE,0xC,0xAA,0xC,0xA5,0xC
	.DB  0xA1,0xC,0x9D,0xC,0x99,0xC,0x95,0xC
	.DB  0x91,0xC,0x8C,0xC,0x88,0xC,0x84,0xC
	.DB  0x80,0xC,0x7C,0xC,0x78,0xC,0x74,0xC
	.DB  0x70,0xC,0x6C,0xC,0x68,0xC,0x64,0xC
	.DB  0x60,0xC,0x5C,0xC,0x58,0xC,0x54,0xC
	.DB  0x50,0xC,0x4C,0xC,0x48,0xC,0x44,0xC
	.DB  0x40,0xC,0x3C,0xC,0x38,0xC,0x69,0x18
	.DB  0x61,0x18,0x59,0x18,0x52,0x18,0x4A,0x18
	.DB  0x42,0x18,0x3A,0x18,0x33,0x18,0x2B,0x18
	.DB  0x23,0x18,0x1C,0x18,0x14,0x18,0xD,0x18
	.DB  0x5,0x18,0xFE,0x17,0xF6,0x17,0xEE,0x17
	.DB  0xE7,0x17,0xDF,0x17,0xD8,0x17,0xD1,0x17
	.DB  0xC9,0x17,0xC2,0x17,0xBA,0x17,0xB3,0x17
	.DB  0xAC,0x17,0xA4,0x17,0x9D,0x17,0x96,0x17
	.DB  0x8E,0x17,0x87,0x17,0x80,0x17,0x79,0x17
	.DB  0x71,0x17,0x6A,0x17,0x63,0x17,0x5C,0x17
	.DB  0x55,0x17,0x4E,0x17,0x46,0x17,0x3F,0x17
	.DB  0x38,0x17,0x31,0x17,0x2A,0x17,0x23,0x17
	.DB  0x1C,0x17,0x15,0x17,0xE,0x17,0x7,0x17
	.DB  0x0,0x17,0xF9,0x16,0xF2,0x16,0xEC,0x16
	.DB  0xE5,0x16,0xDE,0x16,0xD7,0x16,0xD0,0x16
	.DB  0xC9,0x16,0xC3,0x16,0xBC,0x16,0xB5,0x16
	.DB  0xAE,0x16,0xA7,0x16,0xA1,0x16,0x9A,0x16
	.DB  0x93,0x16,0x8D,0x16,0x86,0x16,0x7F,0x16
	.DB  0x79,0x16,0x72,0x16,0x6C,0x16,0x65,0x16
	.DB  0x5E,0x16,0x58,0x16,0x51,0x16,0x4B,0x16
	.DB  0x44,0x16,0x3E,0x16,0x37,0x16,0x31,0x16
	.DB  0x2A,0x16,0x24,0x16,0x1E,0x16,0x17,0x16
	.DB  0x11,0x16,0xA,0x16,0x4,0x16,0xFE,0x15
	.DB  0xF7,0x15,0xF1,0x15,0xEB,0x15,0xE4,0x15
	.DB  0xDE,0x15,0xD8,0x15,0xD2,0x15,0xCB,0x15
	.DB  0xC5,0x15,0xBF,0x15,0xB9,0x15,0xB3,0x15
	.DB  0xAC,0x15,0xA6,0x15,0xA0,0x15,0x9A,0x15
	.DB  0x94,0x15,0x8E,0x15,0x88,0x15,0x82,0x15
	.DB  0x7C,0x15,0x76,0x15,0x6F,0x15,0x69,0x15
	.DB  0x63,0x15,0x5D,0x15,0x57,0x15,0x52,0x15
	.DB  0x4C,0x15,0x46,0x15,0x40,0x15,0x3A,0x15
	.DB  0x34,0x15,0x2E,0x15,0x28,0x15,0x22,0x15
	.DB  0x1C,0x15,0x17,0x15,0x11,0x15,0xB,0x15
	.DB  0x5,0x15,0xFF,0x14,0xFA,0x14,0xF4,0x14
	.DB  0xEE,0x14,0xE8,0x14,0xE3,0x14,0xDD,0x14
	.DB  0xD7,0x14,0xD1,0x14,0xCC,0x14,0xC6,0x14
	.DB  0xC0,0x14,0xBB,0x14,0xB5,0x14,0xB0,0x14
	.DB  0xAA,0x14,0xA4,0x14,0x9F,0x14,0x99,0x14
	.DB  0x94,0x14,0x8E,0x14,0x89,0x14,0x83,0x14
	.DB  0x7E,0x14,0x78,0x14,0x73,0x14,0x6D,0x14
	.DB  0x68,0x14,0x62,0x14,0x5D,0x14,0x57,0x14
	.DB  0x52,0x14,0x4D,0x14,0x47,0x14,0x42,0x14
	.DB  0x3C,0x14,0x37,0x14,0x32,0x14,0x2C,0x14
	.DB  0x27,0x14,0x22,0x14,0x1C,0x14,0x17,0x14
	.DB  0x12,0x14,0xC,0x14,0x7,0x14,0x2,0x14
	.DB  0xFD,0x13,0xF7,0x13,0xF2,0x13,0xED,0x13
	.DB  0xE8,0x13,0xE3,0x13,0xDD,0x13,0xD8,0x13
	.DB  0xD3,0x13,0xCE,0x13,0xC9,0x13,0xC4,0x13
	.DB  0xBF,0x13,0xBA,0x13,0xB4,0x13,0xAF,0x13
	.DB  0xAA,0x13,0xA5,0x13,0xA0,0x13,0x9B,0x13
	.DB  0x96,0x13,0x91,0x13,0x8C,0x13,0x87,0x13

_0x3:
	.DB  0x1
_0x4:
	.DB  0x1
_0x5:
	.DB  0x3
_0x6:
	.DB  0x19

__GLOBAL_INI_TBL:
	.DW  0x01
	.DW  _acceleration
	.DW  _0x3*2

	.DW  0x01
	.DW  _deceleration
	.DW  _0x4*2

	.DW  0x01
	.DW  _temp_key_code
	.DW  _0x5*2

	.DW  0x01
	.DW  _a_boost
	.DW  _0x6*2

_0xFFFFFFFF:
	.DW  0

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  MCUCR,R31
	OUT  MCUCR,R30
	STS  XMCRB,R30

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
	LDI  R24,LOW(__CLEAR_SRAM_SIZE)
	LDI  R25,HIGH(__CLEAR_SRAM_SIZE)
	LDI  R26,LOW(__SRAM_START)
	LDI  R27,HIGH(__SRAM_START)
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

;HARDWARE STACK POINTER INITIALIZATION
	LDI  R30,LOW(__SRAM_END-__HEAP_SIZE)
	OUT  SPL,R30
	LDI  R30,HIGH(__SRAM_END-__HEAP_SIZE)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(__SRAM_START+__DSTACK_SIZE)
	LDI  R29,HIGH(__SRAM_START+__DSTACK_SIZE)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x500

	.CSEG
;/*
;   Author : OZHAN KD
;   www.KnowledgePlus.ir
;
;   ---------------------------------------------------------------
;
;   Sep 18, 2013
;
;   ADC0 (PF0): Frequency setting (0.5 Hz - 100 Hz with 0.1Hz step)
;   ADC1 (PF1): Acceleration setting (100Hz frequency change time) (0.99s - 253.7s)
;   ADC2 (PF2): Deceleration setting (100Hz frequency change time) (0.99s - 253.7s)
;   ADC3 (PF3): Boost volatage setting (9.8% - 21.9%)
;
;   PE2: Run push botton (active low)
;   PE3: Stop push botton (active low)
;   PE4: Dir push botton (active low)
;
;   PB5: SPWM phase1
;   PB6: SPWM phase2
;   PB7: SPWM phase3
;
;   PD0: Motor enable output (active high)
;   PD1: IR2130 fault indicator input (active low)
;   PD2: Run LED (active low)
;   PD3: Motor direction LED1 (active low)
;   PD4: Motor direction LED2 (active low)
;
;   mega64 oscillator frequency = 8MHz
;   Carrier frequency = 15.6 KHz
;
;   ---------------------------------------------------------------
;
;*/
;
;#include <mega64.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.EQU __sm_mask=0x1C
	.EQU __sm_powerdown=0x10
	.EQU __sm_powersave=0x18
	.EQU __sm_standby=0x14
	.EQU __sm_ext_standby=0x1C
	.EQU __sm_adc_noise_red=0x08
	.SET power_ctrl_reg=mcucr
	#endif
;#include <delay.h>
;#include "defines.h"
;#include "tables.c"
;flash unsigned char sine[256]={
;
; 129 , 132 , 135 , 138 , 142 , 145 , 148 , 151 , 154 , 157 , 160 , 163 , 166 , 169 , 172 , 175 ,
; 178 , 181 , 183 , 186 , 189 , 192 , 194 , 197 , 200 , 202 , 205 , 207 , 210 , 212 , 214 , 217 ,
; 219 , 221 , 223 , 225 , 227 , 229 , 231 , 233 , 234 , 236 , 238 , 239 , 241 , 242 , 243 , 245 ,
; 246 , 247 , 248 , 249 , 250 , 251 , 252 , 252 , 253 , 253 , 254 , 254 , 255 , 255 , 255 , 255 ,
; 255 , 255 , 255 , 255 , 254 , 254 , 253 , 253 , 252 , 252 , 251 , 250 , 249 , 248 , 247 , 246 ,
; 245 , 243 , 242 , 241 , 239 , 238 , 236 , 234 , 233 , 231 , 229 , 227 , 225 , 223 , 221 , 219 ,
; 217 , 214 , 212 , 210 , 207 , 205 , 202 , 200 , 197 , 194 , 192 , 189 , 186 , 183 , 181 , 178 ,
; 175 , 172 , 169 , 166 , 163 , 160 , 157 , 154 , 151 , 148 , 145 , 142 , 138 , 135 , 132 , 129 ,
; 126 , 123 , 120 , 117 , 113 , 110 , 107 , 104 , 101 ,  98 ,  95 ,  92 ,  89 ,  86 ,  83 ,  80 ,
; 77  ,  74 ,  72 ,  69 ,  66 ,  63 ,  61 ,  58 ,  55 ,  53 ,  50 ,  48 ,  45 ,  43 ,  41 ,  38 ,
;  36 ,  34 ,  32 ,  30 ,  28 ,  26 ,  24 ,  22 ,  21 ,  19 ,  17 ,  16 ,  14 ,  13 ,  12 ,  10 ,
;   9 ,   8 ,   7 ,   6 ,   5 ,   4 ,   3 ,   3 ,   2 ,   2 ,   1 ,   1 ,   0 ,   0 ,   0 ,   0 ,
;   0 ,   0 ,   0 ,   0 ,   1 ,   1 ,   2 ,   2 ,   3 ,   3 ,   4 ,   5 ,   6 ,   7 ,   8 ,   9 ,
;  10 ,  12 ,  13 ,  14 ,  16 ,  17 ,  19 ,  21 ,  22 ,  24 ,  26 ,  28 ,  30 ,  32 ,  34 ,  36 ,
;  38 ,  41 ,  43 ,  45 ,  48 ,  50 ,  53 ,  55 ,  58 ,  61 ,  63 ,  66 ,  69 ,  72 ,  74 ,  77 ,
;  80 ,  83 ,  86 ,  89 ,  92 ,  95 ,  98 , 101 , 104 , 107 , 110 , 113 , 117 , 120 , 123 , 126
;};
;
;
;flash unsigned int OCR_value[996]={
;62500 - 1 , 52083 - 1 , 44643 - 1 , 39062 - 1 , 34722 - 1 , 31250 - 1 , 28409 - 1 , 26042 - 1 ,
;24038 - 1 , 22321 - 1 , 20833 - 1 , 19531 - 1 , 18382 - 1 , 17361 - 1 , 16447 - 1 , 15625 - 1 ,
;14881 - 1 , 14205 - 1 , 13587 - 1 , 13021 - 1 , 12500 - 1 , 12019 - 1 , 11574 - 1 , 11161 - 1 ,
;10776 - 1 , 10417 - 1 , 10081 - 1 , 9766 - 1 , 9470 - 1 , 9191 - 1 , 8929 - 1 , 8681 - 1 ,
;8446 - 1 , 8224 - 1 , 8013 - 1 , 7812 - 1 , 7622 - 1 , 7440 - 1 , 7267 - 1 , 7102 - 1 ,
;6944 - 1 , 6793 - 1 , 6649 - 1 , 6510 - 1 , 6378 - 1 , 6250 - 1 , 6127 - 1 , 6010 - 1 ,
;5896 - 1 , 5787 - 1 , 5682 - 1 , 5580 - 1 , 5482 - 1 , 5388 - 1 , 5297 - 1 , 5208 - 1 ,
;5123 - 1 , 5040 - 1 , 4960 - 1 , 4883 - 1 , 4808 - 1 , 4735 - 1 , 4664 - 1 , 4596 - 1 ,
;4529 - 1 , 4464 - 1 , 4401 - 1 , 4340 - 1 , 4281 - 1 , 4223 - 1 , 4167 - 1 , 4112 - 1 ,
;4058 - 1 , 4006 - 1 , 3956 - 1 , 3906 - 1 , 3858 - 1 , 3811 - 1 , 3765 - 1 , 3720 - 1 ,
;3676 - 1 , 3634 - 1 , 3592 - 1 , 3551 - 1 , 3511 - 1 , 3472 - 1 , 3434 - 1 , 3397 - 1 ,
;3360 - 1 , 3324 - 1 , 3289 - 1 , 3255 - 1 , 3222 - 1 , 3189 - 1 , 3157 - 1 , 3125 - 1 ,
;3094 - 1 , 3064 - 1 , 3034 - 1 , 3005 - 1 , 2976 - 1 , 2948 - 1 , 2921 - 1 , 2894 - 1 ,
;2867 - 1 , 2841 - 1 , 2815 - 1 , 2790 - 1 , 2765 - 1 , 2741 - 1 , 2717 - 1 , 2694 - 1 ,
;2671 - 1 , 2648 - 1 , 2626 - 1 , 2604 - 1 , 2583 - 1 , 2561 - 1 , 2541 - 1 , 2520 - 1 ,
;2500 - 1 , 2480 - 1 , 2461 - 1 , 2441 - 1 , 2422 - 1 , 2404 - 1 , 2385 - 1 , 2367 - 1 ,
;2350 - 1 , 2332 - 1 , 2315 - 1 , 2298 - 1 , 2281 - 1 , 2264 - 1 , 2248 - 1 , 2232 - 1 ,
;2216 - 1 , 2201 - 1 , 2185 - 1 , 2170 - 1 , 2155 - 1 , 2140 - 1 , 2126 - 1 , 2111 - 1 ,
;2097 - 1 , 2083 - 1 , 2070 - 1 , 2056 - 1 , 2042 - 1 , 2029 - 1 , 2016 - 1 , 2003 - 1 ,
;1990 - 1 , 1978 - 1 , 1965 - 1 , 1953 - 1 , 1941 - 1 , 1929 - 1 , 1917 - 1 , 1905 - 1 ,
;1894 - 1 , 1883 - 1 , 1871 - 1 , 1860 - 1 , 1849 - 1 , 1838 - 1 , 1827 - 1 , 1817 - 1 ,
;1806 - 1 , 1796 - 1 , 1786 - 1 , 1776 - 1 , 1766 - 1 , 1756 - 1 , 1746 - 1 , 1736 - 1 ,
;1727 - 1 , 1717 - 1 , 1708 - 1 , 1698 - 1 , 1689 - 1 , 1680 - 1 , 1671 - 1 , 1662 - 1 ,
;1653 - 1 , 1645 - 1 , 1636 - 1 , 1628 - 1 , 1619 - 1 , 1611 - 1 , 1603 - 1 , 1594 - 1 ,
;1586 - 1 , 1578 - 1 , 1570 - 1 , 3125 - 1 , 3109 - 1 , 3094 - 1 , 3079 - 1 , 3064 - 1 ,
;3049 - 1 , 3034 - 1 , 3019 - 1 , 3005 - 1 , 2990 - 1 , 2976 - 1 , 2962 - 1 , 2948 - 1 ,
;2934 - 1 , 2921 - 1 , 2907 - 1 , 2894 - 1 , 2880 - 1 , 2867 - 1 , 2854 - 1 , 2841 - 1 ,
;2828 - 1 , 2815 - 1 , 2803 - 1 , 2790 - 1 , 2778 - 1 , 2765 - 1 , 2753 - 1 , 2741 - 1 ,
;2729 - 1 , 2717 - 1 , 2706 - 1 , 2694 - 1 , 2682 - 1 , 2671 - 1 , 2660 - 1 , 2648 - 1 ,
;2637 - 1 , 2626 - 1 , 2615 - 1 , 2604 - 1 , 2593 - 1 , 2583 - 1 , 2572 - 1 , 2561 - 1 ,
;2551 - 1 , 2541 - 1 , 2530 - 1 , 2520 - 1 , 2510 - 1 , 2500 - 1 , 2490 - 1 , 2480 - 1 ,
;2470 - 1 , 2461 - 1 , 2451 - 1 , 2441 - 1 , 2432 - 1 , 2422 - 1 , 2413 - 1 , 2404 - 1 ,
;2395 - 1 , 2385 - 1 , 2376 - 1 , 2367 - 1 , 2358 - 1 , 2350 - 1 , 2341 - 1 , 2332 - 1 ,
;2323 - 1 , 2315 - 1 , 2306 - 1 , 2298 - 1 , 2289 - 1 , 2281 - 1 , 2273 - 1 , 2264 - 1 ,
;2256 - 1 , 2248 - 1 , 2240 - 1 , 2232 - 1 , 2224 - 1 , 2216 - 1 , 2208 - 1 , 2201 - 1 ,
;2193 - 1 , 2185 - 1 , 2178 - 1 , 2170 - 1 , 2163 - 1 , 2155 - 1 , 2148 - 1 , 2140 - 1 ,
;2133 - 1 , 2126 - 1 , 2119 - 1 , 2111 - 1 , 2104 - 1 , 2097 - 1 , 2090 - 1 , 2083 - 1 ,
;2076 - 1 , 2070 - 1 , 2063 - 1 , 2056 - 1 , 2049 - 1 , 2042 - 1 , 2036 - 1 , 2029 - 1 ,
;2023 - 1 , 2016 - 1 , 2010 - 1 , 2003 - 1 , 1997 - 1 , 1990 - 1 , 1984 - 1 , 1978 - 1 ,
;1972 - 1 , 1965 - 1 , 1959 - 1 , 1953 - 1 , 1947 - 1 , 1941 - 1 , 1935 - 1 , 1929 - 1 ,
;1923 - 1 , 1917 - 1 , 1911 - 1 , 1905 - 1 , 1900 - 1 , 1894 - 1 , 1888 - 1 , 1883 - 1 ,
;1877 - 1 , 1871 - 1 , 1866 - 1 , 1860 - 1 , 1855 - 1 , 1849 - 1 , 1844 - 1 , 1838 - 1 ,
;1833 - 1 , 1827 - 1 , 1822 - 1 , 1817 - 1 , 1812 - 1 , 1806 - 1 , 1801 - 1 , 1796 - 1 ,
;1791 - 1 , 1786 - 1 , 1781 - 1 , 1776 - 1 , 1771 - 1 , 1766 - 1 , 1761 - 1 , 1756 - 1 ,
;1751 - 1 , 1746 - 1 , 1741 - 1 , 1736 - 1 , 1731 - 1 , 1727 - 1 , 1722 - 1 , 1717 - 1 ,
;1712 - 1 , 1708 - 1 , 1703 - 1 , 1698 - 1 , 1694 - 1 , 1689 - 1 , 1685 - 1 , 1680 - 1 ,
;1676 - 1 , 1671 - 1 , 1667 - 1 , 1662 - 1 , 1658 - 1 , 1653 - 1 , 1649 - 1 , 1645 - 1 ,
;1640 - 1 , 1636 - 1 , 1632 - 1 , 1628 - 1 , 1623 - 1 , 1619 - 1 , 1615 - 1 , 1611 - 1 ,
;1607 - 1 , 1603 - 1 , 1598 - 1 , 1594 - 1 , 1590 - 1 , 1586 - 1 , 1582 - 1 , 1578 - 1 ,
;1574 - 1 , 1570 - 1 , 1566 - 1 , 3125 - 1 , 3117 - 1 , 3109 - 1 , 3102 - 1 , 3094 - 1 ,
;3086 - 1 , 3079 - 1 , 3071 - 1 , 3064 - 1 , 3056 - 1 , 3049 - 1 , 3041 - 1 , 3034 - 1 ,
;3027 - 1 , 3019 - 1 , 3012 - 1 , 3005 - 1 , 2998 - 1 , 2990 - 1 , 2983 - 1 , 2976 - 1 ,
;2969 - 1 , 2962 - 1 , 2955 - 1 , 2948 - 1 , 2941 - 1 , 2934 - 1 , 2927 - 1 , 2921 - 1 ,
;2914 - 1 , 2907 - 1 , 2900 - 1 , 2894 - 1 , 2887 - 1 , 2880 - 1 , 2874 - 1 , 2867 - 1 ,
;2860 - 1 , 2854 - 1 , 2847 - 1 , 2841 - 1 , 2834 - 1 , 2828 - 1 , 2822 - 1 , 2815 - 1 ,
;2809 - 1 , 2803 - 1 , 2796 - 1 , 2790 - 1 , 2784 - 1 , 2778 - 1 , 2772 - 1 , 2765 - 1 ,
;2759 - 1 , 2753 - 1 , 2747 - 1 , 2741 - 1 , 2735 - 1 , 2729 - 1 , 2723 - 1 , 2717 - 1 ,
;2711 - 1 , 2706 - 1 , 2700 - 1 , 2694 - 1 , 2688 - 1 , 2682 - 1 , 2677 - 1 , 2671 - 1 ,
;2665 - 1 , 2660 - 1 , 2654 - 1 , 2648 - 1 , 2643 - 1 , 2637 - 1 , 2632 - 1 , 2626 - 1 ,
;2621 - 1 , 2615 - 1 , 2610 - 1 , 2604 - 1 , 2599 - 1 , 2593 - 1 , 2588 - 1 , 2583 - 1 ,
;2577 - 1 , 2572 - 1 , 2567 - 1 , 2561 - 1 , 2556 - 1 , 2551 - 1 , 2546 - 1 , 2541 - 1 ,
;2535 - 1 , 2530 - 1 , 2525 - 1 , 2520 - 1 , 2515 - 1 , 2510 - 1 , 2505 - 1 , 2500 - 1 ,
;2495 - 1 , 2490 - 1 , 2485 - 1 , 2480 - 1 , 2475 - 1 , 2470 - 1 , 2465 - 1 , 2461 - 1 ,
;2456 - 1 , 2451 - 1 , 2446 - 1 , 2441 - 1 , 2437 - 1 , 2432 - 1 , 2427 - 1 , 2422 - 1 ,
;2418 - 1 , 2413 - 1 , 2408 - 1 , 2404 - 1 , 2399 - 1 , 2395 - 1 , 2390 - 1 , 2385 - 1 ,
;2381 - 1 , 2376 - 1 , 2372 - 1 , 2367 - 1 , 2363 - 1 , 2358 - 1 , 2354 - 1 , 2350 - 1 ,
;2345 - 1 , 2341 - 1 , 2336 - 1 , 2332 - 1 , 2328 - 1 , 2323 - 1 , 2319 - 1 , 2315 - 1 ,
;2311 - 1 , 2306 - 1 , 2302 - 1 , 2298 - 1 , 2294 - 1 , 2289 - 1 , 2285 - 1 , 2281 - 1 ,
;2277 - 1 , 2273 - 1 , 2269 - 1 , 2264 - 1 , 2260 - 1 , 2256 - 1 , 2252 - 1 , 2248 - 1 ,
;2244 - 1 , 2240 - 1 , 2236 - 1 , 2232 - 1 , 2228 - 1 , 2224 - 1 , 2220 - 1 , 2216 - 1 ,
;2212 - 1 , 2208 - 1 , 2205 - 1 , 2201 - 1 , 2197 - 1 , 2193 - 1 , 2189 - 1 , 2185 - 1 ,
;2182 - 1 , 2178 - 1 , 2174 - 1 , 2170 - 1 , 2166 - 1 , 2163 - 1 , 2159 - 1 , 2155 - 1 ,
;2151 - 1 , 2148 - 1 , 2144 - 1 , 2140 - 1 , 2137 - 1 , 2133 - 1 , 2129 - 1 , 2126 - 1 ,
;2122 - 1 , 2119 - 1 , 2115 - 1 , 2111 - 1 , 2108 - 1 , 2104 - 1 , 2101 - 1 , 2097 - 1 ,
;2094 - 1 , 2090 - 1 , 2087 - 1 , 4167 - 1 , 4160 - 1 , 4153 - 1 , 4146 - 1 , 4139 - 1 ,
;4132 - 1 , 4125 - 1 , 4119 - 1 , 4112 - 1 , 4105 - 1 , 4098 - 1 , 4092 - 1 , 4085 - 1 ,
;4078 - 1 , 4072 - 1 , 4065 - 1 , 4058 - 1 , 4052 - 1 , 4045 - 1 , 4039 - 1 , 4032 - 1 ,
;4026 - 1 , 4019 - 1 , 4013 - 1 , 4006 - 1 , 4000 - 1 , 3994 - 1 , 3987 - 1 , 3981 - 1 ,
;3975 - 1 , 3968 - 1 , 3962 - 1 , 3956 - 1 , 3949 - 1 , 3943 - 1 , 3937 - 1 , 3931 - 1 ,
;3925 - 1 , 3918 - 1 , 3912 - 1 , 3906 - 1 , 3900 - 1 , 3894 - 1 , 3888 - 1 , 3882 - 1 ,
;3876 - 1 , 3870 - 1 , 3864 - 1 , 3858 - 1 , 3852 - 1 , 3846 - 1 , 3840 - 1 , 3834 - 1 ,
;3828 - 1 , 3823 - 1 , 3817 - 1 , 3811 - 1 , 3805 - 1 , 3799 - 1 , 3794 - 1 , 3788 - 1 ,
;3782 - 1 , 3776 - 1 , 3771 - 1 , 3765 - 1 , 3759 - 1 , 3754 - 1 , 3748 - 1 , 3743 - 1 ,
;3737 - 1 , 3731 - 1 , 3726 - 1 , 3720 - 1 , 3715 - 1 , 3709 - 1 , 3704 - 1 , 3698 - 1 ,
;3693 - 1 , 3687 - 1 , 3682 - 1 , 3676 - 1 , 3671 - 1 , 3666 - 1 , 3660 - 1 , 3655 - 1 ,
;3650 - 1 , 3644 - 1 , 3639 - 1 , 3634 - 1 , 3628 - 1 , 3623 - 1 , 3618 - 1 , 3613 - 1 ,
;3608 - 1 , 3602 - 1 , 3597 - 1 , 3592 - 1 , 3587 - 1 , 3582 - 1 , 3577 - 1 , 3571 - 1 ,
;3566 - 1 , 3561 - 1 , 3556 - 1 , 3551 - 1 , 3546 - 1 , 3541 - 1 , 3536 - 1 , 3531 - 1 ,
;3526 - 1 , 3521 - 1 , 3516 - 1 , 3511 - 1 , 3506 - 1 , 3501 - 1 , 3497 - 1 , 3492 - 1 ,
;3487 - 1 , 3482 - 1 , 3477 - 1 , 3472 - 1 , 3467 - 1 , 3463 - 1 , 3458 - 1 , 3453 - 1 ,
;3448 - 1 , 3444 - 1 , 3439 - 1 , 3434 - 1 , 3429 - 1 , 3425 - 1 , 3420 - 1 , 3415 - 1 ,
;3411 - 1 , 3406 - 1 , 3401 - 1 , 3397 - 1 , 3392 - 1 , 3388 - 1 , 3383 - 1 , 3378 - 1 ,
;3374 - 1 , 3369 - 1 , 3365 - 1 , 3360 - 1 , 3356 - 1 , 3351 - 1 , 3347 - 1 , 3342 - 1 ,
;3338 - 1 , 3333 - 1 , 3329 - 1 , 3324 - 1 , 3320 - 1 , 3316 - 1 , 3311 - 1 , 3307 - 1 ,
;3303 - 1 , 3298 - 1 , 3294 - 1 , 3289 - 1 , 3285 - 1 , 3281 - 1 , 3277 - 1 , 3272 - 1 ,
;3268 - 1 , 3264 - 1 , 3259 - 1 , 3255 - 1 , 3251 - 1 , 3247 - 1 , 3243 - 1 , 3238 - 1 ,
;3234 - 1 , 3230 - 1 , 3226 - 1 , 3222 - 1 , 3218 - 1 , 3213 - 1 , 3209 - 1 , 3205 - 1 ,
;3201 - 1 , 3197 - 1 , 3193 - 1 , 3189 - 1 , 3185 - 1 , 3181 - 1 , 3177 - 1 , 3173 - 1 ,
;3169 - 1 , 3165 - 1 , 3161 - 1 , 3157 - 1 , 3153 - 1 , 3149 - 1 , 3145 - 1 , 3141 - 1 ,
;3137 - 1 , 3133 - 1 , 3129 - 1 , 6250 - 1 , 6242 - 1 , 6234 - 1 , 6227 - 1 , 6219 - 1 ,
;6211 - 1 , 6203 - 1 , 6196 - 1 , 6188 - 1 , 6180 - 1 , 6173 - 1 , 6165 - 1 , 6158 - 1 ,
;6150 - 1 , 6143 - 1 , 6135 - 1 , 6127 - 1 , 6120 - 1 , 6112 - 1 , 6105 - 1 , 6098 - 1 ,
;6090 - 1 , 6083 - 1 , 6075 - 1 , 6068 - 1 , 6061 - 1 , 6053 - 1 , 6046 - 1 , 6039 - 1 ,
;6031 - 1 , 6024 - 1 , 6017 - 1 , 6010 - 1 , 6002 - 1 , 5995 - 1 , 5988 - 1 , 5981 - 1 ,
;5974 - 1 , 5967 - 1 , 5959 - 1 , 5952 - 1 , 5945 - 1 , 5938 - 1 , 5931 - 1 , 5924 - 1 ,
;5917 - 1 , 5910 - 1 , 5903 - 1 , 5896 - 1 , 5889 - 1 , 5882 - 1 , 5875 - 1 , 5869 - 1 ,
;5862 - 1 , 5855 - 1 , 5848 - 1 , 5841 - 1 , 5834 - 1 , 5828 - 1 , 5821 - 1 , 5814 - 1 ,
;5807 - 1 , 5800 - 1 , 5794 - 1 , 5787 - 1 , 5780 - 1 , 5774 - 1 , 5767 - 1 , 5760 - 1 ,
;5754 - 1 , 5747 - 1 , 5741 - 1 , 5734 - 1 , 5727 - 1 , 5721 - 1 , 5714 - 1 , 5708 - 1 ,
;5701 - 1 , 5695 - 1 , 5688 - 1 , 5682 - 1 , 5675 - 1 , 5669 - 1 , 5663 - 1 , 5656 - 1 ,
;5650 - 1 , 5643 - 1 , 5637 - 1 , 5631 - 1 , 5624 - 1 , 5618 - 1 , 5612 - 1 , 5605 - 1 ,
;5599 - 1 , 5593 - 1 , 5587 - 1 , 5580 - 1 , 5574 - 1 , 5568 - 1 , 5562 - 1 , 5556 - 1 ,
;5549 - 1 , 5543 - 1 , 5537 - 1 , 5531 - 1 , 5525 - 1 , 5519 - 1 , 5513 - 1 , 5507 - 1 ,
;5501 - 1 , 5495 - 1 , 5488 - 1 , 5482 - 1 , 5476 - 1 , 5470 - 1 , 5464 - 1 , 5459 - 1 ,
;5453 - 1 , 5447 - 1 , 5441 - 1 , 5435 - 1 , 5429 - 1 , 5423 - 1 , 5417 - 1 , 5411 - 1 ,
;5405 - 1 , 5400 - 1 , 5394 - 1 , 5388 - 1 , 5382 - 1 , 5376 - 1 , 5371 - 1 , 5365 - 1 ,
;5359 - 1 , 5353 - 1 , 5348 - 1 , 5342 - 1 , 5336 - 1 , 5330 - 1 , 5325 - 1 , 5319 - 1 ,
;5313 - 1 , 5308 - 1 , 5302 - 1 , 5297 - 1 , 5291 - 1 , 5285 - 1 , 5280 - 1 , 5274 - 1 ,
;5269 - 1 , 5263 - 1 , 5258 - 1 , 5252 - 1 , 5247 - 1 , 5241 - 1 , 5236 - 1 , 5230 - 1 ,
;5225 - 1 , 5219 - 1 , 5214 - 1 , 5208 - 1 , 5203 - 1 , 5198 - 1 , 5192 - 1 , 5187 - 1 ,
;5181 - 1 , 5176 - 1 , 5171 - 1 , 5165 - 1 , 5160 - 1 , 5155 - 1 , 5149 - 1 , 5144 - 1 ,
;5139 - 1 , 5133 - 1 , 5128 - 1 , 5123 - 1 , 5118 - 1 , 5112 - 1 , 5107 - 1 , 5102 - 1 ,
;5097 - 1 , 5092 - 1 , 5086 - 1 , 5081 - 1 , 5076 - 1 , 5071 - 1 , 5066 - 1 , 5061 - 1 ,
;5056 - 1 , 5051 - 1 , 5045 - 1 , 5040 - 1 , 5035 - 1 , 5030 - 1 , 5025 - 1 , 5020 - 1 ,
;5015 - 1 , 5010 - 1 , 5005 - 1 , 5000 - 1};
;
;enum
;{
; STOP_MODE=0,
; RUN_MODE,
; FAULT_MODE
;};
;enum
;{
; RUN_CODE=0,
; STOP_CODE,
; DIR_CODE,
; NO_KEY_CODE
;};
;
;unsigned char a_calc(unsigned char,unsigned char);
;volatile unsigned int speed=0,adc_speed=0;
;volatile unsigned char acceleration=1,deceleration=1,temp_key_code=NO_KEY_CODE,

	.DSEG
;mode=STOP_MODE,a_boost=A_BOOST_MIN;
;
;struct
;{
; volatile unsigned char
; dir_f:1,
; deceleration_f:1,
; dir_change_f:1;
;}flags={0,0,0};
;
;//----------------------------------------------------------------
;
;void main(void)
; 0000 0046 {

	.CSEG
_main:
; 0000 0047  unsigned char key_code,old_key_code=NO_KEY_CODE;
; 0000 0048  ENABLE_DDR|=1<<ENABLE_bp;
;	key_code -> R17
;	old_key_code -> R16
	LDI  R16,3
	SBI  0x11,0
; 0000 0049  DISABLE_MOTOR;
	CBI  0x12,0
; 0000 004A  FAULT_PORT|=1<<FAULT_bp; // Turn on fault input pullup
	SBI  0x12,1
; 0000 004B  RUN_LED_DDR|=1<<RUN_LED_bp;
	SBI  0x11,2
; 0000 004C  RUN_LED_OFF;
	SBI  0x12,2
; 0000 004D  DIR_LED1_DDR|=1<<DIR_LED1_bp;
	SBI  0x11,3
; 0000 004E  DIR_LED2_DDR|=1<<DIR_LED2_bp;
	SBI  0x11,4
; 0000 004F  DIR_LED1_ON;
	CBI  0x12,3
; 0000 0050  DIR_LED2_OFF;
	SBI  0x12,4
; 0000 0051  KEY_PORT|=(1<<RUN_KEY_bp)|(1<<STOP_KEY_bp)|(1<<DIR_KEY_bp); // Turn on pullups
	IN   R30,0x3
	ORI  R30,LOW(0x1C)
	OUT  0x3,R30
; 0000 0052 
; 0000 0053  DDRB=(1<<PORTB7)|(1<<PORTB6)|(1<<PORTB5); // PWM outputs
	LDI  R30,LOW(224)
	OUT  0x17,R30
; 0000 0054  ADMUX=1<<REFS0; // AVcc as reference - ADC0
	LDI  R30,LOW(64)
	OUT  0x7,R30
; 0000 0055  ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1);  // ClkADC = ClkIO/64
	LDI  R30,LOW(206)
	OUT  0x6,R30
; 0000 0056  OCR1A=sine[0];
	LDI  R30,LOW(_sine*2)
	LDI  R31,HIGH(_sine*2)
	LPM  R30,Z
	LDI  R31,0
	OUT  0x2A+1,R31
	OUT  0x2A,R30
; 0000 0057  OCR1B=sine[85];
	__POINTW1FN _sine,85
	LPM  R30,Z
	LDI  R31,0
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 0058  OCR1CL=sine[171];
	__POINTW1FN _sine,171
	LPM  R30,Z
	STS  120,R30
; 0000 0059  TCCR1A=1<<WGM10;
	LDI  R30,LOW(1)
	OUT  0x2F,R30
; 0000 005A  TCCR1B=1<<CS10;
	OUT  0x2E,R30
; 0000 005B  OCR2=124; // 1ms
	LDI  R30,LOW(124)
	OUT  0x23,R30
; 0000 005C  TCCR2=(1<<WGM21)|(1<<CS21)|(1<<CS20);
	LDI  R30,LOW(11)
	OUT  0x25,R30
; 0000 005D  TIMSK=1<<OCIE2;
	LDI  R30,LOW(128)
	OUT  0x37,R30
; 0000 005E  #asm("sei")
	sei
; 0000 005F  delay_ms(1000); // For DC bus capacitor charging by NTC
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
; 0000 0060  while (1)
_0x7:
; 0000 0061  {
; 0000 0062   key_code=temp_key_code;
	LDS  R17,_temp_key_code
; 0000 0063   switch(mode)
	LDS  R30,_mode
	LDI  R31,0
; 0000 0064   {
; 0000 0065 
; 0000 0066 //----------------------------------------------------------------
; 0000 0067 
; 0000 0068   case STOP_MODE:
	SBIW R30,0
	BRNE _0xD
; 0000 0069    if((FAULT_PIN&(1<<FAULT_bp))==0)
	SBIC 0x10,1
	RJMP _0xE
; 0000 006A    {
; 0000 006B     old_key_code=key_code;
	MOV  R16,R17
; 0000 006C     mode=FAULT_MODE; // IR2130 fault condition
	LDI  R30,LOW(2)
	STS  _mode,R30
; 0000 006D     break;
	RJMP _0xC
; 0000 006E    }
; 0000 006F    if((key_code==RUN_CODE)&&(key_code!=old_key_code))
_0xE:
	CPI  R17,0
	BRNE _0x10
	CP   R16,R17
	BRNE _0x11
_0x10:
	RJMP _0xF
_0x11:
; 0000 0070    {
; 0000 0071     old_key_code=key_code;
	MOV  R16,R17
; 0000 0072     mode=RUN_MODE;
	LDI  R30,LOW(1)
	STS  _mode,R30
; 0000 0073     RUN_LED_ON;
	CBI  0x12,2
; 0000 0074     ENABLE_MOTOR;
	SBI  0x12,0
; 0000 0075    }
; 0000 0076    else if((key_code==DIR_CODE)&&(key_code!=old_key_code))
	RJMP _0x12
_0xF:
	CPI  R17,2
	BRNE _0x14
	CP   R16,R17
	BRNE _0x15
_0x14:
	RJMP _0x13
_0x15:
; 0000 0077    {
; 0000 0078     old_key_code=key_code;
	MOV  R16,R17
; 0000 0079     if(flags.dir_f)
	LDS  R30,_flags
	ANDI R30,LOW(0x1)
	BREQ _0x16
; 0000 007A     {
; 0000 007B      DIR_LED1_ON;
	CBI  0x12,3
; 0000 007C      DIR_LED2_OFF;
	SBI  0x12,4
; 0000 007D     }
; 0000 007E     else
	RJMP _0x17
_0x16:
; 0000 007F     {
; 0000 0080      DIR_LED1_OFF;
	SBI  0x12,3
; 0000 0081      DIR_LED2_ON;
	CBI  0x12,4
; 0000 0082     }
_0x17:
; 0000 0083     flags.dir_f^=1;
	LDS  R30,_flags
	ANDI R30,LOW(0x1)
	LDI  R26,LOW(1)
	EOR  R30,R26
	ANDI R30,LOW(0x1)
	MOV  R0,R30
	LDS  R30,_flags
	ANDI R30,0xFE
	OR   R30,R0
	STS  _flags,R30
; 0000 0084    }
; 0000 0085    else old_key_code=key_code;
	RJMP _0x18
_0x13:
	MOV  R16,R17
; 0000 0086    break;
_0x18:
_0x12:
	RJMP _0xC
; 0000 0087 
; 0000 0088 //----------------------------------------------------------------
; 0000 0089 
; 0000 008A   case RUN_MODE:
_0xD:
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BRNE _0x19
; 0000 008B    if((FAULT_PIN&(1<<FAULT_bp))==0)
	SBIC 0x10,1
	RJMP _0x1A
; 0000 008C    {
; 0000 008D     DISABLE_MOTOR;
	CBI  0x12,0
; 0000 008E     flags.deceleration_f=0;
	LDS  R30,_flags
	ANDI R30,0xFD
	STS  _flags,R30
; 0000 008F     flags.dir_change_f=0;
	ANDI R30,0xFB
	STS  _flags,R30
; 0000 0090     old_key_code=key_code;
	MOV  R16,R17
; 0000 0091     mode=FAULT_MODE; // Fault condition
	LDI  R30,LOW(2)
	STS  _mode,R30
; 0000 0092     break;
	RJMP _0xC
; 0000 0093    }
; 0000 0094    if((key_code==STOP_CODE)&&(key_code!=old_key_code))
_0x1A:
	CPI  R17,1
	BRNE _0x1C
	CP   R16,R17
	BRNE _0x1D
_0x1C:
	RJMP _0x1B
_0x1D:
; 0000 0095    {
; 0000 0096     old_key_code=key_code;
	MOV  R16,R17
; 0000 0097     flags.deceleration_f=1;
	LDS  R30,_flags
	ORI  R30,2
	STS  _flags,R30
; 0000 0098    }
; 0000 0099    else if((key_code==DIR_CODE)&&(key_code!=old_key_code))
	RJMP _0x1E
_0x1B:
	CPI  R17,2
	BRNE _0x20
	CP   R16,R17
	BRNE _0x21
_0x20:
	RJMP _0x1F
_0x21:
; 0000 009A    {
; 0000 009B     old_key_code=key_code;
	MOV  R16,R17
; 0000 009C     #asm("cli")
	cli
; 0000 009D     flags.deceleration_f=1;
	LDS  R30,_flags
	ORI  R30,2
	STS  _flags,R30
; 0000 009E     flags.dir_change_f=1;
	ORI  R30,4
	STS  _flags,R30
; 0000 009F     #asm("sei")
	sei
; 0000 00A0    }
; 0000 00A1    else old_key_code=key_code;
	RJMP _0x22
_0x1F:
	MOV  R16,R17
; 0000 00A2    break;
_0x22:
_0x1E:
	RJMP _0xC
; 0000 00A3 
; 0000 00A4 //----------------------------------------------------------------
; 0000 00A5 
; 0000 00A6   case FAULT_MODE:
_0x19:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BRNE _0xC
; 0000 00A7    if((FAULT_PIN&(1<<FAULT_bp))&&(key_code==STOP_CODE)&&(key_code!=old_key_code))
	SBIS 0x10,1
	RJMP _0x25
	CPI  R17,1
	BRNE _0x25
	CP   R16,R17
	BRNE _0x26
_0x25:
	RJMP _0x24
_0x26:
; 0000 00A8    {
; 0000 00A9      old_key_code=key_code;
	MOV  R16,R17
; 0000 00AA      mode=STOP_MODE;
	LDI  R30,LOW(0)
	STS  _mode,R30
; 0000 00AB      RUN_LED_OFF;
	SBI  0x12,2
; 0000 00AC    }
; 0000 00AD    else old_key_code=key_code;
	RJMP _0x27
_0x24:
	MOV  R16,R17
; 0000 00AE    break;
_0x27:
; 0000 00AF   }
_0xC:
; 0000 00B0  }
	RJMP _0x7
; 0000 00B1 }
_0x28:
	RJMP _0x28
;
;//----------------------------------------------------------------
;
;unsigned char a_calc(unsigned char sine , unsigned char amplitude)
; 0000 00B6 {
_a_calc:
; 0000 00B7  unsigned int sine_temp;
; 0000 00B8  unsigned char sine1;
; 0000 00B9  sine1=((sine<128)?~sine:sine);
	CALL __SAVELOCR4
;	sine -> Y+5
;	amplitude -> Y+4
;	sine_temp -> R16,R17
;	sine1 -> R19
	LDD  R26,Y+5
	CPI  R26,LOW(0x80)
	BRSH _0x29
	LDD  R30,Y+5
	COM  R30
	RJMP _0x2A
_0x29:
	LDD  R30,Y+5
_0x2A:
	MOV  R19,R30
; 0000 00BA  sine_temp=(((unsigned int)sine1)<<1);
	MOV  R30,R19
	LDI  R31,0
	LSL  R30
	ROL  R31
	MOVW R16,R30
; 0000 00BB  sine_temp-=255;
	__SUBWRN 16,17,255
; 0000 00BC  sine_temp*=amplitude;
	LDD  R30,Y+4
	LDI  R31,0
	MOVW R26,R16
	CALL __MULW12U
	MOVW R16,R30
; 0000 00BD  sine_temp>>=8;
	MOV  R16,R17
	CLR  R17
; 0000 00BE  sine_temp+=255;
	__ADDWRN 16,17,255
; 0000 00BF  sine_temp>>=1;
	LSR  R17
	ROR  R16
; 0000 00C0  sine_temp++;
	__ADDWRN 16,17,1
; 0000 00C1  sine1=(unsigned char)sine_temp;
	MOV  R19,R16
; 0000 00C2  if (sine<128) sine1=~sine1;
	LDD  R26,Y+5
	CPI  R26,LOW(0x80)
	BRSH _0x2C
	COM  R19
; 0000 00C3  return sine1;
_0x2C:
	MOV  R30,R19
	CALL __LOADLOCR4
	ADIW R28,6
	RET
; 0000 00C4 }
;
;//----------------------------------------------------------------
;
;interrupt [TIM2_COMP] void timer2_compare_isr(void)
; 0000 00C9 {
_timer2_compare_isr:
	ST   -Y,R0
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
; 0000 00CA  unsigned int final_speed;
; 0000 00CB  static unsigned char x20=0,x10=0,
; 0000 00CC  acceleration_counter=0, deceleration_counter=0;
; 0000 00CD 
; 0000 00CE  final_speed=(flags.deceleration_f ? 0 : adc_speed);
	ST   -Y,R17
	ST   -Y,R16
;	final_speed -> R16,R17
	LDS  R30,_flags
	LDI  R31,0
	LSR  R31
	ROR  R30
	ANDI R30,LOW(0x1)
	ANDI R31,HIGH(0x1)
	SBRS R30,0
	RJMP _0x2E
	ORI  R30,LOW(0xFFFE)
	ORI  R31,HIGH(0xFFFE)
_0x2E:
	SBIW R30,0
	BREQ _0x2D
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	RJMP _0x2F
_0x2D:
	LDS  R30,_adc_speed
	LDS  R31,_adc_speed+1
_0x2F:
	MOVW R16,R30
; 0000 00CF  if((mode!=RUN_MODE)||(speed==final_speed))
	LDS  R26,_mode
	CPI  R26,LOW(0x1)
	BRNE _0x32
	LDS  R26,_speed
	LDS  R27,_speed+1
	CP   R16,R26
	CPC  R17,R27
	BRNE _0x31
_0x32:
; 0000 00D0  {
; 0000 00D1   acceleration_counter=0;
	LDI  R30,LOW(0)
	STS  _acceleration_counter_S0000002000,R30
; 0000 00D2   deceleration_counter=0;
	STS  _deceleration_counter_S0000002000,R30
; 0000 00D3  }
; 0000 00D4  else if(speed<final_speed)
	RJMP _0x34
_0x31:
	LDS  R26,_speed
	LDS  R27,_speed+1
	CP   R26,R16
	CPC  R27,R17
	BRSH _0x35
; 0000 00D5  {
; 0000 00D6   deceleration_counter=0;
	LDI  R30,LOW(0)
	STS  _deceleration_counter_S0000002000,R30
; 0000 00D7   if(++acceleration_counter>=acceleration)
	LDS  R26,_acceleration_counter_S0000002000
	SUBI R26,-LOW(1)
	STS  _acceleration_counter_S0000002000,R26
	LDS  R30,_acceleration
	CP   R26,R30
	BRLO _0x36
; 0000 00D8   {
; 0000 00D9    acceleration_counter=0;
	LDI  R30,LOW(0)
	STS  _acceleration_counter_S0000002000,R30
; 0000 00DA    speed++;
	LDI  R26,LOW(_speed)
	LDI  R27,HIGH(_speed)
	LD   R30,X+
	LD   R31,X+
	ADIW R30,1
	ST   -X,R31
	ST   -X,R30
	SBIW R30,1
; 0000 00DB    }
; 0000 00DC   }
_0x36:
; 0000 00DD  else
	RJMP _0x37
_0x35:
; 0000 00DE  {
; 0000 00DF   acceleration_counter=0;
	LDI  R30,LOW(0)
	STS  _acceleration_counter_S0000002000,R30
; 0000 00E0   if(++deceleration_counter>=deceleration)
	LDS  R26,_deceleration_counter_S0000002000
	SUBI R26,-LOW(1)
	STS  _deceleration_counter_S0000002000,R26
	LDS  R30,_deceleration
	CP   R26,R30
	BRSH PC+3
	JMP _0x38
; 0000 00E1   {
; 0000 00E2    deceleration_counter=0;
	LDI  R30,LOW(0)
	STS  _deceleration_counter_S0000002000,R30
; 0000 00E3    speed--;
	LDI  R26,LOW(_speed)
	LDI  R27,HIGH(_speed)
	LD   R30,X+
	LD   R31,X+
	SBIW R30,1
	ST   -X,R31
	ST   -X,R30
	ADIW R30,1
; 0000 00E4    if((speed==0)&&(flags.deceleration_f==1))
	LDS  R26,_speed
	LDS  R27,_speed+1
	SBIW R26,0
	BRNE _0x3A
	LDS  R30,_flags
	ANDI R30,LOW(0x2)
	CPI  R30,LOW(0x2)
	BREQ _0x3B
_0x3A:
	RJMP _0x39
_0x3B:
; 0000 00E5    {
; 0000 00E6     flags.deceleration_f=0;
	LDS  R30,_flags
	ANDI R30,0xFD
	STS  _flags,R30
; 0000 00E7     if(flags.dir_change_f)
	ANDI R30,LOW(0x4)
	BREQ _0x3C
; 0000 00E8     {
; 0000 00E9      flags.dir_change_f=0;
	LDS  R30,_flags
	ANDI R30,0xFB
	STS  _flags,R30
; 0000 00EA      if(flags.dir_f)
	ANDI R30,LOW(0x1)
	BREQ _0x3D
; 0000 00EB      {
; 0000 00EC       DIR_LED1_ON;
	CBI  0x12,3
; 0000 00ED       DIR_LED2_OFF;
	SBI  0x12,4
; 0000 00EE      }
; 0000 00EF      else
	RJMP _0x3E
_0x3D:
; 0000 00F0      {
; 0000 00F1       DIR_LED1_OFF;
	SBI  0x12,3
; 0000 00F2       DIR_LED2_ON;
	CBI  0x12,4
; 0000 00F3      }
_0x3E:
; 0000 00F4      flags.dir_f^=1;
	LDS  R30,_flags
	ANDI R30,LOW(0x1)
	LDI  R26,LOW(1)
	EOR  R30,R26
	ANDI R30,LOW(0x1)
	MOV  R0,R30
	LDS  R30,_flags
	ANDI R30,0xFE
	OR   R30,R0
	STS  _flags,R30
; 0000 00F5     }
; 0000 00F6 
; 0000 00F7     else
	RJMP _0x3F
_0x3C:
; 0000 00F8     {
; 0000 00F9      mode=STOP_MODE;
	LDI  R30,LOW(0)
	STS  _mode,R30
; 0000 00FA      RUN_LED_OFF;
	SBI  0x12,2
; 0000 00FB      DISABLE_MOTOR;
	CBI  0x12,0
; 0000 00FC     }
_0x3F:
; 0000 00FD    }
; 0000 00FE   }
_0x39:
; 0000 00FF  }
_0x38:
_0x37:
_0x34:
; 0000 0100 
; 0000 0101  if(speed&&(mode==RUN_MODE))
	LDS  R30,_speed
	LDS  R31,_speed+1
	SBIW R30,0
	BREQ _0x41
	LDS  R26,_mode
	CPI  R26,LOW(0x1)
	BREQ _0x42
_0x41:
	RJMP _0x40
_0x42:
; 0000 0102  {
; 0000 0103   unsigned int temp16=OCR_value[speed-1];
; 0000 0104   OCR3AH=(unsigned char)(temp16>>8);
	SBIW R28,2
;	temp16 -> Y+0
	LDS  R30,_speed
	LDS  R31,_speed+1
	SBIW R30,1
	LDI  R26,LOW(_OCR_value*2)
	LDI  R27,HIGH(_OCR_value*2)
	LSL  R30
	ROL  R31
	ADD  R30,R26
	ADC  R31,R27
	CALL __GETW1PF
	ST   Y,R30
	STD  Y+1,R31
	LDD  R30,Y+1
	STS  135,R30
; 0000 0105   OCR3AL=(unsigned char)temp16;
	LD   R30,Y
	STS  134,R30
; 0000 0106   TCCR1A|=((1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1));
	IN   R30,0x2F
	ORI  R30,LOW(0xA8)
	OUT  0x2F,R30
; 0000 0107   TCCR3A=(1<<WGM31)|(1<<WGM30);
	LDI  R30,LOW(3)
	STS  139,R30
; 0000 0108   TCCR3B=(1<<WGM33)|(1<<WGM32)|(1<<CS30);
	LDI  R30,LOW(25)
	STS  138,R30
; 0000 0109   ETIMSK|=(1<<OCIE3A);
	LDS  R30,125
	ORI  R30,0x10
	STS  125,R30
; 0000 010A  }
	ADIW R28,2
; 0000 010B  else
	RJMP _0x43
_0x40:
; 0000 010C  {
; 0000 010D   TCCR1A&=~((1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1));
	IN   R30,0x2F
	ANDI R30,LOW(0x57)
	OUT  0x2F,R30
; 0000 010E   TCCR3A=0;
	LDI  R30,LOW(0)
	STS  139,R30
; 0000 010F   TCCR3B=0; // Return to normal mode for next immediate update
	STS  138,R30
; 0000 0110   TCNT3H=0;
	STS  137,R30
; 0000 0111   TCNT3L=0;
	STS  136,R30
; 0000 0112   ETIMSK&=~(1<<OCIE3A);
	LDS  R30,125
	ANDI R30,0xEF
	STS  125,R30
; 0000 0113   ETIFR=(1<<OCF3A);
	LDI  R30,LOW(16)
	STS  124,R30
; 0000 0114  }
_0x43:
; 0000 0115  if(++x20==20) // 20ms
	LDS  R26,_x20_S0000002000
	SUBI R26,-LOW(1)
	STS  _x20_S0000002000,R26
	CPI  R26,LOW(0x14)
	BRNE _0x44
; 0000 0116  {
; 0000 0117   unsigned char temp8;
; 0000 0118   x20=0;
	SBIW R28,1
;	temp8 -> Y+0
	LDI  R30,LOW(0)
	STS  _x20_S0000002000,R30
; 0000 0119   temp8=KEY_PIN&((1<<DIR_KEY_bp)|(1<<STOP_KEY_bp)|(1<<RUN_KEY_bp));
	IN   R30,0x1
	ANDI R30,LOW(0x1C)
	ST   Y,R30
; 0000 011A   switch(temp8)
	LDI  R31,0
; 0000 011B   {
; 0000 011C   case RUN_gc:
	CPI  R30,LOW(0x18)
	LDI  R26,HIGH(0x18)
	CPC  R31,R26
	BRNE _0x48
; 0000 011D    temp_key_code=RUN_CODE;
	LDI  R30,LOW(0)
	RJMP _0x6F
; 0000 011E    break;
; 0000 011F   case STOP_gc:
_0x48:
	CPI  R30,LOW(0x14)
	LDI  R26,HIGH(0x14)
	CPC  R31,R26
	BRNE _0x49
; 0000 0120    temp_key_code=STOP_CODE;
	LDI  R30,LOW(1)
	RJMP _0x6F
; 0000 0121    break;
; 0000 0122   case DIR_gc:
_0x49:
	CPI  R30,LOW(0xC)
	LDI  R26,HIGH(0xC)
	CPC  R31,R26
	BRNE _0x4B
; 0000 0123    temp_key_code=DIR_CODE;
	LDI  R30,LOW(2)
	RJMP _0x6F
; 0000 0124    break;
; 0000 0125   default:
_0x4B:
; 0000 0126    temp_key_code=NO_KEY_CODE;
	LDI  R30,LOW(3)
_0x6F:
	STS  _temp_key_code,R30
; 0000 0127   }
; 0000 0128   if(++x10==10) // 200 ms
	LDS  R26,_x10_S0000002000
	SUBI R26,-LOW(1)
	STS  _x10_S0000002000,R26
	CPI  R26,LOW(0xA)
	BRNE _0x4C
; 0000 0129   {
; 0000 012A    x10=0;
	LDI  R30,LOW(0)
	STS  _x10_S0000002000,R30
; 0000 012B    if(mode==FAULT_MODE)TOGGLE_RUN_LED;
	LDS  R26,_mode
	CPI  R26,LOW(0x2)
	BRNE _0x4D
	IN   R30,0x12
	LDI  R26,LOW(4)
	EOR  R30,R26
	OUT  0x12,R30
; 0000 012C   }
_0x4D:
; 0000 012D  }
_0x4C:
	ADIW R28,1
; 0000 012E }
_0x44:
	RJMP _0x73
;
;//----------------------------------------------------------------
;
;interrupt [TIM3_COMPA] void timer3_comparea_isr(void)
; 0000 0133 {
_timer3_comparea_isr:
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
; 0000 0134  static unsigned char index=0;
; 0000 0135  if (speed>795)index+=16;
	LDS  R26,_speed
	LDS  R27,_speed+1
	CPI  R26,LOW(0x31C)
	LDI  R30,HIGH(0x31C)
	CPC  R27,R30
	BRLO _0x4E
	LDS  R30,_index_S0000003000
	SUBI R30,-LOW(16)
	RJMP _0x70
; 0000 0136  else if(speed>595)index+=8;
_0x4E:
	LDS  R26,_speed
	LDS  R27,_speed+1
	CPI  R26,LOW(0x254)
	LDI  R30,HIGH(0x254)
	CPC  R27,R30
	BRLO _0x50
	LDS  R30,_index_S0000003000
	SUBI R30,-LOW(8)
	RJMP _0x70
; 0000 0137  else if(speed>395)index+=4;
_0x50:
	LDS  R26,_speed
	LDS  R27,_speed+1
	CPI  R26,LOW(0x18C)
	LDI  R30,HIGH(0x18C)
	CPC  R27,R30
	BRLO _0x52
	LDS  R30,_index_S0000003000
	SUBI R30,-LOW(4)
	RJMP _0x70
; 0000 0138  else if(speed>195)index+=2;
_0x52:
	LDS  R26,_speed
	LDS  R27,_speed+1
	CPI  R26,LOW(0xC4)
	LDI  R30,HIGH(0xC4)
	CPC  R27,R30
	BRLO _0x54
	LDS  R30,_index_S0000003000
	SUBI R30,-LOW(2)
	RJMP _0x70
; 0000 0139  else ++index;
_0x54:
	LDS  R30,_index_S0000003000
	SUBI R30,-LOW(1)
_0x70:
	STS  _index_S0000003000,R30
; 0000 013A 
; 0000 013B  if(speed>=SPEED_BASE)
	LDS  R26,_speed
	LDS  R27,_speed+1
	CPI  R26,LOW(0x1EF)
	LDI  R30,HIGH(0x1EF)
	CPC  R27,R30
	BRSH PC+3
	JMP _0x56
; 0000 013C  {
; 0000 013D   if (flags.dir_f)
	LDS  R30,_flags
	ANDI R30,LOW(0x1)
	BREQ _0x57
; 0000 013E   {
; 0000 013F    OCR1A=sine[index];
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	LDI  R31,0
	OUT  0x2A+1,R31
	OUT  0x2A,R30
; 0000 0140    OCR1B=sine[(unsigned char)(index+85U)];
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-85)
	SBCI R31,HIGH(-85)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	LDI  R31,0
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 0141   }
; 0000 0142   else
	RJMP _0x58
_0x57:
; 0000 0143   {
; 0000 0144    OCR1B=sine[index];
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	LDI  R31,0
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 0145    OCR1A=sine[(unsigned char)(index+85U)];
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-85)
	SBCI R31,HIGH(-85)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	LDI  R31,0
	OUT  0x2A+1,R31
	OUT  0x2A,R30
; 0000 0146   }
_0x58:
; 0000 0147   OCR1CL=sine[(unsigned char)(index+171U)];
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-171)
	SBCI R31,HIGH(-171)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	STS  120,R30
; 0000 0148  }
; 0000 0149 
; 0000 014A  else
	RJMP _0x59
_0x56:
; 0000 014B  {
; 0000 014C   unsigned long amplitude;
; 0000 014D   amplitude=(((255-a_boost)*(unsigned long)(speed-SPEED_BOOST))/(SPEED_BASE-SPEED_BOOST))+a_boost;
	SBIW R28,4
;	amplitude -> Y+0
	LDS  R30,_a_boost
	LDI  R31,0
	LDI  R26,LOW(255)
	LDI  R27,HIGH(255)
	SUB  R26,R30
	SBC  R27,R31
	LDS  R30,_speed
	LDS  R31,_speed+1
	SBIW R30,1
	CLR  R22
	CLR  R23
	CALL __CWD2
	CALL __MULD12U
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x1EE
	CALL __DIVD21U
	MOVW R26,R30
	MOVW R24,R22
	LDS  R30,_a_boost
	LDI  R31,0
	CALL __CWD1
	CALL __ADDD12
	CALL __PUTD1S0
; 0000 014E   if (flags.dir_f)
	LDS  R30,_flags
	ANDI R30,LOW(0x1)
	BREQ _0x5A
; 0000 014F   {
; 0000 0150    OCR1A=a_calc(sine[index],(unsigned char)amplitude);
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	ST   -Y,R30
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _a_calc
	LDI  R31,0
	OUT  0x2A+1,R31
	OUT  0x2A,R30
; 0000 0151    OCR1B=a_calc(sine[(unsigned char)(index+85U)],(unsigned char)amplitude);
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-85)
	SBCI R31,HIGH(-85)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	ST   -Y,R30
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _a_calc
	LDI  R31,0
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 0152   }
; 0000 0153   else
	RJMP _0x5B
_0x5A:
; 0000 0154   {
; 0000 0155    OCR1B=a_calc(sine[index],(unsigned char)amplitude);
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	ST   -Y,R30
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _a_calc
	LDI  R31,0
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 0156    OCR1A=a_calc(sine[(unsigned char)(index+85U)],(unsigned char)amplitude);
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-85)
	SBCI R31,HIGH(-85)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	ST   -Y,R30
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _a_calc
	LDI  R31,0
	OUT  0x2A+1,R31
	OUT  0x2A,R30
; 0000 0157   }
_0x5B:
; 0000 0158   OCR1CL=a_calc(sine[(unsigned char)(index+171U)],(unsigned char)amplitude);
	LDS  R30,_index_S0000003000
	LDI  R31,0
	SUBI R30,LOW(-171)
	SBCI R31,HIGH(-171)
	LDI  R31,0
	LDI  R31,0
	SUBI R30,LOW(-_sine*2)
	SBCI R31,HIGH(-_sine*2)
	LPM  R30,Z
	ST   -Y,R30
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _a_calc
	STS  120,R30
; 0000 0159  }
	ADIW R28,4
_0x59:
; 0000 015A }
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
;//----------------------------------------------------------------
;
;interrupt [ADC_INT] void ADC_INT_isr(void)
; 0000 015F {
_ADC_INT_isr:
	ST   -Y,R0
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
; 0000 0160  static unsigned int adc_temp=0;
; 0000 0161  static unsigned char n=0,adc_select=0;
; 0000 0162  unsigned int temp;
; 0000 0163 
; 0000 0164  adc_temp+=ADCW;
	ST   -Y,R17
	ST   -Y,R16
;	temp -> R16,R17
	IN   R30,0x4
	IN   R31,0x4+1
	LDS  R26,_adc_temp_S0000004000
	LDS  R27,_adc_temp_S0000004000+1
	ADD  R30,R26
	ADC  R31,R27
	STS  _adc_temp_S0000004000,R30
	STS  _adc_temp_S0000004000+1,R31
; 0000 0165 
; 0000 0166  if(++n==64)
	LDS  R26,_n_S0000004000
	SUBI R26,-LOW(1)
	STS  _n_S0000004000,R26
	CPI  R26,LOW(0x40)
	BREQ PC+3
	JMP _0x5C
; 0000 0167  {
; 0000 0168   n=0;
	LDI  R30,LOW(0)
	STS  _n_S0000004000,R30
; 0000 0169   temp=(adc_temp>>6);
	LDS  R26,_adc_temp_S0000004000
	LDS  R27,_adc_temp_S0000004000+1
	LDI  R30,LOW(6)
	CALL __LSRW12
	MOVW R16,R30
; 0000 016A   adc_temp=0;
	LDI  R30,LOW(0)
	STS  _adc_temp_S0000004000,R30
	STS  _adc_temp_S0000004000+1,R30
; 0000 016B   switch (adc_select)
	LDS  R30,_adc_select_S0000004000
	LDI  R31,0
; 0000 016C   {
; 0000 016D   case 0:
	SBIW R30,0
	BRNE _0x60
; 0000 016E    if(temp<14)adc_speed=0;
	__CPWRN 16,17,14
	BRSH _0x61
	LDI  R30,LOW(0)
	STS  _adc_speed,R30
	STS  _adc_speed+1,R30
; 0000 016F    else if (temp<1010)adc_speed=temp-13;
	RJMP _0x62
_0x61:
	__CPWRN 16,17,1010
	BRSH _0x63
	MOVW R30,R16
	SBIW R30,13
	RJMP _0x71
; 0000 0170    else adc_speed=996;
_0x63:
	LDI  R30,LOW(996)
	LDI  R31,HIGH(996)
_0x71:
	STS  _adc_speed,R30
	STS  _adc_speed+1,R31
; 0000 0171    ADMUX=(1<<REFS0)|(1<<MUX0);
_0x62:
	LDI  R30,LOW(65)
	RJMP _0x72
; 0000 0172    break;
; 0000 0173 
; 0000 0174   case 1:
_0x60:
	CPI  R30,LOW(0x1)
	LDI  R26,HIGH(0x1)
	CPC  R31,R26
	BRNE _0x65
; 0000 0175    acceleration=(temp?(temp>>2):1);
	MOV  R0,R16
	OR   R0,R17
	BREQ _0x66
	MOVW R30,R16
	CALL __LSRW2
	RJMP _0x67
_0x66:
	LDI  R30,LOW(1)
_0x67:
	STS  _acceleration,R30
; 0000 0176    ADMUX=(1<<REFS0)|(1<<MUX1);
	LDI  R30,LOW(66)
	RJMP _0x72
; 0000 0177    break;
; 0000 0178 
; 0000 0179   case 2:
_0x65:
	CPI  R30,LOW(0x2)
	LDI  R26,HIGH(0x2)
	CPC  R31,R26
	BRNE _0x69
; 0000 017A    deceleration=(temp?(temp>>2):1);
	MOV  R0,R16
	OR   R0,R17
	BREQ _0x6A
	MOVW R30,R16
	CALL __LSRW2
	RJMP _0x6B
_0x6A:
	LDI  R30,LOW(1)
_0x6B:
	STS  _deceleration,R30
; 0000 017B    ADMUX=(1<<REFS0)|(1<<MUX1)|(1<<MUX0);
	LDI  R30,LOW(67)
	RJMP _0x72
; 0000 017C    break;
; 0000 017D 
; 0000 017E   case 3:
_0x69:
	CPI  R30,LOW(0x3)
	LDI  R26,HIGH(0x3)
	CPC  R31,R26
	BRNE _0x5F
; 0000 017F    a_boost=(temp>>5)+A_BOOST_MIN; // 25 <= a_boost <= 56
	MOVW R26,R16
	LDI  R30,LOW(5)
	CALL __LSRW12
	SUBI R30,-LOW(25)
	STS  _a_boost,R30
; 0000 0180    ADMUX=(1<<REFS0);
	LDI  R30,LOW(64)
_0x72:
	OUT  0x7,R30
; 0000 0181   }
_0x5F:
; 0000 0182   if(++adc_select==4)adc_select=0;
	LDS  R26,_adc_select_S0000004000
	SUBI R26,-LOW(1)
	STS  _adc_select_S0000004000,R26
	CPI  R26,LOW(0x4)
	BRNE _0x6E
	LDI  R30,LOW(0)
	STS  _adc_select_S0000004000,R30
; 0000 0183  }
_0x6E:
; 0000 0184  ADCSRA|=(1<<ADSC);
_0x5C:
	SBI  0x6,6
; 0000 0185 }
_0x73:
	LD   R16,Y+
	LD   R17,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R0,Y+
	RETI
;
;//----------------------------------------------------------------
;

	.DSEG
_speed:
	.BYTE 0x2
_adc_speed:
	.BYTE 0x2
_acceleration:
	.BYTE 0x1
_deceleration:
	.BYTE 0x1
_temp_key_code:
	.BYTE 0x1
_mode:
	.BYTE 0x1
_a_boost:
	.BYTE 0x1
_flags:
	.BYTE 0x1
_x20_S0000002000:
	.BYTE 0x1
_x10_S0000002000:
	.BYTE 0x1
_acceleration_counter_S0000002000:
	.BYTE 0x1
_deceleration_counter_S0000002000:
	.BYTE 0x1
_index_S0000003000:
	.BYTE 0x1
_adc_temp_S0000004000:
	.BYTE 0x2
_n_S0000004000:
	.BYTE 0x1
_adc_select_S0000004000:
	.BYTE 0x1

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

__ADDD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	ADC  R23,R25
	RET

__LSRW12:
	TST  R30
	MOV  R0,R30
	MOVW R30,R26
	BREQ __LSRW12R
__LSRW12L:
	LSR  R31
	ROR  R30
	DEC  R0
	BRNE __LSRW12L
__LSRW12R:
	RET

__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
	RET

__CWD1:
	MOV  R22,R31
	ADD  R22,R22
	SBC  R22,R22
	MOV  R23,R22
	RET

__CWD2:
	MOV  R24,R27
	ADD  R24,R24
	SBC  R24,R24
	MOV  R25,R24
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

__GETW1PF:
	LPM  R0,Z+
	LPM  R31,Z
	MOV  R30,R0
	RET

__PUTD1S0:
	ST   Y,R30
	STD  Y+1,R31
	STD  Y+2,R22
	STD  Y+3,R23
	RET

__SAVELOCR4:
	ST   -Y,R19
__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

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
