#include <pololu/_myincludes.h>
/*

	A small solar pv panel sun tracker designed for use with the Baby Orangutan B
	robot controller module (Atmel AVR Atmega 328p microcontroller) -- coded in C
   scottfromscott@bellsouth.net

   AVR Studio 4 Users: Under Project, Configuration Options
      Libraries, add libpololu_atmega328p.a, libc.a (to build without errors)
      Custom Options, [linker options], add -Wl,-gc-sections  (builds only library functions used by your program)

   external functions in _myincludes.h:

   void wait(milliseconds)			     -- pauses program execution for 'milliseconds' milliseconds
   int switch_is_open(pin)		 	     -- sets PULL_UP_ENABLED on pin IO_xx; returns 1 if pin is not grounded
   char sc(value, switch)				  -- encode/decode small signed numbers; helper function for declination()
   float declination(lat, lon)    	  -- returns the approximate magnetic declination for a given latitude, longitude
   (optional functions)...
   void debug_d(digit(0..9)) 		     -- flashes optional LED on PC0 n times for n=1..9, 10 times for 0; called directly or by debug()
   void debug(number,format) 		     -- flashes signed multi-digit numbers; see documentation in _myincludes.h

   solartracker functions:
   main()    		      -- initialization and control logic
   get_sun_pos()  		-- returns the the panel's location (lat, lon), sun's altitude and azimuth (s_alt, s_az)
   get_panel_altitude() -- returns panel's current altitude/accelerometer-reading (p_alt)
	get_panel_azimuth()  -- returns the solar panel's current azimuth (p_az) adjusted to true north
	
							BABY O
			  ===========================
ALT MOT(+)+PD6/M1B					  VIN+(+12V; 7812 regulated; rev. voltage prot.; fused)
ALT MOT(-)+PD5/M1A					  GND+(-)
			  PB0(D)					 M2B/PB3+AZ MOT(+)
			  PB1(D)					 M2A/PD3+AZ MOT(-)(SDA) 
			  PB2(D)    			(A/D)PC5+4.7KR+HMC6352 SCL
			  PB4(D,PROG)			(A/D)PC4+4.7KR+HMC6352 SDA
			  PB5(D,PROG)			(A/D)PC3+ALT(Y)
  GPS(SIO)+PD0(D,UARTRX)	   (A/D)PC2
			  PD1(D,UARTTX,LED) 	(A/D)PC1
			  PD2(D)					(A/D)PC0(+[optional debug led])
ALT(MINSW)+PD4(D)					 (A)ADC6
ALT(MAXSW)+PD7(D)		(A,POT) RESET/PC6
			  ========== = ADC7 ==========

GPS, ACCELEROMETER, COMPASS MODULES POWERED BY 7805 REGULATED 5V

*/

