/*************************************************************************************************************************
Electronic Speed Controller Program Version Beta 1.0                         IO-Port and IO-Pin configuration for your ESC
*************************************************************************************************************************/

//You can addapt the program to the pin connections that you use in your ESC by adapting the equ statements here below.

.equ	ESCPORT         = PortB	  //What is the IO Port you use				                       ATtiny13
.equ    ESCPORT_DDR     = DDRB	  //Set Data Direction register for the IO-port                    --------
.equ    RCPulseInputPin =4        //To what PBX-pin is the RC signal connected          BHI <-  [1]PB5  VCC[8] <- +5V
.equ    ALIOutputPin    =1        //To what PBX-pin is ALI connected             Not Connected  [2]PB3  PB2[7] -> BLI
.equ    AHIOutputpin    =0        //To what PBX-pin is AHI connected               RC Pulse ->  [3]PB4  PB1[6] -> ALI  
.equ    BLIOutputpin    =2        //To what PBX-pin is BLI connected                    GND ->  [4]GND  PB0[5] -> AHI
.equ    BHIOutputpin    =5        //To what PBX pin is BHI connected                               --------
