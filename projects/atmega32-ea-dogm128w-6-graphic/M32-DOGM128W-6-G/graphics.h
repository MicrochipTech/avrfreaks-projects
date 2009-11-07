#ifndef GRAPHICS_H
#define GRAPHICS_H


// GRAPHICS /////////////////////////////////////////////////////////////////////////

// SETTINGS:

// Include display driver
#include "driver_dogm128.h"


// Using standard fonts
#include "font.h"
#define G_FONT_DEFAULT font_standard


// Variables
#define true  1
#define false 0


#define LCD_IMGFLAG_INVERT      1   /* invert image (white <-> black) */
#define LCD_IMGFLAG_TRANSWHITE  2   /* replace transparency with white */
#define LCD_IMGFLAG_TRANSBLACK  4   /* replace transparency with black */ 

// Invert all draw functions
uint8_t g_inverted = false;



void g_init()
{
	disp_init();
}

void g_clear()
{
	disp_clear();
}

void g_clear_dont_refresh()
{
	disp_clear_dont_refresh();
}

void g_draw_pixel(uint8_t x, uint8_t y)
{
	disp_set_pixel(x, y, 1 - g_inverted);
}

void g_clear_pixel(uint8_t x, uint8_t y)
{
	disp_set_pixel(x, y, 0 + g_inverted);
}

void g_draw_line(uint8_t x, uint8_t y, int16_t len, uint8_t direction)
{
	// direction: 0 = horizontal,
	//            1 = vertical

	if (direction == 0)
	{
		if (len > 0)
			for (; len > 0; len--, x++)
				disp_set_pixel(x, y, 1);
		else
			for (; len < 0; len++, x--)
				disp_set_pixel(x, y, 1);
	}
	else
	{
		if (len > 0)
			for (; len > 0; len--, y++)
				disp_set_pixel(x, y, 1);
		else
			for (; len < 0; len++, y--)
				disp_set_pixel(x, y, 1);
	}
}
#define g_draw_line_h(x,y,len) g_draw_line(x,y,len,0)
#define g_draw_line_v(x,y,len) g_draw_line(x,y,len,1)

void g_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	g_draw_line_h(x,         y,         (int16_t)w);
	g_draw_line_h(x,         y + h - 1, (int16_t)w);
	g_draw_line_v(x,         y,         (int16_t)h);
	g_draw_line_v(x + w - 1, y,         (int16_t)h);
}


void g_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (; h > 0; h--, y++)
		g_draw_line_h(x, y, (int16_t)w);
}


void g_draw_data_column_transparentBG(int8_t x, uint8_t y, uint8_t data)
{
	uint8_t row;
	for (row = 0; row < 8; row++,y++)
	{
		if ((data & (1<<row)) != 0)
			g_draw_pixel(x, y);
	}
}

void g_draw_data_column_clearBG(int8_t x, uint8_t y, uint8_t data)
{
	uint8_t row;
	for (row = 0; row < 8; row++,y++)
	{
		if ((data & (1<<row)) != 0)
			g_draw_pixel(x, y);
		else
			g_clear_pixel(x, y);
	}
}



uint8_t g_draw_char_transparentBG(uint8_t x, uint8_t y, uint8_t c)
{
	uint8_t w = 0; //width of drawn character (return value)

	uint16_t addr = (uint16_t)(&G_FONT_DEFAULT[c-0x20]);
	uint8_t data = 0x00;

	while (data != 0xAA && w++ < 7)
	{
		data = pgm_read_byte(addr);
		if (data != 0xAA)
			g_draw_data_column_transparentBG(x++, y, data);
		else
			w--;
		addr++;
	}
	if (w > 7) w = 7;
	return (w);
}

uint8_t g_draw_char_clearBG(uint8_t x, uint8_t y, uint8_t c)
{
	uint8_t w = 0; //width of drawn character (return value)

	uint16_t addr = (uint16_t)(&G_FONT_DEFAULT[c-0x20]);
	uint8_t data = 0x00;

	while (data != 0xAA && w++ < 7)
	{
		data = pgm_read_byte(addr);
		if (data != 0xAA)
			g_draw_data_column_clearBG(x++, y, data);
		else
			w--;
		addr++;
	}
	if (w > 7) w = 7;
	return (w);
}

