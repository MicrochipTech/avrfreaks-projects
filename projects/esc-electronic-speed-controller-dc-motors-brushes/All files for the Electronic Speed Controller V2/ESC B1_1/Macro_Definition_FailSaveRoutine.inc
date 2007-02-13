/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.1                                                              FailSave
*************************************************************************************************************************/

.MACRO FailSaveRoutine

Out Portb, OffValue                   						//Turn Motor Off
FailSaveReset:												//FailSaveReset:
clr Temp                             						//Temp = 0
clr RCPulseCounterH                   						//RCPulseCounter = 0
clr RCPulseCounterL											//
FailSaveLoop:                         						//FailSaveLoop:
nop nop nop nop nop nop 									//nop nop nop nop nop nop
sbic	PinB,RCPulseInputPin             					//If (RCPulseInputPin=0) Then
rjmp	ElseA												//		RCPulseCounter=RCPulseCounter+1
      adiw RCPulseCounterH:RCPulseCounterL,1				//  	GoTo FailSaveLoop
	  rjmp FailSaveLoop                 					//EndIf
ElseA:
adiw RCPulseCounterH:RCPulseCounterL,0						//If (RCPulseCounter=0) GoTo FailSaveLoop
brbs SREG_Z,FailSaveLoop		    						//
//A good RC pulse is between 1 and 2 ms long. In counts in this program that translates to :
//A good RC Pulse means : 800 <= RCPulseCounter <= 1600
//After devision by 16  :  50 <= RCPulseCounter <=  100
//To avoid shocking motors at maximum position we will allow 0,8 to 2,2 ms pulses. That translates to :
//A good RC pulse means : 640 <= RCPulseCounter <= 1760
//After devision by 16  :  40 <= RCPulseCounter <=  110	
wdr															//Reset Watchdog timer  	
swap RCPulseCounterH              							//RCPulseCounter=RCPulseCounter/16
andi RCPulseCounterH,$F0
swap RCPulseCounterL
andi RCPulsecounterL,$0F
or   RCPulseCounterL,RCPulseCounterH
cpi  RCPulseCounterL, 40	         						//IF (RCPulseCounter<50)   GoTo FailSaveReset
brlo FailSaveReset
cpi  RCPulseCounterL, 110          							//IF (RCPulseCounter>=100) GoTo FailSaveReset
brsh FailSaveReset
clr  RCPulseCounterH				 						//RCPulseCounter=0
clr  RCPulseCounterL
Inc  Temp                          							//Temp=Temp+1
cpi  Temp,10                       							//If (Temp=10) GoTo MainLoop
breq MainLoop
rjmp FailSaveLoop											//Goto FailSaveLoop

.ENDMACRO
