
;CodeVisionAVR C Compiler V1.24.8d Professional
;(C) Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega128
;Program type           : Application
;Clock frequency        : 4,000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 1024 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega128
	#pragma AVRPART MEMORY PROG_FLASH 131072
	#pragma AVRPART MEMORY EEPROM 4096
	#pragma AVRPART MEMORY INT_SRAM SIZE 4096
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
	.EQU MCUCR=0x35
	.EQU RAMPZ=0x3B
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

	.EQU __se_bit=0x20
	.EQU __sm_mask=0x1C
	.EQU __sm_adc_noise_red=0x08
	.EQU __sm_powerdown=0x10
	.EQU __sm_powersave=0x18
	.EQU __sm_standby=0x14
	.EQU __sm_ext_standby=0x1C

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

	.INCLUDE "LCD_BMI.vec"
	.INCLUDE "LCD_BMI.inc"

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
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x1000)
	LDI  R25,HIGH(0x1000)
	LDI  R26,LOW(0x100)
	LDI  R27,HIGH(0x100)
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

	OUT  RAMPZ,R24

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
	LDI  R30,LOW(0x10FF)
	OUT  SPL,R30
	LDI  R30,HIGH(0x10FF)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x500)
	LDI  R29,HIGH(0x500)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x500
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.8d Professional
;       4 Automatic Program Generator
;       5 © Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 
;       8 Project : 
;       9 Version : 
;      10 Date    : 12-10-2006
;      11 Author  : F4CG                            
;      12 Company : F4CG                            
;      13 Comments: 
;      14 
;      15 
;      16 Chip type           : ATmega128
;      17 Program type        : Application
;      18 Clock frequency     : 4,000000 MHz
;      19 Memory model        : Small
;      20 External SRAM size  : 0
;      21 Data Stack size     : 1024
;      22 *****************************************************/
;      23 
;      24 #include <mega128.h>
;      25 
;      26 // Alphanumeric LCD Module functions
;      27 #asm
;      28    .equ __lcd_port=0x15 ;PORTC
   .equ __lcd_port=0x15 ;PORTC
;      29 #endasm
;      30 #include <lcd.h> 
;      31 #include <delay.h>
;      32 
;      33 // Declare your global variables here
;      34  
;      35 int hoejde=150;
;      36 int vaegt=700;
;      37 
;      38 void SplitInto1_2Digitsh(unsigned int Split)
;      39 {

	.CSEG
_SplitInto1_2Digitsh:
;      40 unsigned char digit[3];
;      41 //Split=(Split*10)/3;
;      42 //digit[0]=(Split/1000);
;      43 digit[0]=(Split/100);		// 100'ere
	CALL SUBOPT_0x0
;	Split -> Y+3
;	digit -> Y+0
;      44 digit[1]=(Split/10)%10;		// 10'ere
;      45 digit[2]=Split%10;		// 1'ere
;      46 lcd_putchar(digit[0]+48);
;      47 lcd_putsf(".");
	CALL SUBOPT_0x1
;      48 lcd_putchar(digit[1]+48);
	LDD  R30,Y+1
	CALL SUBOPT_0x2
;      49 lcd_putchar(digit[2]+48);
	LDD  R30,Y+2
	CALL SUBOPT_0x2
;      50 lcd_putsf(" meter");
	__POINTW1FN _0,2
	CALL SUBOPT_0x3
;      51 Split=0;
;      52 }        
	RJMP _0x2D
;      53 void SplitInto1_2Digitsv(unsigned int Split)
;      54 {
_SplitInto1_2Digitsv:
;      55 unsigned char digit[3];
;      56 //Split=(Split*10)/3;
;      57 //digit[0]=(Split/1000);
;      58 digit[0]=(Split/100);		// 100'ere
	CALL SUBOPT_0x0
;	Split -> Y+3
;	digit -> Y+0
;      59 digit[1]=(Split/10)%10;		// 10'ere
;      60 digit[2]=Split%10;		// 1'ere
;      61 lcd_putchar(digit[0]+48);
;      62 lcd_putchar(digit[1]+48);
	LDD  R30,Y+1
	CALL SUBOPT_0x2
;      63 lcd_putsf(".");
	CALL SUBOPT_0x1
;      64 lcd_putchar(digit[2]+48);
	LDD  R30,Y+2
	CALL SUBOPT_0x2
;      65 lcd_putsf(" kilo");
	__POINTW1FN _0,9
	CALL SUBOPT_0x3
;      66 Split=0;
;      67 }
_0x2D:
	ADIW R28,5
	RET
