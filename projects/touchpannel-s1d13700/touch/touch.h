//************************************************************************
// For 4 wire resistive touch screens
// Functions for reading position of touchscreen press
// uses ADC of Mega Prozessor for calculating the position
// Klaus Meinhardt  created 12.01.2008
//************************************************************************
// Declaration of 32 buttons with start point end end point 
// Resolution of point is 2 pixel (for screen with maximal 512x512 pixel)
//************************************************************************
#include "S1D13700.h"

// Defines the port and port pins where the resistive touch screen is connected
#define ADC_PORT  PORTF
#define TOUCH_Y1  2
#define TOUCH_Y2 0
#define TOUCH_X2 1
#define TOUCH_X1 3


typedef struct
{
	unsigned char sleep;  // sleep = 1 turns on ADC before and off ADC after read the touch position
	unsigned int offset_x; 	// ADC results for minimum position in x direction
	unsigned int offset_y; 	// ADC results for minimum position in y direction
	unsigned int scale_x; 	// scale factor for calculating x direction
	unsigned int scale_y; 	 // scale factor for calculating y direction
} ctouch_t;

extern ctouch_t touch;	// configuration data se typedef

typedef struct
{
	unsigned char x1, x2, y1, y2;
} button_t;

// a button with its coordinates start to stop is added to the array
//             start	+------------------------------------+
//								|																			|
//								+------------------------------------+stop
// buttons coordinates must be given in Xpixel and Ypixel
// return is button number if possible to add
// return is -1 if no more button can be added
extern signed char add_area(point_t start, point_t stop);
extern void clear_areas(void);
// searches if a given pixel position in pos is defined in button array as Xpixel/2 and Ypixel/2 
// return is number of found button
// return is -1 if no button has been found
extern signed char search_areas(void);

// reads the touch pannels adc inputs in x and y direction
// if touchscreen is configered it calculates it screen position in touch_pos structure
extern signed char read_touch(void);

// initializes adc and sets parameter for turning on and off acd every adc read
// adc_off = 1 turns on ADC before and off ADC after read the touch position
extern int8_t touch_init(unsigned char adc_off);
//  call function before calibrate touch screen 
extern void clear_settings(void);

extern point_t touch_pos;			// the result of the x and y position of called function read_touch
extern int8_t capture_area(void);

// function to capture a touch press for calibration
extern void capture_point(uint8_t step);

// calculation of Offset and Scale for calculation of givel ADC value to get the display position in pixel
// pixel_min  pointer to structure which hold the ADC values for pressed display position which is in adc_min
// pixel_max  pointer to structure which hold the ADC values for pressed display position which is in adc_max
// adc_max pointer to structure wich holds 
// adc_min
extern void calc_config( point_t pixel_min, point_t pixel_max);

extern void save_touch_settings(void);
