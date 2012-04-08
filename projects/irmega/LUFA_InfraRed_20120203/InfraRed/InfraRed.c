/**************************************************************

             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.

***************************************************************

	IRmega USB Infrared Receiver/Transmitter
             firmware for MCU ATmega32U4

  Based on the Dean Camera's LUFA VirtualSerial.c demo

            (C) 2011 Anthony Vanek

    See the attached HTML document for details.

  SIMPLIFIED VERSION WITH UNMODULATED OUTPUT ONLY
            AND HEXADECIMAL DATA CODING

***************************************************************


 INTERNAL DATA FORMAT:

 Timers are configured to use 250kHz clock, so time scale is
 4 usec per bit. As maximal width value is 16 sec, we need just
 22 bits for data (2^22 = 4194304). This allows packing time
 value in 3 bytes = 24 bits, leaving two leftmost bits free for
 data type flag. The most significant bit (23) is used as
 pulse/space flag (1 for pulse, 0 for space), the second
 leftmost bit (22) is unused (always 0). Remaining 22 bits
 (21..00) contain time value in 4 usec units, max. 0x3D0900
 (16 sec)

 EXTERNAL DATA FORMAT:

 Data are transfered trough virtual serial port as characters.
 The 24-bit data values are coded as hexadecimal numbers
 (6 digits 0-9, A-F or a-f, no 0x prefix).
 Data type is coded as single character in the range '!' to '/'
 (0x21 to 0x2F) and always _f_o_l_l_o_w_s_ the corresponding
 data value. Assigned characters are:

 '+' pulse (both receive and transmit)
     Argument specifies pulse width in usec.

 '-' space (both receive and transmit)
     Argument specifies space width in usec.

 '#' watermark level (1 to 4095, default 1);
     Watermark suspends start of transmission until specified
     number of items are written into the TX ring buffer

 '*' wakeup code load (0 to 255, default 0);
     Device is switched to redirect mode, in which data items
     (pulses or spaces) are not transmitted, but saved in EEPROM
     as wakeup code patern. Device returns automatically back
     to normal transmission mode after number of items
     specified in argument is processed. Argument is saved
     to EEPROM as wakep code length.

 '&' time tolerance (10-4095, default 50);
     Argument is saved to EEPROM as time tolerance (in usec)
     for wakeup code comparision. 

 '$' buffers reset command.
     Initialize both RX and TX ring buffer (no data expected);


***************************************************************/

#include "InfraRed.h"
#include "stdlib.h"
#include <LUFA/Drivers/Misc/RingBuffer.h>


/* CONSTANTS */

#define BUFSIZE 1020   // each ring buffer capacity is 340 items (3*340 byte)

#define EE_WTOL 0x0002 // EEPROM adressing
#define EE_WCDL 0x0004
#define EE_WCDT 0x0020


/* Hex char to number conversion table (ASCII code as index expected);
   for 0-9 or A-F or a-f, table returns numeric equivalent 0-15, 0xFF othervise */

const uint8_t PROGMEM HexTab[256] = {
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};


/* STATIC VARIABLES */

static CDC_LineEncoding_t LineEncoding = {

	.BaudRateBPS = 0,
	.CharFormat  = CDC_LINEENCODING_OneStopBit,
	.ParityType  = CDC_PARITY_None,
	.DataBits    = 8
};

static uint16_t watermark;
static uint16_t TimeCntr;
static uint16_t WCDL;
static uint16_t WTOL;
static uint16_t EEoffset;
static bool txen;
static bool redir;

static uint8_t rxcntr;
static uint8_t txcntr;

static uint32_t rxbuf;
static uint8_t* rxbuf_ptr = (uint8_t*)&rxbuf;      // see rxbuf as byte array

static uint32_t txbuf;
static uint8_t* txbuf_ptr = (uint8_t*)&txbuf;      // see txbuf as byte array

static uint32_t rxdata;
static uint8_t* rxdata_ptr = (uint8_t*)&rxdata;    // see rxdata as byte array
static uint16_t* rxdata_wptr = (uint16_t*)&rxdata; // see rxdata as word array

static uint32_t txdata;
static uint8_t* txdata_ptr = (uint8_t*)&txdata;    // see txdata as byte array
static uint16_t* txdata_wptr = (uint16_t*)&txdata; // see txdata as word array

