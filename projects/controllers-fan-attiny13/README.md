# Controllers for fan on attiny13

Uploaded by Trolll on 2013-02-15 14:47:00 (rating 0 out of 5)

## Summary

This device allows to reduce the noise from cooling fan. It turn off the fan or reduces the rotation speed depending on the temperature of the radiator. Also the device beeps when temperature exceeds a threshold. There are two variants of device for fans of different power:


- the first variant is designed to control 12-volt computer fan, and has been fully implemented on the SMD-elements:


- the second variant is different only to using a Power MOSFET and voltage regulator, as well as the microcontroller in DIP package.


As the temperature sensor used chip STLM20W87F -analog temperature sensor, value of the voltage at its output is proportional to the temperature (the voltage decreases almost linearly with increasing temperature).


 


-----


Project page: [http://trolsoft.ru/en/sch/cooler...](http://trolsoft.ru/en/sch/coolercontrollers)


 


Here you can download Eagle project and firmware with sources.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- ATtiny13A ATtiny13
