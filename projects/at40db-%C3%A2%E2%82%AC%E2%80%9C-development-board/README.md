# AT40DB â€“ Development board

Uploaded by ecalogiuri on 2013-10-23 10:18:00 (rating 0 out of 5)

## Summary

Hi guys,  

here is my idea to make a development board for 40-pin AVR microcontrollers. This board can work with two different voltages, 5VDC and 3.3VDC, has a 10-pin port ICSP, you can choose the clock source (crystall or oscillator), has a reset button of the microcontroller, and provides 4 LEDs and 4 buttons activated by jumpers. The four ports of the microcontroller are made ??available by 10 pin ports, including the eight digital I/O plus power and ground.


The buttons work at a high logic level, ie when the button is pressed on the corresponding pin of the microcontroller you can read an high level Vcc.


The leds are connect on portb, from Portb0 to Portb3.The buttons are connect on porta, from Porta0 to Porta3.  

The pin number 1 on ICSP port is VCC.  

Pin 9 on I/O port is Vcc.  

Pin 10 on I/O port is Gnd.


The pcb layout is optimized for dual face copper board, but simple to reproduce by toner transfert metod.


More info on: [http://electroprojectshb.co.nf/?...](http://electroprojectshb.co.nf/?p=169)

## Tags

- Complete code with HW
