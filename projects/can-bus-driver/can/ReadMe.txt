The simple AVR CAN driver.

This driver allows you to send and receive CAN messages.  It uses two interrupt based FIFO buffers for RX and TX.

---------------------- Driver Setup ----------------------

1. Set CAN version 2A or 2B.		(can.h)
2. Choose Baud rate.			(can.c)
3. Set delay times.			(can.c)
4. Setup message acceptance filter.	(can.c)

---------------------- Transmission Application code example ----------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include "tim.h"
#include "can.h"

int main(void) 
{
 cli();
  //setup WatchDog Timer
 MCUSR&= ~_BV(WDRF); 
 WDTCR = _BV(WDCE) | _BV(WDE);		//Allow WatchDag Change
 WDTCR = 0x00;  		  	  		//Disable WatchDog
  //Setup timer
 start_timer();
  //Setup CAN
 init_can();
 sei();
  //Wait for all CAN nodes to init before sending.
 unsigned char tx_wait;
 tx_wait = timer_get(500);
 
 unsigned char rpm = 125;

 union can_frame can_engin_rpm;
 can_engin_rpm.id = 0x0123;
 can_engin_rpm.data[0] = rpm;
 can_engin_rpm.length = 1;
 
 // MAIN CONTROL LOOP
 for (;;) {
	if (timer_expired(tx_wait)) {
		timer_reset(tx_wait, 100);		//send every 100mS
		send_can_frame(&can_engin_rpm);
	}	
 }
}

---------- Reception Appliction code example ----------

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "can.h"

int main(void) 
{
 cli();
  //setup WatchDog Timer
 MCUSR&= ~_BV(WDRF); 
 WDTCR = _BV(WDCE) | _BV(WDE);		//Allow WatchDag Chnage
 WDTCR = 0x00;  		  	  		//Disable WatchDog
  //Start UART
 start_UART();
  //Setup CAN
 init_can();
 sei();
 
 // MAIN CONTROL LOOP
 for (;;) {
	if (new_can_frame()) {
		union can_frame *can_engin_rpm;		//temporaraly declare space on stack
	   	can_engin_rpm = read_can_frame();
		if (can_engin_rpm->id == 0x0123) {
			txbyte('\r');
			printu16(can_engin_rpm->data[0]);
		}
		next_can_frame();
	}	
 }
}
