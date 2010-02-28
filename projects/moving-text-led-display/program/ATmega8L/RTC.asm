
;CodeVisionAVR C Compiler V1.24.8d Professional
;(C) Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8L
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
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8L
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

	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70

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

	.INCLUDE "RTC.vec"
	.INCLUDE "RTC.inc"

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
;       1 /*****************************************************
;       2 Project                 : Time Machine
;       3 Version                 : 0.0
;       4 Date                    : 15/04/2007
;       5 Author                  : Ehab Anwar                            
;       6 Company                 : BDR Electronics
;       7                         : flik_80@yahoo.com
;       8                         : +20107697744
;       9 ///////////////////////////////////////////////////                           
;      10 Comments                : There is no god but Allah
;      11 ///////////////////////////////////////////////////
;      12 Chip type               : ATmega8L
;      13 Clock frequency         : 8.000000 MHz
;      14 *****************************************************/
;      15 
;      16 #include <mega8.h>
;      17 #include <delay.h>
;      18 #include <bcd.h>
;      19 
;      20 #define SCLK  PORTB.6
;      21 #define CE    PORTD.5 
;      22 
;      23 #define  prog_sw  PIND.6
;      24 #define  incr_sw  PIND.7
;      25 
;      26 
;      27 unsigned char sec_=0x25,min_=0x55,hr_=0x10;
;      28 
;      29 
;      30 char code[10]={
_code:
;      31                0xfc,//0
;      32                0xc0,//1
;      33                0xb6,//2
;      34                0xe6,//3
;      35                0xca,//4
;      36                0x6e,//5
;      37                0x7e,//6
;      38                0xcc,//7
;      39                0xfe,//8
;      40                0xee //9
;      41               };
	.BYTE 0xA
;      42 
;      43 bit hold_prog_sw,hold_incr_sw;
;      44 
;      45 /////////////////////////
;      46 void ds1302_reset()    //
;      47  {                     //

	.CSEG
_ds1302_reset:
;      48  SCLK = 0;             //
	CBI  0x18,6
;      49  CE = 0;               // 
	CBI  0x12,5
;      50  delay_us(1);          //
	RCALL SUBOPT_0x0
;      51  CE = 1;               // 
	SBI  0x12,5
;      52  delay_us(1);          //
	RCALL SUBOPT_0x0
;      53  }                     //
	RET
;      54 /////////////////////////
;      55 
;      56 //////////////////////////////////////////////
;      57 void ds1302_wbyte(char data)	            //
;      58 {                                           //
_ds1302_wbyte:
;      59 char a;                                     //
;      60 DDRB|=0x80;//IO=output                      //
	ST   -Y,R16
;	data -> Y+1
;	a -> R16
	SBI  0x17,7
;      61 for(a=0;a<8;++a)                            //
	LDI  R16,LOW(0)
_0x8:
	CPI  R16,8
	BRSH _0x9
;      62  {                                          //
;      63  PORTB.7=(0x01&(data>>a));                  //
	MOV  R30,R16
	LDD  R26,Y+1
	RCALL __LSRB12
	BST  R30,0
	IN   R26,0x18
	BLD  R26,7
	OUT  0x18,R26
;      64  SCLK = 0;                                  // 
	CBI  0x18,6
;      65  delay_us(1);                               //
	RCALL SUBOPT_0x0
;      66  SCLK = 1;                                  // 
	SBI  0x18,6
;      67  delay_us(1);                               //
	RCALL SUBOPT_0x0
;      68  }                                          //
	SUBI R16,-LOW(1)
	RJMP _0x8
_0x9:
;      69 }                                           //
	LDD  R16,Y+0
	RJMP _0x38
