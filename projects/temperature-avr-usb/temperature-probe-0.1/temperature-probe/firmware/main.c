/* Temperature probe by Dan Stahlke, based upon the EasyLogger code:
 *     Project: EasyLogger
 *     Author: Christian Starkjohann
 *     Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 *     License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 *     This Revision: $Id: main.c 592 2008-05-04 20:07:30Z cs $
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>	 /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"		/* This is also an example for using debug macros */
#include "osccal.h"

// one byte reserved for oscillator calibration
#define CONFIG_TOP ((uint8_t *)1)
#define CONFIG_LEN 128

static struct {
	uint32_t accum;
	uint32_t samps;
} state_report;

static uint32_t adc_accum = 0;
static uint32_t adc_samps = 0;

static uchar currentAddress;
static uchar bytesRemaining;
static uchar report_id;

PROGMEM char usbHidReportDescriptor[42] = {	/* USB report descriptor */
	0x06, 0x00, 0xff,			// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,					// USAGE (Vendor Usage 1)
	0xa1, 0x01,					// COLLECTION (Application)
	0x15, 0x00,					//   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,			//   LOGICAL_MAXIMUM (255)
	0x75, 0x08,					//   REPORT_SIZE (8)

	// read state
	0x85, 0x00,					//   REPORT_ID (0)
	0x95, sizeof(state_report),	//   REPORT_COUNT
	0x09, 0x00,					//   USAGE (Undefined)
	0xb2, 0x02, 0x01,			//   FEATURE (Data,Var,Abs,Buf)

	// read/set admux
	0x85, 0x01,					//   REPORT_ID (1)
	0x95, 0x01,					//   REPORT_COUNT
	0x09, 0x00,					//   USAGE (Undefined)
	0xb2, 0x02, 0x01,			//   FEATURE (Data,Var,Abs,Buf)

	// read/set eeprom
	0x85, 0x07,					//   REPORT_ID (7)
	0x95, CONFIG_LEN,			//   REPORT_COUNT
	0x09, 0x00,					//   USAGE (Undefined)
	0xb2, 0x02, 0x01,			//   FEATURE (Data,Var,Abs,Buf)

	0xc0						// END_COLLECTION
};

/* ------------------------------------------------------------------------- */

uint32_t bswap_32(uint32_t in) {
	return
		((in & 0xff000000) >> 24) |
		((in & 0x00ff0000) >> 8) |
		((in & 0x0000ff00) << 8) |
		((in & 0x000000ff) << 24);
}

uint16_t bswap_16(uint16_t in) {
	return
		((in & 0xff00) >> 8) |
		((in & 0x00ff) << 8);
}

void buildStateReport() {
	state_report.accum = bswap_32(adc_accum);
	state_report.samps = bswap_32(adc_samps);
	adc_accum = 0;
	adc_samps = 0;
}

uchar usbFunctionRead(uchar *data, uchar len) {
    if(len > bytesRemaining) len = bytesRemaining;
    eeprom_read_block(data, CONFIG_TOP + currentAddress, len);
    currentAddress += len;
    bytesRemaining -= len;
    return len;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
	if(bytesRemaining == 0) return 1;
	if(len > bytesRemaining) len = bytesRemaining;
	if(report_id == 1) {
		if(currentAddress == 0) {
			ADMUX = data[0];
			adc_accum = 0;
			adc_samps = 0;
		}
	} else if(report_id == 7) {
		eeprom_write_block(data, CONFIG_TOP + currentAddress, len);
	}
	currentAddress += len;
	bytesRemaining -= len;
	return bytesRemaining == 0;
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){	/* HID class request */
		report_id = rq->wValue.bytes[0];
		if(rq->bRequest == USBRQ_HID_GET_REPORT){
			if(report_id == 0) {
				buildStateReport();
				usbMsgPtr = (void *)&state_report;
				return sizeof(state_report);
			} else if(report_id == 1) {
				usbMsgPtr = (void *)&ADMUX;
				return 1;
			} else if(report_id == 7) {
				bytesRemaining = CONFIG_LEN;
				currentAddress = 0;
				return USB_NO_MSG;  /* use usbFunctionRead() to receive data from host */
			} else {
				return 0;
			}
		} else if(rq->bRequest == USBRQ_HID_SET_REPORT){
			if(report_id == 1) {
				bytesRemaining = 1;
				currentAddress = 0;
				return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
			} else if(report_id == 7) {
				bytesRemaining = CONFIG_LEN;
				currentAddress = 0;
				return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
			}
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

static void myInit(void) {
	ADMUX  = 0x83; /* Vref=1.1V, measure ADC3 */
	ADCSRA = 0x87; /* enable ADC, not free running, interrupt disable, rate = 1/128 */
}

static void myPoll(void) {
	if(!(ADCSRA & _BV(ADSC))){
		if(adc_samps < 1048576) {
			adc_accum += ADC;
			adc_samps++;
		}
		ADCSRA |= _BV(ADSC);  /* start next conversion */
	}
}

/* ------------------------------------------------------------------------- */

void hadUsbReset(void) {
	calibrateOscillator();
	eeprom_write_byte(0, OSCCAL);   /* store the calibrated value in EEPROM */
}

int main(void) {
	uint8_t calibrationValue;

	calibrationValue = eeprom_read_byte(0); /* calibration value from last time */
	if(calibrationValue != 0xff){
		OSCCAL = calibrationValue;
	}

	wdt_enable(WDTO_1S);

	odDebugInit();
	myInit();
	usbInit();

	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	uint8_t i;
	for(i=0;i<20;i++){  /* 300 ms disconnect */
		_delay_ms(15);
	}
	usbDeviceConnect();

	sei();
	for(;;){				/* main event loop */
		wdt_reset();
		usbPoll();
		myPoll();
	}
	return 0;
}

/* ------------------------------------------------------------------------- */
