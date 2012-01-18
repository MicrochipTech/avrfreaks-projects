# Digital RPM Meter for Automobles

Uploaded by AMKhan on 2012-01-17 18:02:00 (rating 0 out of 5)

## Summary

Small Cars do not have a RPM Meter on its dashboard. Having one on-board may be helpful to the driver.


In this project we have build an RPM meter using the AVR Project Book. Later it can be build using any small micro controller to a size which can be added to the Car Dash Board.


The first thing to make an RPM reading is to decide about the sensing point (from where a reliable RPM signal should be taken). One option is to use the pickup coil on HV wire and other is to tap the distributor wire. Many CNG kits use a pickup coil (a wire wrapped around the current carrying wire) on the HV wire that goes to the plug to sense the Engine RPM. The wire not only picks the High Voltage spark signal but also picks signal from other sources like other HV Lines not being sensed, generator etc. This crude method works well if the accuracy is not important.


After Experimenting a lot with the pickup coil on HV line, it became evident that it can not be used as the RPM sensing method, as the noise on this line is able to plague any kind of signal conditioning circuit. The advantage of using an HV wire is that it is available in all kind of cars. So, I decided to use the distributor wire (the wire that is connected on the side of distributor and internally connected to the [point](http://en.wikipedia.org/wiki/Contact_breaker)) as the sensing point. 


You can see on the website of RPM meter, how clean and clear the signal is even after a simple RC Filter. The problem with the sensing through distributor is that any short circuit to ground or any other point will disturb the High Voltage generation and the Engine will stop.


The output of the LM324 (op-amp) is fed to the PB1 of the Micro controller, as depicted in the picture below. The PB1 has an alternate function of *Timer/Counter1 External Counter Input*, through which this pin can be used as the clocking source of the Timer1. Using the external clock, the frequency measurement can be made.


Another Timer (Timer 0) is used to measure the elapsed time. After every 500ms the counter1 value is obtained. The Counter1 value is filtered using a moving average Low pass filter. The Counter Register is re-initialized to start a new sampling.


The filtered value is multiplied by 40 (a combined effect of multiplying by 120 and dividing by 3) and displayed on the 7 segment LEDs. 120 is the multiplication factor for a frequency measured in cycles/500ms to be converted to cycles/minute, 3 is the number of cylinders in the car.

## Links

- [Pictures and Code](http://www.digisoft.com.pk/Projects/rpm-meter-for-automobiles)
- [Signal Conditioning circuit for RPM Meter](http://www.digisoft.com.pk/Projects/rpm-meter-for-automobiles/image003.gif)
- [Link to AVR project Book Page](http://www.digisoft.com.pk/products/avr-project-book)
- [AVR Project Book Link](https://www.avrfreaks.net/index.php?module=Freaks%20Tools&func=viewItem&item_id=953)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
