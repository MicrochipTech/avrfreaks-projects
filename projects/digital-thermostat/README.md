# Digital Thermostat

Uploaded by spyrosbitoulis on 2005-02-15 07:21:00 (rating 0 out of 5)

## Summary

Digital Thermostat  

using AT90S2313 with high accuracy  

(accuracy up to 0.05°C)  

Digital Room Thermostat - Range -6.00 / 46.00°C  

Microcontroller Avr 8-bit RISC (AT90S2313)  

Author: Bitoulis Spyridon  

Email: [sbitouli@sidenor.vionet.gr](mailto:sbitouli@sidenor.vionet.gr) 


The Digital Room Thermostat was designed mainly for house application. The thermostat controls the room temperature. The heart of the system is the ATMEL's AT90S2313 8-bit microprocessor. The digital thermostat opens the main heating valve of the apartment and the hot water flows in the radiators.


The microcontroller increments a counter every 2 seconds, using the Watchdog timer. Once the thermostat is RESETED the microcontroller performs all the pending jobs, like communication, temperature measurement, manual operations, relay control. When these jobs are completed it goes to power down mode using the sleep command (0.040mA in sleep mode).


Using the icNE555 and a NTC Thermistor 10K (4300), a temperature measurement is been made every 1min. A 4Κ54 Resistor is connected in series with the NTC to reduce the current during the temperature measurement.


The serial interface link (RS232) is used for Downloading and Uploading Parameters from a PC. Every 2seconds an attempt is been made to communicate with the PC. Only a READ or WRITE command can be performed during this period.


All informations are available in <http://geocities.com/sbitouli/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