void get_sun_pos(float *udtLocationdLatitude,float *udtLocationdLongitude,float *altitude,float *azimuth){

   /******************************************************************************************************************
      get gps info from Parallax GPS receiver module -- coded for baby orangutan B
		inputs: none
		outputs: UTC (year,month,day,hour,minute,second), latitude,longitude
		GPS Info: http://www.parallax.com/Portals/0/Downloads/docs/prod/acc/GPSManualV1.1.pdf
      AVR Library Command Reference: http://www.robotshop.ca/content/PDF/pololu-avr-library-command-reference-1220.pdf
		Module must have clear view of sky and be level
		
		Baby O  <->    GPS	<->	7805	<->	POWER
		PD0------------sio			Vin---------+12V, 7812 regulated, fused, rev. voltage prot.
		vin------------vin---------+5Vout
		Gnd------------Gnd---------Gnd---------Gnd
							|	
		         		Raw

                  PARALLAX GPS  
                +--------------+
                |              |  
      +-----GND-+              |
      | +5v-VCC-+              |
      |(PD0)SIO-+              |
      +-----RAW-+              |
      |         |              |
     ===        +--------------+
      -
      .
   	

      format of Parallax GPS GPMRC string (see: http://www.gpsinformation.org/dale/nmea.htm)
      $GPMRC,HHMMSS,A,DDMM.MMMM,H,DDDMM.MMMM,H,KKK.K,HHH.H,DDMMYY,,,A*64
      012345678901234567890123456789012345678901234567890123456789012345
                1         2         3         4         5         6      
      where:   
      $=start of string
      GPMRC=data string id
      HHMMSS=UTC [hours][minutes][seconds]
      S=status 'A'ctive or 'V'oid
      DDMM.MMMM@16=latitude [degrees][minutes].[fractional minutes]
      H@26=latitude hemisphere 'N' or 'S'
      DDMM.MMMM@28=longitude [degrees][minutes].[fractional minutes]
      H@39=longitude hemisphere 'E' or 'W'
      KKK.K=speed over the ground in knots
      HHH.H=track angle in degrees true
      DDMMYY:UTC [day][month][year]

      missing from Parallax GPS output:

      magnetic declination (DD.D) should be at position 60?
      magnetic declination direction 'E' or 'W' 

      NOTE: magnetic declination approximated with declination(lat,lon) library function 
      
      A*64=checksum

   ***************************************************************************************************************/
   // $GPRMC string locations for following data items...
   int utc_year=57,utc_month=55,utc_day=53,utc_hours=7,utc_minutes=9,utc_seconds=11, \
		 latitude=16,latitude_hemisphere=26,longitude=28,longitude_hemisphere=39; 

   serial_set_baud_rate(4800);
   char gps[66]; // array to store $GPRMS string
   char buf[1];
   char type[6] = "$GPRMC";
   int type_matched = 0;
   int byte_count;
   int i;

   while(!type_matched)
	{
		do // test for valid data before continuing...
		{
			serial_receive(buf,1);
			byte_count = serial_get_received_bytes();
			while(byte_count == 0) byte_count = serial_get_received_bytes();
		} while (buf[0] > 127); // invalid data
		
      while (buf[0]!='$')
      { // scan forward to next $...
         serial_receive(buf,1);
   	   byte_count = serial_get_received_bytes();
	      while(byte_count == 0) byte_count = serial_get_received_bytes();
      }
      gps[0]=buf[0];
      // is this $GPRMC?
      type_matched = 1;
      for (i=1;i<sizeof(type);i++)
      {
         serial_receive(buf,1);
   	   byte_count = serial_get_received_bytes();
	      while(byte_count == 0) byte_count = serial_get_received_bytes();
         gps[i]=buf[0];
         if(type[i]!=gps[i])
         {
            type_matched = 0;
            break;
         }
      }
      if (type_matched)
      {
         // get the rest of the line...
         for (i=sizeof(type);i<sizeof(gps);i++)
         {
            serial_receive(buf,1);
      	   byte_count = serial_get_received_bytes();
	         while(byte_count == 0) byte_count = serial_get_received_bytes();
            gps[i] = buf[0];
         }
      }
   }
 
   *udtLocationdLatitude = (gps[latitude+0]-48)*10+(gps[latitude+1]-48)+(float)((gps[latitude+2]-48)*10+(gps[latitude+3]-48))/60;
   if(gps[latitude_hemisphere]=='S') *udtLocationdLatitude= -*udtLocationdLatitude; // apply sign

   *udtLocationdLongitude = (gps[longitude+0]-48)*100+(gps[longitude+1]-48)*10+(gps[longitude+2]-48) \
         +(float)((gps[longitude+3]-48)* 10+(gps[longitude+4]-48))/60;
   if(gps[longitude_hemisphere]=='W') *udtLocationdLongitude = -*udtLocationdLongitude; // apply sign
	
   float udtTimeiYear 	 = (gps[utc_year    + 0]-48)*10+(gps[utc_year    + 1]-48);
   float udtTimeiMonth   = (gps[utc_month   + 0]-48)*10+(gps[utc_month   + 1]-48);
   float udtTimeiDay 	 = (gps[utc_day     + 0]-48)*10+(gps[utc_day     + 1]-48);
   float udtTimedHours 	 = (gps[utc_hours   + 0]-48)*10+(gps[utc_hours   + 1]-48);
   float udtTimedMinutes = (gps[utc_minutes + 0]-48)*10+(gps[utc_minutes + 1]-48);
   float udtTimedSeconds = (gps[utc_seconds + 0]-48)*10+(gps[utc_seconds + 1]-48);
   
/* get sun position */


 /*
	a simplified, yet accurate sun position calculator developed by Blanco-Muriel et al.
	recoded to C from javascript by scottfromscott@bellsouth.net
	closely matches altitude, azimuth values returned by NOAA calculator  
*/

	float zenith;
	float pi =3.14159265358979323846;
	float twopi=(2*pi);
	float rad=(pi/180);
	float dEarthMeanRadius=6371.01;	// In km
	float dAstronomicalUnit=149597890;	// In km
	float dDecimalHours = udtTimedHours + (udtTimedMinutes + udtTimedSeconds / 60.0 ) / 60.0;
	// Calculate current Julian Day not use of trunc since Javascript doesn't support div for integters like C++
	long liAux1 =(udtTimeiMonth-14)/12;
	long liAux2=(1461*(udtTimeiYear + 4800 + liAux1))/4 + (367*(udtTimeiMonth - 2-12*liAux1))/12- (3*(udtTimeiYear + 4900 + liAux1)/100)/4+udtTimeiDay-32075;
	float dJulianDate=(liAux2)-0.5+dDecimalHours/24.0;
	float dElapsedJulianDays = dJulianDate-2451545.0;
	float dOmega=2.1429-0.0010394594*dElapsedJulianDays;
	float dMeanLongitude = 4.8950630+ 0.017202791698*dElapsedJulianDays; // Radians
	float dMeanAnomaly = 6.2400600+ 0.0172019699*dElapsedJulianDays;
	float dEclipticLongitude = dMeanLongitude + 0.03341607*sin( dMeanAnomaly ) + 0.00034894*sin( 2*dMeanAnomaly )-0.0001134 -0.0000203*sin(dOmega);
	float dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays +0.0000396*cos(dOmega);
	float dSin_EclipticLongitude= sin( dEclipticLongitude );
	float dY = cos( dEclipticObliquity ) * dSin_EclipticLongitude;
	float dX = cos( dEclipticLongitude );
	float dRightAscension = atan2( dY,dX );
	if( dRightAscension < 0.0 ) dRightAscension = dRightAscension + twopi;
	float dDeclination = asin( sin( dEclipticObliquity )*dSin_EclipticLongitude );
	float dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283*dElapsedJulianDays + dDecimalHours;
	float dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime*15 + *udtLocationdLongitude)*rad;
	float dHourAngle = dLocalMeanSiderealTime - dRightAscension;
	float dLatitudeInRadians = *udtLocationdLatitude*rad;
	float dCos_Latitude = cos( dLatitudeInRadians );
	float dSin_Latitude = sin( dLatitudeInRadians );
	float dCos_HourAngle= cos( dHourAngle );
	float udtSunCoordinatesdZenithAngle = (acos( dCos_Latitude*dCos_HourAngle*cos(dDeclination) + sin( dDeclination )*dSin_Latitude));
	dY = -sin( dHourAngle );
	dX = tan( dDeclination )*dCos_Latitude - dSin_Latitude*dCos_HourAngle;
	float udtSunCoordinatesdAzimuth = atan2( dY, dX );
	if ( udtSunCoordinatesdAzimuth < 0.0 ) 
		udtSunCoordinatesdAzimuth = udtSunCoordinatesdAzimuth + twopi;
	udtSunCoordinatesdAzimuth = udtSunCoordinatesdAzimuth/rad;
	float dParallax=(dEarthMeanRadius/dAstronomicalUnit)*sin(udtSunCoordinatesdZenithAngle);
	udtSunCoordinatesdZenithAngle=(udtSunCoordinatesdZenithAngle + dParallax)/rad;
	*azimuth=udtSunCoordinatesdAzimuth;
	zenith=udtSunCoordinatesdZenithAngle;
	*altitude=90-udtSunCoordinatesdZenithAngle;
}

