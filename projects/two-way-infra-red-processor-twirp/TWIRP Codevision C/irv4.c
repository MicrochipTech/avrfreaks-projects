// Two-Way Infrared Processor (TWIRP)
// (c) 2003 Stephen L. Childress  stevech@san.rr.com
// v1	Feb 4, 2003

/********************************************
Chip type           : AT90S2313
Clock frequency     : 10.000000 MHz
Memory model        : Tiny
Internal SRAM size  : 128
External SRAM size  : 0
Data Stack size     : 32
*********************************************/

#include <90s2313.h> 
// !!! NOTE !!! The assembly code in the interrupt routines is written
//  for the tiny memory model mode of the 2313. 8 bit pointers are used.
//  Change this to use the high and low registers if needed for other chips.

#asm
	.EQU UCR=0xA		; UART control reg (not in 90s2313.h)
#endasm

// Standard Input/Output functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h> 

//--------------------------------------------------
#define VERSIONID 1
#define TIMER0RELOAD	217	// one milisecond interrupt, for the crystal in use   
#asm (".EQU TIMER0RELOAD = 217")

#define IRDETECT_PORT PINB
#define IRDETECT_PIN 2

#define	IRLRED_PORT PORTD	// Infrared LED connection
#define	IRLED_PIN 3			// must be on this pin which is toggled by Timer1

#define BILED_PORT PORTD	// BI-Color LED connection
#define BILED1_PIN 4
#define BILED2_PIN 5

#define LEADINMIN		3	// minimum IR lead-in pulse width in mSec (byre)
#define LEADINMAX		113	// maximum IR lead-in pulse width in mSec (byte)
#define	IRPWMAX			13	// maximum IR pulse width; wider = restart sensing
  
// I/O Ports B and D Data Direction Registers
#define DDRB_INIT ((1 << BILED1_PIN) + (1 << BILED2_PIN));	//0x00; PD4 and PD5 are outputs
#define DDRD_INIT (1 << IRLED_PIN)			// is an output	

// --------- COMMANDS RECEIVED FROM HOST -----------  
#define CMD_SENDIR  		0	// Host is sending IR data
#define CMD_VERSION  		1  	// Request Version #
#define CMD_VERSIONSIGONLY 	2  	// Request Version #, and suppress all but signature data
#define CMD_TONEOFF  		3  	// Turn off IR tone
#define CMD_TONEON  		4	// Turn on IR tone, constantly (testing)
//#define CMD_TONEOFF  		5  	// Turn off IR tone


// --------- RESPONSES SENT TO HOST -----------
#define	SENDIR_FINISHED_OK				1
#define	SENDIR_FINISHED_UNDERRUN		2
#define SENDIR_HOST_TIMEOUT				3
#define	RECEIVEIR_OK					4
#define	RECEIVEIR_BUFFERFULL			5


// STORAGE

typedef  unsigned char byte;
#asm
	.EQU errorbitUARTrx = 3	; must match below
	.DEF errorflags = R2
#endasm 

//char **pp;

//   0      1       2           3			4   		5
bit tone, irbit, repeating, errorUARTtx, errorUARTrx, signatureonly;	// THIS WILL BE IN R2

// ---- MINIMAL use of variables - need lots of buffer space in RAM
volatile byte ticksmsec;
byte outbufcount, inbufcount;
byte delayval;

register byte b, pw, n, bursts;
byte *outbufpw, *outbufpr;
byte *inbufpw, *inbufpr;
byte k1;
byte signature[6];

#define UARTSENDBUFSIZE 8
byte outbuf[UARTSENDBUFSIZE];			// UART transmit buffer
#asm
_outbufend:			; so the assembly code knows where this is
#endasm

#define UARTRECEIVEBUFSIZE 80			// <<<<< BEWARE. If too large, hardware stack too small
byte inbuf[UARTRECEIVEBUFSIZE];			// UART receive buffer
#asm
_inbufend:
	.EQU INBUFSIZE = _inbufend - _inbuf
#endasm




//-----------------------------------------------
// INTERRUPT SERVICE, TIMER 0   
// Interrupts every milisecond and increments a one byte variable

#pragma savereg- /* Turn registers saving off  in compiler */

interrupt [TIM0_OVF] void timer0_ovf_isr(void)  {
/* now save only the registers that are
   affected by the routines in the handler,
   To minimize stack space and execution time. */
#asm
    push r30
    in   r30,SREG
    push r30

	LDI  R30,LOW(TIMER0RELOAD)		; reset counter 
	OUT  0x32,R30;
	LDS  R30,_ticksmsec		; increment milisecond counter
	SUBI R30,-LOW(1)
	STS  _ticksmsec,R30

    pop r30
    out SREG,r30
    pop r30
    reti
#endasm
}
#pragma savereg+


