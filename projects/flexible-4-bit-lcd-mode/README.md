# Flexible 4 bit LCD mode

Uploaded by tepcuz on 2008-12-15 08:18:00 (rating 0 out of 5)

## Summary

In order to take an alternative way to drive 2x16 LCD's with 4 bit mode using Codevision. Therefore I've made some code that more flexible than codevision code library. 


Here we go....  

//Title : 4 Bit mode LCD for AVR  

//Author: tepcuz


#define LCD\_RS PORTD.3  

#define LCD\_E PORTD.4  

#define LCD\_D4 PORTD.5  

#define LCD\_D5 PORTD.6  

#define LCD\_D6 PORTD.7  

#define LCD\_D7 PORTB.0  

#define LCD\_STROBE ((LCD\_E=1),(LCD\_E=0))


void lcd\_write(unsigned char); //write nibble for 4 bit mode LCD


void lcd\_clear(void); //clear LCD display


void lcd\_puts(char flash * s); //write the string s located in flash to LCD


void lcd\_goto(unsigned char pos);//specified LCD cursors


void lcd\_init(void);//LCD initialitation for 4 bit Mode


void lcd\_putch(char);//write character data to LCD


void lcd\_putnum(char *strg);// write the string strg located in SRAM to the LCD


#define LINE1 0x00 // position of line1  

#define LINE2 0x40 // position of line2


void lcd\_write(unsigned char c)  

{  

 if(c & 0x80) LCD\_D7=1; else LCD\_D7=0;  

 if(c & 0x40) LCD\_D6=1; else LCD\_D6=0;  

 if(c & 0x20) LCD\_D5=1; else LCD\_D5=0;  

 if(c & 0x10) LCD\_D4=1; else LCD\_D4=0;  

 LCD\_STROBE;  

 if(c & 0x08) LCD\_D7=1; else LCD\_D7=0;  

 if(c & 0x04) LCD\_D6=1; else LCD\_D6=0;  

 if(c & 0x02) LCD\_D5=1; else LCD\_D5=0;  

 if(c & 0x01) LCD\_D4=1; else LCD\_D4=0;  

 LCD\_STROBE;  

 delay\_us(40);  

} 


void lcd\_clear(void)  

{  

 LCD\_RS=0;  

 lcd\_write(0x1);  

 delay\_ms(2);  

} 


void lcd\_puts(char flash *s)  

{  

 LCD\_RS=1;  

 while(*s) lcd\_write(*s++);  

} 


void lcd\_putch(unsigned char c)  

{  

 LCD\_RS = 1;  

 lcd\_write(c);  

} 


void lcd\_goto(unsigned char pos)  

{  

 LCD\_RS = 0;  

 lcd\_write(0x80 + pos);  

} 


void lcd\_init(void)  

{  

 LCD\_RS=0;  

 delay\_ms(15);  

 LCD\_D4=1;  

 LCD\_D5=1;  

 LCD\_STROBE;  

 delay\_ms(5);


 LCD\_STROBE;  

 delay\_us(100);


 LCD\_STROBE;  

 delay\_ms(5);


 LCD\_D4 = 0;  

 LCD\_STROBE;  

 delay\_us(40);


 lcd\_write(0x28);  

 lcd\_write(0x0C);  

 lcd\_write(0x06);  

 lcd\_write(0x01);  

} 


void lcd\_putnum(char *strg)  

{  

char k;  

LCD\_RS=1;  

while (k=*strg++) lcd\_write(k);  

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
