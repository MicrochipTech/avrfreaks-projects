;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Half duplex software UART in assembly compatible with ATTiny15L.
;
; Targeted at AVR chips without SRAM, this code uses registers
; in place of SRAM.  should work with minimal modification on 
; virtually all AVR chips.	( see REQUIREMENTS & USAGE below )
;
; 3 Byte receive buffer.  Its not too hard to modify the code to use any
; length provided the registers are available for it.
;
; source is formatted with a tab size of 3.  I suggest using that
; to read/modify it.
;
; Written using WinAVR.
; Version 1.0
; Oct 2005
; Tyler Colbert (tyler@colberts.us)
; variable Baud, 7 or 8 data bits, no parity, 1 or 2 stop bits
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; based in part on code obtained through avrfreaks.net with this header:
;		Project : UART3: Half Duplex Software UART (9600 bps, 8N1)
;		Version : 1.3
;		Date    : October 2002
;		Author  : Radu Hristea (radu.hristea@aptrans-group.com; transeast@programmer.net)
;		Company : TransEast s.r.l.                       
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; REQUIREMENTS:
;;   Understand that this code has some REQUIREMENTS that must be met.
;; 	A) some registers MUST be permanently allocated to it.
;;			A1) specifically, 3 registers between R16-R31, and 6 other registers
;;			A2) 4 of the other registers can be dropped out if you dont need to 
;;				receive data.
;; 	B) Exclusive use of one 8 bit timer/counter with a COMPARE interrupt.
;;			B1) There is some hope for changing the requirement for a COMPARE
;;				interrupt to an OVERFLOW interrupt.  But I havent tried yet.  It 
;;				might be useful in some cases (including on the Tiny15 which is my
;;				current target).
;; 	C) Exclusive use of one pin with an external interrupt (the RX pin)
;;		D) Exclusive use of one additional pin (the TX pin)
;;		E) The clock must be calibrated
;;		F) While this code uses NO "rcall" instructions, it does require 1 spot
;;			on the hardware stack due to its use of interrupts, and it does expect
;;			you to rcall it, which, at times, takes another entry.  Take this into
;;			account when writing your code.  You can, remove the need for the 
;;			RCALLs.  See USAGE below for details.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; USAGE:
;; 	1) Change register definitions to use the registers you want the code
;;			to use
;; 	2) Change signal defines to match the timer & extint you have allocated
;;			to the UART
;; 	3) Change timer register defines to match that same timer's registers
;;		4) Select a baud rate & # of stop bits.
;;		5) Select and set the timer divider values.
;;			5a) Its probably best to select the lowest divider value that does
;;				not generate a compiler error (I have some checks in there).
;;				but some trial and error may prove me wrong.
;;			5b) you need to fill in the values for the defines that set the clock
;;				rate up.
;;		6) uncomment "#define NO_TRANSMIT" if you want to strip out the transmitter
;;		7) uncomment "#define NO_RECEIVE" if you want to strip out the receiver
;;		8) do "rcall UART_init" to start the thing up (this will "sei")
;;		9)	Call UART_getchar to get characters
;;			9a) check for characters in the buffer (rx_buffer_valid > 0)
;;			9b) call the UART_getchar routine
;;				9b1) use "rcall UART_getchar", or,
;;				9b2) uncomment "#define UART_GETCHAR_RETADDR label" and put in the
;;					label of the location you want that routine to return to. Now 
;;					"rjmp UART_getchar" to it instead of "rcall"ing it.
;;			9c) resultant character will be in getchar_result register you
;;				defined in step 1).
;;		10) Call UART_putchar to send characters
;;			10a) put character in putchar_input register you defined in step 1).
;;			10b) call UART_putchar one of two ways
;;				10b1) use "rcall UART_putchar", or,
;;				10b2) uncomment "#define UART_PUTCHAR_RETADDR label" and put the
;;					label of the location you want that routine to return to. Now
;;					"rjmp UART_putchar" to it instead of "rcall"ing it.
;;				10bNOTE) this is a blocking call that will stall till the UART is
;;					idle, so if your receiving a bunch of data very quickly, this
;;					may cause a buffer overflow. Since this whole thing is only
;;					half duplex, you should design a protocol that assumes a half
;;					duplex connection to avoid this.  Alternately, you could modify
;;					the code to simply drop current RX data in favor of TX.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; headers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Use the standard header.  This helps make it all more portable
#include <avr/io.h>

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; configuration in a big way
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;#define UART_GETCHAR_RETADDR GetCharReturn
;#define UART_PUTCHAR_RETADDR PutCharReturn
;#define NO_TRANSMIT
;#define NO_RECEIVE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Register allocations (any register will do)
; these are permenantly reserved by the UART routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; first byte of receive buffer (not needed if NO_RECEIVE)
#define rx_buffer0		r2 
; second byte of receive buffer (not needed if NO_RECEIVE)
#define rx_buffer1		r3 
; third byte of receive buffer (not needed if NO_RECEIVE)
#define rx_buffer2		r4	
; Number of entries in the receive buffer that are valid (not needed if NO_RECEIVE)
#define rx_buffer_valid	r5	
; uart serial buffer
#define buffer				r6
; place to save the SREG register during interrupts
#define sreg_save			r7

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Register allocations (must be R16-R31)
; these are permenantly reserved by the UART routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; temporary
#define tempA				r16
; bit counter
#define bit_counter		r17 
; status register
#define status				r18

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Register allocations (any register will do)
; These are only used by their respective routines and can
; in fact be the same register, but must not be the same as
; any registers above.  These are still available for
; you to use.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; register that gets input for putchar (not needed if NO_TRANSMIT)
#define putchar_input	r20
; return value from getchar routine (not needed if NO_RECEIVE) 
#define getchar_result	r20

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; bit positions in the Status Register
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; internal use only
#define PUTCHAR_SPECIAL_RETURN 3
; receive got a frame error (client read/write)
#define RX_FRAME_ERROR 4
; receive buffer overflow (client read/write)
#define  RX_OVERFLOW 5
; busy sending data (client read only)
#define	TX_BUSY 	6 		
; busy receiving data (client read only)   
#define	RX_BUSY	7 		

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Pin definitions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;; RX -- on Tiny15, PB2 is the only pin that will do.
#define RX_BIT				PB2
#define RX_PORT			_SFR_IO_ADDR(PORTB)
#define RX_DDR				_SFR_IO_ADDR(DDRB)
#define RX_PIN				_SFR_IO_ADDR(PINB)

	; these are used to set up the external interrupt.
