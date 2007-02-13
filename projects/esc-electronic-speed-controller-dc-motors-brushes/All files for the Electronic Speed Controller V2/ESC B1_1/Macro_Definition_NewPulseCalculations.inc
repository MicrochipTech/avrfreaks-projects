/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.1                                Macro Definition: NewPulseCalculations
**************************************************************************************************************************

Calculating the values after a new RC-Pulse has been received takes 19 cycles. @ 9,6 MHz executing 17 cycles takes about
1,98 µs. Since a new pulse is coming in every 20 ms the program is executing perfect PWM-loops for over 99,99% of the time
So the disturbance of the time spend on NewPulseCalculations makes the program inperfect but has no practical effect on
the correct operation. If you really want always perfect PWM signals then you could insert GeneratePWMSignal in the
NewPulseCalculations so that GeneratePWMSignal always starts after 12 CPU clock cycles.

The loop that counts the incoming RCPulse takes exactualy 12 cycles to execute. When we receive an incoming RC Pulse of
1.5 ms (=neutral position of the stearig stick) we will count in RCPulseCounter to (1,5*10^-3)/(12 * (1/9,6*10^6))=1200.
A 1.1ms pulse will make RCPulseCounter=880, a 1.9ms pulse will make RCPulseCounter=1520                                 */

.MACRO NewPulseCalculations;19 cycles

wdr															//Reset Watchdog timer
lsr	 RCPulseCounterH				  						//RCPulseCounter=RCPulseCounter/2
ror  RCPulseCounterL										//
subi RCPulseCounterL,low(600)		  						//RCPulsecounter=RCPulseCounter-600
sbci RCPulseCounterH,high(600)								//									*now RCPulseCounterH is 0*
brbs SREG_N,Else1							 				//If (RCPulseCounter>0) then
    mov  OnValue,ForwardOnValue		  						//   OnValue=ForwardOnValue
    rjmp EndIf1												//
Else1:								  						//Else	
    mov  OnValue,BackwardOnValue	  						//   OnValue=BackwardOnValue
    com  RCPulseCounterL			 						//   RCPulseCounter=-RCPulseCounter
EndIf1:								  						//EndIf
ldi Temp, 220						  						//If RCPulseCounter>=220 GoTo Failsave (perfect value is 160)
CP 	RCPulseCounterL,Temp									//
brbc SREG_C,Failsave										//
ldi	ZH,	high(Data<<1)			      						//PWMOnTime=RCPulseCounter*255/160
ldi ZL, low (Data<<1)			     						//(lookup table)
clr RCPulseCounterH
add	ZL, RCPulsecounterL										//
adc	ZH, RCPulseCounterH										//
lpm	PWMOntime, Z											//
clr RCPulseCounterL  				  						//RCPulseCounter=0

.ENDMACRO
