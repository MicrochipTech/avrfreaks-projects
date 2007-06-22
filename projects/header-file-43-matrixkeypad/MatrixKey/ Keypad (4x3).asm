
;CodeVisionAVR C Compiler V1.24.8e Evaluation
;(C) Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8535
;Program type           : Application
;Clock frequency        : 1.000000 MHz
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

	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0

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

	.INCLUDE "Matrix Keypad (4x3).vec"
	.INCLUDE "Matrix Keypad (4x3).inc"

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
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.8e Evaluation
;       4 Automatic Program Generator
;       5 © Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 
;       8 Project : Matrix Keypad(4x3)
;       9 Version : 1.0
;      10 Date    : 25/25/2007
;      11 Author  : SRINIVASAN.M
;      12 Company : Scorpion Controls
;      13 Comments: 
;      14 This programe is made to support matrix kepad with the specification of 4 row and 3 columns.
;      15 If you have any doubts in this programe, then visit ATMEL community in www.Orkut.com.
;      16 
;      17 
;      18 Chip type           : ATmega8535
;      19 Program type        : Application
;      20 Clock frequency     : 1.000000 MHz
;      21 Memory model        : Small
;      22 External SRAM size  : 0
;      23 Data Stack size     : 128
;      24 *****************************************************/
;      25 
;      26 #include <mega8535.h>
;      27 
;      28 // Standard Input/Output functions
;      29 #include <stdio.h>
;      30 #include <lcds_old.h>

	.CSEG
_lcd_init:
;	lcdport -> Y+0
	LD   R30,Y
	CPI  R30,LOW(0x61)
	BREQ _0x5
	CPI  R30,LOW(0x62)
	BREQ _0x5
	CPI  R30,LOW(0x63)
	BRNE _0x8
	LDI  R30,LOW(255)
	OUT  0x14,R30
	LDI  R30,LOW(0)
	OUT  0x15,R30
	RCALL _statement
	RJMP _0x5
_0x8:
	CPI  R30,LOW(0x64)
	BREQ _0x5
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	RJMP _0x9E
_0x5:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	RJMP _0x9E
_statement:
	RCALL __SAVELOCR2
;	k -> R16,R17
	RCALL _lcd_delay1s
	RCALL SUBOPT_0x0
_0xC:
	__CPWRN 16,17,3
	BRGE _0xD
	LDI  R30,LOW(40)
	ST   -Y,R30
	RCALL _bus2lcd
	RCALL _lcd_delay5ms
	RCALL SUBOPT_0x1
	RJMP _0xC
_0xD:
	RCALL _lcd_delay50us
	LDI  R30,LOW(40)
	RCALL SUBOPT_0x2
	LDI  R30,LOW(20)
	RCALL SUBOPT_0x2
	LDI  R30,LOW(8)
	RCALL SUBOPT_0x2
	LDI  R30,LOW(15)
	RCALL SUBOPT_0x2
	LDI  R30,LOW(6)
	RCALL SUBOPT_0x2
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_command
	RJMP _0x9D
_lcd_clr:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _lcd_command
	RET
_lcd_command:
;	command -> Y+0
	RCALL SUBOPT_0x3
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x5
	RCALL SUBOPT_0x4
	RCALL _lcd_delay5ms
	RJMP _0x9E
_lcd_char:
;	character -> Y+0
	RCALL SUBOPT_0x3
	LDI  R30,LOW(8)
	OR   R4,R30
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x5
	LDI  R30,LOW(8)
	OR   R4,R30
	RCALL SUBOPT_0x4
	RCALL _lcd_delay50us
	RJMP _0x9E
;	hexvalue -> Y+0
_bus2lcd:
;	x -> Y+0
	LD   R30,Y
	OUT  0x15,R30
	RCALL _lcd_delay50us
	SBI  0x15,2
	RCALL _lcd_delay50us
	CBI  0x15,2
	RCALL _lcd_delay50us
_0x9E:
	ADIW R28,1
	RET
;	line -> Y+0
_lcd_delay50us:
	RCALL __SAVELOCR2
;	i -> R16,R17
	RCALL SUBOPT_0x0
_0x14:
	__CPWRN 16,17,1
	BRGE _0x15
	RCALL SUBOPT_0x1
	RJMP _0x14
_0x15:
	RJMP _0x9D
_lcd_delay5ms:
	RCALL __SAVELOCR2
;	j -> R16,R17
	RCALL SUBOPT_0x0
