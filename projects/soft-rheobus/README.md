# Soft-Rheobus

Uploaded by _tourist on 2010-05-17 11:19:00 (rating 0 out of 5)

## Summary

Hardware devices is simplified so the minimum is, of course there are a handful of details without which "and so come down, but this is the security, reliability, and EMC, which better not be neglected.


The main part - Microcontroller ATtiny 2313 clocked from the internal oscillator of 8 MHz (this decision was made only to simplify the scheme, in general when working with UART is better to use a crystal).  

The microcontroller has 4 hardware PWM channel utilized to control FETs, as well as the hardware UART allows to communicate with the PC.  

As the power switches used transistors company IR - IRF7105, P-channel transistors can break plus chain, while the signal is not distorted tahodatchika (of course if enough voltage to work tahodatchika). IRF7105 in a single package containing two N-channel transistor and P-channel. Since the P-channel transistors can not be controlled directly with a microcontroller - N-channel transistor is the driver for the P-channel. Learn more about managing stress test with field-effect transistors is well written here.  

Each of the four PWM channels is equipped with LC filter and a resettable fuse that protects the transistors from short circuit.  

For communication with the PC chip used converter MAX-232. In order to equip reobas USB interface is enough to replace MAX-232 FT-232 (in perspective).  

This solution allowed to refuse such elements of regulation and control as buttons, displays, etc. On the other hand to retain full functionality in terms of management channels - enough to write the software from the computer - the driver. Such decisions are somehow not very accustomed with the major manufacturers.  

A small portion of hinged elements ensures stable operation of the microcontroller, but more on that can be read here.


Originally a reobas stitched boot (optional but very convenient), I used this one: AVR Bootloader FastBoot von Peter Dannegger First he poddrezhivaet bunch of microcontrollers (including ATtiny2313), and has autodetect speed UART.  

In the future, changing firmware reobasa via COM-port with the aid of application FBOOT.EXE available on the website loader.


Read more: soft-rheobus.blogspot.com

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- General Lib.functions
- IAR Embedded Workbench for AVR
