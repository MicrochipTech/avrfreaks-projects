/*********************************************
*
* tcj 3-9-2015
*
* usbb_bulk_example.c
*
* 15031601
*
* example program: Bulk Vendor class USB for AT32UC3B series
*
*
* receives 2 bytes on endpoint 4 and echoes them on enpoint 3
*
*
*
* Chip type           : at32uc3b1256
* Clock frequency     : Internal clock oscillator at 48 MHz using external 12 MHz crystal
*
* note that when chip comes up in dfu mode (default) the external crystal is also default and it works
* automatically for USB (for an 8, 12, or 16 MHz crystal). 
* 
*
*
* In order to boot into dfu mode and thus allow programming, it is necessary to do a reset (bring the reset pin
* 47 down then up) while PA13 (pin 25) is down. This is done with pushbuttons. 
*
*
*
*********************************************/

#include <avr32/io.h>
#include <stdlib.h>
#include <inttypes.h>

#include "tcj_usb.h"

#define F_PBA 48000000L
#define PRINT_USART 2

#include "usart_io.h"

#define INTCB17			// must be defined before #include of "intcb.h" - 17 is USBB

#include "intcb.h"




void service_setup_packet(void)							//  called by intcb17() on receipt of USB setup packet
{
	uint8_t i;
	uint8_t data_in[8];

	sr_printf("got setup packet\n");

	for(i = 0; i < 8; i++)	 data_in[i] = *(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i );		
			
	AVR32_USBB.UESTA0CLR.rxstpic = 1;						// clear to ACK received setup packet 
	
	sr_printf("setup packet:  %2x %2x %2x %2x %2x %2x %2x %2x \n", data_in[0],data_in[1],data_in[2],data_in[3],data_in[4],data_in[5],data_in[6],data_in[7] );


	if(data_in[0] == 0) 								// host-to-device
	{
		if(data_in[1] == 5)								// set address
		{
			AVR32_USBB.UDCON.uadd = data_in[2];
			AVR32_USBB.UDCON.adden = 0;
			AVR32_USBB.UESTA0CLR.txinic = 1;			// ACK

			while(!AVR32_USBB.UESTA0.txini)	{}			// wait for data to come into tx buffer
			AVR32_USBB.UESTA0CLR.txinic = 1;			// ACK
			AVR32_USBB.UDCON.adden = 1;
	
			sr_printf("end of get address\n");
		}

		else if(data_in[1] == 9)						// set configuration: it's time to configure endpoints 3 and 4
		{
			AVR32_USBB.UESTA0CLR.txinic = 1;			// ACK

			AVR32_USBB.UERST.epen3 = 1;					// enable endpoint 3
			AVR32_USBB.UECFG3.eptype = 2;				// this is a bulk endpoint
			AVR32_USBB.UECFG3.epdir = 1;				// direction is in
			AVR32_USBB.UECFG3.epsize = 3;				// memory size is 64 bytes
			AVR32_USBB.UECFG3.epbk = 0;					// one bank
			AVR32_USBB.UECFG3.alloc = 1;				// allocate memory for this endpoint

			AVR32_USBB.UERST.epen4 = 1;					// enable endpoint 4
			AVR32_USBB.UECFG4.eptype = 2;				// this is a bulk endpoint
			AVR32_USBB.UECFG4.epdir = 0;				// direction is out
			AVR32_USBB.UECFG4.epsize = 3;				// memory size is 64 bytes
			AVR32_USBB.UECFG4.epbk = 0;					// one bank
			AVR32_USBB.UECFG4.alloc = 1;				// allocate memory for this endpoint

			AVR32_USBB.UECON4SET.rxoutes = 1;				// enable ep 4 received data out int

	
			sr_printf("endpoints 3, 4 configured\n");
		}
	}

	else if(data_in[0] == 0x80)							// device-to-host
	{
		if(data_in[1] == 6)								// get descriptor
		{
			if(data_in[3] == 1)							// get device descriptor
			{
				while(!AVR32_USBB.UESTA0.txini);																// is fifo ready to accept data?

				uint8_t darray[18] = {0x12, 0x01, 0x00, 0x02, 0, 0, 0, 0x40, 0x66, 0x66, 0xab, 0x89, 0, 0, 0x07, 0x08, 0, 0x01}; // the device descriptor
				for (i = 0;i < 18; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];

				AVR32_USBB.UESTA0CLR.txinic = 1;		// send packet
				sr_printf("sent device descriptor\n");
			}

			else if(data_in[3] == 2)					// get config descriptor
			{
				while(!AVR32_USBB.UESTA0.txini);																// is fifo ready to accept data?

				if(data_in[6] == 9)						// host wants just the config descriptor by itself
				{
					uint8_t darray[9] = {9, 2, 0x20, 0, 1, 0, 0, 0x80, 0x32};
					for (i = 0;i < 9; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];			// the configuration descriptor
					sr_printf("sent config descriptor\n");
				}


				else									// host wants the config descriptor plus the others
				{
					uint8_t darray[9] = {9, 2, 0x20, 0, 1, 0, 0, 0x80, 0x32};												// the configuration descriptor (again)
					for (i = 0;i < 9; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];

					uint8_t d1array[9] = {0x09, 0x04, 0, 0, 0x02, 0xff, 0xff, 0xff, 0};										// the interface descriptor
					for (i = 0;i < 9; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i + 9 ) = d1array[i];

					uint8_t d2array[7] = {0x07, 0x05, 0x83, 0x02, 0x40, 0, 0};												// endpoint descriptor (for in endpoint 3)
					for (i = 0;i < 7; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i + 18 ) = d2array[i];

					uint8_t d3array[7] = {0x07, 0x05, 0x04, 0x02, 0x40, 0, 0};												// endpoint descriptor (for out endpoint 4)
					for (i = 0;i < 7; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i + 25 ) = d3array[i];

					sr_printf("sent config descriptor plus the others\n");
				}

				AVR32_USBB.UESTA0CLR.txinic = 1;		// send packet
			}

			else if (data_in[3] == 3)					// get string descriptor
			{
				while(!AVR32_USBB.UESTA0.txini);																// is fifo ready to accept data?

				if (data_in[2] == 0)
				{
					uint8_t darray[4] = {0x04, 0x03, 0x09, 0x04};															// language string
					for (i = 0;i < 4; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];
				}

				else if (data_in[2] == 7)
				{
					uint8_t darray[44] = {44, 3, 0x47, 0, 0x61, 0, 0x72, 0, 0x67, 0, 0x6f, 0, 0x79, 0, 0x6c, 0, 0x65, 0, 0x20, 0, 0x45, 0, 0x78, 0, 0x74, 0, 
																		0x65, 0, 0x6e, 0, 0x73, 0, 0x69, 0, 0x6f, 0, 0x6e, 0, 0x20, 0, 0x53, 0, 0x41, 0}; // "Gargoyle Extension SA"
					for (i = 0;i < 44; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];
				}

				else if (data_in[2] == 8)
				{
					uint8_t darray[30] = {30, 3, 0x55, 0, 0x43, 0, 0x33, 0, 0x42, 0, 0x20, 0, 0x42, 0, 0x55, 0, 0x4c, 0, 
																									0x4b, 0, 0x20, 0, 0x54, 0, 0x45, 0, 0x53, 0, 0x54, 0}; // "UC3B BULK TEST"
					for (i = 0;i < 30; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];
				}

				AVR32_USBB.UESTA0CLR.txinic = 1;				// send packet
				sr_printf("sent string descriptor\n");
			}

			else if (data_in[3] == 6)							// get device qualifier - which is used only for high speed devices, so this request is pointless -
			{													// but providing a meaningless device qualifier keeps the host from repeating the request 3 times
				uint8_t darray[10] = {0x0A, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00};

				while(!AVR32_USBB.UESTA0.txini);																// is fifo ready to accept data?
				for (i = 0;i < 10; i++)			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + i ) = darray[i];

				AVR32_USBB.UESTA0CLR.txinic = 1;				// send packet
				sr_printf("device qualifier requested - not pertinent\n");
			}

			else 												// asking for descriptor type I don't support
			{
				AVR32_USBB.UESTA0CLR.txinic = 1;				// ACK
				sr_printf("data_in[3] is not 1, 2, 3, or 6\n");
			}
		}

		else if(data_in[1] == 0)								// status request - not yet implented (not needed so far)
		{
			AVR32_USBB.UESTA0CLR.txinic = 1;					// ACK
			sr_printf("status request - not yet implemented\n");
		}

		else
		{	
			AVR32_USBB.UESTA0CLR.txinic = 1;					// ACK
			sr_printf("data_in[1] is not 0 or 6\n");	
		}
	}

	else if ((data_in[0] & 0x60) == 0x40)
	{
		AVR32_USBB.UESTA0CLR.txinic = 1;						// ACK
		sr_printf("vendor request error\n");
	}

	else
	{
		AVR32_USBB.UESTA0CLR.txinic = 1;						// ACK
 		sr_printf("screw up - setup packet: %x %x %x %x %x %x %x %x \n", data_in[0],data_in[1],data_in[2],data_in[3],data_in[4],data_in[5],data_in[6],data_in[7] );
	}
}

