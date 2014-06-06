# MOBILE USING AVR ATMEGA8

Uploaded by admin on 2014-06-05 18:45:00 (rating 0 out of 5)

## Summary

Project aims at designing a mobile interfaced with a 4wire resistive touchscreen.  

HARDWARE: GSM MODULE SIM900,4WIRE RESISTIVE TOUCHSCREEN,AVR ATMEGA8


CODE:  

/***********************************************************************  

***********************************************************************/  

#include //ddrx portx x=b,d,c  

#include //for interrupt  

#define F\_CPU 12000000 //for freq.  

#include //\_delay\_ms();  

#include"lcd118010.h"  

#include"delay.h"  

#include"adc.h"  

#include"uart.h"  

void start();  

int touchvalue();  

unsigned char a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,day,xcod,ycod,p,pp;  

unsigned char msg[60],ms=0;  

unsigned char ph[10],cc=0;  

ISR (INT1\_vect)  

{  

 lcd\_clear();  

 lcd\_gotoxy(0,1);  

 uart\_init();  

 uart\_string("AT+CMGF=1\r\n");  

 \_delay\_ms(200);  

 uart\_string("AT+CMGS=\"7838562166\"");  

 \_delay\_ms(200);  

 uart\_string("HELP ME");  

 lcd\_string("HELP ME");  

 uart\_char(0x1a);  

 \_delay\_ms(200);  

 while(1)  

 {  

 lcd\_gotoxy(1,1);  

 lcd\_string("sending...");  

 \_delay\_ms(3000);  

 lcd\_clear();  

 start();  

 }


}


void xaxis() // reading X cordinate  

 {  

 DDRC=0x05;  

 PORTC=0x01;  

 \_delay\_ms(1);  

 xcod=getdata(1);  

 // \_delay\_ms(100);  

 }


void yaxis() // reading Y coordinate  

 {  

 DDRC=0x0A;  

 PORTC=0x08;  

 \_delay\_ms(1);  

 ycod=getdata(0);  

 //\_delay\_ms(100);  

 }


void main()  

{  

 adc\_init();  

 DDRC=0x0A;  

 PORTC=0x01;  

 \_delay\_ms(100);  

while((PINC & 0x04)==0x04);  

 lcd\_init();  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,2);  

 lcd\_string("START MOBILE");  

 \_delay\_ms(3000);  

 lcd\_clear();  

 \_delay\_ms(200);  

 start();


}


void start()  

