
;CodeVisionAVR C Compiler V1.24.8d Professional
;(C) Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega16
;Program type           : Application
;Clock frequency        : 4.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : float, width, precision
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

	#pragma AVRPART ADMIN PART_NAME ATmega16
	#pragma AVRPART MEMORY PROG_FLASH 16384
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

	.INCLUDE "p2.vec"
	.INCLUDE "p2.inc"

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

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x160
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.8d Professional
;       4 Automatic Program Generator
;       5 © Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 
;       8 Project : 
;       9 Version : 
;      10 Date    : 6/27/2007
;      11 Author  : F4CG                            
;      12 Company : F4CG                            
;      13 Comments: 
;      14 
;      15 
;      16 Chip type           : ATmega16
;      17 Program type        : Application
;      18 Clock frequency     : 4.000000 MHz
;      19 Memory model        : Small
;      20 External SRAM size  : 0
;      21 Data Stack size     : 256
;      22 *****************************************************/
;      23 
;      24 #include <mega16.h>
;      25 
;      26 // Alphanumeric LCD Module functions
;      27 #asm
;      28    .equ __lcd_port=0x1B ;PORTA
   .equ __lcd_port=0x1B ;PORTA
;      29 #endasm
;      30 #include <lcd.h>
;      31 #include <delay.h>
;      32 #include <stdio.h>
;      33 #include <math.h>
;      34 
;      35   float a,d=0,d1,f=0,i,de; 
_a:
	.BYTE 0x4
_d:
	.BYTE 0x4
_d1:
	.BYTE 0x4
_f:
	.BYTE 0x4
_i:
	.BYTE 0x4
_de:
	.BYTE 0x4
;      36 // External Interrupt 0 service routine
;      37 interrupt [EXT_INT0] void ext_int0_isr(void)
;      38 {

	.CSEG
_ext_int0_isr:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R22
	ST   -Y,R23
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      39 // Place your code here
;      40   a=TCNT1;
	IN   R30,0x2C
	IN   R31,0x2C+1
	CLR  R22
	CLR  R23
	CALL __CDF1
	STS  _a,R30
	STS  _a+1,R31
	STS  _a+2,R22
	STS  _a+3,R23
;      41   TCNT1=0;
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x2C+1,R31
	OUT  0x2C,R30
;      42   }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R23,Y+
	LD   R22,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;      43 
;      44 // Timer 2 overflow interrupt service routine
;      45 interrupt [TIM2_OVF] void timer2_ovf_isr(void)
;      46 {
_timer2_ovf_isr:
;      47 // Place your code here
;      48 
;      49 }
	RETI
;      50 
;      51 // Timer 2 output compare interrupt service routine
;      52 interrupt [TIM2_COMP] void timer2_comp_isr(void)
;      53 {
_timer2_comp_isr:
;      54 // Place your code here
;      55 
;      56 }
	RETI
