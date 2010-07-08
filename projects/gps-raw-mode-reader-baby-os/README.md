# GPS raw mode reader for Baby O's

Uploaded by scottfromscott on 2010-07-07 23:57:00 (rating 0 out of 5)

## Summary

This is part of a larger project which would combine an electronic compass, a GPS, a solar panel and two motors into a portable solar PV panel tracker. The GPS module provides the latitude, longitude and UTC date and time required to calculate the sun's position for tracking.


Code follows...


/*  

A 'raw mode' data reader for the Parallax GPS receiver module #28146 (based on Polstar PMB-248)  

for use with the Baby Orangutan robot controller or with slight modifications, other Pololu orangutans  

(data stream is captured and parsed) [scottfromscott@bellsouth.net](mailto:scottfromscott@bellsouth.net)  

*/


#define F\_CPU 20000000UL // Baby Orangutan frequency (20MHz)  

#include   

#include   

#include  // uses F\_CPU to achieve us and ms delays  

#include  


#include   

#include   

#include 


char gps[650]; // array to store $GPRMS string


//format of Parallax GPS GPMRC string...  

//$GPMRC,HHMMSS,A,DDMM.MMMM,S,DDDMM.MMMM,S,KKK.K,HHH.H,DDMMYY,,,A*64<13>  

//0123456789012345678901234567890123456789012345678901234567890123456  

// 1 2 3 4 5 6  

// $GPRMS string locations for following data items...  

int utc\_hours=7, utc\_minutes=9, utc\_seconds=11, latitude=16, \  

 latitude\_hemisphere=26, longitude=28, longitude\_hemisphere=39, speed=41, \  

 heading=47, utc\_day=53, utc\_month=55, utc\_year=57;


void delayms(unsigned int time\_ms)  

{for (unsigned int i = 0; i < time\_ms; i++) \_delay\_ms(1);  

}


//the following 4 debug functions flash an external LED I attached to #2 pwm motor port  

//because the built-in RED LED is not available when the UART is being used  

void debug\_port(unsigned char pwm)  

{ OCR2A = 0;  

 OCR2B = pwm;  

}


void debug\_init()  

{ TCCR2A = 0xF3;  

 TCCR2B = 0x02;  

 OCR2A = OCR2B = 0;  

 DDRD |= (1 << PORTD3) | (1 << PORTD5) | (1 << PORTD6);  

 DDRB |= (1 << PORTB3);  

}


void debug\_pulse\_digit(int digit)  

{ // pulse external led 1..10 times depending on value of digit  

 // 1 -> 1 pulse; 2 -> 2 pulses; ... 9 -> 9 pulses; BUT 0 -> 10 pulses  

 int counter, freq=75, delay=250;  

 if(digit==0) digit = 10; // special case for 0  

 for(counter=1;counter<=digit;counter++)  

 { debug\_port(freq);  

 delayms(delay);  

 debug\_port(0);  

 delayms(delay);  

 }  

 delayms(1000);  

}


void debug(float v)  

{ // pulse 6 digits (hto.hto[-]) if negative, single pulse follows 6 pulse sequences  

 // example output: -23.5 -> 023.500- -> 10 pulses, 2 pulses, 3 pulses, 5 pulses, 10 pulses, 10 pulses, 1 pulse  

 int o,t,h,d,n=0;  

 if(v<0){v=-v;n=1;}  

 h=(int)(v/100);t=(int)((v-h*100)/10);o=(int)(v-t*10-h*100);  

 debug\_pulse\_digit(h);debug\_pulse\_digit(t);debug\_pulse\_digit(o);  

 d=(int)((v-h*100-t*10-o)*1000); // .hto -> hto.  

 h=(int)(d/100);t=(int)((d-h*100)/10);o=(int)(d-t*10-h*100);  

 debug\_pulse\_digit(h);debug\_pulse\_digit(t);debug\_pulse\_digit(o);  

 if(n) debug\_pulse\_digit(1); // seventh pulse indicates negative  

 delayms(1000); //pause between numbers  

}


float get\_gps(int request)  

