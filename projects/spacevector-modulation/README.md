# Spacevector Modulation

Uploaded by admin on 2002-05-20 18:01:00 (rating 0 out of 5)

## Summary

This Project uses the Timers of the 8535 to generate 3 PWM Signals on the Output Compare Pins PD.4, PD.5, and PD.7. These Signals can be used to Drive an Inverter to generate a 3-Phase Sinusoidal Voltages for Motor Control. The Amplitude is controled with PA0 and the Frequency with PA1. The direction is controlled by the PB.7 Bit.  

Switching Frequency: 15.858 or 2000 Hz, Amplitude 0-86.6%, Frequency: 0-280 Hz (res: 1,3 Hz)


Iits been tested on a real system and works.  

The svm.prj is an Project file for VMLAB simulation. With VMlab you can simulate an RC Filter and watch the 3-Phase System in the Virtual Ozi. (Works only with avrbeta13 or higher)  

Additional Hardware you need to test this on a real System: DeadtimeInsertionIC, Gatedrivers, MOSFETs, Motor  

Applications: Syncron-, Asyncron Motor Control, USVs


For additional Information see:  

Infineon Application Note: AP0836, "Space Vector Modulation and Overmodulation with an 8-bit MC"  

Analog Devices Application Note "Implementing Space Vector Modulation with the ADMCF32X"


Contact:  

Email: [svm@tseronis.net](mailto:svm@tseronis.net)

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