void intcb17(void)																// ISR for USBB
{
	if(AVR32_USBB.UDINT.wakeup)	AVR32_USBB.UDINTCLR.wakeupc = 1;				// clear the wakeup interrupt on wakeup
	
	if(AVR32_USBB.UDINT.eorst)
	{
		AVR32_USBB.UERST.epen0 = 1;					// enable endpoint 0, the control endpoint
		AVR32_USBB.UECFG0.eptype = 0;				// this is a control endpoint
		AVR32_USBB.UECFG0.epdir = 0;				// direction is out (because control endpoint)
		AVR32_USBB.UECFG0.epsize = 3;				// memory size is 64 bytes
		AVR32_USBB.UECFG0.epbk = 0;					// one bank
		AVR32_USBB.UECFG0.alloc = 1;				// allocate memory for this endpoint

		AVR32_USBB.UDINTCLR.eorstc = 1;				// clear the end of reset interrupt

		AVR32_USBB.UECON0SET.rxstpes = 1;			// enable ep 0 received setup int
		AVR32_USBB.UECON0SET.rxoutes = 1;			// enable ep 0 received data out int

	}

	if(AVR32_USBB.UESTA0.rxstpi) service_setup_packet();		// a setup packet has been received
	
	if(AVR32_USBB.UESTA4.rxstpi) {}								// if interrupt driven rx from ep4 is desired, this is where to put it
}