/* Circular buffer to hold data from the host before they are transmitted */

static RingBuffer_t TX_Buffer;
static uint8_t      TX_Buffer_Data[BUFSIZE];

/* Circular buffer to hold received data before they are sent to the host */

static RingBuffer_t RX_Buffer;
static uint8_t      RX_Buffer_Data[BUFSIZE];


/*****************************************************/
/* Configure the board hardware and chip peripherals */
/*****************************************************/

/*
	   Tested on the MattairTech MT-DB-U4 module
	     (ATmega32U4 MCU, 16 MHz clock)
*/ 

void SetupHardware(void) {

	/* Disable watchdog if enabled by bootloader/fuses */

	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */

	clock_prescale_set(clock_div_1);

	/* Initialise output pins */

	DDRD = (1<<PD7) | (1<<PD0); // configure PD7, PD0 as output
	DDRC = (1<<PC6);            // configure PC6 as output (unmodulated out)
	PORTC = (1<<PC6);           // switch output high (space)

	/* Set default watermark */

	redir = false;
	watermark = 1;
	TimeCntr = 3;  // preset counter to 3 so the LED stays off 

	/* Set WCDL and WTOL */
 
	WTOL = eeprom_read_word ( (uint16_t*)EE_WTOL ); // wakeup tolerance
	WCDL = eeprom_read_word ( (uint16_t*)EE_WCDL ); // wakeup code length 

	if ((WTOL<10) || (WTOL>4095)) { // unacceptable value, set defaults 
	   WTOL=50; WCDL=0;
	}


	/* Initialise TX timer T1 */

	/* Timer is set to 250kHz clock (1:64 prescaler), CTC mode.
	   Before timer is enabled, the extended (24-bit) timer counter is set
	   to the pulse/space width value divided by 4 (>>2 shift). While
	   OCR1A register is set to low 16 bits, txcntr is set to high 8 bits
	   of the value. Both overflow and compare match interrupts are enabled.
	   On every overflow interrupt, the txcntr is decremented by 1;
	   on compare match interrupts, the txcntr is checked for zero.
	   If txcntr=0, interrupt is accepted and pulse/space ends */

	TCCR1B = 0;          // stop timer
	TCCR1A = 0;          // normal mode 

	/* Set transmitter to pause mode. In this mode, 10 msec wide spaces
	   are transmitted, so the TX ring buffer is examined for new data
	   every 10 msec */

	OCR1A = 2500;        // 10 msec space
	txcntr = 0;

	/* enable compare match and overflow interrupts */
	TIMSK1 = (1<<OCIE1A)|(1<<TOIE1);

 	txen = false;        // disable transmission
	TCCR1B = (3<<CS10);  // start timer, 1:64 prescaler

	/* Initialise RX timer T3 */

	/* Timer is set to 250kHz clock (1:64 prescaler), normal mode, capture
	   input ICP3, noise canceler enabled. Intially, falling (negative)
	   edge is selected so edge detection will start at the beggining of
	   the next pulse (active low input expected). On each capture
	   interrupt, capture edge is toggled */

	TCCR3B = 0;            // stop timer
	TCNT3  = 0;            // reset timer counter
	TCCR3A = 0;            // normal mode
 
	/* noise canceler on, capture on falling edge, 1:64 prescaler */ 
	TCCR3B = (1<<ICNC3)|(3<<CS30);

	/* enable input capture and overflow interrupts */ 
	TIMSK3 = (1<<ICIE3)|(1<<TOIE3);

	/* Initialise USB */	
	USB_Init();
}


/*********************************************/
/* Timer1 overflow interrupt service routine */
/*********************************************/

ISR(TIMER1_OVF_vect, ISR_BLOCK) {

	/* On T1 overflow interrupt, the 8-bit software couter txcntr
	   is decremented by 1. As timer clock period is 4 usec, this
	   can occur just if width is greater than 4*65526 ~ 0.25 sec */

	txcntr -= 1;  // decrement counter
}


/*****************************************************/
/* Timer1 interrupt on compare match service routine */
/*****************************************************/