;      70 //////////////////////////////////////////////
;      71 
;      72 //////////////////////////////////////////////
;      73 /* --- read one byte from the device --- */ //
;      74 char ds1302_rbyte(void)	                    //
;      75 {                                           //
_ds1302_rbyte:
;      76 char a;                                     //
;      77 char data=0;                                //
;      78 DDRB&=0x7f;//IO=input                       //
	RCALL __SAVELOCR2
;	a -> R16
;	data -> R17
	LDI  R17,0
	CBI  0x17,7
;      79 for(a=0;a<8;a++)                            //
	LDI  R16,LOW(0)
_0xB:
	CPI  R16,8
	BRSH _0xC
;      80  {                                          //
;      81  SCLK = 1;                                  // 
	SBI  0x18,6
;      82  delay_us(1);                               //
	RCALL SUBOPT_0x0
;      83  SCLK = 0;                                  // 
	CBI  0x18,6
;      84  delay_us(1);                               //
	RCALL SUBOPT_0x0
;      85  if(PINB.7)                                 //
	SBIS 0x16,7
	RJMP _0xD
;      86   data|=((char)1)<<a;                       //
	MOV  R30,R16
	LDI  R26,LOW(1)
	RCALL __LSLB12
	OR   R17,R30
;      87  }                                          //
_0xD:
	SUBI R16,-1
	RJMP _0xB
_0xC:
;      88 return data;                                //
	MOV  R30,R17
	RCALL __LOADLOCR2P
	RET
;      89 }                                           //
;      90 //////////////////////////////////////////////
;      91 
;      92 /////////////////////////////////////////////////////////////
;      93 /* --- write one byte using values entered by user --- */  //
;      94 void ds1302_writebyte(char ClkAdd,char ClkData)	           //
;      95 {                                                          //
_ds1302_writebyte:
;      96 ds1302_reset();                                            //
;	ClkAdd -> Y+1
;	ClkData -> Y+0
	RCALL _ds1302_reset
;      97 ds1302_wbyte(ClkAdd);                                      //
	RCALL SUBOPT_0x1
;      98 ds1302_wbyte(ClkData);                                     //
;      99 ds1302_reset();                                            //
	RCALL _ds1302_reset
;     100 }                                                          //
_0x38:
	ADIW R28,2
	RET
;     101 /////////////////////////////////////////////////////////////
;     102 
;     103 /////////////////////////////////////////////////////////////////
;     104 /* --- initialize time & date from user entries ---  */        //
;     105 void write_clk_regs(char sec,char min,char hr)                 //
;     106 {                                                              //
_write_clk_regs:
;     107 ds1302_reset();                                                //
;	sec -> Y+2
;	min -> Y+1
;	hr -> Y+0
	RCALL _ds1302_reset
;     108 ds1302_writebyte(0x8e,0); /* control register */               //
	LDI  R30,LOW(142)
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	RCALL _ds1302_writebyte
;     109 ds1302_reset();                                                //
	RCALL _ds1302_reset
;     110 ds1302_writebyte(0x90,0xab);/* trickle charger register */     //
	LDI  R30,LOW(144)
	ST   -Y,R30
	LDI  R30,LOW(171)
	ST   -Y,R30
	RCALL _ds1302_writebyte
;     111 ds1302_reset();                                                //
	RCALL _ds1302_reset
;     112 ds1302_wbyte(0xbe); /* clock burst write (eight registers) */  //
	LDI  R30,LOW(190)
	RCALL SUBOPT_0x2
;     113 ds1302_wbyte(sec);                                             //
	LDD  R30,Y+2
	RCALL SUBOPT_0x2
;     114 ds1302_wbyte(min);                                             //
	RCALL SUBOPT_0x1
;     115 ds1302_wbyte(hr);                                              //
;     116 ds1302_wbyte(1);                                               //
	LDI  R30,LOW(1)
	RCALL SUBOPT_0x2
;     117 ds1302_wbyte(1);                                               //
	LDI  R30,LOW(1)
	RCALL SUBOPT_0x2
;     118 ds1302_wbyte(1);                                               //
	LDI  R30,LOW(1)
	RCALL SUBOPT_0x2
;     119 ds1302_wbyte(07);                                              //
	LDI  R30,LOW(7)
	RCALL SUBOPT_0x2
;     120 ds1302_wbyte(0x80);//must write control register in burst mode //
	LDI  R30,LOW(128)
	RCALL SUBOPT_0x2
;     121 ds1302_reset();                                                //
	RCALL _ds1302_reset
;     122 }                                                              //
	ADIW R28,3
	RET