{ float r;  

 if(request==utc\_hours)  

 r=(gps[utc\_hours+0]-48)*10+(gps[utc\_hours+1]-48);  

 else if(request==utc\_minutes)  

 r=(gps[utc\_minutes+0]-48)*10+(gps[utc\_minutes+1]-48);  

 else if(request==utc\_seconds)  

 r=(gps[utc\_seconds+0]-48)*10+(gps[utc\_seconds+1]-48);  

 else if(request==latitude)  

 { //accuracy: DDMM -- .MMMM ignored; minutes converted to decimal degrees  

 r=(gps[latitude+0]-48)*10+(gps[latitude+1]-48)+(float)((gps[latitude+2]-48)*10+(gps[latitude+3]-48))/60;  

 if(gps[latitude\_hemisphere]=='S') r=-r; // apply sign  

 }  

 else if(request==longitude)  

 { //accuracy: DDDMM -- .MMMM ignored; minutes converted to decimal degrees  

 r=(gps[longitude+0]-48)*100+(gps[longitude+1]-48)*10+(gps[longitude+2]-48) \  

 +(float)((gps[longitude+3]-48)* 10+(gps[longitude+4]-48))/60;  

 if(gps[longitude\_hemisphere]=='W') r=-r; // apply sign  

 }  

 else if(request==speed)  

 //fyi: mph = 6076/5280 * knots ~= 1.15*knots; speed below is in knots  

 r=(gps[speed+0]-48)*100+(gps[speed+1]-48)*10+(gps[speed+2]-48)+(float)(gps[speed+3]-48)*0.1;  

 else if(request==heading)  

 r=(gps[heading+0]-48)*100+(gps[heading+1]-48)*10+(gps[heading+2]-48)+(float)(gps[heading+3]-48)*0.1;  

 else if(request==utc\_day)  

 r=(gps[utc\_day+0]-48)*10+(gps[utc\_day+1]-48);  

 else if(request==utc\_month)  

 r=(gps[utc\_month+0]-48)*10+(gps[utc\_month+1]-48);  

 else if(request==utc\_year)  

 r=(gps[utc\_year+0]-48)*10+(gps[utc\_year+1]-48);  

 else  

 {  

 debug(555);  

 while(1); // error  

 r = 0;  

 }  

 return r;  

}


void init\_gps()  

{ //delayms(40000); //may require delay on cold start for gps module to acquire satellites  

 debug\_init();  

 serial\_set\_baud\_rate(4800);  

 char buf[1];  

 // request 1 char at a time from gps and copy to char array gps[]  

 int gps\_index, byte\_count;  

 for(gps\_index=0;gps\_index<=649;gps\_index++)  

 { serial\_receive(buf,1);  

 byte\_count = serial\_get\_received\_bytes();  

 while(byte\_count == 0) byte\_count = serial\_get\_received\_bytes();  

 gps[gps\_index]=buf[0];  

 }  

 //find first instance of "$GPRMC" in gps[]  

 char type[6] = "$GPRMC";  

 int type\_matched = 0;  

 int type\_index;


 for(gps\_index=0;gps\_index<=644;gps\_index++)  

 { for(type\_index=0; type\_index<=5;type\_index++)  

 { if(gps[gps\_index+type\_index] == type[type\_index]) type\_matched=1;  

 else  

 { type\_matched = 0;  

 break;  

 }  

 }  

 if(type\_matched) break;  

 }  

 // upon exit type\_matched==1 and gps\_index <645 else error...


 if(!type\_matched)  

 { debug(555); // my arbitrary error code is 555  

 while(1);  

 }  

 // if we made it this far, everything ok


 //move data to start of array...


 for(int k=0;k<=65;k++) gps[k] = gps[gps\_index+k];  

}


int main()  

{  

 init\_gps();


 // at this point you may access gps values captured using 'get\_gps()'  

 // where  is any of: utc\_hours, utc\_minutes, utc\_seconds, latitude,  

 // longitude, speed, heading, utc\_day, utc\_month, utc\_year  

 // return type is float  

 // note:  

 // [-]longitude -> west of prime meridian, [-]latitude -> south of equator  

 //example queries...  

 //float lon = get\_gps(longitude);  

 //debug(lon);  

 //debug(get\_gps(latitude));  

 //delayms(5000);  

 //debug(get\_gps(utc\_hours));


 while(1); // wait forever  

 return 0;  

}

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
