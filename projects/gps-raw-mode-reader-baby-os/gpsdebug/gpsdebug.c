/*
A 'raw mode' data reader for the Parallax GPS receiver module #28146 (based on Polstar PMB-248)
for use with the Baby Orangutan robot controller or with slight modifications, other Pololu orangutans
(data stream is captured and parsed) scottfromscott@bellsouth.net
*/

#define F_CPU 20000000UL	// Baby Orangutan frequency (20MHz)
#include <math.h> 
#include <avr/io.h>
#include <util/delay.h> // uses F_CPU to achieve us and ms delays
#include <pololu/orangutan.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char gps[650]; // array to store $GPRMS string

//format of Parallax GPS GPMRC string...
//$GPMRC,HHMMSS,A,DDMM.MMMM,S,DDDMM.MMMM,S,KKK.K,HHH.H,DDMMYY,,,A*64<13>
//0123456789012345678901234567890123456789012345678901234567890123456
//          1         2         3         4         5         6      
// $GPRMS string locations for following data items...
int utc_hours=7, utc_minutes=9, utc_seconds=11, latitude=16, \
                   latitude_hemisphere=26, longitude=28, longitude_hemisphere=39, speed=41, \
                   heading=47, utc_day=53, utc_month=55, utc_year=57;

void delayms(unsigned int time_ms)   
{for (unsigned int i = 0; i < time_ms; i++) _delay_ms(1);
}

//the following 4 debug functions flash an external LED I attached to #2 pwm motor port
//because the built-in RED LED is not available when the UART is being used
void debug_port(unsigned char pwm)
{  OCR2A = 0;
   OCR2B = pwm;
}

void debug_init()
{  TCCR2A = 0xF3;
   TCCR2B = 0x02;
   OCR2A = OCR2B = 0;
   DDRD |= (1 << PORTD3) | (1 << PORTD5) | (1 << PORTD6);
   DDRB |= (1 << PORTB3);
}

void debug_pulse_digit(int digit)
{  // pulse external led 1..10 times depending on value of digit
   // 1 -> 1 pulse; 2 -> 2 pulses; ... 9 -> 9 pulses; BUT 0 -> 10 pulses
   int counter, freq=75, delay=250;
   if(digit==0) digit = 10; // special case for 0
   for(counter=1;counter<=digit;counter++)
   {  debug_port(freq); 
      delayms(delay); 
      debug_port(0); 
      delayms(delay); 
   }
   delayms(1000);
}

void debug(float v)
{  // pulse 6 digits (hto.hto[-]) if negative, single pulse follows 6 pulse sequences
   // example output: -23.5 -> 023.500- -> 10 pulses, 2 pulses, 3 pulses, 5 pulses, 10 pulses, 10 pulses, 1 pulse 
   int o,t,h,d,n=0;
   if(v<0){v=-v;n=1;}
   h=(int)(v/100);t=(int)((v-h*100)/10);o=(int)(v-t*10-h*100);
   debug_pulse_digit(h);debug_pulse_digit(t);debug_pulse_digit(o);
   d=(int)((v-h*100-t*10-o)*1000); // .hto -> hto.
   h=(int)(d/100);t=(int)((d-h*100)/10);o=(int)(d-t*10-h*100);
   debug_pulse_digit(h);debug_pulse_digit(t);debug_pulse_digit(o);
   if(n) debug_pulse_digit(1); // seventh pulse indicates negative
   delayms(1000); //pause between numbers
}

float get_gps(int request)
{  float r;
   if(request==utc_hours)
      r=(gps[utc_hours+0]-48)*10+(gps[utc_hours+1]-48);
   else if(request==utc_minutes)
      r=(gps[utc_minutes+0]-48)*10+(gps[utc_minutes+1]-48);
   else if(request==utc_seconds)
      r=(gps[utc_seconds+0]-48)*10+(gps[utc_seconds+1]-48);
   else if(request==latitude)
   {  //accuracy: DDMM -- .MMMM ignored; minutes converted to decimal degrees
      r=(gps[latitude+0]-48)*10+(gps[latitude+1]-48)+(float)((gps[latitude+2]-48)*10+(gps[latitude+3]-48))/60;
      if(gps[latitude_hemisphere]=='S') r=-r; // apply sign
   }
   else if(request==longitude)
   {  //accuracy: DDDMM -- .MMMM ignored; minutes converted to decimal degrees
      r=(gps[longitude+0]-48)*100+(gps[longitude+1]-48)*10+(gps[longitude+2]-48) \
           +(float)((gps[longitude+3]-48)* 10+(gps[longitude+4]-48))/60;
      if(gps[longitude_hemisphere]=='W') r=-r; // apply sign
   }
   else if(request==speed)
      //fyi: mph = 6076/5280 * knots ~= 1.15*knots; speed below is in knots 
      r=(gps[speed+0]-48)*100+(gps[speed+1]-48)*10+(gps[speed+2]-48)+(float)(gps[speed+3]-48)*0.1;
   else if(request==heading)
      r=(gps[heading+0]-48)*100+(gps[heading+1]-48)*10+(gps[heading+2]-48)+(float)(gps[heading+3]-48)*0.1;
   else if(request==utc_day)
      r=(gps[utc_day+0]-48)*10+(gps[utc_day+1]-48);
   else if(request==utc_month)
      r=(gps[utc_month+0]-48)*10+(gps[utc_month+1]-48);
   else if(request==utc_year)
      r=(gps[utc_year+0]-48)*10+(gps[utc_year+1]-48);
   else
   {
      debug(555);
      while(1); // error
      r = 0;
   }
   return r;
}

void init_gps()  
{  //delayms(40000); //may require delay on cold start for gps module to acquire satellites
   debug_init();
   serial_set_baud_rate(4800);
   char buf[1];
 	// request 1 char at a time from gps and copy to char array gps[]
   int gps_index, byte_count;
   for(gps_index=0;gps_index<=649;gps_index++)
   {  serial_receive(buf,1);
	   byte_count = serial_get_received_bytes();
	   while(byte_count == 0) byte_count = serial_get_received_bytes();
	   gps[gps_index]=buf[0];
   }
   //find first instance of "$GPRMC" in gps[]
   char type[6] = "$GPRMC";
   int type_matched = 0;
   int type_index;

   for(gps_index=0;gps_index<=644;gps_index++)
   {  for(type_index=0; type_index<=5;type_index++)
	   {  if(gps[gps_index+type_index] == type[type_index]) type_matched=1;
		   else 
		   {  type_matched = 0;
		      break;
         }
	   } 
	  if(type_matched) break;
   }
   // upon exit type_matched==1 and gps_index <645 else error...

   if(!type_matched)
   {  debug(555); // my arbitrary error code is 555
      while(1);
   } 
   // if we made it this far, everything ok

	//move data to start of array...

	for(int k=0;k<=65;k++) gps[k] = gps[gps_index+k];
}

int main()
{
   init_gps();

   // at this point you  may access gps values captured using 'get_gps(<keyword>)'
   // where <keyword> is any of: utc_hours, utc_minutes, utc_seconds, latitude,
   // longitude, speed, heading, utc_day, utc_month, utc_year
   // return type is float
   // note:
   // [-]longitude -> west of prime meridian, [-]latitude -> south of equator
   //example queries...
   //float lon = get_gps(longitude);
   //debug(lon); 
   //debug(get_gps(latitude));
   //delayms(5000);
   //debug(get_gps(utc_hours));

   while(1); // wait forever
   return 0;
}