;     123 /////////////////////////////////////////////////////////////////
;     124 
;     125 ///////////////////////////////////////////////////
;     126 void read_clk_regs(char *sec,char *min,char *hr) //
;     127  {                                               //
_read_clk_regs:
;     128  ds1302_reset();                                 //
;	*sec -> Y+4
;	*min -> Y+2
;	*hr -> Y+0
	RCALL _ds1302_reset
;     129  ds1302_wbyte(0xBF);	/* clock burst */        //
	LDI  R30,LOW(191)
	RCALL SUBOPT_0x2
;     130  *sec=ds1302_rbyte();                            //
	RCALL _ds1302_rbyte
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	ST   X,R30
;     131  *min=ds1302_rbyte();                            //
	RCALL _ds1302_rbyte
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	ST   X,R30
;     132  *hr=ds1302_rbyte();                             //
	RCALL _ds1302_rbyte
	LD   R26,Y
	LDD  R27,Y+1
	ST   X,R30
;     133  ds1302_reset();                                 //
	RCALL _ds1302_reset
;     134  }                                               //
	ADIW R28,6
	RET
;     135  //////////////////////////////////////////////////
;     136  
;     137 ///////////////////////////////
;     138  void set_data(char data)    //
;     139   {                          //
_set_data:
;     140   PORTC.0=(data>>7)&1;//a    //
;	data -> Y+0
	LD   R30,Y
	ROL  R30
	LDI  R30,0
	ROL  R30
	RCALL SUBOPT_0x3
	BLD  R26,0
	RCALL SUBOPT_0x4
;     141   PORTC.2=(data>>6)&1;//b    //
	SWAP R30
	ANDI R30,0xF
	LSR  R30
	LSR  R30
	RCALL SUBOPT_0x3
	BLD  R26,2
	RCALL SUBOPT_0x4
;     142   PORTD.0=(data>>5)&1;//c    //
	SWAP R30
	ANDI R30,0xF
	LSR  R30
	BST  R30,0
	IN   R26,0x12
	BLD  R26,0
	OUT  0x12,R26
;     143   PORTD.1=(data>>4)&1;//d    //
	LD   R30,Y
	RCALL SUBOPT_0x5
	BST  R30,0
	IN   R26,0x12
	BLD  R26,1
	OUT  0x12,R26
;     144   PORTC.3=(data>>3)&1;//e    //
	LD   R30,Y
	LSR  R30
	LSR  R30
	LSR  R30
	RCALL SUBOPT_0x3
	BLD  R26,3
	RCALL SUBOPT_0x4
;     145   PORTC.1=(data>>2)&1;//f    //
	LSR  R30
	LSR  R30
	RCALL SUBOPT_0x3
	BLD  R26,1
	RCALL SUBOPT_0x4
;     146   PORTC.5=(data>>1)&1;//g    //
	LSR  R30
	RCALL SUBOPT_0x3
	BLD  R26,5
	RCALL SUBOPT_0x4
;     147   PORTC.4=(data>>0)&1;//dot  //
	RCALL SUBOPT_0x3
	BLD  R26,4
	OUT  0x15,R26
;     148   }                          //
	ADIW R28,1
	RET