uint8_t g_draw_string(uint8_t x, uint8_t y, char *s, uint8_t transparent_background)
{
	if (transparent_background)
		while (*s)
		{   // so lange *s != '\0' also ungleich dem "String-Endezeichen"
			x += g_draw_char_transparentBG(x, y, *s) + 1;
			s++;
		}
	else
		while (*s)
		{   // so lange *s != '\0' also ungleich dem "String-Endezeichen"
			x += g_draw_char_clearBG(x, y, *s) + 1;
			s++;
		}
}



void g_frame_begin()
{
	disp_frame_begin();
}

void g_frame_end()
{
	disp_frame_end();
}

void GLCD_Circle(unsigned char xcenter, unsigned char ycenter ,unsigned char radius,unsigned char color)
{
  int tswitch, y, x = 0;
  unsigned char d;

  d = ycenter - xcenter;
  y = radius;
  tswitch = 3 - 2 * radius;
  while (x <= y) {
    disp_set_pixel(xcenter + x, xcenter + y,color);     disp_set_pixel(xcenter + x, ycenter - y,color);
    disp_set_pixel(xcenter - x, ycenter + y,color);     disp_set_pixel(xcenter - x, ycenter - y,color);
    disp_set_pixel(ycenter + y - d, ycenter + x,color); disp_set_pixel(ycenter + y - d, ycenter - x,color);
    disp_set_pixel(ycenter - y - d, ycenter + x,color); disp_set_pixel(ycenter - y - d, ycenter - x,color);

    if (tswitch < 0) tswitch += (4 * x + 6);
    else {
      tswitch += (4 * (x - y) + 10);
      y--;
	    }
    x++;
  				}
}


void GLCD_Line(int X1, int Y1,int X2,int Y2, unsigned char color)
{
int CurrentX, CurrentY, Xinc, Yinc, 
    Dx, Dy, TwoDx, TwoDy, 
	TwoDxAccumulatedError, TwoDyAccumulatedError;

Dx = (X2-X1); // obliczenie sk³adowej poziomej
Dy = (Y2-Y1); // obliczenie sk³adowej pionowej

TwoDx = Dx + Dx; // podwojona sk³adowa pozioma
TwoDy = Dy + Dy; // podwojona sk³adowa pionowa

CurrentX = X1; // zaczynamy od X1
CurrentY = Y1; // oraz Y1

Xinc = 1; // ustalamy krok zwiêkszania pozycji w poziomie 
Yinc = 1; // ustalamy krok zwiêkszania pozycji w pionie

if(Dx < 0) // jesli sk³adowa pozioma jest ujemna 
  {
  Xinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
  Dx = -Dx;  // zmieniamy znak sk³adowej na dodatni
  TwoDx = -TwoDx; // jak równie¿ podwojonej sk³adowej
  }

if (Dy < 0) // jeœli sk³adowa pionowa jest ujemna
  {
  Yinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
  Dy = -Dy; // zmieniamy znak sk³adowej na dodatki
  TwoDy = -TwoDy; // jak równiez podwojonej sk³adowej
  }

disp_set_pixel(X1,Y1, color); // stawiamy pierwszy krok (zapalamy pierwszy piksel)

if ((Dx != 0) || (Dy != 0)) // sprawdzamy czy linia sk³ada siê z wiêcej ni¿ jednego punktu ;)
  {
  // sprawdzamy czy sk³adowa pionowa jest mniejsza lub równa sk³adowej poziomej
  if (Dy <= Dx) // jeœli tak, to idziemy "po iksach"
    { 
    TwoDxAccumulatedError = 0; // zerujemy zmienn¹ 
    do // ruszamy w drogê
	  {
      CurrentX += Xinc; // do aktualnej pozycji dodajemy krok 
      TwoDxAccumulatedError += TwoDy; // a tu dodajemy podwojon¹ sk³adow¹ pionow¹
      if(TwoDxAccumulatedError > Dx)  // jeœli TwoDxAccumulatedError jest wiêkszy od Dx
        {
        CurrentY += Yinc; // zwiêkszamy aktualn¹ pozycjê w pionie
        TwoDxAccumulatedError -= TwoDx; // i odejmujemy TwoDx
        }
       disp_set_pixel(CurrentX,CurrentY, color);// stawiamy nastêpny krok (zapalamy piksel)
       }while (CurrentX != X2); // idziemy tak d³ugo, a¿ osi¹gniemy punkt docelowy
     }
   else // w przeciwnym razie idziemy "po igrekach" 
      {
      TwoDyAccumulatedError = 0; 
      do 
	    {
        CurrentY += Yinc; 
        TwoDyAccumulatedError += TwoDx;
        if(TwoDyAccumulatedError>Dy) 
          {
          CurrentX += Xinc;
          TwoDyAccumulatedError -= TwoDy;
          }
         disp_set_pixel(CurrentX,CurrentY, color); 
         }while (CurrentY != Y2);
    }
  }
}


