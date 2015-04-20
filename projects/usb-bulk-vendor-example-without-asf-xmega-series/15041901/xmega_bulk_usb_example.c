/*********************************************
* TCJ 4-2-15
*
* Example firmware to demo USB bulk out/in on ATxmega16c4
* 
*
*
*
*
*
* 
* Chip type           : atxmega16c4
* Clock frequency     : External crystal at 12 MHz, cpu clock at 24 MHz, peripheral clock at 24 MHz
* 						PLL produces 48 MHz for USB
*
* BROWNOUT DETECTOR (BOD):
*
* The BOD must be enabled; otherwise, residual charge in power supply capacitance will prevent a power-on
* reset at USB plug-in, which will prevent USB enumeration.  FUSE5 has been changed from default 0xff to 0xe9.
* This is BODACT = 10, BODLEVEL = 001 (2.8 volt threshhold).  (Also EESAVE = 1, the default)
*
*
*********************************************/
#include <avr/io.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#define F_CPU 24000000UL	// 24 MHz - this def needs to be before the delay.h include
#include <util/delay.h>		// not actually used

#define F_PER 24000000UL	// 24 MHZ - this is for USART
#define PRINT_USART 0
#include "usart_io.h"		// for serial io, printf

#include "descriptors.h"	// USB descriptors for this file


uint16_t ep0_out_data_p, ep0_in_data_p, ep1_out_data_p, ep1_in_data_p;


uint8_t arraycat(uint8_t numArrays, volatile uint8_t *p, ...)		// Fcn to concatenate series of arrays.  Takes as arguments: number of arrays, the (empty to start)
{																	//  result array, then the series of arrays in order.  Returns number of items in result array.
	uint16_t i, j, posit = 0;

	va_list args;            
	va_start (args, p);

	for(i = 0; i < numArrays; i++)
	{
		uint8_t* arr = va_arg(args,uint8_t*);
		for(j = 0; j < arr[0]; j++)
		p[posit++] = arr[j];
	}

	va_end(args);
	
	return posit;
}

