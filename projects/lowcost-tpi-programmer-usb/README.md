# lowcost TPI programmer - USB

Uploaded by PRosendahl on 2010-10-01 18:39:00 (rating 0 out of 5)

## Summary

This is a lowcost driverless generic USB bitbanger customized for TPI programming for the new ATTiny4/5/9/10.


A) ATTiny25 V-USB firmware for the bitbanger / flasher / programmer  

B) .NET Application to drive the USB flasher  

C) sample ATTiny10 program using PWM and interrupts  

D) schematics (Eagle and PNG)  

E) sample ATTiny10 program using PWM outputs in complementary modes (inverting / non-inverting)  

F) schematics for 12V high voltage programming (necessary if RSTDISBL fuse is programmed)  

G) I2C bus support  

H) screenshots


Discussion is here:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=675770#675770)


2010-10-01:  

- .NET application: fixed writing of fuses (will erase chip to reset fuses if necessary)


2010-04-30:  

- .NET application: added support for I2C bus  

- .NET application: added support for Microchip 24FC256 (I2C flash memory device)  

- .NET application: lowlevel bit banger supports async read operations  

- .NET application: bug fixes  

- .NET application: added screenshots


2010-03-28:  

- schematics for 12V high voltage RESET option (Eagle and PNG)


2010-03-25:  

- changed description from "flasher" to "programmer" so it can be google'd better


2010-03-20:  

- updated .NET application 1.5 has better diagnostic/error messages


2010-03-19:  

- updated .NET application 1.3:  

- "fast flash" procedure (7 secs for entire chip)  

- faster verification procedure (4 secs)  

- more error messages  

- updated .NET application 1.4 defaults to "reset line is active low" (which is consistent with the published schematics)


2010-03-16:  

- updated .NET application 1.1 to read signature and program fuses  

- added second PWM example (inverting / non-inverting PWM modes)  

- updated .NET application 1.2

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