;      57 
;      58 // Declare your global variables here
;      59 
;      60 void main(void)
;      61 {
_main:
;      62 // Declare your local variables here
;      63 float t;
;      64 char lcd_buf[30],i;
;      65 // Input/Output Ports initialization
;      66 // Port A initialization
;      67 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      68 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      69 PORTA=0x00;
	SBIW R28,34
;	t -> Y+30
;	lcd_buf -> Y+0
;	i -> R16
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;      70 DDRA=0x00;
	OUT  0x1A,R30
;      71 
;      72 // Port B initialization
;      73 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      74 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      75 PORTB=0x00;
	OUT  0x18,R30
;      76 DDRB=0x00;
	OUT  0x17,R30
;      77 
;      78 // Port C initialization
;      79 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      80 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      81 PORTC=0x00;
	OUT  0x15,R30
;      82 DDRC=0x00;
	OUT  0x14,R30
;      83 
;      84 // Port D initialization
;      85 // Func7=Out Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
;      86 // State7=0 State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
;      87 PORTD=0x00;
	OUT  0x12,R30
;      88 DDRD=0x80;
	LDI  R30,LOW(128)
	OUT  0x11,R30
;      89 
;      90 // Timer/Counter 0 initialization
;      91 // Clock source: System Clock
;      92 // Clock value: Timer 0 Stopped
;      93 // Mode: Normal top=FFh
;      94 // OC0 output: Disconnected
;      95 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
;      96 TCNT0=0x00;
	OUT  0x32,R30
;      97 OCR0=0x00;
	OUT  0x3C,R30
;      98 
;      99 // Timer/Counter 1 initialization
;     100 // Clock source: System Clock
;     101 // Clock value: 500.000 kHz
;     102 // Mode: Normal top=FFFFh
;     103 // OC1A output: Discon.
;     104 // OC1B output: Discon.
;     105 // Noise Canceler: Off
;     106 // Input Capture on Falling Edge
;     107 // Timer 1 Overflow Interrupt: Off
;     108 // Input Capture Interrupt: Off
;     109 // Compare A Match Interrupt: Off
;     110 // Compare B Match Interrupt: Off
;     111 TCCR1A=0x00;
	OUT  0x2F,R30
;     112 TCCR1B=0x02;
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     113 TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
;     114 TCNT1L=0x00;
	OUT  0x2C,R30
;     115 ICR1H=0x00;
	OUT  0x27,R30
;     116 ICR1L=0x00;
	OUT  0x26,R30
;     117 OCR1AH=0x00;
	OUT  0x2B,R30
;     118 OCR1AL=0x00;
	OUT  0x2A,R30
;     119 OCR1BH=0x00;
	OUT  0x29,R30
;     120 OCR1BL=0x00;
	OUT  0x28,R30
;     121 
;     122 // Timer/Counter 2 initialization
;     123 // Clock source: System Clock
;     124 // Clock value: 500.000 kHz
;     125 // Mode: Fast PWM top=FFh
;     126 // OC2 output: Non-Inverted PWM
;     127 ASSR=0x00;
	OUT  0x22,R30
;     128 TCCR2=0x6A;
	LDI  R30,LOW(106)
	OUT  0x25,R30
;     129 TCNT2=0x00;
	LDI  R30,LOW(0)
	OUT  0x24,R30
;     130 OCR2=0x99;
	LDI  R30,LOW(153)
	OUT  0x23,R30
;     131 
;     132 // External Interrupt(s) initialization
;     133 // INT0: On
;     134 // INT0 Mode: Falling Edge
;     135 // INT1: Off
;     136 // INT2: Off
;     137 GICR|=0x40;
	IN   R30,0x3B
	ORI  R30,0x40
	OUT  0x3B,R30
;     138 MCUCR=0x02;
	LDI  R30,LOW(2)
	OUT  0x35,R30
;     139 MCUCSR=0x00;
	LDI  R30,LOW(0)
	OUT  0x34,R30
;     140 GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
;     141 
;     142 // Timer(s)/Counter(s) Interrupt(s) initialization
;     143 TIMSK=0xC0;
	LDI  R30,LOW(192)
	OUT  0x39,R30
;     144 
;     145 // Analog Comparator initialization
;     146 // Analog Comparator: Off
;     147 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     148 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     149 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     150 
;     151 // LCD module initialization
;     152 lcd_init(16);
	LDI  R30,LOW(16)
	ST   -Y,R30
	CALL _lcd_init
;     153 
;     154 // Global enable interrupts
;     155 #asm("sei")
	sei
;     156 
;     157 while (1) {
_0x3:
;     158    delay_ms(100);
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     159    t=a*0.000002;
	LDS  R26,_a
	LDS  R27,_a+1
	LDS  R24,_a+2
	LDS  R25,_a+3
	__GETD1N 0x360637BD
	CALL __MULF12
	__PUTD1S 30
;     160    f=1/t;
	__GETD2N 0x3F800000
	CALL __DIVF21
	STS  _f,R30
	STS  _f+1,R31
	STS  _f+2,R22
	STS  _f+3,R23
;     161    sprintf(lcd_buf,"f=%5.4f ",f);
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_f
	LDS  R31,_f+1
	LDS  R22,_f+2
	LDS  R23,_f+3
	CALL __PUTPARD1
	LDI  R24,4
	CALL _sprintf
	ADIW R28,8
;     162    lcd_clear();
	CALL _lcd_clear
;     163    lcd_puts(lcd_buf);
	MOVW R30,R28
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_puts
;     164    delay_ms(10);
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     165    d1=d;
	CALL SUBOPT_0x0
	STS  _d1,R30
	STS  _d1+1,R31
	STS  _d1+2,R22
	STS  _d1+3,R23
;     166    //detect the error
;     167    d=f-36;
	LDS  R26,_f
	LDS  R27,_f+1
	LDS  R24,_f+2
	LDS  R25,_f+3
	__GETD1N 0x42100000
	CALL SUBOPT_0x1
	STS  _d,R30
	STS  _d+1,R31
	STS  _d+2,R22
	STS  _d+3,R23
;     168    //i is the integral parameter
;     169    i=d+d1;
	LDS  R30,_d1
	LDS  R31,_d1+1
	LDS  R22,_d1+2
	LDS  R23,_d1+3
	LDS  R26,_d
	LDS  R27,_d+1
	LDS  R24,_d+2
	LDS  R25,_d+3
	CALL __ADDF12
	CALL __CFD1
	MOV  R16,R30
;     170    //de is the derive parameter
;     171    de=d-d1;
	LDS  R26,_d1
	LDS  R27,_d1+1
	LDS  R24,_d1+2
	LDS  R25,_d1+3
	CALL SUBOPT_0x0
	CALL __SUBF12
	STS  _de,R30
	STS  _de+1,R31
	STS  _de+2,R22
	STS  _de+3,R23
;     172    //and we assume the pwm by the combination of these three parameters
;     173    OCR2-=((int)(d))-((int)(i*0.001))-((int)(de*0.1));
	IN   R30,0x23
	PUSH R30
	CALL SUBOPT_0x0
	CALL __CFD1
	PUSH R31
	PUSH R30
	MOV  R30,R16
	CALL SUBOPT_0x2
	__GETD2N 0x3A83126F
	CALL __MULF12
	CALL __CFD1
	POP  R26
	POP  R27
	CALL SUBOPT_0x3
	PUSH R31
	PUSH R30
	LDS  R26,_de
	LDS  R27,_de+1
	LDS  R24,_de+2
	LDS  R25,_de+3
	__GETD1N 0x3DCCCCCD
	CALL __MULF12
	CALL __CFD1
	POP  R26
	POP  R27
	CALL SUBOPT_0x3
	POP  R26
	LDI  R27,0
	CALL SUBOPT_0x3
	OUT  0x23,R30
;     174 // Place your code here
;     175       };
	RJMP _0x3
;     176 }
_0x6:
	RJMP _0x6

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
	LDD  R4,Y+1
	LDD  R5,Y+0
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
	MOV  R5,R30
	MOV  R4,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R4
	CP   R6,R4
	BRSH _0x8
	__lcd_putchar1:
	INC  R5
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R5
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x8:
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
	ST   -Y,R16
