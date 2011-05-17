Readme.txt

The hookup is explained in the *.asm file, but here it is again:

PINB.0 --------- DI to pin 12 on RFM12B (in) (MCU pin 1)
PINB.1 --------- DO to pin 1 on FRM12B (out) (MCU pin 2)
PINB.2 --------- SCK to pin 13 on RFM12B (out) (MCU pin 3)
PINB.3 --------- SS to pin 14 on RFM12B (out) (MCU pin 4)
PINB.4 --------- RED led (out) (MCU pin 7) with series resistor
PINB,5 --------- GREEN led (out) (MCU pin 8) with series resistor

Then, there should be an additional 10k resistor from the FSK pin (3) on the RFM12B
module to Vcc.

The SS (Slave Select) line may be optional on a SPI interface, I found that you MUST use it 
in my example, because of the somewhat odd behaviour of the RFM module.

What I have done is basically the following. None of the interrupt lines are used, instead I 
use the SDO line (on the RFM12B module) as an indicator of a ‘ready’ state. Apparently this 
seems to work. There is, however one caveat. After the module has been configured as a 
receiver and has received the pre-programmed number of data bytes, the receiver part of the 
module is switched off (normal procedure). If the module then is configured as a receiver 
AGAIN, the SDO line will incorrectly indicate a ‘ready’ state for a few (100 or so) microseconds, 
in my case resulting in receiving data that was no data at all. A small delay of 125 usec. 
took care of this problem (line 405 in the source code). So, in the end the setup worked.

Jan van Rijsewijk


