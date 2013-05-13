# obstacle avoiding robot

Uploaded by urs_ricky on 2013-05-13 08:42:00 (rating 0 out of 5)

## Summary

/******************************************************************************


Simple Obstacle Avoiding Robot Program.


Compiler : avr-gcc toolchain  

IDE: AVR Studio from Atmel


 NOTICE


#include   

#include 


#include "motor.h"


//Threshold Values For Sensor Triggering  

#define RTHRES 195  

#define CTHRES 275  

#define LTHRES 195


//Function To Initialize the ADC Module  

void InitADC()  

{  

 ADMUX=(1<<REFS0); // For Aref=AVcc;


 ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Rrescalar div factor =128


}


/* 


Function To Read ADC Channel


Argument: Channel Number between 0-7  

Return Value : Between 0-1023


*/  

uint16\_t ReadADC(uint8\_t ch)  

{  

 //Select ADC Channel ch must be 0-7  

 ch=ch&0b00000111;  

 ADMUX&=0b11100000;  

 ADMUX|=ch;


 //Start Single conversion


 ADCSRA|=(1<<ADSC);


 //Wait for conversion to complete  

 while(!(ADCSRA & (1<<ADIF)));


 //Clear ADIF by writing one to it


 //Note you may be wondering why we have write one to clear it


 //This is standard way of clearing bits in io as said in datasheets.  

 //The code writes '1' but it result in setting bit to '0' !!!


 ADCSRA|=(1<<ADIF);


 return(ADC);  

}  

/* 


Simple Wait Function


Argument: NONE  

Return Value : NONE


*/  

void Wait()  

{  

 uint8\_t i;  

 for(i=0;i<10;i++)  

 \_delay\_loop\_2(0);  

}


void main()  

{  

 //Initialize motor subsystem  

 MotorInit();


 //Init ADCs  

 InitADC();


 //Wait  

 Wait();


 //Start Moving Forword


 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise  

 MotorB(MOTOR\_CCW,255); //Motor B (left motor) moves Counter Clockwise


 while(1)  

 {


 //Varriable To store sensor values


 uint16\_t sensor\_l,sensor\_c,sensor\_r;


 //Read Sensor Values  

 sensor\_r=ReadADC(0);//Right Sensor  

 sensor\_c=ReadADC(1);//Center Sensor  

 sensor\_l=ReadADC(2);//Left Sensor


 if(sensor\_c > CTHRES)  

 {


 //Start Rotating Left  

 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise  

 MotorB(MOTOR\_CW,255); //Motor B (left motor) moves Clockwise


 //Now Rotate till Path is NOT Clear  

 while((sensor\_r >(RTHRES-10)) || (sensor\_c>(CTHRES-10)))  

 {  

 //Read Sensor Values  

 sensor\_r=ReadADC(0);//Right Sensor


 sensor\_c=ReadADC(1);//Center Sensor


 \_delay\_loop\_2(30000);  

 }


 //Now Path is Clear So Move Forword  

 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise


 MotorB(MOTOR\_CCW,255); //Motor B (left motor) moves Counter Clockwise


 }


 if(sensor\_r > RTHRES)  

 {


 //Start Rotating Left  

 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise


 MotorB(MOTOR\_CW,255); //Motor B (left motor) moves Clockwise


 //Now Rotate Until Path is Clear  

 while(sensor\_r >(RTHRES-10))  

 {  

 //Read Sensor Values


 sensor\_r=ReadADC(0);//Right Sensor


 \_delay\_loop\_2(30000);  

 }


 //Now Path is Clear So Move Forword  

 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise


 MotorB(MOTOR\_CCW,255); //Motor B (left motor) moves Counter Clockwise


 }


 if(sensor\_l > LTHRES)  

 {  

 //Start Rotating Right  

 MotorA(MOTOR\_CCW,255); //Motor A (right motor) moves Counter Clockwise


 MotorB(MOTOR\_CCW,255); //Motor B (left motor) moves Clockwise


 //Now Rotate Until Path is Clear  

 while(sensor\_l >(LTHRES-10))  

 {  

 //Read Sensor Values


 sensor\_l=ReadADC(2);//Left Sensor


 \_delay\_loop\_2(30000);  

 }


 //Now Path is Clear So Move Forword  

 MotorA(MOTOR\_CW,255); //Motor A (right motor) moves Clockwise


 MotorB(MOTOR\_CCW,255); //Motor B (left motor) moves Counter Clockwise


 }


 \_delay\_loop\_2(0);  

 }  

}

## Compilers

- AtmanAvr C/C++ IDE

## Tags

- Complete code
- AtmanAvr C/C++ IDE
