# Customizable LCD Connection

Uploaded by mhsnrah on 2009-09-14 02:00:00 (rating 0 out of 5)

## Summary

This shows how to connect an LCD to every uc pin you want!


Code:


#include   

#define F\_CPU 8000000UL  

#include 


#define SBI(x,y) (x |= (1<<y));  

#define CBI(x,y) (x &= (~(1<<y)));


/*----- LCD Pins -----*/  

// RS  

#define LCD\_DDR\_RS DDRD  

#define LCD\_PRT\_RS PORTD  

#define LCD\_RS 4  

// EN  

#define LCD\_DDR\_EN DDRD  

#define LCD\_PRT\_EN PORTD  

#define LCD\_EN 3  

// D4  

#define LCD\_DDR\_D4 DDRD  

#define LCD\_PRT\_D4 PORTD  

#define LCD\_D4 2  

// D5  

#define LCD\_DDR\_D5 DDRD  

#define LCD\_PRT\_D5 PORTD  

#define LCD\_D5 1  

// D6  

#define LCD\_DDR\_D6 DDRD  

#define LCD\_PRT\_D6 PORTD  

#define LCD\_D6 0  

// D7  

#define LCD\_DDR\_D7 DDRC  

#define LCD\_PRT\_D7 PORTC  

#define LCD\_D7 6  

/*--- LCD Pins End ---*/


void lcd\_init(void);  

void lcd\_cmd(unsigned char);  

void lcd\_dat(unsigned char);


int main(void){  

 lcd\_init();


 lcd\_dat('T');  

 lcd\_dat('e');  

 lcd\_dat('S');  

 lcd\_dat('t');  

 lcd\_dat('r');  

 lcd\_dat('i');  

 lcd\_dat('n');  

 lcd\_dat('g');  

 lcd\_dat('.');  

 lcd\_dat('.');  

 lcd\_dat('.');


 lcd\_cmd(0xBF); // Move cursor to second line


 for(signed char i=10;i>=0;i--){  

 lcd\_dat(i+'0');  

 }


 lcd\_cmd(0x02); \_delay\_ms(2); // Returns cursor to home position (address 0). Also returns display being shifted to the original position. DDRAM contents remains unchanged.


 return 0;  

}


void lcd\_init(void){  

 SBI(LCD\_DDR\_RS,LCD\_RS);  

 SBI(LCD\_DDR\_EN,LCD\_EN);  

 SBI(LCD\_DDR\_D4,LCD\_D4);  

 SBI(LCD\_DDR\_D5,LCD\_D5);  

 SBI(LCD\_DDR\_D6,LCD\_D6);  

 SBI(LCD\_DDR\_D7,LCD\_D7);


 CBI(LCD\_PRT\_RS,LCD\_RS);  

 \_delay\_ms(2);  

 SBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_us(6);  

 CBI(LCD\_PRT\_D4,LCD\_D4);  

 SBI(LCD\_PRT\_D5,LCD\_D5);  

 CBI(LCD\_PRT\_D6,LCD\_D6);  

 CBI(LCD\_PRT\_D7,LCD\_D7);  

 \_delay\_us(6);  

 CBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_ms(2);  

 lcd\_cmd(0x28); // 2 Lines  

 //lcd\_cmd(0x06);// Cursor Move Direction: cursor is in increment position  

 lcd\_cmd(0x0D); // Display on, Cursor off, Cursor blink on  

 lcd\_cmd(0x01); // Clear Screen and returns cursor to the home position (address 0)  

 \_delay\_ms(2);  

 lcd\_cmd(0x80); // Sets the DDRAM address. DDRAM data is sent and received after this setting.  

}


void lcd\_cmd(unsigned char cmd){  

 CBI(LCD\_PRT\_RS,LCD\_RS);  

 SBI(LCD\_PRT\_EN,LCD\_EN);  

 if(cmd&0x10){SBI(LCD\_PRT\_D4,LCD\_D4);}  

 else CBI(LCD\_PRT\_D4,LCD\_D4);  

 if(cmd&0x20){SBI(LCD\_PRT\_D5,LCD\_D5);}  

 else CBI(LCD\_PRT\_D5,LCD\_D5);  

 if(cmd&0x40){SBI(LCD\_PRT\_D6,LCD\_D6);}  

 else CBI(LCD\_PRT\_D6,LCD\_D6);  

 if(cmd&0x80){SBI(LCD\_PRT\_D7,LCD\_D7);}  

 else CBI(LCD\_PRT\_D7,LCD\_D7);  

 CBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_us(40);  

 SBI(LCD\_PRT\_EN,LCD\_EN);  

 if(cmd&1){SBI(LCD\_PRT\_D4,LCD\_D4);}  

 else CBI(LCD\_PRT\_D4,LCD\_D4);  

 if(cmd&2){SBI(LCD\_PRT\_D5,LCD\_D5);}  

 else CBI(LCD\_PRT\_D5,LCD\_D5);  

 if(cmd&4){SBI(LCD\_PRT\_D6,LCD\_D6);}  

 else CBI(LCD\_PRT\_D6,LCD\_D6);  

 if(cmd&8){SBI(LCD\_PRT\_D7,LCD\_D7);}  

 else CBI(LCD\_PRT\_D7,LCD\_D7);  

 CBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_us(40);  

}


void lcd\_dat(unsigned char cmd){  

 SBI(LCD\_PRT\_RS,LCD\_RS);  

 SBI(LCD\_PRT\_EN,LCD\_EN);  

 if(cmd&0x10){SBI(LCD\_PRT\_D4,LCD\_D4);}  

 else CBI(LCD\_PRT\_D4,LCD\_D4);  

 if(cmd&0x20){SBI(LCD\_PRT\_D5,LCD\_D5);}  

 else CBI(LCD\_PRT\_D5,LCD\_D5);  

 if(cmd&0x40){SBI(LCD\_PRT\_D6,LCD\_D6);}  

 else CBI(LCD\_PRT\_D6,LCD\_D6);  

 if(cmd&0x80){SBI(LCD\_PRT\_D7,LCD\_D7);}  

 else CBI(LCD\_PRT\_D7,LCD\_D7);  

 CBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_us(40);  

 SBI(LCD\_PRT\_EN,LCD\_EN);  

 if(cmd&1){SBI(LCD\_PRT\_D4,LCD\_D4);}  

 else CBI(LCD\_PRT\_D4,LCD\_D4);  

 if(cmd&2){SBI(LCD\_PRT\_D5,LCD\_D5);}  

 else CBI(LCD\_PRT\_D5,LCD\_D5);  

 if(cmd&4){SBI(LCD\_PRT\_D6,LCD\_D6);}  

 else CBI(LCD\_PRT\_D6,LCD\_D6);  

 if(cmd&8){SBI(LCD\_PRT\_D7,LCD\_D7);}  

 else CBI(LCD\_PRT\_D7,LCD\_D7);  

 CBI(LCD\_PRT\_EN,LCD\_EN);  

 \_delay\_us(40);  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
