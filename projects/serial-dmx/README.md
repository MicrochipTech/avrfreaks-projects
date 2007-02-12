# Serial to DMX

Uploaded by chayagb on 2007-02-11 19:19:00 (rating 0 out of 5)

## Summary

Here are the details (I haven't done a schematic as it was a  

'back of a napkin' job! - pretty simple, just connect a MAX232  

to the UART (PD0 and PD1) and a MAX485 to PB0. It also requires  

a 4.00MHz Xtal or ceramic resonator for the timing to be correct.  

Baud rate is 19200, 8 data bits, No Parity, 1 stop bit.


It uses the UART on an AVR (tested and working with a 90S2313), to  

receive 3 bytes of data from the PC or whatever is talking to  

the UART. The protocol goes like this:


Send to AVR: 0xFF (Start of command)  

Send to AVR: 0x01 to 0x6C (DMX Channel Number - Up to 108)  

Send to AVR: 0x00 to 0xFF (DMX Channel Value)  

On Success, the AVR will send back: 0x00  

On failure (e.g. Re-sync required) AVR will send back 0xFF)


Have incloded Sample VB proggy (requires NetComm OCX to be able to talk to the com ports)


Please please please come back and rate this project if it's useful!

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
