# DHT22 C++

Uploaded by m.h.s on 2014-10-29 19:53:00 (rating 0 out of 5)

## Summary

/*****************************************************  

This program was produced by the  

CodeWizardAVR V1.25.9 Professional  

Project : Heater Machine  

Version : 1.0.0  

Date    : 7/18/2015  

Author  : M.H.Soufiyan                                                   


Chip type           : ATmega32-16  

Program type        : Application  

Clock frequency     : 8.000000 MHz  

Memory model        : Small  

External SRAM size  : 0  

Data Stack size     : 256  

*****************************************************/  

//#include <mega32.h>  

#include <mega16.h>  

#include<math.h>  

#include<delay.h>  

#include<stdio.h>  

#include<stdlib.h>


// Alphanumeric LCD Module functions  

#asm  

   .equ \_\_lcd\_port=0x1B ;PORTA  

#endasm  

#include <lcd.h>


#define up             PINC.2   

#define down           PINC.3   

#define set            PINC.4   

#define esc            PINC.5  

#define dht            PINB.0    

#define heater\_fan     PORTD.6   

#define humidity\_fan   PORTD.5   

#define motor\_left     PORTD.4  

#define motor\_right    PORTD.3  


////////////////////////////////////////////////////


///////////////////////////Variable//////////////////////////  

char buffer0[16]={0};  

char buffer1[16]={0};  

char tmp[6]={0};  

int  data[5]={0};  

float temp=0, hum=0 ;  

int  cntr=0 , result ,j=0 ,i=0 , flag=1 ,error\_flag=0 , reset=0;  

int  counter\_time=0, counter\_round=0 ,counter=0 ;  

eeprom  int second=0, minute=0, hour=0 ,day=10, counter\_relay = 0;  

eeprom  float temp\_ref=37.5, humidity\_ref =60 ,round\_ref=12, round=90;  

long int dat=0;


//////////////////////////////////////////////////////  

//////////////////////////Function(s)/\\\\\\\\\\\\\\\\\\\


void show\_value(void);  

void read\_dht22(void);  

void cleanbuffer(void);  

void config(void);  

/////////////////////////////////////////////////////////


      

interrupt [EXT\_INT0] void ext\_int0\_isr(void)  

{  

    if(dht == 1)  

        flag=1;  

    if(dht == 0)  

        flag=0;  

}


// Timer 2 overflow interrupt service routine  

interrupt [TIM2\_OVF] void timer2\_ovf\_isr(void)  

{  

    #asm("wdr")        

    counter\_time++;  

    counter\_round++;  

    second++ ;   

    if(second >= 60)  

    {  

        second=0;  

        counter\_round++;  

        minute++;      

    }  

    if(minute >= 60)  

    {  

        minute=0;  

        hour++;  

    }  

    if(hour >= 24)  

    {  

        hour=0;  

        day++;  

    }  

    if ((set ==0) && (esc ==0))  

    {  

        config ();  

        counter\_time=0;  

    }                

}    

////////////////////////////////////////////////////////////////  

void cleanbuffer(void)  

{  

    int i = 0;  

    for(i=0; i<16; i++)  

    {  

        buffer0[i]=0;  

        buffer1[i]=0;  

    }  

}


/////////////////////////Read Temperature & humidity\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  

void DHT\_Start(void)   

{   

    DDRB = 0x01;  

    PORTB.0 = 0; //datasheet  

    delay\_ms(25);      

    PORTB.0 = 1; //datasheet  

    delay\_us(20);  

    DDRB = 0x00;  

    //delay\_us(200);   

    // INT0: any change  

    GICR|=0x40;  

    MCUCR=0x01;  

    MCUCSR=0x00;  

    GIFR=0x40;  

    #asm("sei")      

} 


void read\_dht22(void)   

{      

    int data\_temp[50]={0};


    for(i=0; i<42 ; i++)  

    {  

        while(flag == 1)  

        {  

            //delay\_us(1);  

            counter++;         

        }  

        data\_temp[i] = counter;  

        counter=0;  

        while(flag == 0)  

            j++;      

    }  

    #asm("cli")  //disable intrrupt  

          

    for(i=2; i<34 ; i++ )  

    {  

    if(data\_temp[i]>28)  

        dat= ( dat << 1) | 1;              

    if((data\_temp[i]<20) && (data\_temp[i]> 3))  

        dat=( dat << 1);   

    }  

      

    for(i= 34; i<42; i++)  

    {  

    if(data\_temp[i]>28)  

        data[4]= ( data[4] << 1) | 1;              

    if((data\_temp[i]<20) && (data\_temp[i]> 3))  

        data[4]=( data[4] << 1);   

    }  

      

    show\_value();      

}


void show\_value(void)  