;     149  //////////////////////////////
;     150  
;     151  //////////////////////////////////////////ok
;     152 void show_time(int a,int b,int c,char d) //
;     153 {                                        //
_show_time:
;     154 char x;                                  //
;     155 int y;                                   //
;     156 for(x=0;x<d;x++)                         //
	RCALL __SAVELOCR3
;	a -> Y+8
;	b -> Y+6
;	c -> Y+4
;	d -> Y+3
;	x -> R16
;	y -> R17,R18
	LDI  R16,LOW(0)
_0xF:
	LDD  R30,Y+3
	CP   R16,R30
	BRSH _0x10
;     157  {                                       //
;     158  set_data(code[sec_&0x0f]);              //
	MOV  R30,R4
	RCALL SUBOPT_0x6
;     159  PORTB.4=1;                              //
	SBI  0x18,4
;     160  for(y=0;y<a;y++)                        //
	RCALL SUBOPT_0x7
_0x12:
	RCALL SUBOPT_0x8
	BRGE _0x13
;     161   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x12
_0x13:
;     162  PORTB.4=0;                              //
	CBI  0x18,4
;     163                                          //
;     164  set_data(code[(sec_&0xf0)>>4]);         //
	MOV  R30,R4
	RCALL SUBOPT_0xA
;     165  PORTB.3=1;                              //
	SBI  0x18,3
;     166  for(y=0;y<a;y++)                        //
	RCALL SUBOPT_0x7
_0x15:
	RCALL SUBOPT_0x8
	BRGE _0x16
;     167   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x15
_0x16:
;     168  PORTB.3=0;                              //
	CBI  0x18,3
;     169                                          //
;     170  set_data(code[min_&0x0f]);              //
	MOV  R30,R5
	RCALL SUBOPT_0x6
;     171  PORTB.1=1;                              //
	SBI  0x18,1
;     172  for(y=0;y<b;y++)                        //
	RCALL SUBOPT_0x7
_0x18:
	RCALL SUBOPT_0xB
	BRGE _0x19
;     173   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x18
_0x19:
;     174  PORTB.1=0;                              //
	CBI  0x18,1
;     175                                          //
;     176  set_data(code[(min_&0xf0)>>4]);         //
	MOV  R30,R5
	RCALL SUBOPT_0xA
;     177  PORTB.0=1;                              //
	SBI  0x18,0
;     178  for(y=0;y<b;y++)                        //
	RCALL SUBOPT_0x7
_0x1B:
	RCALL SUBOPT_0xB
	BRGE _0x1C
;     179   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x1B
_0x1C:
;     180  PORTB.0=0;                              // 
	CBI  0x18,0
;     181                                          //
;     182  set_data(code[hr_&0x0f]);               //
	MOV  R30,R6
	RCALL SUBOPT_0x6
;     183  PORTB.2=1;                              //
	SBI  0x18,2
;     184  for(y=0;y<c;y++)                        //
	RCALL SUBOPT_0x7
_0x1E:
	RCALL SUBOPT_0xC
	BRGE _0x1F
;     185   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x1E
_0x1F:
;     186  PORTB.2=0;                              //
	CBI  0x18,2
;     187                                          //
;     188  set_data(code[(hr_&0xf0)>>4]);          //
	MOV  R30,R6
	RCALL SUBOPT_0xA
;     189  PORTB.5=1;                              //
	SBI  0x18,5
;     190  for(y=0;y<c;y++)                        //
	RCALL SUBOPT_0x7
_0x21:
	RCALL SUBOPT_0xC
	BRGE _0x22
;     191   {delay_us(1);}                         //
	RCALL SUBOPT_0x0
	RCALL SUBOPT_0x9
	RJMP _0x21
_0x22:
;     192  PORTB.5=0;                              //
	CBI  0x18,5
;     193  }                                       //
	SUBI R16,-1
	RJMP _0xF
_0x10:
;     194                                          //
;     195 }                                        //
	RCALL __LOADLOCR3
	ADIW R28,10
	RET
