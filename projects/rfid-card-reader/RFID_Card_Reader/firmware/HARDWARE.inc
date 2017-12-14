;Include all CPU hardware dependancies regarding the ACORN kernel 
;When the kernel is used for onather AVR CPU make sure timers are properly defined here

;Timer0
;Provides the timing for task scheduling

;Executed during kernel init - no task context defined yet
.MACRO _INIT_TASKSHEDUAL_TIMER
	ldi temp,(1<<CS01)  ;preskaler 1/8 ;clock on every 0.5ms     
	out TCCR0,temp
.ENDMACRO

;Executed in the LIMBO state or during CPU init - no context defined
;Use any register
.MACRO _ENABLE_TASKSHEDUAL_TIMER
     clr temp
     out TCNT0,temp  
     in temp,TIMSK
	 sbr temp,(1<<TOIE0)
     out TIMSK,temp	 
.ENDMACRO

;Executed in the LIMBO state or CPU init - no context defined
;Use any register
.MACRO _DISABLE_TASKSHEDUAL_TIMER
	 in temp,TIMSK
	 cbr temp,(1<<TOIE0)
     out TIMSK,temp
.ENDMACRO

;Fills up the timer in order to provoke time overflow interrupt and activate scheduler.
;Context is defined so use a register as a parameter. 
;INPUT: @0 - register to use
.MACRO _SET_TASKSHEDUAL_TIMER
	ldi @0,0xFF
	out TCNT0,@0 
.ENDMACRO


;**********************SOFTWARE INTERRUPT**********************************
/*
Implementing software interrupt is tricky - the only way to be used for this is 
the external interrupt INT0 or INT1 which unfortunatly is bound to a platform dependant port pin.
If enabled, the interrupts will trigger even if the INT0 or INT1 pins are configured as
outputs.
I will use INT0 interrupt which has the highest priority.
THIS IS	ATMEGA8 implementation!Change for your MCU 
*/
;Executed once during CPU or Task init
;INPUT:	none
;USAGE:	temp
.MACRO  __SWI_INIT
;****INT0******
	in temp,MCUCR
    sbr temp,(1<<ISC01)+(1<<ISC00)       ;intr on rising edge INT0
    out MCUCR,temp
  
    in temp,GIMSK
    sbr temp,1<<INT0
    out GIMSK,temp
.ENDMACRO

;USER API
;INPUT:	none
;USAGE:	none
.MACRO	__SWI_INVOKE
	sbi DDRD,PD2 
    sbi PORTD,PD2     
sw_wait:
    sbic PORTD,PD2	
	rjmp sw_wait
.ENDMACRO

;KERNEL API - used to reset the SW in INT0 or INT1 interrupt.
;INPUT:	none
;USAGE:	none
.MACRO __SWI_END
	cbi PORTD,PD2
.ENDMACRO