float get_panel_altitude(){
	/*
		read raw voltage on channel 3, then convert it to degrees 0..90 using pin Y only
		see: http://www.pololu.com/docs/0J18/2   for explanation of analog functions
		see: http://www.dimensionengineering.com/DE-ACCM2G2.htm for DE-ACCM2G2 specs

		Baby O  <-> DE-ACCM2G2 <-> 7805	<->	Battery (14AH, AGM)

		Pc3-------------Y				Vin---------+12V
		vin------------Vin--------5Vout
		Gnd------------Gnd---------Gnd----------Gnd
		

     DE-ACCM2G2
   VCC+-----+GND
      |     |
      |     |        ^
      |     |        | Y-axis, -> X-axis
      |     |
     Y+-----+X
	*/

	float de_accm2g2_mv;
   unsigned char calibrated;
  	
	//set_analog_mode(MODE_10_BIT); // maximum resolution (1024 steps)
	set_millivolt_calibration(read_vcc_millivolts()); // recommended in docs
   de_accm2g2_mv = analog_read_average_millivolts(3, 100); // get average of accelerometer values
   
   calibrated = 1; // set calibrated to 0 for raw millivolt output during calibration
                   // set calibrated to 1 to return degrees after calibration 

   if (calibrated)
   {
      // NOTE: table endpoints of 990 and 2310 are min and max millivolts de_accm2g2 is capable of -- do not remove from table
      //       with calibrated set to 0, take readings every 10 degrees starting with 0; and ending with 90
      //       using a protractor or similar angle measurement device.
      //       place these mv readings between the 990, and 2310 in the de_accm2g2_mv_tbl[] lookup table 
      //       then set calbrated to 1 and test your new degree readings against the protractor.

      //             Degrees:     (-10),   0,  10,  20,  30,  40,  50,  60,  70,  80,  90,(100)  
      float de_accm2g2_mv_tbl[]  = {990,1051,1105,1187,1283,1375,1491,1604,1723,1819,1924,2310};
      int i=0;
      float de_accm2g2_deg;
      while (de_accm2g2_mv >= de_accm2g2_mv_tbl[i+1]) i++;
      de_accm2g2_deg = (de_accm2g2_mv - de_accm2g2_mv_tbl[i]) / (de_accm2g2_mv_tbl[i+1] - de_accm2g2_mv_tbl[i]) * 10 + 10 * (i-1);
      return de_accm2g2_deg;   
   }
   else return de_accm2g2_mv;
}

