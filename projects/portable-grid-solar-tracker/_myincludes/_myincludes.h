#define F_CPU 20000000UL	// Baby Orangutan frequency (20MHz)
#include <math.h> 
#include <avr/io.h>
#include <util/delay.h> // uses F_CPU to achieve us and ms delays
#include <pololu/orangutan.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <i2cmaster.h>  //located in include directory
#include <i2cmaster.c>  //located in include directory 

void wait( uint16_t millis ){
   while(millis--) _delay_ms(1);
}

void debug_d(int digit)
{
	/*
		pulse user LED on PC0
		digit 1 to 9: 1 to 9 normal (250ms) pulses
		digit 0: one long (500ms) pulse
		digit 0xA (10): (decimal point) 3 short (125ms) pulses
		digit 0xB (11): (negative number) 9 short (125ms) pulses
	*/
	
	int long_delay = 0, short_delay = 0;
	if (digit == 0) {digit = 1; long_delay = 750;}
	if (digit == 0xA) {digit = 3; short_delay = -125;}
	if (digit == 0xB) {digit = 9; short_delay = -125;}
  
   while(digit--)
   {  set_digital_output(IO_C0,HIGH);
      wait(250+long_delay+short_delay);
      set_digital_output(IO_C0,LOW);
      wait(250+short_delay);
   }
   wait(500);
}

int debug(double v)
{	
	/*
		pulse number using external LED on PC0 for debugging purposes
		calls debug_d() to pulse each digit, decimal point or sign, if negative
		ex: debug(-20.5) => [(9 short):2:(1 long):(3 short):5] pulses
		function returns 0 for OK, 1 for error
		numbers are rounded to 3 decimal places; function returns 1
		numbers outside the range +|- MAX_VALUE will be truncated to +|- MAX_VALUE; function returns 0;
		numbers are leading|trailing zero suppressed, eg. debug(-0.70) <=> debug(-.7) => [9 long][3 short][7 medium] pulses
	*/
    
    int status = 1;
    long u6,u5,u4,u3,u2,u1, wp, dp;
    const double MAX_VALUE = 32767.999;
    
    if(v<0){v=-v;debug_d(0xB);} // flash that number is negative
    
	if (fabs(v)>MAX_VALUE)  // exceeding supported limit flashes [-]MAX_VALUE, error status
    {
        v = MAX_VALUE;
        status = 0;
    }

	wp = floor(v); // whole part of number
    dp = round((v-wp)*10000/10); // decimal part

	u6 = floor(wp/100000);
	u5 = floor((wp-u6*100000)/10000);
	u4 = floor((wp-u6*100000-u5*10000)/1000);
	u3 = floor((wp-u6*100000-u5*10000-u4*1000)/100);
	u2 = floor((wp-u6*100000-u5*10000-u4*1000-u3*100)/10);
	u1 = wp-u6*100000-u5*10000-u4*1000-u3*100-u2*10;
 
    if(u6!=0) debug_d(u6);
	if(u6!=0 || u5!=0) debug_d(u5); 
	if(u6!=0 || u5!=0 || u4!=0) debug_d(u4);
	if(u6!=0 || u5!=0 || u4!=0 || u3!=0) debug_d(u3);
	if(u6!=0 || u5!=0 || u4!=0 || u3!=0 || u2!=0) debug_d(u2);
	if((u6!=0 || u5!=0 || u4!=0 || u3!=0 || u2!=0 || u1!=0) || dp==0) debug_d(u1); // yes: 0, 20, -0; no: -0.23

	// handle decimal part
	
	if (dp!=0)
	{
		debug_d(0xA); // flash decimal point
		
		u3 = floor(dp/100);
		u2 = floor((dp-u3*100)/10);
		u1 = dp-u3*100-u2*10;

		// no trailing zeros...
		if (u3!=0||u2!=0||u1!=0) debug_d(u3); // .001
		if (u2!=0||u1!=0) debug_d(u2);
		if (u1!=0) debug_d(u1);
   }

    wait(2500); //pause between numbers
    return status;
}

int switch_is_open(int pin)
{
   /*
      test limit switch connected to any general i/o pin
      '..When the AVR powers up, all I/O pins are configured as inputs with their
      pull-up resistors disabled." Docs also say must set to PULL_UP_ENABLED before reading state
		pin ungrounded: is_digital_input_high() returns 1;
		pin grounded: is_digital_input_high() returns 0
  */
   
   set_digital_input(pin, PULL_UP_ENABLED);
	wait(100);
   return is_digital_input_high(pin);   
}