_0x9:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R16,R30
	CPI  R30,0
	BREQ _0xB
	ST   -Y,R16
	CALL _lcd_putchar
	RJMP _0x9
_0xB:
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
	LDD  R6,Y+0
	LD   R30,Y
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G2,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G2,3
	CALL SUBOPT_0x4
	CALL SUBOPT_0x4
	CALL SUBOPT_0x4
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	CALL SUBOPT_0x5
	LDI  R30,LOW(4)
	CALL SUBOPT_0x5
	LDI  R30,LOW(133)
	CALL SUBOPT_0x5
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0xF
	LDI  R30,LOW(0)
	RJMP _0xBC
_0xF:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0xBC:
	ADIW R28,1
	RET
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
	LD   R26,Y
	LDD  R27,Y+1
	CALL __GETW1P
	SBIW R30,0
	BREQ _0x10
	LD   R30,X+
	LD   R31,X+
	ADIW R30,1
	ST   -X,R31
	ST   -X,R30
	SBIW R30,1
	LDD  R26,Y+2
	STD  Z+0,R26
	RJMP _0x11
_0x10:
	LDD  R30,Y+2
	ST   -Y,R30
	CALL _putchar
_0x11:
	ADIW R28,3
	RET
__ftoa_G3:
	SBIW R28,4
	ST   -Y,R17
	ST   -Y,R16
	LDD  R26,Y+8
	CPI  R26,LOW(0x6)
	BRLO _0x12
	LDI  R30,LOW(5)
	STD  Y+8,R30
_0x12:
	LDD  R30,Y+8
	LDI  R26,LOW(__fround_G3*2)
	LDI  R27,HIGH(__fround_G3*2)
	LDI  R31,0
	CALL __LSLW2
	ADD  R30,R26
	ADC  R31,R27
	CALL __GETD1PF
	CALL SUBOPT_0x6
	CALL __ADDF12
	CALL SUBOPT_0x7
	LDI  R16,LOW(0)
	__GETD1N 0x3F800000
	CALL SUBOPT_0x8
_0x13:
	CALL SUBOPT_0x9
	CALL __CMPF12
	BRLO _0x15
	CALL SUBOPT_0xA
	CALL __MULF12
	CALL SUBOPT_0x8
	SUBI R16,-LOW(1)
	RJMP _0x13
_0x15:
	CPI  R16,0
	BRNE _0x16
	CALL SUBOPT_0xB
	LDI  R30,LOW(48)
	ST   X,R30
	RJMP _0x17
_0x16:
_0x18:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0x1A
	CALL SUBOPT_0xA
	CALL SUBOPT_0xC
	CALL SUBOPT_0x8
	CALL SUBOPT_0x9
	CALL __DIVF21
	CALL __CFD1
	MOV  R17,R30
	CALL SUBOPT_0xB
	CALL SUBOPT_0xD
	__GETD2S 2
	CALL SUBOPT_0x2
	CALL __MULF12
	CALL SUBOPT_0x6
	CALL SUBOPT_0x1
	CALL SUBOPT_0x7
	RJMP _0x18
_0x1A:
_0x17:
	LDD  R30,Y+8
	CPI  R30,0
	BRNE _0x1B
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(0)
	ST   X,R30
	RJMP _0xBB
_0x1B:
	CALL SUBOPT_0xB
	LDI  R30,LOW(46)
	ST   X,R30
_0x1C:
	LDD  R30,Y+8
	SUBI R30,LOW(1)
	STD  Y+8,R30
	SUBI R30,-LOW(1)
	BREQ _0x1E
	CALL SUBOPT_0x6
	CALL SUBOPT_0xE
	CALL SUBOPT_0x7
	__GETD1S 9
	CALL __CFD1
	MOV  R17,R30
	CALL SUBOPT_0xB
	CALL SUBOPT_0xD
	CALL SUBOPT_0x6
	CALL SUBOPT_0x2
	CALL SUBOPT_0x1
	CALL SUBOPT_0x7
	RJMP _0x1C
