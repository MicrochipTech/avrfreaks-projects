# Scientific Calculator

Uploaded by Reshma Subash on 2015-11-03 10:26:00 (rating 1 out of 5)

## Summary

#include <avr/io.h>  

#include <util/delay.h>  

#include<stdlib.h>  

#include<math.h>  

#include<string.h>  

#define MrLCDsCrib PORTB  

#define DataDir\_MrLCDsCrib DDRB  

#define MrLCDsControl PORTC  

#define DataDir\_MrLCDsControl DDRC  

#define LightSwitch 7//enable  

#define ReadWrite 1//rw  

#define BiPolarMood 0//rs  

void Check\_IF\_MrLCD\_isBusy(void);  

void Peek\_A\_Boo(void);  

void Send\_A\_Command(unsigned char command);  

void Send\_A\_Character(unsigned char character);  

void Send\_A\_String(char *string);  

char buffer[25];  

char buffer1[25];  

int z;  

double a=0,b=0,c=0;  

double t;  

int y;  

char val;  

char value;  

double q;  

void Mohasebe(void);  

void Mohandes(void);  

void Alamat(int,int);  

float calc(void);  

int key()  

{


char KEY=1;  

      

    while(KEY)  

    {  

      

      

    DDRD=0b10000000;  

      

    if(bit\_is\_clear(PIND,PD0))  

    {  

      

      

    return 0;  

    KEY=0;  

    \_delay\_ms(50);  

    break;


    }  

    else if(bit\_is\_clear(PIND,PD1))  

    {  

      

    return 1;  

    KEY=0;


    \_delay\_ms(50);  

    break;


    }  

    else if(bit\_is\_clear(PIND,PD2))  

    {  

      

      

    return 2;  

    KEY=0;  

      

    \_delay\_ms(50);


    }  

    else if(bit\_is\_clear(PIND,PD3))  

      

    return 3;  

    KEY=0;  

      

    \_delay\_ms(50);


    }  

    DDRD=0b01000000;  

if(bit\_is\_clear(PIND,PD0))  

{


return 4;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD1))  

{


return 5;  

KEY=0;


\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD2))  

{  

return 6;  

KEY=0;


\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD3))  

{  

return 7;  

KEY=0;  

\_delay\_ms(50);  

}  

DDRD=0b00100000;  

if(bit\_is\_clear(PIND,PD0))  

{  

return 8;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD1))  

{  

return 9;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD2))  

{


return 14;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PIND,PD3))  

{  

return 15;  

KEY=0;  

\_delay\_ms(50);  

}  

DDRD=0b00010000;  

if(bit\_is\_clear(PIND,PD0))  

{  

return 11;  

KEY=0;  

\_delay\_ms(50);  

break;  

}  

if(bit\_is\_clear(PIND,PD1))  

{  

return 16;  

KEY=0;  

\_delay\_ms(50);  

break;  

}  

if(bit\_is\_clear(PIND,PD2))  

{  

return 17;


KEY=0;  

\_delay\_ms(50);  

break;  

}  

if(bit\_is\_clear(PIND,PD3))  

{  

return 77;  

KEY=0;  

\_delay\_ms(50);  

break;  

}  

DDRD=0x00;  

PORTD=0x0F;


DDRA=0b00010000;  

      

    if(bit\_is\_clear(PINA,PA0))  

    {  

      

    //Send\_A\_Character('4');  

      

    return 16;  

    KEY=0;  

    \_delay\_ms(50);  

    break;


    }  

    else if(bit\_is\_clear(PINA,PA1))  

    {  

    return 17;  

    KEY=0;


    \_delay\_ms(50);  

    break;


    }  

    else if(bit\_is\_clear(PINA,PA2))  

    {  

    return 18;  

    KEY=0;  

      

    \_delay\_ms(50);


    }  

    else if(bit\_is\_clear(PINA,PA3))  

    {  

    return 19;  

    KEY=0;  

      

    \_delay\_ms(50);


    }  

    DDRA=0b01000000;  

if(bit\_is\_clear(PINA,PA0))  

{


return 20;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA1))  