ISR(TIMER1_COMPA_vect, ISR_BLOCK) {

	/* On every compare match interrupt, the txcntr is checked for zero.
	   Interrupt is accepted as end of pulse/space only if txcntr=0 */

	if (txcntr==0) {

		/* reset timer counter */
		TCNT1 = 0;

		/* Stop transmission if no data */
		if (RingBuffer_GetCount(&TX_Buffer)<3) txen = false;

		if (txen) {

			/* set timer counter */
			txdata_ptr[0] = RingBuffer_Remove(&TX_Buffer); // LSB
			txdata_ptr[1] = RingBuffer_Remove(&TX_Buffer);
			txdata_ptr[2] = RingBuffer_Remove(&TX_Buffer); // MSB

			/* check data type bit */
			if (txdata_ptr[2]&0x80) PORTC &= ~(1<<PC6); // set PC6 low
			   else PORTC |= (1<<PC6);                  // set PC6 high

			OCR1A = txdata_wptr[0]; // low 16 bits to compare register
			txcntr = (txdata_ptr[2] & 0x3F); // high 6 bits to txcntr
			TimeCntr = 0; // clear counter if TX active

		} else { // transmitter locked, stay in pause mode

			PORTC |= (1<<PC6);  // set PC6 high (space)
			OCR1A = 2500;       // 10 msec space
			txcntr = 0;
			TimeCntr += 1;    // count time spent in pause mode
		}

	}
}


/*********************************************/
/* Timer3 overflow interrupt service routine */
/*********************************************/

ISR(TIMER3_OVF_vect, ISR_BLOCK) {

	/* On timer1 overflow interrupt, the 8-bit software couter rxcntr
	   is incremented by 1. As timer clock period is 4 usec, this occures
	   once per 262144 usec(~0.26 sec). With 8-bit software extension,
	   time intervals up to ~67 sec can be measured. As we need measurement
	   range just up to 16 sec (3D 0900 in hex), we limit the software
	   counter to 0x3F maximum. If rxcntr reaches the 0x3F, it stays at
	   this value until next signal edge arrives */

	if (rxcntr<0xFF) rxcntr += 1;  // increment counter until 0xFF
}


/***********************************************/
/* Timer3 interrupt on capture service routine */
/***********************************************/

ISR(TIMER3_CAPT_vect, ISR_BLOCK) {

	/* The 22-bit time value in timer periods (Tc=4usec) is saved into rxdata.
	   Then both TCNT1 and rxcntr are reset and the capture edge is toggled */

	rxdata_wptr[0] = ICR3;
	rxdata_ptr[2] = rxcntr;

	TCNT3 = 0;            // reset hardware timer counter
	rxcntr=0;             // reset overflow counter
	TCCR3B ^= (1<<ICES3); // toggle capture edge selection bit

	rxdata += 1;  // heuristic correction +4 usec

	/* check value for 16 sec limit and cut if greater */ 

	if (rxdata>4000000) rxdata = 4000000;

	/* set pulse/space flag */

	if ((TCCR3B &(1<<ICES3))==0) rxdata_ptr[2] |= 0x80;

	/* check buffer for free space */

	if ((RingBuffer_GetFreeCount(&RX_Buffer)-1)>3) { // write data to buffer

		RingBuffer_Insert(&RX_Buffer, rxdata_ptr[0]); // LSB
		RingBuffer_Insert(&RX_Buffer, rxdata_ptr[1]);
		RingBuffer_Insert(&RX_Buffer, rxdata_ptr[2]); // MSB

	}  // else ignore input until free space available

	TimeCntr = 0; // reset counter if RX active

}


/*******************************************/
/* Event handler for the USB_Connect event */ 
/*******************************************/

/* This indicates that the device is enumerating */

void EVENT_USB_Device_Connect(void) {

	/* Indicate USB enumerating */
//	PORTD |= (1<<PD7);  // start of enumeration = LED on
}

/**********************************************/
/* Event handler for the USB_Disconnect event */
/**********************************************/

/* This indicates that the device is no longer connected to a host */

void EVENT_USB_Device_Disconnect(void) {

	/* Indicate USB not ready */
//	PORTD &= ~(1<<PD7);  // switch LED off
}

/********************************************************/
/* Event handler for the USB_ConfigurationChanged event */
/********************************************************/

/* This is fired when the host set the current configuration of
   the USB device after enumeration. The device endpoints are
   configured and the CDC management task started */