;      68           
;      69 void SplitInto4Digits(unsigned int Split)
;      70 {
_SplitInto4Digits:
;      71 	unsigned int digit[4];
;      72 	digit[0]=(Split/1000);		// 100'ere
	SBIW R28,8
;	Split -> Y+8
;	digit -> Y+0
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	CALL __DIVW21U
	ST   Y,R30
	STD  Y+1,R31
;      73 	digit[1]=(Split/100)%10;		// 10'ere
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	CALL SUBOPT_0x4
	STD  Y+2,R30
	STD  Y+2+1,R31
;      74 	digit[2]=(Split/10)%10;		// 10'ere
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL SUBOPT_0x4
	STD  Y+4,R30
	STD  Y+4+1,R31
;      75 	digit[3]=Split%10;		// 1'ere	
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __MODW21U
	STD  Y+6,R30
	STD  Y+6+1,R31
;      76 	lcd_putchar(digit[0]+48);
	LD   R30,Y
	LDD  R31,Y+1
	CALL SUBOPT_0x5
;      77 	lcd_putchar(digit[1]+48);
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	CALL SUBOPT_0x5
;      78 	lcd_putsf(".");
	CALL SUBOPT_0x1
;      79 	lcd_putchar(digit[2]+48);
	LDD  R30,Y+4
	LDD  R31,Y+4+1
	CALL SUBOPT_0x5
;      80         lcd_putchar(digit[3]+48);
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	CALL SUBOPT_0x5
;      81 
;      82 }
	ADIW R28,10
	RET
;      83 void SplitInto10Digits(float Split)
;      84 {
;      85 	float digit[5];
;      86 	digit[0]=(Split/100000)%10;		//      100.000'ere
;	Split -> Y+20
;	digit -> Y+0
;      87 	digit[1]=(Split/10000)%10;		//       10.000'ere
;      88 	digit[2]=(Split/1000)%10;		//         1000'ere
;      89 	digit[3]=(Split/100)%10;		//          100'ere
;      90 	digit[4]=(Split/10)%10;		        //           10'ere
;      91 	digit[5]=Split%10;		        //            1'ere	
;      92 	lcd_putchar(digit[0]+48);
;      93 	lcd_putchar(digit[1]+48);
;      94 	lcd_putchar(digit[2]+48);
;      95         lcd_putchar(digit[3]+48);
;      96         lcd_putchar(digit[4]+48);
;      97         lcd_putchar(digit[5]+48);
;      98         }
;      99 void udregnet_bmi(float bmi)
;     100         {
_udregnet_bmi:
;     101 
;     102         lcd_gotoxy(7,1);
;	bmi -> Y+0
	LDI  R30,LOW(7)
	CALL SUBOPT_0x6
;     103         bmi=vaegt/((hoejde/100.0)*(hoejde/100.0))*10.0;
	MOVW R30,R4
	CALL __CWD1
	CALL __CDF1
	MOVW R26,R30
	MOVW R24,R22
	__GETD1N 0x42C80000
	CALL __DIVF21
	MOVW R26,R30
	MOVW R24,R22
	CALL __MULF12
	MOVW R26,R6
	CALL __CWD2
	CALL __CDF2
	CALL __DIVF21
	__GETD2N 0x41200000
	CALL __MULF12
	__PUTD1S 0
;     104         SplitInto4Digits(bmi);
	CALL __CFD1
	ST   -Y,R31
	ST   -Y,R30
	CALL _SplitInto4Digits
;     105         }  
	ADIW R28,4
	RET