_0x1E:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	LDI  R30,LOW(0)
	ST   X,R30
_0xBB:
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,13
	RET
__ftoe_G3:
	SBIW R28,4
	CALL __SAVELOCR3
	__GETD1N 0x3F800000
	CALL SUBOPT_0xF
	LDD  R26,Y+10
	CPI  R26,LOW(0x6)
	BRLO _0x1F
	LDI  R30,LOW(5)
	STD  Y+10,R30
_0x1F:
	LDD  R16,Y+10
_0x20:
	MOV  R30,R16
	SUBI R16,1
	CPI  R30,0
	BREQ _0x22
	CALL SUBOPT_0x10
	CALL SUBOPT_0xF
	RJMP _0x20
_0x22:
	__GETD1S 11
	CALL __CPD10
	BRNE _0x23
	LDI  R18,LOW(0)
	CALL SUBOPT_0x10
	CALL SUBOPT_0xF
	RJMP _0x24
_0x23:
	LDD  R18,Y+10
	CALL SUBOPT_0x11
	BREQ PC+2
	BRCC PC+3
	JMP  _0x25
	CALL SUBOPT_0x10
	CALL SUBOPT_0xF
_0x26:
	CALL SUBOPT_0x11
	BRLO _0x28
	CALL SUBOPT_0x12
	CALL SUBOPT_0x13
	RJMP _0x26
_0x28:
	RJMP _0x29
_0x25:
_0x2A:
	CALL SUBOPT_0x11
	BRSH _0x2C
	CALL SUBOPT_0x12
	CALL SUBOPT_0xE
	CALL SUBOPT_0x14
	SUBI R18,LOW(1)
	RJMP _0x2A
_0x2C:
	CALL SUBOPT_0x10
	CALL SUBOPT_0xF
_0x29:
	__GETD1S 11
	__GETD2N 0x3F000000
	CALL __ADDF12
	CALL SUBOPT_0x14
	CALL SUBOPT_0x11
	BRLO _0x2D
	CALL SUBOPT_0x12
	CALL SUBOPT_0x13
_0x2D:
_0x24:
	LDI  R16,LOW(0)
_0x2E:
	LDD  R30,Y+10
	CP   R30,R16
	BRLO _0x30
	__GETD2S 3
	__GETD1N 0x41200000
	CALL SUBOPT_0xC
	CALL SUBOPT_0xF
	__GETD1S 3
	CALL SUBOPT_0x12
	CALL __DIVF21
	CALL __CFD1
	MOV  R17,R30
	CALL SUBOPT_0x15
	CALL SUBOPT_0xD
	CALL SUBOPT_0x2
	__GETD2S 3
	CALL __MULF12
	CALL SUBOPT_0x12
	CALL SUBOPT_0x1
	CALL SUBOPT_0x14
	MOV  R30,R16
	SUBI R16,-1
	CPI  R30,0
	BRNE _0x2E
	CALL SUBOPT_0x15
	LDI  R30,LOW(46)
	ST   X,R30
	RJMP _0x2E
_0x30:
	CALL SUBOPT_0x16
	LDD  R26,Y+9
	STD  Z+0,R26
	CPI  R18,0
	BRGE _0x32
	CALL SUBOPT_0x15
	LDI  R30,LOW(45)
	ST   X,R30
	NEG  R18
_0x32:
	CPI  R18,10
	BRLT _0x33
	CALL SUBOPT_0x16
	MOVW R22,R30
	MOV  R26,R18
	LDI  R30,LOW(10)
	CALL __DIVB21
	SUBI R30,-LOW(48)
	MOVW R26,R22
	ST   X,R30
_0x33:
	CALL SUBOPT_0x16
	MOVW R22,R30
	MOV  R26,R18
	LDI  R30,LOW(10)
	CALL __MODB21
	SUBI R30,-LOW(48)
	MOVW R26,R22
	ST   X,R30
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	LDI  R30,LOW(0)
	ST   X,R30
	CALL __LOADLOCR3
	ADIW R28,15
	RET
__print_G3:
	SBIW R28,28
	CALL __SAVELOCR6
	LDI  R16,0
_0x34:
	LDD  R30,Y+38
	LDD  R31,Y+38+1
	ADIW R30,1
	STD  Y+38,R30
	STD  Y+38+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+3
	JMP _0x36
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x3A
	CPI  R19,37
	BRNE _0x3B
	LDI  R16,LOW(1)
	RJMP _0x3C
_0x3B:
	CALL SUBOPT_0x17
_0x3C:
	RJMP _0x39
_0x3A:
	CPI  R30,LOW(0x1)
	BRNE _0x3D
	CPI  R19,37
	BRNE _0x3E
	CALL SUBOPT_0x17
	RJMP _0xBD
