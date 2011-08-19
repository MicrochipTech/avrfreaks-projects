# PWM Led aquarium light control

Uploaded by deearhunter on 2011-08-19 10:50:00 (rating 0 out of 5)

## Summary

Dawn and dusk periods is essential for breeding some egglaying aquarium fish species. (Also good for easily scared species)  

This project uses an olimex avr-io-20pin board with an Maxxtronic 15A PWM motor controller board. Relay 1 is removed on the olimex board and the LM324 op from the PWM board.  

Both boards are connected to a 12V supply and a wire is connected between  

the relay1 transistor's collector and pin 1 on the 14 pin socket.


LED strips 12V 0.7W are connected to the M+ and M- terminals on the PWM controller. The strips are specified as non dimmable but are just leds and resistors no active components.  

Ordinary fluo tube lights are switched on/off with relay 4 on the olimex board.


This design supplements the ordinary aquarium light with dusk/dawn capability.


An AT90S2313 with 10 bit pwm at full speed is used as processor.  

The pwm is calibrated to get an almost linearly perceived light ramp.  

To get a smooth response a running average filter modifies the pwm data before it is sent to timer 1.


[http://www.mediafire.com/file/i0...](http://www.mediafire.com/file/i06la3xjf78iu69/pwm.zip)  

only get a white screen when trying to associate a file

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