;     106 void main(void)
;     107 {
_main:
;     108 
;     109 
;     110 
;     111 // Declare your local variables here
;     112 
;     113 // Input/Output Ports initialization
;     114 // Port A initialization
;     115 // Func7=In Func6=Out Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     116 // State7=T State6=0 State5=T State4=T State3=T State2=T State1=T State0=T 
;     117 PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;     118 DDRA=0x40;
	LDI  R30,LOW(64)
	OUT  0x1A,R30
;     119 
;     120 // Port B initialization
;     121 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     122 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     123 PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
;     124 DDRB=0x00;
	OUT  0x17,R30
;     125 
;     126 // Port C initialization
;     127 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     128 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     129 PORTC=0x00;
	OUT  0x15,R30
;     130 DDRC=0x00;
	OUT  0x14,R30
;     131 
;     132 // Port D initialization
;     133 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     134 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     135 PORTD=0x00;
	OUT  0x12,R30
;     136 DDRD=0x00;
	OUT  0x11,R30
;     137 
;     138 // Port E initialization
;     139 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     140 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     141 PORTE=0x00;
	OUT  0x3,R30
;     142 DDRE=0x00;
	OUT  0x2,R30
;     143 
;     144 // Port F initialization
;     145 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;     146 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;     147 PORTF=0x00;
	STS  98,R30
;     148 DDRF=0x00;
	STS  97,R30
;     149 
;     150 // Port G initialization
;     151 // Func4=In Func3=In Func2=In Func1=In Func0=In 
;     152 // State4=T State3=T State2=T State1=T State0=T 
;     153 PORTG=0x00;
	STS  101,R30
;     154 DDRG=0x00;
	STS  100,R30
;     155 
;     156 // Timer/Counter 0 initialization
;     157 // Clock source: System Clock
;     158 // Clock value: Timer 0 Stopped
;     159 // Mode: Normal top=FFh
;     160 // OC0 output: Disconnected
;     161 ASSR=0x00;
	OUT  0x30,R30
;     162 TCCR0=0x00;
	OUT  0x33,R30
;     163 TCNT0=0x00;
	OUT  0x32,R30
;     164 OCR0=0x00;
	OUT  0x31,R30
;     165 
;     166 // Timer/Counter 1 initialization
;     167 // Clock source: System Clock
;     168 // Clock value: Timer 1 Stopped
;     169 // Mode: Normal top=FFFFh
;     170 // OC1A output: Discon.
;     171 // OC1B output: Discon.
;     172 // OC1C output: Discon.
;     173 // Noise Canceler: Off
;     174 // Input Capture on Falling Edge
;     175 // Timer 1 Overflow Interrupt: Off
;     176 // Input Capture Interrupt: Off
;     177 // Compare A Match Interrupt: Off
;     178 // Compare B Match Interrupt: Off
;     179 // Compare C Match Interrupt: Off
;     180 TCCR1A=0x00;
	OUT  0x2F,R30
;     181 TCCR1B=0x00;
	OUT  0x2E,R30
;     182 TCNT1H=0x00;
	OUT  0x2D,R30
;     183 TCNT1L=0x00;
	OUT  0x2C,R30
;     184 ICR1H=0x00;
	OUT  0x27,R30
;     185 ICR1L=0x00;
	OUT  0x26,R30
;     186 OCR1AH=0x00;
	OUT  0x2B,R30
;     187 OCR1AL=0x00;
	OUT  0x2A,R30
;     188 OCR1BH=0x00;
	OUT  0x29,R30
;     189 OCR1BL=0x00;
	OUT  0x28,R30
;     190 OCR1CH=0x00;
	STS  121,R30
;     191 OCR1CL=0x00;
	STS  120,R30
;     192 
;     193 // Timer/Counter 2 initialization
;     194 // Clock source: System Clock
;     195 // Clock value: Timer 2 Stopped
;     196 // Mode: Normal top=FFh
;     197 // OC2 output: Disconnected
;     198 TCCR2=0x00;
	OUT  0x25,R30
;     199 TCNT2=0x00;
	OUT  0x24,R30
;     200 OCR2=0x00;
	OUT  0x23,R30
;     201 
;     202 // Timer/Counter 3 initialization
;     203 // Clock source: System Clock
;     204 // Clock value: Timer 3 Stopped
;     205 // Mode: Normal top=FFFFh
;     206 // Noise Canceler: Off
;     207 // Input Capture on Falling Edge
;     208 // OC3A output: Discon.
;     209 // OC3B output: Discon.
;     210 // OC3C output: Discon.
;     211 // Timer 3 Overflow Interrupt: Off
;     212 // Input Capture Interrupt: Off
;     213 // Compare A Match Interrupt: Off
;     214 // Compare B Match Interrupt: Off
;     215 // Compare C Match Interrupt: Off
;     216 TCCR3A=0x00;
	STS  139,R30
;     217 TCCR3B=0x00;
	STS  138,R30
;     218 TCNT3H=0x00;
	STS  137,R30
;     219 TCNT3L=0x00;
	STS  136,R30
;     220 ICR3H=0x00;
	STS  129,R30
;     221 ICR3L=0x00;
	STS  128,R30
;     222 OCR3AH=0x00;
	STS  135,R30
;     223 OCR3AL=0x00;
	STS  134,R30
;     224 OCR3BH=0x00;
	STS  133,R30
;     225 OCR3BL=0x00;
	STS  132,R30
;     226 OCR3CH=0x00;
	STS  131,R30
;     227 OCR3CL=0x00;
	STS  130,R30
;     228 
;     229 // External Interrupt(s) initialization
;     230 // INT0: Off
;     231 // INT1: Off
;     232 // INT2: Off
;     233 // INT3: Off
;     234 // INT4: Off
;     235 // INT5: Off
;     236 // INT6: Off
;     237 // INT7: Off
;     238 EICRA=0x00;
	STS  106,R30
;     239 EICRB=0x00;
	OUT  0x3A,R30
;     240 EIMSK=0x00;
	OUT  0x39,R30
;     241 
;     242 // Timer(s)/Counter(s) Interrupt(s) initialization
;     243 TIMSK=0x00;
	OUT  0x37,R30
;     244 ETIMSK=0x00;
	STS  125,R30
;     245 
;     246 // Analog Comparator initialization
;     247 // Analog Comparator: Off
;     248 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     249 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     250 SFIOR=0x00;   
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     251 
;     252 
;     253 
;     254 // LCD module initialization
;     255 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     256 
;     257 while (1)
_0x5:
;     258       {
;     259                 
;     260          while (PINA.3==0)
_0x8:
	SBIS 0x19,3
;     261          {
;     262          } 
	RJMP _0x8
;     263         
;     264         //forste raekke
;     265         lcd_gotoxy(2,0);
	LDI  R30,LOW(2)
	CALL SUBOPT_0x7
;     266 
;     267         lcd_putsf("indtast hoejde");
	__POINTW1FN _0,15
	CALL SUBOPT_0x8
;     268         
;     269         //anden reakke
;     270         while (PINA.3==1)  //!        
_0xB:
	SBIS 0x19,3
	RJMP _0xD
;     271         {
;     272         if (PINA.0==0)     //!
	SBIC 0x19,0
	RJMP _0xE
;     273         {
;     274 	hoejde++;
	MOVW R30,R4
	ADIW R30,1
	MOVW R4,R30
;     275 	if (hoejde>250)
	LDI  R30,LOW(250)
	LDI  R31,HIGH(250)
	CP   R30,R4
	CPC  R31,R5
	BRGE _0xF
;     276         hoejde = 150;
	LDI  R30,LOW(150)
	LDI  R31,HIGH(150)
	MOVW R4,R30
;     277         }
_0xF:
;     278         else if (PINA.4==0)  //!
	RJMP _0x10
_0xE:
	SBIC 0x19,4
	RJMP _0x11
;     279 	{
;     280 	hoejde--;
	MOVW R30,R4
	SBIW R30,1
	MOVW R4,R30
;     281 	if (hoejde<100)
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	CP   R4,R30
	CPC  R5,R31
	BRGE _0x12
;     282         hoejde = 150;
	LDI  R30,LOW(150)
	LDI  R31,HIGH(150)
	MOVW R4,R30
;     283         }
_0x12:
;     284         lcd_gotoxy(5,1);
_0x11:
_0x10:
	LDI  R30,LOW(5)
	CALL SUBOPT_0x6
;     285         SplitInto1_2Digitsh(hoejde);
	ST   -Y,R5
	ST   -Y,R4
	CALL _SplitInto1_2Digitsh
;     286         delay_ms(500);
	CALL SUBOPT_0x9
;     287         }                       
	RJMP _0xB
_0xD:
;     288         
;     289         
;     290          while (PINA.3==0)
_0x13:
	SBIC 0x19,3
	RJMP _0x15
;     291          {
;     292          lcd_clear();
	CALL _lcd_clear
;     293          } 
	RJMP _0x13
_0x15:
;     294         
;     295         while (PINA.3==1)  //!        
_0x16:
	SBIS 0x19,3
	RJMP _0x18
;     296         {
;     297                 //forste raekke
;     298         lcd_gotoxy(1,0);
	LDI  R30,LOW(1)
	CALL SUBOPT_0x7
;     299 
;     300         lcd_putsf("indtast vaegt");
	__POINTW1FN _0,30
	CALL SUBOPT_0x8
;     301         
;     302         //anden reakke
;     303         if (PINA.0==0)     //!
	SBIC 0x19,0
	RJMP _0x19
;     304         {
;     305 	vaegt++;
	MOVW R30,R6
	ADIW R30,1
	RJMP _0x2E
;     306 
;     307         }
;     308         else if (PINA.4==0)  //!
_0x19:
	SBIC 0x19,4
	RJMP _0x1B
;     309 	{
;     310 	vaegt--;
	MOVW R30,R6
	SBIW R30,1
_0x2E:
	MOVW R6,R30
;     311         }
;     312         lcd_gotoxy(5,1);
_0x1B:
	LDI  R30,LOW(5)
	CALL SUBOPT_0x6
;     313         SplitInto1_2Digitsv(vaegt);
	ST   -Y,R7
	ST   -Y,R6
	CALL _SplitInto1_2Digitsv
;     314         delay_ms(500);
	CALL SUBOPT_0x9
;     315         }  
	RJMP _0x16
_0x18:
;     316                 //forste raekke
;     317         
;     318         while (PINA.3==0)
_0x1C:
	SBIC 0x19,3
	RJMP _0x1E
;     319         {
;     320          lcd_clear();               
	CALL _lcd_clear
;     321         }
	RJMP _0x1C
_0x1E:
;     322         
;     323         while (PINA.3==1)
_0x1F:
	SBIS 0x19,3
	RJMP _0x21
;     324         {
;     325         lcd_gotoxy(5,0);
	LDI  R30,LOW(5)
	CALL SUBOPT_0x7
;     326         lcd_putsf("Din BMI");
	__POINTW1FN _0,44
	CALL SUBOPT_0x8
;     327         //anden raekke
;     328         
;     329         udregnet_bmi(0); 
	__GETD1N 0x0
	CALL __PUTPARD1
	CALL _udregnet_bmi
;     330         }
	RJMP _0x1F
_0x21:
;     331             
;     332       };
	RJMP _0x5
;     333 }
_0x22:
	RJMP _0x22

    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G2:
	.BYTE 0x4

	.CSEG