void g_draw_bitmap_from_flash(const uint8_t _x,const uint8_t _y, uint16_t addr)
{
  uint8_t w = pgm_read_byte(addr++); //width - sirka
  			addr++;	
  uint8_t h = pgm_read_byte(addr++); //height - vyska
  			addr++;
  uint8_t s;  //scanlength
  uint8_t p; //position in data row (in bytes)
  uint8_t x; //x position of current pixel relative to _x
  uint8_t y; //y position of current pixel relative to _y
  uint8_t d; //read data

	s = h/8;

  for (p = 0; p < s; p++)
  	{
    for (x = 0; x < w; x++)
    	{
      	d = pgm_read_byte(addr + (uint16_t)w*(uint16_t)p + (uint16_t)x);
 
      	for (y = p*8; y < p*8+8; y++)
      		
			{
        	if (d & 1<<(y-p*8)) disp_set_pixel(x+_x, y+_y,1);//-p*8
        	else            disp_set_pixel(x+_x, y+_y,0);
      		}
    	}
  	}
}




void ks0108DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
  	int16_t tSwitch, x1 = 0, y1 = radius;
  	tSwitch = 3 - 2 * radius;
	
	while (x1 <= y1) {
	    disp_set_pixel(x+radius - x1, y+radius - y1, color);
	    disp_set_pixel(x+radius - y1, y+radius - x1, color);

	    disp_set_pixel(x+width-radius + x1, y+radius - y1, color);
	    disp_set_pixel(x+width-radius + y1, y+radius - x1, color);
	    
	    disp_set_pixel(x+width-radius + x1, y+height-radius + y1, color);
	    disp_set_pixel(x+width-radius + y1, y+height-radius + x1, color);

	    disp_set_pixel(x+radius - x1, y+height-radius + y1, color);
	    disp_set_pixel(x+radius - y1, y+height-radius + x1, color);

	    if (tSwitch < 0) {
	    	tSwitch += (4 * x1 + 6);
	    } else {
	    	tSwitch += (4 * (x1 - y1) + 10);
	    	y1--;
	    }
	    x1++;
	}
	  	
	g_draw_line(x+radius, y, width-(2*radius),0);			// top
	g_draw_line(x+radius, y+height,width-(2*radius),0);	// bottom
	g_draw_line(x, y+radius, height-(2*radius),1);			// left
	g_draw_line(x+width, y+radius, height-(2*radius),1);	// right
}

//#define ks0108DrawVertLine(x, y, length, color) {ks0108FillRect(x, y, 0, length, color);}
//#define ks0108DrawHoriLine(x, y, length, color) {ks0108FillRect(x, y, length, 0, color);}
#define ks0108DrawCircle(xCenter, yCenter, radius, color) {ks0108DrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);}









 
#endif
