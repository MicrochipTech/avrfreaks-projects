# Direct read cap sensor no ADC

Uploaded by joeata2wh on 2010-01-01 01:54:00 (rating 0 out of 5)

## Summary

Read inexpensive Capacitive Humidity sensors such as HCH-1000-002 and the HS1101 directly from the Micro controller without ADC. It uses 1 resistor, 1 diode and 2 digital IO lines and the sensor. 1 of the two IO lines can be multiplexed across multiple sensors so 7 sensors can be supported on 1 port. Tested on Mega16 but should work on most 4K CPU for single sensor version. 


It works without an ADC by measuring the amount of time it requires to charge the capacitor through a resistor until the charge level is sufficient to register as logic 1. As the humidity changes the capacitance changes which changes the time required to charge the circuit. 


We measure the time in a tight processor loop and average the readings to reduce hysteresis. The readings are calibrated to known humidity points using aqueous salt solutions. The calibration allows simple conversion to percentages which can then be used internally or shipped out through a digital network such as RS422. Most humidity sensors change consistently at a given capacitance change per 1% humidity change so the humidity can be calculated as a direct correlation to capacitance changes.


 Very low power per sensor.  

 very stable reads.  

 Not real high performance about 1 read per second after averaging.  

 Easy calibration allows 7 different sensors on a micro controller each with a different offset and slope. Very high repeatability and supports up to 5 calibration points. 


While this circuit was designed specifically to support humidity sensors it should work the same with any circuit which uses a variable capacitor that predictably changes across itâ€™s set of input such as stress gauges. It can be extended with look up tables to support devices which do not deliver a linear ramp across itâ€™s operating range.


My Motivation: I ran out of ADC pins and didn't want to install a multiplexer or external ADC for my 3 humidity sensors. I was tired of paying $15 to $35 each for humidity sensors like the vout HM1500 or the digital Sensirel SH75. I needed an adequate solution that was cheaper and could be easily field re-calibrated. 


After a very primitive calibration run, I ran a quick check using a sodium chloride (table salt) calibration solution my readings after averaging ran +- 1.5% of the 75.3% expected which is within the hysteresis ratings of the HCH-1000-002 sensor. 


Basic Circuit Operation  

It basically uses a two digital IO lines one as a charge and the other as a sense. We charge the capacitor circuit through a resistor with a diode for fast drain back. We feed the sense line from the cap back to the micro controller. The capacitor is fully drained before the feed line is activated. We measure the time it requires for sense to convert from a logic low to logic high in a tight loop with a counter. Once the transition high on sense occurs the feed line is set to low which re-drains the sensor cap. As the capacitor value rises or falls with humidity the count value rises and falls proportionately. We measure the count in a tight loop because we found the variables in interrupt service for the timer caused undesirable variability in the readings.  

the resistor.


Note: The individual reads are very fast about 1,300 machine cycles in a tight loop. They also have a high degree of noise. I manage the noise by oversampling and averaging the results. The current version works out to about 140 reads and we need a pause between each read to allow the cap to drain. This works out to about 1 second per read. If you can tolerate more variability in your readings then it can easily be reduced to under 0.01 second. 


See: [http://correctenergysolutions.co...](http://correctenergysolutions.com/electronics/cap-humidity-sensor-circuit) for most current code and schematic. Free to use but no warranty. No successful software is truly ever done but this version works and I will continue posting more features.


When I tried to upload the source the freaks site gave an error and refused to save. Looked like a website failure so for now just download the source from my site.

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
