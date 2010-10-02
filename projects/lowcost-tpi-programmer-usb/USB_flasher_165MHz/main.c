/* Name: main.c
 * Project: hid-data, example how to use HID for data transfer
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: main.c 692 2008-11-07 15:07:40Z cs $
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/

#define USB_PUBLIC static

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

#include "usbdrv.c"

/*
	PB0 - D
	PB1 - C
	PB3 - Q
	PB5 - /S
*/

#define D_OUTPUT	DDRB|=(1<<DDB0);
#define D_LOW		PORTB&=255-(1<<PORTB0);
#define D_HIGH		PORTB|=(1<<PORTB0);
#define C_OUTPUT	DDRB|=(1<<DDB1);
#define C_LOW		PORTB&=255-(1<<PORTB1);
#define C_HIGH		PORTB|=(1<<PORTB1);
#define Q_INPUT	PORTB&=255-(1<<PORTB3);DDRB&=255-(1<<DDB3);
#define Q_READ		((PINB&(1<<PINB3)) != 0)
#define S_OUTPUT	DDRB|=(1<<DDB5);
#define S_LOW		PORTB&=255-(1<<PORTB5);
#define S_HIGH		PORTB|=(1<<PORTB5);

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

/* Since we define only one feature report, we don't use report-IDs (which
 * would be the first byte of the report). The entire report consists of 64
 * opaque data bytes.
 */
#define BUFFERSIZE 24
static uchar    reportBuffer[BUFFERSIZE+2];    /* buffer for HID reports, type game cntrler*/

PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, BUFFERSIZE,		     //   REPORT_COUNT (64)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};

/* The following variables store the status of the current data transfer */
static uchar    bytesRemaining;

static uchar	readingStatus;
static uchar	writingStatus;

static uchar	_osccal;
static uchar	delay_us = 1;

/* ------------------------------------------------------------------------- */

__attribute__((noinline))
void wait()
{
	uchar i=delay_us;
	while(i>0) {
		_delay_us(1);
		i--;
	}
}

__attribute__((noinline))
static void waitHIGHwait()
{
	wait();
	C_HIGH;
	wait();
}

__attribute__((noinline))
static void sendByte(uchar c)
{
	for(uchar i=0; i<8 && writingStatus>0; i++)
	{
		C_LOW;
		if (c&0b10000000)
		{
			D_HIGH;
		}
		else
		{
			D_LOW;
		}
		waitHIGHwait();

		c <<= 1;
		writingStatus--;
	}
}

//__attribute__((noinline))
static uchar recvByte()
{
	uchar res = 0;
	for(uchar i=0; (i<8) && (readingStatus>0); i++)
	{
		res <<= 1;

		C_LOW;
		waitHIGHwait();
		if (Q_READ)
		{
			res |= 1;
		}

		readingStatus--;
	}
	return res;
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
//__attribute__((noinline))
uchar usbFunctionWrite(uchar *data, uchar len)
{
//    if(bytesRemaining == 0)
//        return 1;               /* end of transfer */
	if(len > bytesRemaining)
	{
     	len = bytesRemaining;
	}

	uchar currentAddress = BUFFERSIZE-bytesRemaining;

	for(uchar i=0; i<len; i++)
	{
		uchar dat = data[i];
		reportBuffer[currentAddress] = dat;

		if ( readingStatus>0 )
		{
			reportBuffer[currentAddress] = recvByte();
		}
		else if (writingStatus==0xff )
		{
			switch ( dat )
			{
				case 0:
					S_LOW;
					break;
				case 1:
					S_HIGH;
					break;
				case 2:
					C_LOW;
					break;
				case 3:
					C_HIGH;
					break;
				case 4:
					D_LOW;
					break;
				case 5:
					D_HIGH;
					break;
				case 6:
					if ( Q_READ )
					{
						reportBuffer[currentAddress] = 1;
					}
					else
					{
						reportBuffer[currentAddress] = 0;
					}
					break;
				case 0x7f:
					reportBuffer[currentAddress] = delay_us;
					break;
				default:
					delay_us = dat-0x80;
			}
			wait();
			writingStatus=0;			
		}
		else if (writingStatus>0 )
		{
			sendByte(dat);
		}
		else
		{
			if ( dat==0b10000000 )
			{
				writingStatus=0xff;
			}
			else if (dat==0b00000000)
			{
				//nop
			}
			else if(dat&0b10000000)
			{
				writingStatus = (dat&0b01111111)+1;
			}
			else
			{
				readingStatus = dat + 1;//&0b01111111;
			}
		}

		currentAddress++;
	}

	bytesRemaining -= len;
	return bytesRemaining == 0; /* return 1 if this was the last chunk */
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* since we have only one report type, we can ignore the report-ID */
            //buildReport();
		  usbMsgPtr = reportBuffer;
            return BUFFERSIZE;
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = BUFFERSIZE;
		  readingStatus = 0;
		  writingStatus = 0;
            return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
        }
    }else{
        /* ignore vendor type requests, we don't use any */
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/
void usbEventResetReady(void)
{
#if 1
	uchar	optVal = 0;
	int		optDelta = 0x0fff;
	int		delta;
	int		targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
	uchar	i;
	uchar	j;
	if ( _osccal == 0 )
	{
		// first search for the right OSCCAL
		i = 0x20;
		j = 0x7c;
	}
	else
	{
		// subsequent corrections
		i = _osccal - 2;
		j = _osccal + 2;
	}

	for(; i<=j; i++)
	{
		OSCCAL = i;
		delta = usbMeasureFrameLength() - targetValue;
		if ( delta<0 )
		{
			delta = -delta;
		}
		if ( delta < optDelta )
		{
			optVal = i;
			optDelta = delta;
		}
	}

	_osccal = optVal;
	OSCCAL = optVal;
#else
uchar       step = 128;
uchar       trialValue = 0, optimumValue;
int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
#endif
}

/* ------------------------------------------------------------------------- */

// dont generate prologue/epilogue
__attribute__((naked))
void main(void)
{
uchar	i;
uchar	j;
	Q_INPUT;
	C_OUTPUT;
	D_OUTPUT;
	S_OUTPUT;
	C_HIGH;
	D_HIGH;
	S_HIGH;

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */

    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    odDebugInit();
    usbInit();
    // enforce re-enumeration, do this while interrupts are disabled!
    usbDeviceDisconnect();  
    // fake USB disconnect for > 250 ms
    i = 0;
    j = 1;
    while(--j){ // WTF?
    while(--i){             
        wdt_reset();
        _delay_ms(1);
    }
    }
    usbDeviceConnect();
    sei();
	// main event loop
    for(;;){                
        wdt_reset();
        usbPoll();
    }
}

/* ------------------------------------------------------------------------- */