{  

p=0;  

\_delay\_ms(300);  

while(1){  

 xaxis(); //xcod //calling function of xaxis  

 yaxis(); //ycod  

 p=touch\_value();  

 lcd\_gotoxy(1,10);  

 lcd\_showvalue(p);  

 if(p==8)  

 {


 gsm\_clk();


 }  

 lcd\_gotoxy(0,3);  

 lcd\_string("8-GSM");  

 MCUCR=0x04; //any logic change pg 149  

 GICR=0x80;  

 sei();  

 }  

}


 int touch\_value()  

 {  

 unsigned char x;  

 if(xcod >100 & xcod<140 & ycod<80)  

 {  

 x=1;  

 }  

 else if(xcod>100 & xcod<140 & ycod>80 & ycod<150)  

 {  

 x=2;  

 }  

 else if(xcod>100 & xcod<140 & ycod>150)  

 {  

 x=3;  

 }  

 else if(xcod>140 & xcod<175 & ycod<80)  

 {  

 x=4;  

 }  

 else if(xcod>140 & xcod<175 & ycod>80 & ycod<150)  

 {  

 x=5;  

 }  

 else if(xcod>140 & xcod<175 & ycod>150)  

 {  

 x=6;  

 }  

 else if(xcod>175 & xcod<210 & ycod<80)  

 {  

 x=7;  

 }  

 else if(xcod>175 & xcod<210 & ycod>80 & ycod<150)  

 {  

 x=8;  

 }  

 else if(xcod>175 & xcod<210 & ycod>150)  

 {  

 x=9;  

 }  

 else if(xcod>210 & ycod>80 & ycod<150)  

 {  

 x=0 ;  

 }  

 else if(xcod>210 & ycod<80)  

 {  

 x=91;  

 }  

 else if(xcod>210 & ycod>150)  

 {  

 x=93;  

 }  

 else if(xcod>0 & xcod< 60 & ycod<80)  

 {  

 x=11;  

 }  

 else if(xcod<60 & ycod>80 & ycod<150)  

 {  

 x=12;  

 }  

 else if(xcod<60 & ycod>150)  

 {  

 x=13;  

 }  

 else if(xcod>60 & xcod<100 & ycod<80)  

 {  

 x=21;  

 }  

 else if(xcod>60 & xcod<100 & ycod>80 & ycod<150)  

 {  

 x=22;  

 }  

 else if(xcod>60 & xcod<100 & ycod>150)  

 {  

 x=23;  

 }  

return x;  

 }


 int gsm\_clk()  

 {  

 p=0;  

 \_delay\_ms(1000);  

 {  

 lcd\_clear();  

 while(1){  

 lcd\_gotoxy(0,0);  

 lcd\_string("7-call");  

 lcd\_gotoxy(0,9);  

 lcd\_string(" 8-msg");  

 lcd\_gotoxy(1,0);  

 lcd\_string("9-exit");  

 lcd\_gotoxy(1,9);  

 lcd\_string("1-rec");


 xaxis();//toushscreen x input  

 yaxis();//touchscreen y input  

 p=touch\_value();  

 switch(p)  

 {


 case 7:  

 lcd\_clear();  

 call();  

 break;  

 case 8:  

 lcd\_clear();  

 msg\_();  

 break;  

 case 9:  

 lcd\_clear();  

 start();  

 break;


 case 1:  

 lcd\_clear();  

 receive();  

 break;  

 }  

 } }  

 return 0;  

 }


 int call()  

 {  

 cc=0;  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 lcd\_string("enter no");  

 lcd\_gotoxy(1,1);  

 while(cc<10){  

 adc\_init();  

 DDRC=0x0A;  

 PORTC=0x01;  

 \_delay\_ms(100);  

 while((PINC & 0x04)==0x04);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 \_delay\_ms(500);  

 ph[cc]=p;  

 lcd\_showvalue\_F(p);  

 cc++;  

 }  

 uart\_init();  

 uart\_string("ATD");  

 //for(cc=0;cc<10;c++)  

 {  

 uart\_char(ph[0]+48);  

 uart\_char(ph[1]+48);  

 uart\_char(ph[2]+48);  

 uart\_char(ph[3]+48);  

 uart\_char(ph[4]+48);  

 uart\_char(ph[5]+48);  

 uart\_char(ph[6]+48);  

 uart\_char(ph[7]+48);  

 uart\_char(ph[8]+48);  

 uart\_char(ph[9]+48);  

 //\_delay\_ms(100);  

 }  

 uart\_string(";\r\n");  

 lcd\_gotoxy(0,1);  

 lcd\_string(" ");  

 lcd\_gotoxy(0,1);  

 lcd\_string("calling....");  

 while(1){  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 if(p==93)  

 {  

 uart\_string("ATH\r\n");  

 \_delay\_ms(1000);  

 lcd\_clear();  

 \_delay\_ms(200);  

 p=0;  

 start();  

 }


 }  

 }


 int receive()  

 {  

 uart\_string("ATA");  

 uart\_string("\r\n");  

 lcd\_gotoxy(0,1);  

 lcd\_string("in call");  

 \_delay\_ms(200);  

 \_delay\_ms(500);  

 // lcd\_clear();  

 //\_delay\_ms(200);  

 start();  

 return 0;  

 }


 int msg\_()  

 {  

 lcd\_clear();  

 \_delay\_ms(200);  

 while(1){  

 lcd\_gotoxy(0,0);  

 lcd\_string("7-write");  

 lcd\_gotoxy(0,10);  

 lcd\_string("6-temp");  

 lcd\_gotoxy(1,0);  

 lcd\_string("0-readmsg");  

 lcd\_gotoxy(1,10);  

 lcd\_string("5-delete");  

 \_delay\_ms(2000);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 switch(p){  

 case 7:  

 write\_msg();  

 break;  

 case 6:  

 use\_temp();  

 break;  

 case 0:  

 msg\_read();  

 break;  

 case 5:  

 del\_msg();  

 break;  

 }  

 }  

 }


 void msg\_read()  

 {


 unsigned char msg\_r,rr;  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 uart\_string("AT+CMGF=1\r\n");  

 uart\_string("AT+CMGR=1\r\n");  

 \_delay\_ms(200);  

 msg\_r=uart\_read();  

 \_delay\_ms(1000);  

 lcd\_string(msg\_r);  

 \_delay\_ms(5000);  

 lcd\_char('a');  

 start();  

 }


 void del\_msg()  

 {  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(1,1);  

 uart\_string("AT+CMGD=1");  

 uart\_string("\r\n");  

 lcd\_string("msg deleted");  

 \_delay\_ms(2000);  

 start();  

 }


 void write\_msg()  

 {  

 \_delay\_ms(500);  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,0);  

 while(ms<60)  

 {  

 adc\_init();  

 DDRC=0x0A;  

 PORTC=0x01;  

 \_delay\_ms(100);  

 while((PINC & 0x04)==0x04);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 \_delay\_ms(500);  

 if(p==91) //to send msg  

 {  

 no\_msg();  

 }  

 msg[ms]=p;  

 lcd\_showvalue\_F(p);  

 ms++;  

 }  

 }


 //number for sending message  

 void no\_msg()  

 {


 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 lcd\_string("enter no");  

 \_delay\_ms(500);  

 lcd\_gotoxy(1,2);  

 cc=0;  

 uart\_init();  

 while(cc<11){  

 adc\_init();  

 DDRC=0x0A;  

 PORTC=0x01;  

 \_delay\_ms(100);  

 while((PINC & 0x04)==0x04);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 \_delay\_ms(500);  

 ph[cc]=p;  

 lcd\_showvalue\_F(p);  

 cc++;  

 }


 send\_msg();


 }


 void no\_msg\_temp()  

 {


 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 lcd\_string("enter no");  

 \_delay\_ms(500);  

 lcd\_gotoxy(1,2);  

 cc=0;  

 uart\_init();  

 while(cc<11){  

 adc\_init();  

 DDRC=0x0A;  

 PORTC=0x01;  

 \_delay\_ms(100);  

 while((PINC & 0x04)==0x04);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 \_delay\_ms(500);  

 ph[cc]=p;  

 lcd\_showvalue\_F(p);  

 cc++;  

 }


 if(pp=7)  

 {  

 send\_msg\_temp2();  

 }  

 else  

 {  

 send\_msg\_temp1();  

 }


 }


 void use\_temp()  

 {  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,0);  

 lcd\_string("7-call me");  

 lcd\_gotoxy(1,0);  

 lcd\_string("5-i am busy");  

 \_delay\_ms(2000);  

 xaxis();  

 yaxis();  

 p=touch\_value();  

 /*switch(p)  

 {  

 pp=p;  

 case 7:  

 no\_msg\_temp();  

 /*msg[0]="c";  

 msg[1]="a";  

 msg[2]='l';  

 msg[3]='l';  

 msg[4]=' ';  

 msg[5]='m';  

 msg[6]='e';  

 */  

 // break;  

 /*case 8:  

 msg[0]='i';  

 msg[1]=' ';  

 msg[2]='a';  

 msg[3]='m';  

 msg[4]=' ';  

 msg[5]='b';  

 msg[6]='u';  

 msg[7]='s';  

 msg[8]='y';  

 break;  

 }  

 */  

 no\_msg\_temp();  

 }


 //sending message....


 void send\_msg()  

 {  

 ms=0;  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 uart\_init();  

 uart\_string("AT+CMGF=1\r\n");  

 \_delay\_ms(200);  

 uart\_string("AT+CMGS=");  

 uart\_string("\"");  

 //uart\_string(ph);  

 uart\_char(ph[0]+48);  

 uart\_char(ph[1]+48);  

 uart\_char(ph[2]+48);  

 uart\_char(ph[3]+48);  

 uart\_char(ph[4]+48);  

 uart\_char(ph[5]+48);  

 uart\_char(ph[6]+48);  

 uart\_char(ph[7]+48);  

 uart\_char(ph[8]+48);  

 uart\_char(ph[9]+48);


 uart\_string("\"");  

 \_delay\_ms(200);  

 uart\_string("\r\n");  

 \_delay\_ms(200);  

 uart\_char(msg[0]+48);  

 uart\_char(msg[1]+48);  

 uart\_char(msg[2]+48);  

 uart\_char(msg[3]+48);  

 uart\_char(msg[4]+48);  

 uart\_char(msg[5]+48);  

 uart\_char(msg[6]+48);  

 uart\_char(0x1a);  

 \_delay\_ms(200);  

 while(1)  

 {  

 lcd\_gotoxy(0,0);  

 lcd\_string("sending...");  

 \_delay\_ms(5000);  

 lcd\_clear();  

 \_delay\_ms(200);  

 start();  

 }


 }