_0x17:
	__CPWRN 16,17,100
	BRGE _0x18
	RCALL _lcd_delay50us
	RCALL SUBOPT_0x1
	RJMP _0x17
_0x18:
	RJMP _0x9D
_lcd_delay1s:
	RCALL __SAVELOCR2
;	k -> R16,R17
	RCALL SUBOPT_0x0
_0x1A:
	__CPWRN 16,17,2000
	BRGE _0x1B
	RCALL _lcd_delay50us
	RCALL SUBOPT_0x1
	RJMP _0x1A
_0x1B:
_0x9D:
	RCALL __LOADLOCR2P
	RET
;	*str -> Y+0
;      31 #include <MatrixKeypad(4x3).h>
_matrix_key_init:
	RCALL SUBOPT_0x6
	RET
_matrix_key_chk:
	RCALL _chk_column1
	MOV  R5,R30
	RCALL _key2lcd
	RCALL _chk_column2
	MOV  R5,R30
	RCALL _key2lcd
	RCALL _chk_column3
	MOV  R5,R30
	RCALL _key2lcd
	RET
_chk_column1:
	LDI  R30,LOW(191)
	RCALL SUBOPT_0x7
	BREQ _0x1F
	MOV  R30,R5
	CPI  R30,LOW(0x1C)
	BRNE _0x23
	LDI  R30,LOW(48)
	RET
	RJMP _0x22
_0x23:
	CPI  R30,LOW(0x1A)
	BRNE _0x24
	LDI  R30,LOW(55)
	RET
	RJMP _0x22
_0x24:
	CPI  R30,LOW(0x16)
	BRNE _0x25
	LDI  R30,LOW(52)
	RET
	RJMP _0x22
_0x25:
	CPI  R30,LOW(0xE)
	BRNE _0x27
	LDI  R30,LOW(49)
	RET
	RJMP _0x22
_0x27:
	LDI  R30,LOW(120)
	RET
_0x22:
_0x1F:
	LDI  R30,LOW(120)
	RET
_chk_column2:
	LDI  R30,LOW(223)
	RCALL SUBOPT_0x7
	BREQ _0x28
	MOV  R30,R5
	CPI  R30,LOW(0x1C)
	BRNE _0x2C
	RCALL _lcd_clr
	LDI  R30,LOW(120)
	RET
	RJMP _0x2B
_0x2C:
	CPI  R30,LOW(0x1A)
	BRNE _0x2D
	LDI  R30,LOW(56)
	RET
	RJMP _0x2B
_0x2D:
	CPI  R30,LOW(0x16)
	BRNE _0x2E
	LDI  R30,LOW(53)
	RET
	RJMP _0x2B
_0x2E:
	CPI  R30,LOW(0xE)
	BRNE _0x30
	LDI  R30,LOW(50)
	RET
	RJMP _0x2B
_0x30:
	LDI  R30,LOW(120)
	RET
_0x2B:
_0x28:
	LDI  R30,LOW(120)
	RET
_chk_column3:
	LDI  R30,LOW(127)
	RCALL SUBOPT_0x7
	BREQ _0x31
	MOV  R30,R5
	CPI  R30,LOW(0x1C)
	BRNE _0x35
	LDI  R30,LOW(115)
	RET
	RJMP _0x34
_0x35:
	CPI  R30,LOW(0x1A)
	BRNE _0x36
	LDI  R30,LOW(57)
	RET
	RJMP _0x34
_0x36:
	CPI  R30,LOW(0x16)
	BRNE _0x37
	LDI  R30,LOW(54)
	RET
	RJMP _0x34
_0x37:
	CPI  R30,LOW(0xE)
	BRNE _0x39
	LDI  R30,LOW(51)
	RET
	RJMP _0x34
_0x39:
	LDI  R30,LOW(120)
	RET
_0x34:
_0x31:
	LDI  R30,LOW(120)
	RET
_key2lcd:
	LDI  R30,LOW(120)
	CP   R30,R5
	BREQ _0x3A
	ST   -Y,R5
	RCALL _lcd_char
	__POINTW1FN _0,0
	RCALL SUBOPT_0x8
	MOV  R30,R5
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	LDI  R24,4
	RCALL _printf
	ADIW R28,6
	RCALL _lcd_delay1s
	RCALL _lcd_delay1s
_0x3A:
	RET