void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	/* Setup CDC Data Endpoints */

	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM,
		EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
		ENDPOINT_BANK_SINGLE);

	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
		ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);

	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
		ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);

	/* Reset line encoding baud rate so that the host knows to send
	   new values */

	LineEncoding.BaudRateBPS = 0;

	/* Indicate endpoint configuration success or failure */

//	if (ConfigSuccess) PORTD &= ~(1<<PD7); // end of configuration = LED off
}

/**************************************************/
/* Event handler for the USB_ControlRequest event */
/**************************************************/

/* This is used to catch and process control requests sent to
   the device from the USB host before passing along unhandled
   control requests to the library for processing internally */

void EVENT_USB_Device_ControlRequest(void) {

	/* Process CDC specific control requests */

	switch (USB_ControlRequest.bRequest) {

	   case CDC_REQ_GetLineEncoding:

	      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST |
					REQTYPE_CLASS | REQREC_INTERFACE)) {
	         Endpoint_ClearSETUP();

	         /* Write the line coding data to the control endpoint */

	         Endpoint_Write_Control_Stream_LE(&LineEncoding,
						sizeof(CDC_LineEncoding_t));
	         Endpoint_ClearOUT();
	      }
	      break;

	   case CDC_REQ_SetLineEncoding:

	      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE |
					REQTYPE_CLASS | REQREC_INTERFACE)) {
	         Endpoint_ClearSETUP();

	         /* Read the line coding data in from the host into the global struct */

	         Endpoint_Read_Control_Stream_LE(&LineEncoding,
						sizeof(CDC_LineEncoding_t));
	         Endpoint_ClearIN();
	      }
	      break;

	   case CDC_REQ_SetControlLineState:

	      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE |
					REQTYPE_CLASS | REQREC_INTERFACE)) {
	         Endpoint_ClearSETUP();
	         Endpoint_ClearStatusStage();
	      }
	      break;
	}
}


/*
	======================================================
	#  CDC data transmission/reception to/from the host  #
	======================================================
*/