;     196 /////////////////////////////////////////// 
;     197 
;     198 /////////////////////////////////////// 
;     199 void set_time(void)                  //
;     200 {                                    //
_set_time:
;     201 char time[3],a;                      //
;     202 char time_max[3]={12,59,59};         //
;     203 char time_min[3]={1,0,0};            //
;     204 int time_illumnas[7]={               //
;     205                       900,50         //
;     206                      ,50,800         //
;     207                      ,50,50          //
;     208                      ,900            //
;     209                     };               //
;     210 read_clk_regs(                       //
	SBIW R28,23
	LDI  R24,20
	LDI  R26,LOW(0)
	LDI  R27,HIGH(0)
	LDI  R30,LOW(_0x23*2)
	LDI  R31,HIGH(_0x23*2)
	RCALL __INITLOCB
	ST   -Y,R16
;	time -> Y+21
;	a -> R16
;	time_max -> Y+18
;	time_min -> Y+15
;	time_illumnas -> Y+1
;     211               &time[2]               //
;     212              ,&time[1]               //
;     213              ,&time[0]               //
;     214              );                      //
	MOVW R30,R28
	ADIW R30,23
	RCALL SUBOPT_0xD
	MOVW R30,R28
	ADIW R30,24
	RCALL SUBOPT_0xD
	MOVW R30,R28
	ADIW R30,25
	RCALL SUBOPT_0xD
	RCALL _read_clk_regs
;     215 time[2]&=0x7f;                       //
	MOVW R30,R28
	ADIW R30,23
	RCALL SUBOPT_0xE
;     216 time[1]&=0x7f;                       //
	MOVW R30,R28
	ADIW R30,22
	RCALL SUBOPT_0xE
;     217 time[0]&=0x1f;                       //
	LDD  R30,Y+21
	ANDI R30,LOW(0x1F)
	STD  Y+21,R30
;     218                                      //
;     219 time[2]=bcd2bin(time[2]);            //
	RCALL SUBOPT_0xF
	RCALL _bcd2bin
	STD  Y+23,R30
;     220 time[1]=bcd2bin(time[1]);            //
	LDD  R30,Y+22
	ST   -Y,R30
	RCALL _bcd2bin
	STD  Y+22,R30
;     221 time[0]=bcd2bin(time[0]);            //
	LDD  R30,Y+21
	ST   -Y,R30
	RCALL _bcd2bin
	STD  Y+21,R30
;     222                                      //
;     223 for(a=0;a<3;a++)                     //
	LDI  R16,LOW(0)
_0x25:
	CPI  R16,3
	BRLO PC+2
	RJMP _0x26
;     224  {                                   //
;     225  while(1)                            //
_0x27:
;     226   {                                  //
;     227 ///////////////////////////////////////
;     228   hold_prog_sw=prog_sw;              //
	RCALL SUBOPT_0x10
;     229   hold_incr_sw=incr_sw;              //
	RCALL SUBOPT_0x11
;     230   sec_=0x7f&bin2bcd(time[2]);             //
	RCALL SUBOPT_0xF
	RCALL SUBOPT_0x12
	RCALL SUBOPT_0x13
;     231   min_=0x7f&bin2bcd(time[1]);             //
	RCALL SUBOPT_0x14
;     232   hr_=0x1f&bin2bcd(time[0]);              //
;     233   show_time(
;     234              time_illumnas[(char)(a*2)+2]
;     235             ,time_illumnas[(char)(a*2)+1]
;     236             ,time_illumnas[(char)(a*2)]
;     237             ,25
;     238            );                        //
;     239 ///////////////////////////////////////
;     240   if(prog_sw!=hold_prog_sw)          //
	LDI  R30,0
	SBIC 0x10,6
	LDI  R30,1
	MOV  R26,R30
	LDI  R30,0
	SBRC R2,0
	LDI  R30,1
	CP   R30,R26
	BREQ _0x2A
;     241    {                                 //
;     242    hold_prog_sw=prog_sw;             //
	RCALL SUBOPT_0x10
;     243    if(hold_prog_sw==0)               //
	SBRS R2,0
;     244     break;                           //
	RJMP _0x29
;     245    }                                 //
;     246 /////////////////////////////////////// 
;     247   if(incr_sw!=hold_incr_sw)          //
_0x2A:
	LDI  R30,0
	SBIC 0x10,7
	LDI  R30,1
	MOV  R26,R30
	LDI  R30,0
	SBRC R2,1
	LDI  R30,1
	CP   R30,R26
	BREQ _0x2C
;     248    {                                 //
;     249    hold_incr_sw=incr_sw;             //
	RCALL SUBOPT_0x11
;     250    while(incr_sw==0)                 //
_0x2D:
	SBIC 0x10,7
	RJMP _0x2F
;     251     {                                //
;     252     hold_incr_sw=incr_sw;            //
	RCALL SUBOPT_0x11
;     253     if(time[a]>=time_max[a])         //
	RCALL SUBOPT_0x15
	MOVW R26,R28
	ADIW R26,21
	RCALL SUBOPT_0x16
	LD   R0,X
	MOVW R26,R28
	ADIW R26,18
	RCALL SUBOPT_0x16
	LD   R30,X
	CP   R0,R30
	BRLO _0x30
;     254      time[a]=time_min[a];            //
	RCALL SUBOPT_0x15
	MOVW R22,R30
	MOVW R26,R28
	ADIW R26,21
	ADD  R30,R26
	ADC  R31,R27
	MOVW R0,R30
	MOVW R30,R22
	MOVW R26,R28
	ADIW R26,15
	RCALL SUBOPT_0x16
	LD   R30,X
	MOVW R26,R0
	RJMP _0x39
;     255     else                             //
_0x30:
;     256      ++time[a];                      //
	RCALL SUBOPT_0x15
	MOVW R26,R28
	ADIW R26,21
	RCALL SUBOPT_0x16
	LD   R30,X
	SUBI R30,-LOW(1)
_0x39:
	ST   X,R30
;     257 ///////////////////////////////////////
;     258     sec_=0x7f&bin2bcd(time[2]);             //
	RCALL SUBOPT_0xF
	RCALL SUBOPT_0x12
	RCALL SUBOPT_0x13
;     259     min_=0x7f&bin2bcd(time[1]);             //
	RCALL SUBOPT_0x14
;     260     hr_=0x1f&bin2bcd(time[0]);              //
;     261     show_time(
;     262                time_illumnas[(char)(a*2)+2]
;     263               ,time_illumnas[(char)(a*2)+1]
;     264               ,time_illumnas[(char)(a*2)]
;     265               ,25
;     266              );                        //
;     267 ///////////////////////////////////////
;     268     }                                //
	RJMP _0x2D
_0x2F:
;     269    }                                 //
;     270   }                                  //
_0x2C:
	RJMP _0x27
_0x29:
;     271  }                                   //
	SUBI R16,-1
	RJMP _0x25
_0x26:
;     272 write_clk_regs(bin2bcd(time[2])      //
;     273               ,bin2bcd(time[1])      //
;     274               ,0x80|bin2bcd(time[0]) //
;     275               );                     //
	RCALL SUBOPT_0xF
	RCALL _bin2bcd
	ST   -Y,R30
	RCALL SUBOPT_0xF
	RCALL _bin2bcd
	ST   -Y,R30
	RCALL SUBOPT_0xF
	RCALL _bin2bcd
	ORI  R30,0x80
	ST   -Y,R30
	RCALL _write_clk_regs
;     276 }                                    //
	LDD  R16,Y+0
	ADIW R28,24
	RET
