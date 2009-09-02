//
//	Include Header
//
#include	<avr/io.h>
#include 	<avr/wdt.h>
#include	<avr/interrupt.h>
#include	<avr/pgmspace.h>
#include	<avr/boot.h>
#include 	<util/delay.h>     /* for _delay_ms() */
#include 	"usbdrv.h"


#define BOOTLOADER_CONDITION ((PINB & (1 << PB4)) == 1)

void (*jump_to_app)(void) = 0x0000;

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */
PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};

/* ------------------------------------------------------------------------- */
/* The following variables store the status of the current data transfer */

static uchar    currentAddress;
static uchar    bytesRemaining;
static uint16_t _Address;
static char	_dataFlag = 0;

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
/**
 * 受信したデータを書き込む
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{
	int	 i;
	uint16_t pos;

    if(bytesRemaining == 0)
        return 1;               /* end of transfer */

    if(len > bytesRemaining)
        len = bytesRemaining;

	if ( currentAddress == 0 && _dataFlag == 0 ) {

		_Address = *(data+1);
		_Address <<= 8;
		_Address |= *(data+2);
        cli();
        boot_page_erase(_Address); /* erase page */
        sei();
	}

	if ( _dataFlag != 0 ) {		// 1 or 2

		for ( i=0; i<len; i+=2 ) {
	        pos = currentAddress+i;			
			if ( _dataFlag == 2 ) {
				pos += 64;
			}
	        cli();
	        boot_page_fill(_Address+pos, *(short *)(data+i));
	        sei();
		}
	}

    currentAddress += len;
    bytesRemaining -= len;

	if ( bytesRemaining == 0 ) {

		_dataFlag ++;			// 次へ 1, 2
		if ( _dataFlag == 3 ) {
			cli();
            boot_page_write(_Address);
            sei();
			_dataFlag = 0;
		}
        boot_spm_busy_wait();
	}

	return (bytesRemaining == 0);
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
		if(rq->bRequest == USBRQ_HID_SET_REPORT){

            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 64;	//128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
        }
	}
    return 0;
}

/* ------------------------------------------------------------------------- */
void leaveBootloader() {
      cli();
      boot_rww_enable();
      MCUCR = (1 << IVCE);  /* enable change of interrupt vectors */
      MCUCR = (0 << IVSEL); /* move interrupts to application flash section */
      jump_to_app();
}

/**
 * メイン・プロシージャ
 */
int main(void)
{
	//uchar   i;

    /* initialize hardware */
    PORTB = 0xff;
    DDRB  = 0;

	//_delay_ms(1);
//    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    //DBG1(0x00, 0, 0);       /* debug output: main starts */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
	if ( (PINB & 0x10)  )
	{

	    MCUCR = (1 << IVCE);  /* enable change of interrupt vectors */
	    MCUCR = (1 << IVSEL); /* move interrupts to boot flash section */
	    usbInit();

		/*
	    usbDeviceDisconnect();  // enforce re-enumeration, do this while interrupts are disabled!
	    i = 0;
	    while(--i){             // fake USB disconnect for > 250 ms
	        wdt_reset();
	        _delay_ms(1);
	    }
	    usbDeviceConnect();
		*/
	    sei();

	//	_delay_ms(10);

	    //while ((PINB & 0x10) == 0x10) {
		while(1) {
	  
	//            wdt_reset();
	        usbPoll();

	    }
	}
    leaveBootloader();

    return 0;
}
