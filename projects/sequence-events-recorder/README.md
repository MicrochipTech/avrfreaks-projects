# Sequence Of Events Recorder

Uploaded by kashyaphs on 2008-01-02 18:26:00 (rating 0 out of 5)

## Summary

ATmega32 is heart of the system. It acquires 16 Digital Inputs(0-24VDC) and 4 Analog inputs (4-20mA). An event is defined whenever any DI changes or a threshold of Analog value is crossed. That is stored as an event in the Internal RAM with time stamp (upto milliseconds)and is transmitted to the Serial Port when demanded by the Other end of the Communication link.  

All chanels can be enabled disabled through parametr file and time can be adjusted. Thresholds of analog channels can also be modified.  

The system is currently used in Mini Hydo Electric Power Plants to Monitor the reasons for Generator Tripping.  

I will be adding latest version files for this project. OrCAD designs and C files everything. It might appear incomplete in starting but i ll put all info after observing how it appears to someone eles on this website.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