//-----------------------------------------------
// INTERRUPT SERVICE, UART TRANSMIT
//  transmit data register empty condition.



#pragma savereg- /* Turn registers saving off  in compiler */

interrupt [UART_DRE] void UART_TX_isr(void) {
//---- Qssembly language interrupt handler for UART.
//     Optimizes speed and minimizes RAM used for register saves
#asm
    push r30			; save registers used herein
    in   r30,SREG
    push r30
    push r26
    tst	_outbufcount	; buffer empty?
    breq uartTX20                      
    dec	_outbufcount	; decrement count	
	LDS  r26,_outbufpr	; buffer pointer in X reg
	LD   R30,X+         ; get from buffer (8 bit pointer)
	OUT  UDR,R30		; send to UART
	ldi	r30,LOW(_outbufend)	; address of end of buffer+1
	cp	r26,r30			; at end of buffer?
	brne uartTX10		; not at end of circular buffer
	ldi r26,LOW(_outbuf)	; wrap
uartTX10:
	STS  _outbufpr,R26
	rjmp uartTX30		; return from interrupt
	
uartTX20:
	cbi  UCR,5			; disable TX interrupts
uartTX30:
	pop r26
    pop r30
    out SREG,r30
    pop r30
    reti
#endasm
}

#pragma savereg+ 

//-----------------------------------------------
// // INTERRUPT SERVICE, UART RECEIVE
#pragma savereg- /* Turn registers saving off  in compiler */

interrupt [UART_RXC] void UART_RX_isr(void) {

#asm
    push r30			; save registers used herein
    in   r30,SREG
    push r30
    push r26

	ldi r26,INBUFSIZE	; size of input buffer storage area
	cp	r26,_inbufcount	; compare to count of bytes alread in buffer 
    in	r30,UDR			; input from UART (flags unaffected)
	brne uartRX05		; branch if buffer is not full
	set					; buffer full error, set the T bit
	bld	errorflags,errorbitUARTrx ; set flag bit
    rjmp uartRX20		; exit, discarding new byte
; store received byte in buffer   
uartRX05:
	inc	_inbufcount 	; incrm count
	lds	r26,_inbufpw
	st	X+,r30 			; store in buffer
	ldi	r30,LOW(_inbufend)
	cp	r26,r30			; wrap pointer if at end of buffer
	brne uartRX10
	ldi	r26,LOW(_inbuf)
uartRX10:
	sts	_inbufpw,r26

uartRX20:				; restore registers, exit interrupt	
	pop r26
    pop r30
    out SREG,r30
    pop r30
    reti
#endasm
}
#pragma savereg+





//-----------------------------------------------
// UART TRANSMIT, BUFFERED 
// This code adds a non-interrupt driven buffer to the
// UART transmit chip's data register. Buffer must be emptied
// with code other than the below.
// We ASSUME that the buffer empties faster than it can be filled.
void sendbyte(byte d)
{
	if (outbufcount >= UARTSENDBUFSIZE)  {
		errorUARTtx = 1;		// error, transmit buffer full, data discarded
		return;
	}
#asm ("cli")	// disable interrupts while changing variable use in interrupt
	if (UCR.5 == 0)  { // if interrupt handler has finished buffer
		outbufpw = outbufpr = outbuf;  // initialize pointers
		UDR = d;		// send byte to UART, no need to buffer
		UCR.5 = 1;		// enable uART TX interrupt
	}
	else {
		++outbufcount;
		*outbufpw++ = d;	// store in buffer for interrupt
		if (outbufpw == &outbuf[UARTSENDBUFSIZE])
			outbufpw = outbuf;
	}
#asm ("sei")
}


//----------------------------------------------------------------  
// RETURNS ONE BYTE FROM SERIAL PORT RECEIVE DATA, IF AVAILABLE
//   
// returns 1 if byte was placed in argument b, else 0
byte receivebyte(byte *by)
{
	byte ret;
	
	if (inbufcount == 0)		// is buffer empty?
		ret = 0;				// no, chagne return value
	else  {
		ret=1;
#asm ("cli")	// disable interrupts while changing variable use in interrupt
		*by = *inbufpr++;		// get from buffer, storein param passed as pointer
  		if (inbufpr == &inbuf[UARTRECEIVEBUFSIZE])	// wrap write pointer
			inbufpr = inbuf;		
		--inbufcount;				// one less byte is in the buffer	
#asm ("sei")	
	}
	return(ret);
}	
	
