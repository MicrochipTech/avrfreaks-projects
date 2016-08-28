# obstacle avoider robot

Uploaded by linesh thakre on 2016-08-28 10:01:00 (rating 0 out of 5)

## Summary

/*  

Title: obstacle\_avoider.c  

Author: Linesh Thakre  

Date Created: 28/08/16  

Last Modified:28/08/16  

Purpose: Mini Project for my friends  

*/  

//set clock speed  

#define F\_CPU 16000000UL  

//these are the include files.   

#include <avr/io.h>  

#include <util/delay.h>


int main (void)  

 {  

   DDRB = 0xFF; //Output port  

   DDRA = 0x00; //input port  

   int left\_sensor = 0;  

   int right\_sensor = 0;  

    

   //create an infinite loop  

   while(1)   

    {  

      left\_sensor = (PINA & 0b00000100);  

      right\_sensor = (PINA & 0b00001000);  

   

          

      if((( left\_sensor==0b00000000) && (right\_sensor==0b00000000)))  

         PORTD=0b00001100;  //move forward



    


      if(left\_sensor==0b00000010)   

         

       {  

            PORTB = 0b00000000; //stop  

             \_delay\_ms(300);  

            PORTB = 0b00001001;    //move backward  

             \_delay\_ms(300);  

            \_delay\_ms(300);  

            PORTB=0b00001010;  //turn right  

             \_delay\_ms(300);    //wait   

            \_delay\_ms(300);  

      

     

        }  

       

      if(right\_sensor==0b00000001)   

         

       {  

             PORTB = 0b00000000; //stop  

             \_delay\_ms(300);  

               PORTB = 0b00001001;  //move backward  

              \_delay\_ms(300);  

             \_delay\_ms(300);  

               

             PORTB=0b00000101;  //turn left  

             \_delay\_ms(300);     //wait   

            \_delay\_ms(300);

## Links

- [obstacle avoder robo](https://community.atmel.com/lthakre18%40gmail.com)

## Compilers

- AVR Studio 3

## Tags

- Complete code with HW
- AVR Studio 3
- #ATmega16
- Fire and Security
- #atmega16 #adc #differential #differentialAdc #adc gain
