/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.0                                   Macro Definition: GeneratePWMSignal
*************************************************************************************************************************/

//With PWMOnTime = 0 GeneratePWMSignal will NEVER put power on the motor. Perfect !

//When PWMProgressCounter = 255 and PWMOnTime = 255 this routine will output the OffValue.
//With PWMStep = n that will happen every n-PWM-cycles when PWMOnTime = 255. That means that this program will never use
//100% of the available motor power. The power we do use can be calculated from P% = (1-(1/(256*PWMStep)))*100. In the
//worst case senario with PMWStep = 1 we will use 99,6% of the available motor power. With PWMStep = 6 we will use 99,99%.

.MACRO GeneratePWMSignal									//GeneratePWMSignal always takes exactly 6 cycles to excute

add  PWMProgressCounter,PWMStep       						//PWMProgressCounter = PWMProgressCounter + PWMStep
cp   PWMProgressCounter,PWMOnTime     						//If PWMProgressCounter => PWMOnTime Then
brlo OutOnValue						  						//	
	out  PortB,OffValue				  						//	Out PortB, OffValue
	rjmp ExitGeneratePWMSignal		  						//
OutOnValue:							  						//ElseIf PWMProgressCounter < PWMOnTime
    nop								  						//	NOP
    out  PortB,OnValue				  						//	Out PortB, OnValue
ExitGeneratePWMSignal:				  						//EndIf

.ENDMACRO
