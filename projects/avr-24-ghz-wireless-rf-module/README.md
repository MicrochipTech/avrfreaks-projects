# AVR 2.4 GHz  wireless RF module

Uploaded by Decibit on 2008-05-21 17:54:00 (rating 0 out of 5)

## Summary

It has 8 pins, just like ATtiny85. Yet it's not an IC, but comes in a module form. It's built on ATmega168 and nRF24L01: Presenting to you "DCBT-24AX".


Other 2.4 GHz projects here base on old TRW-2.4G module (it is a has been) or require you to depend on big size HW or external SW libraries.


With this tiny module, a mere 2.5 cm^2 of PCB gives you a total solution to run your code in it. Only needed is a DC power source 2.7V to 5.25V.


TRANSPORT LAYER in OSI model is implemented, thus 100% link integrity is provided. Other solutions provide LINK LAYER only which is not sufficient to work with wireless links. It shall not be your job to think of these wireless general problems, rather concentrate on your application.


If your application needs no more than 6 AVR I/O pins, then you're done. Your entire code can run in the module. Else you can interface this module via AVR HW IIC to your external MCU.


More data to be found at <http://www.decibit.com>

## Links

- [Windows Installer for example user codes](http://www.decibit.com/SetupDDK.exe)
- [Datasheet](http://www.decibit.com/dcbt-24.pdf)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