__lcd_delay_G2:
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
	CALL __lcd_delay_G2
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G2:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G2
    ld    r26,y
    swap  r26
	CALL __lcd_write_nibble_G2
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G2:
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
    andi  r30,0xf0
	RET
_lcd_read_byte0_G2:
	CALL __lcd_delay_G2
	CALL __lcd_read_nibble_G2
    mov   r26,r30
	CALL __lcd_read_nibble_G2
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	CALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G2)
	SBCI R31,HIGH(-__base_y_G2)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	ST   -Y,R30
	CALL __lcd_write_data
	LDD  R8,Y+1
	LDD  R9,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R30,LOW(2)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R30,LOW(12)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	LDI  R30,LOW(0)
	MOV  R9,R30
	MOV  R8,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R8
	CP   R10,R8
	BRSH _0x24
	__lcd_putchar1:
	INC  R9
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R9
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x24:
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
_lcd_putsf:
	ST   -Y,R16
_0x28:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R16,R30
	CPI  R30,0
	BREQ _0x2A
	ST   -Y,R16
	CALL _lcd_putchar
	RJMP _0x28
_0x2A:
	LDD  R16,Y+0
	ADIW R28,3
	RET
__long_delay_G2:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G2:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G2
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LDD  R10,Y+0
	LD   R30,Y
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G2,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G2,3
	CALL SUBOPT_0xA
	CALL SUBOPT_0xA
	CALL SUBOPT_0xA
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	CALL SUBOPT_0xB
	LDI  R30,LOW(4)
	CALL SUBOPT_0xB
	LDI  R30,LOW(133)
	CALL SUBOPT_0xB
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x2B
	LDI  R30,LOW(0)
	RJMP _0x2C