{


return 21;  

KEY=0;


\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA2))  

{  

return 22;  

KEY=0;


\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA3))  

{  

return 23;  

KEY=0;  

\_delay\_ms(50);  

}  

DDRA=0b00100000;  

if(bit\_is\_clear(PINA,PA0))  

{  

return 10;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA1))  

{  

return 11;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA2))  

{  

return 12;  

KEY=0;  

\_delay\_ms(50);  

}  

if(bit\_is\_clear(PINA,PA3))  

{  

return 13;  

KEY=0;  

\_delay\_ms(50);  

}


DDRA=0x00;  

PORTA=0x0F;  

}  

}  

int main(void)  

{


DDRA=0x00;  

PORTA=0x0F;  

DDRD=0x00;  

PORTD=0x0F;  

DataDir\_MrLCDsControl |= 1<<LightSwitch | 1<<ReadWrite | 1<<BiPolarMood;  

\_delay\_ms(15);


Send\_A\_Command(0x01); //Clear Screen 0x01 = 00000001  

\_delay\_ms(2);  

Send\_A\_Command(0x38);  

\_delay\_us(50);  

Send\_A\_Command(0b00001110);  

\_delay\_us(50);  

Send\_A\_Command(0x06);  

\_delay\_ms(50);


 


 


int y;  

PORTA=0x0F;  

PORTD=0x0F;  

while(1)  

{  

calc();  

}  

}  

float calc(void)  

{  

int loop=1;  

val=PIND;  

value=PINA;  

if(value!=0x0F|val!=0x0F)  

{


//Send\_A\_string("press a key");  

//Send\_A\_Command(0xc0);  

y=key();  

//itoa(y,buffer,10);  

//Send\_A\_string(buffer);  

\_delay\_ms(500);


if( y == 15 ){a = 0 ;b = 0 ;c = 0 ; Send\_A\_Command(0x01);return 0;}


if( y < 10 )  

{  

        a = (a*10)+y ;  

        itoa(y,buffer,10);  

        Send\_A\_string(buffer);  

        \_delay\_ms(100);  

}


 


if( y > 9 &&       y<16 ){  

        if( y == 15 ){a = 0 ;b = 0 ;c = 0 ;Send\_A\_Command(0x01);return 0;}  

          

          

        z = y ;  

        Alamat(y,1);  

        while(loop){  

        y = key();  

        \_delay\_ms(500);  

        if( y == 15 ){a = 0 ;b = 0 ;c = 0 ;Send\_A\_Command(0x01);return 0;}  

        if( y < 10 ){  

                b = (b*10)+y ;  

                itoa(y,buffer,10);  

                Send\_A\_string(buffer);  

                \_delay\_ms(50);   

        }else if(y == 14){  

                Send\_A\_Character('=');  

                Mohasebe();  

                y = 0 ;  

                loop = 0 ;  

        }  

        }  

} 


if( y >15 ){  

a=0;  

b=0;  

c=0;


 


Alamat(y , 2);  

z = y ;  

loop = 1 ;  

while(loop){  

y = key();  

\_delay\_ms(500);  

if( y == 15 ){a = 0 ;b = 0 ;c = 0 ;Send\_A\_Command(0x01);return 0;}  

if(y <  10){  

        a = (a*10) + y ;  

        itoa(y,buffer,10);  

        Send\_A\_string(buffer);  

        \_delay\_ms(50);  

}else if ( y == 14){  

        Send\_A\_Character('=');  

        Mohandes();  

}  

loop = 1 ;  

}  

}  

}  

return 0;  

}


 


void Mohasebe(void){                                            

if(z == 10)c = a / b ;  

if(z == 11)c = a * b ;  

if(z == 12)c = a - b ;  

if(z == 13)c = a + b ;  

 int i,j;  

i=c;  

itoa(i,buffer,10)  ;  

j=(c-i)*10000;  

itoa(j,buffer1,10);  

strcat(buffer,".");  

strcat(buffer,buffer1);  

Send\_A\_string(buffer);


 


\_delay\_ms(100);  

}  

double Emoji(int rr){  

q=1;  

int i=0;  

for(i=0;i<rr;i++)q = q * 2.71728 ;  

return q;      

}  