;     277 ///////////////////////////////////////
;     278 
;     279 //Timer 1 output compare A interrupt service routine
;     280 interrupt [TIM1_COMPA] void timer1_compa_isr(void)
;     281 {
_timer1_compa_isr:
;     282 // Place your code here
;     283 
;     284 }
	RETI
;     285 
;     286 // Declare your global variables here
;     287 
;     288 void main(void)
;     289 {
_main:
;     290 // Declare your local variables here
;     291 
;     292  unsigned char _sec=60;
;     293 
;     294 
;     295 // Input/Output Ports initialization
;     296 // Port B initialization
;     297 // Func7=In Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     298 // State7=P State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     299 PORTB=0x80;
;	_sec -> R16
	LDI  R16,60
	LDI  R30,LOW(128)
	OUT  0x18,R30
;     300 DDRB=0x7F;
	LDI  R30,LOW(127)
	OUT  0x17,R30
;     301 
;     302 // Port C initialization
;     303 // Func6=In Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     304 // State6=T State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     305 PORTC=0x80;
	LDI  R30,LOW(128)
	OUT  0x15,R30
;     306 DDRC=0x3F;
	LDI  R30,LOW(63)
	OUT  0x14,R30
;     307 
;     308 // Port D initialization
;     309 // Func7=in Func6=in Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
;     310 // State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
;     311 PORTD=0xc0;
	LDI  R30,LOW(192)
	OUT  0x12,R30
;     312 DDRD=0x3f;
	LDI  R30,LOW(63)
	OUT  0x11,R30
;     313 
;     314 // Timer/Counter 0 initialization
;     315 // Clock source: System Clock
;     316 // Clock value: Timer 0 Stopped
;     317 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
;     318 TCNT0=0x00;
	OUT  0x32,R30
;     319 
;     320 // Timer/Counter 1 initialization
;     321 // Clock source: System Clock
;     322 // Clock value: 7.813 kHz
;     323 // Mode: Normal top=FFFFh
;     324 // OC1A output: Discon.
;     325 // OC1B output: Discon.
;     326 // Noise Canceler: Off
;     327 // Input Capture on Falling Edge
;     328 // Timer 1 Overflow Interrupt: Off
;     329 // Input Capture Interrupt: Off
;     330 // Compare A Match Interrupt: On
;     331 // Compare B Match Interrupt: Off
;     332 TCCR1A=0x00;
	OUT  0x2F,R30
;     333 TCCR1B=0x05;
	LDI  R30,LOW(5)
	OUT  0x2E,R30
;     334 TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     335 TCNT1L=0x00;
	OUT  0x2C,R30
;     336 ICR1H=0x00;
	OUT  0x27,R30
;     337 ICR1L=0x00;
	OUT  0x26,R30
;     338 OCR1AH=0x1E;
	LDI  R30,LOW(30)
	OUT  0x2B,R30
;     339 OCR1AL=0x85;
	LDI  R30,LOW(133)
	OUT  0x2A,R30
;     340 OCR1BH=0x00;
	LDI  R30,LOW(0)
	OUT  0x29,R30
;     341 OCR1BL=0x00;
	OUT  0x28,R30
;     342 
;     343 // Timer/Counter 2 initialization
;     344 // Clock source: System Clock
;     345 // Clock value: Timer 2 Stopped
;     346 // Mode: Normal top=FFh
;     347 // OC2 output: Disconnected
;     348 ASSR=0x00;
	OUT  0x22,R30
;     349 TCCR2=0x00;
	OUT  0x25,R30
;     350 TCNT2=0x00;
	OUT  0x24,R30
;     351 OCR2=0x00;
	OUT  0x23,R30
;     352 
;     353 // External Interrupt(s) initialization
;     354 // INT0: Off
;     355 // INT1: Off
;     356 MCUCR=0x00;
	OUT  0x35,R30
;     357 
;     358 // Timer(s)/Counter(s) Interrupt(s) initialization
;     359 TIMSK=0x10;
	LDI  R30,LOW(16)
	OUT  0x39,R30
;     360 
;     361 // Analog Comparator initialization
;     362 // Analog Comparator: Off
;     363 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     364 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     365 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     366 
;     367 // Global enable interrupts
;     368 #asm("sei") 
	sei
;     369 //write_clk_regs(0,0x18,06);
;     370 while (1)
_0x32:
;     371       {
;     372        hold_prog_sw=prog_sw;
	RCALL SUBOPT_0x10
;     373       /////////////////////////////////////////////////
;     374       read_clk_regs(&sec_,&min_,&hr_);
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	RCALL SUBOPT_0xD
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	RCALL SUBOPT_0xD
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	RCALL SUBOPT_0xD
	RCALL _read_clk_regs
;     375       _sec =sec_;
	MOV  R16,R4
;     376       sec_&=0x7f;
	LDI  R30,LOW(127)
	AND  R4,R30
;     377       min_&=0x7f;
	AND  R5,R30
;     378       hr_&=0x1f;
	LDI  R30,LOW(31)
	AND  R6,R30
;     379       show_time(80,80,80,10);
	RCALL SUBOPT_0x17
	RCALL SUBOPT_0x17
	RCALL SUBOPT_0x17
	LDI  R30,LOW(10)
	ST   -Y,R30
	RCALL _show_time
;     380       /////////////////////////////////////////////////
;     381       if(hold_prog_sw!=prog_sw)
	LDI  R30,0
	SBRC R2,0
	LDI  R30,1
	MOV  R26,R30
	LDI  R30,0
	SBIC 0x10,6
	LDI  R30,1
	CP   R30,R26
	BREQ _0x35
;     382        {
;     383        delay_ms(1);
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	RCALL SUBOPT_0xD
	RCALL _delay_ms
;     384        if(prog_sw==0)
	SBIS 0x10,6
;     385         set_time();
	RCALL _set_time
;     386        }
;     387       };
_0x35:
	RJMP _0x32
;     388 }
_0x37:
	RJMP _0x37


