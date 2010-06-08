# Self Perpendicular parking robot

Uploaded by banerjen on 2010-06-08 11:04:00 (rating 0 out of 5)

## Summary

This project uses three Sharp GP2D120 sensors to determine the parking spot and then perpendicular park itself.


A link to my robot in action is given below - 


[http://www.youtube.com/watch?v=G...](http://www.youtube.com/watch?v=GZtVLyPoUnA)


The code is given below. It has practically no comments. So, if you are wondering how I put together my project, then please feel free to mail me.


#include   

#include   

#include   

#include 


#define F\_CPU 1000000  

#include 


#include "lcd.h"


uint8\_t H, L;


uint16\_t read\_ADC\_value()  

{  

 while (ADCSRA & (1<<ADSC))  

 {  

 // wait until the conversion is complete  

 }


 uint16\_t result = ADCL; // Low order AD Data register  

 uint16\_t temp = ADCH; // High order AD Data register


 result = result + (temp<<8); // 10 bit resolved ADC value


 return result;  

}


void resolve(int val)  

{  

 H = 0;  

 L = 0;


 int temp;  

 temp = val;


 if (val >= 1000)  

 {  

 val = val / 1000;  

 H = (val % 10);  

 }  

 val = temp;  

 if (temp >= 100)  

 {  

 val = val / 100;  

 H = (H * 10) + (val % 10);  

 }  

 val = temp;  

 if (temp >= 10)  

 {  

 val = val / 10;  

 L = (val % 10);  

 }  

 val = temp;  

 L = (L * 10) + (val % 10);  

}


uint16\_t inverse(uint16\_t val)  

{  

 uint16\_t temp, temp2, temp3;  

 temp = 10000 / val;  

 temp2 = temp * 100;  

 temp = 10000 - (val * temp);  

 temp = temp * 10;  

 temp3 = temp;  

 temp = temp / val;  

 temp2 = temp2 + temp * 10;  

 temp = temp3 - (val * temp);  

 temp = temp * 10;  

 temp = temp / val;  

 temp2 = temp2 + temp;  

 return temp2;  

}


void motor\_forward()  

{  

 PORTB |= ((1<<PB2));  

 PORTB &= ~((1<<PB3));  

}


void motor\_backward()  

{  

 PORTB &= ~(1<<PB2);  

 PORTB |= (1<<PB3);  

}


void motor\_right()  

{  

 PORTB |= ((1<<PB0));  

 PORTB &= ~((1<<PB1));  

 \_delay\_ms(10);  

}


void motor\_left()  

{  

 PORTB |= ((1<<PB1));  

 PORTB &= ~((1<<PB0));  

 \_delay\_ms(10);  

}


void straight()  

{  

 PORTB |= ((1<<PB0) | (1<<PB1));  

 \_delay\_ms(1000);  

 PORTB &= ~((1<<PB0) | (1<<PB1));  

}


void all\_stop()  

{  

 //straight();  

 PORTB &= ~((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3));  

}


int main()  

{  

 uint8\_t parking\_spot = 0, spot\_start = 0, parked = 0, park\_count = 0, backing = 0;//, correcting = 0;  

 uint8\_t sample, counter = 0, sensor = 1, s1 = 25, s2 = 25, s3 = 25, count = 0;  

 uint16\_t read\_val = 0;  

 uint16\_t A2D = 0;  

 uint16\_t x;  

 double y, c, m;


 lcd\_init();  

 lcd\_home();


 prog: DDRD |= (1<<PD6);  

 PORTD &= ~(1<<PD6);  

 DDRD &= ~(1<<PD6);


 while (PIND & (1<<PD6))  

 {  

 lcd\_home();  

 lcd\_write\_string(PSTR("PROGRAMMING MODE"));  

 }


 lcd\_clear\_and\_home();


 ADMUX |= (1<<MUX0); // Selecting the ADC1 pin out of the 8 ADC pins  

 ADCSRA |= ((1<<ADEN) | (1<<ADPS1) | (1<<ADPS0)); // Setting ADC freq. to [(1 MHz / 8) = 125 KHz]  

 ADCSRA |= (1<<ADSC); 


 DDRB |= ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3));


 while (1)  

 {  

 if (PIND & (1<<PD6))  

 {  

 PORTB &= ~((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3));  

 lcd\_clear\_and\_home();  

 goto prog;  

 }


 while (1)  

 {  

 lcd\_home();


 switch (sensor)  

 {  

 case 1: ADMUX &= ~((1<<MUX0) | (1<<MUX1));  

 lcd\_write\_string(PSTR("S1 - "));  

 break;  

 case 2: ADMUX &= ~(1<<MUX1);  

 ADMUX |= (1<<MUX0);  

 lcd\_goto(0, 9);  

 lcd\_write\_string(PSTR("S2 - "));  

 break;  

 case 3: ADMUX &= ~(1<<MUX0);  

 ADMUX |= (1<<MUX1);  

 lcd\_line\_two();  

 lcd\_write\_string(PSTR("S3 - "));  

 break;  

 }


 read\_val = 0;  

 A2D = 0;  

 x = 0;


 y = 0;


 for (sample = 0; sample < 10; sample++)  

 {  

 ADCSRA |= (1<<ADSC);  

 read\_val = read\_val + read\_ADC\_value();  

 }


 // 1432  

 counter = 0;  

 read\_val = (read\_val / 10);  

 A2D = read\_val;


 while (read\_val > 131)  

 {  

 counter++;  

 read\_val = read\_val - 131;  

 }


 read\_val = (((read\_val * 5) * 100) / 1023);  

 read\_val = read\_val + (64 * counter);  

 resolve(read\_val);


 if (sensor == 3)  

 sensor = 1;  

 else  

 sensor++;  

 if (A2D > 617)  

 {  

 lcd\_write\_string(PSTR("NR"));  

 // x = -3;  

 // continue;  

 }


 if (A2D <= 100)  

 {  

 lcd\_write\_string(PSTR("RG"));  

 // x = -25;  

 // continue;  

 }


 if (/*(A2D < 617) &&*/ (A2D > 577))  

 {  

 m = 0.0020;  

 c = 0.0952;  

 y = (((m) * ((double) A2D)) - ((c)));  

 x = y * 10000;  

 }


 if ((A2D > 205) && (A2D <= 577))  

 {  

 if (A2D > 287)  

 {  

 m = 0.0005;  

 c = 0.0385;  

 }  

 else  

 {  

 m = 0.0004;  

 c = 0.0031;  

 }  

 y = (((m) * ((double) A2D)) - ((c)));  

 x = y * 10000;  

 }


 if (/*(A2D > 101) && */(A2D <= 205))  

 {  

 m = 0.0003;  

 c = 0.0114;  

 y = (((m) * ((double) A2D)) + ((c)));  

 x = y * 10000;  

 }


 if (x > 0)  

 {  

 x = inverse(x);  

 resolve(x);  

 }  

 else  

 {  

 H = -x;  

 }  

 if (H <= 9)  

 lcd\_write\_int16(0);  

 lcd\_write\_int16(H);


 if (sensor == 2)  

 s1 = H; // Front sensor  

 else if (sensor == 3)  

 s2 = H; // Rear sensor  

 else if (sensor == 1)  

 s3 = H; // Right sensor


 count++;


 if (count == 3)  

 break;


 }


 count = 0;


 // Identifying the parking spot  

 if (parking\_spot == 0)  

 {  

 motor\_forward();  

 if (spot\_start == 0)  

 {  

 if (s3 > 10)  

 spot\_start = 1;  

 continue;  

 }  

 else if (spot\_start == 1)  

 {  

 if (s3 < 10)  

 {  

 \_delay\_ms(2000);  

 all\_stop();  

 spot\_start = 2;  

 parking\_spot = 1;  

 }  

 continue;  

 }  

 }


 // Now parking the car  

 if (!parked)  

 {


 if (backing == 4)  

 {  

 if (s2 < 6)  

 {  

 all\_stop();  

 lcd\_clear\_and\_home();  

 lcd\_write\_string(PSTR("PARKED"));  

 lcd\_line\_two();  

 lcd\_write\_string(PSTR("SUCCESSFULLY"));  

 parked = 1;  

 break;  

 }  

 }


 if (backing == 3)  

 {  

 if (s2 > 18)  

 {  

 all\_stop();  

 backing = 0;  

 }


 if (s3 > 18)  

 {  

 all\_stop();  

 backing = 0;  

 }  

 }


 if (backing == 2)  

 {  

 motor\_left();  

 motor\_forward();  

 backing = 3;  

 }


 if (backing == 1)  

 {  

 if (s2 < 14)  

 {  

 all\_stop();  

 park\_count++;  

 backing = 2;  

 }


 if (s1 < 10)  

 {  

 all\_stop();  

 straight();  

 backing = 4;  

 motor\_backward();  

 }  

 }


 if (backing == 0)  

 {  

 motor\_right();  

 motor\_backward();  

 backing = 1;  

 }


 }  

 }  

}

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
