# Jar of Fireflies

Uploaded by Keso on 2007-01-10 19:48:00 (rating 0 out of 5)

## Summary

This project simulates firefly behavior in a common household jar using green LEDs and an ATTiny45 microcontroller.


My girlfriend and I built a number of these and gave them out as holiday presents in 2006 and we're still getting thank-you's for them. So far the favorite place people have found to put them has been in the bedroom so you can watch them as you fall asleep.


The software impliments two channels of digitized software PWM playback of pre-encoded firefly flash patterns (flash patterns were digitized from research data found online). Timer0 is used in CTC mode to drive the PWM while the watchdog timer is used in interrupt mode to govern the time between playbacks and to enable us to go into a deep sleep state between playbacks. The code has been heavily optimized to minimize power consumption.


The software PWM is done using an inline assembler macro which impliments a PWM strategy based on Bresenham's line drawing algorithm. With all the variables permanently bound in registers, we're able to do the entire PWM calculation in 9 cycles for each channel. In retrospect and with the hardware design I settled on, I could have used the hardware PWM functions but my original design idea would have prevented this.


Illustrated instructions and source files available [here](http://www.instructables.com/id/E7U5HYMSVIEWP86SAL).


This project was [featured on Make Magazine's blog on Jan 10th, 2007](http://www.makezine.com/blog/archive/2007/01/jar_of_fireflie.html).

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