void CDC_Task(void) {

	/* Device must be connected and configured for the task to run */

	if (USB_DeviceState != DEVICE_STATE_Configured) return;


	/*********** RECEIVE DATA FROM HOST ***********/


	Endpoint_SelectEndpoint(CDC_RX_EPNUM); // Select the Serial Rx Endpoint

	if (Endpoint_IsOUTReceived()) { // Check to see if any data available

		uint16_t n, eedata;
		uint8_t ch;
		uint16_t e = Endpoint_BytesInEndpoint();
		   
		for (n=0; n<e; n++) { 

		   Endpoint_Read_Stream_LE(&ch, 1, NULL); // get character

		   uint8_t digit = pgm_read_byte(&HexTab[ch]);
		   if (digit<16) { // hex character

		      txbuf = (txbuf<<4) | digit; // convert digits to number

		   } else { // maybe flag character?

		      txbuf &= 0x00FFFFFF; // mask out unused txbuf bits
		      switch (ch) {

		         case '+': // pulse

				   txbuf = (txbuf>>2); // divide value by 4

				   if (redir) { // write data to EEPROM
				      eedata = (uint16_t)(txbuf & 0xFFFF);
				      eeprom_write_word ((uint16_t*)(EE_WCDT+EEoffset), eedata);
				      EEoffset +=2;
				      if (EEoffset>2*WCDL) redir=false;
				   break;
	
				} else {

				   txbuf_ptr[2] |= 0x80; // set pulse flag

				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[0]); //LSB
				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[1]);
				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[2]); //MSB

				   /* watermark value defines how many items in the
				      TX ring buffer are enough for transmitter
				      activation. As every data item takes 3 bytes,
				      we multiply watermark value by 3 and decrement
				      result by 1. Example: if watermark=5,
				      transmission will start after 15-th byte is
				      written into buffer (3*5-1=14) */

				   if (RingBuffer_GetCount(&TX_Buffer) > (3*watermark-1) )
				      txen = true; // enable transmitter
				   break;
				}

		         case '-': // space
				   txbuf = (txbuf>>2); // divide value by 4

				   if (redir) { // write data to EEPROM 

				      eedata = (uint16_t)(txbuf & 0xFFFF);
				      eeprom_write_word ((uint16_t*)(EE_WCDT+EEoffset), eedata);
				      EEoffset +=2;
				      if (EEoffset>2*WCDL) redir=false;
				   break;

				} else {


				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[0]); // LSB
				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[1]);
				   RingBuffer_Insert(&TX_Buffer, txbuf_ptr[2]); // MSB
				   break;
				}

		         case '*': // set wakeup code

				/* data part (0-255 allowed) is saved in EEPROM
				   as WCDL and device is switched to redirection
				   mode. Specified number of data items (pulses
				   '+' or spaces '-') following this command are
				   saved in EEPROM as WCode[WCDL] array.
				   After specified number of items is saved,
				   the device returns automatically back to
				   normal/ transmission mode */

				/* set and save WCDL */
				if (txbuf>0x00FF) WCDL = 0x00FF;
				else WCDL = txbuf;
				eeprom_write_word ((uint16_t*)EE_WCDL, WCDL);

				/* set transmit redirection mode */
				redir = true;
				EEoffset = 0;
				break;

		         case '&': // set wakeup time tolerance

				/* data part (10-4095 allowed) is saved
				in EEPROM as WTOL value (in usec) */

				/* set and save WTOL */
				if (txbuf>0x0FFF) WTOL = 0x0FFF;
				else WTOL = txbuf;
				eeprom_write_word ((uint16_t*)EE_WTOL, WTOL);
				break;

		         case '#': // set watermark

				/* cut watermark value to max. 4095 */
				if (txbuf>0x0FFF) watermark = 0x0FFF;
				else watermark = txbuf; 
 
				/* as watermark value changed, it is necessary
				   to check ring buffer again */

				if (RingBuffer_GetCount(&TX_Buffer) > (3*watermark-1) )
				   txen = true;  // enable transmitter
				break;

		         case '$': // reset (data value ignored)

				RingBuffer_InitBuffer(&TX_Buffer,
					TX_Buffer_Data, sizeof(TX_Buffer_Data));
				RingBuffer_InitBuffer(&RX_Buffer,
					RX_Buffer_Data, sizeof(RX_Buffer_Data));
				break;
		      } // end of switch

		   }  // end of char processing; unknown chars ignored!
		   
		} // end of FOR loop	

		/* Finalize the stream transfer to send the next packet */
		Endpoint_ClearOUT();
	}


	/*********** SEND DATA TO HOST ***********/


	   if (!RingBuffer_IsEmpty(&RX_Buffer)) {  // Ring buffer not empty

		Endpoint_SelectEndpoint(CDC_TX_EPNUM); // Select the Serial Tx Endpoint

		uint16_t BufCnt = RingBuffer_GetCount(&RX_Buffer);

		/* At least 3 bytes are needed for processing */

		while (BufCnt>2) {

			/* The 22-bit data are buffered as 3 bytes, Little Endian first.
			   The leftmost bit of the MSB is pulse-space bit (1 for pulse,
			   0 for space) */

			rxbuf_ptr[0] = RingBuffer_Remove(&RX_Buffer); // get LSB byte
			rxbuf_ptr[1] = RingBuffer_Remove(&RX_Buffer); // get next byte
			rxbuf_ptr[2] = RingBuffer_Remove(&RX_Buffer); // get MSB byte
			BufCnt -= 3;  // decrement counter by 3

			/* extract pulse-space bit and set the proper data type
			   flag character ('+' for pulse, '-' for space) */

			uint8_t code[8];
			if (rxbuf_ptr[2]&0x80) code[6]='+'; else code[6]='-';

			/* multiply data by 4 */

			rxbuf = (rxbuf<<2);

			/* convert data to hex */

			uint8_t HEX[] = "0123456789ABCDEF";

			code[5] = HEX[ rxbuf_ptr[0]&0x0F ];
			code[4] = HEX[ rxbuf_ptr[0]>>4   ];
			code[3] = HEX[ rxbuf_ptr[1]&0x0F ];
			code[2] = HEX[ rxbuf_ptr[1]>>4   ];
			code[1] = HEX[ rxbuf_ptr[2]&0x0F ];
			code[0] = HEX[ rxbuf_ptr[2]>>4   ];

			 /* Send code */

			Endpoint_Write_Stream_LE(&code, 7, NULL);
		}

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();

		/* Wait until the endpoint is ready for the next packet */
		Endpoint_WaitUntilReady();

		/* Send an empty packet to prevent host buffering */
		Endpoint_ClearIN();
	}
}