_0x3E:
	LDI  R16,LOW(2)
	LDI  R30,LOW(0)
	STD  Y+17,R30
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x3F
	LDI  R17,LOW(1)
	RJMP _0x39
_0x3F:
	CPI  R19,43
	BRNE _0x40
	LDI  R30,LOW(43)
	STD  Y+17,R30
	RJMP _0x39
_0x40:
	CPI  R19,32
	BRNE _0x41
	LDI  R30,LOW(32)
	STD  Y+17,R30
	RJMP _0x39
_0x41:
	RJMP _0x42
_0x3D:
	CPI  R30,LOW(0x2)
	BRNE _0x43
_0x42:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x44
	ORI  R17,LOW(128)
	RJMP _0x39
_0x44:
	RJMP _0x45
_0x43:
	CPI  R30,LOW(0x3)
	BRNE _0x46
_0x45:
	CPI  R19,48
	BRLO _0x48
	CPI  R19,58
	BRLO _0x49
_0x48:
	RJMP _0x47
_0x49:
	MOV  R26,R20
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R20,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x39
_0x47:
	LDI  R21,LOW(0)
	CPI  R19,46
	BRNE _0x4A
	LDI  R16,LOW(4)
	RJMP _0x39
_0x4A:
	RJMP _0x4B
_0x46:
	CPI  R30,LOW(0x4)
	BRNE _0x4D
	CPI  R19,48
	BRLO _0x4F
	CPI  R19,58
	BRLO _0x50
_0x4F:
	RJMP _0x4E
_0x50:
	ORI  R17,LOW(32)
	MOV  R26,R21
	LDI  R30,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	MOV  R21,R30
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R21,R30
	RJMP _0x39
_0x4E:
_0x4B:
	CPI  R19,108
	BRNE _0x51
	ORI  R17,LOW(2)
	LDI  R16,LOW(5)
	RJMP _0x39
_0x51:
	RJMP _0x52
_0x4D:
	CPI  R30,LOW(0x5)
	BREQ PC+3
	JMP _0x39
_0x52:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x57
	CALL SUBOPT_0x18
	LD   R30,X
	CALL SUBOPT_0x19
	RJMP _0x58
_0x57:
	CPI  R30,LOW(0x45)
	BREQ _0x5B
	CPI  R30,LOW(0x65)
	BRNE _0x5C
_0x5B:
	RJMP _0x5D
_0x5C:
	CPI  R30,LOW(0x66)
	BREQ PC+3
	JMP _0x5E
_0x5D:
	MOVW R30,R28
	ADIW R30,18
	STD  Y+10,R30
	STD  Y+10+1,R31
	CALL SUBOPT_0x18
	CALL __GETD1P
	CALL SUBOPT_0x1A
	MOVW R26,R30
	MOVW R24,R22
	CALL __CPD20
	BRLT _0x5F
	LDD  R26,Y+17
	CPI  R26,LOW(0x2B)
	BREQ _0x61
	RJMP _0x62
_0x5F:
	CALL SUBOPT_0x1B
	CALL __ANEGF1
	CALL SUBOPT_0x1A
	LDI  R30,LOW(45)
	STD  Y+17,R30
_0x61:
	SBRS R17,7
	RJMP _0x63
	LDD  R30,Y+17
	CALL SUBOPT_0x19
	RJMP _0x64
_0x63:
	CALL SUBOPT_0x1C
	LDD  R26,Y+17
	STD  Z+0,R26
_0x64:
_0x62:
	SBRS R17,5
	LDI  R21,LOW(5)
	CPI  R19,102
	BRNE _0x66
	CALL SUBOPT_0x1B
	CALL __PUTPARD1
	ST   -Y,R21
	LDD  R30,Y+15
	LDD  R31,Y+15+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __ftoa_G3
	RJMP _0x67
_0x66:
	CALL SUBOPT_0x1B
	CALL __PUTPARD1
	ST   -Y,R21
	ST   -Y,R19
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __ftoe_G3
_0x67:
	MOVW R30,R28
	ADIW R30,18
	CALL SUBOPT_0x1D
	RJMP _0x68
_0x5E:
	CPI  R30,LOW(0x73)
	BRNE _0x6A
	CALL SUBOPT_0x18
	CALL __GETW1P
	CALL SUBOPT_0x1D
	RJMP _0x6B
_0x6A:
	CPI  R30,LOW(0x70)
	BRNE _0x6D
	CALL SUBOPT_0x18
	CALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x6B:
	ANDI R17,LOW(127)
	CPI  R21,0
	BREQ _0x6F
	CP   R21,R16
	BRLO _0x70
_0x6F:
	RJMP _0x6E
_0x70:
	MOV  R16,R21
_0x6E:
_0x68:
	LDI  R21,LOW(0)
	LDI  R30,LOW(0)
	STD  Y+16,R30
	LDI  R18,LOW(0)
	RJMP _0x71
