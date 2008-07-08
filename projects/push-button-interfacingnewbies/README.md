# Push Button Interfacing(Newbies)

Uploaded by nilay0045 on 2008-07-08 04:52:00 (rating 0 out of 5)

## Summary

Microcontroller Used:  

ATMEGA32


Connections:  

LED----> PORTB.0  

Pushbutton----> Between PORTB.4 and GND  

RESET----> +5V through 10k resistor 


Software Debouncing Description:  

As push buttons are mechanical switches, they do not change their state immediately. They normally take about 20-30ms to attain a stabilized state. In software debouncing, simply we have to check if the button is depressed, wait for 20-30ms. After that, check the state of the button again. If it is still pressed, execute the corresponding code.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