void send\_msg\_temp1()  

 {  

 ms=0;  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 uart\_init();  

 uart\_string("AT+CMGF=1\r\n");  

 \_delay\_ms(200);  

 uart\_string("AT+CMGS=");  

 uart\_string("\"");  

 //uart\_string(ph);  

 uart\_char(ph[0]+48);  

 uart\_char(ph[1]+48);  

 uart\_char(ph[2]+48);  

 uart\_char(ph[3]+48);  

 uart\_char(ph[4]+48);  

 uart\_char(ph[5]+48);  

 uart\_char(ph[6]+48);  

 uart\_char(ph[7]+48);  

 uart\_char(ph[8]+48);  

 uart\_char(ph[9]+48);


 uart\_string("\"");  

 \_delay\_ms(200);  

 uart\_string("\r\n");  

 \_delay\_ms(200);


 uart\_string("i am busy");  

 uart\_char(0x1a);  

 \_delay\_ms(200);  

 while(1)  

 {  

 lcd\_gotoxy(0,0);  

 lcd\_string("sending");  

 \_delay\_ms(5000);  

 lcd\_clear();  

 \_delay\_ms(200);  

 start();  

 }


 }


void send\_msg\_temp2()  

 {  

 ms=0;  

 lcd\_clear();  

 \_delay\_ms(200);  

 lcd\_gotoxy(0,1);  

 uart\_init();  

 uart\_string("AT+CMGF=1\r\n");  

 \_delay\_ms(200);  

 uart\_string("AT+CMGS=");  

 uart\_string("\"");  

 //uart\_string(ph);  

 uart\_char(ph[0]+48);  

 uart\_char(ph[1]+48);  

 uart\_char(ph[2]+48);  

 uart\_char(ph[3]+48);  

 uart\_char(ph[4]+48);  

 uart\_char(ph[5]+48);  

 uart\_char(ph[6]+48);  

 uart\_char(ph[7]+48);  

 uart\_char(ph[8]+48);  

 uart\_char(ph[9]+48);


 uart\_string("\"");  

 \_delay\_ms(200);  

 uart\_string("\r\n");  

 \_delay\_ms(200);


 uart\_string("call me");  

 uart\_char(0x1a);  

 \_delay\_ms(200);  

 while(1)  

 {  

 lcd\_gotoxy(0,0);  

 lcd\_string("sending");  

 \_delay\_ms(5000);  

 lcd\_clear();  

 \_delay\_ms(200);  

 start();  

 }


}

## Tags

- Complete code