;      32 
;      33 // Declare your global variables here    
;      34 
;      35 void main(void)
;      36 {
_main:
;      37 // Declare your local variables here
;      38 
;      39 // Input/Output Ports initialization
;      40 // Port A initialization
;      41 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      42 // State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
;      43 PORTA=0xE0;
	RCALL SUBOPT_0x6
;      44 DDRA=0xE0;
;      45 
;      46 // Port B initialization
;      47 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      48 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      49 PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;      50 DDRB=0x00;
	OUT  0x17,R30
;      51 
;      52 // Port C initialization
;      53 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      54 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      55 PORTC=0x00;
	OUT  0x15,R30
;      56 DDRC=0x00;
	OUT  0x14,R30
;      57 
;      58 // Port D initialization
;      59 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      60 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      61 PORTD=0x00;
	OUT  0x12,R30
;      62 DDRD=0x00;
	OUT  0x11,R30
;      63 
;      64 // Timer/Counter 0 initialization
;      65 // Clock source: System Clock
;      66 // Clock value: Timer 0 Stopped
;      67 // Mode: Normal top=FFh
;      68 // OC0 output: Disconnected
;      69 TCCR0=0x00;
	OUT  0x33,R30
;      70 TCNT0=0x00;
	OUT  0x32,R30
;      71 OCR0=0x00;
	OUT  0x3C,R30
;      72 
;      73 // Timer/Counter 1 initialization
;      74 // Clock source: System Clock
;      75 // Clock value: Timer 1 Stopped
;      76 // Mode: Normal top=FFFFh
;      77 // OC1A output: Discon.
;      78 // OC1B output: Discon.
;      79 // Noise Canceler: Off
;      80 // Input Capture on Falling Edge
;      81 // Timer 1 Overflow Interrupt: Off
;      82 // Input Capture Interrupt: Off
;      83 // Compare A Match Interrupt: Off
;      84 // Compare B Match Interrupt: Off
;      85 TCCR1A=0x00;
	OUT  0x2F,R30
;      86 TCCR1B=0x00;
	OUT  0x2E,R30
;      87 TCNT1H=0x00;
	OUT  0x2D,R30
;      88 TCNT1L=0x00;
	OUT  0x2C,R30
;      89 ICR1H=0x00;
	OUT  0x27,R30
;      90 ICR1L=0x00;
	OUT  0x26,R30
;      91 OCR1AH=0x00;
	OUT  0x2B,R30
;      92 OCR1AL=0x00;
	OUT  0x2A,R30
;      93 OCR1BH=0x00;
	OUT  0x29,R30
;      94 OCR1BL=0x00;
	OUT  0x28,R30
;      95 
;      96 // Timer/Counter 2 initialization
;      97 // Clock source: System Clock
;      98 // Clock value: Timer 2 Stopped
;      99 // Mode: Normal top=FFh
;     100 // OC2 output: Disconnected
;     101 ASSR=0x00;
	OUT  0x22,R30
;     102 TCCR2=0x00;
	OUT  0x25,R30
;     103 TCNT2=0x00;
	OUT  0x24,R30
;     104 OCR2=0x00;
	OUT  0x23,R30
;     105 
;     106 // External Interrupt(s) initialization
;     107 // INT0: Off
;     108 // INT1: Off
;     109 // INT2: Off
;     110 MCUCR=0x00;
	OUT  0x35,R30
;     111 MCUCSR=0x00;
	OUT  0x34,R30
;     112 
;     113 // Timer(s)/Counter(s) Interrupt(s) initialization
;     114 TIMSK=0x00;
	OUT  0x39,R30
;     115 
;     116 // USART initialization
;     117 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     118 // USART Receiver: On
;     119 // USART Transmitter: On
;     120 // USART Mode: Asynchronous
;     121 // USART Baud rate: 9600 (Double Speed Mode)
;     122 UCSRA=0x02;
	LDI  R30,LOW(2)
	OUT  0xB,R30
;     123 UCSRB=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
;     124 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     125 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     126 UBRRL=0x0C;
	LDI  R30,LOW(12)
	OUT  0x9,R30
;     127 
;     128 // Analog Comparator initialization
;     129 // Analog Comparator: Off
;     130 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     131 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     132 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     133             
;     134 lcd_init('c');
	LDI  R30,LOW(99)
	ST   -Y,R30
	RCALL _lcd_init
;     135 lcd_clr();
	RCALL _lcd_clr
;     136 lcd_char('S');
	LDI  R30,LOW(83)
	ST   -Y,R30
	RCALL _lcd_char
;     137 matrix_key_init(); 
	RCALL _matrix_key_init
;     138 getchar();
	RCALL _getchar
;     139 while (1)
_0x3B:
;     140       {
;     141 		matrix_key_chk();
	RCALL _matrix_key_chk
;     142       }
	RJMP _0x3B
;     143 }
_0x3E:
	RJMP _0x3E

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
	LD   R26,Y
	LDD  R27,Y+1
	RCALL __GETW1P
	SBIW R30,0
	BREQ _0x3F
	LD   R30,X+
	LD   R31,X+
	ADIW R30,1
	ST   -X,R31
	ST   -X,R30
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x40
_0x3F:
	LDD  R30,Y+2
	ST   -Y,R30
	RCALL _putchar
