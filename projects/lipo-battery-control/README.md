# LiPo Battery Control

Uploaded by jsvens64 on 2011-04-29 10:12:00 (rating 0 out of 5)

## Summary

The LiPo-battery is very sensitive to low voltage. If cell voltage go below a certain value the battery will break.


In my solution to this problem I've used a ATtiny24A to measure the cell voltages and comparing them to a defined value. When this value is reached the battery shuts off.  

In order not to have an exteranal battery, the LiPo battery also feed the control circuitry from the secondary side of the relay. To be sure that the ADC circuitry don't keep on pulling out power from the battery the dividers are disconnected with optocouplers.  

The "ON" button is normaly open and the off button is normaly closed.


The project is compiled with the new AVR studio 5 (AVR-GCC).


Maybe the code isn't optimized but it works...

## Tags

- Complete code with HW
