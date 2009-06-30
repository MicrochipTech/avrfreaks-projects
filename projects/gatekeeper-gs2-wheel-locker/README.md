# Gatekeeper GS2 wheel locker.

Uploaded by Headtrip on 2009-06-29 20:00:00 (rating 0 out of 5)

## Summary

 The company Gatekeeper Systems (<http://www.gatekeepersystems.com/>) has a system that employs "smart wheels" that lock when they cross over an electromagnetic loop around the parking lot. This is mostly my hello-world project as it is the first thing I have done with an AVR uC. My code is probably not the most efficient way of doing things, but it works. The circuit also is not the best way to do this but again, it works. 


 The code used is an 8khz signal pulsed on and off with roughly 4ms spaces between the on pulses, and roughly 360ms between bursts. The timing for the on pulses is 20ms,8ms.4ms.4ms,4ms,8ms,8ms,8ms,4ms,16ms.


 The circuit is a simple 1 transistor amplifier to energize the coil, these things are EM not RF, so the range is limited to the size of the magnetic field. Then again a 8khz a transmitter wouldn't really practical anyway, as even a 1/4 wave antenna would be nearly 30000 feet long O\_O 


 For the circuit, I used a general purpose NPN transistor, with the Base wired to the PWM output from the AVR Butterfly, then the Emitter wired to GND with the Collector feeding on side of the coil through a 220ohm resistor, with the other side of the coil tied to +5v from the battery. The coil itself is simply a few hundred winds of magnet wire around a metal core (Not very precise, but it's my first AVR project)


For some reason I just get a blank white page when I try to upload files here... so the code is pasted below:


/*  

 ## This code generates the signal to lock the wheels on Gatekeeper GS2  

 ## shopping cart wheels.  

*/


#include   

#define F\_CPU 1000000l // Tell delay.h what speed the CPU is running at  

#include   

#define SPACE 0x03 // Space between pulses  

#define WORD\_SPACE 360 // Space between code repeats


int main(void) {


 char lock\_code[10] = "\x14\x08\x04\x04\x04\x08\x08\x08\x04\x10"; // The code word


 DDRB=0xFF; // Port-B as output  

 PORTB=0xFF; // Turn on all bits of port-B


 ICR1=126; // Set the frequency (ICR1 is setup as TOP) 8khz


 TCCR1A=0x82; // Set up 16 bit timer for fast PWM  

 TCCR1B=0x19; // with no prescaling using ICR1 as counter TOP


 OCR1A+=63; // Set the output compare for 50% duty cycle.


 /*  

 ## Toggle the timer on and off to create pulses and spaces  

 ## using delay\_ms control pulse timing. I couldn't quite get the  

 ## SPACE to sync up with the real signal so the \_delay\_loop\_2(210)  

 ## was added and the SPACE def was reduced by one.  

 */


 for (;;) {  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[0]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[1]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[2]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[3]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[4]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[5]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[6]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[7]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[8]);  

 TCCR1B=0x18;  

 \_delay\_ms(SPACE);  

 \_delay\_loop\_2(210);  

 TCCR1B=0x19;  

 \_delay\_ms((long)lock\_code[9]);  

 TCCR1B=0x18;  

 \_delay\_loop\_2(210);  

 TCCR1B=0x18;  

 \_delay\_ms(WORD\_SPACE);  

 }


}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
