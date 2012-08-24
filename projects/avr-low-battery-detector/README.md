# AVR Low Battery Detector

Uploaded by ksvitale on 2012-08-24 05:50:00 (rating 0 out of 5)

## Summary

This project was spurred into existence as a support board for the AVR Data Recorder project.


On that project, with no available I/O pins and parts count increasing, I did not have a low-battery detector for the on-board Li-Ion battery.


After investigating various low-power methods using "micro-power" comparators, I found a design based on a MICROCHIP part. The power drain was on the order of 135ua until the LED was blinked for 25mS every 10 seconds. However, the parts count was on the order of 15+. Too many to stuff onto a "small" vector-board.


It occurred to me that the AVR Tiny13, which I had a tube of left over from a previous project, would be an ideal candidate since it had an ADC, Analog Comp, PWM, on-chip oscillator and was ISP programmable. 


As it turned out, this part is new enough that it has the AIN(-) input tied to the ADC's MUX, which freed up the OC0B pin for use as a PWM DAC to feed the AIN(-) input of the analog comp. The PWM DAC is used as the programmable reference to set the low-battery detect threshold.


Since the goal was not to discharge an already low battery by consuming too much current while informing the user with an LED, I choose to use the analog comp. instead of the ADC to further reduce power consumption. I also don't need to waste CPU cycles waiting for and averaging conversions. With the PWM running at a low frequency, the power consumption is lower as well.


The total parts count ended up to be eight (8) including the ISP header.


The AVR is designed to run from a regulated supply, 3 volts in my case. The 2:1 voltage divider allows measuring up to 5.9 volts, certainly capable of measuring the 4.2 volts from a Li-Ion cell. 


After building up the circuit on a vector-board and checking connections, including my voltage divider's accuracy, I set out to write some code.


I decided to use the AVR in "System-Reset" watchdog mode set to 8 seconds timeout. The program flow is:  

1 RESET  

2 load preset ref value  

3 set I/O ports  

4 set PWM  

5 wait for PWM to settle  

6 check analog comp  

7 if low-batt  

7a set LED on  

7b set WDT for IRQ @ 64ms  

7c sleep till WDT IRQ  

7d set LED off  

8 reset I/O ports  

9 sleep in "power-down" till WD reset


In order to further conserve power, the wait periods also use the WDT in IRQ mode while the AVR sleeps in "idle mode".


When the software detects a low battery-condition, it increases the PWM reference voltage by one (1) bit (11.7mv @3 volt supply) and saves the new reference value for next measurement. With each low-battery detection, it increments the reference voltage for a maximum of three (3) times. This has the effect of adding hysteresis. If at any time a not-low-battery is detected, the PWM reference is reloaded from the preset value, which is stored in EEPROM.


The 2-wire on-chip DEBUG-WIRE interface was a big help in verifying correct code execution.


I originally tested with the 128KHz oscillator but saw a fairly slow response. Using the 4.8Mhz internal RC oscillator only added about 2 ua to the overall average power consumption.


Power consumption measurements were as such:  

1 AVR in powerdown = 4 ua  

2 AVR executing code for 64ms (setup and measure) = 160 ua  

3 AVR + LED "on" for 64ms = 1300 ua


Average power consumption = 125 ua with LED on, 42 ua with the LED off. It beat the micro-power comparator in half as many parts!


Schematics and source code included.


Be sure to remove the ".png" extension from the ZIP file.


Have fun!


Peace,  

Scott

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