_0x40:
	ADIW R28,3
	RET
__print_G2:
	SBIW R28,6
	RCALL __SAVELOCR6
	LDI  R16,0
_0x41:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x43
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x47
	CPI  R19,37
	BRNE _0x48
	LDI  R16,LOW(1)
	RJMP _0x49
_0x48:
	RCALL SUBOPT_0x9
_0x49:
	RJMP _0x46
_0x47:
	CPI  R30,LOW(0x1)
	BRNE _0x4A
	CPI  R19,37
	BRNE _0x4B
	RCALL SUBOPT_0x9
	RJMP _0x9F
_0x4B:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x4C
	LDI  R17,LOW(1)
	RJMP _0x46
_0x4C:
	CPI  R19,43
	BRNE _0x4D
	LDI  R21,LOW(43)
	RJMP _0x46
_0x4D:
	CPI  R19,32
	BRNE _0x4E
	LDI  R21,LOW(32)
	RJMP _0x46
_0x4E:
	RJMP _0x4F
_0x4A:
	CPI  R30,LOW(0x2)
	BRNE _0x50
_0x4F:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x51
	ORI  R17,LOW(128)
	RJMP _0x46
_0x51:
	RJMP _0x52
_0x50:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x46
_0x52:
	CPI  R19,48
	BRLO _0x55
	CPI  R19,58
	BRLO _0x56
_0x55:
	RJMP _0x54
_0x56:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x46
_0x54:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x5A
	RCALL SUBOPT_0xA
	LD   R30,X
	RCALL SUBOPT_0xB
	RJMP _0x5B
_0x5A:
	CPI  R30,LOW(0x73)
	BRNE _0x5D
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xC
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x5E
_0x5D:
	CPI  R30,LOW(0x70)
	BRNE _0x60
	RCALL SUBOPT_0xA
	RCALL SUBOPT_0xC
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x5E:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x61
_0x60:
	CPI  R30,LOW(0x64)
	BREQ _0x64
	CPI  R30,LOW(0x69)
	BRNE _0x65
_0x64:
	ORI  R17,LOW(4)
	RJMP _0x66
_0x65:
	CPI  R30,LOW(0x75)
	BRNE _0x67
_0x66:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	RCALL SUBOPT_0xD
	LDI  R16,LOW(5)
	RJMP _0x68
_0x67:
	CPI  R30,LOW(0x58)
	BRNE _0x6A
	ORI  R17,LOW(8)
	RJMP _0x6B
_0x6A:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x9C
_0x6B:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	RCALL SUBOPT_0xD
	LDI  R16,LOW(4)
_0x68:
	SBRS R17,2
	RJMP _0x6D
	RCALL SUBOPT_0xA
	RCALL __GETW1P
	RCALL SUBOPT_0xE
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x6E
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	RCALL SUBOPT_0xE
	LDI  R21,LOW(45)
_0x6E:
	CPI  R21,0
	BREQ _0x6F
	SUBI R16,-LOW(1)
	RJMP _0x70
_0x6F:
	ANDI R17,LOW(251)
_0x70:
	RJMP _0x71
_0x6D:
	RCALL SUBOPT_0xA
	RCALL __GETW1P
	RCALL SUBOPT_0xE
_0x71:
_0x61:
	SBRC R17,0
	RJMP _0x72
_0x73:
	CP   R16,R20
	BRSH _0x75
	SBRS R17,7
	RJMP _0x76
	SBRS R17,2
	RJMP _0x77
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x78
_0x77:
	LDI  R19,LOW(48)
_0x78:
	RJMP _0x79
_0x76:
	LDI  R19,LOW(32)
_0x79:
	RCALL SUBOPT_0x9
	SUBI R20,LOW(1)
	RJMP _0x73