//---------------------------------------------------------------- 
// GET HOST's TIMING DATA FROM SERIAL PORT RECEIVE SIDE AND 
//  RECREATE THE IR TIMING USING THE IR LED.
void sendir()
{
	byte b1;
	byte nsent;

	errorUARTrx = 0;	// clear error flag
	tone = 1;			// by definition, first data is tone on
	PORTD.3 = 0;
	nsent = 0;
	b1 = 0xff;
	
	// loop for all host data received on serial port
	for ( ;; )  {
		ticksmsec = 0;
		for ( ;; )  {		// wait for serial data to arrive
		  	if (receivebyte(&delayval) != 0)	// Serial port data arrived
		  		break;
			if (ticksmsec >= IRPWMAX)  {
			TCCR1A = 0x00;	// disassociate LED port with timer
			sendbyte(SENDIR_HOST_TIMEOUT);   // tell PC error
			return;
			}
		}
		
		if (delayval == 0)  {  		// 0 is an indicator of ... see below
			if (b1 == 0)	   		// two 0's in a rwo means quit (b1 is prior data)
				break;
		}  							// remember the 0, but wait for next byte
		else  {  					// turn tone on or off, then delay
			++nsent;
	   		if (tone == 1)
				TCCR1A = 0x40;		// turn tone on (timer 1 toggles PB.3)
			else
				TCCR1A = 0x80;		// turn tone off
			tone = ~tone;  			// toggle for use in the delay period
			if (b1 != 0)  {			// if prior was not zero, then use Timer 0
				TCNT0 = 0;
				while (TCNT0 != delayval)	// delay per received value
					;
	   		}
	   		else  {  				// prior was 0, so use milisecond timer
	   			TCNT0 = 0xD9;		// initialize hardware timer
	   			ticksmsec = 0;
	   			while (ticksmsec != delayval) 	// Timer interrupt increments counter
	   				;
	   		}
		}	
		b1 = delayval;	// remember this delay value
	}
	
	// end of data
	TCCR1A = 0x00;						// disassociate LED port with timer
//	delay_ms(10);
	// Respond with status
	if (errorUARTrx)	 				// error flag
		sendbyte(SENDIR_FINISHED_UNDERRUN);			// 2 bytes of status to PC
	else
		sendbyte(SENDIR_FINISHED_OK);			// 2 bytes of status to PC
}

	
//---------------------------------------------
//--------------- MAIN ------------------------
//---------------------------------------------
void main(void)
{

// Input/Output Ports initialization
// Port B initialization 
PORTB=0x00;
DDRB=DDRD_INIT;	

// Port D initialization
PORTD=0xFF;
DDRD=DDRB_INIT;

// Timer/Counter 0 initialization
// Clock source: System Clock = 10MHz

TCCR0=0x04;  // timer clock = system clock / 256 = 256 microseconds
// load timer with this for overflow at 1 milisecond
TCNT0=0xD9;	// = 217 decimal
// 256 uSec * (256 - 217) = 0.9984 milisecond

// Timer/Counter 1 initialization
// No interrupt needed. Operated as an up-counter,
// counting from 0 to 1/2 the period of the IR carrier.
// At each counter roll-over, the OC1 bit is either toggled or
// set to 0 according to TCCR1A control bits. This creates the IR carrier.
// Clock source: System Clock divided by 1
TCCR1A=0x00;
TCCR1B=0x09;	// CTC1 mode is bit 3; prescaler choice is bits 1 and 0 = 01
TCNT1H=0x00;
TCNT1L=0x00;

// Timer1 compare register. Count up to this number, the reset to 0.
// And at this point, toggle or clear the PB.3 I/O bit
OCR1H=0; 	// 16 bit register = 1/2 period of 36KHz = 13.8 uSec @ 10MHz Xtal
OCR1L=138;	// 		LSB = 10 MHz = 0.1 microsecond

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x02;

// UART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// UART Receiver: On
// UART Transmitter: On

UCR=0x18;
//UBRR=0x40; // 9600 baud w/10MHz Xtal
UBRR=0x20; // 19200 baud w/10MHz Xtal
//UBRR=0x10; // 38400 baud w/10MHz Xtal

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
// Analog Comparator Output: Off
//ACSR=0x80;



	outbufpw = outbufpr = outbuf;  	// UART send buffer pointers
	inbufpw = inbufpr = inbuf;  	// UART send buffer pointer	
	outbufcount = inbufcount = 0;
	UCR.7 = 1;						// UART receive interrupt enable
	
// Global enable interrupts
#asm("sei")  
 
// MAIN LOOP PROGRAM  -------------------------------
top:
	ticksmsec = 0;
	for ( ;; )  { 
		b = 0;  
		while (IRDETECT_PORT.IRDETECT_PIN == 1)  { 	// IR sensed yields a 0 on this bit
			if (receivebyte(&b))  { // no IR present, so see if serial data arrived from host
				switch(b)  {   		// decode 1st byte of received data
				case CMD_VERSIONSIGONLY:	// program version ID
					signatureonly = 1;		
					sendbyte(VERSIONID);
					break;
				case CMD_VERSION:	// program version ID
					signatureonly = 0;
					sendbyte(VERSIONID);				
					break;
				case CMD_SENDIR:
					BILED_PORT.BILED1_PIN = 0; // toggle LED 
					BILED_PORT.BILED2_PIN = 1;
					sendir();		// take host's data and pulse the IR diode
					break;
#ifdef TONESUPPORT
				case CMD_TONEON:
					TCCR1A = 0x40;	// constant IR tone on
					break;
				case CMD_TONEOFF:
					TCCR1A = 0x80;	//	IR tone off
					break;
#endif	     
				}
			goto top;
			}
			// No host data received yet, so check timer
			if (ticksmsec == 100)  {	// 100 mSec elapsed yet?
				ticksmsec=0;
				b = (++b) & 31;		// blink LED every few seconds
				if ((b == 29) || (b == 31))  {  // causes two quick blinks
					BILED_PORT.BILED1_PIN = 0; // toggle LED while waiting (low duty cycle)
					BILED_PORT.BILED2_PIN = 1;	
				}
				else  {
					BILED_PORT.BILED1_PIN = 1; // LED off
					BILED_PORT.BILED2_PIN = 1;
				}
			}
		}	// while no IR tone detected
		
		// IR sensor has begun to sense IR
		
		
		
		
		BILED_PORT.BILED1_PIN = 1; // toggle LED color
		BILED_PORT.BILED2_PIN = 0;		
		
		
		errorUARTtx = 0;			// UART transmit error flag
		ticksmsec=0;				// prepare to measure lead-in IR bit

		while (IRDETECT_PORT.IRDETECT_PIN == 0)  {   		// wait for Lead-in to end
			if (ticksmsec > LEADINMAX)
				goto top;      			// excessively long lead-in
	    }
	 	if (ticksmsec < LEADINMIN)		// lead-in too short
			goto top;
		if (!signatureonly)	{
	  		sendbyte(0); 					// means miliseconds follow
   	   		sendbyte( (byte) ticksmsec);   // duration of lead-in
		}
		// prepare to parse incoming IR bit stream
	    irbit = 1;					// The I/O bit is now this state 
	    bursts = k1 = 0;
	    repeating = 0;
  		for (n = 0; n < 6; ++n)		// zero out signature
  			signature[n] = 0;
  			
	    // --------------------------------------------------------
	    // measure and report IR pulse widths - both low's and hi's
	    for ( ;; )   {
	   		ticksmsec=0;
	   		TCNT0=0;
    	    while (IRDETECT_PORT.IRDETECT_PIN == irbit)  {	// wait for IR sensor bit state change
    	    	if (ticksmsec >= 100)		// if long duration
    	    		goto endmsg;
    	    }
	        irbit = ~irbit;				// remember bit state

	        // Send measured hi or low pulse duration
	        b = TCNT0; 					// sub-milisecond counter
	        TCNT0=0; 
	   		if (ticksmsec == 0)  {		// if pulse width less than a milisecond
	     		if (!signatureonly)
	     			sendbyte(b);		// send the timer counts as measurement 
	     		if (irbit == 0)	 {	// if 0, then this is start of a new bit cell
	     			pw += b;
	     			if (++bursts == 2)		// the first "1" tone burst after lead-in
	     				k1 = (pw >> 2) + (pw >> 1);		// k1 = 0.75 * pw
	     			if (!repeating)  {
        	     		for (n = 5; n != 0; --n)  {   // shift 6 byte number left 
        					signature[n] <<= 1;
        					if (signature[n-1] & 128)
        						signature[n] |= 1;
        				}
        				signature[0] <<= 1;
        				if (pw >= k1) 				// decode 1 based on pulse widths
        					signature[0] |= 1;
    				}
				}
				else
					pw = b;
	     	}		
	      	else  {						// width was more than a milisecond
	      		if (!signatureonly)  {
	      			sendbyte(0);			// flags that next is miliseconds
	      			sendbyte(ticksmsec);	// send number of miliseconds 
	      		}
	      		if ((ticksmsec >= 4) && (k1 != 0))
	      			repeating = 1;
 			}
    	} 
    	// loop until no data for IRPWMAX
endmsg:
		// send 0 0 meaning end of data
		if (!signatureonly)  {
        	sendbyte(0);		// 0 0 means end of data
        	sendbyte(0);
        	// send status
            if (errorUARTtx)
            	sendbyte(RECEIVEIR_BUFFERFULL);
    		else       	
            	sendbyte(RECEIVEIR_OK);
  		}
  		
  		// send the signature bytes
  		for (n = 6; n != 0; --n) 		// send signature bytes
  			sendbyte(signature[n-1]);

 	} // the for( ;; ) loop of the main program
}
