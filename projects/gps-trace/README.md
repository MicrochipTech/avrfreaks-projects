# GPS-TRACE

Uploaded by wenoi6 on 2007-11-28 21:18:00 (rating 0 out of 5)

## Summary

<<This project is download 809 times now. and only three people give me a thanks of feedback :S ..talking about absorbing information. be polite people!>>


GPS-TRACE


The following is included:  

- Sources (C for AVR)  

- Electronic designs (Scheme/PCB) (PDF-format)  

- Functional designs (visio format)  

- Documentation (doxygen)  

(currently dutch only, still working on english).


Please any feedback about what you think of it would be great. If you know any improvements (more efficient codes or something like that), let me know!


This project is about GPS tracing. it works as follows:  

- GPS traces the position of the device via satelite  

- Somebody (who wants to know the position) calls the number of the  

GSM-Module  

- The Microcontroller noticed the call and ask the position which is  

stored in a global var  

- The Microcontroller sends a command to the GSM-Module to send a SMS  

with position information back to the caller.


GSM-Module------->Microcontroller<-----------GPS


GSM-Module  

Siemens TC35i


Microcontroller  

Atmega162


GPS  

Garmin GPS17HVS


GPS data-type  

NMEA


Used Interface  

The GSM-Module & GPS is connected with RS232 to the microcontroller.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
