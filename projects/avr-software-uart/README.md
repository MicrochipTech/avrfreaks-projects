# avr software uart

Uploaded by babukhan on 2011-01-12 11:09:00 (rating 0 out of 5)

## Summary

hi,  

recently i am working with avr software uart.but my code is not working.i am working with atmega32.i am using icp1 pin only.first timer1 is started at normal mode.after chaptering the start beat timer1 is in ctc mode and icp1 resister is used as top value and icp1 pin is used as rx pin.here is my code..


#include  

#include  

#include  

#include  

#include   

#define F\_CPU 8000000UL  

#define usart\_baudrate 9600UL  

#define baud\_prescaler (((F\_CPU / (usart\_baudrate * 16))) - 1)  

#define PD6 6  

void usart\_to\_send(char *string\_data);  

void send\_hex(void);  

void usart\_init(void);  

void usart\_send(char value);  

void timer1\_init(void);  

void capture\_bit(void);  

void timer1\_ctc\_init(void);  

int flag;  

void main(void)  

{  

 usart\_init();  

 flag=0;  

 timer1\_init();  

 while(flag=0);//wait untill flag is set  

 timer1\_ctc\_init();  

 while(flag=1);//wait till flag is set  

} 


ISR(TIMER1\_CAPT\_vect)  

{  

 static unsigned char raw\_data,count=0,gps\_data[250];


 static int counter=0,i=0;  

 unsigned char uart\_data;


 counter++;  

 if(flag==0)  

 capture\_bit();  

 else  

 {  

 if(count<8)  

 {  

 uart\_data=PIND;  

 uart\_data = (uart\_data>>6);  

 raw\_data |= (uart\_data<<count);  

 count++;  

 }  

 else if(count==8)  

 {  

 gps\_data[i]=raw\_data;  

 count++;  

 i++;  

 if(counter==2000)  

 {  

 TCCR1B=0x00;//stop timer1.  

 gps\_data[i]='\0';  

 usart\_to\_send(gps\_data);  

 }  

 }  

 else  

 {  

 count=0;  

 }  

 } 


}  

void timer1\_init(void)  

{  

 DDRD=0x00;  

 // PORTD |=0XFF;  

 TIMSK |=(1<<TICIE1);//timer1 input capture interrupt enable  

 sei();//global interrupt enable.  

 TCCR1B &=~(1<<ICES1);//falling edge is used as trigger.  

 TCCR1B |=(1<<CS11);//timer1 starts in normal mode at prescalare of 8.  

}  

void capture\_bit(void)  

{  

 static unsigned int first\_capture\_value,i=0,second\_capture\_value;  

 unsigned int start\_bit\_length;  

 if(i=0)  

 {  

 TCCR1B |=(1<<ICES1);//raiging edge is used as trigger.  

 first\_capture\_value=ICR1;  

 i=1;  

 }  

 else  

 {  

 second\_capture\_value=ICR1;  

 start\_bit\_length=(second\_capture\_value-first\_capture\_value);  

 if(start\_bit\_length >= 270)  

 {  

 TCCR1B=0x00;//stop timer1.  

 flag=1;  

 }  

 else  

 {  

 TCCR1B &=~(1<<ICES1);//again falling edge is used as trigger.  

 first\_capture\_value=0;  

 second\_capture\_value=0;  

 i=0;  

 }  

 } 


}  

void timer1\_ctc\_init(void)  

{  

 DDRD= 0x00;  

 // PORTD |=0XFF;  

 //TCCR1B=0x00;//stop timer1.  

 TIMSK |=(1<<TICIE1);//timer1 input capture interrupt enable  

 sei();//global interrupt enable.  

 TCCR1B |=(1<<WGM13)|(1<<WGM12);//set timer at ctc mode and ICR1 as top value;  

 ICR1=207;//compare value;  

 TCCR1B |=(1<<CS11);//start timer1 at prescaler of 8;


}  

void usart\_to\_send(char *string\_data)  

 {  

 while(*string\_data)  

 usart\_send(*string\_data++);  

 }  

void send\_hex(void)  

{  

 char z = 0x1A;


 usart\_send(z);  

}  

void usart\_init(void)  

{


 unsigned int ubrrval;  

 ubrrval =((F\_CPU/(16*usart\_baudrate))-1);  

 UBRRH=ubrrval>>8; // shifting the full data 8 bit to get the high byte of data of 2 byte.  

 UBRRL=ubrrval;//now the trancated value(8 bit) is loaded in the lower byte.  

 UCSRC|=(1<<URSEL)|(3<<UCSZ0);// register select, stop bit 1, char size 8 bit select.  

 UCSRB|=(1<<TXEN)|(1<<RXEN);//|(1<<RXCIE);//receive and transmit enable. AND RX INT ENABLE.  

 //sei();  

}  

void usart\_send(char value)  

{  

 char z=value;  

 while((UCSRA&(1<<UDRE))==0);  

 UDR=z;  

}  

please help me to solve the problem. :roll:

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