float get_panel_azimuth(float lat,float  lon){
/*
								
         HMC6352
        +-------+ 
        |  o o  | 
   SDA--+		 +--VIN      
     x--+		 +--x    NORTH ==>
	GND--+		 +--SCL      
		  +-------+  
		  
   put 4.7k ohm resistors between SDA and PC4, SCL and PC5

*/
   unsigned char hi_byte, lo_byte;
   float compass_bearing;

   i2c_init();
   i2c_start_wait(0x42+I2C_WRITE);
   i2c_write(0x41); // get heading
   wait(6); // takes 6000 microseconds to respond to heading command
   i2c_rep_start(0x42+I2C_READ);
   hi_byte = i2c_readAck();
   lo_byte = i2c_readNak();                   
   i2c_stop(); 
   compass_bearing = (float)(hi_byte * 256 + lo_byte) / 10;

   /*
		Disclaimer: my compass is rotated 90 degrees CCW.
		Your solution may be different.
		Buy a pocket compass with degree markings every 2 degrees.
		Set calibration = 0.
		Run this function from main() in a loop to observe values returned by the HMC6352.
		Using the following table, fill in the right column as indicated below:
		
		HMC6352 	| pocket
		-----------------
			0		|	90		reading on pocket compass when HMC6352 reads 0
			78.4	|	135	add 45 degees to value above it
			118.3	|	180	"
			149.2	|	225	"	
			179.6	|	270	"
			208.5	|	315	"
			233.6	|	0		"	(if >= 360, subtract 360)
			272.4	|	45		"
		0=>360	|	90		" 
		
		Now, turn the pocket compass to the value in column2, row 2.
		Align the HMC6352 with the pocket compass and put that value in column 1.
		Repeat for all rows. Change column 1, row 9 to 360 (HMC6352 column values must be increasing for lookup to work) 
		Using column 1, fill in compass_tbl[].
		Set compass_offset = (first pocket compass reading)
		Set calibrated = 1.
      Test the calibrated readings. The hmc6352 readings should closely match the compass rose markings.
    */

   short calibrated = 1;

   if (calibrated)
   {
      float compass_offset = 90;
      // with algorithm below, degrees may be >=360, so normalize >=0, <360 before returning bearing...
      // measurements taken every 45 degrees; the more measurements taken, the more accurate the calibration
      //              pocket:  90,  135,   180,   225,   270,   315,   360(0),   405(45), 450(90)
      float compass_tbl[] = {  0, 78.4, 118.3, 149.2, 179.6, 208.5, 233.6, 272.4, 360}; 
      int i=0;
      while (compass_bearing >= compass_tbl[i+1]) i++;
      compass_bearing = (compass_bearing - compass_tbl[i]) / (compass_tbl[i+1] - compass_tbl[i]) * 45 + 45 * i + compass_offset;
		compass_bearing += declination(lat, lon); // adjust to approx. true north
      if (compass_bearing >= 360) compass_bearing -= 360;
      else if (compass_bearing < 0) compass_bearing += 360;
   }
   return compass_bearing;
}

