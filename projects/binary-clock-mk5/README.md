# Binary clock mk5

Uploaded by stiant04 on 2007-02-18 19:47:00 (rating 0 out of 5)

## Summary

Binary clock code made for the stk500/atmega128.  

displays the time on the led`s (portB) in binary coded desimal form (BCD) . Displays seconds unless any of the buttons (pinA)  

is held down. Button 0 makes it display minutes and button 1 makes it display hours.  

The clock uses timer1 for every second interrupt and updating of the time.  

It also features an UART echo to be used with the stk500s two com ports and an aditional terminal program. Any characters typed will be written back to the terminal . The characer "t" will make it display the time (hh:mm:ss)on the terminal .  

It also has a 8 byte FIFO buffer where the characters are stored when waiting to be written .

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