_0x2B:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x2C:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:27 WORDS
SUBOPT_0x0:
	SBIW R28,3
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	CALL __DIVW21U
	ST   Y,R30
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __DIVW21U
	MOVW R26,R30
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __MODW21U
	STD  Y+1,R30
	LDD  R26,Y+3
	LDD  R27,Y+3+1
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __MODW21U
	STD  Y+2,R30
	LD   R30,Y
	SUBI R30,-LOW(48)
	ST   -Y,R30
	JMP  _lcd_putchar

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x1:
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	JMP  _lcd_putsf

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x2:
	SUBI R30,-LOW(48)
	ST   -Y,R30
	JMP  _lcd_putchar

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x3:
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
	LDI  R30,0
	STD  Y+3,R30
	STD  Y+3+1,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x4:
	CALL __DIVW21U
	MOVW R26,R30
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	CALL __MODW21U
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x5:
	ADIW R30,48
	ST   -Y,R30
	JMP  _lcd_putchar

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x6:
	ST   -Y,R30
	LDI  R30,LOW(1)
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x7:
	ST   -Y,R30
	LDI  R30,LOW(0)
	ST   -Y,R30
	JMP  _lcd_gotoxy

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	ST   -Y,R31
	ST   -Y,R30
	JMP  _lcd_putsf

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x9:
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0xA:
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xB:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G2

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x3E8
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__ANEGD1:
	COM  R30
	COM  R31
	COM  R22
	COM  R23
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	SBCI R23,-1
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