void service_setup_packet(void)
{
	uint8_t i, j, tot;
	uint8_t data_in[8];									// the setup packet
	volatile uint8_t big_a[64] = {0};					// used to hold concatenated descriptor arrays

	USB_EP_t volatile * const ep0_out_p = (USB_EP_t *) (USB.EPPTR + 0); 	// create ptr to structure of type USB_EP_t at base address of config_block - for ep0 OUT
	USB_EP_t volatile * const ep0_in_p = (USB_EP_t *) (USB.EPPTR + 8); 		// create ptr to structure of type USB_EP_t 1 slot above base address of config_block - for ep0 IN

	sr_printf("got setup packet\n");

	for(i = 0; i < 8; i++)	 data_in[i] = *(volatile uint8_t *)(ep0_out_data_p + i);		
			
	sr_printf("setup packet:  %2x %2x %2x %2x %2x %2x %2x %2x \n", data_in[0],data_in[1],data_in[2],data_in[3],data_in[4],data_in[5],data_in[6],data_in[7] );

	if(data_in[0] == 0) 								// host-to-device
	{
		if(data_in[1] == 0)								//host has probably sent packet 0 0 0 0 0 0 0 0 asking to clear endpoint 0 stall (I don't think stall has occured though)
		{																									// so I'm not going to deal with it now!
			ep0_out_p->STATUS &= ~0x22;					// clear ep0 OUT STATUS TRNCMP0, BUSNACK0
			ep0_in_p->CNT = 0x0;						// put 0 in IN buffer count
			ep0_in_p->STATUS &= ~0x22;					// clear ep0 IN STATUS TRNCMP0, BUSNACK0
		}

		else if(data_in[1] == 5)						// set address
		{
			ep0_out_p->STATUS &= ~0x22;					// clear ep0 OUT STATUS TRNCMP0, BUSNACK0
			ep0_in_p->CNT = 0x0;						// put 0 in IN buffer count
			ep0_in_p->STATUS &= ~0x22;					// clear ep0 IN STATUS TRNCMP0, BUSNACK0

			while(!(ep0_in_p->STATUS & 0x20));			// wait for ep0 IN TRNCOMPL0 to go high

			USB.ADDR = data_in[2];						// set the address
		}
		
		else if(data_in[1] == 9)						// set configuration: it's time to configure endpoint 1 - but it's already done, so just acknowledge
		{
			ep0_out_p->STATUS &= ~0x22;					// clear ep0 OUT STATUS TRNCMP0, BUSNACK0
			ep0_in_p->CNT = 0x0;						// put 0 in IN buffer count
			ep0_in_p->STATUS &= ~0x22;					// clear ep0 IN STATUS TRNCMP0, BUSNACK0
		}

	}

	else if(data_in[0] == 0x80)							// device-to-host
	{
		if(data_in[1] == 6)								// get descriptor
		{
			if(data_in[3] == 1)							// get device descriptor
			{ 
				sr_printf("ep0 out count: %x\n", ep0_out_p->CNTL);
				sr_printf("ep0 out status: %x     ep0 in status: %x\n", ep0_out_p->STATUS, ep0_in_p->STATUS);
				
				for (i = 0;i < 18; i++)			*(volatile uint8_t *)(ep0_in_data_p + i) = des01[i];
				ep0_in_p->CNT = 18;
	
				ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
				ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

				sr_printf("sent device descriptor\n");
				sr_printf("ep0 out status: %x     ep0 in status: %x\n", ep0_out_p->STATUS, ep0_in_p->STATUS);
			}
		

			else if(data_in[3] == 2)					// get config descriptor
			{

				if(data_in[6] == 9)						// host wants just the config descriptor by itself
				{
					for (i = 0;i < 9; i++)			*(volatile uint8_t *)(ep0_in_data_p + i) = des02[i];
					ep0_in_p->CNT = 9;

					ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
					ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

					sr_printf("sent config descriptor\n");
			}

				else 									// host wants the config descriptor plus the others - note that this will need to be improved
				{										//													if there will be more than 64 bytes in config block
					tot = arraycat(4, big_a, des02, des03, des04, des05);  //string them all together

					for (i = 0;i < tot; i++)			*(volatile uint8_t *)(ep0_in_data_p + i) = big_a[i];
					ep0_in_p->CNT = tot;

					ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
					ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

					sr_printf("sent mass descriptors\n");
				}
			}

			else if (data_in[3] == 3)					// get string descriptor
			{
				if (data_in[2] == 0)
				{
					uint8_t darray[4] = {0x04, 0x03, 0x09, 0x04};															// language string

					for (i = 0;i < 4; i++)			*(volatile uint8_t *)(ep0_in_data_p + i) = darray[i];
					ep0_in_p->CNT = 4;

					ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
					ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in
				}

				else																										// send a real string
				{
					i = 0;
					j = 0;

					while(data_in[2] != indexorder[i++]);																	// determine which string to send
					i--;
											
					*(volatile uint8_t *)(ep0_in_data_p) = 2 + 2*(uint8_t)strlen(defined_strings[i]);

					*(volatile uint8_t *)(ep0_in_data_p + 1) = 3;

					for(j = 0; j < strlen(defined_strings[i]); j++)
					{
						*(volatile uint8_t *)(ep0_in_data_p + 2*j + 2) = defined_strings[i][j];

						*(volatile uint8_t *)(ep0_in_data_p + 2*j +3) = 0;
					}

					ep0_in_p->CNT = 2 + 2*(uint8_t)strlen(defined_strings[i]);

					ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
					ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in
				}

				sr_printf("sent string descriptor\n");
			}

			else if (data_in[3] == 6)							// get device qualifier - which is used only for high speed devices, so this request is pointless -
			{													// but providing a meaningless device qualifier keeps the host from repeating the request 3 times
				uint8_t qarray[10] = {0x0A, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00};

				for (i = 0;i < 10; i++)			*(volatile uint8_t *)(ep0_in_data_p + i) = qarray[i];
				ep0_in_p->CNT = 10;

				ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
				ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

				sr_printf("device qualifier requested - not pertinent\n");
			} 

			else 												// asking for descriptor type I don't support
			{
				ep0_in_p->CNT = 0;
				ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
				ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

				sr_printf("data_in[3] is not 1, 2, 3, or 6 - not a supported descriptor\n");
			}
		}

		else if(data_in[1] == 0)								// status request - not yet implented (not needed so far)
		{
			ep0_in_p->CNT = 0;
			ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
			ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

			sr_printf("status request - not yet implemented\n");
		}

		else
		{	
			ep0_in_p->CNT = 0;
			ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
			ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

			sr_printf("data_in[1] is not 0 or 6 - unimplemented get request\n");	
		}
 	}

	else if ((data_in[0] & 0x60) == 0x40)
	{
		ep0_in_p->CNT = 0;
		ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
		ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

		sr_printf("vendor request error - host driver shouldn't send this!\n");
	}

	else
	{
		ep0_in_p->CNT = 0;
		ep0_out_p->STATUS &= ~0x02;				// clear BUSNACK0 out
		ep0_in_p->STATUS &= ~0x02;				// clear BUSNACK0 in

 		sr_printf("screw up - setup packet: %x %x %x %x %x %x %x %x \n", data_in[0],data_in[1],data_in[2],data_in[3],data_in[4],data_in[5],data_in[6],data_in[7] );
	}
}