int main(){
   // get data from gps receiver to calculate sun's current position
   // use accelerometer and compass to determine altitude and azimuth of panel
   // turn panel to match sun's altitude and azimuth

   //short demo_mode =0; // set demo_mode = 1 to loop sun tracking simulation
  	//short s_alt_increasing = 1; // for demo mode
 
	/* contain the panel and sun's altitude and azimuth */

	float s_alt, p_alt;
	float s_az, p_az;
	float lat, lon;

	// aliases for limit-switch pins
	int alt_max = IO_D7, alt_min = IO_D4;
	
	// fine-tuning for motors
	
	//int speed    = 128;      	// 1/2 speed at 12v	 // -255..255?
   int speed = 200;
	/* #milliseconds motors run during realignment */

	int alt_runtime = 250;
   int az_runtime = 2500; 



   int max_alignment_error = 2.5;   // #degrees misalignment allowed b/w sun and panel before
	
//   int max_alignment_error = 3;   // #degrees misalignment allowed b/w sun and panel before
											 // adjustment is made; if the number is
											 // too small, the motors may oscillate back and forth
											 // (value is smaller than sensor resolution) or 
											 // they adjust too frequently (power consumed by
											 // realignment greater than benefit of realignment)
											 // too large and panel collection efficiency is reduced

	int sensor_update_delay = 1000;// give sensors time to take new readings and settle
	
   // how long to wait between readings of the sun's position
	// if this is set too low, the difference between the panel's angle
	// and the sun's angle will not be great enough to move the panel
	// and a bit more power may be consumed by the sensors
	// if it is set too high, the misalignment between the panel and sun
	// would result in some loss in panel efficiency
	   
	float minutes_between_readings = .1;

   //wait(45000); // takes up to 45 sec for GPS to acquire satellites after cold start
   
   /*
   the following loop compares the panel position and sun position and keeps
	them aligned during the daylight hours. 'minutes_between_readings' determines
   how often this process repeats
   */

	while(1)
   {
	/*
      if (demo_mode)
      {
   		alt_runtime = 500;
	   	az_runtime = 5000; 

	   	lat = 30;
	   	lon = -92;

   		s_alt = 0;
	   	s_az = 70;

         if (s_alt_increasing) // going 0..90
         {
            s_alt += 5;
            s_az += 5;
            if (s_alt > 85) // stop stop near 90
            {
               s_alt = 90;
               s_alt_increasing = 0;
            }
         }
         else // going 90..0
         {
            s_alt -= 5;
            s_az += 5;
            if (s_alt < 5) // stop near 0
            {
               // go back to start position and start over...
               s_alt = 0;
               s_az = 70;
               s_alt_increasing = 1;   
            } 
         }
      }
   */ 
      get_sun_pos(&lat, &lon, &s_alt, &s_az); 

      if (s_alt >= 0) // no movement unless sun at or above horizon
      {
			// align panel's altitude with sun's altitude
			
       	p_alt = get_panel_altitude(); // read accelerometer
			
			// do we need to increase panel's altitude to match sun's altitude and is alt_max limit switch open?

			while ((abs(s_alt - p_alt) > max_alignment_error)&&(p_alt < s_alt)&&switch_is_open(alt_max))
			{
            set_m1_speed(speed);
				wait(alt_runtime);
				set_m1_speed(0);
				wait(sensor_update_delay);
				p_alt = get_panel_altitude();
			}
			
			// do we need to decrease panel's altitude to match sun's altitude and is alt_min limit switch open?
			
			while ((abs(s_alt - p_alt) > max_alignment_error)&&(p_alt > s_alt)&&switch_is_open(alt_min))
			{ 
   			set_m1_speed(-speed);
				wait(alt_runtime);
				set_m1_speed(0); 
				wait(sensor_update_delay);
				p_alt = get_panel_altitude();
			}
			
			// align panel's azimuth with sun's azimuth
			
			p_az = get_panel_azimuth(lat, lon); // read compass, adjust for declination
			
			// do we need to decrease panel's azimuth to catch sun's azimuth?

			while ((abs(s_az - p_az) > max_alignment_error)&&(p_az > s_az))
			{ 
            set_m2_speed((p_az - s_az) > 180 ? speed : -speed); // take shortest arc
				wait(az_runtime);
				set_m2_speed(0);
				wait(sensor_update_delay);
				p_az = get_panel_azimuth(lat, lon);
			}
	
			// do we need to increase panel's azimuth to catch sun's azimuth?
			
			while ((abs(s_az - p_az) > max_alignment_error)&&(p_az < s_az))
			{ 
				set_m2_speed((s_az - p_az) > 180 ? -speed : speed); // take shortest arc
				wait(az_runtime);
				set_m2_speed(0);
				wait(sensor_update_delay);
				p_az = get_panel_azimuth(lat, lon);
			}
			
      } // end if (s_alt >= 0)
      for (int i=1;i <= minutes_between_readings * 60; i++) wait(1000);
   } /* end main loop */
	return 0;
}
