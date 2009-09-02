/* Name: main.c
 * Project: hid-data, example how to use HID for data transfer
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: main.c 692 2008-11-07 15:07:40Z cs $
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

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
/* Since we define only one feature report, we don't use report-IDs (which
 * would be the first byte of the report). The entire report consists of 128
 * opaque data bytes.
 */

/* The following variables store the status of the current data transfer */
static uchar    currentAddress;
static uchar    bytesRemaining;

/**
 * Prototypes
 */
char	_read_data();
void	cmnd8bit( char cDat );
void	data8bit( char cDat );
void	lcd_init();
void	lcd_sendstr( char *ptr );
void	putX( char cc );
void	putHex( char cc );
void	lcd_clear();
void	lcd_cursor( short sx, short sy ) ;
void	lcd_control( char cdata );

//=========================================================================================
#define	LED_Y_OFF()			sbi( PORTC, PC3 );
#define	LED_Y_ON()			cbi( PORTC, PC3 );
#define	LED_R_OFF()			sbi( PORTC, PC4 );
#define	LED_R_ON()			cbi( PORTC, PC4 );
#define	LED_B_OFF()			cbi( PORTC, PC5 );
#define	LED_B_ON()			sbi( PORTC, PC5 );

unsigned long  _nLedBlue   = 0;
unsigned long  _nLedRed    = 0;
unsigned long  _nLedYellow = 0;
unsigned long  _nLedFlag   = 0x01;

unsigned short _nTimerParam = 0xfc18;		// 50msec
int _nLedCycle = 0;							// 0-20


/* ------------------------------------------------------------------------- */

/* usbFunctionRead() is called when the host requests a chunk of data from
 * the device. 
 * For more information see the documentation in usbdrv/usbdrv.h.
 */
/**
 * 読み出し機能. <br/>
 *
 * 
 */
uchar   usbFunctionRead(uchar *data, uchar len)
{
	int i;

    if(len > bytesRemaining)
        len = bytesRemaining;
	
	if ( currentAddress < 32 ) {
		lcd_cursor( currentAddress, 0 );
		for ( i=0; i<len; i++ ) {
			*(data+i) = _read_data();
		}
	}
	else if ( currentAddress < 64 ) {
		lcd_cursor( currentAddress-32, 1 );
		for ( i=0; i<len; i++ ) {
			*(data+i) = _read_data();
		}
	}
	else {
		for ( i=0; i<len; i++ ) {
			*(data+i) = i;
		}
	}

    currentAddress += len;
    bytesRemaining -= len;

    return len;
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
/**
 * 受信したデータをLCD制御へ
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{
	volatile int	 i;
    if(bytesRemaining == 0)
        return 1;               /* end of transfer */
    if(len > bytesRemaining)
        len = bytesRemaining;

	for ( i=0; i<len; i++ ) {
		lcd_control( *(data+i) );
	}

    currentAddress += len;
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
            bytesRemaining = 64;	//128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionRead() to obtain data */
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */
            bytesRemaining = 64;	//128;
            currentAddress = 0;
            return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
        }
    }else{
        /* ignore vendor type requests, we don't use any */
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
//
//	Define定義
//
#define sbi(addr,bit)    asm("sbi %0,%1"::"I" _SFR_IO_ADDR(addr),"I" (bit));
#define cbi(addr,bit)    asm("cbi %0,%1"::"I" _SFR_IO_ADDR(addr),"I" (bit));


#define	RS_H()			sbi( PORTD, PD5 );
#define	RS_L()			cbi( PORTD, PD5 );
#define	RW_W()			cbi( PORTD, PD6 );
#define	RW_R()			sbi( PORTD, PD6 );
#define	ECL_H()			sbi( PORTD, PD7 );
#define	ECL_L()			cbi( PORTD, PD7 );

short	_curX;
short	_curY;

/**
 * LCD YM1602Cへのデータ出力. <br/>
 */
void	_put4bit( char cModeRS, char cDat )
{
	char cc;
	cc = cDat & 0x0f;
	PORTB = PORTB & 0xf0;

	RW_W();
	__asm volatile ("nop;");
	if ( cModeRS == 1 ) {
		RS_H();
	}
	else {
		RS_L();
	}
	__asm volatile ("nop;");
	ECL_H();
	__asm volatile ("nop;");

	PORTB |= cc;			// PB0-3 = DB4-7
	__asm volatile ("nop;");


	ECL_L();
	__asm volatile ("nop;");
	__asm volatile ("nop;");
	__asm volatile ("nop;");
	__asm volatile ("nop;");


	PORTB = PORTB & 0xf0;
	_delay_us(100);

	return;
}

/**
 * データ読み出し. <br>
 */
char	_read_data() {
	char cWork = 0;
	char cRead = 0;

	DDRB = 0xf0;
	__asm volatile ("nop");

	RW_R();

	RS_H();

	ECL_H();
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );

	cRead = PINB;
	ECL_L();
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );

	cRead &= 0x0f;
	
	ECL_H();
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );

	cWork = PINB;
	ECL_L();
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );
	__asm volatile ( "nop" );

	cWork &= 0x0f;

	cRead <<= 4;
	cRead |= cWork;

	DDRB = 0xff;

	_delay_us( 200 );

	return cRead;
}