_0x6D:
	CPI  R30,LOW(0x64)
	BREQ _0x74
	CPI  R30,LOW(0x69)
	BRNE _0x75
_0x74:
	ORI  R17,LOW(4)
	RJMP _0x76
_0x75:
	CPI  R30,LOW(0x75)
	BRNE _0x77
_0x76:
	LDI  R30,LOW(10)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x78
	__GETD1N 0x3B9ACA00
	CALL SUBOPT_0x1E
	LDI  R16,LOW(10)
	RJMP _0x79
_0x78:
	__GETD1N 0x2710
	CALL SUBOPT_0x1E
	LDI  R16,LOW(5)
	RJMP _0x79
_0x77:
	CPI  R30,LOW(0x58)
	BRNE _0x7B
	ORI  R17,LOW(8)
	RJMP _0x7C
_0x7B:
	CPI  R30,LOW(0x78)
	BREQ PC+3
	JMP _0xBA
_0x7C:
	LDI  R30,LOW(16)
	STD  Y+16,R30
	SBRS R17,1
	RJMP _0x7E
	__GETD1N 0x10000000
	CALL SUBOPT_0x1E
	LDI  R16,LOW(8)
	RJMP _0x79
_0x7E:
	__GETD1N 0x1000
	CALL SUBOPT_0x1E
	LDI  R16,LOW(4)
_0x79:
	CPI  R21,0
	BREQ _0x7F
	ANDI R17,LOW(127)
	RJMP _0x80
_0x7F:
	LDI  R21,LOW(1)
_0x80:
	SBRS R17,1
	RJMP _0x81
	CALL SUBOPT_0x18
	CALL __GETD1P
	RJMP _0xBE
_0x81:
	SBRS R17,2
	RJMP _0x83
	CALL SUBOPT_0x18
	CALL __GETW1P
	CALL __CWD1
	RJMP _0xBE
_0x83:
	CALL SUBOPT_0x18
	CALL __GETW1P
	CLR  R22
	CLR  R23
_0xBE:
	__PUTD1S 6
	SBRS R17,2
	RJMP _0x85
	CALL SUBOPT_0x1F
	CALL __CPD20
	BRGE _0x86
	CALL SUBOPT_0x1B
	CALL __ANEGD1
	CALL SUBOPT_0x1A
	LDI  R30,LOW(45)
	STD  Y+17,R30
_0x86:
	LDD  R30,Y+17
	CPI  R30,0
	BREQ _0x87
	SUBI R16,-LOW(1)
	SUBI R21,-LOW(1)
	RJMP _0x88
_0x87:
	ANDI R17,LOW(251)
_0x88:
_0x85:
	MOV  R18,R21
_0x71:
	SBRC R17,0
	RJMP _0x89
_0x8A:
	CP   R16,R20
	BRSH _0x8D
	CP   R18,R20
	BRLO _0x8E
_0x8D:
	RJMP _0x8C
_0x8E:
	SBRS R17,7
	RJMP _0x8F
	SBRS R17,2
	RJMP _0x90
	ANDI R17,LOW(251)
	LDD  R19,Y+17
	SUBI R16,LOW(1)
	RJMP _0x91
_0x90:
	LDI  R19,LOW(48)
_0x91:
	RJMP _0x92
_0x8F:
	LDI  R19,LOW(32)
_0x92:
	CALL SUBOPT_0x17
	SUBI R20,LOW(1)
	RJMP _0x8A
_0x8C:
_0x89:
_0x93:
	CP   R16,R21
	BRSH _0x95
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0x96
	ANDI R17,LOW(251)
	LDD  R30,Y+17
	CALL SUBOPT_0x19
	CPI  R20,0
	BREQ _0x97
	SUBI R20,LOW(1)
_0x97:
	SUBI R16,LOW(1)
	SUBI R21,LOW(1)
_0x96:
	LDI  R30,LOW(48)
	CALL SUBOPT_0x19
	CPI  R20,0
	BREQ _0x98
	SUBI R20,LOW(1)
_0x98:
	SUBI R21,LOW(1)
	RJMP _0x93
_0x95:
	MOV  R18,R16
	LDD  R30,Y+16
	CPI  R30,0
	BRNE _0x99
_0x9A:
	CPI  R18,0
	BREQ _0x9C
	SBRS R17,3
	RJMP _0x9D
	CALL SUBOPT_0x1C
	LPM  R30,Z
	RJMP _0xBF
_0x9D:
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	LD   R30,X+
	STD  Y+10,R26
	STD  Y+10+1,R27
_0xBF:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	CALL __put_G3
	CPI  R20,0
	BREQ _0x9F
	SUBI R20,LOW(1)
_0x9F:
	SUBI R18,LOW(1)
	RJMP _0x9A
_0x9C:
	RJMP _0xA0
