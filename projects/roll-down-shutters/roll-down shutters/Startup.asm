;CodeVisionAVR C Compiler
;(C) 1998-2002 Pavel Haiduc, HP InfoTech s.r.l.

;EXAMPLE STARTUP FILE

	.EQU __CLEAR_START=0X60 ;START ADDRESS OF SRAM AREA TO CLEAR
				;SET THIS ADDRESS TO 0X100 FOR THE
				;ATmega128 OR ATmega64 CHIPS
	.EQU __CLEAR_SIZE=256   ;SIZE OF SRAM AREA TO CLEAR IN BYTES

	CLI		;DISABLE INTERRUPTS
	CLR  R30
	OUT  EECR,R30	;DISABLE EEPROM ACCESS

;DISABLE THE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	LDI  R31,0x10
	OUT  WDTCR,R31

	OUT  MCUCR,R30	;MCUCR=0, NO EXTERNAL SRAM ACCESS

;CLEAR R2-R14
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(__CLEAR_SIZE)
	LDI  R25,HIGH(__CLEAR_SIZE)
	LDI  R26,LOW(__CLEAR_START)
	LDI  R27,HIGH(__CLEAR_START)
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM
	MOV  R1,R0
	ADIW R30,1
	LPM
	ADIW R30,1
	MOV  R22,R30
	MOV  R23,R31
	MOV  R31,R0
	MOV  R30,R1
	SBIW R30,0
	BREQ __GLOBAL_INI_END
	LPM
	MOV  R26,R0
	ADIW R30,1
	LPM
	MOV  R27,R0
	ADIW R30,1
	LPM
	MOV  R24,R0
	ADIW R30,1
	LPM
	MOV  R25,R0
	ADIW R30,1
__GLOBAL_INI_LOOP:
	LPM
	ST   X+,R0
	ADIW R30,1
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOV  R30,R22
	MOV  R31,R23
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:


