
#ifndef _LCD_DRIVER_C
#define _LCD_DRIVER_C
#include <avr/io.h>
#include <avr/pgmspace.h>
//#include "ff.h"

#include "6x8_ascii.txt"
#include "palitra.txt"



#define SPIPORT PORTA
#define SPIDDR DDRA
#define CS 3
#define CLK 2
#define SDA 1
#define RESET 0
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

#define CS_BIT 0x08//0x80
#define SCLK_BIT 0x04//0x40
#define SDATA_BIT 0x02//0x20
#define RST_BIT 0x00

#define byte unsigned char

#define DISON       0x29//PH_DISPON  //0xaf
//#define DISNOR      0x13//0xa6
#define DISINV      0x21//PH_INVON   //0xa7
#define DISCTL      0x36//PH_MADCTL  //0xca
#define SLPIN       0x10//PH_SLEEPIN //0x95
#define SLPOUT      0x11//PH_SLEEPOUT //0x94
#define PASET       0x2B//PH_PASETP   //0x75
#define CASET       0x2A//PH_CASETP  //0x15
#define DATCTL      0x36//PH_MADCTL   //0xbc
#define RGBSET8     0x2D//PH_RGBSET   //0xce
#define RAMWR       0x2C//PH_RAMWRP  //0x5c
#define OSCON       0x03//PH_BSTRON  0xd1
#define PWRCTR      0xBE//0x20
#define VOLCTR      0x25//PH_SETCON  //0x81
#define MADCTL_HORIZ		0x00
#define MADCTL_VERT_FLIP	0x80
#define MADCTL_VERT		0x20


#define NOP         0x25

#define dxLCDScreen 132
#define dyLCDScreen 132
static const uint8_t xlMin = 0;
static const uint8_t ylMin = 2;


char buff[1];


//unsigned char RAM_BUFF[23];
/////////////////////////////////////////////////

/////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//////////         Send_Byte/Cmd                //////////
//////////////////////////////////////////////////////////
void send_cmd(uint8_t data) {

	CLK0
	SDA0
	CLK1
	send_byte_(data);

}
/////////////////////////////////////////////

void send_data(uint8_t data) {

	CLK0
	SDA1
	CLK1

	send_byte_(data);

}

