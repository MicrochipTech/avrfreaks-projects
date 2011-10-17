#include <avr/io.h>
#include <util/delay.h>
#include "lcd_driver.c"
#include "lcd_driver.h"

#define SPIPORT PORTA
#define SPIDDR DDRA
#define CS 3
#define CLK 2
#define SDA 1
#define RESET 0

//#define xtal 20000000 
#define baud 128000

#define USR UCSRA
#define UCR UCSRB
#define UBRR UBRRL

#define cbi(reg, bit) (reg&=~(1<<bit))
#define sbi(reg, bit) (reg|= (1<<bit))

#define CS_0 cbi(SPIPORT,CS);
#define CS_1 sbi(SPIPORT,CS);
#define CLK0 cbi(SPIPORT,CLK);
#define CLK1 sbi(SPIPORT,CLK);
#define SDA0 cbi(SPIPORT,SDA);
#define SDA1 sbi(SPIPORT,SDA);
#define RESET0 cbi(SPIPORT,RESET);
#define RESET1 sbi(SPIPORT,RESET);
unsigned char BUFF[16];

void waitms(int ms);
const  char hello[] PROGMEM = " ÇÀÏÓÑÊ ÏÐÎØÅË !!! ";

char get_data(void);
void put_data(char c);

void printString(char* string);

#define byte unsigned char
byte bg=255; // bacground color
byte fg=0; // figure color
byte xp=0,yp=0;// ãëîáàëüíûå X,Y

void test_palitra(void);
void ascii_test(void);

///////////////////////////////////////////////////////////
int main (void) {



	SPIDDR=(1<<SDA)|(1<<CLK)|(1<<CS)|(1<<RESET); //Port-Direction Setup


	UCSR1B= ((1<<RXEN1)|(1<<TXEN1));
    UBRR1L=(F_CPU/(16*baud)-1);



	init_lcd();
	clrscr(0,0,132,132,0);

	test_palitra();
	//ascii_test();


//////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////



//putch(10, 30, 'Þ', 0, 255);


//putstr_pgm(10,20, hello,0,255);




//while(1){

//   bmp_rcv_uart();

//}


/*
uint8_t x=0; 
uint8_t y=0;
uint8_t x1=128;
uint8_t y1=128;
uint8_t i=0;

while (1){
	
	if(x<=150) {x++;} else x-=250;
	if(y<=128) {y++;} else y+=50;
	if(x1<=0) {x1--;} else x1=128;
	if(y1<=128) {y1--;}else y1=100;

	i++;
	drawLine(x, y, x1, y1, i,2);
	
 }
*/
//while(1){

//   bmp_rcv_uart();

//}



/*
	clrscr(0,0,131,131,0xe0);



putch(10, 10, 'Þ', 0, 255);


while(1){
 window(2,2,129,129);
	bmp_rcv_uart();
}



// 	putstr_pgm(10, i, hello, 0xFE, 0x00);

//	char *text = "All work and no play make Jack a dull boy. "; // ÊÎÏÈÐÓÅÒÑß Â RAM !!!
//	putstr_ram(10, 10, text, 0xFE, 0x00);

//	printDecimal(40,40,0xFFFFFF, 0xFF, 0x00);


*/
} // MAIN!!!!!
////////////////////////////////////////////////////////////////////////



char get_data(void) 
{ 
 
	while ( !(UCSR1A & (1<<RXC1)) ); 
         return UDR1; 
} 
//////////////////////////////

void put_data(char c) 
{ 

 while ( !( UCSR1A & (1<<UDRE1)) ); 
    UDR1 =c; 

 //		UDR1 = 'O';


} 

/////////////////////////////////////////////
void waitms(int ms) {
  int i;
  for (i=0;i<ms;i++) _delay_ms(1);
}

/////////////////////////////////////////////////////////////
////////////////////// palitra //////////////////////////////
/////////////////////////////////////////////////////////////
void test_palitra(void){

	send_cmd (0x36);//  0x36  //;Memory data acces control
	send_data(0x28);//MADCTL_VERT);// 0x20-MADCTL_VERT

	unsigned char *p = s_palitra8bit;
	for(uint8_t y=1; y<129; y+=8){
	for(uint8_t x=1; x<129; x+=8){
	uint8_t a = pgm_read_byte(p);
	window(x,y,x+8,y+8);
	for(uint8_t i=0;i<128; i++)
	{send_data(a);}
	p++;
	}
	}
}
//////////////////////////////////////////////////////
//////////////////// ASCII-TAB ///////////////////////
//////////////////////////////////////////////////////
void ascii_test(void){

	uint8_t c=0,d=1;
	for(uint8_t y=2; y<130; y+=8){
	for(uint8_t x=2; x<98; x+=6){
	putch(x, y, c, fg, bg);
	c++;
	}
	printDecimal(110,y, d, fg, bg);
	d++;
	}
}

//////////////////////////////////////////////////////
/*

void drawCircle(int x0, int y0, int radius) {
        int x = 0;
        int y = radius;
        int delta = 2 - 2 * radius;
        int error = 0;
        while(y >= 0) {
                setPixel(x0 + x, y0 + y);
                setPixel(x0 + x, y0 - y);
                setPixel(x0 - x, y0 + y);
                setPixel(x0 - x, y0 - y);
                error = 2 * (delta + y) - 1;
                if(delta < 0 && error <= 0) {
                        ++x;
                        delta += 2 * x + 1;
                        continue;
                }
                error = 2 * (delta - x) - 1;
                if(delta > 0 && error > 0) {
                        --y;
                        delta += 1 - 2 * y;
                        continue;
                }
                ++x;
                delta += 2 * (x - y);
                --y;
        }
}
*/
