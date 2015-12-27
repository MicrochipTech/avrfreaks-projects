# simple programablle count down timer

Uploaded by Akhil vinayak on 2015-12-27 07:59:00 (rating 0 out of 5)

## Summary

/*  

 * switchint.c  

 *  

 * Created: 15-12-2015 6:45:47 PM  

 *  Author: Akhil vinayak  

 */ 


#include <avr/io.h>  

double e;  

double d;


int main(void)  

{  

DDRA=0xff;  

DDRB=0x00;  

PORTA=0x00;  

DDRD=0xff;  

e=0X00;  

    while(1)  

   {  

      

    if(PINB==0x01)  

    {  

        if(e==0xff)  

        {  

            PORTD=0xff;  

        }  

        inc();  

    }  

    if(PINB==0x02)  

    {     

        dec();     

        if(e==0x00)  

        {  

            PORTD=0xff;  

        pp:    goto pp;  

        }          

    }  

    if (PINB==0x03)//timer reset without reset cpu  

    {  

        PORTA=0x00;  

    }                  

   }  

}


delay(d)// function to delay  

{  

    double i,m;  

    for(i=0;i<d;i++)  

    {  

        for(m=0;m<i;m++);  

    }  

}


void inc() //fn to incerment to set time  

   {  

       delay(25);  

       PORTA=++e;  

       delay(25);  

         

   }  

void dec()//function to count down  

{  

    {  

       delay(80);  

       PORTA=--e;  

       delay(80);  

         

   }


}


 


 


 


//this is a simple programe

## Compilers

- AVR Studio 5

## Tags

- Complete code with HW
- AVR Studio 5