void	cmnd4bit( volatile char cDat )
{
	_put4bit( 0, cDat );
}

void	cmnd8bit( char cDat )
{
	volatile char cc = (cDat & 0xf0);
	cc >>= 4;
	_put4bit( 0, cc );
	_put4bit( 0, (cDat & 0x0f) );
}

void	data8bit( volatile char cDat )
{
	_put4bit( 1, (cDat >> 4) );
	_put4bit( 1, (cDat & 0x0f) );
}

void	lcd_init()
{
	_curX = 0;
	_curY = 0;

	_delay_ms(15);
	// Function set
	cmnd4bit( 0x02 );
	_delay_ms( 5 );
	cmnd4bit( 0x02 );
	_delay_us( 100 );
	cmnd4bit( 0x0c );
	_delay_ms( 1 );
	// Display ON	: 38us
	cmnd8bit( 0x0c );	// Display on, Cursor off, Blinks off
	_delay_ms( 1 );
	// Display Clear: 1.52ms
	cmnd8bit( 0x01 );
	_delay_ms( 2 );
	// Entry Mode
	cmnd8bit( 0x06 );	// Incriment,dose not shift
	_delay_ms( 1 );
	cmnd8bit( 0x02 );	// Return home
	_delay_ms( 2 );
}

void	lcd_clear()
{
	cmnd8bit( 0x01 );
	_delay_ms( 2 );
	cmnd8bit( 0x02 );	// Return home
	_delay_ms( 2 );
	_curY = 0;
	_curX = 0;
}

/**
 * 文字列データ送信. <br>
 */
void	lcd_sendstr( char *ptr )
{
	while( *ptr != 0 ) {
		data8bit( *ptr );
		_delay_ms(1);
		ptr ++;
	}
}

void	putX( char cc ) {
	if ( cc<10 ) {
		data8bit( '0'+cc );
	}
	else {
		cc -=10;
		data8bit( 'A'+cc );
	}
}

void	putHex( char cc )  {
	unsigned char c = (cc & 0xf0);
	c >>= 4;
	putX( c );
	c = cc & 0x0f;
	putX( c );
}

/**
 * カーソル位置指定. <br>
 */
void	lcd_cursor( short sx, short sy ) 
{
	char data = 0x80;
	data |= (char)sx;
	data |= (char)(0x40 * sy);
	cmnd8bit( data );
}

volatile char	_lcdControlFlag;
volatile char   _ledColorData;
volatile char	_ledColorCount;