;OPTIMIZER ADDED SUBROUTINE, CALLED 12 TIMES, CODE SIZE REDUCTION:20 WORDS
SUBOPT_0x0:
	__DELAY_USB 3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x1:
	LDD  R30,Y+1
	ST   -Y,R30
	RCALL _ds1302_wbyte
	LD   R30,Y
	ST   -Y,R30
	RJMP _ds1302_wbyte

;OPTIMIZER ADDED SUBROUTINE, CALLED 8 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x2:
	ST   -Y,R30
	RJMP _ds1302_wbyte

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x3:
	BST  R30,0
	IN   R26,0x15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x4:
	OUT  0x15,R26
	LD   R30,Y
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x5:
	SWAP R30
	ANDI R30,0xF
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:14 WORDS
SUBOPT_0x6:
	ANDI R30,LOW(0xF)
	LDI  R31,0
	SUBI R30,LOW(-_code)
	SBCI R31,HIGH(-_code)
	LD   R30,Z
	ST   -Y,R30
	RJMP _set_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x7:
	__GETWRN 17,18,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	LDD  R30,Y+8
	LDD  R31,Y+8+1
	CP   R17,R30
	CPC  R18,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x9:
	__ADDWRN 17,18,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:16 WORDS
SUBOPT_0xA:
	ANDI R30,LOW(0xF0)
	RCALL SUBOPT_0x5
	LDI  R31,0
	SUBI R30,LOW(-_code)
	SBCI R31,HIGH(-_code)
	LD   R30,Z
	ST   -Y,R30
	RJMP _set_data

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xB:
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	CP   R17,R30
	CPC  R18,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xC:
	LDD  R30,Y+4
	LDD  R31,Y+4+1
	CP   R17,R30
	CPC  R18,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 16 TIMES, CODE SIZE REDUCTION:13 WORDS
