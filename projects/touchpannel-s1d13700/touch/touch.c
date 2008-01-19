//************************************************************************
// For 4 wire resistive touch screens
// Functions for reading position of touchscreen press
// uses ADC of Mega Prozessor for calculating the position
// Klaus Meinhardt  created 12.01.2008
//************************************************************************
// Declaration of 32 buttons with start point end end point 
// Resolution of point is 2 pixel (for screen with maximal 512x512 pixel)
//************************************************************************


#include <util\delay.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include "a2d.h"
#include "global.h"
#include "touch.h"
#include "s1D13700.h"

#define TOUCH_MIN 10 // Minimum adc reasult for touch press is recognized
#define MAXBUTTON 32
#define TS_VERSION 110		// version of Settings Data

point_t touch_pos;			// the result of the x and y position of called function read_touch
point_t adc_min; 
ctouch_t touch;							// configuration data se typedef
ctouch_t dummy;

uint8_t ee_version __attribute__ ((section (".eeprom")));
ctouch_t  ee_touch __attribute__ ((section (".eeprom")));  // settingsdata from eeprom

button_t buttons[MAXBUTTON];				// Array of buttoncoordinates 
uint8_t max_button;  // number of declared buttons

int8_t read_touch_settings(void);

// initializes adc and sets parameter for turning on and off acd every adc read
// adc_off = 1 turns on ADC before and off ADC after read the touch position
int8_t touch_init(uint8_t adc_off)
{
	touch.sleep = adc_off;
/*	touch.scale_x = 387;
	touch.scale_y = 407;
	touch.offset_x = 139;
	touch.offset_y = 405; */
	a2dInit();
	if (touch.sleep) a2dOff();
	*(&ADC_PORT -1) |= (1<<TOUCH_X1) | (1 << TOUCH_X2) | (1<< TOUCH_Y1) | (1<< TOUCH_Y2);  // define all for outputs

	return(read_touch_settings());
}

// reads the touch pannels adc inputs in x and y direction
// if touchscreen is configered it calculates it screen position in touch_pos structure
int8_t read_touch(void)
{
	if (touch.sleep) a2dOn();
	int8_t result=0;
	ADC_PORT |= (1<<TOUCH_X1);		// put 5 volt an TOUCH_X1 and 0V on TOUCH_X2
	_delay_us(100);		// delay cause touchpad is like a capacitor
#ifdef AD_ON
	*(&ADC_PORT -1) &=  ~((1<< TOUCH_Y1) | (1<< TOUCH_Y2));  // inputs for measuring wires
	_delay_us(40);		// time delay for signal to settle
	touch_pos.x = a2dConvert10bit(TOUCH_Y2);  // read voltage of X direction
#else
	touch_pos.x = 512;
#endif
	ADC_PORT &= ~(1 <<TOUCH_X1);  // deactivate pullup
	*(&ADC_PORT -1) |= (1<<TOUCH_X1) | (1 << TOUCH_X2) | (1<< TOUCH_Y1) | (1<< TOUCH_Y2);  // define all for outputs
	if (touch_pos.x > touch.offset_x)
	{
		ADC_PORT |= (1<< TOUCH_Y1);		// put 5 volt an TOUCH_Y1 and 0V on TOUCH_Y2
		_delay_us(100);			// delay cause touchpad is like a capacitor
		*(&ADC_PORT -1) &= ~((1<<TOUCH_X1) | (1 << TOUCH_X2)); // put them as input port pins again
#ifdef AD_ON
		_delay_us(40);	// time delay for signal to settle
		touch_pos.y = a2dConvert10bit(TOUCH_X2);  // read voltage of Y direction
#else
		touch_pos.y=512;
#endif
		ADC_PORT &= ~(1 <<TOUCH_Y1);  // deactivate pullup
		*(&ADC_PORT -1) |= (1<<TOUCH_X1) | (1 << TOUCH_X2) | (1<< TOUCH_Y1) | (1<< TOUCH_Y2);  // define all for outputs
		if (touch_pos.y > touch.offset_y)
		{
			result =1;
			// a touch has been recognized -> calculate the display position if possible
			if ((touch.scale_x != 0) && (touch.scale_y != 0)) 
			{
				if (touch_pos.x > touch.offset_x) touch_pos.x -= touch.offset_x;		// remove adc offset for position 0
				else touch_pos.x =0;
				if (touch_pos.y > touch.offset_y) touch_pos.y -= touch.offset_y;
				else touch_pos.y=0;
				touch_pos.x = (unsigned long) touch_pos.x * touch.scale_x /1000;
				touch_pos.y = (unsigned long) touch_pos.y * touch.scale_y /1000;
			} else result = -1;  // calculation not possible
		}
	} 
	if (touch.sleep) a2dOff();
	return(result);
}