{     

    #asm("cli")  

    data[0]= (dat & 0xFF000000)>> 24 ;  

    data[1]= (dat & 0x00FF0000)>> 16 ;  

    data[2]= (dat & 0x0000FF00)>> 8 ;  

    data[3]= (dat & 0x000000FF) ;  

       

      if(data[4] != (data[0] + data[1] + data[2] + data[3]) & 0xFF)  

    {  

        lcd\_clear();  

        lcd\_gotoxy(0,0);  

        lcd\_putsf("Error In Read");  

        delay\_ms(1500);  

        error\_flag =1;  

        data[4]=0;  

        #asm("sei")  

        return;  

    }    

    error\_flag=0;  

    hum = (data[0] * 25.6 + data[1]*0.1);  

    temp = (data[2] & 0x7F)* 25.6 + data[3]*0.1;  

    

    if (data[2] & 0x80){  

      temp = temp * -1; // temp negativa;  

    }     

    lcd\_clear();  

    cleanbuffer();  

    lcd\_gotoxy(0,0);  

    lcd\_putsf("Temp.:");  

    ftoa(temp,2,tmp);  

    lcd\_gotoxy(8,0);  

    lcd\_puts(tmp);  

    lcd\_gotoxy(13,0);  

    lcd\_putsf("'C");  

      

    lcd\_gotoxy(0,1);  

    lcd\_putsf("Humidity:");  

    ftoa(hum,2,tmp);  

    lcd\_gotoxy(10,1);  

    lcd\_puts(tmp);  

    lcd\_gotoxy(15,1);  

    lcd\_putsf("%");  

    data[4]=0;  

    #asm("sei")   

    //return;  

}  

     

      

void timer (void)  

{  

    //lcd\_init(16);  

    //lcd\_clear();  

    cleanbuffer();  

    sprintf(buffer0, "Time :%2d:%2d:%2d",hour,minute,second);  

    lcd\_gotoxy(0,0);  

    lcd\_puts(buffer0);  

    sprintf(buffer1, "Day :%2dth",day);  

    lcd\_gotoxy(0,1);  

    lcd\_puts(buffer1);  

}


//up ==0 Up   // n = 0  

//down ==0 Down // n = 1  

//set ==0 ESC  // n = 2  

//esc ==0 set  // n = 3


char read\_key(void)  

{  

    int l=1;  

    int n=0,j=0;  

    while(j==0)  

    {      

        delay\_ms(l);  

        if(up == 0)  

        {  

            n=0; j++; return n;  

        }  

        delay\_ms(l);  

        if(down == 0)  

        {  

            n=1; j++; return n;  

        }  

        delay\_ms(l);  

        if(esc == 0)


        {  

            n=2; j++; cntr++; return n;  

        }  

        delay\_ms(l);  

        if(set == 0)


        {  

            n=3; j++; cntr++; return n;  

        }  

        delay\_ms(l);   

    }          

 }          

            

void config (void)  

