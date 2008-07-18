/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.5 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : SimpleIO
Version : 1.0
Date    : 9/21/2007
Author  : SRINIVASAN.M                    
Company : Scorpion Controls                        
Comments: 
First Task in IO Handeling..................

I have Adopted "SC"(Study Comparitively) method created by Scorpion Controls. 


Chip type           : ATmega8535
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 128
*****************************************************/

#include <mega8535.h>     

void delay(unsigned int);

void main(void)
{

unsigned char s;  
int delay;
PORTA=0x00;
DDRA=0x0F;   

while (1)
      {      
           s=PINA >> 4;
           PORTA=s;  
           delay(10000);
           
      };
}
void delay(unsigned int i)
{       
        for(;i==0;i--)
        ;
}

/*
while(1)
{
        if (PINA.4==0)
        {      
                PORTA.0=0; 
                delay(10000);
                PORTA.0=1;
        }
        else if (PINA.5==0) 
        {       
                PORTA.1=0;
                delay(10000);
                PORTA.1=1;
        }
        else if (PINA.6==0)
        {     
                PORTA.2=0;
                delay(10000);
                PORTA.2=1;
        }
        else if (PINA.7==0) 
        {      
                PORTA.3=0;
                delay(10000);
                PORTA.3=1;
        }
};
} 

*/