_0x99:
_0xA2:
	CALL SUBOPT_0x20
	CALL __DIVD21U
	MOV  R19,R30
	CPI  R19,10
	BRLO _0xA4
	SBRS R17,3
	RJMP _0xA5
	SUBI R19,-LOW(55)
	RJMP _0xA6
_0xA5:
	SUBI R19,-LOW(87)
_0xA6:
	RJMP _0xA7
_0xA4:
	SUBI R19,-LOW(48)
_0xA7:
	SBRC R17,4
	RJMP _0xA9
	CPI  R19,49
	BRSH _0xAB
	__GETD2S 12
	__CPD2N 0x1
	BRNE _0xAA
_0xAB:
	RJMP _0xAD
_0xAA:
	CP   R21,R18
	BRLO _0xAE
	RJMP _0xC0
_0xAE:
	CP   R20,R18
	BRLO _0xB0
	SBRS R17,0
	RJMP _0xB1
_0xB0:
	RJMP _0xAF
_0xB1:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0xB2
_0xC0:
	LDI  R19,LOW(48)
_0xAD:
	ORI  R17,LOW(16)
	SBRS R17,2
	RJMP _0xB3
	ANDI R17,LOW(251)
	LDD  R30,Y+17
	CALL SUBOPT_0x19
	CPI  R20,0
	BREQ _0xB4
	SUBI R20,LOW(1)
_0xB4:
_0xB3:
_0xB2:
_0xA9:
	CALL SUBOPT_0x17
	CPI  R20,0
	BREQ _0xB5
	SUBI R20,LOW(1)
_0xB5:
_0xAF:
	SUBI R18,LOW(1)
	CALL SUBOPT_0x20
	CALL __MODD21U
	CALL SUBOPT_0x1A
	LDD  R30,Y+16
	__GETD2S 12
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __DIVD21U
	CALL SUBOPT_0x1E
	__GETD1S 12
	CALL __CPD10
	BREQ _0xA3
	RJMP _0xA2
_0xA3:
_0xA0:
	SBRS R17,0
	RJMP _0xB6
_0xB7:
	CPI  R20,0
	BREQ _0xB9
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	CALL SUBOPT_0x19
	RJMP _0xB7
_0xB9:
_0xB6:
_0xBA:
_0x58:
_0xBD:
	LDI  R16,LOW(0)
_0x39:
	RJMP _0x34
_0x36:
	CALL __LOADLOCR6
	ADIW R28,40
	RET
_sprintf:
	PUSH R15
	MOV  R15,R24
	SBIW R28,2
	ST   -Y,R17
	ST   -Y,R16
	MOVW R26,R28
	CALL __ADDW2R15
	MOVW R16,R26
	MOVW R26,R28
	ADIW R26,6
	CALL __ADDW2R15
	CALL __GETW1P
	STD  Y+2,R30
	STD  Y+2+1,R31
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
	CALL __print_G3
	LDD  R26,Y+2
	LDD  R27,Y+2+1
	LDI  R30,LOW(0)
	ST   X,R30
	LDD  R17,Y+1
	LDD  R16,Y+0
	ADIW R28,4
	POP  R15
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:9 WORDS
SUBOPT_0x0:
	LDS  R30,_d
	LDS  R31,_d+1
	LDS  R22,_d+2
	LDS  R23,_d+3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x1:
	CALL __SWAPD12
	CALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0x2:
	CLR  R31
	CLR  R22
	CLR  R23
	CALL __CDF1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x3:
	CALL __SWAPW12
	SUB  R30,R26
	SBC  R31,R27
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x4:
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x5:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:7 WORDS
SUBOPT_0x6:
	__GETD2S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x7:
	__PUTD1S 9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	__PUTD1S 2
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x9:
	__GETD1S 2
	RJMP SUBOPT_0x6

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0xA:
	__GETD2S 2
	__GETD1N 0x41200000
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:9 WORDS
SUBOPT_0xB:
	LDD  R26,Y+6
	LDD  R27,Y+6+1
	ADIW R26,1
	STD  Y+6,R26
	STD  Y+6+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:7 WORDS
SUBOPT_0xC:
	CALL __DIVF21
	__GETD2N 0x3F000000
	CALL __ADDF12
	CALL __PUTPARD1
	JMP  _floor

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0xD:
	MOV  R30,R17
	SUBI R30,-LOW(48)
	ST   X,R30
	MOV  R30,R17
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:17 WORDS
SUBOPT_0xE:
	__GETD1N 0x41200000
	CALL __MULF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:7 WORDS
SUBOPT_0xF:
	__PUTD1S 3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:9 WORDS
SUBOPT_0x10:
	__GETD2S 3
	RJMP SUBOPT_0xE

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:21 WORDS
SUBOPT_0x11:
	__GETD1S 3
	__GETD2S 11
	CALL __CMPF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x12:
	__GETD2S 11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:6 WORDS
