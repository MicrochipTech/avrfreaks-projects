#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "usbdrv.h"


/* ----------------------- hardware I/O abstraction ------------------------ */

/* pin assignments:
PB0	Key 1
PB1	Key 2
PB2	Key 3
PB3	Key 4
PB4	Key 5
PB5 Key 6

PC0	Key 7
PC1	Key 8
PC2	Key 9
PC3	Key 10
PC4	Key 11
PC5	Key 12

PD0	USB-
PD1	debug tx
PD2	USB+ (int0)
PD3	Key 13
PD4	Key 14
PD5	Key 15
PD6	Key 16
PD7	Key 17
*/

static void hardwareInit(void)
{
uchar	i, j;

    PORTB = 0x1f;   /* activate all pull-ups */
    DDRB = 0;       /* all pins input */
    PORTC = 0xff;   /* activate all pull-ups */
    DDRC = 0;       /* all pins input */
    //PORTD = 0xfa;
    PORTD = 0xf2;   /* 1111 1010 bin: activate pull-ups except on USB lines */
   // DDRD = 0x07;
    DDRD = 0x0F;    /* 0000 0111 bin: all pins input except USB (-> USB reset) */
	j = 0;
	while(--j){     /* USB Reset by device only required on Watchdog Reset */
		i = 0;
		while(--i); /* delay >10ms for USB reset */
	}
    DDRD = 0x02;    /* 0000 0010 bin: remove USB reset condition */
    /* configure timer 0 for a rate of 12M/(1024 * 256) = 45.78 Hz (~22ms) */
    TCCR0 = 5;      /* timer 0 prescaler: 1024 */
}

/* ------------------------------------------------------------------------- */

#define NUM_KEYS    15

/* The following function returns an index for the first key pressed. It
 * returns 0 if no key is pressed.
 */

uchar alastkey=0;
#define AKEY_POS 0
#define BKEY_POS 1
#define MUTE_POS 2

static uchar    keyPressed(void)
{
	// 0 - No key, 1 - Vol up, 2 - Vol down, 3 - Mute
	uchar   akey,bkey, x;

	x = PINB;

	if ((x & (1 << MUTE_POS)) == 0)
		return 3; 
	akey = ((x & (1 << AKEY_POS)) >> AKEY_POS); // PB0 =  AKey
	if (akey != alastkey)
	{
		alastkey=akey;
		bkey=((x & (1 << BKEY_POS)) >> BKEY_POS);// PB1 = BKey
		if(bkey != akey)
			return 1;
		else
			return 2;
	}
    
    	return 0;
}

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */
typedef struct
{
			uint8_t Modifier; /**< Keyboard modifier byte, indicating pressed modifier keys (a combination of
			                   *   \c HID_KEYBOARD_MODIFER_* masks).
			                   */
			uint8_t Reserved; /**< Reserved for OEM use, always set to 0. */
			uint8_t KeyCode[6]; /**< Key codes of the currently pressed keys. */
} __attribute__ ((packed)) USB_KeyboardReport_Data_t;

//USB_KeyboardReport_Data_t report;

//static uchar    reportBuffer[2];    /* buffer for HID reports */
static uchar    reportBuffer[8];
static uchar    idleRate;           /* in 4 ms units */
// 64 array elements instead of 35
const PROGMEM char usbHidReportDescriptor[] = {
(0x04 | 0x00 | 0x01) , ((0x01) & 0xFF), 
(0x08 | 0x00 | 0x01) , ((0x06) & 0xFF), 
(0x00 | 0xA0 | 0x01) , ((0x01) & 0xFF), 
(0x04 | 0x00 | 0x01) , ((0x07) & 0xFF), 
(0x08 | 0x10 | 0x01) , ((0xE0) & 0xFF), 
(0x08 | 0x20 | 0x01) , ((0xE7) & 0xFF), 
(0x04 | 0x10 | 0x01) , ((0x00) & 0xFF), 
(0x04 | 0x20 | 0x01) , ((0x01) & 0xFF), 
(0x04 | 0x70 | 0x01) , ((0x01) & 0xFF), 
(0x04 | 0x90 | 0x01) , ((0x08) & 0xFF), 
(0x00 | 0x80 | 0x01) , (((0 << 0) | (1 << 1) | (0 << 2)) & 0xFF), 
(0x04 | 0x90 | 0x01) , ((0x01) & 0xFF), 
(0x04 | 0x70 | 0x01) , ((0x08) & 0xFF), 
(0x00 | 0x80 | 0x01) , (((1 << 0)) & 0xFF), 
(0x04 | 0x00 | 0x01) , ((0x08) & 0xFF), 
(0x08 | 0x10 | 0x01) , ((0x01) & 0xFF), 
(0x08 | 0x20 | 0x01) , ((0x05) & 0xFF), 
(0x04 | 0x90 | 0x01) , ((0x05) & 0xFF), 
(0x04 | 0x70 | 0x01) , ((0x01) & 0xFF), 
(0x00 | 0x90 | 0x01) , (((0 << 0) | (1 << 1) | (0 << 2) | (0 << 7)) & 0xFF), 
(0x04 | 0x90 | 0x01) , ((0x01) & 0xFF), 
(0x04 | 0x70 | 0x01) , ((0x03) & 0xFF), 
(0x00 | 0x90 | 0x01) , (((1 << 0)) & 0xFF), 
(0x04 | 0x10 | 0x01) , ((0x00) & 0xFF), 
(0x04 | 0x20 | 0x02) , ((0xFF) & 0xFF) , 
((0xFF) >> 8 & 0xFF), (0x04 | 0x00 | 0x01) , 
((0x07) & 0xFF), (0x08 | 0x10 | 0x01) , 
((0x00) & 0xFF), (0x08 | 0x20 | 0x01) , 
((0xFF) & 0xFF), (0x04 | 0x90 | 0x01) , 
((6) & 0xFF), (0x04 | 0x70 | 0x01) , 
((0x08) & 0xFF), (0x00 | 0x80 | 0x01) , 
(((0 << 0) | (0 << 1) | (0 << 2)) & 0xFF), (0x00 | 0xC0 | 0x00)
};