float declination(float lat, float lon)
{
	/*
		lookup declination values from 10 x 10 degree grid and return approximate declination for (lat,lon) 
		data; 482 declination values (rounded to nearest degree) stored in 482 bytes
	*/

// -60..0..60 step 10 => 2(6) + 1 = 13 dimensions for lat; -180..0..180 step 10 => 2(18) + 1 = 37 dimensions for lon 	
	char dec_tbl[13][37] = \
{46,45,44,42,41,40,38,36,33,28,23,16,10,4,-1,-5,-9,-14,-19,-26,-33,-40,-48,-55,-61, \
-66,-71,-74,-75,-72,-61,-25,22,40,45,47,46,30,30,30,30,29,29,29,29,27,24,18,11,3, \
-3,-9,-12,-15,-17,-21,-26,-32,-39,-45,-51,-55,-57,-56,-53,-44,-31,-14,0,13,21,26, \
29,30,21,22,22,22,22,22,22,22,21,18,13,5,-3,-11,-17,-20,-21,-22,-23,-25,-29,-35, \
-40,-44,-45,-44,-40,-32,-22,-12,-3,3,9,14,18,20,21,16,17,17,17,17,17,16,16,16,13, \
8,0,-9,-16,-21,-24,-25,-25,-23,-20,-21,-24,-28,-31,-31,-29,-24,-17,-9,-3,0,4,7, \
10,13,15,16,12,13,13,13,13,13,12,12,11,9,3,-4,-12,-19,-23,-24,-24,-22,-17,-12,-9, \
-10,-13,-17,-18,-16,-13,-8,-3,0,1,3,6,8,10,12,12,10,10,10,10,10,10,10,9,9,6,0,-6, \
-14,-20,-22,-22,-19,-15,-10,-6,-2,-2,-4,-7,-8,-8,-7,-4,0,1,1,2,4,6,8,10,10,9,9,9, \
9,9,9,8,8,7,4,-1,-8,-15,-19,-20,-18,-14,-9,-5,-2,0,1,0,-2,-3,-4,-3,-2,0,0,0,1,3,5, \
7,8,9,8,8,8,9,9,9,8,8,6,2,-3,-9,-15,-18,-17,-14,-10,-6,-2,0,1,2,2,0,-1,-1,-2,-1,0, \
0,0,0,1,3,5,7,8,8,9,9,10,10,10,10,8,5,0,-5,-11,-15,-16,-15,-12,-8,-4,-1,0,2,3,2,1,0, \
0,0,0,0,-1,-2,-2,-1,0,3,6,8,6,9,10,11,12,12,11,9,5,0,-7,-12,-15,-15,-13,-10,-7,-3, \
0,1,2,3,3,3,2,1,0,0,-1,-3,-4,-5,-5,-2,0,3,6,5,8,11,13,15,15,14,11,5,-1,-9,-14,-17, \
-16,-14,-11,-7,-3,0,1,3,4,5,5,5,4,3,1,-1,-4,-7,-8,-8,-6,-2,1,5,4,8,12,15,17,18,16, \
12,5,-3,-12,-18,-20,-19,-16,-13,-8,-4,-1,1,4,6,8,9,9,9,7,3,-1,-6,-10,-12,-11,-9,-5, \
0,4,3,9,14,17,20,21,19,14,4,-8,-19,-25,-26,-25,-21,-17,-12,-7,-2,1,5,9,13,15,16,16, \
13,7,0,-7,-12,-15,-14,-11,-6,-1,3};

	float decSW, decSE, decNW, decNE, decmin, decmax;
	float lonmin,latmin;
	short latmin_index,lonmin_index;
	/* set base point (latmin, lonmin) of grid */
	
	/* no limits test on lon */
	if (lon == 180) lonmin = 170;
	else lonmin = floor(lon/10)*10;
		
	/* supported lat's -60..60, so range check... */
	if (lat >= 60) latmin = 50;
	else if (lat < -60) latmin = -60;
	else latmin = floor(lat/10)*10;
	
	/* array index = (degrees+[60|180])/10 */
	latmin_index= (60+latmin)/10;
	lonmin_index= (180+lonmin)/10;
		
	decSW = dec_tbl[latmin_index][lonmin_index];
	decSE = dec_tbl[latmin_index][lonmin_index+1];
	decNE = dec_tbl[latmin_index+1][lonmin_index+1];
	decNW = dec_tbl[latmin_index+1][lonmin_index];
	
	/* approximate declination within the grid using bilinear interpolation */

	decmin = (lon - lonmin) / 10 * (decSE - decSW) + decSW;
	decmax = (lon - lonmin) / 10 * (decNE - decNW) + decNW;
	return   (lat - latmin) / 10 * (decmax - decmin) + decmin;
}


/*
	DIAGRAM OF 10X10 GRID SQUARE:
	
(+60)						(lon,latmin+10,decmax=?)		
l	(lonmin,latmin+10,decNW)|	|		|(lonmin+10,latmin+10,decNE)
a								 --o--x-----o--
t									|	|		|
i									|	|		|
t									+--x(lon,lat,dec=?)		
u									|	|		|
d								 --o--x-----o--
e		(lonmin,latmin,decSW)|	|		|(lonmin+10,latmin,decSE)
(-60)						(lon,latmin,decmin=?)

				 		(-180)<- longitude ->(+180)

					o = decs from table, x = calculated decs
*/