#define SIGEXTINT			SIG_INTERRUPT0
#define EXTINTBIT			6
#define EXTINTMSK			_SFR_IO_ADDR(GIMSK)
#define EXTINTFR			_SFR_IO_ADDR(GIFR)

	; which register do we need to change to make the external
	; interrupt be level triggered by a low value (falling edge
	; would probably also work).
#define EXTINTTYPE		_SFR_IO_ADDR(MCUCR)
	; which bits of the register should be changed
#define EXTINTTYPEMASK	0x03
	; what is the new value for those bits
#define EXTINTTYPEVAL	0x00

		;; TX -- Any pin should do
#define TX_BIT				PB4
#define TX_PORT			_SFR_IO_ADDR(PORTB)
#define TX_DDR				_SFR_IO_ADDR(DDRB)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Timer signals & registers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define SIGCOMPARE		SIG_OUTPUT_COMPARE1A
#define TIMER_OCR			_SFR_IO_ADDR(OCR1A)
#define TIMER_TCNT		_SFR_IO_ADDR(TCNT1)
#define TIMER_TIFR		_SFR_IO_ADDR(TIFR)
#define TIMER_TIMSK		_SFR_IO_ADDR(TIMSK)
#define TIFR_BIT			6
#define TIMSK_BIT			6

; define this to indicate that the timer can clear on a compare match
; I havent looked to see if its common, but the Tiny15 can.  This should
; improve accuracy, especially if you have a multiplied timer clock.
#define TIMER_AUTOCLEAR
; timer control register
#define TIMER_TCCR			_SFR_IO_ADDR(TCCR1)
; value to set the TCCR to
;#define TIMER_TCCR_VAL	((1<<CTC1)|(1<<CS12)|(1<<CS10))
;#define TIMER_TCCR_VAL	((1<<CTC1)|(1<<CS12)|(1<<CS11))
;#define TIMER_TCCR_VAL	((1<<CTC1)|(1<<CS12)|(1<<CS11)|(1<<CS10))
#define TIMER_TCCR_VAL	((1<<CTC1)|(1<<CS13))
; frequency divider => (CLK * CLOCK_MULITPLIER / CLOCK_DIVIDER)
; some chips (like the Tiny15) have a higher resolution mode with one
; of their timers, hence the CLOCK_MULTIPLIER
#define CLOCK_DIVIDER		8
#define CLOCK_MULTIPLIER	1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; serial protocol
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; I tried to make the compiler complain if you selected a baud
; rate that was too high, or was a bad match for your clock
; divider value.  But I havent tested this much.
#define BAUD		1200

; STOP_BITS can be 1 or 2
#define STOP_BITS	1

; DATA_BITS can be 6 or 7 or 8
#define DATA_BITS 8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; clock frequency
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define CLK		1600000


