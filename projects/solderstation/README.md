# SolderStation

Uploaded by greg0ry on 2014-08-07 18:06:00 (rating 0 out of 5)

## Summary

Temperature controlled two part soldering station. LCD Display of actual and set temperature plus air flow setting for hot air pistol and soldering iron. PID controlled temperature with Type K Thermocouples and MAX6675 a/d. TRIAC controlled power delivery to heat elements (240V 50Hz 60W air pistol, 24Vac 60W, soldering pencil) 240V 50Hz diaphragm air pump for air flow. KS0108 128x64 mono LCD, 20 pin interface. Arduino Mega2560 R2 'breakout' of Atmega2560 at 16MHz. TRIAC phase control outputs on atmega hardware timers 3 and 4, with mains zerocross input on timer5 input capture pin. Arduino PID (predictive, integral, diferential) control (Arduino Library) and PID Autotune library designed for PWM ouput but mapped for phase control of triacs (all but the same).

## Compilers

- AVR Studio 5

## Tags

- Complete code with HW
- AVR Studio 5