void Wakeup_Task(void) {

	if (USB_DeviceState == DEVICE_STATE_Suspended) {

	   if (WCDL==0) {

	      /* Value 0 is interpreted as wakeup by any code. In order
		 to protect system against randomly fired wakeups, minimum
		 8 items in the buffer is required. Note system will wake
		 up by (possibly) ANY code from (allmost) ANY remote in
		 this case! */

	      if (RingBuffer_GetCount(&RX_Buffer)>8) {
		 TimeCntr = 0; // signalise match by LED
		 USB_Device_SendRemoteWakeup();
		 RingBuffer_InitBuffer(&RX_Buffer, RX_Buffer_Data, sizeof(RX_Buffer_Data));
	      }	
	      return; // from Wakeup_Task
	   }

	   if (RingBuffer_GetCount(&RX_Buffer) >= 3*WCDL) {
	      /* have enough data in RX buffer so we can check code for match */

	      uint16_t j=0;
	      uint16_t k=0;
	      uint16_t BufCnt = RingBuffer_GetCount(&RX_Buffer);
	      uint16_t WCode;

	      while (BufCnt>2) {

		/* The 22-bit data are buffered as 3 bytes, Little Endian first.
		   The leftmost bit of the MSB is pulse-space bit (1 for pulse,
		   0 for space) */ 

		rxbuf_ptr[0] = RingBuffer_Remove(&RX_Buffer); // get LSB byte
		rxbuf_ptr[1] = RingBuffer_Remove(&RX_Buffer); // get next byte
		rxbuf_ptr[2] = RingBuffer_Remove(&RX_Buffer); // get MSB byte
		BufCnt -= 3;

		/* mask out pulse/space bit */
		rxbuf &= 0x3FFFFF;

		/* if width>50000 (~200 msec) the Wakeup_Task ends, leaving rest
		   of data in RX buffer untouched */

		if (rxbuf>50000) return;

		/* WCode table is saved in EEPROM (max. 256 words allowed).
		   We use just 16-bit words here as we expect pulse/space width
		   is less than 50000*4 usec = 200 msec. Code length is set by
		   WCDL (max. 255, 0 means any code). Tolerance window is set
           by WTOL (default +/- 50 usec) */

		if (j<WCDL) { // compare

		   /* After j reaches WCDL value, any additional data are read
		      but ignored. Note that j is incremented unconditionally,
		      while k only if match occures. At the end, k==j indicates
		      that all checked items match */

		   WCode = eeprom_read_word ( (uint16_t*)(EE_WCDT+(j<<1)) );

		   if ( (rxbuf<(WCode+(WTOL>>2)))
			&& (rxbuf>(WCode-(WTOL>>2))) ) k+=1;
		   j += 1;
		}

	      } // RX buffer processed

	      if (k==j) {
		 TimeCntr = 0; // signalise match by LED
		 USB_Device_SendRemoteWakeup();
	      }

	   } // else no data to check
	}
}


void LED_Task(void) {

	/* This task controls LED signaling. If the TimeCntr counter is reset,
	   this task switches LED on. If TimeCntr reaches 20 (no activity for
	   200 msec), LED is switched off. Timer is reset periodically if
	   RX or TX (or both) are active. Timer clock period is 10 msec 
	   but timing is not precise as timer does not count if TX is busy */

	if (TimeCntr<3) PORTD |= (1<<PD7);        // RX or TX active, LED on
	else if (TimeCntr>20) PORTD &= ~(1<<PD7); //  no activity, LED off
}


/*======================== Main program entry point ========================*/


int main(void) {

	SetupHardware();

	RingBuffer_InitBuffer(&TX_Buffer, TX_Buffer_Data, sizeof(TX_Buffer_Data));
	RingBuffer_InitBuffer(&RX_Buffer, RX_Buffer_Data, sizeof(RX_Buffer_Data));

	sei();

	for (;;) {
		CDC_Task();
		USB_USBTask();  // external
		Wakeup_Task();
		LED_Task(); 
	}
}

