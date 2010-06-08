# HD44780 char LCD interface

Uploaded by banerjen on 2010-06-08 13:57:00 (rating 0 out of 5)

## Summary

This code uses the PC0 to PC7 pins as the 8-bit data bus and PA7-PA5 as the control lines.


This code works fine for me. If while implementing any problem in encountered, then I would be glad to help. 


Code:


// LCD.h file for a 16*2 char LCD interfaced with the ATMEL ATMega32 MCU  

// By - Nandan Banerjee (08/CSE/15)  

// Date - 06/05/2010  

// Compiled using AVR-GCC (WinAVR)  

// IDE - AVRStudio 4.13


// 8-bit communication mode for the HD44780  

// I am using the internal RC oscillator of 1 MHz with +/- 3 %  

// tolerance. 


#include stdio.h  

#include inttypes.h


#include avr/io.h  

#include avr/pgmspace.h


#define F\_CPU 1000000  

#include util/delay.h


#define D0 PC0  

#define D1 PC1  

#define D2 PC2  

#define D3 PC3  

#define D4 PC4  

#define D5 PC5  

#define D6 PC6  

#define D7 PC7


#define DATA\_PORT PORTC


#define RS PA7  

#define RW PA6 // Will be 0 most of the time since we will be writing  

#define E PA5


#define COMM\_PORT PORTA


void lcd\_set\_write\_instruction()  

{  

 COMM\_PORT &= ~(1<<RS);  

 \_delay\_us(10);  

}


void lcd\_set\_write\_data()  

{  

 COMM\_PORT |= (1<<RS);  

 \_delay\_us(10);  

}


void lcd\_write\_byte(char c)  

{  

 DATA\_PORT = c;


 // E toggle  

 COMM\_PORT |= (1<<E);  

 \_delay\_us(2);  

 COMM\_PORT &= ~(1<<E);  

 \_delay\_ms(10); 


}


void lcd\_clear\_and\_home()  

{  

 lcd\_set\_write\_instruction();  

 lcd\_write\_byte(0x01);  

 \_delay\_ms(50);  

 lcd\_write\_byte(0x02);  

 \_delay\_ms(50);  

}


void lcd\_home()  

{  

 lcd\_set\_write\_instruction();  

 lcd\_write\_byte(0x02);  

 \_delay\_ms(50);  

}


void lcd\_write\_data(char c)  

{  

 lcd\_set\_write\_data();  

 lcd\_write\_byte(c);  

}


void lcd\_write\_string(const char *x)  

{  

 while (pgm\_read\_byte(x) != 0x00)  

 lcd\_write\_data(pgm\_read\_byte(x++));  

}


void lcd\_write\_int16(int16\_t in)  

{  

 uint8\_t started = 0;  

 uint16\_t pow = 10000;


 while (pow >= 1)  

 {  

 if (in/pow > 0 || started || pow == 1)  

 {  

 lcd\_write\_data((uint8\_t) (in/pow) + '0');  

 started = 1;  

 in = in % pow;  

 }  

 pow = pow / 10;  

 }  

}


void lcd\_goto(uint8\_t line, uint8\_t pos)  

{  

 lcd\_set\_write\_instruction();


 uint8\_t position = 0;


 switch(line)  

 {  

 case 0: position = 0;  

 break;  

 case 1: position = 0x40;  

 break;  

 }


 lcd\_write\_byte(0x80 | (position + pos));  

}


void lcd\_line\_one() { lcd\_goto(0, 0); }  

void lcd\_line\_two() { lcd\_goto(1, 0); }


void lcd\_init()  

{  

 MCUCSR |= (1<<7);  

 MCUCSR |= (1<<7); // Setting the JTD bit in MCU control  

 // and status  

 MCUCSR |= (1<<7); // register 1 which disables the JTAG on  

 // pins PC2-PC5  

 MCUCSR |= (1<<7);


 DDRC |= 0xFF;  

 DDRA |= ((1<<RS) | (1<<RW) | (1<<E));  

 PORTA |= (1<<E);  

 PORTA &= ~(1<<RW);


 \_delay\_ms(100);


 lcd\_set\_write\_instruction();


 lcd\_write\_byte(0x38); // Set Data length as 8 (DL bit set) and  

 \_delay\_ms(10); // no. of display lines to 2 (N bit set)


 lcd\_write\_byte(0x0c); // Enable LCD (D bit set)  

 \_delay\_ms(10); 


 lcd\_write\_byte(0x01); // Clear the LCD display  

 \_delay\_ms(10); 


 lcd\_write\_byte(0x06); // Set entry mode: Increment cursor by 1  

 \_delay\_ms(10); // after data read/write (I/D bit set)


 lcd\_write\_byte(0x14); // Cursor shift  

 \_delay\_ms(10); 


 lcd\_clear\_and\_home(); // LCD cleared and cursor is brought at  

 \_delay\_ms(10); // the beginning  

}

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
