/*
**	Orangutan (ATmega168) demo for I2C and Devantech SRF08
**
**	Steven G. Judd, Tentacle Robotics
**	sgjudd@gmail.com
**
**	V0-00	Feb-1-2007

*/

#include	"device.h"

#include	"stdio.h"

#include	"util/delay.h"

// Orangutan support files
//
// Note: this program calls alternate version of the LCD code to get around a
// still-unresolved timing problem when using I2C.

#include	"bitfield.h"
#include	"lcd2.h"
#include	"srf08.h"

	int main(void);
	void init(void);
	void delay_secs(unsigned char);


#ifndef _ENABLE_LCD_
#define lcd_init()
#define lcd_line1()
#define lcd_line2()
#define lcd_string(s)
#define lcd_clear()
#define lcd_hide()
#endif

int main(void) {

	unsigned char srf_addr = SRF08_UNIT_5;	// or whatever unit you are using

	unsigned char ret;

	union 	{ int ints[17];
			unsigned char bytes[34];
			} uRange;

	unsigned char 	ucLight, i;
	char			lcd_buf[8];

// Begin

    // Make sure all our registers are clear
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;

    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
			
	lcd_init();
	lcd_clear();
	lcd_hide();
	lcd_line1();
	lcd_string("INIT");
	delay_secs(2);
	lcd_clear();	

	init();	// complete initialization

	lcd_line1();
	lcd_string("SRF08");
	delay_secs(2);
	

//
// Uncomment this section to test address reset
//
/*  
	lcd_line1();
	lcd_string("Set Adr");


	ret=srf08_setAddr(srf_addr, SRF08_UNIT_4);
	lcd_line2();
	if (ret==0) {
		lcd_string("Adr OK");
	} else {
		lcd_string("Adr FAIL");
	}
	delay_secs(5);



	lcd_line1();
	lcd_string("Set Adr2");

	ret=srf08_setAddr(SRF08_UNIT_4, srf_addr);
	lcd_line2();
	if (ret==0) {
		lcd_string("Adr2 OK");
	} else {
		lcd_string("Adr2 FAIL");
	}

*/


	lcd_line1();
	lcd_string("Set Rng");
	lcd_line2();

	ret = srf08_setRange(srf_addr, SRF08_MAX_RANGE);	// set range = max
	if (ret==0) {
		lcd_string("Rng OK");
	} else {
		lcd_string("Rng FAIL");
	}
		
	delay_secs(2);

//
// Uncomment to set gain
//

	lcd_line1();
	lcd_string("Set Gain");
	lcd_line2();

	ret = srf08_setGain(srf_addr, SRF08_MAX_GAIN);	// set gain = 31 (default)
	if (ret==0) {
		lcd_string("Gain OK");
	} else {
		lcd_string("Gain FAIL");
	}
	delay_secs(2);

	lcd_line1();
	lcd_string("Ping.   ");
	lcd_line2();

	ret=srf08_ping(srf_addr,RANGE_CM);

	if (ret==0) {
		lcd_string("Ping OK");
	} else {
		lcd_string("Png FAIL");
	}

	delay_secs(2);

	lcd_line1();
	lcd_string("Echoes. ");
	lcd_line2();

	ret=srf08_echo(srf_addr,sizeof(uRange.ints),uRange.ints);

	if (ret==0) {
		lcd_string("Echo OK");
		for (i=0;i<(sizeof(uRange.ints)/2);i++) {
			sprintf(lcd_buf,"%02d:%03dCM",i,uRange.ints[i]);
			delay_secs(1);
			lcd_line2();
			lcd_string(lcd_buf);
		}
	} else {
		lcd_string("Ech FAIL");
	}

	delay_secs(2);
	lcd_line1();
	lcd_string("Light.  ");
	lcd_line2();

	ret=srf08_light(srf_addr,&ucLight);
	if (ret==0) {
		lcd_string("Light OK");
		sprintf(lcd_buf,"%03d     ",ucLight);
		lcd_line2();
		lcd_string(lcd_buf);
	} else {
		lcd_string("Lit FAIL");
	}

	delay_secs(2);
	lcd_clear();
	lcd_line1();
	lcd_string("Done.   ");
	return(0);
}

void init() 
{
	// set pull-ups on PortC Pins 4 and 5
	// If the i2c bus has external 4.7k pull-ups on SDA and SCL this is not necessary.
	//

	_DDRC.B4 	= 0;
	_DDRC.B5 	= 0;
	_PORTC.B4	= 1;
	_PORTC.B5	= 1;

	i2c_init();
}

void delay_secs(unsigned char secs) {
	unsigned char i, j;
	for (i=0;i<secs;i++) {
		for (j=0;j<10;j++) {	
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
		}
	}
}


