# uIP-0.9 With ENC18J60 Driver

Uploaded by webbi_arg on 2008-04-14 08:30:00 (rating 0 out of 5)

## Summary

I did it because I only found the uip-0.6 version with support for ENC28J60, I tried this several times and appear to be working fine, but consider this as BETA until some expert take a look and make any correction.


All of the code was done by the people mentioned on the VERSION.h file, I just add the ENC28J60 support to this.


It was compiled in GNU avr-gcc, makefile included.


IMPORTANT NOTE:  

I have been problems with the uip\_connect(). I can't make a simple telnet client, I cant found why it doesnt work, if anyone who download this found a way to get it working, please let me know here: 


[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=62484)


I will try to make a simple website to upload this and other projects.


Thanks in advance,


Jonathan


************ NEW EDIT **********************  

For all those people that cant connect to a server with this and other ports of uIP to avr, I solve this problem by making a few changes on the main.c file.  

I'm using an ATmega32 and the problem is with the timer handler, so it may vary depend of your uC model.


Here is the edited fragment:


//#define TIMERCOUNTER\_PERIODIC\_TIMEOUT (F\_CPU / TIMER\_PRESCALE / 2 / 256)


#define TIMERCOUNTER\_PERIODIC\_TIMEOUT 39/5


static unsigned char timerCounter;


void initTimer(void)


{  

/*  

 outp( 7, TCCR0 ) ; // timer0 prescale 1/1024 (7)


 // interrupt on overflow


 sbi( TIMSK, TOIE0 ) ;


 timerCounter = 0;  

*/


 TCCR0=0x05;  

 TCNT0=0x00;  

 OCR0=0x00;  

 //TIMSK=0x00;  

 TIMSK |= \_BV(TOIE0);


 timerCounter = 0;  

}


//SIGNAL(SIG\_OVERFLOW0)  

ISR(TIMER0\_OVF\_vect)  

{  

 timerCounter++;


}


Hope all people can work with this now!

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
