#ifndef LCD8310_H_
#define LCD8310_H_

// set output pins for lcd here
#define LCD_PORT PORTB
#define RES 6
#define SDATA 5
#define SCLK 7
#define CS  4

#define set(val,bitn) (val|=(1<<bitn))        
#define clr(val,bitn) (val&=~(1<<bitn))
#define get(val,bitn) (val&(1<<bitn)) 

#define CMD		0
#define DATA	1

void delay_ms(unsigned int ms);
void init(void); 
void Lcd_Init(void);
void Lcd_Write(unsigned char cd,unsigned char c);
void Lcd_Clear(void);
void SetX(unsigned char x);
void SetY(unsigned char y);
void Gotoxy(unsigned char x,unsigned char y);
void VLine(unsigned char x,unsigned char y,unsigned char on);
void Line(unsigned char x,unsigned char y,unsigned char y2,unsigned char on);
void print_char(unsigned char c);
void print_string(unsigned char * message);

#endif /*LCD8310_H_*/