/**
 * ＬＣＤ制御部
 *
 *	制御コードを１バイト与え、続く１バイトデータの出力先を変える。<br/>
 *		(0x12) : 続く１バイトデータはコマンド出力
 *		(0x14) : 続く１バイトデータはデータ出力
 *		(0x16) : 続く１バイトがLED色、続く４バイトは点滅データ
 *		(0x03) : 画面をクリアする
 */
void	lcd_control( volatile char cdata )
{
	if ( _lcdControlFlag != 0 ) {
		switch( _lcdControlFlag ) {
			case 0x12:	// command put
				cmnd8bit( cdata );
				_lcdControlFlag = 0;
				break;
			case 0x14:	// data put
				data8bit( cdata );
				_lcdControlFlag = 0;
				break;
			case 0x16:
				if ( _ledColorCount == 4 ) {
					_ledColorData = cdata;		// 色番号格納
				}
				else {
					if ( _ledColorData == 1 ) {	// Blue
						_nLedBlue <<= 8;
						_nLedBlue |= cdata;
					}
					else if ( _ledColorData == 2 ) { // Red
						_nLedRed <<= 8;
						_nLedRed |= cdata;
					}
					else if ( _ledColorData == 3 ) { // Yellow
						_nLedYellow <<= 8;
						_nLedYellow |= cdata;
					}
				}

				if ( _ledColorCount == 0 ) {
					_lcdControlFlag = 0;
					_ledColorData = 0;

				}
				else {
					_ledColorCount --;
				}
		}
	}
	else {
		switch( cdata ) {
			case 0x03:
				lcd_clear();
				break;
			case 0x12:	// cmnd put
			case 0x14:	// data put
				_lcdControlFlag = cdata;
				break;
			case 0x16:	// LED color
				_lcdControlFlag = cdata;
				_ledColorCount = 4;
				break;
		}
	}
}

//=========================================================================================



/**
 * タイマ割り込み
 */
SIGNAL ( SIG_OVERFLOW1 )
{
	
	TCNT1 = _nTimerParam;

	if ( (_nLedFlag & _nLedBlue)== 0 ) {
		LED_B_OFF();
	}
	else {
		LED_B_ON();
	}

	if ( (_nLedFlag & _nLedRed)== 0 ) {
		LED_R_OFF();
	}
	else {
		LED_R_ON();
	}

	if ( (_nLedFlag & _nLedYellow)== 0 ) {
		LED_Y_OFF();
	}
	else {
		LED_Y_ON();
	}

	_nLedCycle ++;
	_nLedFlag <<= 1;
	if ( _nLedCycle >= 20 ) {
		_nLedCycle = 0;
		_nLedFlag = 0x01;
	}

	TIFR1  |= (1<<TOV1);		// オーバーフローフラグをクリア
	TIMSK1 |= (1<<TOIE1);		// オーバーフロー割り込み許可
}

/**
 * タイマ割り込み初期化
 */
void timer1_init( void )
{
	TCCR1A = 0;		// TCCR1A タイマモード
	TCCR1B = 5;		// プリスケーラ (1024分周)
	TCNT1 = _nTimerParam;

	TIFR1  |= (1<<TOV1);		// オーバーフローフラグをクリア
	TIMSK1 |= (1<<TOIE1);		// オーバーフロー割り込み許可
}


/**
 * メイン・プロシージャ
 */
int main(void)
{
	uchar   i;

	DDRC = 0x3f;
	DDRB = 0x0f;
	DDRD = 0xe0;
	PORTD = 0x00;
	PORTC = 0x38;
	PORTB = 0x00;

	lcd_init();
	lcd_sendstr( "MB-LCD03 ready." );
	_lcdControlFlag = 0;

	_nLedBlue = 0x01;

	timer1_init();
	sei();

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    DBG1(0x00, 0, 0);       /* debug output: main starts */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    odDebugInit();
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();

	_delay_ms(15);

    DBG1(0x01, 0, 0);       /* debug output: main loop starts */
    for(;;){                /* main event loop */
        DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
        wdt_reset();
        usbPoll();
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
