/************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.1                                                         MAIN PROGRAM
-------------------------------------------------------------------------------------------------------------------------
Written for   : Atmel "AVR Assembler" and "AVR Assembler 2"
        when  : 2006 April 20
        where : BRUSSELS - BELGIUM - EUROPE
        who   : Jan Huygh
        E-mail: jan.huygh@skynet.be
*************************************************************************************************************************/

.Include "tn13def.inc"
.Include "IO_Port_And_IO_Pin_Configuration.inc"
.Include "Assign_Variables_To_Registers.inc"
.Include "Macro_Definition_Initialize.inc"
.Include "Macro_Definition_GeneratePWMSignal.inc"
.Include "Macro_Definition_NewPulseCalculations.inc"
.Include "Macro_Definition_FailSaveRoutine.inc"
.Include "Macro_Definition_LookupTable.Inc"

.ORG    0

Initialize

ldi PWMStep,1			              						//PWMStep = 6
//With the main loop taking 12 CPU clock cycles we can calculate Fpwm as a function of PWMStep @9.6 MHz CPU clock.
//Fpwm=(9.6*PWMStep*1000000)/(256*12) So: 1=>3125Hz 2=>6250Hz 3=>9375Hz 4=>12500Hz 5=>15750Hz 6=>18750Hz 7=>21875Hz

FailSave:
	FailSaveRoutine

MainLoop:                             						//MainLoop:
	GeneratePWMSignal                     					//Generate PWMSignal takes always exactly 6 cycles
	sbic    PinB,RCPulseInputPin          					//If (RCPulseInputPin=0)then
	rjmp    ML_Else1										//
	    adiw RCPulseCounterH:RCPulseCounterL,1				//	RCPulseCounter=RCPulseCounter+1
	    rjmp MainLoop										//
	ML_Else1:                             					//Else
	    cpi	 RCPulseCounterH,0            					//    If (RCPulseCounter>0) then
	    breq MainLoop										//
	        NewPulseCalculations          					//        NewPulseCalculations takes always exactly 17 cycles
	                                      					//    EndIf
	                                      					//EndIf
	rjmp MainLoop						  					//GoTo MainLoop

/*The main loop always takes 12 CPU clock cycles to execute except just after an RC pulse has been reveived.
Just after an RC pulse has been received the main loop takes 13 cycles + cycles needed for NewPulseCalculations.
This has no effect on the measurement accuracy of the incoming RC pulse. This has a minor effect on the stability of the
generated PWM signal. Every 20ms the PWM signal will have one cycle that is 18 CPU cycles longer.
With PWMStep = 6 we will have every 372 PWM cycles 1 cycle that takes 1.8 µs or 3.4% longer.
With PWMStep = 1 we will have everey 62 PWM cycles 1 cycle that takes 1.8 µs or 0.5% longer.                           */

Data:
	LookupTable
