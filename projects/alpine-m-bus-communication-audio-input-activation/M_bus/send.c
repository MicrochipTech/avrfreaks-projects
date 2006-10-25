/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 02/03/2006
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contrôleur     : AT TINY 26
Clock frequency       : 16,000000 MHz
*********************************************/
#include <avr/io.h>
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"

/*********************************************************************************/
void send(unsigned char nmbr)                                   // Send on M-BUS
{
    unsigned char n;
    unsigned char m;
    unsigned char s_temp;

    while (bus_busy>0);                             //wait if there is receiving
    GIMSK &= 0xbf;                                      //INT0 interrupt disable
    TIMSK = 0x20;                                           //T1 compare 1B interrupt enable (T0 disable)

                                                                        //PWM settings (use OC1B)
    TCCR1A = 0x21;                                      //COM1B1=H, COM1B0=L, PWM1B=H
    TCCR1B = 0x80;                                      //CTC1=H, T1-stoped
    OCR1C = 187;                                            //upper limit T1 (3000us)
    for(m=0;m<nmbr;m++)
    {
        s_temp=*(S_BUS+m);                              //copy byte to temp
        for(n=4;n>0;n--)                                    //4 times - 4 bits
        {
                                                //put the value to compare register
            if(s_temp&0x08) OCR1B = 112;                        //if bit is 1 (1800us)
            else            OCR1B = 37;                     //if bit is 0 (600us)
            TCCR1B = 0x85;                                      //start T1 with prescaler clk/16
            s_temp = (s_temp << 1);                         //shift temp left 1 time
            while (s_flag==0);
            s_flag = 0;                             //reset flag

        }
    }
    TCCR1B = 0x80;                                      //CTC1=H, T1-stoped
    TIMSK = 0x02;                                       //T0 overflow interrupt enable
    MCUCR = 0x03;                                       //INT0 rising edge sensitive
    GIMSK |= 0x40;                                      //INT0 interrupt enable
    s_flag = 0;                                     //reset flag
    bus_busy=0;                                     //reset flag "bus is busy"
}
