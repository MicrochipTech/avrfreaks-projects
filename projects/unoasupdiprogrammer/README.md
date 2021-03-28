# UNO_as_UPDI_programmer

Uploaded by osbornema on 2021-03-27 17:42:00 (rating 0 out of 5)

## Summary

This project enables the UNO to be used to program devices via the UPDI interface. It is based on the UNO AVR programmer that I posted some months ago, however the MOSI,  

MISO hardware interface has been replace by a UPDI interface.  The project code consists of two main sections: the first downloads and parses a hex file putting addresses and commands  

into a buffer and the second reads the buffer and programs the commands to flash.  


 


The code is set up for the ATtiny1606, but can easily be adjusted for the 806.  Clearly the Atmega328 is a very popular device with newcomers, it is after all used in the UNO.  I suggest that the ATtiny1606  

is also a great device especially for anyone wishing increase their exposure to the world of electronics. For simplicity it has hardware UART and I2C modules and is available with a SOIC package providing an excellent introduction   

to the many advantages of surface mount.  


 


Students may find that the ATtiny1606 provides the ideal incentive to master pcb design software (Eagle for example) and give Atmel Studio 7 a go.


 


Personally I would not want to be without either Arduino or Studio 7.  They are two great software tools with overlapping but different applications and both easy to get used to. 


I very much hope that this posting will enable some newcomers to sample the ATtiny1606 quickly and easily using tools that they are already familiar with.

## Compilers

- Arduino IDE

## Tags

- Complete code with HW
- Arduino IDE