{  

    lcd\_clear();  

    while(1)  

    {        

        lcd\_gotoxy(0,0);  

        lcd\_putsf("Change Config?");  

        lcd\_gotoxy(0,1);  

        lcd\_putsf("If Ok Press Set!");  

        delay\_ms(1500);  

        result = read\_key();  

        if(result == 3)  

        {  

            lcd\_clear();  

            cleanbuffer();  

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Plate Position");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("Up=Left Do.=Rig.");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 0)  

                {  

                    motor\_left = 1;  

                    delay\_ms(100);  

                    motor\_left = 0;   

                }  

                if(result == 1)  

                {  

                    motor\_right = 1;  

                    delay\_ms(100);  

                    motor\_right = 0;   

                }


                if(result == 2) //esc  

                    break;  

                if(result == 3)//set  

                    break;  

                delay\_ms(900);  

            }  

            lcd\_clear();  

            cleanbuffer();  

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Reset Date&Time");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("Yes=Set***No=ESC");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 2) //esc  

                    break;  

                if((result == 3) && (result == 2))//set  

                {  

                    second=0, minute=0, hour=0 ,day=0;  

                    break;  

                }  

                delay\_ms(900);  

            }  

            lcd\_clear();  

            cleanbuffer();  

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Reset Position");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("Yes=Set***No=ESC");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 2) //esc  

                    break;  

                if(result == 3)//set  

                {  

                    counter\_relay=0;  

                    break;  

                }  

                delay\_ms(900);  

            }  

            lcd\_clear();  

            cleanbuffer();              

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Set Temperature");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("up/down:");  

                ftoa(temp\_ref,2,tmp);  

                lcd\_gotoxy(8,1);  

                lcd\_puts(tmp);  

                lcd\_gotoxy(13,1);  

                lcd\_putsf("'C");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 0)  

                    temp\_ref = temp\_ref + 0.1;  

                if(result == 1 && temp\_ref > 0)  

                    temp\_ref = temp\_ref - 0.1;  

                if(result == 2) //esc  

                    break;  

                if(result == 3)//set  

                    break;  

                delay\_ms(900);  

            }  

            lcd\_clear();  

            cleanbuffer();  

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Set Humidity");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("up/down:");  

                ftoa(humidity\_ref,2,tmp);  

                lcd\_gotoxy(8,1);  

                lcd\_puts(tmp);  

                lcd\_gotoxy(13,1);  

                lcd\_putsf("%");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 0 && humidity\_ref < 100)  

                    humidity\_ref = humidity\_ref + 0.5;  

                if(result == 1 && humidity\_ref > 0.5)  

                    humidity\_ref = humidity\_ref - 0.5;  

                if(result == 2) //esc  

                    break;  

                if(result == 3)//set  

                    break;  

                delay\_ms(900);  

            }  

            lcd\_clear();  

            cleanbuffer();  

            while(1)  

            {  

                lcd\_gotoxy(0,0);  

                lcd\_putsf("Set round in day");  

                lcd\_gotoxy(0,1);  

                lcd\_putsf("up/down:");  

                ftoa(round\_ref,1,tmp);  

                lcd\_gotoxy(8,1);  

                lcd\_puts(tmp);  

                lcd\_gotoxy(12,1);  

                lcd\_putsf("Times");  

                result = read\_key();  

                if(cntr > 1)  

                {  

                    delay\_ms(1000);  

                    result=5;  

                    cntr=0;      

                }  

                if(result == 0 && round\_ref < 24)  

                    round\_ref = round\_ref + 0.5;  

                if(result == 1 && round\_ref > 1)  

                    round\_ref = round\_ref - 0.5;  

                      

                round = (86400 / round\_ref);  //Second in 1 day = 60*60*24  

                round = round / 72;


                if(result == 3)//set  

                {   

                    counter\_time=0;  

                    lcd\_clear();  

                    lcd\_init(16);  

                    return;  

                }  

                if(result == 2) //esc  

                {  

                    counter\_time=0;  

                    lcd\_clear();  

                    lcd\_init(16);                      

                    return;          

                }  

                delay\_ms(900);  

            }                              

        }   

    }  

}


 


void main (void)  

{        

    // Timer/Counter 2 initialization  

    // Clock source: TOSC1 pin  

    // Clock value: PCK2/128  

    // Mode: Normal top=FFh  

    // OC2 output: Disconnected  

    ASSR=0x08;  

    TCCR2=0x05;  

    TCNT2=0x00;  

    OCR2=0x80;  

    // Timer(s)/Counter(s) Interrupt(s) initialization  

    TIMSK=0x40;  

    DDRD = 0xF8;  

    DDRC = 0x00;  

    lcd\_init(16);   

      

    #pragma optsize-  

    WDTCR=0x1F;  

    WDTCR=0x0F;  

    #ifdef \_OPTIMIZE\_SIZE\_  

    #pragma optsize+  

    #endif  

    #asm("sei")  

    while(1)  

    {                   

         /*while(error\_flag == 1)  

        {  

            while(counter\_time < 5)  

                timer();  

            PORTD=0;  

            DHT\_Start();  

            read\_dht22();  

            counter\_time=0;  

        }    */               

                 

        if(counter\_round >= round)  

        {  

            counter\_relay++;  

            counter\_round=0;  

            if(counter\_relay < 10)  

            {  

               motor\_left = 1;  

               delay\_ms(100);  

               motor\_left = 0;   

            }  

            else if((counter\_relay < 30) && (counter\_relay > 9))  

            {  

               motor\_right = 1;  

               delay\_ms(100);  

               motor\_right = 0;   

            }                      

            else if((counter\_relay < 42) && (counter\_relay > 29))  

            {  

               motor\_left = 1;  

               delay\_ms(100);  

               motor\_left = 0;   

            }  

            else if(counter\_relay >= 42)  

                counter\_relay=0;  

            lcd\_init(16);                          

        }   

         

        if(counter\_time < 5)  

            timer();  

               

        else if(counter\_time == 5)  

        {  

            DHT\_Start();  

            read\_dht22();  

            if(temp < temp\_ref - 0.3)  

                heater\_fan = 1;  

            if(temp > temp\_ref + 0.3)  

                heater\_fan = 0;  

            if(hum < humidity\_ref - 5)  

                humidity\_fan = 1;  

            if(hum > humidity\_ref + 2)  

                humidity\_fan = 0;  

            while(counter\_time <7);  

                counter\_time=0;  

            lcd\_clear();  

        }  

        else if(counter\_time > 5)  

        {  

            counter\_time=0;  

            WDTCR=0x00;  

        }  

    }   

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
- ATmega16
- atmega16/32