#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

static const uchar  keyReport[NUM_KEYS + 1][2] PROGMEM = {
/* none */  {0, 0},                     /* no key pressed */
{0, 0x80}, // VOL UP 
{0, 0x81}, // VOL DOWN
{0, 0x7f}, // MUTE
///*  1 */    {0, KEY_A},
///*  2 */    {MOD_SHIFT_LEFT, KEY_B},
///*  3 */    {MOD_SHIFT_LEFT, KEY_C},
/*  4 */    {MOD_SHIFT_LEFT, KEY_D},
/*  5 */    {MOD_SHIFT_LEFT, KEY_E},
/*  6 */    {MOD_SHIFT_LEFT, KEY_F},
/*  7 */    {MOD_SHIFT_LEFT, KEY_G},
/*  8 */    {MOD_SHIFT_LEFT, KEY_H},
/*  9 */    {MOD_SHIFT_LEFT, KEY_I},
/* 10 */    {MOD_SHIFT_LEFT, KEY_J},
/* 11 */    {MOD_SHIFT_LEFT, KEY_K},
/* 12 */    {MOD_SHIFT_LEFT, KEY_L},
/* 13 */    {MOD_SHIFT_LEFT, KEY_M},
/* 14 */    {MOD_SHIFT_LEFT, KEY_N},
/* 15 */    {MOD_SHIFT_LEFT, KEY_O}
};

static void buildReport(uchar key)
{
/* This (not so elegant) cast saves us 10 bytes of program memory */
   // *(int *)reportBuffer = pgm_read_word(keyReport[key]);
   int data = pgm_read_word(keyReport[key]);
    ((USB_KeyboardReport_Data_t *)reportBuffer)->Modifier = data & 0x00FF;
   ((USB_KeyboardReport_Data_t *)reportBuffer)->KeyCode[0] = ((data & 0xFF00) >> 8);
  
}

uchar	usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    usbMsgPtr = reportBuffer;
        
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            buildReport(keyPressed());
            return sizeof(reportBuffer);
	    
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = &idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        /* no vendor specific requests implemented */
    }
	return 0;
}

/* ------------------------------------------------------------------------- */

int	main(void)
{
uchar   key, lastKey = 0, keyDidChange = 0;
uchar   idleCounter = 0;
uchar times=0;


	wdt_enable(WDTO_2S);
    hardwareInit();
	
	usbInit();
	sei();
    
	for(;;){	/* main event loop */
		wdt_reset();
		usbPoll();
		
		  key = keyPressed();
		  
		  if(lastKey != key)
		  {
		    lastKey = key;
		    keyDidChange = 1;
		  }
		
        if(TIFR & (1<<TOV0)){   /* 22 ms timer */
            TIFR = 1<<TOV0;
            if(idleRate != 0){
                if(idleCounter > 4){
                    idleCounter -= 5;   /* 22 ms in units of 4 ms */
                }else{
                    idleCounter = idleRate;
                    keyDidChange = 1;
                }
            }
        }
	  if(keyDidChange && usbInterruptIsReady())
	  {
            keyDidChange = 0;
            /* use last key and not current key status in order to avoid lost
               changes in key status. */
	   
            buildReport(lastKey);
	 
            usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
	       
	  }
	}
	return 0;
}

/* ------------------------------------------------------------------------- */