SUBOPT_0x13:
	__GETD1N 0x41200000
	CALL __DIVF21
	__PUTD1S 11
	SUBI R18,-LOW(1)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x14:
	__PUTD1S 11
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x15:
	LDD  R26,Y+7
	LDD  R27,Y+7+1
	ADIW R26,1
	STD  Y+7,R26
	STD  Y+7+1,R27
	SBIW R26,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x16:
	LDD  R30,Y+7
	LDD  R31,Y+7+1
	ADIW R30,1
	STD  Y+7,R30
	STD  Y+7+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:12 WORDS
SUBOPT_0x17:
	ST   -Y,R19
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G3

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES, CODE SIZE REDUCTION:21 WORDS
SUBOPT_0x18:
	LDD  R26,Y+36
	LDD  R27,Y+36+1
	SBIW R26,4
	STD  Y+36,R26
	STD  Y+36+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:22 WORDS
SUBOPT_0x19:
	ST   -Y,R30
	LDD  R30,Y+35
	LDD  R31,Y+35+1
	ST   -Y,R31
	ST   -Y,R30
	JMP  __put_G3

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x1A:
	__PUTD1S 6
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x1B:
	__GETD1S 6
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x1C:
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	ADIW R30,1
	STD  Y+10,R30
	STD  Y+10+1,R31
	SBIW R30,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x1D:
	STD  Y+10,R30
	STD  Y+10+1,R31
	ST   -Y,R31
	ST   -Y,R30
	CALL _strlen
	MOV  R16,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x1E:
	__PUTD1S 12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x1F:
	__GETD2S 6
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x20:
	__GETD1S 12
	RJMP SUBOPT_0x1F

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

__ftrunc:
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
	rcall __ftrunc
	brne __floor1
__floor0:
	ret
__floor1:
	brtc __floor0
	ldi  r25,0xbf

__addfc:
	clr  r26
	clr  r27
	ldi  r24,0x80
	rjmp __addf12

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

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

__LSLW2:
	LSL  R30
	ROL  R31
	LSL  R30
	ROL  R31
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

__DIVB21U:
	CLR  R0
	LDI  R25,8
__DIVB21U1:
	LSL  R26
	ROL  R0
	SUB  R0,R30
	BRCC __DIVB21U2
	ADD  R0,R30
	RJMP __DIVB21U3
__DIVB21U2:
	SBR  R26,1
__DIVB21U3:
	DEC  R25
	BRNE __DIVB21U1
	MOV  R30,R26
	MOV  R26,R0
	RET

__DIVB21:
	RCALL __CHKSIGNB
	RCALL __DIVB21U
	BRTC __DIVB211
	NEG  R30
__DIVB211:
	RET

__DIVD21U:
	PUSH R19
	PUSH R20
	PUSH R21
	CLR  R0
	CLR  R1
	CLR  R19
	CLR  R20
	LDI  R21,32
__DIVD21U1:
	LSL  R26
	ROL  R27
	ROL  R24
	ROL  R25
	ROL  R0
	ROL  R1
	ROL  R19
	ROL  R20
	SUB  R0,R30
	SBC  R1,R31
	SBC  R19,R22
	SBC  R20,R23
	BRCC __DIVD21U2
	ADD  R0,R30
	ADC  R1,R31
	ADC  R19,R22
	ADC  R20,R23
	RJMP __DIVD21U3
__DIVD21U2:
	SBR  R26,1
__DIVD21U3:
	DEC  R21
	BRNE __DIVD21U1
	MOVW R30,R26
	MOVW R22,R24
	MOVW R26,R0
	MOV  R24,R19
	MOV  R25,R20
	POP  R21
	POP  R20
	POP  R19
	RET

__MODB21:
	CLT
	SBRS R26,7
	RJMP __MODB211
	NEG  R26
	SET
__MODB211:
	SBRC R30,7
	NEG  R30
	RCALL __DIVB21U
	MOV  R30,R26
	BRTC __MODB212
	NEG  R30
__MODB212:
	RET

__MODD21U:
	RCALL __DIVD21U
	MOVW R30,R26
	MOVW R22,R24
	RET

__CHKSIGNB:
	CLT
	SBRS R30,7
	RJMP __CHKSB1
	NEG  R30
	SET
__CHKSB1:
	SBRS R26,7
	RJMP __CHKSB2
	NEG  R26
	BLD  R0,0
	INC  R0
	BST  R0,0
__CHKSB2:
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

__GETD1PF:
	LPM  R0,Z+
	LPM  R1,Z+
	LPM  R22,Z+
	LPM  R23,Z
	MOVW R30,R0
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

__SWAPACC:
	MOV  R21,R30
	MOV  R30,R26
	MOV  R26,R21
	MOV  R21,R31
	MOV  R31,R27
	MOV  R27,R21
	MOV  R21,R22
	MOV  R22,R24
	MOV  R24,R21
	MOV  R21,R23
	MOV  R23,R25
	MOV  R25,R21
	MOV  R21,R0
	MOV  R0,R1
	MOV  R1,R21
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

__CPD20:
	SBIW R26,0
	SBCI R24,0
	SBCI R25,0
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
