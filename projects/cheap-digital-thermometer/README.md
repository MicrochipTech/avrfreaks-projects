# Cheap Digital Thermometer

Uploaded by ashuinfy on 2009-09-13 13:08:00 (rating 0 out of 5)

## Summary

Thermometers are used for measuring temperatures, we have seen a lots of digital watches which has inbuilt thermometer too. So we think that why not we made that one. So here we introducing a cheap Digital Thermometer Using LM35 Temperature Sensor and ATMEGA8 Microcontroller with three 7 Segment Displays.  

Input signals in microcontrollers may be digital or analog. Digital signal may have two values â€œ0â€ and â€œ1â€ while analog any value in given interval. while AVR microcontroller may operate with only digital signals, analog signals have to be converted to digital. Mega series of AVR have built in ADC inside chip what makes this task much easier. before building this you have to know the ADC peripheral details of the AVR ATMEGA8 so refer Datasheet of it.  

The Accuracy is somewhat 1'C less than the original temperature because to make the reading stable we do the average of twelve variables for a stable display of the temperature. So there is a trick to troubleshoot it. Just add 1 to the result ' adc\_temp ' you will get  

accurate result. See below the previous and new expression both in 


[http://robolabz.multiply.com/jou...](http://robolabz.multiply.com/journal/item/45/Build_your_own_Digital_ThermometerATMEGA8)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