//  call function before calibrate touch screen 
void clear_settings(void)
{
	touch.scale_x= touch.scale_y=0;
	touch.offset_x=touch.offset_y=0;
}


// function to capture a touch press for calibration
void capture_point(uint8_t step)
{
	signed char res;
	do
	{
		res=read_touch();
		if (res==-1) 
		{
			for (uint8_t cnt=0;cnt<50;cnt++) _delay_ms(10);  // wait 500ms to stable the signal for accuracy
			res=read_touch();
			if (step==1)
			{
				adc_min.x= touch_pos.x;
				adc_min.y= touch_pos.y;
			}
		}
	}	while (res !=-1);
}


// calculation of Offset and Scale for calculation of givel ADC value to get the display position in pixel
// pixel_min  pointer to structure which hold the ADC values for pressed display position which is in adc_min
// pixel_max  pointer to structure which hold the ADC values for pressed display position which is in adc_max
// adc_max pointer to structure wich holds 
// adc_min
void calc_config( point_t pixel_min, point_t pixel_max)
{
	touch.scale_x = (unsigned long) (pixel_max.x - pixel_min.x) *1000/ (touch_pos.x - adc_min.x);
	touch.scale_y = (unsigned long) (pixel_max.y - pixel_min.y) *1000/ (touch_pos.y - adc_min.y);

	touch.offset_x =  adc_min.x - ((unsigned long) pixel_min.x *1000 / touch.scale_x ) ;
//	touch.offset_x += ((unsigned long) adc_max->x* touch.scale_x / 1000)- pixel_max->x;
//	touch.offset_x /= 2;

	touch.offset_y =  adc_min.y - ((unsigned long) pixel_min.y * 1000/ touch.scale_y );
//	touch.offset_y += ((unsigned long) adc_max->y* touch.scale_y / 1000) - pixel_max->y;
//	touch.offset_y /= 2;
	
}


void save_touch_settings(void)
{
	eeprom_busy_wait();
	eeprom_write_byte(&ee_version,TS_VERSION);
	eeprom_write_block(&touch, &ee_touch, sizeof(ctouch_t));

}

int8_t read_touch_settings(void)
{
	uint8_t version;
	if (eeprom_read_byte(&ee_version) == TS_VERSION) 
	{
		eeprom_read_block(&touch, &ee_touch,  sizeof(ctouch_t));
		return(1);
	} else return(-1);
}

// searches if a given pixel position in pos is defined in button array as Xpixel/2 and Ypixel/2 
// return is number of found button
// return is -1 if no button has been found
int8_t search_areas(void)
{
	uint8_t posx = touch_pos.x/2;
	uint8_t posy = touch_pos.y/2;
	
	uint8_t btn=0;
	do
	{
		if ((posx >= buttons[btn].x1) && (posx <= buttons[btn].x2) && (posy >= buttons[btn].y1) &&(posy <= buttons[btn].y2)) return(btn+1);
		btn++;
	} while (btn < max_button);
	return(-1);
}


// a button with its coordinates start to stop is added to the array
//             start	+------------------------------------+
//								|																			|
//								+------------------------------------+stop
// buttons coordinates must be given in Xpixel and Ypixel
// return is button number if possible to add
// return is -1 if no more button can be added
int8_t add_area(point_t start, point_t stop)
{
	if (max_button < MAXBUTTON)
	{
		buttons[max_button].x1 = start.x/2;
		buttons[max_button].x2 = stop.x/2;
		buttons[max_button].y1 = start.y/2;
		buttons[max_button].y2 = stop.y/2;
		return(max_button++);
	} else return(-1);
}

void clear_areas(void)
{
	max_button=0;
}

int8_t capture_area(void)
{
	int8_t res;
	do
	{
		res = read_touch();
		if (res==1)
		{
			res = search_areas();
			if (res >0) break;
		}
	} while (res <= 0);
	return(res);
}