__MODW21U:
	RCALL __DIVW21U
	MOVW R30,R26
	RET

__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
	RET

__CDF2U:
	SET
	RJMP __CDF2U0
__CDF2:
	CLT
__CDF2U0:
	RCALL __SWAPD12
	RCALL __CDF1U0

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

__CFD1:
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
	BRLO __CFD17
	SER  R30
	SER  R31
	SER  R22
	LDI  R23,0x7F
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

__MINRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	SER  R23
	POP  R21
	RET

__ZERORES:
	CLR  R30
	CLR  R31
	CLR  R22
	CLR  R23
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
	PUSH R19
	PUSH R20
	CLR  R1
	CLR  R19
	CLR  R20
	CLR  R21
	LDI  R25,24
__MULF120:
	LSL  R19
	ROL  R20
	ROL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	BRCC __MULF121
	ADD  R19,R26
	ADC  R20,R27
	ADC  R21,R24
	ADC  R30,R1
	ADC  R31,R1
	ADC  R22,R1
__MULF121:
	DEC  R25
	BRNE __MULF120
	POP  R20
	POP  R19
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
	RCALL __REPACK
	POP  R21
	RET

__DIVF21:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BRNE __DIVF210
	TST  R1
__DIVF211:
	BRPL __MAXRES
	RJMP __MINRES
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
	LSR  R22
	ROR  R31
	ROR  R30
	LSR  R24
	ROR  R27
	ROR  R26
	PUSH R20
	CLR  R1
	CLR  R20
	CLR  R21
	LDI  R25,24
__DIVF212:
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __DIVF213
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SEC
	RJMP __DIVF214
__DIVF213:
	CLC
__DIVF214:
	ROL  R1
	ROL  R20
	ROL  R21
	ROL  R26
	ROL  R27
	ROL  R24
	DEC  R25
	BRNE __DIVF212
	MOV  R30,R1
	MOV  R31,R20
	MOV  R22,R21
	LSR  R26
	ADC  R30,R25
	ADC  R31,R25
	ADC  R22,R25
	POP  R20
	TST  R22
	BRMI __DIVF215
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVS __DIVF217
__DIVF215:
	RCALL __REPACK
	POP  R21
	RET

;END OF CODE MARKER
__END_OF_CODE:
