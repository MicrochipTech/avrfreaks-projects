/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.0                                         Assign Variables to Registers
*************************************************************************************************************************/
    
.def    Temp  = R16                   						// working register

.def    ForwardOnValue = R17          						//The OnValue  when running forward
.def    BackwardOnValue = R18         						//The OnValue  when running backward
.def    OffValue = R19                						//The OffValue same for forward and backward)
.def    OnValue = R20                 						//The OnValue: ForwardOnValue  when running forward
															//			   BackwardOnValue when running backward
.def    PWMProgressCounter = R21
.def    PWMOnTime = R22
.def    PWMStep = R23
.def    RCPulseCounterL = R26         						//Low  byte of the RCPulseCounter
.def    RCPulseCounterH = R27         						//High byte of the RCPulseCounter