SUBOPT_0xD:
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xE:
	MOVW R26,R30
	LD   R30,X
	ANDI R30,0x7F
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xF:
	LDD  R30,Y+23
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x10:
	CLT
	SBIC 0x10,6
	SET
	BLD  R2,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x11:
	CLT
	SBIC 0x10,7
	SET
	BLD  R2,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x12:
	RCALL _bin2bcd
	ANDI R30,0x7F
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x13:
	MOV  R4,R30
	LDD  R30,Y+22
	ST   -Y,R30
	RJMP SUBOPT_0x12

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:47 WORDS
SUBOPT_0x14:
	MOV  R5,R30
	LDD  R30,Y+21
	ST   -Y,R30
	RCALL _bin2bcd
	ANDI R30,LOW(0x1F)
	MOV  R6,R30
	MOV  R30,R16
	LSL  R30
	SUBI R30,-LOW(2)
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,1
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RCALL SUBOPT_0xD
	MOV  R30,R16
	LSL  R30
	SUBI R30,-LOW(1)
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,3
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RCALL SUBOPT_0xD
	MOV  R30,R16
	LSL  R30
	LDI  R31,0
	MOVW R26,R28
	ADIW R26,5
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RCALL SUBOPT_0xD
	LDI  R30,LOW(25)
	ST   -Y,R30
	RJMP _show_time

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x15:
	MOV  R30,R16
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x16:
	ADD  R26,R30
	ADC  R27,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x17:
	LDI  R30,LOW(80)
	LDI  R31,HIGH(80)
	RJMP SUBOPT_0xD

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

_bcd2bin:
	ld   r30,y
	swap r30
	andi r30,0xf
	mov  r26,r30
	lsl  r26
	lsl  r26
	add  r30,r26
	lsl  r30
	ld   r26,y+
	andi r26,0xf
	add  r30,r26
	ret

_bin2bcd:
	ld   r26,y+
	clr  r30
__bin2bcd0:
	subi r26,10
	brmi __bin2bcd1
	subi r30,-16
	rjmp __bin2bcd0
__bin2bcd1:
	subi r26,-10
	add  r30,r26
	ret

__LSLB12:
	TST  R30
	MOV  R0,R30
	MOV  R30,R26
	BREQ __LSLB12R
__LSLB12L:
	LSL  R30
	DEC  R0
	BRNE __LSLB12L
__LSLB12R:
	RET

__LSRB12:
	TST  R30
	MOV  R0,R30
	MOV  R30,R26
	BREQ __LSRB12R
__LSRB12L:
	LSR  R30
	DEC  R0
	BRNE __LSRB12L
__LSRB12R:
	RET

__GETW1P:
	LD   R30,X+
	LD   R31,X
	SBIW R26,1
	RET

__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

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

__INITLOCB:
__INITLOCW:
	ADD R26,R28
	ADC R27,R29
__INITLOC0:
	LPM  R0,Z+
	ST   X+,R0
	DEC  R24
	BRNE __INITLOC0
	RET

;END OF CODE MARKER
__END_OF_CODE:
