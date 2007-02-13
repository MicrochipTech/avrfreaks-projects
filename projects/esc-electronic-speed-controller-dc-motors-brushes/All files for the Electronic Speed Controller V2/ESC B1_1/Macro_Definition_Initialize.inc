/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.1                                           Macro Definition Initialize
*************************************************************************************************************************/

.MACRO Initialize

cli                                   						//Clear Global Interrupt

wdr															//Reset the Watchdog timer
ldi temp, 0b00011000										//Set Watchdog Change Enable and Watchdog System Reset Enable
out WDTCR,temp												//
ldi temp, 0b00001011										//Confirm Watchdog System Reset Enable and 0.125ms time out
out WDTCR,temp												//

set															//Set T-bit for use in the coming instruction
//Calculate ForwardOnValue
ldi ForwardOnValue,0x00               						//Set all bits in ForwardOnValue low
bld ForwardOnValue,AHIOutputPin       						//Set AHIOutputPin-bit high
bld ForwardOnValue,BLIOutputPin       						//Set BLIOutputPin-bit high
//Calculate BackwardOnValue
ldi BackwardOnValue,0x00              						//Set all bits in BackwardOnValue low
bld BackwardOnValue,ALIOutputPin      						//Set ALIOutputPin-bit high
bld BackwardOnValue,BHIOutputpin      						//Set BHIOutputPin-bit high
//Calculate OffValue
ldi OffValue,0x00                     						//Set all bits in OffValue low
bld OffValue,AHIOutputPin             						//Set AHIOutputPin-bit high
bld OffValue,BHIOutputPin             						//Set BHIOutputPin-bit high
    
ldi	Temp,0xFF                         						//Set all I/O pins of port B as output 
out ESCPORT_DDR,Temp                
cbi ESCPORT_DDR,RCPulseInputPin        						//except RCPulseInputPin

ldi Temp,0x40												//Disable Pull Ups
out MCUCR,Temp

.ENDMACRO