int main(void)
{
		
	setup_usart(2,19200,8,'N',1,'B');				// sets up USART 2 for 19200 baud, 8 bits, no parity, 1 stop bit, routed to function B pins
	setup_intcb();									// setup interrupt handling (function in intcb.h)

	uint8_t data[2];								// used to get and send USB character data
	uint8_t ep_select;								// the endpoint we want to use at the moment
	
// set up main clock

	AVR32_PM.MCCTRL.osc0en = 1;						// enable oscillator 0 (12 MHz crystal)
	AVR32_PM.OSCCTRL0.mode = 7;						// select crystal > 8 MHz

	AVR32_PM.PLL[0].pllen = 1;						// enable PLL0
	AVR32_PM.PLL[0].pllosc = 0;						// OSC0 is source for PLL0
	AVR32_PM.PLL[0].pllmul = 3;						// multiply by 8 = 96 MHz
	AVR32_PM.PLL[0].plldiv = 0;						// no input division
	AVR32_PM.PLL[0].pllopt = 3;						// divide output by 2 = 48 MHz

	while(!(AVR32_PM.POSCSR.lock0 & 1));			// wait for PLL0 to lock

	AVR32_PM.MCCTRL.mcsel = 2;						// PLL0 is source for main clock

// Note that PBA clock is equal to main clock (48 MHz) as I have left pbadiv = 0


// set up generic clock 3 for USBB

	AVR32_PM.GCCTRL[3].cen = 1;						// enable generic clock 3
	AVR32_PM.GCCTRL[3].pllsel = 1;					// select a PLL as source
	AVR32_PM.GCCTRL[3].oscsel = 0;					// select PLL0 as the source


//  set up USBB

	AVR32_USBB.USBCON.uimod = 1;					// device mode
	AVR32_USBB.USBCON.uide = 0;						// use uimod bit and not pin!

	AVR32_USBB.UERST.eprst0 = 1;					// reset endpoint 0
	AVR32_USBB.UERST.eprst3 = 1;					// reset endpoint 3
	AVR32_USBB.UERST.eprst4 = 1;					// reset endpoint 4

	AVR32_USBB.USBCON.frzclk = 0;					// unfreeze the clock
	AVR32_USBB.USBCON.otgpade = 1;					// needed to start; no explanation in datasheet

	AVR32_USBB.UDCON.ls = 0;						// full speed resistor pullup select
	AVR32_USBB.USBCON.usbe = 1;						// enable USBB	
	AVR32_USBB.UDCON.detach = 0;					// attach the pullup

	AVR32_USBB.UDINTESET.wakeupes = 1;				//enable wakeup int
	AVR32_USBB.UDINTESET.eorstes = 1;				//enable end of reset int


	START_GLOB_INTS									// enable global interrupts

	sr_printf("\nusbb slave address: %8lx\n\n", AVR32_USBB_SLAVE_ADDRESS);

	while(1)
	{

		if(sr_kbhit())  sr_putchar(sr_getchar());   // rs232: echo character


			
		if(AVR32_USBB.UESTA4.rwall)					// data in USB out buffer?
		{
			ep_select = 4;							// the out endpoint; get data from host
			data[0] = *(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + ep_select*0x10000);
			data[1] = *(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + ep_select*0x10000 + 1);

			AVR32_USBB.UESTA4CLR.rxoutic = 1;			
			AVR32_USBB.UECON4CLR.fifoconc = 1;

			sr_printf("ep4 bytes: %d   %d\n", data[0], data[1]);
		}

		if(AVR32_USBB.UESTA3.nakini)				// IN request from USB host?
		{
			while(!AVR32_USBB.UESTA3.txini);																// is fifo ready to accept data?
			AVR32_USBB.UESTA3CLR.txinic = 1;

			ep_select = 3;							// the in endpoint; send data to host
			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + ep_select*0x10000)       = data[0];
			*(volatile uint8_t *)(AVR32_USBB_SLAVE_ADDRESS + ep_select*0x10000 + 1)   = data[1];
		
			AVR32_USBB.UECON3CLR.fifoconc = 1;		
			AVR32_USBB.UESTA3CLR.nakinic = 1;

		}
	
	}	
}