void Mohandes(void){  

t = (3.1415926535897932384626433832795/180)*a ;  

if(z == 16)c = sin(t) ;  

if(z == 17)c = cos(t) ;  

if(z == 18)c = tan(t) ;  

if(z == 19)c = 1/tan(t) ;  

if(z == 20)c = a*a ;  

if(z == 21)c = log(a) ;  

if(z == 22)c = sqrt(a) ;  

if(z == 23)c = Emoji(a) ;  

int i,j;  

i=c;  

itoa(i,buffer,10);  

j=(c-i)*10000;  

itoa(j,buffer1,10);  

strcat(buffer,".");  

strcat(buffer,buffer1);          

Send\_A\_string(buffer);  

\_delay\_ms(100);  

}


void Alamat(int Moji,int Halat){  

if(Halat == 1){  

        if(Moji == 10)Send\_A\_Character('/') ;  

        if(Moji == 11)Send\_A\_Character('*') ;  

        if(Moji == 12)Send\_A\_Character('-') ;  

        if(Moji == 13)Send\_A\_Character('+') ;   

        \_delay\_ms(100);        

}  

if(Halat == 2){  

        if(Moji == 16)Send\_A\_string("Sin ") ;  

        if(Moji == 17)Send\_A\_string("Cos ") ;  

       if(Moji == 18)Send\_A\_string("Tan ") ;  

        if(Moji == 19)Send\_A\_string("Cot ") ;    

        if(Moji == 20)Send\_A\_string("sqr") ;  

        //if(Moji == 21)lcd\_putsf("aCos") ;  

        if(Moji == 21)Send\_A\_string("Log ") ;  

        if(Moji == 22)Send\_A\_string("Sqrt ") ;    

        if(Moji == 23)Send\_A\_string("exp ") ;  

        \_delay\_ms(100);


}  

}


/*if( y > 15 ){  

lcd\_clear();  

a= 0 ; b = 0 ; c = 0;  

Alamat(y , 2);  

z = y ;  

Loop = 1 ;  

while(Loop){  

y = key();  

if( y == 15 ){a = 0 ;b = 0 ;c = 0 ;lcd\_clear();return 0 ;}  

if(y <  10){  

        a = (a*10) + y ;  

        itoa(y , lcd);  

        lcd\_puts(lcd);  

        delay\_ms(50);  

}else if ( y == 14)  

        lcd\_putchar('=');  

        Mohandes();  

}  

Loop = 1 ;  

}  

}  

return 0;  

}


 


}  

}


}


 


void Check\_IF\_MrLCD\_isBusy()  

{  

DataDir\_MrLCDsCrib = 0;  

MrLCDsControl |= 1<<ReadWrite;  

MrLCDsControl &= ~1<<BiPolarMood;


while (MrLCDsCrib >= 0x80)  

{  

Peek\_A\_Boo();  

}  

DataDir\_MrLCDsCrib = 0xFF; //0xFF means 0b11111111  

}


void Peek\_A\_Boo()  

{  

MrLCDsControl |= 1<<LightSwitch;  

asm volatile ("nop");  

asm volatile ("nop");  

MrLCDsControl &= ~1<<LightSwitch;  

}


void Send\_A\_Command(unsigned char command)  

{  

Check\_IF\_MrLCD\_isBusy();  

MrLCDsCrib = command;  

MrLCDsControl &= ~ ((1<<ReadWrite)|(1<<BiPolarMood));  

Peek\_A\_Boo();  

MrLCDsCrib = 0;  

}


void Send\_A\_Character(unsigned char character)  

{  

Check\_IF\_MrLCD\_isBusy();  

MrLCDsCrib = character;  

MrLCDsControl &= ~ (1<<ReadWrite);  

MrLCDsControl |= 1<<BiPolarMood;  

Peek\_A\_Boo();  

MrLCDsCrib = 0;  

}  

void Send\_A\_string(unsigned char *string)  

{  

int i;  

for(i=0;string[i];i++)  

{  

Send\_A\_Character(string[i]);  

}  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