_0x75:
_0x72:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x7A
_0x7B:
	CPI  R18,0
	BREQ _0x7D
	SBRS R17,3
	RJMP _0x7E
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	RCALL SUBOPT_0xD
	SBIW R30,1
	LPM  R30,Z
	RJMP _0xA0
_0x7E:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LD   R30,X+
	STD  Y+6,R26
	STD  Y+6+1,R27
_0xA0:
	ST   -Y,R30
	RCALL SUBOPT_0xF
	CPI  R20,0
	BREQ _0x80
	SUBI R20,LOW(1)
_0x80:
	SUBI R18,LOW(1)
	RJMP _0x7B
_0x7D:
	RJMP _0x81
_0x7A:
_0x83:
	LDI  R19,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	RCALL SUBOPT_0xD
	SBIW R30,2
	RCALL __GETW1PF
	STD  Y+8,R30
	STD  Y+8+1,R31
_0x85:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	CP   R26,R30
	CPC  R27,R31
	BRLO _0x87
	SUBI R19,-LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	SUB  R30,R26
	SBC  R31,R27
	RCALL SUBOPT_0xE
	RJMP _0x85
_0x87:
	CPI  R19,58
	BRLO _0x88
	SBRS R17,3
	RJMP _0x89
	SUBI R19,-LOW(7)
	RJMP _0x8A
_0x89:
	SUBI R19,-LOW(39)
_0x8A:
_0x88:
	SBRC R17,4
	RJMP _0x8C
	CPI  R19,49
	BRSH _0x8E
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,1
	BRNE _0x8D
_0x8E:
	RJMP _0xA1
_0x8D:
	CP   R20,R18
	BRLO _0x92
	SBRS R17,0
	RJMP _0x93
_0x92:
	RJMP _0x91
_0x93:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x94
	LDI  R19,LOW(48)
_0xA1:
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0x95
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL SUBOPT_0xF
	CPI  R20,0
	BREQ _0x96
	SUBI R20,LOW(1)
_0x96:
_0x95:
_0x94:
_0x8C:
	RCALL SUBOPT_0x9
	CPI  R20,0
	BREQ _0x97
	SUBI R20,LOW(1)
_0x97:
_0x91:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	SBIW R26,2
	BRLO _0x84
	RJMP _0x83
_0x84:
_0x81:
	SBRS R17,0
	RJMP _0x98
_0x99:
	CPI  R20,0
	BREQ _0x9B
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0xB
	RJMP _0x99
_0x9B:
_0x98:
_0x9C:
_0x5B:
_0x9F:
	LDI  R16,LOW(0)
_0x46:
	RJMP _0x41
_0x43:
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
	RCALL SUBOPT_0x8
	ST   -Y,R17
	ST   -Y,R16
	MOVW R30,R28
	ADIW R30,6
	RCALL SUBOPT_0x8
	RCALL __print_G2
	RCALL __LOADLOCR2
	ADIW R28,4
	POP  R15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x0:
	__GETWRN 16,17,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x1:
	__ADDWRN 16,17,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0x2:
	ST   -Y,R30
	RCALL _lcd_command
	RJMP _lcd_delay50us

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x3:
	LDI  R30,LOW(240)
	MOV  R4,R30
	LD   R30,Y
	AND  R4,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x4:
	ST   -Y,R4
	RJMP _bus2lcd

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x5:
	LDI  R30,LOW(15)
	MOV  R4,R30
	LD   R30,Y
	AND  R4,R30
	SWAP R4
	LDI  R30,0xF0
	AND  R4,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x6:
	LDI  R30,LOW(224)
	OUT  0x1B,R30
	OUT  0x1A,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:8 WORDS
SUBOPT_0x7:
	OUT  0x1B,R30
	IN   R30,0x19
	ANDI R30,LOW(0x1E)
	MOV  R5,R30
	LDI  R30,LOW(30)
	CP   R30,R5
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 13 TIMES, CODE SIZE REDUCTION:10 WORDS
SUBOPT_0x8:
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:10 WORDS
SUBOPT_0x9:
	ST   -Y,R19
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	RCALL SUBOPT_0x8
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:18 WORDS
SUBOPT_0xA:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0xB:
	ST   -Y,R30
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	RCALL SUBOPT_0x8
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xC:
	RCALL __GETW1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	RJMP SUBOPT_0x8

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xD:
	STD  Y+6,R30
	STD  Y+6+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xE:
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xF:
	LDD  R30,Y+13
	LDD  R31,Y+13+1
	RCALL SUBOPT_0x8
	RJMP __put_G2

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
