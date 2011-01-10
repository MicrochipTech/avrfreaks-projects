//***************************************************************************
//
//  File........: GPS.c
//
//  Author(s)...: Gerd Bartelt
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.3.4; avr-libc 1.0
//
//  Description.: AVR Butterfly GPS routines
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20110106 - 1.0  - Created                                       - Gerd Bartelt
//
//***************************************************************************


#include <avr/io.h>
#include <stdio.h>
#include <math.h>

#include "button.h"
#include "main.h"
#include "GPS.h"
#include "RTC.h"
#include "LDR.h"
#include "LCD_functions.h"

// Global parameter like sun position
double r[2];
double dec[2];
double t0;

// Global position
volatile double pos_lat;
volatile double pos_lon;

// Time of sunset and sunrise
volatile double sunrise_calc;
volatile double sunset_calc;

// Calculated values
double daytime_calc;
double daymedian_calc;

/**
 * Calculate the parameters like sun position and some parameters for
 * the day of today.
 *
 * Input: Global values: gDAY, gMONTH, gYEAR
 *
 * Output: Global values: r[2] and dec[2] for sun position and t0
 *
 */
void CalcDayParameter(void)
{
	long juliandate;
    double jd;
	double MSun;
	double lon;
    double t;
    double e;
    double a;
    int i;
    int day;
    int month;
    int year;

    // Use the day of today
    day = gDAY;
    month = gMONTH;
    year = gYEAR;

	if ( month < 3 ) {
		month += 12;
		year -= 1;
	}

	// Calculate the julian date of today
	juliandate = (2451511  + (int)( year*365.25 ) + (int)( 30.6 * month + 0.61) + day);

	// Calculate the sun position of today at 0:00 and 24:00
	for (i = 0; i<2 ; i++) {
    	jd = juliandate + i;
    	MSun = 0.017202791632524146*(jd-2455929.741570342);
    	lon = MSun + 0.033426*sin(MSun)+4.935239984568769;
    	t = (jd -2451544.499247685);
    	e = 0.4090928139186025 + t*-6.213977407568221e-9 + t*t* -2.1804476688922454e-18 + t*t*t* 1.8008716772507256e-22;
    	r[i] =  atan2( (sin(lon)*cos(e)), cos(lon) );
    	dec[i] = asin(sin(e)*sin(lon) );
    }

	// Calculate t0
	jd = juliandate;
	t = (jd-2451544.5)/36525.0;
	a = 6.697374558 + t*2400.051336 + t*t*0.000025862;
	t0 = a - floor(a/24)*24.0;


}

/**
 * Calculates the time of the sun rise and sun set
 *
 * Input: Position pos_lon and pos_lat
 *        Position of the sun
 *
 * Output: Time of sun rise and sun set: sunrise_calc, sunset_calc
 *
 */
void CalcSunSetAndRise(void) {

    double dayarc;
    double rise[2];
    double set[2];
    double tdiff;
    int i;

    // Calculates sun rise and sun set based on the sun position
    // at 0:00 and 24:00
    for (i = 0; i<2; i++) {
    	dayarc = acos((-0.1045 - sin(pos_lat)*sin(dec[i])) / (cos(pos_lat)*cos(dec[i])));
    	rise[i]    = 24.+3.819718634*(-dayarc+r[i]-pos_lon);
    	set[i]     =     3.819718634*(+dayarc+r[i]-pos_lon);
    }

    // Calculate the sun rise
	tdiff = rise[1]-rise[0];
	if (tdiff < 0) tdiff += 24.0;
	sunrise_calc = 0.9972695663*( (24.07*rise[0]- t0*tdiff) / (24.07-tdiff) -t0);
	if (sunrise_calc < 0) sunrise_calc +=24.0;

    // Calculate the sun set
	tdiff = set[1]-set[0];
	if (tdiff < 0) tdiff += 24.0;
	sunset_calc = 0.9972695663*( (24.07*set[0]- t0*tdiff) / (24.07-tdiff) -t0);
	if (sunset_calc < 0) sunset_calc +=24.0;
}

/**
 * Calculates the position for a given date, sun rise and sun set.
 * This is done by using the function to calculate the sun rise and
 * sun set time and search for the position iteratively.
 *
 */
void GPSCalc(void)
{
    // Start with this coordinates
	pos_lat = 0.5;
    pos_lon = 0.0;
    int i;

    // Calculate the parameters for this day. Do it only one time.
    CalcDayParameter();

    // Search for the position iteratively. Do it 100 times.
    for (i = 0; i<100; i++) {

    	// Calculate sun rise and sun set
        CalcSunSetAndRise();

        // Calculate the time between sun rise and sun set and
        // the mean time.
        daytime_calc = sunset_calc - sunrise_calc;
        daymedian_calc = (sunset_calc + sunrise_calc) * 0.5;

        // Adapt the longitude
        pos_lon += 0.15 * (daymedian_calc - daymedian_meas);

        // Use this in the summer
        //pos_lat -= 0.05 * (daytime_calc - daytime_meas);

        // Adapt the latitude
        // Use this in the winter
        pos_lat += 0.05 * (daytime_calc - daytime_meas);

    }

}


/*****************************************************************************
*
*   Function name : ShowGPSPos
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Calculates and show the global position
*
*****************************************************************************/
char ShowGPSPos(char input)
{
    static char enter = 1;
    char mystr[60];

    if (enter)
    {
    
        enter = 0;
        
        // Do the GPS calculation
        GPSCalc();

        // Display calculated position,
        //         LDR value at calculated sun rise and sun set time,
        //         time of sun rise and sun set
        sprintf(mystr,"E%f N%f R%d S%d r%f s%f" , pos_lon, pos_lat, sunRiseLDRVal, sunSetLDRVal, sunrise_meas, sunset_meas);
        LCD_puts(mystr);
    }
    else {

    }

    if (input == KEY_PREV)
    {
        enter = 1;  // Set enter to 1 before leaving the ShowGPSPos
    
        return ST_GPS;
    }
    else
        return ST_GPS_POS;
}
