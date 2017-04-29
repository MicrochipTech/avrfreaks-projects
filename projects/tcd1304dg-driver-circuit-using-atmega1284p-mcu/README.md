# TCD1304DG driver circuit using an ATMega1284P MCU

Uploaded by dhaffnersr on 2017-04-29 13:54:00 (rating 0 out of 5)

## Summary

This is a project that I have been working on for almost a year on Hackaday.com, it is the CCD detector portion of the 3D printable Raman spectrometer project of mine and I finally got someone far smarter than me to help me out on the AVR part of all this and I guess I must have impressed him by transcribing his schematics on LTspice so he allowed me to use his design (with his invaluble programming help I might add,) and so I put the board together and it should be ready for offical testing sometime next week.


I am including the firmware ( C coding) for the ATMega1284P MCU, which is part of the development board I am using from MCUdude;


![ATMega1284P Dev Board](http://cdn.hackaday.io/images/5209491493472367640.jpg)


This is the CCD driver circuit board which houses the TCD1304DG chip and the ATMega1284P MCU;


![](http://cdn.hackaday.io/images/5086821493471107024.png)


Finished view;


![](http://cdn.hackaday.io/images/2136641493471151788.png)


I'm still kind of new to C programming and AVR and I have recently downloaded AtmelStudio 7.0 and I am really glad I did, so I am re-engineering the firmware to remove some "glichyness," from the timing sequences, and this platform is perfect![smiley](https://community.atmel.com/profiles/commons/libraries/ckeditor/plugins/smiley/images/regular_smile.png "smiley")


Here is how my Dev board is currently set up with the ATMega1284P chip;


![](http://cdn.hackaday.io/images/4453881493488478647.png)


![](http://cdn.hackaday.io/images/5181671493488530472.png)


![](http://cdn.hackaday.io/images/9057821493488580664.png)


My entire project can be accessed at the link below


*Update* 5/5/2017


I got the idea for the heat sink for the TCD1304dg chip from @esben rossel he recently had a post where he showed how he cut a piece of aluminum block to act as a heat sink for the TCD1304 chip. That got me thinking, and so I formed my own aluminum heat sink out of aluminum industrial tape and will post the procedure also in my build files.


Also I found the perfect heat sinks for the AD7667 (ADC) chip, these little 15 x 15mm aluminum heat sinks are used in a kit for the Xbox 360, for the RAM chips inside, it cools them between 10-12 deg F. Perrrrrrfect :)


Here is the magic below;


![](http://cdn.hackaday.io/images/5017821494011445717.png)


![](http://cdn.hackaday.io/images/4187691494011468724.png)


![](http://cdn.hackaday.io/images/8668231494011512145.png)


![](http://cdn.hackaday.io/images/6751161494011533073.png)


![](http://cdn.hackaday.io/images/4420071494011575970.png)


![](http://cdn.hackaday.io/images/7834791494011601722.png)


![](http://cdn.hackaday.io/images/446481494011628676.png)

## Links

- [3D printable Raman Spectrometer](http://hackaday.io/project/18126-3d-printable-raman-spectrometer)
- [Open Source Framework](http://osf.io/x9gm6/files/)

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- #Atmega #avr #atmega128 #atmega2560
- #Atmega avr
- AVRDragon
- #Atmega128
- WEBCAM
- 555-TIMER-PULSE-GENERATOR-CIRCUIT TIMER
- ATMEGA-1284
- ATMEGA1284P
- Industrial Automation
- MESH
- OSF
- FREECAD
- THORLABS
- Arduino
- STL
- OPEN-SCIENCE-FRAMEWORK
- RAMAN-SPECTROMETER
- CCD-DETECTOR-GOOGLE-SEARCH
- Cortex-M3
- SPECTROPHOTOMETER
- complete-code
- DHAFFNERSR
- CCD
- ARM
- RAMAN
- DAVID-H-HAFFNER
- CCD-DETECTOR
- Atmel
- SCIENCE
- DAVID-H-HAFFNER-SR
- CCD-CIRCUIT-DRIVER
- Google
- COOL-SCIENCE
- 3D-PRINTING
- DAVID
- GOOGLE-EARTH
- 3D-PRINTER
- H
- HOW-TO
- HOMEMADE-SPECTROMETER
- HAFFNER
- circuit
- HOW-TO-BUILD
- PARTS
- SR
- PCB
- HOW-TO-BUILD-A-SPECTROMETER
- 3D-PARTS
- 2017HACKADAYPRIZE
- Detector
- ARDUINO-DUE
- 3D-PART
- XR2206
- toshiba
- MEGA-2560
- SPECTROMETER
- FUNCTION-GENERATOR
- camera
- AD7667
- CMOS
- PULSE-GENERATOR
- driver
- ARM-32-BIT
- CMOS-CAMERA
- 555-TIMER
- ARDUINO-MEGA