/////////////////////////////////////////////
void send_byte_(uint8_t data){
 byte i;

 for (i=0;i<8;i++)
  {
   CLK0
   if  ((data&128)!=0) SDA1
               else SDA0
   data=data<<1; 
   CLK1 
  }
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

void init_lcd(void) {
	SPIDDR=(1<<SDA)|(1<<CLK)|(1<<CS)|(1<<RESET); //Port-Direction Setup


  // reset the display
  DDRA |= (1 << RST_BIT);
  PORTA &= ~(1 << RST_BIT);
 waitms(100);
  PORTA |= (1 << RST_BIT);
  waitms(100);


	send_cmd (0x01);  //Software Reset
	send_cmd(0x11);  //Sleep Out
 	send_cmd(0x03);  //Booster ON
	waitms(10);
  send_cmd(0x29);  //Display On
  send_cmd(0x13);  //Normal display mode
  send_cmd(0x21);  //Display inversion on
  send_cmd(0xBA);  //Data order
  send_cmd(0x36); //Memory data access control
 //send_data(8|64);   //rgb + MirrorX
 send_data(8|128);   //rgb + MirrorY
//	send_data(0x08);   //rgb 

	send_cmd(0x3A); 
	send_data(2);   //8-Bit per Pixel (default)
   // 0x02   8-bit/pixel
  // 0x03   12-bit/pixel
  // 0x05   16-bit/pixel

 // if 256-color mode, bytes represent RRRGGGBB; the following
 //    maps to 4-bit color for each value in range (0-7 R/G, 0-3 B) 
	send_cmd(RGBSET8);	// 256-color position set
  send_data(0x00);	// 000 RED
  send_data(  0x02);	// 001  
  send_data(  0x04);	// 010
  send_data(  0x06);	// 011
  send_data(  0x08);	// 100
  send_data(  0x0a);	// 101
  send_data(  0x0c);	// 110
  send_data(  0x0f);	// 111
  send_data(  0x00);	// 000 GREEN
  send_data(  0x02);	// 001  
  send_data(  0x04);	// 010
  send_data(  0x06);	// 011
  send_data(  0x08);	// 100
  send_data(  0x0a);	// 101
  send_data(  0x0c);	// 110
  send_data(  0x0f);	// 111
  send_data(  0x00);	//  00 BLUE
  send_data(  0x06);	//  01
  send_data(  0x09);	//  10
  send_data(  0x0f);	//  11

  //Set Constrast
  //send_cmd(0x25);
  //send_data(63);

  send_cmd(0x2C);  //Memory Write


//	send_cmd (0x36);//  0x36  //;Memory data acces control
//	send_data(0x28);// 0x20-MADCTL_VERT

//	clrscr(0,0,132,132,31);

}
/////////////////////////////////////////////////////////
///////////////    CLEAR SCREEN        //////////////////
/////////////////////////////////////////////////////////

void clrscr(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy,uint8_t bg)
{
	window(x,y,dx,dy); // clear screen

	uint8_t ddx = dx-x+1;
	uint8_t ddy = dy-y+1;
	uint16_t a = ddx*ddy;	
	for(uint16_t i=0; i<a; i++){
	send_data(bg);
	}


}




///////////////////////////////////////////////////////////////////////
//////////////  PRINT_CHAR   //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
uint8_t putch(uint8_t x, uint8_t y, char c, uint8_t fg, uint8_t bg) {

	send_cmd (0x36);//  0x36  //;Memory data acces control
	send_data(0x28);//MADCTL_VERT);// 0x20-MADCTL_VERT

	window(x,y,x+5,y+7);

	if(c>127) c-=0x40; // TEST For RUS char
	uint8_t idx = c;
	unsigned char *p = s_apxFont6x8 + idx*6-1; // следующий байт символа
	int a=0;

	for(int i=0; i<6; i++) {
	p++;
	a = pgm_read_byte(p);

	uint8_t d; // - отправить на экран 1н байт из шести символа
	for(int j=0; j<8; j++) 
			{
	if((a&128)!=0) d=fg; else d=bg;
	send_data(d);
	a=a<<1;
			}
	
	
							}

	return x+6;//update X position to next char

}

/////////////////////////////////////////////


void window(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy)
{

	send_cmd (CASET);
	send_data(x);	
	send_data(dx);
		
	send_cmd (PASET);	
	send_data(y);
	send_data(dy);
	send_cmd (RAMWR);	
	
}

//uint16_t cpxSetAndValidateLCDRect(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy) {
//  uint8_t xlFirst, ylFirst, xlLast, ylLast;	// LCD coordinates

  /* check upper left corner */
  /* (x and y aren't too low since unsigned can't be < 0!) */
//  if (x >= dxLCDScreen || y >= dyLCDScreen)	// completely off-screen
//    return 0;
  
  /* check lower right corner */
//  if (x + dx > dxLCDScreen)
//    dx = dxLCDScreen - x;
 // if (y + dy > dyLCDScreen)
//    dy = dyLCDScreen - y;

  /* convert to LCD coordinates */
//  xlLast = (xlFirst = xlMin + x) + dx - 1;
// ylLast = (ylFirst = ylMin + y) + dy- 1;

  /* note: for PASET/CASET, docs say that start must be < end,
     but <= appears to be OK; end is a "last" not "lim" value */
//	send_cmd (PASET);	// Page Address Set
//	send_data(ylFirst);	// start page (line)
///	send_data(ylLast);	// end page
//	send_cmd (CASET);	// Column Address Set
//	send_data(xlFirst);	// start address
//	send_data(xlLast);	// end address
//	send_cmd (RAMWR);	// Memory Write
  
//  return (uint16_t)dx * dy;
//}

/////////////////////////////////////////////////////////


void lcd_set_brightness_parms(uint8_t a, uint8_t b) {
	send_cmd (VOLCTR);
	send_data(a);
	send_data(b);
}
/////////////////////////////////////////////////////
//void fillrect(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t color) {
	
//  uint16_t cpx = cpxSetAndValidateLCDRect(sx, sy, dx, dy);
//  while (cpx-- > 0)
//	send_data(color);
//}
////////////////////////////////////////////////////////
//void setup_pix_blit(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy) {
//  cpxSetAndValidateLCDRect(sx,sy,dx,dy);	
//}
///////////////////////////////////////////////////
void pix_blit(uint8_t color){
	send_data(color);
}

////////////////////////////////////////////////////////
uint8_t putstr_ram(uint8_t x, uint8_t y, char * p, uint8_t fg, uint8_t bg) {
  //void putstr_ram(uint8_t x, uint8_t y, char * p, uint8_t fg, uint8_t bg) {
  uint8_t c;
  while ((c = *p++))
    x = putch(x, y, c, fg, bg);	
  return x;
}




/////////////////////////////////////////////////////////////
uint8_t putstr_pgm(uint8_t x, uint8_t y, PGM_P p, uint8_t fg, uint8_t bg) {
	
  uint8_t c;
  while ((c = pgm_read_byte(p++)))
    x = putch(x, y, c, fg, bg);
	
  return x;
}
/////////////////////////////////////////////////////////////////

uint8_t printDecimal(uint8_t x, uint8_t y, uint32_t val, uint8_t fg, uint8_t bg) {
  char buf[9];
  buf[8] = 0;
  char * ptr = &buf[8];
	
  while(val > 0) {
    ptr--;
    *ptr = val%10 + '0';
    val /= 10;
  }
//  if (ptr == &buf[8])
//    *(ptr = &buf[7]);// = '0';

  return putstr_ram(x,y,ptr,fg,bg);

}

/*
//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void test_12bit_color(void){

	window(0,0,131,131);
	send_cmd(0x3A); 
	send_data(3);   //12-Bit per Pixel (default)


	int i;
  for (i=0;i<132*33;i++) {
    setPixel(255,0,0);
  }

  //green bar
  for (i=0;i<132*33;i++) {
    setPixel(0,255,0);
  }

  //blue bar
  for (i=0;i<132*33;i++) {
    setPixel(0,0,255);
  }

  //white bar
  for (i=0;i<132*33;i++) {
    setPixel(255,255,255);
  }

  //wait for RGB-Data on serial line and display on lcd

  while(1==1) {


    r = receiveByte();
    g = receiveByte();
    b = receiveByte();
    setPixel(r,g,b);

  }
	window(0,0,131,131);
	send_cmd(0x3A); 
	send_data(2);   //вернуть режим 8-Bit per Pixel (default)

}

//converts a 3*8Bit-RGB-Pixel to the 2-Byte-RGBRGB Format of the Display
void setPixel(byte r,byte g,byte b) {
	
  if (n==0) {
    s1=(r & 240) | (g>>4);
    s2=(b & 240);
    n=1;
  } else {
    n=0;
    send_data(s1);//  sendData(s1);
    send_data(s2|(r>>4));//    sendData(s2|(r>>4));
    send_data((g&240) | (b>>4));// sendData((g&240) | (b>>4));
  }
}

*/
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////  ѕ–»Ќя“№ по UART - BMP //////////////////////////
////////////////////////////////////////////////////////////////////

void bmp_rcv_uart(void){


//	char k =0;
//	uint8_t c =0;
//	c = get_data();

//	while (c != 0) {	c = get_data();}
	uint8_t l =0;
	uint8_t h =0;
	uint8_t x =0;
 	uint8_t y =0;


  for(int i = 0; i<23; i++){ // 1078 // 0x436
	uint8_t	t = receiveByte();//receiveByte//get_data

  switch (i){ // получить данные из заголовка BMP
  case 10: { l = t; } 
  case 11: { h = t; }
  case 18: { x = t; }
  case 22: { y = t; }
			}

	}

	uint16_t b = l + ((unsigned int)h << 8); //адрес  начала BMP
	b = b - 23; // начало BMP - данные которые уже получили 

//	uint16_t e = x*y; // e - фактическое поле картинки
 
//	a -= b; a -= e; a /= y; x += a; // расчет остатка - дл€ правильного размера картинки

//	printDecimal(40,40,b, 0, 255);
//	printDecimal(40,50,x, 0, 255);
//	printDecimal(40,60,y, 0, 255);




	for(uint16_t i = 0; i<b; i++){receiveByte();}



	send_cmd (0x36);//  0x36  //;Memory data acces control
	send_data(0x08);// 0x20-MADCTL_VERT
	send_cmd (0x2c);


	 window(2,2,x+1,y+1);

//	uint8_t dy = 1;
	
	for(uint8_t i = 0; i<y; i++){
	for(uint8_t h = 0; h<x; h++){
	uint8_t k=receiveByte();
	unsigned char *p = s_palitra8bit+k;
	int a = pgm_read_byte(p);
	send_data(a);

	}
//	send_cmd (PASET);	
//	send_data(dy++);
//	send_data(dy);
//	send_cmd (RAMWR);
	}



//	send_cmd (CASET);
//	send_data(x);	
//	send_data(dx);
		
//	send_cmd (PASET);	
//	send_data(y);
//	send_data(dy);
//	send_cmd (RAMWR);


/*
//		char c =0;
//while(1){

	for(int h=0; h<16385; h++){ //132*132=17424  128*128=16384
	k=get_data();
	unsigned char *p = s_palitra8bit+k;
	int a = pgm_read_byte(p);
	send_data(a);
	}
*/

}

////////////////////////////////////////////////////
//////////  ѕќ ј«ј“ь ¬≈—№ «Ќј ќ√≈Ќ≈–ј“ќ–   /////////
////////////////////////////////////////////////////
void test_all_char(void){
	char j = 1;
	char k =0;
	char l=0;
	for(int i=0; i<128; i+=8){
	for(int h=2; h<114; h+=7){
	putch(h, i, k++, 255, l++);// l++);
	}
	printDecimal(118,i,j++, 0xFE, 0x00);

	}
}
///////////////////////////////
///////////////////////////////

void drawLine(int x1, int y1, int x2, int y2, uint8_t fg, uint8_t l)
{
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
 
    for (;;)
    {
        set_Pixel(x1, y1, fg, l);//uint8_t fg, uint8_t bg
 
        if(x1 == x2 && y1 == y2)
            break;
 
        int error2 = error * 2;
 
        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
 
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
}



void set_Pixel(int x1,int y1,uint8_t fg, uint8_t l)
{

	send_cmd (CASET);
	send_data(x1);	
	send_data(x1+l);
		
	send_cmd (PASET);	
	send_data(y1);
	send_data(y1+l);
	send_cmd (RAMWR);

	l *=l;
	for(uint8_t i=0; i<l; i++){
	send_data(fg);
	}
  send_cmd(0x2C);  //Memory Write

}
//////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*void print_bmp_sd(FIL *fil_ob){

	  FIL fil_obj;

	fil_obj = *fil_ob;

 	UINT s2, cnt=1;				//	printString (f_gets (buff,1,&fil_obj));	//Read data from newfile
	uint8_t al=0,ah=0,l =0,h =0,x =0,y =0;


  for(int i = 0; i<23; i++){ // 1078 // 0x436

	f_read(&fil_obj, buff, cnt, &s2);

	uint8_t	t = buff[0];//receiveByte//get_data

  switch (i){ // получить данные из заголовка BMP
  case 2:  {al = t; } 
  case 3:  {ah = t; } 
  case 10: {l = t; } 
  case 11: {h = t; }
  case 18: {x = t; }
  case 22: {y = t; }
			}
							}// end for
//	uint16_t a = al + ((unsigned int)ah << 8); //размер файла							}
	uint16_t b = l + ((unsigned int)h << 8); //адрес  начала BMP

	b = b - 23; // начало BMP - данные которые уже получили 

	for(uint16_t i = 0; i<b; i++){	f_read(&fil_obj, buff, cnt, &s2);}

	send_cmd (0x36);//  0x36  //;Memory data acces control
	send_data(0x08);// 0x20-MADCTL_VERT
	send_cmd (0x2c);

	 window(2,2,x+1,y+1);
	
	for(uint8_t i = 0; i<y; i++){
	for(uint8_t h = 0; h<x; h++){

	f_read(&fil_obj, buff, cnt, &s2);
	uint8_t k = buff[0];

	unsigned char *p = s_palitra8bit+k;
	int a = pgm_read_byte(p);// цвет из BMP сопоставить с местной палитрой
	send_data(a);
	}
	}

}
*/
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

#endif// _LCD_DRIVER_C
