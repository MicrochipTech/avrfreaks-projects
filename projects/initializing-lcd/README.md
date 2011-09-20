# Initializing the LCD

Uploaded by sanjoy on 2011-09-19 17:27:00 (rating 0 out of 5)

## Summary

This is an extremely efficient code for initializing and writing few letters on the JHD162A 16x2 LCD dislay. code writtn using avrstudio5.on atmega16.with 4bit mode connections.  

modifications and suggestions welcomed. shortly i will be preparing a library file for display modifications.


pin connections can be given if interested..  

All operation has been performed bitwise as per HITACHI manual...


/*  

 * LCD\_INIT2.c  

 *  

 * Created: 9/18/2011 4:10:12 PM  

 * Author: SANJU  

 */  

#define F\_CPU 8000000UL  

#include   

#include   

#include   

#define wait1 \_delay\_ms(50)  

#define wait2 \_delay\_ms(1) 


char txt1[16]="HELLO WORLD !;";  

char txt2[16]="Fuck You !!! ";  

int i;


void clk(void)  

{  

 PORTA=0xff;  

 PORTA=0x00;  

 wait2; // clock pulse to EN  

}  

void clr\_disp(void)  

{  

 PORTD=0b00000000;  

 clk();  

 PORTD=0b00010000;  

 clk(); //clr disp.  

 wait2;  

 PORTD=0b00000000;  

 clk();  

 PORTD=0b00100000;  

 clk();  

 wait2; //ret.home


}


void writetxt1(void)  

{  

 int j;  

 i=strlen(txt1);  

 for (j=0;j
 {  

 PORTD=(txt1[j]|1) ;  

 clk();  

 PORTD=(txt1[j]<<4)|1; //write to LCD  

 clk();  

 wait2;


 }  

} 


void writetxt2(void)  

{  

 int j;  

 i=strlen(txt2);  

 PORTD=0b11000000;  

 clk();  

 PORTD=0b00000000;  

 clk(); //set cursor to 1st digit of 2nd line


 for (j=0;j
 {  

 PORTD=(txt2[j]|1) ;  

 clk();  

 PORTD=(txt2[j]<<4)|1; //write to LCD  

 clk();  

 wait2;


 }  

}


void csr\_on(void)  

{  

 PORTD=0b00000000;  

 clk();  

 PORTD=0b11110000;  

 clk(); //display on /off & cursor  

}  

void csr\_off(void)  

{  

 PORTD=0b00000000;  

 clk();  

 PORTD=0b11000000;  

 clk(); //display off & cursor  

}  

int main(void)  

{


 DDRA=0xff;  

 DDRD=0xff;  

 wait1;  

 PORTD=0b00110000;  

 clk();  

 PORTD=0b00110000;  

 clk();  

 PORTD=0b00110000;  

 clk(); //reset  

 wait1;


 PORTD=0b00100000;  

 clk();  

 PORTD=0b00100000;  

 clk();  

 PORTD=0b10000000; //4 bit op x 2 lines  

 //PORTD=0b00100000; //4bit operation select with 1 line display  

 clk(); 


 csr\_off();


 PORTD=0b00000000;  

 clk();  

 PORTD=0b01100000; // set entry mode to increment the addres by one and disp cursor shifted.  

 clk(); 


 clr\_disp();


 writetxt1();  

 writetxt2();


 \_delay\_ms(500);  

 wait1;  

 clr\_disp();


 main();


 }

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