void service_trnif (void)
{
	// not needed
}

ISR(USB_BUSEVENT_vect)				// USB bus event vector - I'm not doing anything with these, doesn't seem to matter
{
	sr_printf("bus event interrupt\n");
	sr_printf("usb flags a: %x\n\n", USB_INTFLAGSASET);

	USB_INTFLAGSACLR |= 0x70;	// clear interrupt flags: suspend, resume, reset

} 

ISR(USB_TRNCOMPL_vect)				// USB transaction complete vector
{
	sr_printf("\ntransaction complete interrupt\n");
	
	if(USB_INTFLAGSBSET & 0x01)
	{
		USB_INTFLAGSBCLR = 0x01;		// clear setup transaction complete int flag SETUPIF
		sr_printf("setup transaction complete interrupt flag\n");
		service_setup_packet();	
	}
	
	if(USB_INTFLAGSBSET & 0x02)
	{
		USB_FIFOWP = 0;					// clears transaction complete interrupt flag TRNIF (not what the datasheet says: "USB_INTFLAGSBCLR = 0x02" doesn't work)
		sr_printf("transaction transaction complete interrupt flag\n");
		service_trnif();
	}
} 


int main(void)
{
	uint8_t data[2] = {0};								// used to get USB character data

	uint8_t i, gotch;

	setup_usart(0, 19200, 8, 'N', 1);

// clock setup

	OSC_XOSCCTRL = 0x8B;			// xtal freq range is 9-12 MHz; low power; high-freq. xtal
	OSC_CTRL |= 0x08;				// enable external xtal oscillator
	while(!(OSC_STATUS & 0x08));	// wait for oscillator to be stable

	OSC_PLLCTRL = 0xC4;				// PLL source is ext osc; PLL mult is 4
	OSC_CTRL |= 0x10;				// enable PLL
	while(!(OSC_STATUS & 0x10));	// wait for PLL to be stable

	CLK_USBCTRL = 0x01;				// enable USB clock source (48 MHz) [note this is mistakenly called CLK_USBSCTRL in datasheet]

	CCP = 0xD8;						// magic number into CCP register so next operation works
	CLK_PSCTRL = 0x04;				// prescaler A is divide by 2 - this gives CPU clock and periph clock of 24 MHz, after next operation

	CCP = 0xD8;						// magic number into CCP register so next operation works
	CLK_CTRL = 0x04;				// switch over to PLL as system clock source (48 MHz)



//	set up USB


	static uint8_t config_block[32] __attribute__((aligned(2))) = {0};	// grab 32 byte area, aligned to 2-byte word, for config table, which is 8 bytes each for:
																		// ep0 OUT, ep0 IN, ep1 OUT, ep1 IN
	USB.EPPTR = (uint16_t)(&config_block);								// tell USB where config table is


	sr_printf("config block start address: %x\n", USB.EPPTR);

	static uint8_t epdata_block[256]  = {0};								// grab 256 byte area for four 64 byte endpoints (ep0 OUT, ep0 IN, ep1 OUT, ep1 IN)

	ep0_out_data_p = (uint16_t)(&epdata_block) + 0;
	ep0_in_data_p  = (uint16_t)(&epdata_block) + 64;
	ep1_out_data_p = (uint16_t)(&epdata_block) + 128;
	ep1_in_data_p  = (uint16_t)(&epdata_block) + 192;

	USB_EP_t volatile * const ep0_out_p = (USB_EP_t *) (USB.EPPTR + 0); 		// create ptr to structure of type USB_EP_t at base address of config_block - for ep0 OUT

	ep0_out_p->CTRL = 0x43;									// control ep, 64 byte size
	ep0_out_p->DATAPTR = ep0_out_data_p;					// 64 byte data block at bottom of epdata_block


	USB_EP_t volatile * const ep0_in_p = (USB_EP_t *) (USB.EPPTR + 8); 		// create ptr to structure of type USB_EP_t 1 slot above base address of config_block - for ep0 IN

	ep0_in_p->CTRL = 0x43;									// control ep, 64 byte size
	ep0_in_p->DATAPTR = ep0_in_data_p;						// 64 byte data block 64 bytes above bottom of epdata_block (ep0 OUT is at 0; ep 0 IN is at 16, ep 1 OUT is at 32...)


	USB_EP_t volatile * const ep1_out_p = (USB_EP_t *) (USB.EPPTR + 16); 		// create ptr to structure of type USB_EP_t 2 slots above base address of config_block - for ep1 OUT

	ep1_out_p->CTRL = 0x83;									// bulk ep, 64 byte size
	ep1_out_p->DATAPTR = ep1_out_data_p;					// 64 byte data block 128 bytes above bottom of epdata_block


	USB_EP_t volatile * const ep1_in_p = (USB_EP_t *) (USB.EPPTR + 24); 		// create ptr to structure of type USB_EP_t 3 slots above base address of config_block - for ep1 IN

	ep1_in_p->CTRL = 0x83;									// bulk ep, 64 byte size
	ep1_in_p->DATAPTR = ep1_in_data_p;						// 64 byte data block 192 bytes above bottom of epdata_block



	USB_CTRLA = 0xC1;				// enable; full speed; max address is 1
	USB_CTRLB = 0x01;				// attach

	USB_INTCTRLA = 0x42;			// bus event interrupts enabled (suspend, resume, reset); interrupt level is medium

	USB_INTCTRLB = 0x03;			// transaction complete and setup transaction complete interrupts enabled

	PMIC_CTRL = 0x07;				// all interrupt levels enabled

	sei();							// start interrupts going

//------------------------------------------------


	while(1)
	{

		ep1_in_p->STATUS |= 0x02;		// set ep1 IN BUSNACK0 so host doesn't grab IN buffer before it's been written

		if(sr_kbhit())
		{
			sr_putchar(gotch = sr_getchar());   // rs232: echo character
		}

		if(ep1_out_p->STATUS & 0x20)				// is ep1 OUT TRNCOMPL0 set? 
		{
			data[0] =  *(volatile uint8_t *)(ep1_out_data_p);
			data[1] =  *(volatile uint8_t *)(ep1_out_data_p + 1);
			
			ep1_out_p->STATUS &= ~0x22;						// clear ep1 OUT STATUS TRNCMP0, BUSNACK0

			sr_printf("ep1 bytes: %d   %d\n", data[0], data[1]);
		}



		else if(ep1_in_p->STATUS & 0x40)				// is ep1 IN OVF set? 
		{
			ep1_in_p->STATUS &= ~0x40;					// clear  IN OVF	

			for (i = 0;i < 2; i++)			*(volatile uint8_t *)(ep1_in_data_p + i) = data[i];

			ep1_in_p->CNT = 2;
			
			ep1_in_p->STATUS &= ~0x02;				// clear IN BUSNACK0

			sr_printf("wrote to ep1 IN data buffer\n");
		}
	}

	return(0);
}
