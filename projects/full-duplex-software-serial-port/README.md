# Full duplex software serial port

Uploaded by barnacle on 2009-02-03 10:14:00 (rating 0 out of 5)

## Summary

I needed two async serial ports on an AT Mega32 and sadly, there's only one UART on the chip. They both had to be full duplex and couldn't block the other interrupt driven stuff going on - the application is an engine ECU monitoring device with fairly tight response times in the access protocol, and there's an LCD dot-matrix display to update too.


Hence a software serial port. It works on an 8MHz M32 at 2400 baud comfortably and although I haven't tested it, I believe it will manage 4800 (certainly at 16MHz :) and with suitable changes to the names of the registers will work on other chips. I've used it on 2313, Mega8, and Mega16. Any unused port pins can be used for the in and out connections, including the txin and txout (though why you'd want to...)


Because I needed an internal clock for timestamping data and generating comms timeouts, I implemented this in the same routine.


It uses 14 bytes of storage (including the time to 20ms), plus a circular receive buffer which is definable - I've chosen 32 bytes. The routines compiled from the C code take around four hundred bytes - though that's with no optimisation, so this could be improved. I have an assembler version with the same logic which is somewhat smaller, but currently hiding from me on a removable disc.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

~  

~ EDIT - UPDATE - EDIT - UPDATE  

~  

Travis Kent Beste has kindly pointed out a bug which for some reason hasn't affected my use over the last four years, but which bit him: the following lines should be changed to make the UART registers volatile as well as static:



```
<br />
// software uart registers<br />
static uint8\_t	uart\_rxd;		// the byte being received<br />
static uint8\_t	uart\_txd;		// the byte being transmitted<br />
static uint8\_t	uart\_txtick;	// tick count for the transmitter<br />
static uint8\_t	uart\_rxtick;	// tick count for the receiver<br />
static uint8\_t	uart\_txbit;		// bit count for tx<br />
static uint8\_t	uart\_rxbit;		// bit count for rx<br />
static uint8\_t	uart\_status;	// uart status<br />
static uint8\_t next\_read\_2;<br />
static uint8\_t next\_write\_2;	// the pointers to the receive2 buffer<br />

```



They're up near the top of the file.  

~  

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


SIGNAL (SIG\_OUTPUT\_COMPARE1A)  

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

A timer tick is set up on Timer 1 such that a Compare 1 interrupt is generated at four times the baud rate. On every tick, the input line is sampled to see if a start bit might be present; if it is, a state machine waits two further samples to confirm that it wasn't just a glitch and then proceeds to sample every fourth tick for the actual data. When a complete byte has been received, it is placed in the next available space in the receive buffer.


Also in each tick, the same routine checks to see if there are any bits being transmitted - in which case another state machine ensures the correct sequence and bit duration - or it forces the tx output to the 'waiting' high condition.


The final part of the interrupt routine increments a real-time clock - although the hours simply increment and do not reset at midnight.


void ser2\_init (void)  

~~~~~~~~~~~~~~~~~~~~~  

Initialises the timer and interrupts, clears the buffers, and resets the rtc to zero.


void ser2\_out (char ch)  

~~~~~~~~~~~~~~~~~~~~~~~  

This is a blocking call to send a single character. It waits until the internal busy flag is cleared (which can be done prior to calling it, of course) as one would with the normal txout register on the hardware uart.


void ser2\_print (char * text)  

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

Sends a normal C zero-terminate string to the serial out line; again, this is blocking from the main program routine but interrupts continue without problem and input from either serial port continues to fill the buffers.


char ser2\_ischar (void)  

~~~~~~~~~~~~~~~~~~~~~~~  

Checks the input buffer to see if new data has arrived; this enables the input routine to be non-blocking. The input buffer byte cannot be read directly since there is no guarantee of synchronisation between it and the state machine, but the state machine places each received byte in the buffer as it is complete.  

The routine must be used prior to reading from the buffer as the buffer pointers are changed by the read routine.


char ser2\_in (void)  

~~~~~~~~~~~~~~~~~~~  

Returns the next available character from the buffer, having first checked that there is one available.


I have tended to use the two routines above (there are identical routines for the uart so I don't have to think too hard) is like this:


 if (ser2\_ischar())  

 {  

 // there's a character waiting, deal  

 ch = ser2\_in();


I trust this will be of help to someone...


Neil

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
