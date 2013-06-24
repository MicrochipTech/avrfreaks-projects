

; int vectors for ATTINY261/461/861


	.org	0x0000
	rjmp RESET ; Reset Handler
	.org	0x0001 
	rjmp EXT_INT0 ; IRQ0 Handler (INT from RFM12)
	.org	0x0002 
;	rjmp PCINT ; PCINT Handler
	.org	0x0003 
;	rjmp TIM1_COMPA ; Timer1 CompareA Handler
	.org	0x0004 
;	rjmp TIM1_COMPB ; Timer1 CompareB Handler
	.org	0x0005 
;	rjmp TIM1_OVF ; Timer1 Overflow Handler
	.org	0x0006 
;	rjmp TIM0_OVF ; Timer0 Overflow Handler
	.org	0x0007 
;	rjmp USI_START ; USI Start Handler
	.org	0x0008 
;	rjmp USI_OVF ; USI Overflow Handler
	.org	0x0009 
;	rjmp EE_RDY ; EEPROM Ready Handler
	.org	0x000A 
;	rjmp ANA_COMP ; Analog Comparator Handler
	.org	0x000B 
;	rjmp ADC ; ADC Conversion Handler
	.org	0x000C 
;	rjmp WDT ; WDT Interrupt Handler
	.org	0x000D 
;	rjmp EXT_INT1 ; IRQ1 Handler
	.org	0x000E 
;	rjmp TIM0_COMPA ; Timer0 CompareA Handler
	.org	0x000F 
;	rjmp TIM0_COMPB ; Timer0 CompareB Handler
	.org	0x0010 
;	rjmp TIM0_CAPT ; Timer0 Capture Event Handler
	.org	0x0011 
;	rjmp TIM1_COMPD ; Timer1 CompareD Handler
	.org	0x0012 
;	rjmp FAULT_PROTECTION ; Timer1 Fault Protection